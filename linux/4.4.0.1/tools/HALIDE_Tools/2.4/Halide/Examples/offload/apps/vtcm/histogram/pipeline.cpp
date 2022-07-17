/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#include "Halide.h"
#include "process.h"
using namespace Halide;

// Histogram example using scatter-accumulates.
class Histogram : public Halide::Generator<Histogram> {
public:
    Input<Buffer<DTYPE>> input{"input", 2};
    Output<Buffer<HTYPE>> output{"output", 1};

    GeneratorParam<bool> use_vtcm{"use_vtcm", true};

    void generate() {
        RDom r(input);
        Expr bin = clamp(cast<int32_t>(input(r.x, r.y)), 0, HSIZE - 1);
        hist(x) = cast<HTYPE>(0);
        hist(bin) += cast<HTYPE>(1);
        output(x) = hist(x);

        if (get_target().has_feature(Target::HVX)) {
            constexpr int vector_size = 128;
            hist
                .compute_at(output, Var::outermost())
                .vectorize(x, vector_size / 2);
            hist
                .update(0)
                .allow_race_conditions()
                .vectorize(r.x, vector_size / 2);
            if (use_vtcm) {
                hist.store_in(MemoryType::VTCM);
            }
        }
    }

    void schedule() {
        input.dim(0).set_min(0);
        input.dim(1).set_min(0);
        output.dim(0).set_min(0);

        input.dim(0).set_extent(W);
        input.dim(1).set_extent(H);
        output.dim(0).set_extent(HSIZE);

        if (get_target().has_feature(Target::HVX)) {
            constexpr int vector_size = 128;
            output
                .hexagon()
                .vectorize(x, vector_size / 2);
        }
    }

private:
    Func hist{"hist"};
    Var x{"x"}, y{"y"}, yi{"yi"};
};

HALIDE_REGISTER_GENERATOR(Histogram, histogram);

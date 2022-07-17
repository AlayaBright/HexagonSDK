/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#include "Halide.h"
#ifdef USE_SCHEDULE
#include "dilate3x3.schedule.h"
#endif

using namespace Halide;

class Dilate3x3 : public Generator<Dilate3x3> {
public:
    // Takes an 8 bit image; one channel.
    Input<Buffer<uint8_t>> input{"input", 2};
    // Outputs an 8 bit image; one channel.
    Output<Buffer<uint8_t>> output{"output", 2};

    GeneratorParam<bool> use_parallel_sched{"use_parallel_sched", true};
    GeneratorParam<bool> use_prefetch_sched{"use_prefetch_sched", true};

    void generate() {
        bounded_input(x, y) = BoundaryConditions::repeat_edge(input)(x, y);
        max_y(x, y) = max(bounded_input(x, y - 1), bounded_input(x, y),
                          bounded_input(x, y + 1));

        output(x, y) = max(max_y(x - 1, y), max_y(x, y), max_y(x + 1, y));
    }

    void schedule() {
        input.dim(0).set_min(0);
        input.dim(1).set_min(0);
        output.dim(0).set_min(0);
        output.dim(1).set_min(0);

        int vector_size = natural_vector_size<uint8_t>();
        if (get_target().has_feature(Target::HVX)) {
            vector_size = 128;
        }
        Expr input_stride = input.dim(1).stride();
        Expr output_stride = output.dim(1).stride();

        input.dim(1).set_stride((input_stride / vector_size) * vector_size);
        output.dim(1).set_stride((output_stride / vector_size) * vector_size);

        input.set_host_alignment(vector_size);
        output.set_host_alignment(vector_size);

#ifdef USE_SCHEDULE
        apply_schedule_dilate3x3(get_pipeline(), target);
#else
        if (auto_schedule) {
            const int W = 3840;
            const int H = 2160;
            input.dim(0).set_estimate(0, W);
            input.dim(1).set_estimate(0, H);

            output.dim(0).set_estimate(0, W);
            output.dim(1).set_estimate(0, H);
        } else {
            Var xi{"xi"}, yi{"yi"};

            if (get_target().has_feature(Target::HVX)) {
                Expr ht = output.dim(1).extent();

                bounded_input
                    .compute_at(Func(output), y)
                    .align_storage(x, 128)
                    .vectorize(x, vector_size * 2, TailStrategy::RoundUp);
                output
                    .hexagon()
                    .split(y, yo, y, ht / 2)
                    .tile(x, y, xi, yi, vector_size * 2, 8, TailStrategy::RoundUp)
                    .vectorize(xi)
                    .unroll(yi);
                if (use_prefetch_sched) {
                    output.prefetch(input, y, 2);
                }
                if (use_parallel_sched) {
                    output.parallel(yo);
                }
            } else {
                // optimized while testing for ARM "host"
                bounded_input
                    .compute_at(Func(output), y)
                    .vectorize(x, vector_size, TailStrategy::RoundUp);
                output
                    .split(y, yo, y, 16)
                    .tile(x, y, xi, yi, vector_size, 4, TailStrategy::RoundUp)
                    .vectorize(xi)
                    .unroll(yi)
                    .parallel(yo);
            }
        }
#endif
    }

private:
    Var x{"x"}, y{"y"}, yo{"yo"};
    Func max_y{"max_y"};
    Func bounded_input{"bounded_input"};
};

HALIDE_REGISTER_GENERATOR(Dilate3x3, dilate3x3)

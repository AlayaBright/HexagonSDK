/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#include "Halide.h"
#ifdef USE_SCHEDULE
#include "gaussian5x5.schedule.h"
#endif

using namespace Halide;

class Gaussian5x5 : public Generator<Gaussian5x5> {
public:
    Input<Buffer<uint8_t>> input{"input", 2};
    Output<Buffer<uint8_t>> output{"output", 2};

    GeneratorParam<bool> use_parallel_sched{"use_parallel_sched", true};
    GeneratorParam<bool> use_prefetch_sched{"use_prefetch_sched", true};

    void generate() {
        bounded_input(x, y) = BoundaryConditions::repeat_edge(input)(x, y);

        Func input_16("input_16");
        input_16(x, y) = cast<int16_t>(bounded_input(x, y));

        rows(x, y) = input_16(x - 2, y) + 4 * input_16(x - 1, y) + 6 * input_16(x, y) + 4 * input_16(x + 1, y) + input_16(x + 2, y);
        cols(x, y) = rows(x, y - 2) + 4 * rows(x, y - 1) + 6 * rows(x, y) + 4 * rows(x, y + 1) + rows(x, y + 2);

        output(x, y) = cast<uint8_t>(cols(x, y) >> 8);
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
        apply_schedule_gaussian5x5(get_pipeline(), target);
#else

        if (auto_schedule) {
            const int W = 3840;
            const int H = 2160;
            input.dim(0).set_estimate(0, W);
            input.dim(1).set_estimate(0, H);

            output.dim(0).set_estimate(0, W);
            output.dim(1).set_estimate(0, H);
        } else {
            Var xi{"xi"}, yi{"yi"}, yo{"yo"};

            if (get_target().has_feature(Target::HVX)) {
                Expr ht = output.dim(1).extent();

                output
                    .hexagon()
                    .split(y, yo, y, ht / 2)
                    .tile(x, y, xi, yi, vector_size * 2, 4, TailStrategy::RoundUp)
                    .unroll(yi)
                    .vectorize(xi);

                rows.compute_at(output, y)
                    .store_at(output, yo)
                    .align_storage(x, vector_size)
                    .vectorize(x, vector_size, TailStrategy::RoundUp);

                if (use_prefetch_sched) {
                    output.prefetch(input, y, 2);
                }
                if (use_parallel_sched) {
                    output.parallel(yo);
                }
            } else {
                // optimized while testing for ARM "host"
                output
                    .split(y, yo, y, 16)
                    .tile(x, y, xi, yi, vector_size, 4, TailStrategy::RoundUp)
                    .unroll(yi)
                    .parallel(yo)
                    .vectorize(xi);

                rows.compute_at(output, y)
                    .store_at(output, yo)
                    .vectorize(x, vector_size, TailStrategy::RoundUp);
            }
        }
#endif
    }

private:
    Func rows{"rows"}, cols{"cols"}, bounded_input{"bounded_input"};
    Var x{"x"}, y{"y"};
};

HALIDE_REGISTER_GENERATOR(Gaussian5x5, gaussian5x5)

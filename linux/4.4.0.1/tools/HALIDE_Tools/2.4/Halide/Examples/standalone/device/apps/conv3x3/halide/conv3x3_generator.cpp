/* ======================================================================== */
/*  QUALCOMM TECHNOLOGIES, INC.                                             */
/*                                                                          */
/*  Halide for HVX Image Processing                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*          Copyright (c) 2021 QUALCOMM TECHNOLOGIES Incorporated.          */
/*                           All Rights Reserved.                           */
/*                  QUALCOMM Confidential and Proprietary                   */
/* ======================================================================== */

#include "Halide.h"
#ifdef USE_SCHEDULE
#include "conv3x3_halide.schedule.h"
#endif

using namespace Halide;

class conv3x3 : public Generator<conv3x3> {
public:
    // Takes an 8 bit image; one channel.
    Input<Buffer<uint8_t>> input{"input", 2};
    Input<Buffer<int8_t>> mask{"mask", 2};
    // Outputs an 8 bit image; one channel.
    Output<Buffer<uint8_t>> output{"output", 2};

    void generate() {
        bounded_input(x, y) = BoundaryConditions::repeat_edge(input)(x, y);
        Expr sum = cast<int16_t>(0);
        for (int j = -1; j <= 1; j++) {
            for (int i = -1; i <= 1; i++) {
                sum += cast<int16_t>(bounded_input(x + j, y + i)) * cast<int16_t>(mask(j + 1, i + 1));
            }
        }
        output(x, y) = cast<uint8_t>(clamp(sum >> 4, 0, 255));
    }

    void schedule() {

        input.dim(0).set_min(0);
        input.dim(1).set_min(0);

        auto output_buffer = Func(output).output_buffer();
        output_buffer.dim(0).set_min(0);
        output_buffer.dim(1).set_min(0);

        const int W = 1024;
        const int H = 1024;
        // Set estimates for input and output buffers. Required for autoscheduling.
        input.dim(0).set_estimate(0, W);
        input.dim(1).set_estimate(0, H);
        mask.dim(0).set_estimate(0, 3);
        mask.dim(1).set_estimate(0, 3);

        output.dim(0).set_estimate(0, W);
        output.dim(1).set_estimate(0, H);

#ifdef USE_SCHEDULE
        apply_schedule_conv3x3_halide(get_pipeline(), target);
#else

        if (!auto_schedule) {
            // Hand schedule.
            Var xi{"xi"}, yi{"yi"}, yo{"yo"};
            if (get_target().has_feature(Target::HVX)) {
                constexpr int vector_size = 128;
                Expr input_stride = input.dim(1).stride();
                input.dim(1).set_stride((input_stride / vector_size) * vector_size);
                input.set_host_alignment(vector_size);

                Expr output_stride = output_buffer.dim(1).stride();
                output_buffer.dim(1).set_stride((output_stride / vector_size) * vector_size);
                output_buffer.set_host_alignment(vector_size);
                Expr yExtent = output.dim(1).extent();

                bounded_input
                    .compute_at(Func(output), y)
                    .align_storage(x, 128)
                    .vectorize(x, vector_size, TailStrategy::RoundUp);
                Func(output)
                    .split(y, yo, y, yExtent / 2)
                    .prefetch(input, y, 2)
                    .parallel(yo)
                    .tile(x, y, xi, yi, vector_size, 8, TailStrategy::RoundUp)
                    .vectorize(xi)
                    .unroll(yi);
            } else {
                const int vector_size = natural_vector_size<uint8_t>();
                Func(output)
                    .vectorize(x, vector_size)
                    .parallel(y, 16);
            }
        }
#endif

    }

private:
    Var x{"x"}, y{"y"};
    Func bounded_input{"input_bounded"};
};

HALIDE_REGISTER_GENERATOR(conv3x3, conv3x3_halide);

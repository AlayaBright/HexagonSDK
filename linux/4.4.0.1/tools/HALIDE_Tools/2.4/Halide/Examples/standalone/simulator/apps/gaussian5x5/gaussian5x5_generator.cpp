/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#include "Halide.h"

using namespace Halide;

class Gaussian5x5 : public Generator<Gaussian5x5> {
public:
    // Takes an 8 bit image; one channel.
    Input<Buffer<uint8_t>> input{"input", 2};
    // Outputs an 8 bit image; one channel.
    Output<Buffer<uint8_t>> output{"output", 2};

    void generate() {
#ifdef BORDERS
        bounded_input(x, y) = BoundaryConditions::repeat_edge(input)(x, y);
        input_16(x, y) = cast<int16_t>(bounded_input(x, y));
#else
        input_16(x, y) = cast<int16_t>(input(x, y));
#endif
        rows(x, y) = input_16(x - 2, y) + 4 * input_16(x - 1, y) + 6 * input_16(x, y) + 4 * input_16(x + 1, y) + input_16(x + 2, y);
        cols(x, y) = rows(x, y - 2) + 4 * rows(x, y - 1) + 6 * rows(x, y) + 4 * rows(x, y + 1) + rows(x, y + 2);

        output(x, y) = cast<uint8_t>(cols(x, y) >> 8);
    }

    void schedule() {
        Var xi{"xi"}, yi{"yi"}, yo{"yo"};

        input.dim(0).set_min(0);
        input.dim(1).set_min(0);

        auto output_buffer = Func(output).output_buffer();
        output_buffer.dim(0).set_min(0);
        output_buffer.dim(1).set_min(0);

        if (get_target().has_feature(Target::HVX)) {
            const int vector_size = get_target().has_feature(Target::HVX_128) ? 128 : 64;
            Expr input_stride = input.dim(1).stride();
            input.dim(1).set_stride((input_stride / vector_size) * vector_size);
            input.set_host_alignment(vector_size);

            Expr output_stride = output_buffer.dim(1).stride();
            output_buffer.dim(1).set_stride((output_stride / vector_size) * vector_size);
            output_buffer.set_host_alignment(vector_size);
            Expr ht = output.dim(1).extent();
#ifdef BORDERS
            bounded_input.compute_root();
#endif
            Func(output).split(y, yo, y, ht / 2).tile(x, y, xi, yi, vector_size, 4, TailStrategy::RoundUp).unroll(yi).vectorize(xi);

            rows.compute_at(output, y)
                .store_at(output, yo)
                .vectorize(x, vector_size, TailStrategy::RoundUp);

        } else {
            const int vector_size = natural_vector_size<uint8_t>();
            Func(output)
                .vectorize(x, vector_size)
                .parallel(y, 16);
        }
    }

private:
    Var x{"x"}, y{"y"};
    Func rows{"rows"}, cols{"cols"};
    Func input_16{"input_16"};
#ifdef BORDERS
    Func bounded_input{"bounded_input"};
#endif
};

HALIDE_REGISTER_GENERATOR(Gaussian5x5, gaussian5x5);

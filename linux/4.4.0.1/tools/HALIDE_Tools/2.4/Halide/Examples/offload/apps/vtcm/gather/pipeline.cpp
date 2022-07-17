/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#include "Halide.h"
#include "process.h"
using namespace Halide;

// This pipeline demonstrates how to use VTCM and have Halide
// generate gather instructions. In addition, if you compile this
// with target feature 'hvx_v68' then you will be able to use
// User DMA also.

// After compiling this file along with $(HALIDE_ROOT/tools/GenGen.cpp
// we get a generator executable. This executable can be run on the
// command line to generate an object file for this Generator.
// The generator executable takes a command line option called target.
class Gather : public Halide::Generator<Gather> {
public:
    // Our pipeline takes two inputs, each of which is a buffer
    // of two dimensions. One of them is the look-up table, (LUT)
    // while the other is the input of indices we will use to
    // to look into the table.
    // The output is a two dimensionak buffer of the looked-up
    // values.
    Input<Buffer<DTYPE>> input{"input", 2};
    Input<Buffer<DTYPE>> lut{"lut", 2};
    Output<Buffer<DTYPE>> output{"output", 2};

    GeneratorParam<bool> use_vtcm{"use_vtcm", true};

    void generate() {
        // We create two expressions for indexing into the LUT. Since we do not
        // want to access the LUT out-of-bounds, we clamp the values of the
        // indices.
        Expr xCoord = clamp(cast<int32_t>(input(x, 0)), 0, lut.width() - 1);
        Expr yCoord = clamp(cast<int32_t>(input(x, 1)), 0, lut.height() - 1);

        // Gather operations on Hexagon can be performed only on VTCM. So,
        // the first stage (Func) of our pipeline will be to copy the LUT
        // into VTCM. Later, when we are scheduling this pipeline, we'll
        // see how to ensure lut_vtcm ends up into VTCM.
        lut_vtcm(x, y) = lut(x, y);

        // Now we simply look up from the LUT.
        output_vtcm(x, y) = lut_vtcm(xCoord, yCoord);
        output(x, y) = output_vtcm(x, y);
    }

    void schedule() {

        // We let the Halide compiler know that the first element of
        // the input buffer has x coordinate 0.
        input.dim(0).set_min(0);
        // We let the Halide compiler know that the first element of
        // the input buffer has y coordinate 0. So, our first element
        // is (0, 0).
        input.dim(1).set_min(0);

        // Similarly, the first element of the LUT is (0, 0).
        lut.dim(0).set_min(0);
        lut.dim(1).set_min(0);

        // First element of the LUT is (0, 0).
        output.dim(0).set_min(0);
        output.dim(1).set_min(0);

        // We let the Halide compiler know that all the external buffers
        // are aligned to the HVX native vector width.
        input.set_host_alignment(128);
        lut.set_host_alignment(128);
        output.set_host_alignment(128);

        // We will fix the size of the external buffers. This will help
        // the Halide compiler generate better code.
        input.dim(0).set_extent(W);
        input.dim(1).set_extent(H);
        lut.dim(0).set_extent(W);
        lut.dim(1).set_extent(H);
        output.dim(1).set_extent(H);
        output.dim(0).set_extent(W);

        if (get_target().has_feature(Target::HVX)) {
            constexpr int vector_size = 128;

            // The loop nest in pseudo code for this pipeline is
            // for (int outermost = 0; outermost < 1; ++outermost) {
            //   for (int output.y = 0; output.y < height; ++output.y) {
            //     for (int output.x = 0; output.x < width; ++output.x) {
            //     }
            //   }
            // }
            // We will compute the copy stage at Var::outermost.
            //
            // for (int outermost = 0; outermost < 1; ++outermost) {
            //   lut_vtcm[][] = input[][];
            //   for (int output.y = 0; output.y < height; ++output.y) {
            //     for (int output.x = 0; output.x < width; ++output.x) {
            //     }
            //   }
            // }

            lut_vtcm
                .compute_at(output, Var::outermost());

            // We will compute the look up operation per row of the output.
            // While doing so, we will vectorize output_vtcm by a factor
            // of vector_size/2. The data type of output_vtcm is at least
            // 2 bytes, so if we vectorize by a factor of vector_size/2, we'll
            // get a full native vector. Since, we need only one row of
            // output_vtcm per row or output, the Halide compiler will allocate
            // and compute only one row of output_vtcm at a time.
            //
            // The loop nest in pseudo code is
            // for (int outermost = 0; outermost < 1; ++outermost) {
            //   lut_vtcm[][] = input[][];
            //   for (int output.y = 0; output.y < height; ++output.y) {
            //     for (int output_vtcm.y = 0; output_vtcm.y < 1; ++output_vtcm.y) {
            //       for (int output_vtcm.x = 0; output_vtcm.x < output.x.extent; ++ output_vtcm.x) {
            //          output_vtcm[][] = input_vtcm[][];
            //       }
            //     }
            //     for (int output.x = 0; output.x < width; ++output.x) {
            //     }
            //   }
            // }

            output_vtcm
                .compute_at(output, y)
                .vectorize(x, vector_size / 2, TailStrategy::RoundUp);

            // We will compute the final stage, that is output on the DSP.
            // While doing so, we will split the output in half along the
            // height and parallelize. We will vectorize the x dimension.
            // The data type of output is at least 2 bytes, so if we
            // vectorize by a factor of vector_size/2, we'll get a full native
            // vector.
            //
            // The loop nest in pseudo code is
            // for<Hexagon> (int outermost = 0; outermost < 1; ++outermost) {
            //   lut_vtcm[][] = input[][];
            //   for<parallel> (int output.y = 0; output.y < 2; ++output.y) {
            //     int yi_start = (output.y) * (height/2));
            //     int yi_end  = yi_start + (height/2);
            //     for(int output.yi = yi_start;
            //         output.yi < yi_end: ++output.yi) {
            //       for (int output_vtcm.y = 0; output_vtcm.y < 1; ++output_vtcm.y) {
            //         for (int output_vtcm.x = 0; output_vtcm.x < output.x.extent; ++ output_vtcm.x) {
            //           output_vtcm[][] = input_vtcm[][];
            //       }
            //     }
            //     for<vectorized>(int output.x = 0; output.x < width; ++output.x) {
            //       output[][] = output_vtcm[][];
            //     }
            //   }
            // }

            output
                .hexagon()
                .split(y, y, yi, H / 2)
                .parallel(y)
                .vectorize(x, vector_size / 2);

            if (use_vtcm) {
                // Additionally, if we are compiling for a target that has VTCM
                // support (HVX ISA > v65), we can use gather instructions,
                // which means that the input and the output of the gather need
                // to be in VTCM.

                lut_vtcm
                    .store_in(MemoryType::VTCM)
                    .vectorize(x, vector_size / 2, TailStrategy::RoundUp);
                output_vtcm.store_in(MemoryType::VTCM);

                if (get_target().has_feature(Target::HVX_v68)) {
                    // Further, if we are compiling for HVX_v68, then we can
                    // use User DMA to copy data into VTCM.
                    // User DMA is a feature available on Hexagon ISA v68 or
                    // later.
                    lut_vtcm
                        .hexagon_user_dma();
                }
            }
        }
    }

private:
    Func lut_vtcm{"lut_vtcm"}, output_vtcm{"output_vtcm"};
    Var x{"x"}, y{"y"}, yi{"yi"};
};

HALIDE_REGISTER_GENERATOR(Gather, gather);

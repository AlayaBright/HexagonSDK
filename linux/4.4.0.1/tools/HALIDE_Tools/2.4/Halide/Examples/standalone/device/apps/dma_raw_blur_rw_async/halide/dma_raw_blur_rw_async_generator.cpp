/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

// This example demonstrates blur of input data in RAW format using UBWCDMA.
#include "Halide.h"

using namespace Halide;
#define LINE_BUFFERING 1

// Define a 1D Gaussian blur (a [1 4 6 4 1] filter) of 5 elements.
Expr blur5(Expr x0, Expr x1, Expr x2, Expr x3, Expr x4) {
    // Widen to 16 bits, so we don't overflow while computing the stencil.
    x0 = cast<uint16_t>(x0);
    x1 = cast<uint16_t>(x1);
    x2 = cast<uint16_t>(x2);
    x3 = cast<uint16_t>(x3);
    x4 = cast<uint16_t>(x4);
    return cast<uint8_t>((x0 + 4 * x1 + 6 * x2 + 4 * x3 + x4 + 8) / 16);
}

class DmaPipeline : public Generator<DmaPipeline> {
public:
    Input<Buffer<uint8_t>> input{"input", 2};
    Output<Buffer<uint8_t>> output{"output", 2};
    GeneratorParam<bool> use_line_buffering{"use_line_buffering", false};

    void generate() {
        Var x{"x"}, y{"y"};
        // We need a wrapper for the output so we can schedule the
        // multiply update in tiles.
        Func copy_bounded("copy_bounded");
        Func blur_y("blur_y");
        Func input_copy("input_copy"), output_copy("output_copy");
        Func work("work");
        /////////////////////////////////
        // Algorithm
        ////////////////////////////////
        // The first step is to copy the data to Locked L2 cache using UBWCDMA Read.
        // Once the data is transfered to Locked L2 cache, it is blurred by applying
        // a Gaussian blur filter. After the processing is complete, the result is
        // copied back from L2 cache to memory using UBWCDMA Write.

        // UBWCDMA Read
        input_copy(x, y) = input(x, y);
        // Bounding the X dimension accesses to be with in frame boundary
        Expr bounded_x = max(input.dim(0).min(), min(x, input.dim(0).extent() - 1));
        // Bounding the Y dimension accesses to be with in frame boundary
        Expr bounded_y = max(input.dim(1).min(), min(y, input.dim(1).extent() - 1));
        copy_bounded(x, y) = input_copy(bounded_x, bounded_y);
        // BLUR Filter Algorithm ( Y direction Blur)
        blur_y(x, y) = blur5(copy_bounded(x, y - 2),
                             copy_bounded(x, y - 1),
                             copy_bounded(x, y),
                             copy_bounded(x, y + 1),
                             copy_bounded(x, y + 2));

        // BLUR Filter Algorithm (X direction Blur)
        work(x, y) = blur5(blur_y(x - 2, y),
                           blur_y(x - 1, y),
                           blur_y(x, y),
                           blur_y(x + 1, y),
                           blur_y(x + 2, y));
        // UBWCDMA Write
        output(x, y) = work(x, y);

        /////////////////////////////////////////////////////
        // Schedule
        // Note: Order of scheduling directives are important
        ////////////////////////////////////////////////////
        // In this example, we have two choices of schedule - line buffered or tiled way.
        // In line buffered schedule, a full strip of input data is transferred in READ and WRITE phase.
        // In tiled schedule, we divide the input data into tiles and then transfer.
        // In the schedule, we also demonstrate that UBWCDMA READ-WRITES can be asynchronous.
        // Also, we demonstrate the re-use of buffer used for transfer by using fold_storage() directive.

        Func(output).hexagon_ubwcdma_write();  // Scheduling directive to use UBWCDMA engine for UBWCDMA write
        output
            .compute_root();  // Pure Function, Allocated Full Sized and computed at root
        if (use_line_buffering) {
            // Line Buffering
            Var yo("yo"), yi("yi"), yii("yii");
            // Break the output into Line strips

            Expr strip_size = output.dim(1).extent() / 2;
            strip_size = (strip_size / 2) * 2;

            // We will compute the output by dividing it into two halves, top and bottom.
            // Then we will use the parallel directive to compute over these two parts in parallel.
            // Then we follow this by splitting each half of the output image into strips of size strip_size.
            // This is the unit of data that we will move using UBWCDMA read.
            output
                .split(y, yo, y, strip_size)  // Split Outputs in to Strips of Two Lines
                .split(y, yi, yii, 128)
                .parallel(yo);  // Execute Parallely on Two Lines

            // Now, we will schedule input_copy. This is a copy operation whose destination is in
            // Hexagon L2 locked cache. By using the hexagon_ubwcdma_read scheduling directive,
            // we ensure that we use the UBWCDMA engine to do the copy (UBWCDMA read).
            // Note again that a UBWCDMA read requires us to store the buffer backing the Func in L2 locked cache.
            // The use of async() directive here implies that next read of the input_copy unit (line/tile)
            // won't wait for current unit of input_copy data to be processed and written back to output buffer.
            // This essentially means that read-write of transfer units is asynchronous.
            input_copy
                .compute_at(output, yi)             // Do the UBWCDMA Read for each Line.
                .prefetch(input, y, 2)
                .align_storage(x, 128)
                .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is a locked L2 cache.
                .hexagon_ubwcdma_read()             // Schedule directive to use UBWCDMA engine for UBWCDMA Read.
                .async()
                .fold_storage(y, 2);

            work.async()
                .compute_at(output, yi)
                .store_in(MemoryType::LockedCache)  // After Processing put the Output in a Locked cache.
                .vectorize(x, 128, TailStrategy::RoundUp);

            blur_y
                .async()
                .compute_at(output, yi)
                .store_in(MemoryType::LockedCache)  // After Processing put the Output in a Locked cache.
                .vectorize(x, 128, TailStrategy::RoundUp);

        } else {

            // Tiling Output with boundary condition as roundup
            Var tx("tx"), ty("ty");
            Var yo("yo"), yi("yi");
            // Break the output into Line strips
            Expr strip_size = output.dim(1).extent() / 2;
            strip_size = (strip_size / 2) * 2;
            // Break the output into tiles.
            const int tile_width = 256;
            const int tile_height = 64;

            // We will compute the output by dividing it into two halves, top and bottom.
            // Then we will use the parallel directive to compute over these two parts in parallel.
            // Then we follow this by splitting each half of the output image into tiles of size
            // tile_width * tile_height. This is the unit of data that we will move using UBWCDMA read.
            output
                .split(y, yo, yi, strip_size)
                .tile(x, yi, tx, ty, x, y, tile_width, tile_height, TailStrategy::RoundUp)
                .parallel(yo);

            // Now, we will schedule input_copy. This is a copy operation whose destination is in
            // Hexagon L2 locked cache. By using the hexagon_ubwcdma_read scheduling directive,
            // we ensure that we use the UBWCDMA engine to do the copy (UBWCDMA read).
            // Note again that a UBWCDMA read requires us to store the buffer backing the Func in L2 locked cache.
            input_copy
                .compute_at(output, tx)             // Do the UBWCDMA Read at each tile in the horizontal order.
                .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is a locked L2 cache.
                .store_at(output, ty)
                .hexagon_ubwcdma_read()             // Schedule directive to use UBWCDMA engine for UBWCDMA read.
                .async();                           // This directive ensure ping pong nature of the dma.

            // Schedule the work in tiles.
            work
                .compute_at(output, tx)
                .async()                            //This directive ensure ping pong nature of the dma.
                .store_in(MemoryType::LockedCache)  // After Processing, put the output in a locked Cache.
                .store_at(output, ty)
                .vectorize(x, 128, TailStrategy::RoundUp)
                .fold_storage(x, tile_width * 2);

            blur_y
                .compute_at(output, tx)
                .async()
                .store_in(MemoryType::LockedCache)
                .store_at(output, ty)
                .vectorize(x, 128, TailStrategy::RoundUp);
        }
    }
};

HALIDE_REGISTER_GENERATOR(DmaPipeline, dma_raw_blur_rw_async_halide)

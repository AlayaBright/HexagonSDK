/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#include "Halide.h"
#define LINE_BUFFERING 1
using namespace Halide;
// Helper function for gaussian 5x5 blur.
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
    Input<Buffer<uint8_t>> input_y{"input_y", 2};
    Input<Buffer<uint8_t>> input_uv{"input_uv", 3};
    Output<Buffer<uint8_t>> output_y{"output_y", 2};
    Output<Buffer<uint8_t>> output_uv{"output_uv", 3};

    void generate() {

        Var x{"x"}, y{"y"}, c{"c"};

        // We could use 'in' to generate the input copies, but we can't name the variables that way.
        Func input_y_copy("input_y_copy"), input_uv_copy("input_uv_copy");

        Func work_y("work_y");
        Func work_uv("work_uv");
        Func blur_y("blur_y");
        Func copy_bounded("copy_bounded");

        ////////////////////////////////////////
        // Algorithm
        ////////////////////////////////////////
        // This example demonstrates 5x5 gaussian blur for data in NV12 format using UBWCDMA.
        // The first step is to copy the data to Locked L2 cache using UBWCDMA Read.
        // Once the data is transfered to Locked L2 cache, it is blurred by applying a
        // Gaussian blur filter. After the processing is complete, the result is copied back
        // from Locked L2 cache to memory using UBWCDMA Write.

        // UBWCDMA Read
        input_y_copy(x, y) = input_y(x, y);
        Expr bounded_x = max(input_y.dim(0).min(), min(x, input_y.dim(0).extent() - 1));
        Expr bounded_y = max(input_y.dim(1).min(), min(y, input_y.dim(1).extent() - 1));
        copy_bounded(x, y) = input_y_copy(bounded_x, bounded_y);

        // BLUR Filter
        blur_y(x, y) = blur5(copy_bounded(x, y - 2),
                             copy_bounded(x, y - 1),
                             copy_bounded(x, y),
                             copy_bounded(x, y + 1),
                             copy_bounded(x, y + 2));

        work_y(x, y) = blur5(blur_y(x - 2, y),
                             blur_y(x - 1, y),
                             blur_y(x, y),
                             blur_y(x + 1, y),
                             blur_y(x + 2, y));

        // UBWCDMA Write
        output_y(x, y) = work_y(x, y);
        // UBWCDMA Read for UV
        input_uv_copy(x, y, c) = input_uv(x, y, c);
        // Basic Processing
        work_uv(x, y, c) = input_uv_copy(x, y, c) * 2;
        // UBWCDMA Write for UV
        output_uv(x, y, c) = work_uv(x, y, c);

        //////////////////////////////////////////////////////
        // Schedule
        // Note: Order of scheduling directives are important
        //////////////////////////////////////////////////////

        // In this example, we use schedule in such a way that a full strip of
        // input data is transferred in READ and WRITE phase for y component of the input data.
        // For UV component, we divide the input data into tiles and then transfer.
        // In the schedule, we also demonstrate that UBWCDMA READ-WRITES can be asynchronous.
        // Also, we demonstrate the re-use of buffer used for transfer by using fold_storage() directive.

        Var tx("tx"), ty("ty"), new_x("new_x"), yo("yo"), yi("yi"), yoo("yoo"), yii("yii");

        // Scheduling directive to use UBWCDMA engine for UBWCDMA write
        Func(output_y).hexagon_ubwcdma_write();
        Func(output_uv).hexagon_ubwcdma_write();

        output_y
            .compute_root();

        output_uv
            .compute_root()     // Pure Function, Allocated Full Sized and computed at root
            .bound(c, 0, 2)     // UV Plane has only Two components
            .reorder(c, x, y);  // Reorder so that the UBWCDMA transfer happen in natural Order

        // Adjusting the Buffer Stride and extent to support Interleaved UV data for 4:2:0 format
        // For both input and output
        input_uv.dim(0).set_stride(2);
        input_uv.dim(2).set_stride(1).set_bounds(0, 2);
        output_uv.dim(0).set_stride(2);
        output_uv.dim(2).set_stride(1).set_bounds(0, 2);

        // Break the output into tiles.
        const int tile_width = 256;
        const int tile_height = 128;

        const int strip_size_y = 32;
        Expr image_height = output_y.dim(1).extent();

        // We will compute the output by dividing it into two halves, top and bottom.
        // Then we will use the parallel directive to compute over these two parts in parallel.
        // Then we follow this by splitting each half of the output image into strips of size strip_size_y.
        // This is the unit of data that we will move using UBWCDMA read.

        output_y
            .split(y, yo, y, image_height / 2)
            .split(y, yi, yii, strip_size_y)
            .parallel(yo);

        // Now, we will schedule input_y_copy. This is a copy operation whose destination is in
        // Hexagon L2 locked cache. By using the hexagon_ubwcdma_read scheduling directive,
        // we ensure that we use the UBWCDMA engine to do the copy (UBWCDMA read).
        // Note again that a UBWCDMA read requires us to store the buffer backing the Func in L2 locked cache.
        // The use of async() directive here implies that next read of the input_y_copy unit (line/tile)
        // won't wait for current unit of input_y_copy data to be processed and written back to output_y buffer.
        // This essentially means that read-write of transfer units is asynchronous.
        input_y_copy
            .compute_at(output_y, yi)
            .align_bounds(y, strip_size_y)
            .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is a locked cache.
            .hexagon_ubwcdma_read()             // Schedule directive to use UBWCDMA engine for UBWCDMA read.
            .async()                            // UBWCDMA Read-Write can be asynchronous.
            .fold_storage(y, strip_size_y * 6); // Use the same allocated buffer for UBWCDMA Read-Write instead of freshly allocating each time.

        work_y
            .compute_at(output_y, yi)
            .async()                            // UBWCDMA Read-Write can be asynchronous.
            .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is L2 locked cache.
            .vectorize(x, 128, TailStrategy::RoundUp);

        blur_y
            .compute_at(output_y, yi)
            .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is L2 locked cache.
            .vectorize(x, 128, TailStrategy::RoundUp);

        // We will compute the output by dividing it into two halves, top and bottom.
        // Then we will use the parallel directive to compute over these two parts in parallel.
        // Then we follow this by splitting each half of the output image into tiles of size tile_width * tile_height.
        // This is the unit of data that we will move using UBWCDMA read.
        Expr strip_size_uv = output_uv.dim(1).extent() / 2;
        strip_size_uv = (strip_size_uv / 2) * 2;
        output_uv
            .split(y, yo, y, strip_size_uv)
            .tile(x, y, tx, ty, x, y, tile_width, tile_height, TailStrategy::RoundUp)
            .parallel(yo);

        // Now, we will schedule input_y_copy. This is a copy operation whose destination is in
        // Hexagon L2 locked cache. By using the hexagon_ubwcdma_read scheduling directive,
        // we ensure that we use the UBWCDMA engine to do the copy (UBWCDMA read).
        // Note again that a UBWCDMA read requires us to store the buffer backing the Func in L2 locked cache.
        input_uv_copy
            .compute_at(output_uv, tx)          // Do the UBWCDMA Read at each tile in the horizontal order
            .hexagon_ubwcdma_read()             // Schedule directive to use UBWCDMA engine for UBWCDMA Read
            .bound(c, 0, 2)                     // Limit the c dimension to 2(UV interleaved data) for optimization
            .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is a locked cache
            .reorder_storage(c, x, y)           // Reorder dimensions to make C innermost loop to access U and V sequentially
            .reorder(c, x, y)
            .fuse(c, x, new_x);

        work_uv
            .compute_at(output_uv, tx)          // Do the Processing at each tile in the horizontal order
            .bound(c, 0, 2)                     // Limit the c dimension to 2(UV interleaved data) for optimization
            .store_in(MemoryType::LockedCache)  // After Processing, put the Output in a locked cache
            .reorder_storage(c, x, y)           // Reorder dimensions to make C innermost loop to access U and V sequentially
            .reorder(c, x, y)
            .fuse(c, x, new_x)
            .vectorize(new_x, 128, TailStrategy::RoundUp);
    }
};

HALIDE_REGISTER_GENERATOR(DmaPipeline, dma_nv12_blur_stencil_halide)

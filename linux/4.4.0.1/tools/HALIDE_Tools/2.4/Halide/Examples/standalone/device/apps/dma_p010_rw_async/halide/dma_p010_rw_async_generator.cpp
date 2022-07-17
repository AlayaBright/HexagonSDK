/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

// This Example demonstrates data processing in P010 format using UBWCDMA engine.
#include "Halide.h"

using namespace Halide;

class DmaPipeline : public Generator<DmaPipeline> {
public:
    Input<Buffer<uint16_t>> input_y{"input_y", 2};
    Input<Buffer<uint16_t>> input_uv{"input_uv", 3};
    Output<Buffer<uint16_t>> output_y{"output_y", 2};
    Output<Buffer<uint16_t>> output_uv{"output_uv", 3};

    void generate() {

        Var x{"x"}, y{"y"}, c{"c"};

        // We could use 'in' to generate the input copies, but we can't name the variables that way.
        Func input_y_copy("input_y_copy"), input_uv_copy("input_uv_copy");

        Func work_y("work_y");
        Func work_uv("work_uv");
        ////////////////////////////////////////
        // Algorithm
        ////////////////////////////////////////
        // The first step is to copy the data to Locked L2 cache using UBWCDMA Read.
        // Once the data is transfered to Locked L2 cache, it is further processed for simple
        // processing. After the processing is complete, the result is copied back
        // from L2 cache to memory using UBWCDMA Write.

        // UBWCDMA Read
        input_y_copy(x, y) = input_y(x, y);
        // Basic Processing
        work_y(x, y) = input_y_copy(x, y) * 2;
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
        Var tx("tx"), ty("ty"), new_x("new_x");
        // Scheduling directive to use UBWCDMA engine for UBWCDMA write
        Func(output_y).hexagon_ubwcdma_write();
        Func(output_uv).hexagon_ubwcdma_write();

        output_y
            .compute_root();

        output_uv
            .compute_root()     // Pure Function, Allocated Full Sized and computed at root.
            .bound(c, 0, 2)     // UV Plane has only Two components.
            .reorder(c, x, y);  // Re-order so that the UBWCDMA transfer happen in natural Order.

        // Adjusting the Buffer Stride and extent to support Interleaved UV data for 4:2:0 format
        // for both input and output.
        input_uv.dim(0).set_stride(2);
        input_uv.dim(2).set_stride(1).set_bounds(0, 2);
        output_uv.dim(0).set_stride(2);
        output_uv.dim(2).set_stride(1).set_bounds(0, 2);

        // Break the output into tiles.
        const int tile_width = 256;
        const int tile_height = 64;
        Var yo, yi;
        // Tiling Output with boundary condition as roundup.
        // We will compute the output by dividing it into two halves, top and bottom.
        // Then we will use the parallel directive to compute over these two parts in parallel.
        // Then we follow this by splitting each half of the output image into tiles of size 
        // tile_width * tile_height. This is the unit of data that we will move using UBWCDMA read.
        // The use of async() directive here implies that next read of the input_y_copy unit (line/tile)
        // won't wait for current unit of input_y_copy data to be processed and written back to output_y buffer.
        // This essentially means that read-write of transfer units is asynchronous.
        Expr fac_y = output_y.dim(1).extent() / 2;
        output_y
            .split(y, yo, yi, fac_y)
            .tile(x, yi, tx, ty, x, y, tile_width, tile_height, TailStrategy::RoundUp)
            .parallel(yo);
 
        Expr fac_uv = output_uv.dim(1).extent() / 2;
        output_uv
            .split(y, yo, yi, fac_uv)
            .tile(x, yi, tx, ty, x, y, tile_width, tile_height, TailStrategy::RoundUp)
            .parallel(yo);

        input_y_copy
            .compute_at(output_y, tx)           // Do the UBWCDMA Read at each tile in the horizontal order.
            .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is a locked cache.
            .hexagon_ubwcdma_read()             // Schedule directive to use UBWCDMA engine for UBWCDMA Read.
            .async()                            // Directive to enable ping pong function in UBWCDMA along with fold.
            .fold_storage(x, tile_width * 2);

        input_uv_copy
            .compute_at(output_uv, tx)          // Do the UBWCDMA Read at each tile in the horizontal order.
            .hexagon_ubwcdma_read()             // Schedule directive to use UBWCDMA engine for UBWCDMA Read.
            .async()                            // Directive to enable ping pong function in UBWCDMA along with fold.
            .bound(c, 0, 2)                     // Limit the c dimension to 2(UV interleaved data) for optimization.
            .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is a locked cache.
            .reorder_storage(c, x, y)           // Reorder dimensions to make C innermost loop to access U and V sequentially.
            .reorder(c, x, y)
            .fuse(c, x, new_x)
            .fold_storage(x, tile_width * 2);
        // Schedule the work in tiles (same for all UBWCDMA schedules).
        work_y.compute_at(output_y, tx)         // Do the Processing at each tile in the horizontal order.
            .async()                            // Directive to enable ping pong function in UBWCDMA along with fold.
            .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is a locked cache.
            .fold_storage(x, tile_width * 2)
            .vectorize(x, 128, TailStrategy::RoundUp);

        work_uv
            .async()                            // Directive to enable ping pong function in UBWCDMA along with fold.
            .store_in(MemoryType::LockedCache)  // The destination of UBWCDMA read is a locked cache.
            .fold_storage(x, tile_width * 2)
            .compute_at(output_uv, tx)  // Do the Processing at each tile in the horizontal order.
            .bound(c, 0, 2)             // Limit the c dimension to 2(UV interleaved data) for optimization.
            .reorder_storage(c, x, y)   // Reorder dimensions to make C innermost loop to access U and V sequentially.
            .reorder(c, x, y)
            .fuse(c, x, new_x)
            .vectorize(new_x, 128, TailStrategy::RoundUp);
    }
};

HALIDE_REGISTER_GENERATOR(DmaPipeline, dma_p010_rw_async_halide)

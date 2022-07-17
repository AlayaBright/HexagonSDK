/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

// This file demonstrates the how a Halide pipeline is called
// from the application side. In offload mode, this is a host
// (CPU) executable.
// We also demonstrate the calls that can be made into the
// Halide runtime to set it up for HVX execution.

#include "process.h"
#include "HalideBuffer.h"
#include "HalideRuntimeHexagonHost.h"
#include "halide_benchmark.h"
#include "pipeline.h"
#include "test_report.h"
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
using namespace Halide::Runtime;
using namespace Halide::Tools;

// Verify result for the halide pipeline
int checker(Buffer<DTYPE> &in,
            Buffer<DTYPE> &lut,
            Buffer<DTYPE> &out) {
    int errcnt = 0, maxerr = 10;
    printf("Checking...\n");

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            DTYPE xidx = std::min(std::max((int)in(x, 0), 0), W - 1);
            DTYPE yidx = std::min(std::max((int)in(x, 1), 0), H - 1);
            if (out(x, y) != lut(xidx, yidx)) {
                errcnt++;
                if (errcnt <= maxerr) {
                    printf("Mismatch at (%d, %d): %ld (Halide) == %ld (Expected)\n",
                           x, y, (long)out(x, y), (long)lut(xidx, yidx));
                }
            }
        }
    }
    if (errcnt > maxerr) {
        printf("...\n");
    }
    if (errcnt > 0) {
        printf("Mismatch at %d places\n", errcnt);
    }
    return errcnt;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s (iterations)\n", argv[0]);
        return 0;
    }
    assert(W % 128 == 0);

    int iterations = atoi(argv[1]);

    // Define three external buffers for the pipeline
    // Two input buffers - the look-up table (LUT) and the indices to be looked
    //                     up.
    // One output buffer - The result of the look up operation.
    // The buffers are all (W x H) in size. we allocate no memory for them
    // initially.
    Buffer<DTYPE> in(nullptr, W, H);
    Buffer<DTYPE> out(nullptr, W, H);
    Buffer<DTYPE> lut(nullptr, W, H);

    // Now we'll allocate memory for the buffers. If we use
    // halide_hexagon_device_interface() for the buffers, we get two
    // benefits:
    // 1. In Device Offload mode, the ION memory manager is used to allocate a
    //    buffer such that no copies are needed to move the buffer from the host
    //    to the DSP (Zero-copy buffers)
    // 2. Buffers are aligned to a 128 byte boundary, ideal for HVX.
    // Note that the following calls will set both the host and device pointers
    // of each buffer.
    in.device_malloc(halide_hexagon_device_interface());
    out.device_malloc(halide_hexagon_device_interface());
    lut.device_malloc(halide_hexagon_device_interface());

    srand(0);
    // Fill the input image
    in.for_each_value([&](DTYPE &x) {
        x = static_cast<DTYPE>(rand());
    });
    // Fill the lookup table
    lut.for_each_value([&](DTYPE &x) {
        x = static_cast<DTYPE>(rand());
    });

    // To avoid the cost of powering HVX on in each call of the
    // pipeline, power it on once now. Also, set Hexagon performance to turbo.
    halide_hexagon_set_performance_mode(NULL, halide_hexagon_power_turbo);
    halide_hexagon_power_hvx_on(NULL);

    printf("Running pipeline...\n");
    printf("Image size: %dx%d pixels\n", W, H);
    printf("Image type: %d bits\n", (int)sizeof(DTYPE) * 8);
    printf("Table size: %d elements\n", W * H);

    // Make a call to run the pipeline.
    double time = benchmark(iterations, 10, [&]() {
        int result = pipeline(in, lut, out);
        if (result != 0) {
            printf("pipeline failed! %d\n", result);
        }
    });
    out.copy_to_host();

    // We're done with HVX, power it off, and reset the performance mode
    // to default to save power.
    halide_hexagon_power_hvx_off(NULL);
    halide_hexagon_set_performance_mode(NULL, halide_hexagon_power_default);

    if (checker(in, lut, out) != 0) {
        TestReport tr("vtcm/gather", (W * H) / (1024 * 1024 * time), "MPixels/sec", Mode::Unknown_Mode, Result::Fail);
        tr.print();
        return 1;
    }

    // Everything worked!
    TestReport tr("vtcm/gather", (W * H) / (1024 * 1024 * time), "MPixels/sec", Mode::Unknown_Mode, Result::Pass);
    tr.print();

    printf("Success!\n");
    return 0;
}

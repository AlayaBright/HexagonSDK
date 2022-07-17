/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

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
int checker(Buffer<DTYPE> &in, Buffer<DTYPE> &lut, Buffer<DTYPE> &out) {
    int errcnt = 0, maxerr = 10;
    printf("Checking...\n");

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            DTYPE idx = std::min(std::max((int)in(x, y), 0), lut.width() - 1);
            DTYPE part0 = lut(x) + 1;
            // part1 and lut have identical values
            DTYPE part1 = part0 - 1;
            DTYPE part2 = in(x, y) % 32;
            DTYPE part3 = in(x, y) / 16;
            DTYPE part4 = lut(idx);
            DTYPE expected = part0 + part1 + part2 + part3 + part4;
            if (out(x, y) != expected) {
                errcnt++;
                if (errcnt <= maxerr) {
                    printf("Mismatch at (%d, %d): %ld (Halide) == %ld (Expected)\n",
                           x, y, (long)out(x, y), (long)expected);
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

    Buffer<DTYPE> in(nullptr, W, H);
    Buffer<DTYPE> lut(nullptr, LUT_SIZE);
    Buffer<DTYPE> out(nullptr, W, H);

    in.device_malloc(halide_hexagon_device_interface());
    lut.device_malloc(halide_hexagon_device_interface());
    out.device_malloc(halide_hexagon_device_interface());

    // Fill the input buffers
    srand(0);
    in.for_each_value([&](DTYPE &x) {
        x = static_cast<DTYPE>(rand() % 256);
    });
    lut.for_each_value([&](DTYPE &x) {
        x = static_cast<DTYPE>(rand() % 256);
    });

    // To avoid the cost of powering HVX on in each call of the
    // pipeline, power it on once now. Also, set Hexagon performance to turbo.
    halide_hexagon_set_performance_mode(NULL, halide_hexagon_power_turbo);
    halide_hexagon_power_hvx_on(NULL);

    printf("Running pipeline...\n");
    printf("Image size: %dx%d pixels\n", W, H);
    printf("Image type: %d bits\n", (int)sizeof(DTYPE) * 8);

    double time = benchmark(iterations, 1, [&]() {
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

    printf("Time           : %lfms\n", time * 1000);

    if (checker(in, lut, out) != 0) {
        TestReport tr("vtcm/vtcm_alloc", (W * H) / (1024 * 1024 * time), "MPixels/sec", Mode::Unknown_Mode, Result::Fail);
        tr.print();
        return 1;
    }

    // Everything worked!
    TestReport tr("vtcm/vtcm_alloc", (W * H) / (1024 * 1024 * time), "MPixels/sec", Mode::Unknown_Mode, Result::Pass);
    tr.print();
    printf("Success!\n");
    return 0;
}

/* =============================================================================================== */
/*  QUALCOMM TECHNOLOGIES, INC.                                                                    */
/*                                                                                                 */
/*  Halide for HVX Image Processing Example                                                        */
/*                                                                                                 */
/* ----------------------------------------------------------------------------------------------- */
/*          Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.           */
/*                           All Rights Reserved.                                                  */
/*                  QUALCOMM Confidential and Proprietary                                          */
/* =============================================================================================== */
#include "io.h"
#include "simulator_benchmark.h"
#include "test_report.h"
#include <hexagon_standalone.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#if RUN_BASELINE == 0
#include "HalideBuffer.h"
#include "conv3x3a32.h"

#else
extern "C" {
#include "conv3x3a32.h"
}
#endif
#define VLEN 128
#define LOG2VLEN 7

signed char mask[9] =
    {
        1, -4, 7,
        2, -5, 8,
        3, -6, 9};

signed char mask_intrinsic[12] =
    {
        1, -4, 7, 0,
        2, -5, 8, 0,
        3, -6, 9, 0};

int main(int argc, char **argv) {
    // Create the Input.

    int x, y;
    int i, j;
    FH fp;
    constexpr int dims = 2;

    /* -----------------------------------------------------*/
    /*  Get input parameters                                */
    /* -----------------------------------------------------*/
    if (argc != 5) {
        printf("usage: %s <width> <height> <input.bin> <output.bin>\n", argv[0]);
        return 1;
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int stride = (width + (VLEN)-1) & (-(VLEN));

    /* -----------------------------------------------------*/
    /*  Allocate memory for input/output                    */
    /* -----------------------------------------------------*/
    unsigned char *input = (unsigned char *)memalign(1 << LOG2VLEN, width * height * sizeof(unsigned char));
    unsigned char *output = (unsigned char *)memalign(1 << LOG2VLEN, width * height * sizeof(unsigned char));

    if (input == NULL || output == NULL) {
        printf("Error: Could not allocate Memory for image\n");
        return 1;
    }

    /* -----------------------------------------------------*/
    /*  Read image input from file                          */
    /* -----------------------------------------------------*/
    if ((fp = open(argv[3], O_RDONLY)) < 0) {
        printf("Error: Cannot open %s for input\n", argv[3]);
        return 1;
    }

    for (i = 0; i < height; i++) {
        if (read(fp, &input[i * width], sizeof(unsigned char) * width) != width) {
            printf("Error, Unable to read from %s\n", argv[3]);
            close(fp);
            return 1;
        }
    }
    close(fp);

#if RUN_BASELINE == 0

    long long cycles;

    halide_dimension_t x_dim{0, width, 1};
    halide_dimension_t y_dim{0, height, width};
    halide_dimension_t io_shape[2] = {x_dim, y_dim};
    halide_dimension_t mask_shape[2];
    mask_shape[0].min = 0;
    mask_shape[0].extent = 3;
    mask_shape[0].stride = 1;
    mask_shape[1].min = 0;
    mask_shape[1].extent = 3;
    mask_shape[1].stride = 3;

    Halide::Runtime::Buffer<uint8_t> input1_buf(input, dims, io_shape);
    Halide::Runtime::Buffer<int8_t> mask_buf(mask, dims, mask_shape);
    Halide::Runtime::Buffer<uint8_t> output_buf(output, dims, io_shape);

    SIM_ACQUIRE_HVX;
    SIM_SET_HVX_DOUBLE_MODE;
    cycles = benchmark([&]() {
        int error = conv3x3a32(input1_buf, mask_buf, output_buf);
        if (error != 0) {
            printf("conv3x3a32 pipeline failed: %d\n", error);
        }
    });

    SIM_RELEASE_HVX;
    double cycles_per_pixel = (double)cycles / width / height;
    printf("AppReported: Image %dx%d - conv3x3a32(128B): %0.4f cycles/pixel\n", (int)width, (int)height, cycles_per_pixel);
    TestReport tr("conv3x3a32", cycles_per_pixel, "cyc/pix", Mode::Simulator_Standalone);
    tr.print();

    /* -----------------------------------------------------*/
    /*  Write image output to file                          */
    /* -----------------------------------------------------*/
    if ((fp = open(argv[4], O_CREAT_WRONLY_TRUNC, 0777)) < 0) {
        printf("Error: Cannot open %s for output\n", argv[4]);
        return 1;
    }

    if (write_file(fp, output, height, width, 1) != 0) {
        printf("Error: Cannot write to file %s\n", argv[4]);
    }

    close(fp);

#else

    long long cycles_baseline;
    FH fp_baseline;

    // Run basline version
    memset(output, 0, width * height);
    SIM_ACQUIRE_HVX;
    SIM_SET_HVX_DOUBLE_MODE;
    cycles_baseline = benchmark([&]() {
        int error = conv3x3a32_i(input, stride, width, height, mask_intrinsic, 4, output);
        if (error != 0) {
            printf("conv3x3a32_i pipeline failed: %d\n", error);
        }
    });

    SIM_RELEASE_HVX;
    printf("AppReported: Image %dx%d - conv3x3a32_baseline(128B): %0.4f cycles/pixel\n", (int)width, (int)height, (float)cycles_baseline / width / height);

    /* -----------------------------------------------------*/
    /*  Write image output to file                          */
    /* -----------------------------------------------------*/
    if ((fp_baseline = open(argv[4], O_CREAT_WRONLY_TRUNC, 0777)) < 0) {
        printf("Error: Cannot open %s for output\n", argv[4]);
        return 1;
    }

    if (write_file(fp_baseline, output, height, width, 1) != 0) {
        printf("Error: Cannot write to file %s\n", argv[4]);
    }

    close(fp_baseline);
#endif

    free(input);
    free(output);
    free(mask);
    free(mask_intrinsic);

    printf("Success!\n");
    return 0;
}

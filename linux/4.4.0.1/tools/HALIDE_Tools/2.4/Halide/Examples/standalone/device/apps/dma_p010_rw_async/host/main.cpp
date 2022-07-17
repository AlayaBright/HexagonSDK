/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

// Headers
#include "buffer.h"
#include "dma_p010_rw_async.h"
#include "ion_allocation.h"
#include "rpcmem.h"
#include "test_report.h"
#include <algorithm>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
// Verification function.
template<typename T>
bool verify(T *input_ptr, T *output_ptr, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            T correct = input_ptr[i * w + j] * 2;
            if ((correct != output_ptr[i * w + j]) &&
                (correct != output_ptr[i * w + j] + 1) &&
                (correct != output_ptr[i * w + j] - 1)) {
                printf("Mismatch at %d : %d != %d\n", i, correct, output_ptr[i * w + j]);
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char *argv[]) {

    // The arguments to the main function are supplied through command line.
    // width : Width of the Input Image.
    // height : Height of Input Image.
    // stride : Stride of the Input Image.
    // format : Format of Input Image. 0 : RAW. 1 : NV12. 4 : P010.
    // is_input_ubwc : This is a boolean flag indicating if the input image is UBWC or Linear frame.
    // is_output_ubwc: This is a boolean flag indicating if the required output image should be UBWC or Linear frame.
    // input_filename: This is the name of the file from where input data will be read into a buffer.
    // validate_filename: This is the name of the file which will be used to validate the output produced by halide pipeline.
    if (argc != 9) {
        printf("usage: %s <width> <height> <stride> <format NV12 : 1 > <is_input_ubwc> <is_output_ubwc> <input_filename> <validate_filename> \n", argv[0]);
        return 1;
    }
    // Initialize the ion allocator.
    // Use Ion Memory for Input and OutPut Frames ( DMA Needs Contigious Memory and Ion provides that).
    alloc_init();
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int stride = atoi(argv[3]);
    int format = atoi(argv[4]);
    int is_input_ubwc = atoi(argv[5]);
    int is_output_ubwc = atoi(argv[6]);
    char *input_filename = argv[7];
    char *validate_filename = argv[8];
    // Set HVX to Turbo mode for Peak Performance.
    int set_perf_mode_turbo = dma_p010_rw_async_set_hvx_perf_mode_turbo();
    if (set_perf_mode_turbo != 0) {
        printf("Error: Couldn't set perf mode to turbo: %d\n", set_perf_mode_turbo);
    }
    // Power on HVX.
    int power_on = dma_p010_rw_async_power_on_hvx();
    if (power_on != 0) {
        printf("Error: Couldn't power on hvx: %d\n", power_on);
        abort();
    }

    // To allocate the buffer for UBWC/Linear input frames, we query the UBWCDMA engine to get the size of the buffers.
    // UBWCDMA decides the size of the compressed data frame.
    int linear_frame_size = 0;
    int ret = dma_p010_rw_async_getLinearFrameSize(width, height, stride, format, &linear_frame_size);
    if (ret != 0) {
        printf("dma_p010_rw_async_getLinearFrameSize failed\n");
        error("dma_p010_rw_async");
    }
    int linear_frame_height = linear_frame_size / stride;
    int ubwc_frame_size = 0;
    ret = dma_p010_rw_async_getUBWCFrameSize(width, height, stride, format, &ubwc_frame_size);
    if (ret != 0) {
        printf("dma_p010_rw_async_getUBWCFrameSize failed\n");
        error("dma_p010_rw_async");
    }
    int ubwc_frame_height = ubwc_frame_size / stride;

    printf("width and height of ubwc frame = %d, %d \n", width, ubwc_frame_height);
    printf("width and height of linear frame = %d, %d \n", width, linear_frame_height);

    buffer_2d<uint8_t> input(width, is_input_ubwc ? ubwc_frame_height : linear_frame_height);            // Input frame ION Allocation.
    buffer_2d<uint8_t> input_validate(width, is_output_ubwc ? ubwc_frame_height : linear_frame_height);  // Validation Buffer Ion Memory Allocation.
    buffer_2d<uint8_t> output(width, is_output_ubwc ? ubwc_frame_height : linear_frame_height);          // Output Frame Ion Allocation.

    uint8_t *internal_memory = input.get_buffer();              // Pixel Buffer of Input Frame.
    uint8_t *memory_to_validate = input_validate.get_buffer();  // Pixel Buffer of Validation Frame.

    int fp;
    if ((fp = open(input_filename, O_RDONLY)) < 0) {
        printf("Error: Cannot open %s for input\n", input_filename);
        return 1;
    }
    // Read the Input Frame to Input Frame Buffer.
    for (int i = 0; i < (is_input_ubwc ? ubwc_frame_height : linear_frame_height); i++) {
        if (read(fp, &internal_memory[i * width], sizeof(unsigned char) * width) != width) {
            printf("Error, Unable to read from %s\n", input_filename);
            close(fp);
            return 1;
        }
    }
    close(fp);
    // Read the Validation frame to Validation Frame Buffer.
    if ((fp = open(validate_filename, O_RDONLY)) < 0) {
        printf("Error: Cannot open %s for input\n", validate_filename);
        return 1;
    }

    for (int i = 0; i < (is_output_ubwc ? ubwc_frame_height : linear_frame_height); i++) {
        if (read(fp, &memory_to_validate[i * width], sizeof(unsigned char) * width) != width) {
            printf("Error, Unable to read from %s\n", validate_filename);
            close(fp);
            return 1;
        }
    }
    close(fp);

    int iterations = 10;
    printf("Running pipeline... \n");
    if (format == 1)
        printf(" for format NV12");
    else if (format == 4)
        printf(" for format P010");

    printf("ubwc %d \n", is_input_ubwc);
    uint64 avg_time = 0;

    printf("Running pipeline\n");
    // Invoke the Host and Hexagon Interfacing Function to Perform DMA RD + WR.
    int result = dma_p010_rw_async_run(input.get_buffer(), input.len(), width, height, stride, format, is_input_ubwc, is_output_ubwc, output.get_buffer(), output.len(), iterations, &avg_time);
    if (result) {
        printf("Error: HVX pipeline failed with error %d\n", result);
        exit(1);
    }
    // Power of HVX as the processing is done.
    dma_p010_rw_async_power_off_hvx();

    if (!verify<uint16_t>((uint16_t *)(input_validate.get_buffer()), (uint16_t *)(output.get_buffer()), width, (is_output_ubwc ? ubwc_frame_height : linear_frame_height) / 2)) {
        error("dma_p010_rw_async", avg_time);
    } else {
        TestReport tr("dma_p010_rw_async", avg_time, "microseconds", Mode::Device_Standalone, Result::Pass);
        tr.print();
    }
    // Free the ION Buffers.
    input.free_buff();
    input_validate.free_buff();
    output.free_buff();
    alloc_finalize();
    printf("Success\n");
    return 0;
}

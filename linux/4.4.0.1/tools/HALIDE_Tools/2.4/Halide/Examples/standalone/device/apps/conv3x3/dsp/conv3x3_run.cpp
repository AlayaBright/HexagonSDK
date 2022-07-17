#include "HAP_farf.h"
#include "HAP_power.h"
#include "hexagon_benchmark.h"
#include "hexagon_types.h"
#include "hvx_interface.h"
#include "qurt_error.h"
#include "qurt_hvx.h"
#include <stdlib.h>

#include "conv3x3.h"
#include "conv3x3_halide.h"

#include "HalideRuntime.h"

#include <stdlib.h>
#ifndef SIMULATOR
#undef FARF_LOW
#define FARF_LOW 1
#undef FARF_HIGH
#define FARF_HIGH 1
#endif

int conv3x3_power_on_hvx() {
    FARF(HIGH, "\npower_on\n");
    return power_on_hvx();
}

int conv3x3_power_off_hvx() {
    FARF(HIGH, "\npower_off\n");
    return power_off_hvx();
}

int conv3x3_set_hvx_perf_mode_low() {
    FARF(HIGH, "\nperf_low\n");
    return set_hvx_perf_mode_low();
}

int conv3x3_set_hvx_perf_mode_nominal() {
    FARF(HIGH, "\nperf_nominal\n");
    return set_hvx_perf_mode_nominal();
}

int conv3x3_set_hvx_perf_mode_turbo() {
    FARF(HIGH, "\nperf_turbo\n");
    return set_hvx_perf_mode_turbo();
}

int conv3x3_run(const unsigned char *input_buffer,
                int input_bufferLen, const char *mask, int mask_bufferLen,
                int stride, int height, int width, int vlen,
                uint8_t *output_buffer, int output_bufferLen,
                int iterations, uint64_t *avg_time) {
    int y;

    halide_buffer_t input1_buf = {0}, output_buf = {0};
    halide_buffer_t mask_buf = {0};
    halide_dimension_t in_dim[2] = {{0, 0, 0}, {0, 0, 0}};
    halide_dimension_t out_dim[2] = {{0, 0, 0}, {0, 0, 0}};
    halide_dimension_t mask_dim[2] = {{0, 0, 0}, {0, 0, 0}};

    input1_buf.type.code = halide_type_uint;
    input1_buf.type.bits = 8;
    input1_buf.type.lanes = 1;
    input1_buf.dimensions = 2;

    output_buf = input1_buf;
    mask_buf = input1_buf;
    mask_buf.type.code = halide_type_int;

    input1_buf.host = (uint8_t *)input_buffer;
    output_buf.host = (uint8_t *)output_buffer;
    mask_buf.host = (uint8_t *)mask;

    input1_buf.dim = in_dim;
    output_buf.dim = out_dim;
    mask_buf.dim = mask_dim;

    input1_buf.dim[0].stride = 1;
    input1_buf.dim[0].extent = width;
    input1_buf.dim[1].stride = width;
    input1_buf.dim[1].extent = height;

    output_buf.dim[0].stride = 1;
    output_buf.dim[0].extent = width;
    output_buf.dim[1].stride = width;
    output_buf.dim[1].extent = height;

    mask_buf.dim[0].stride = 1;
    mask_buf.dim[0].extent = 3;
    mask_buf.dim[1].stride = 3;
    mask_buf.dim[1].extent = 3;

    int error = 0;
    (*avg_time) = benchmark(iterations, [&]() {
        error = conv3x3_halide(&input1_buf, &mask_buf, &output_buf);
        if (error != 0) {
            FARF(LOW, "Pipeline failed with error code = %d\n", error);
            exit(1);
        }
    });

    if (error) {
        return error;
    }

    return 0;
}

#ifndef __hexagon__
#include "ion_allocation.h"
#include "rpcmem.h"
#endif
#include "buffer.h"
#include "conv3x3.h"
#include "test_report.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#define FASTRPC_THREAD_PARAMS (1)
#define CDSP_DOMAIN_ID    3

using namespace std;

template<typename T>
static T clamp(T val, T min, T max) {
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}

bool verify(buffer_2d<uint8_t> &input, buffer_2d<int8_t> &mask, buffer_2d<uint8_t> &output, int w, int h) {
#ifdef VERIFY
    auto u8_in = [&](int x_, int y_) { return input(clamp(x_, 0, w - 1), clamp(y_, 0, h - 1)); };
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int16_t sum = 0;
            for (int ry = -1; ry <= 1; ry++) {
                for (int rx = -1; rx <= 1; rx++) {
                    sum += static_cast<int16_t>(u8_in(x + rx, y + ry)) * static_cast<int16_t>(mask(rx + 1, ry + 1));
                }
            }
            sum = sum >> 4;
            if (sum > 255) {
                sum = 255;
            } else if (sum < 0) {
                sum = 0;
            }
            uint8_t u8_sum = static_cast<uint8_t>(sum);
            uint8_t out_val = output(x, y);
            if (out_val != u8_sum) {
                printf("mismatch at (x = %d, y= %d). Expected: %d, got %d\n", x, y, u8_sum, out_val);
                for (int i = -1; i <= 1; ++i) {
                    int16_t s = 0;
                    for (int j = -1; j <= 1; ++j) {
                        s += static_cast<int16_t>(u8_in(x + j, y + i)) * static_cast<int16_t>(mask(j + 1, i + 1));
                        printf("(%d*%d)\t", u8_in(x + j, y + i), mask(j + 1, i + 1));
                        if (j < 1) printf(" + ");
                    }
                    printf("s (row %d) = %d\n", i + 1, s);
                }
                return false;
            }
        }
    }
#endif
    return true;
}

int main(int argc, char *argv[]) {

    struct remote_rpc_thread_params th;
    th.domain = CDSP_DOMAIN_ID;
    th.prio = -1;
    th.stack_size = 32*1024;
    if (&remote_session_control) {
        int set_stack_size = remote_session_control(FASTRPC_THREAD_PARAMS, (void*)&th, sizeof(th));
        if (set_stack_size != 0) {
            printf("Error: Couldn't set stack size for remote rpc session: %d\n", set_stack_size);
        }
    }

    alloc_init();

    const int width = atoi(argv[1]);
    const int height = atoi(argv[2]);
    const int vlen = atoi(argv[3]);
    const int iterations = atoi(argv[4]);

    int stride = (width + vlen - 1) & ~(vlen - 1);

    buffer_2d<uint8_t> input(stride, height);
    buffer_2d<int8_t> mask(3, 3);
    buffer_2d<uint8_t> output(stride, height);

    // Allocate memory
    input.set_all((uint8_t)(rand() & 0xFF));
    output.set_zero();
    mask.set_zero();

    mask(0, 0) = 1;
    mask(1, 0) = -4;
    mask(2, 0) = 7;
    mask(0, 1) = 2;
    mask(1, 1) = -5;
    mask(2, 1) = 8;
    mask(0, 2) = 3;
    mask(1, 2) = -6;
    mask(2, 2) = 9;

    int set_perf_mode_turbo = conv3x3_set_hvx_perf_mode_turbo();
    if (set_perf_mode_turbo != 0) {
        printf("Error: Couldn't set perf mode to turbo: %d\n", set_perf_mode_turbo);
    }

    int power_on = conv3x3_power_on_hvx();
    if (power_on != 0) {
        printf("Error: Couldn't power on hvx: %d\n", power_on);
        abort();
    }

    uint64 avg_time = 0;
    printf("Running pipeline... \n");
    int result = conv3x3_run(input.get_buffer(), input.len(), (const char *)(mask.get_buffer()), mask.len(),
                             stride, height, width, vlen, output.get_buffer(), output.len(),
                             iterations, &avg_time);
    conv3x3_power_off_hvx();

    if (!verify(input, mask, output, width, height)) {
        TestReport tr("conv3x3", avg_time, "microseconds", Mode::Device_Standalone, Result::Fail);
        tr.print();
        printf("Failed\n");
        exit(1);
    } else {
        TestReport tr("conv3x3", avg_time, "microseconds", Mode::Device_Standalone, Result::Pass);
        tr.print();
    }

    input.free_buff();
    mask.free_buff();
    output.free_buff();
    alloc_finalize();
    printf("Success\n");
}

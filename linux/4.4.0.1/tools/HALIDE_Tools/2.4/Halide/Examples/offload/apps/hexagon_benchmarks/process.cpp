/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "halide_benchmark.h"
#include "process.h"
#include "test_report.h"
void usage(char *prg_name) {
    const char usage_string[] = " Run a bunch of small filters\n\n"
                                "\t -n -> number of iterations\n"
                                "\t -h -> print this help message\n";
    printf("%s - %s", prg_name, usage_string);
}

int main(int argc, char **argv) {
    // Set some defaults first.
    const int W = 3840;
    const int H = 2160;
    int iterations = 10;

    // Process command line args.
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'h':
                usage(argv[0]);
                return 0;
                break;
            case 'n':
                iterations = atoi(argv[i + 1]);
                i++;
                break;
            }
        }
    }

    Conv3x3a16Descriptor conv3x3a16_pipeline(W, H);
    Dilate3x3Descriptor dilate3x3_pipeine(W, H);
    Median3x3Descriptor median3x3_pipeline(W, H);
    Gaussian5x5Descriptor gaussian5x5_pipeline(W, H);
    SobelDescriptor sobel_pipeline(W, H);
    MorphedgeDescriptor morph_edge_pipeline(W, H);
    Conv3x3a32Descriptor conv3x3a32_pipeline(W, H);

    std::vector<PipelineDescriptorBase *> pipelines = {&conv3x3a16_pipeline, &dilate3x3_pipeine, &median3x3_pipeline,
                                                       &gaussian5x5_pipeline, &sobel_pipeline, &morph_edge_pipeline, &conv3x3a32_pipeline};

    for (PipelineDescriptorBase *p : pipelines) {
        if (!p->defined()) {
            continue;
        }
        p->init();
        printf("Running %s...\n", p->name());

#ifdef HALIDE_RUNTIME_HEXAGON
        // To avoid the cost of powering HVX on in each call of the
        // pipeline, power it on once now. Also, set Hexagon performance to turbo.
        halide_hexagon_set_performance_mode(NULL, halide_hexagon_power_turbo);
        halide_hexagon_power_hvx_on(NULL);
#endif

        double time = Halide::Tools::benchmark(iterations, 10, [&]() {
            int result = p->run();
            if (result != 0) {
                printf("pipeline failed! %d\n", result);
            }
        });
        printf("Done, time (%s): %g s\n", p->name(), time);

#ifdef HALIDE_RUNTIME_HEXAGON
        // We're done with HVX, power it off, and reset the performance mode
        // to default to save power.
        halide_hexagon_power_hvx_off(NULL);
        halide_hexagon_set_performance_mode(NULL, halide_hexagon_power_default);
#endif

        if (!p->verify(W, H)) {
            TestReport tr(p->name(), time * 1000000, "microseconds", Mode::Unknown_Mode, Result::Fail);
            tr.print();
            printf("Failed\n");
            abort();
        } else {
            TestReport tr(p->name(), time * 1000000, "microseconds", Mode::Unknown_Mode, Result::Pass);
            tr.print();
            printf("%s passed\n", p->name());
        }
        p->finalize();
    }

    printf("Success!\n");
    return 0;
}

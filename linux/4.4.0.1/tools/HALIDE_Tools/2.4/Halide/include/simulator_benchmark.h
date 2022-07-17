/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#ifndef SIMULATOR_BENCHMARK_H
#define SIMULATOR_BENCHMARK_H
#include "io.h"
template<typename F>
long long benchmark(F op) {
    RESET_PMU();
    long long start_time = READ_PCYCLES();

    op();

    long long total_cycles = READ_PCYCLES() - start_time;
    DUMP_PMU();
    return total_cycles;
}
#endif

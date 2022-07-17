/**=============================================================================
@file
    qhblas_test.h

@brief
    Macros and datatypes used in QHBLAS tests.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHBLAS_TEST_H
#define _QHBLAS_TEST_H

#include <string.h>
#if __hexagon__
#include <hexagon_cache.h>
#endif
#include "qhblas.h"
#include "qhblas_internal.h"
#include "qhl_common.h"

typedef struct test_invalid_f2
{
    float_a8_t *input_arr1;    // Pointer to first input array
    float_a8_t *input_arr2;    // Pointer to second input array
    float_a8_t *output;        // Pointer to output array
    uint32_t length;           // Length of input arrays
    int32_t expect_res;        // Expected return value
} test_invalid_f2;

typedef struct test_invalid_f1
{
    float_a8_t *input_arr;     // Pointer to input array
    float *output;             // Pointer to output array
    uint32_t length;           // Length of input arrays
    int32_t expect_res;        // Expected return value
} test_invalid_f1;

typedef struct test_invalid_fscale
{
    float_a8_t *input_arr;     // Pointer to input array
    float scaling_factor;      // scaling factor
    float_a8_t *output;        // Pointer to output array
    uint32_t length;           // Length of input arrays
    int32_t expect_res;        // Expected return value
} test_invalid_fscale;

typedef struct test_invalid_h2
{
    int16_a8_t *input_arr1;     // Pointer to first input array
    int16_a8_t *input_arr2;     // Pointer to second input array
    int16_a8_t *output;         // Pointer to output array
    uint32_t length;            // Length of input arrays
    int16_a8_t expect_res;      // Expected return value
} test_invalid_h2;

typedef struct test_invalid_w2
{
    int32_a8_t *input_arr1;     // Pointer to first input array
    int32_a8_t *input_arr2;     // Pointer to second input array
    int32_a8_t *output;         // Pointer to output array
    uint32_t length;            // Length of input arrays
    int32_a8_t expect_res;      // Expected return value
} test_invalid_w2;

typedef struct test_invalid_h1
{
    int16_a8_t *input_arr;     // Pointer to input array
    int16_t *output;           // Pointer to output array
    uint32_t length;           // Length of input arrays
    int32_t expect_res;        // Expected return value
} test_invalid_h1;

typedef struct test_invalid_hnorm
{
    int16_a8_t *input_arr;     // Pointer to input array
    int32_t *output;             // Pointer to output array
    uint32_t length;           // Length of input arrays
    int32_t expect_res;        // Expected return value
} test_invalid_hnorm;

typedef struct test_invalid_hdot
{
    int16_a8_t *input_arr1;     // Pointer to first input array
    int16_a8_t *input_arr2;     // Pointer to second input array
    int64_t *output;            // Pointer to output
    uint32_t length;            // Length of input arrays
    int16_a8_t expect_res;      // Expected return value
} test_invalid_hdot;

typedef struct test_invalid_hscale
{
    int16_a8_t *input_arr;      // Pointer to first input array
    int16_t scaling_factor;     // scaling factor
    int16_a8_t *output;         // Pointer to output array
    uint32_t length;            // Length of input arrays
    int16_a8_t expect_res;      // Expected return value
} test_invalid_hscale;

typedef struct test_invalid_cf2
{
    cfloat_a8_t *input_arr1;    // Pointer to first input array
    cfloat_a8_t *input_arr2;    // Pointer to second input array
    cfloat_a8_t *output;        // Pointer to output array
    uint32_t length;            // Length of input arrays
    int32_t expect_res;         // Expected return value
} test_invalid_cf2;

typedef struct test_invalid_w
{
    int32_t *input_arr;         // Pointer to first input array
    int32_t *output;            // Pointer to output array
    uint32_t length;            // Length of input arrays
    int32_t expect_res;      // Expected return value
} test_invalid_w;

typedef struct test_invalid_hw
{
    int16_t *input_arr;         // Pointer to first input array
    int32_t *output;            // Pointer to output array
    uint32_t length;            // Length of input arrays
    int32_t expect_res;      // Expected return value
} test_invalid_hw;

typedef struct test_invalid_bh
{
    int8_t *input_arr;         // Pointer to first input array
    int16_t *output;            // Pointer to output array
    uint32_t length;            // Length of input arrays
    int32_t expect_res;      // Expected return value
} test_invalid_bh;

typedef struct test_invalid_f
{
    float *input_arr;         // Pointer to first input array
    float *output;            // Pointer to output array
    uint32_t length;            // Length of input arrays
    int32_t expect_res;      // Expected return value
} test_invalid_f;

typedef struct test_invalid_f16
{
    __fp16 *input_arr;         // Pointer to first input array
    __fp16 *output;            // Pointer to output array
    uint32_t length;            // Length of input arrays
    int32_t expect_res;      // Expected return value
} test_invalid_f16;

/* Macro to initialize float complex input array. */
#define INIT_INPUT_CFLOAT_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, START_INDEX)                                      \
    do                                                                                                                       \
    {                                                                                                                        \
        for (uint32_t i = START_INDEX; i < ARR_LEN; ++i)                                                                     \
        {                                                                                                                    \
            uint32_t range_index = i % RANGE_CNT;                                                                            \
            float lo = RANGES[range_index].lo;                                                                               \
            float hi = RANGES[range_index].hi;                                                                               \
                                                                                                                             \
            float real = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);                                                 \
            float imag = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);                                                 \
            ARR[i] = _F_COMPLEX_(real, imag);                                                                                \
        }                                                                                                                    \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to initialize float complex input array from the beginning. */
#define INIT_INPUT_CFLOAT(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX)                                                         \
    INIT_INPUT_CFLOAT_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, 0)                                                    \

/* Macro to initialize float input array. */
#define INIT_INPUT_FLOAT_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, START_INDEX)                                       \
    do                                                                                                                       \
    {                                                                                                                        \
        for (uint32_t i = START_INDEX; i < ARR_LEN; ++i)                                                                     \
        {                                                                                                                    \
            uint32_t range_index = i % RANGE_CNT;                                                                            \
            float lo = RANGES[range_index].lo;                                                                               \
            float hi = RANGES[range_index].hi;                                                                               \
                                                                                                                             \
            ARR[i] = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);                                                     \
        }                                                                                                                    \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to initialize float input array from the beginning. */
#define INIT_INPUT_FLOAT(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX)                                                          \
    INIT_INPUT_FLOAT_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, 0)                                                     \

/* Macro to initialize float16 input array. */
#define INIT_INPUT_FLOAT16_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, START_INDEX)                                     \
    do                                                                                                                       \
    {                                                                                                                        \
        for (uint32_t i = START_INDEX; i < ARR_LEN; ++i)                                                                     \
        {                                                                                                                    \
            uint32_t range_index = i % RANGE_CNT;                                                                            \
            float lo = RANGES[range_index].lo;                                                                               \
            float hi = RANGES[range_index].hi;                                                                               \
                                                                                                                             \
            ARR[i] = (__fp16)(lo + (hi - lo) * ((float)rand() / (float)RAND_MAX));                                           \
        }                                                                                                                    \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to initialize float input array from the beginning. */
#define INIT_INPUT_FLOAT16(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX)                                                        \
    INIT_INPUT_FLOAT16_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, 0)

/* Macro to initialize int8_t input array. */
#define INIT_INPUT_INT8_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, START_INDEX)                                        \
    do                                                                                                                       \
    {                                                                                                                        \
        for (uint32_t i = START_INDEX; i < ARR_LEN; ++i)                                                                     \
        {                                                                                                                    \
            uint32_t range_index = i % RANGE_CNT;                                                                            \
            int8_t lo = RANGES[range_index].lo;                                                                             \
            int8_t hi = RANGES[range_index].hi;                                                                             \
                                                                                                                             \
            ARR[i] = lo + ((int8_t) (((float) (hi - lo)) * (((float)rand() / (float)RAND_MAX))));                                  \
        }                                                                                                                    \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to initialize int8_t input array from the beginning. */
#define INIT_INPUT_INT8(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX)                                                          \
    INIT_INPUT_INT8_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, 0)

/* Macro to initialize int16_t input array. */
#define INIT_INPUT_INT16_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, START_INDEX)                                       \
    do                                                                                                                       \
    {                                                                                                                        \
        for (uint32_t i = START_INDEX; i < ARR_LEN; ++i)                                                                     \
        {                                                                                                                    \
            uint32_t range_index = i % RANGE_CNT;                                                                            \
            int16_t lo = RANGES[range_index].lo;                                                                             \
            int16_t hi = RANGES[range_index].hi;                                                                             \
                                                                                                                             \
            ARR[i] = lo + ((int16_t) (((float) (hi - lo)) * (((float)rand() / (float)RAND_MAX))));                           \
        }                                                                                                                    \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to initialize int16_t input array from the beginning. */
#define INIT_INPUT_INT16(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX)                                                          \
    INIT_INPUT_INT16_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, 0)                                                     \

/* Macro to initialize int32_t input array. */
#define INIT_INPUT_INT32_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, START_INDEX)                                       \
    do                                                                                                                       \
    {                                                                                                                        \
        for (uint32_t i = START_INDEX; i < ARR_LEN; ++i)                                                                     \
        {                                                                                                                    \
            uint32_t range_index = i % RANGE_CNT;                                                                            \
            int32_t lo = RANGES[range_index].lo;                                                                             \
            int32_t hi = RANGES[range_index].hi;                                                                             \
                                                                                                                             \
            ARR[i] = lo + ((int32_t) (((float) (hi - lo)) * (((float)rand() / (float)RAND_MAX))));                           \
        }                                                                                                                    \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to initialize int32_t input array from the beginning. */
#define INIT_INPUT_INT32(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX)                                                          \
    INIT_INPUT_INT32_INDEX(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX, 0)

/* Macro to call a benchmark function. */
#define BENCH_TCALL(FNAME, NR_OF_ITERATIONS, LENGTHS_OF_ARRAYS, VEC_LEN)                                                     \
    test_##FNAME(NR_OF_ITERATIONS, LENGTHS_OF_ARRAYS, VEC_LEN);                                                              \

/* Macro to create a benchmark for QHBLAS functions. */
#define BENCH_TFUN(FNAME, PARAMS...)                                                                                         \
    void test_##FNAME(uint64_t n_iter, uint32_t* lengths_of_arrays, uint32_t vec_len)                                        \
    {                                                                                                                        \
        performance_high();                                                                                                  \
        printf("Benchmark for %s function:\n", #FNAME);                                                                      \
                                                                                                                             \
        double cpc_sum = 0.0;                                                                                                \
        double cpm_sum = 0.0;                                                                                                \
        double uspc_sum = 0.0;                                                                                               \
        double uspm_sum = 0.0;                                                                                               \
        uint64_t length_sum = 0;                                                                                             \
        for (uint32_t i = 0; i < vec_len; ++i)                                                                               \
        {                                                                                                                    \
            double cpc = 0.0;                                                                                                \
            double cpm = 0.0;                                                                                                \
            double uspc = 0.0;                                                                                               \
            double uspm = 0.0;                                                                                               \
            uint32_t length = lengths_of_arrays[i];                                                                          \
            for (uint32_t j = 0; j < n_iter; ++j)                                                                            \
            {                                                                                                                \
                clear_cache(length);                                                                                         \
                measurement_start();                                                                                         \
                FNAME(PARAMS, length);                                                                                       \
                measurement_finish();                                                                                        \
                cpc += measurement_get_cycles();                                                                             \
                uspc += measurement_get_us();                                                                                \
            }                                                                                                                \
                                                                                                                             \
            cpc /= (double) n_iter;                                                                                          \
            cpm = cpc / (double) length;                                                                                     \
            uspc /= (double)n_iter;                                                                                          \
            uspm = uspc / (double)length;                                                                                    \
            length_sum += length;                                                                                            \
            cpc_sum += cpc;                                                                                                  \
            uspc_sum += uspc;                                                                                                \
            printf("  %11.4f CPC %11.4f MPS |  %11.4f CPM  %11.4f MPSpm | vector length = %lu\n",                            \
                   cpc, 1.0 / uspc, cpm, 1.0 / uspm, length);                                                                \
        }                                                                                                                    \
                                                                                                                             \
        cpm_sum = cpc_sum / (double) length_sum;                                                                             \
        uspm_sum = uspc_sum / (double)length_sum;                                                                            \
        printf("\nSummary:\n");                                                                                              \
        printf("  %11.4f CPC %11.4f MPS | %11.4f CPM %11.4f MPSPM | vector length = %llu\n",                                 \
               cpc_sum, 1.0 / uspc_sum, cpm_sum, 1.0 / uspm_sum, length_sum);                                                \
        performance_normal();                                                                                                \
    }                                                                                                                        \



#define BENCH_TFUN_CLIPPING(FNAME, INPUT, OUTPUT, PARAMS...)                                                                 \
    void test_##FNAME(uint64_t n_iter, uint32_t* lengths_of_arrays, uint32_t vec_len)                                        \
    {                                                                                                                        \
        performance_high();                                                                                                  \
        printf("Benchmark for %s function:\n", #FNAME);                                                                      \
                                                                                                                             \
        double cpc_sum = 0.0;                                                                                                \
        double cpm_sum = 0.0;                                                                                                \
        double uspc_sum = 0.0;                                                                                               \
        double uspm_sum = 0.0;                                                                                               \
        uint64_t length_sum = 0;                                                                                             \
        for (uint32_t i = 0; i < vec_len; ++i)                                                                               \
        {                                                                                                                    \
            double cpc = 0.0;                                                                                                \
            double cpm = 0.0;                                                                                                \
            double uspc = 0.0;                                                                                               \
            double uspm = 0.0;                                                                                               \
            uint32_t length = lengths_of_arrays[i];                                                                          \
            for (uint32_t j = 0; j < n_iter; ++j)                                                                            \
            {                                                                                                                \
                clear_cache(length);                                                                                         \
                measurement_start();                                                                                         \
                FNAME(INPUT, OUTPUT, length, PARAMS);                                                                        \
                measurement_finish();                                                                                        \
                cpc += measurement_get_cycles();                                                                             \
                uspc += measurement_get_us();                                                                                \
            }                                                                                                                \
                                                                                                                             \
            cpc /= (double) n_iter;                                                                                          \
            cpm = cpc / (double) length;                                                                                     \
            uspc /= (double)n_iter;                                                                                          \
            uspm = uspc / (double)length;                                                                                    \
            length_sum += length;                                                                                            \
            cpc_sum += cpc;                                                                                                  \
            uspc_sum += uspc;                                                                                                \
            printf("  %11.4f CPC %11.4f MPS |  %11.4f CPM  %11.4f MPSpm | vector length = %lu\n",                            \
                   cpc, 1.0 / uspc, cpm, 1.0 / uspm, length);                                                                \
        }                                                                                                                    \
                                                                                                                             \
        cpm_sum = cpc_sum / (double) length_sum;                                                                             \
        uspm_sum = uspc_sum / (double)length_sum;                                                                            \
        printf("\nSummary:\n");                                                                                              \
        printf("  %11.4f CPC %11.4f MPS | %11.4f CPM %11.4f MPSPM | vector length = %llu\n",                                 \
               cpc_sum, 1.0 / uspc_sum, cpm_sum, 1.0 / uspm_sum, length_sum);                                                \
        performance_normal();                                                                                                \
    }                                                                                                                        \


/* Macro to create a benchmark for QHBLAS functions. */
#define BENCH_TFUN_HVX_MATRIX(FNAME, PARAMS...)                                                                                         \
    void test_##FNAME(uint64_t n_iter, uint32_t* lengths_of_arrays, uint32_t vec_len)                                        \
    {                                                                                                                        \
        performance_high();                                                                                                  \
        printf("Benchmark for %s function:\n", #FNAME);                                                                      \
                                                                                                                             \
        double cpc_sum = 0.0;                                                                                                \
        double cpm_sum = 0.0;                                                                                                \
        double uspc_sum = 0.0;                                                                                               \
        double uspm_sum = 0.0;                                                                                               \
        uint64_t length_sum = 0;                                                                                             \
        for (uint32_t i = 1; i < vec_len; ++i)                                                                               \
        {                                                                                                                    \
            double cpc = 0.0;                                                                                                \
            double cpm = 0.0;                                                                                                \
            double uspc = 0.0;                                                                                               \
            double uspm = 0.0;                                                                                               \
            uint32_t length = lengths_of_arrays[i] * lengths_of_arrays[i-1];                                                                          \
            for (uint32_t j = 0; j < n_iter; ++j)                                                                            \
            {                                                                                                                \
                clear_cache(length);                                                                                         \
                measurement_start();                                                                                         \
                FNAME(PARAMS, lengths_of_arrays[i], lengths_of_arrays[i-1]);                                                                                       \
                measurement_finish();                                                                                        \
                cpc += measurement_get_cycles();                                                                             \
                uspc += measurement_get_us();                                                                                \
            }                                                                                                                \
                                                                                                                             \
            cpc /= (double) n_iter;                                                                                          \
            cpm = cpc / (double) length;                                                                                     \
            uspc /= (double)n_iter;                                                                                          \
            uspm = uspc / (double)length;                                                                                    \
            length_sum += length;                                                                                            \
            cpc_sum += cpc;                                                                                                  \
            uspc_sum += uspc;                                                                                                \
            printf("  %11.4f CPC %11.4f MPS |  %11.4f CPM  %11.4f MPSpm | vector length = %lu | matrix size %lu x %lu\n",                             \
                   cpc, 1.0 / uspc, cpm, 1.0 / uspm, length, lengths_of_arrays[i], lengths_of_arrays[i-1]);                                                                \
        }                                                                                                                    \
                                                                                                                             \
        cpm_sum = cpc_sum / (double) length_sum;                                                                             \
        uspm_sum = uspc_sum / (double)length_sum;                                                                            \
        printf("\nSummary:\n");                                                                                              \
        printf("  %11.4f CPC %11.4f MPS | %11.4f CPM %11.4f MPSPM | vector length = %llu\n",                                   \
               cpc_sum, 1.0 / uspc_sum, cpm_sum, 1.0 / uspm_sum, length_sum);                                                \
        performance_normal();                                                                                                \
    }                                                                                                                        \

/* Macro to initialize lengths of vector. */
#define BENCH_VECTOR_INIT_INPUT(ARR, ARR_LEN, RANGES, RANGE_CNT, RAND_MAX)                                                   \
    do                                                                                                                       \
    {                                                                                                                        \
        for (uint32_t i = 0; i < ARR_LEN; ++i)                                                                               \
        {                                                                                                                    \
            uint32_t range_index = i % RANGE_CNT;                                                                            \
            uint32_t lo = RANGES[range_index].lo;                                                                            \
            uint32_t hi = RANGES[range_index].hi;                                                                            \
                                                                                                                             \
            ARR[i] = lo + ((uint32_t) (((float) (hi - lo)) * (ABSF((float)rand() / (float)RAND_MAX))));                      \
        }                                                                                                                    \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to initalize input array with special values. */
#define TEST_INIT_SPEC_VAL(ARR, SPEC_ARR, SPEC_NUM)                                                                          \
    do                                                                                                                       \
    {                                                                                                                        \
        for (uint32_t i = 0; i < SPEC_NUM; ++i)                                                                              \
        {                                                                                                                    \
            ARR[i] = SPEC_ARR[i];                                                                                            \
        }                                                                                                                    \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test invalid inputs of QHBLAS functions with two input arrays. */
#define TEST_INVALID_INPUT_A2(FNAME)                                                                                         \
    do                                                                                                                       \
    {                                                                                                                        \
        printf("Test for %s function:\n", #FNAME);                                                                           \
        int test_failed = 0;                                                                                                 \
        for (uint32_t i = 0; i < NUM_TEST_TYPE; ++i)                                                                         \
        {                                                                                                                    \
            int32_t res = FNAME(test_data[i].input_arr1, test_data[i].input_arr2,                                            \
                                test_data[i].output, test_data[i].length);                                                   \
                                                                                                                             \
            if (test_data[i].expect_res != res)                                                                              \
            {                                                                                                                \
                test_failed = 1;                                                                                             \
                printf("Error:\t got = %ld | OK = %d | iter = %lu\n",                                                        \
                       res, (int) test_data[i].expect_res, i);                                                               \
            }                                                                                                                \
        }                                                                                                                    \
        if (test_failed)                                                                                                     \
            return 1;                                                                                                        \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test invalid inputs of QHBLAS functions with one input array. */
#define TEST_INVALID_INPUT_A1(FNAME)                                                                                         \
    do                                                                                                                       \
    {                                                                                                                        \
        printf("Test for %s function:\n", #FNAME);                                                                           \
        int test_failed = 0;                                                                                                 \
        for (uint32_t i = 0; i < NUM_TEST_TYPE; ++i)                                                                         \
        {                                                                                                                    \
            int32_t res = FNAME(test_data[i].input_arr, test_data[i].output,                                                 \
                                test_data[i].length);                                                                        \
                                                                                                                             \
            if (test_data[i].expect_res != res)                                                                              \
            {                                                                                                                \
                test_failed = 1;                                                                                             \
                printf("Error:\t got = %lu | OK = %lu | iter = %lu\n",                                                          \
                       res, test_data[i].expect_res, i);                                                                     \
            }                                                                                                                \
        }                                                                                                                    \
        if (test_failed)                                                                                                     \
            return 1;                                                                                                        \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test invalid inputs of scale QHBLAS functions. */
#define TEST_INVALID_INPUT_SCALE(FNAME)                                                                                      \
    do                                                                                                                       \
    {                                                                                                                        \
        printf("Test for %s function:\n", #FNAME);                                                                           \
        int test_failed = 0;                                                                                                 \
        for (uint32_t i = 0; i < NUM_TEST_TYPE; ++i)                                                                         \
        {                                                                                                                    \
            int32_t res = FNAME(test_data[i].input_arr, test_data[i].scaling_factor,                                         \
                                test_data[i].output, test_data[i].length);                                                   \
                                                                                                                             \
            if (test_data[i].expect_res != res)                                                                              \
            {                                                                                                                \
                test_failed = 1;                                                                                             \
                printf("Error:\t got = %ld | OK = %ld | iter = %ld\n",                                                       \
                       res, test_data[i].expect_res, i);                                                                     \
            }                                                                                                                \
        }                                                                                                                    \
        if (test_failed)                                                                                                     \
            return 1;                                                                                                        \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test a QHBLAS complex float functions that return scalar. */
#define TEST_INPUT_SCALAR_CFLOAT(FNAME, ARR_LEN, OUTPUT, PARAMS...)                                                          \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            float complex ref_output = generate_ref_output(i);                                                               \
            FNAME(PARAMS, &OUTPUT, i);                                                                                       \
                                                                                                                             \
            if (!isequal(qhcomplex_creal_f(ref_output), qhcomplex_creal_f(OUTPUT))                                           \
                || !isequal(qhcomplex_cimag_f(ref_output), qhcomplex_cimag_f(OUTPUT)))                                       \
            {                                                                                                                \
                printf("Error:\t got = %.7f + %.7f*i | OK = %.7f + %.7f*i | vector length = %lu\n",                          \
                       qhcomplex_creal_f(OUTPUT), qhcomplex_cimag_f(OUTPUT),                                                 \
                       qhcomplex_creal_f(ref_output), qhcomplex_cimag_f(ref_output) , i);                                    \
                ++fail;                                                                                                      \
            }                                                                                                                \
            else                                                                                                             \
            {                                                                                                                \
                ++pass;                                                                                                      \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test a QHBLAS float functions that return scalar. */
#define TEST_INPUT_SCALAR_FLOAT(FNAME, ARR_LEN, OUTPUT, PARAMS...)                                                          \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            float ref_output = generate_ref_output(i);                                                                       \
            FNAME(PARAMS, &OUTPUT, i);                                                                                       \
                                                                                                                             \
            if (!isequal(ref_output, OUTPUT))                                                                                \
            {                                                                                                                \
                printf("Error:\t got = %.7f | OK = %.7f | vector length = %lu\n",                                            \
                       OUTPUT, ref_output, i);                                                                               \
                ++fail;                                                                                                      \
            }                                                                                                                \
            else                                                                                                             \
            {                                                                                                                \
                ++pass;                                                                                                      \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test a QHBLAS int16 functions that return scalar. */
#define TEST_INPUT_SCALAR_INT16(FNAME, ARR_LEN, OUTPUT, PARAMS...)                                                           \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            __typeof__(OUTPUT) ref_output = generate_ref_output(i);                                                          \
            FNAME(PARAMS, &OUTPUT, i);                                                                                       \
                                                                                                                             \
            if (ref_output != OUTPUT)                                                                                        \
            {                                                                                                                \
                printf("Error:\t got = %lld | OK = %lld | vector length = %lu\n",                                            \
                       (int64_t) OUTPUT, (int64_t) ref_output, i);                                                           \
                ++fail;                                                                                                      \
            }                                                                                                                \
            else                                                                                                             \
            {                                                                                                                \
                ++pass;                                                                                                      \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test QHBLAS functions with two int16_t input arrays. */
#define TEST_INPUT_AH2(FNAME)                                                                                                \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(__typeof__(*output))*ARR_LEN);                                                          \
            FNAME(input_arr1, input_arr2, output, i);                                                                        \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                if (ref_output[j] != output[j])                                                                              \
                {                                                                                                            \
                    printf("Error:\tx = {%.7ld, %.7ld} | %s(x) = %.7ld | OK = %.7ld "                                        \
                           "| vector length = %lu | i = %lu\n",                                                              \
                           (long) input_arr1[j], (long) input_arr2[j], #FNAME, (long) output[j],                             \
                           (long) ref_output[j], i, j);                                                                      \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);

/* Macro to test QHBLAS functions with one int32_t input array. */
#define TEST_INPUT_AW(FNAME)                                                                                                 \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(int32_t)*ARR_LEN);                                                                      \
            FNAME(input_arr, output, i);                                                                                     \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                if (ref_output[j] != output[j])                                                                              \
                {                                                                                                            \
                    printf("Error:\tx = {%.7ld} | %s(x) = %.7ld | OK = %.7ld "                                               \
                           "| vector length = %lu | i = %lu\n",                                                              \
                           (long) input_arr[j], #FNAME, (long) output[j],                                                    \
                           (long) ref_output[j], i, j);                                                                      \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test a QHBLAS qfloat32 functions that return scalar. */
#define TEST_INPUT_SCALAR_QFLOAT32(FNAME, ARR_LEN, OUTPUT, TOLERANCE, PARAMS...)                                             \
    do                                                                                                                       \
    {                                                                                                                        \
        float error;                                                                                                         \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            float ref_output = generate_ref_output(i);                                                                       \
            FNAME(PARAMS, &OUTPUT, i);                                                                                       \
                                                                                                                             \
            error = ref_output - (float)OUTPUT;                                                                              \
            if (error < 0) error = -error;                                                                                   \
            if (error > TOLERANCE)                                                                                           \
            {                                                                                                                \
                printf("Error:\t got = %.7f | OK = %.7f | vector length = %lu\n",                                            \
                       OUTPUT, ref_output, i);                                                                               \
                ++fail;                                                                                                      \
            }                                                                                                                \
            else                                                                                                             \
            {                                                                                                                \
                ++pass;                                                                                                      \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test a QHBLAS qfloat16 functions that return scalar. */
#define TEST_INPUT_SCALAR_QFLOAT16(FNAME, ARR_LEN, OUTPUT, TOLERANCE, PARAMS...)                                             \
    do                                                                                                                       \
    {                                                                                                                        \
        float error;                                                                                                         \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            float ref_output = generate_ref_output(i);                                                                       \
            FNAME(PARAMS, &OUTPUT, i);                                                                                       \
                                                                                                                             \
            error = ref_output - (float)OUTPUT;                                                                              \
            if (error < 0) error = -error;                                                                                   \
            if (error > TOLERANCE)                                                                                           \
            {                                                                                                                \
                printf("Error:\t got = %.7f | OK = %.7f | vector length = %lu\n",                                            \
                       OUTPUT, ref_output, i);                                                                               \
                ++fail;                                                                                                      \
            }                                                                                                                \
            else                                                                                                             \
            {                                                                                                                \
                ++pass;                                                                                                      \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test QHBLAS functions with two qfloat16 input arrays. */
#define TEST_INPUT_QFLOAT16_A2_T(FNAME, TOLERANCE)                                                                           \
    do                                                                                                                       \
    {                                                                                                                        \
        float error;                                                                                                         \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(__typeof__(*output))*ARR_LEN);;                                                         \
            FNAME(input_arr1, input_arr2, output, i);                                                                        \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                error = ref_output[j] - (float)output[j];                                                                    \
                if (error < 0) error = -error;                                                                               \
                if (error > TOLERANCE)                                                                                       \
                {                                                                                                            \
                    printf("Error:\tx = {%.7f, %.7f} | %s(x) = %.7f | OK = %.7f "                                            \
                           "| vector length = %lu | i = %lu | tolerance = %f\n",                                             \
                           input_arr1[j], input_arr2[j], #FNAME, output[j],                                                  \
                           ref_output[j], i, j, TOLERANCE);                                                                  \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test QHBLAS functions with two qfloat input arrays. */
#define TEST_INPUT_QFLOAT32_A2_T(FNAME, TOLERANCE)                                                                           \
    do                                                                                                                       \
    {                                                                                                                        \
        float error;                                                                                                         \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(__typeof__(*output))*ARR_LEN);;                                                         \
            FNAME(input_arr1, input_arr2, output, i);                                                                        \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                error = ref_output[j] - output[j];                                                                           \
                if (error < 0) error = -error;                                                                               \
                if (error > TOLERANCE)                                                                                       \
                {                                                                                                            \
                    printf("Error:\tx = {%.7f, %.7f} | %s(x) = %.7f | OK = %.7f "                                            \
                           "| vector length = %lu | i = %lu | tolerance = %f\n",                                             \
                           input_arr1[j], input_arr2[j], #FNAME, output[j],                                                  \
                           ref_output[j], i, j, TOLERANCE);                                                                  \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

    /* Macro to test QHBLAS functions with one int32_t input array. */
    #define TEST_INPUT_AF16_T(FNAME, TOLERANCE)                                                                              \
        do                                                                                                                   \
        {                                                                                                                    \
            float error;                                                                                                     \
            uint32_t pass = 0;                                                                                               \
            uint32_t fail = 0;                                                                                               \
            generate_ref_output(ARR_LEN);                                                                                    \
                                                                                                                             \
            for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                          \
            {                                                                                                                \
                memset(output, 0, sizeof(__fp16)*ARR_LEN);                                                                   \
                FNAME(input_arr, output, i);                                                                                 \
                                                                                                                             \
                for (uint32_t j = 0; j < i; ++j)                                                                             \
                {                                                                                                            \
                    error = ref_output[j] - output[j];                                                                       \
                    if (error < 0) error = -error;                                                                           \
                    if ((error) > TOLERANCE )                                                                                \
                    {                                                                                                        \
                        printf("Error:\tx = {%.7f} | %s(x) = %.7f | OK = %.7f "                                              \
                               "| vector length = %lu | i = %lu | Tol = %f\n",                                               \
                               input_arr[j], #FNAME, output[j],                                                              \
                               ref_output[j], i, j, TOLERANCE);                                                              \
                        ++fail;                                                                                              \
                    }                                                                                                        \
                    else                                                                                                     \
                    {                                                                                                        \
                        ++pass;                                                                                              \
                    }                                                                                                        \
                }                                                                                                            \
            }                                                                                                                \
                                                                                                                             \
            printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                \
            printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                          \
        }                                                                                                                    \
        while(0);

/* Macro to test QHBLAS functions with one int32_t input array. */
#define TEST_INPUT_AH(FNAME)                                                                                                 \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(int16_t)*ARR_LEN);                                                                      \
            FNAME(input_arr, output, i);                                                                                     \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                if (ref_output[j] != output[j])                                                                              \
                {                                                                                                            \
                    printf("Error:\tx = {%.7d} | %s(x) = %.7d | OK = %.7ld "                                                 \
                           "| vector length = %lu | i = %lu\n",                                                              \
                           input_arr[j], #FNAME, output[j],                                                                  \
                           (long) ref_output[j], i, j);                                                                      \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);

    /* Macro to test QHBLAS functions with one float input array. */
    #define TEST_INPUT_AF_T(FNAME, TOLERANCE)                                                                                    \
        do                                                                                                                       \
        {                                                                                                                        \
            float error;                                                                                                         \
            uint32_t pass = 0;                                                                                                   \
            uint32_t fail = 0;                                                                                                   \
            generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                                 \
            for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
            {                                                                                                                    \
                memset(output, 0, sizeof(float)*ARR_LEN);                                                                        \
                FNAME(input_arr, output, i);                                                                                     \
                                                                                                                                 \
                for (uint32_t j = 0; j < i; ++j)                                                                                 \
                {                                                                                                                \
                    error = ref_output[j] - output[j];                                                                           \
                    if (error < 0) error = -error;                                                                               \
                    if ((error)> TOLERANCE )                                                                                     \
                    {                                                                                                            \
                        printf("Error:\tx = {%.7f} | %s(x) = %.7f | OK = %.7f "                                                  \
                               "| vector length = %lu | i = %lu | Tol = %f\n",                                                   \
                               input_arr[j], #FNAME, output[j],                                                                  \
                               ref_output[j], i, j, TOLERANCE);                                                                  \
                        ++fail;                                                                                                  \
                    }                                                                                                            \
                    else                                                                                                         \
                    {                                                                                                            \
                        ++pass;                                                                                                  \
                    }                                                                                                            \
                }                                                                                                                \
            }                                                                                                                    \
                                                                                                                                 \
            printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
            printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
        }                                                                                                                        \
        while(0);

/* Macro to test QHBLAS functions with two float input arrays. */
#define TEST_INPUT_AF2(FNAME)                                                                                                \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(float)*ARR_LEN);                                                                        \
            FNAME(input_arr1, input_arr2, output, i);                                                                        \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                if (!isequal(ref_output[j], output[j]))                                                                      \
                {                                                                                                            \
                    printf("Error:\tx = {%.7f, %.7f} | %s(x) = %.7f | OK = %.7f "                                            \
                           "| vector length = %lu | i = %lu\n",                                                              \
                           input_arr1[j], input_arr2[j], #FNAME, output[j],                                                  \
                           ref_output[j], i, j);                                                                             \
                                                                                                                             \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test QHBLAS functions with two complex float input arrays. */
#define TEST_INPUT_ACF2(FNAME)                                                                                               \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(complex float)*ARR_LEN);                                                                \
            FNAME(input_arr1, input_arr2, output, i);                                                                        \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                float complex cref = ref_output[j];                                                                          \
                float complex cout = output[j];                                                                              \
                                                                                                                             \
                if (!isequal(qhcomplex_creal_f(cref), qhcomplex_creal_f(cout))                                               \
                    || !isequal(qhcomplex_cimag_f(cref), qhcomplex_cimag_f(cout)))                                           \
                {                                                                                                            \
                    float complex in1 = input_arr1[j];                                                                       \
                    float complex in2 = input_arr2[j];                                                                       \
                                                                                                                             \
                    printf("Error:\tin1 = %.7f + %.7f*i | in2 = %.7f + %.7f*i | "                                            \
                           "%s = %.7f + %.7f*i | OK = %.7f + %.7f*i | "                                                      \
                           "vector length = %u, | i = %u\n",                                                                 \
                           qhcomplex_creal_f(in1), qhcomplex_cimag_f(in1),                                                   \
                           qhcomplex_creal_f(in2), qhcomplex_cimag_f(in2), #FNAME,                                           \
                           qhcomplex_creal_f(cout), qhcomplex_cimag_f(cout),                                                 \
                           qhcomplex_creal_f(cref), qhcomplex_cimag_f(cref), i, j);                                          \
                                                                                                                             \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test QHBLAS scaling float function. */
#define TEST_INPUT_SCALE_FLOAT(FNAME)                                                                                        \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(float)*ARR_LEN);                                                                        \
            FNAME(input_arr, scale_factor, output, i);                                                                       \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                if (!isequal(ref_output[j], output[j]))                                                                      \
                {                                                                                                            \
                    printf("Error:\tx = {%.7f, %.7f} | %s(x) = %.7f | OK = %.7f "                                            \
                           "| vector length = %lu | i = %lu\n",                                                              \
                           input_arr[j], scale_factor, #FNAME, output[j],                                                    \
                           ref_output[j], i, j);                                                                             \
                                                                                                                             \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test QHBLAS scaling int16 function. */
#define TEST_INPUT_SCALE_INT16(FNAME)                                                                                        \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(__typeof__(*output))*ARR_LEN);                                                          \
            FNAME(input_arr, scale_factor, output, i);                                                                       \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                if (ref_output[j] != output[j])                                                                              \
                {                                                                                                            \
                    printf("Error:\tx = {%.7ld, %.7ld} | %s(x) = %.7ld | OK = %.7ld "                                        \
                           "| vector length = %lu | i = %lu\n",                                                              \
                           (long) input_arr[j], (long) scale_factor, #FNAME, (long) output[j],                               \
                           (long) ref_output[j], i, j);                                                                      \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test QHBLAS scaling qfloat16 function. */
#define TEST_INPUT_SCALE_QFLOAT16_T(FNAME, TOLERANCE)                                                                        \
    do                                                                                                                       \
    {                                                                                                                        \
        float error;                                                                                                         \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(__typeof__(*output))*ARR_LEN);                                                          \
            FNAME(input_arr, &scale_factor, output, i);                                                                      \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                error = ref_output[j] - (float)output[j];                                                                    \
                if (error < 0) error = -error;                                                                               \
                if (error > TOLERANCE)                                                                                       \
                {                                                                                                            \
                    printf("Error:\tx = {%.7f, %.7f} | %s(x) = %.7f | OK = %.7f "                                            \
                           "| vector length = %lu | i = %lu | tolerance = %f\n",                                             \
                           input_arr[j], scale_factor, #FNAME, output[j],                                                    \
                           ref_output[j], i, j, TOLERANCE);                                                                  \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro to test QHBLAS scaling qfloat32 function. */
#define TEST_INPUT_SCALE_QFLOAT32_T(FNAME, TOLERANCE)                                                                        \
    do                                                                                                                       \
    {                                                                                                                        \
        float error;                                                                                                         \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(__typeof__(*output))*ARR_LEN);                                                          \
            FNAME(input_arr, scale_factor, output, i);                                                                       \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                error = ref_output[j] - output[j];                                                                           \
                if (error < 0) error = -error;                                                                               \
                if (error > TOLERANCE)                                                                                       \
                {                                                                                                            \
                    printf("Error:\tx = {%.7f, %.7f} | %s(x) = %.7f | OK = %.7f "                                            \
                           "| vector length = %lu | i = %lu | tolerance = %f\n",                                             \
                           input_arr[j], scale_factor, #FNAME, output[j],                                                    \
                           ref_output[j], i, j, TOLERANCE);                                                                  \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \


    #define TEST_INPUT_QFLOAT_A1(FNAME)                                                                                      \
    do                                                                                                                       \
    {                                                                                                                        \
        uint32_t pass = 0;                                                                                                   \
        uint32_t fail = 0;                                                                                                   \
        generate_ref_output(ARR_LEN);                                                                                        \
                                                                                                                             \
        for (uint32_t i = 1; i <= ARR_LEN; ++i)                                                                              \
        {                                                                                                                    \
            memset(output, 0, sizeof(__typeof__(*output))*ARR_LEN);;                                                         \
            FNAME(input_arr, output, i);                                                                                     \
                                                                                                                             \
            for (uint32_t j = 0; j < i; ++j)                                                                                 \
            {                                                                                                                \
                if (ref_output[j] != output[j])                                                                              \
                {                                                                                                            \
                    printf("Error:\tx = {%.7f} | %s(x) = %.7f | OK = %.7f "                                                  \
                           "| vector length = %lu | i = %lu\n",                                                              \
                           input_arr[j], #FNAME, output[j],                                                                  \
                           ref_output[j], i, j);                                                                             \
                    ++fail;                                                                                                  \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    ++pass;                                                                                                  \
                }                                                                                                            \
            }                                                                                                                \
        }                                                                                                                    \
                                                                                                                             \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                    \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                              \
    }                                                                                                                        \
    while(0);                                                                                                                \

/* Macro for function implementation of test_##NAME which is d */
#define BENCH_TFUN_AF2_MATRIX(FNAME)                                                                                                                \
    void test_##FNAME(uint64_t n, uint32_t* lengths_of_arrays, uint32_t p )                                                                         \
    {                                                                                                                                               \
        uint64_t p_cycles_start;                                                                                                                    \
        uint64_t p_cycles_end;                                                                                                                      \
        double cpc = 0.0;                                                                                                                           \
        double cpm = 0.0;                                                                                                                           \
        double length_sum = 0.0;                                                                                                                    \
          for (uint32_t j = 0; j < p; ++j)                                                                                                          \
          {                                                                                                                                         \
            p_cycles_start = read_pcycles();                                                                                                        \
            for (uint32_t i = 0; i < n; ++i)                                                                                                        \
            {                                                                                                                                       \
              r = FNAME(matrix_vector[j], a1);                                                                                                      \
            }                                                                                                                                       \
            p_cycles_end = read_pcycles();                                                                                                          \
            cpc += CYCLES_PER_CALL(n, p_cycles_end - p_cycles_start);                                                                               \
            length_sum += lengths_of_arrays[j];                                                                                                     \
          }                                                                                                                                         \
        cpm = cpc / (double) length_sum;                                                                                                            \
        printf("  %-8s:  %7.4f CPC | %5.2f cycles per member count\n", #FNAME, cpc, cpm);                                                           \
    }                                                                                                                                               \

/* Macro for function implementation of test_##NAME which is d */
#define BENCH_TFUN_AF4_MATRIX(FNAME)                                                                                                                \
    void test_##FNAME(uint64_t n, uint32_t* lengths_of_arrays, uint32_t p )                                                                         \
    {                                                                                                                                               \
        uint64_t p_cycles_start;                                                                                                                    \
        uint64_t p_cycles_end;                                                                                                                      \
        double cpc = 0.0;                                                                                                                           \
        double cpm = 0.0;                                                                                                                           \
        double length_sum = 0.0;                                                                                                                    \
          for (uint32_t j = 0; j < p; ++j)                                                                                                          \
          {                                                                                                                                         \
            p_cycles_start = read_pcycles();                                                                                                        \
            for (uint32_t i = 0; i < n; ++i)                                                                                                        \
            {                                                                                                                                       \
              r = FNAME(matrix_input_vector[j], matrix_output_vector[j],lengths_of_arrays[j]);                                                      \
            }                                                                                                                                       \
            p_cycles_end = read_pcycles();                                                                                                          \
            cpc += CYCLES_PER_CALL(n, p_cycles_end - p_cycles_start);                                                                               \
            length_sum += lengths_of_arrays[j];                                                                                                     \
          }                                                                                                                                         \
        cpm = cpc / (double) length_sum;                                                                                                            \
        printf("  %-8s:  %7.4f CPC | %5.2f cycles per matrix count | %5.2f cycles per member count\n", #FNAME, cpc, cpm*MATRIX_N*MATRIX_N ,cpm);                                                           \
    }                                                                                                                                               \

/* Macro for function implementation of test_##NAME which is d */
#define BENCH_TFUN_AH4_MATRIX(FNAME)                                                                                                                \
    void test_##FNAME(uint64_t n, uint32_t* lengths_of_arrays, uint32_t p )                                                                         \
    {                                                                                                                                               \
        uint64_t p_cycles_start;                                                                                                                    \
        uint64_t p_cycles_end;                                                                                                                      \
        double cpc = 0.0;                                                                                                                           \
        double cpm = 0.0;                                                                                                                           \
        double length_sum = 0.0;                                                                                                                    \
          for (uint32_t j = 0; j < p; ++j)                                                                                                          \
          {                                                                                                                                         \
            p_cycles_start = read_pcycles();                                                                                                        \
            for (uint32_t i = 0; i < n; ++i)                                                                                                        \
            {                                                                                                                                       \
              r = FNAME(matrix_input_vector[j], matrix_output_vector[j],lengths_of_arrays[j]);                                                      \
            }                                                                                                                                       \
            p_cycles_end = read_pcycles();                                                                                                          \
            cpc += CYCLES_PER_CALL(n, p_cycles_end - p_cycles_start);                                                                               \
            length_sum += lengths_of_arrays[j];                                                                                                     \
          }                                                                                                                                         \
        cpm = cpc / (double) length_sum;                                                                                                            \
        printf("  %-8s:  %7.4f CPC | %5.2f cycles per matrix count | %5.2f cycles per member count\n", #FNAME, cpc, cpm*MATRIX_N*MATRIX_N ,cpm);    \
    }                                                                                                                                               \

#define TEST_AH3_INIT_INPUT(START_INDEX, ARR_LEN)                         \
        uint32_t range_index0;                                            \
        float lo0;                                                        \
        float hi0;                                                        \
                                                                          \
        for (uint32_t i = START_INDEX; i < ARR_LEN; ++i)                  \
        {                                                                 \
            range_index0 = i % RANGE_CNT0;                                \
            lo0 = ranges0[range_index0].lo;                               \
            hi0 = ranges0[range_index0].hi;                               \
                                                                          \
            input_data[i] = lo0 + ((int16_t) (((float) (hi0 - lo0)) * (((float)rand() / (float)RAND_MAX)))); \
        }                                                                 \


/* Macro for function implementation of test_##NAME which is d */
#define BENCH_TFUN_ACF4_MATRIX(FNAME)                                                                                                               \
    void test_##FNAME(uint64_t n, uint32_t* lengths_of_arrays, uint32_t p )                                                                         \
    {                                                                                                                                               \
        uint64_t p_cycles_start;                                                                                                                    \
        uint64_t p_cycles_end;                                                                                                                      \
        double cpc = 0.0;                                                                                                                           \
        double cpm = 0.0;                                                                                                                           \
        double length_sum = 0.0;                                                                                                                    \
          for (uint32_t j = 0; j < p; ++j)                                                                                                          \
          {                                                                                                                                         \
            p_cycles_start = read_pcycles();                                                                                                        \
            for (uint32_t i = 0; i < n; ++i)                                                                                                        \
            {                                                                                                                                       \
              r = FNAME(matrix_input_vector[j], matrix_output_vector[j],lengths_of_arrays[j]);                                                      \
            }                                                                                                                                       \
            p_cycles_end = read_pcycles();                                                                                                          \
            cpc += CYCLES_PER_CALL(n, p_cycles_end - p_cycles_start);                                                                               \
            length_sum += lengths_of_arrays[j];                                                                                                     \
          }                                                                                                                                         \
        cpm = cpc / (double) length_sum;                                                                                                            \
        printf("  %-8s:  %7.4f CPC | %5.2f cycles per matrix count | %5.2f cycles per member count\n", #FNAME, cpc, cpm*MATRIX_N*MATRIX_N ,cpm);    \
    }                                                                                                                                               \

/* Macro for creating legths of vector in per specific ranges */
#define BENCH_MATRIX_INPUT_INIT_C(VEC_LEN, N)                                                                               \
        matrix_vector =  malloc(VEC_LEN*sizeof(float complex *));                                                           \
        for (uint32_t i = 0; i < VEC_LEN; ++i)                                                                              \
        {                                                                                                                   \
            matrix_vector[i] = malloc(N*N*sizeof(float complex));                                                           \
            lengths_of_arrays[i] = N*N;                                                                                     \
          }                                                                                                                 \


/* Macro for creating legths of vector in per specific ranges */
#define BENCH_MATRIX_INPUT_INIT(VEC_LEN, N)                                                                                 \
        matrix_vector =  malloc(VEC_LEN*sizeof(float_a8_t *));                                                              \
        for (uint32_t i = 0; i < VEC_LEN; ++i)                                                                              \
        {                                                                                                                   \
            matrix_vector[i] = malloc(N*N*sizeof(float_a8_t));                                                              \
            lengths_of_arrays[i] = N*N;                                                                                     \
          }                                                                                                                 \

/* Macro for creating legths of vector in per specific ranges */
#define BENCH_MATRIX_INPUT_INIT_H(VEC_LEN, N)                                                                               \
        matrix_vector =  malloc(VEC_LEN*sizeof(int16_t *));                                                                 \
        for (uint32_t i = 0; i < VEC_LEN; ++i)                                                                              \
        {                                                                                                                   \
            matrix_vector[i] = malloc(N*N*sizeof(int16_t));                                                                 \
            lengths_of_arrays[i] = N*N;                                                                                     \
          }                                                                                                                 \


/* Macro for creating legths of vector in per specific ranges */
#define BENCH_MATRIX_NxN_INPUT_INIT(VEC_LEN)                                                                                \
        matrix_input_vector =  malloc(VEC_LEN*sizeof(float *));                                                             \
        matrix_output_vector =  malloc(VEC_LEN*sizeof(float *));                                                            \
        for (uint32_t i = 0; i < VEC_LEN; ++i)                                                                              \
        {                                                                                                                   \
            matrix_input_vector[i] = malloc(((i+2)*(i+2))*sizeof(float));                                                   \
            matrix_output_vector[i] = malloc(((i+2)*(i+2))*sizeof(float));                                                  \
            lengths_of_arrays[i] = ((i+2)*(i+2));                                                                           \
          }
                                                                                                                        \

/* Macro for creating legths of vector in per specific ranges */
#define BENCH_MATRIX_4x4_INPUT_INIT_H(VEC_LEN)                                                                                \
        matrix_input_vector =  malloc(VEC_LEN*sizeof(int16_t *));                                                               \
        matrix_output_vector =  malloc(VEC_LEN*sizeof(int32_t *));                                                              \
        for (uint32_t i = 0; i < VEC_LEN; ++i)                                                                              \
        {                                                                                                                   \
            int32_t h = 2 + (i % 3) ;                                                                                       \
            matrix_input_vector[i] = malloc(((h+2)*(h+2))*sizeof(int16_t));                                                   \
            matrix_output_vector[i] = malloc(((h+2)*(h+2))*sizeof(int32_t));                                                  \
            lengths_of_arrays[i] = ((h+2)*(h+2));                                                                           \
          }                                                                                                                 \

/* Macro for creating legths of vector in per specific ranges */
#define BENCH_MATRIX_4x4_INPUT_INIT(VEC_LEN)                                                                                \
        matrix_input_vector =  malloc(VEC_LEN*sizeof(float *));                                                               \
        matrix_output_vector =  malloc(VEC_LEN*sizeof(float *));                                                              \
        for (uint32_t i = 0; i < VEC_LEN; ++i)                                                                              \
        {                                                                                                                   \
            int32_t h = 2 + (i % 3) ;                                                                                       \
            matrix_input_vector[i] = malloc(((h+2)*(h+2))*sizeof(float));                                                   \
            matrix_output_vector[i] = malloc(((h+2)*(h+2))*sizeof(float));                                                  \
            lengths_of_arrays[i] = ((h+2)*(h+2));                                                                           \
          }                                                                                                                 \

/* Macro for creating legths of vector in per specific ranges */
#define BENCH_MATRIX_NxN_INPUT_INIT_C(VEC_LEN)                                                                                \
        matrix_input_vector =  malloc(VEC_LEN*sizeof(complex float *));                                                               \
        matrix_output_vector =  malloc(VEC_LEN*sizeof(complex float *));                                                              \
        for (uint32_t i = 0; i < VEC_LEN; ++i)                                                                              \
        {                                                                                                                   \
            int32_t h = 2 + (i % 3) ;                                                                                       \
            matrix_input_vector[i] = malloc(((h+2)*(h+2))*sizeof(complex float));                                                   \
            matrix_output_vector[i] = malloc(((h+2)*(h+2))*sizeof(complex float));                                                  \
            lengths_of_arrays[i] = ((h+2)*(h+2));                                                                           \
          }                                                                                                                 \



/* Macro for creating legths of vector in per specific ranges */
#define BENCH_MATRIX_INPUT_CLEANUP(VEC_LEN, N)                                                                              \
        for (uint32_t i = 0; i < VEC_LEN; ++i)                                                                              \
        {                                                                                                                   \
            free(matrix_vector[i]);                                                                                         \
          }                                                                                                                 \
          free(matrix_vector);                                                                                              \

/* Macro for creating legths of vector in per specific ranges */
#define BENCH_MATRIX_NXN_INPUT_CLEANUP(VEC_LEN)                                                                              \
        for (uint32_t i = 0; i < VEC_LEN; ++i)                                                                              \
        {                                                                                                                   \
            free(matrix_input_vector[i]);                                                                                   \
            free(matrix_output_vector[i]);                                                                                  \
          }                                                                                                                 \
          free(matrix_input_vector);                                                                                        \
          free(matrix_output_vector);                                                                                       \

/* Macro for populating two vectors with data from ranges */
#define BENCH_AF4_INPUT_INIT_MATRIX(RANGE_CNT0, RAND_MAX, LENGTHS_OF_ARRAYS, VEC_LEN)                            \
        float_a8_t lo0;                                                                       \
        float_a8_t hi0;                                                                       \
        uint32_t range_index0;                                                                \
        \
        for(uint16_t j = 0; j < VEC_LEN; j++)                                                                                      \
        {                                                                                       \
          for (uint32_t i = 0; i < LENGTHS_OF_ARRAYS[j]; ++i)                                                \
          {                                                                                     \
              range_index0 = i % RANGE_CNT0;                                                    \
              lo0 = ranges0[range_index0].lo;                                                   \
              hi0 = ranges0[range_index0].hi;                                                   \
                                                                                                \
              matrix_vector[j][i] = lo0 + ((((float) (hi0 - lo0)) * (((float)rand() / (float)RAND_MAX)))); \
          }                                                                           \
        }

/* Macro for populating two vectors with data from ranges */
#define BENCH_AH4_INPUT_INIT_MATRIX(RANGE_CNT0, RAND_MAX, LENGTHS_OF_ARRAYS, VEC_LEN)                            \
        float_a8_t lo0;                                                                       \
        float_a8_t hi0;                                                                       \
        uint32_t range_index0;                                                                \
        \
        for(uint16_t j = 0; j < VEC_LEN; j++)                                                                                      \
        {                                                                                       \
          for (uint32_t i = 0; i < LENGTHS_OF_ARRAYS[j]; ++i)                                                \
          {                                                                                     \
              range_index0 = i % RANGE_CNT0;                                                    \
              lo0 = ranges0[range_index0].lo;                                                   \
              hi0 = ranges0[range_index0].hi;                                                   \
                                                                                                \
              matrix_vector[j][i] = lo0 + ((int16_t) (((float) (hi0 - lo0)) * (((float)rand() / (float)RAND_MAX)))); \
          }                                                                           \
        }


/* Macro for populating two vectors with data from ranges */
#define BENCH_AH3_INPUT_INIT_MATRIX(RANGE_CNT0, RAND_MAX, LENGTHS_OF_ARRAYS, VEC_LEN)                            \
        int16_t lo0;                                                                       \
        int16_t hi0;                                                                       \
        uint32_t range_index0;                                                                \
        \
        for(uint16_t j = 0; j < VEC_LEN; j++)                                                                                      \
        {                                                                                       \
          for (uint32_t i = 0; i < LENGTHS_OF_ARRAYS[j]; ++i)                                                \
          {                                                                                     \
              range_index0 = i % RANGE_CNT0;                                                    \
              lo0 = ranges0[range_index0].lo;                                                   \
              hi0 = ranges0[range_index0].hi;                                                   \
                                                                                                \
              matrix_input_vector[j][i] = lo0 + ((int16_t) (((float) (hi0 - lo0)) * (((float)rand() / (float)RAND_MAX)))); \
          }                                                                           \
        }


/* Macro for populating two vectors with data from ranges */
#define BENCH_AF4_INPUT_INIT_MATRIX_NxN(RANGE_CNT0, RAND_MAX, LENGTHS_OF_ARRAYS, VEC_LEN)                            \
        float_a8_t lo0;                                                                       \
        float_a8_t hi0;                                                                       \
        uint32_t range_index0;                                                                \
        \
        for(uint16_t j = 0; j < VEC_LEN; j++)                                                                                      \
        {                                                                                       \
          for (uint32_t i = 0; i < LENGTHS_OF_ARRAYS[j]; ++i)                                                \
          {                                                                                     \
              range_index0 = i % RANGE_CNT0;                                                    \
              lo0 = ranges0[range_index0].lo;                                                   \
              hi0 = ranges0[range_index0].hi;                                                   \
                                                                                                \
              matrix_input_vector[j][i] = lo0 + ((((float) (hi0 - lo0)) * (((float)rand() / (float)RAND_MAX)))); \
          }                                                                           \
        }

/* */
#define TEST_AF3_INIT_INPUT(START_INDEX, ARR_LEN)                                      \
        for (uint32_t i = START_INDEX; i < ARR_LEN; ++i)                               \
        {                                                                              \
            uint32_t range_index0 = i % RANGE_CNT0;                                    \
            float lo0 = ranges0[range_index0].lo;                                      \
            float hi0 = ranges0[range_index0].hi;                                      \
                                                                                       \
            input_data[i] = lo0 + (hi0 - lo0)* ((float) rand()/(float)RAND_MAX);       \
        }                                                                              \

#define MATRIX_MULTIPLY(MATRIX1, MATRIX2, OUTPUT, N)                                      \
{                                                                                         \
  int32_t row = 0;                                                                        \
  for(row = 0; row < N; row++)                                                            \
   {                                                                                      \
    int32_t col = 0;                                                                      \
    for(col=0; col < N; col++)                                                            \
     {                                                                                    \
      int32_t add_row = 0;                                                                \
      OUTPUT[row*N + col] = 0.0;                                                          \
      for(add_row = 0; add_row < N; add_row++)                                            \
       {                                                                                  \
        OUTPUT[row*N + col] += MATRIX1[row*N + add_row] *                                 \
         MATRIX2[add_row*N + col];                                                        \
      }                                                                                   \
    }                                                                                     \
  }                                                                                       \
}                                                                                         \


#define MATRIX_MULTIPLY_TEST(MATRIX1, MATRIX2, OUTPUT, N)                                      \
{                                                                                         \
  int32_t row = 0;                                                                        \
  for(row = 0; row < N; row++)                                                            \
   {                                                                                      \
    int32_t col = 0;                                                                      \
    for(col=0; col < N; col++)                                                            \
     {                                                                                    \
      int32_t add_row = 0;                                                                \
      OUTPUT[row*N + col] = 0;                                                          \
      for(add_row = 0; add_row < N; add_row++)                                            \
       {                                                                                  \
        OUTPUT[row*N + col] += MATRIX1[row*N + add_row] *                                 \
         MATRIX2[add_row*N + col];                                                        \
      }                                                                                   \
    }                                                                                     \
  }                                                                                       \
}                                                                                         \

#define VALIDATE_MATRIX_INVERSE_F(MATRIX, N, TOL)                                         \
number_of_errors = 0;                                                                     \
for (int32_t i = 0; i < N; i++) {                                                         \
  for (int32_t j = 0; j < N; j++) {                                                       \
    if ((i == j) && ((1.0 - ABSF(MATRIX[i*N + j])) > TOL))                        \
    {                                                                                     \
      printf("[%d, %d] =  %f ; correct is 1.00 plus_minus %f \n",i , j, MATRIX[i*N + j], TOL);   \
      number_of_errors = number_of_errors + 1;                                                  \
    }                                                                                     \
    if ((i != j) && ((ABSF(MATRIX[i*N + j])) > TOL))                              \
    {                                                                                     \
      printf("[%d, %d] =  %f ; correct is 0.00 plus_minus %f \n",i , j, MATRIX[i*N + j], TOL) ;   \
      number_of_errors = number_of_errors + 1;                                                  \
    }                                                                                     \
  }                                                                                       \
}


#define VALIDATE_MATRIX_INVERSE(MATRIX, N)                                              \
number_of_errors = 0;                                                                     \
float tol = TOL;                                                                          \
for (int32_t i = 0; i < N; i++) {                                                         \
  for (int32_t j = 0; j < N; j++) {                                                       \
    if ((i == j) && ((1.0 - ABSF(MATRIX[i*N + j])) > tol))                        \
    {                                                                                     \
      printf("[%d, %d] =  %f ; correct is 1.00 plus_minus %f \n",i , j, MATRIX[i*N + j], tol);   \
      number_of_errors = number_of_errors + 1;                                                  \
    }                                                                                     \
    if ((i != j) && ((ABSF(MATRIX[i*N + j])) > tol))                              \
    {                                                                                     \
      printf("[%d, %d] =  %f ; correct is 0.00 plus_minus %f \n",i , j, MATRIX[i*N + j], tol) ;   \
      number_of_errors = number_of_errors + 1;                                                  \
    }                                                                                     \
  }                                                                                       \
}                                                                                         \
                                                                                         \

#define VALIDATE_MATRIX_INVERSE_H(MATRIX, N, TOL)                                         \
number_of_errors = 0;                                                                     \
int32_t one = 1073741824;                                                                 \
for (int32_t i = 0; i < N; i++) {                                                         \
  for (int32_t j = 0; j < N; j++) {                                                       \
    if ((i == j) && ((one - abs(MATRIX[i*N + j])) > TOL))                        \
    {                                                                                     \
      printf("[%d, %d] =  %.7d ; correct is %7.d (1.0f) plus_minus %.7d \n",i , j, MATRIX[i*N + j],one, TOL);   \
      number_of_errors = number_of_errors + 1;                                                  \
    }                                                                                     \
    if ((i != j) && ((abs(MATRIX[i*N + j])) > TOL))                              \
    {                                                                                     \
      printf("[%d, %d] =  %.7d ; correct is 0 plus_minus %.7d \n",i , j, MATRIX[i*N + j], TOL) ;   \
      number_of_errors = number_of_errors + 1;                                                  \
    }                                                                                     \
  }                                                                                       \
}                                                                                         \
                                                                                         \



#endif //_QHBLAS_TEST_H

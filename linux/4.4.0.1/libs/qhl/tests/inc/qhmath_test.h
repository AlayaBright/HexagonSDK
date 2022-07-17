/**=============================================================================
@file
    qhmath_test.h

@brief
    Macros and datatypes used in QHMATH tests.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHMATH_TEST_H
#define _QHMATH_TEST_H

#include "qhl_common.h"

#define BENCH_TCALL(FNAME) test_##FNAME()

#define BENCH_INPUT_INIT_TYPE(TYPE, ranges, arr_len, VARR)                                                \
    for (int i = 0; i < arr_len; ++i)                                                                     \
    {                                                                                                     \
        uint32_t range_index0 = i % (sizeof(ranges) / sizeof(*ranges));                                   \
        TYPE lo0 = ranges[range_index0].lo;                                                               \
        TYPE hi0 = ranges[range_index0].hi;                                                               \
        VARR[i] = lo0 + ((TYPE)(((float)(hi0 - lo0)) * (qhmath_abs_f((float)rand() / (float)RAND_MAX)))); \
    }

#define BENCH_TFUN_AF(FNAME, nr_of_iterations, arr_len, ...)     \
    void test_##FNAME(void)                                      \
    {                                                            \
        performance_high();                                      \
        printf("Benchmark for %s function:\n", #FNAME);          \
        r = FNAME(__VA_ARGS__);                                  \
        measurement_start();                                     \
        for (uint32_t ni = 0; ni < nr_of_iterations; ++ni)       \
        {                                                        \
            r = FNAME(__VA_ARGS__);                              \
        }                                                        \
        measurement_finish();                                    \
        measurement_print_af(#FNAME, nr_of_iterations, arr_len); \
        performance_normal();                                    \
    }

/* Macro for benchmark preparing the 2 array of float data based on ranges and number elements of array  */
#define BENCH_TFUN_F2_INIT(ARR_LEN, RANGE_CNT, RAND_MAX)                                                                  \
    for (uint32_t i = 0; i < ARR_LEN; ++i)                                                                                \
    {                                                                                                                     \
    uint32_t range_index = i % RANGE_CNT;                                                                           \
    float lo = ranges_a0[range_index].lo;                                                                           \
    float hi = ranges_a0[range_index].hi;                                                                           \
    a0[i] = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);                                                     \
    lo = ranges_a1[range_index].lo;                                                                                 \
    hi = ranges_a1[range_index].hi;                                                                                 \
    a1[i] = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);                                                     \
    }

#define BENCH_MOD_EXP_INIT(ARR_LEN, RANGE_CNT, RAND_MAX)                                                                  \
       for (uint32_t i = 0; i < ARR_LEN; ++i)                                                                             \
       {                                                                                                                  \
          uint32_t range_index = i % RANGE_CNT;                                                                           \
          float lo = ranges_a0[range_index].lo;                                                                           \
          float hi = ranges_a0[range_index].hi;                                                                           \
          a0[i] = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);                                                     \
          lo = ranges_a1[range_index].lo;                                                                                 \
          hi = ranges_a1[range_index].hi;                                                                                 \
          a1[i] = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);                                                     \
       }                                                                                                                  \

#define BENCH_TFUN_MODEXP(FNAME)                                                                                              \
    void test_##FNAME(uint64_t n)                                                                                             \
        {    for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                           \
            {                                                                                                                 \
            r[j] = FNAME(a0[j],a1[j]);                                                                                        \
            }                                                                                                                 \
            measurement_start();                                                                                              \
            for (uint64_t i = 0; i < n; ++i)                                                                                  \
            {                                                                                                                 \
                for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                        \
                {                                                                                                             \
                    r[j] = FNAME(a0[j], new_exponent);                                                                        \
                }                                                                                                             \
            }                                                                                                                 \
            measurement_finish();                                                                                             \
            measurement_print_f(#FNAME, n, ARR_LEN);                                                                          \
        }

#define BENCH_TFUN_FAP1(FNAME, ARR_LEN, arg1)                                                                                              \
    void test_##FNAME(uint64_t n)                                                                                             \
        {    for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                           \
            {                                                                                                                 \
            r[j] = FNAME(a[j], arg1);                                                                                        \
            }                                                                                                                 \
            measurement_start();                                                                                              \
            for (uint64_t i = 0; i < n; ++i)                                                                                  \
            {                                                                                                                 \
                for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                        \
                {                                                                                                             \
                    r[j] = FNAME(a[j], arg1);                                                                        \
                }                                                                                                             \
            }                                                                                                                 \
            measurement_finish();                                                                                             \
            measurement_print_f(#FNAME, n, ARR_LEN);                                                                          \
        }

/* Macro for benchmark of function that returns float value and takes 2 float arguments */
#define BENCH_TFUN_F2(FNAME, ARR_LEN)                                                                                     \
void test_##FNAME(uint64_t n)                                                                                                 \
    {    for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                                \
        {                                                                                                                 \
        r[j] = FNAME(a0[j],a1[j]);                                                                                        \
        }                                                                                                                 \
        measurement_start();                                                                                              \
        for (uint64_t i = 0; i < n; ++i)                                                                                       \
        {                                                                                                                 \
            for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                             \
            {                                                                                                             \
                r[j] = FNAME(a0[j],a1[j]);                                                                                \
            }                                                                                                             \
        }                                                                                                                 \
        measurement_finish();                                                                                             \
        measurement_print_f(#FNAME, n, ARR_LEN);                                                                          \
    }

/* Macro for benchmark preparing the 1 array of float data based on ranges and number elements of array  */
#define BENCH_TFUN_F1_INIT(ARR_LEN, RANGE_CNT, RAND_MAX)                                                                  \
        for (uint32_t i = 0; i < ARR_LEN; ++i)                                                                            \
        {                                                                                                                 \
        uint32_t range_index = i % RANGE_CNT;                                                                             \
        float lo = ranges[range_index].lo;                                                                                \
        float hi = ranges[range_index].hi;                                                                                \
        a[i] = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);                                                        \
        }                                                                                                                 \

/* Macro for benchmark of function that returns float value and takes 1 float argument */
#define BENCH_TFUN_F1(FNAME, ARR_LEN)                                                                                     \
    void test_##FNAME(uint64_t n)                                                                                         \
    {                                                                                                                     \
        for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                            \
            r[j] = FNAME(a[j]);                                                                                           \
        measurement_start();                                                                                              \
        for (uint64_t i = 0; i < n; ++i)                                                                                  \
        {                                                                                                                 \
            for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                             \
            {                                                                                                             \
        r[j] = FNAME(a[j]);                                                                                               \
            }                                                                                                             \
        }                                                                                                                 \
        measurement_finish();                                                                                             \
        measurement_print_f(#FNAME, n, ARR_LEN);                                                                          \
    }

/* Macro for testing accuracy of clipping function that returns array of floats and takes float array 2 float argument and 1 integer */
#define TEST_FUN_CLIPPING(FNAME, TEST_INPUTS, TEST_INPUTS_CNT,LOW_LEVEL, HIGH_LEVEL, TEST_EXPECTED_OUTPUTS)               \
    uint32_t pass = 0;                                                                                                    \
    uint32_t fail = 0;                                                                                                    \
    int validator;                                                                                                        \
    float out_data[TEST_INPUTS_CNT];                                                                                      \
    validator = FNAME(TEST_INPUTS, out_data, TEST_INPUTS_CNT, LOW_LEVEL, HIGH_LEVEL);                                     \
    printf("Accuracy test for %s function:\n", #FNAME);                                                                   \
                                                                                                                          \
      if(validator == 0)                                                                                                  \
      {                                                                                                                   \
        for (uint32_t i = 0; i < TEST_INPUTS_CNT; i++)                                                                    \
        {                                                                                                                 \
            float x = TEST_INPUTS[i]; /* function input */                                                                \
            float r = out_data[i]; /* function output */                                                                  \
            float t = TEST_EXPECTED_OUTPUTS[i]; /* expected output */                                                     \
                                                                                                                          \
            if ((isequal(r,t) == 0) )                                                                                     \
            {                                                                                                             \
                printf("Error:\tx[%.7d] = %.7f | low_level = %.7d | high_level = %.7d | %s(x) = %.7f | OK = %.7f \n",     \
                             x, i, LOW_LEVEL, HIGH_LEVEL, #FNAME,  r,            t);                                      \
                                                                                                                          \
                fail+=1;                                                                                                  \
            }                                                                                                             \
            else                                                                                                          \
            {                                                                                                             \
                pass+=1;                                                                                                  \
            }                                                                                                             \
        }                                                                                                                 \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                 \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                                             \
                                                                                                                          \
      }                                                                                                                   \
      else                                                                                                                \
      {                                                                                                                   \
        printf("Error:\t signal is not valid!");                                                                          \
      }                                                                                                                   \
                                                                                                                          \



/* Macro for testing accuracy of function that returns float value and takes 1 float argument and one integer */
#define TEST_FUN_F1I1(FNAME, TEST_INPUTS1, TEST_INPUTS2, TEST_INPUTS_CNT, TEST_EXPECTED_OUTPUTS)                      \
    uint32_t pass = 0;                                                                                                \
    uint32_t fail = 0;                                                                                                \
                                                                                                                      \
    printf("Accuracy test for %s function:\n", #FNAME);                                                               \
                                                                                                                      \
    for (uint32_t i = 0; i < TEST_INPUTS_CNT; i++)                                                                    \
    {                                                                                                                 \
        float x = TEST_INPUTS1[i]; /* function input */                                                               \
        int y = TEST_INPUTS2[i]; /* function input */                                                                 \
        float r = FNAME(x, y); /* function output */                                                                  \
        float t = TEST_EXPECTED_OUTPUTS[i]; /* expected output */                                                     \
                                                                                                                      \
        if (isequal(r,t) == 0)                                                                                        \
        {                                                                                                             \
            printf("Error:\tx = %.7f | y = %.7d | %s(x) = %.7f | OK = %.7f \n",                                       \
                         x, y,  #FNAME,  r,            t);                                                            \
                                                                                                                      \
            fail+=1;                                                                                                  \
        }                                                                                                             \
        else                                                                                                          \
        {                                                                                                             \
            pass+=1;                                                                                                  \
        }                                                                                                             \
    }                                                                                                                 \
                                                                                                                      \
    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                 \
    printf("(%lu tests passed, %lu failed)\n", pass, fail);


/* Macro for testing accuracy of function that returns float value and takes 2 float argument */
#define TEST_FUN_F2(FNAME, TEST_INPUTS1, TEST_INPUTS2, TEST_INPUTS_CNT, TEST_EXPECTED_OUTPUTS)                        \
    uint32_t pass = 0;                                                                                                \
    uint32_t fail = 0;                                                                                                \
                                                                                                                      \
    printf("Accuracy test for %s function:\n", #FNAME);                                                               \
                                                                                                                      \
    for (uint32_t i = 0; i < TEST_INPUTS_CNT; i++)                                                                    \
    {                                                                                                                 \
        float x = TEST_INPUTS1[i]; /* function input */                                                               \
        float y = TEST_INPUTS2[i]; /* function input */                                                               \
        float r = FNAME(x, y); /* function output */                                                                  \
        float t = TEST_EXPECTED_OUTPUTS[i]; /* expected output */                                                     \
                                                                                                                      \
        if (isequal(r,t) == 0)                                                                                        \
        {                                                                                                             \
            printf("Error:\tx = %.7f | y = %.7f | %s(x) = %.7f | OK = %.7f \n",                                       \
                         x, y,  #FNAME,  r,            t);                                                            \
                                                                                                                      \
            fail+=1;                                                                                                  \
        }                                                                                                             \
        else                                                                                                          \
        {                                                                                                             \
            pass+=1;                                                                                                  \
        }                                                                                                             \
    }                                                                                                                 \
                                                                                                                      \
    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                 \
    printf("(%lu tests passed, %lu failed)\n", pass, fail);



/* Macro for testing accuracy of function that returns float value and takes 1 argument */
#define TEST_FUN_F1(FNAME, TEST_INPUTS, TEST_INPUTS_CNT, TEST_EXPECTED_OUTPUTS)                                       \
    uint32_t pass = 0;                                                                                                \
    uint32_t fail = 0;                                                                                                \
                                                                                                                      \
    printf("Accuracy test for %s function:\n", #FNAME);                                                               \
                                                                                                                      \
    for (uint32_t i = 0; i < TEST_INPUTS_CNT; i++)                                                                    \
    {                                                                                                                 \
        float x = TEST_INPUTS[i]; /* function input */                                                                \
        float r = FNAME(x); /* function output */                                                                     \
        float t = TEST_EXPECTED_OUTPUTS[i]; /* expected output */                                                     \
                                                                                                                      \
        if (isequal(r,t) == 0)                                                                                        \
        {                                                                                                             \
            printf("Error:\tx = %.7f | %s(x) = %.7f | OK = %.7f \n",                                                  \
                         x,   #FNAME,  r,            t);                                                              \
                                                                                                                      \
            fail+=1;                                                                                                  \
        }                                                                                                             \
        else                                                                                                          \
        {                                                                                                             \
            pass+=1;                                                                                                  \
        }                                                                                                             \
    }                                                                                                                 \
                                                                                                                      \
    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                 \
    printf("(%lu tests passed, %lu failed)\n", pass, fail);

#define BENCH_TFUN_AF3(FNAME, ARR_LEN)                                                                    \
    void test_##FNAME(long n)                                                                    \
    {                                                                                            \
        measurement_start();                                                                     \
        for (int i = 0; i < n; ++i)                                                              \
        {                                                                                        \
          r = FNAME(a0, a1, ARR_LEN);                                                            \
        }                                                                                        \
        measurement_finish();                                                                    \
        measurement_print_af(#FNAME, n, ARR_LEN);                                                \
    }

#define BENCH_TFUN_AF4(FNAME, ARR_LEN)                                                                    \
    void test_##FNAME(long n)                                                                    \
    {                                                                                            \
        measurement_start();                                                                     \
        for (int i = 0; i < n; ++i)                                                              \
        {                                                                                        \
          r = FNAME(a0, a1, a2, ARR_LEN);                                                        \
        }                                                                                        \
        measurement_finish();                                                                    \
        measurement_print_af(#FNAME, n, ARR_LEN);                                                \
    }

#define BENCH_TFUN_POW_AF4(FNAME, ARR_LEN)                                                       \
    void test_##FNAME(long n)                                                                    \
    {                                                                                            \
        measurement_start();                                                                     \
        for (int j = 0; j < ARR_LEN; j++)                                                        \
        {                                                                                        \
            r = FNAME(a0, a1[j], a2, ARR_LEN);                                                   \
        }                                                                                        \
        measurement_finish();                                                                    \
        measurement_print_af(#FNAME, ARR_LEN, ARR_LEN);                                          \
    }


/* Macro for calling the function on which is needed to
   perform test - 1 argument for input data */
#define TEST_CALL_FUNC_AF3(FNAME, INPUT, OUTPUT, LEN)           \
                           FNAME(INPUT, OUTPUT, LEN)            \

/* Macro for calling the function on which is needed to
   perform test - 2 argument for input data */
#define TEST_CALL_FUNC_AF4(FNAME, INPUT1, INPUT2, OUTPUT, LEN) \
                           FNAME(INPUT1, INPUT2, OUTPUT, LEN)  \

/* Macro for generating reference output data which will be using
   for comparision purposes - 1 argument for input data */
#define TEST_GEN_REF_AF3(FNAME)                  \
        for (int i = 0; i < ARR_LEN; i++)        \
        {                                        \
            ref_data[i] = FNAME(input_data[i]);  \
        }                                        \

/* Macro for generating referent output data which will be using
   for comparision purposes - 2 argument for input data */
#define TEST_GEN_REF_AF4(FNAME)                                   \
        for (int i = 0; i < ARR_LEN; i++)                         \
        {                                                         \
            ref_data[i] = FNAME(input_data1[i], input_data2[i]);  \
        }                                                         \

/* Macro for executing parameter test - 1 argument for input data */
#define TEST_EXEC_PARAM_AF3(FNAME)                                \
        printf("Test for %s function:\n", #FNAME);       \
        for (int i = 0; i < NUM_TEST_TYPE; i++)                   \
        {                                                         \
            if (test_data[i].expect_res !=                        \
                TEST_CALL_FUNC_AF3(FNAME, test_data[i].input_arr, test_data[i].output_arr, test_data[i].length))\
            {                                          \
                printf("FAILED \n");                \
                printf("ITER : %d\n", i);         \
                return 1;                            \
            }                                          \
        }                                              \

/* Macro for executing parameter test - 2 argument for input data */
#define TEST_EXEC_PARAM_AF4(FNAME)                          \
    printf("Test for %s function:\n", #FNAME); \
        for (int i = 0; i < NUM_TEST_TYPE; i++)             \
        {                          \
            if (test_data[i].expect_res !=                  \
                TEST_CALL_FUNC_AF4(FNAME, test_data[i].input1_arr, test_data[i].input2_arr, \
                                   test_data[i].output_arr, test_data[i].length))           \
            {                                         \
                printf("FAILED \n");                            \
                printf("ITER : %d\n", i);                     \
                return 1;                                          \
            }                                                      \
        }                                                          \

/* Macro for executing accuracy test - 1 argument for input data */
#define TEST_EXEC_ACC_AF3(FNAME)                                 \
        uint32_t pass = 0;                                       \
        uint32_t fail = 0;                                       \
        int32_t res;                                             \
        res = FNAME(input_data, output_data, ARR_LEN);           \
        for (int i = 0; i < ARR_LEN; ++i)                        \
        {                                                        \
            if ( (*((uint32_t*) &ref_data[i])) != (*((uint32_t*) &output_data[i])))       \
            {                                                                             \
                printf("Error:\tx = %.7f | %s(x) = %.7f | OK = %.7f \n",                  \
                             input_data[i],   #FNAME,  output_data[i], ref_data[i]);      \
                fail+=1;                                                                  \
            }                                                                             \
            else                                                                          \
            {                                                                             \
                pass+=1;                                                                  \
            }                                                                             \
        }                                                                                 \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                 \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                             \

/* Macro for executing accuracy test - 2 argument for input data */
#define TEST_EXEC_ACC_AF4(FNAME)                                                              \
        uint32_t pass = 0;                                                                    \
        uint32_t fail = 0;                                                                    \
        int32_t res;                                                                          \
        res = FNAME(input_data1, input_data2, output_data, ARR_LEN);                          \
        for (int i = 0; i < ARR_LEN; ++i)                                                     \
        {                                                                                     \
            if ( (*((uint32_t*) &ref_data[i])) != (*((uint32_t*) &output_data[i])))           \
            {                                                                                 \
                printf("Error:\tx = {%.7f, %.7f} | %s(x) = %.7f | OK = %.7f \n",              \
                       input_data1[i], input_data2[i], #FNAME,  output_data[i], ref_data[i]); \
                                                                                              \
                fail+=1;                                                                      \
            }                                                                                 \
            else                                                                              \
            {                                                                                 \
                pass+=1;                                                                      \
            }                                                                                 \
        }                                                                                     \
        printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                     \
        printf("(%lu tests passed, %lu failed)\n", pass, fail);                                 \

#define BENCH_AF3_INPUT_INIT(ARR_LEN, RANGE_CNT, RAND_MAX)                  \
        for (int i = 0; i < ARR_LEN; ++i)                                   \
        {                                                                   \
            uint32_t range_index0 = i % RANGE_CNT;                          \
            float lo0 = ranges0[range_index0].lo;                           \
            float hi0 = ranges0[range_index0].hi;                           \
                                                                            \
            a0[i] = lo0 + (hi0 - lo0) * ((float)rand() / (float)RAND_MAX);  \
        }

#define BENCH_AF4_INPUT_INIT(ARR_LEN, RANGE_CNT0, RANGE_CNT1, RAND_MAX)          \
        float lo0;                                                               \
        float hi0;                                                               \
        uint32_t range_index0;                                                   \
        uint32_t range_index1;                                                   \
        float lo1;                                                               \
        float hi1;                                                               \
                                                                                 \
        for (int i = 0; i < ARR_LEN; ++i)                                        \
        {                                                                        \
            range_index0 = i % RANGE_CNT0;                                       \
            lo0 = ranges0[range_index0].lo;                                      \
            hi0 = ranges0[range_index0].hi;                                      \
                                                                                 \
            range_index1 = i % RANGE_CNT1;                                       \
            lo1 = ranges1[range_index1].lo;                                      \
            hi1 = ranges1[range_index1].hi;                                      \
                                                                                 \
            a0[i] = lo0 + (hi0 - lo0) * ((float)rand() / (float)RAND_MAX);       \
            a1[i] = lo1 + (hi1 - lo1) * ((float)rand() / (float)RAND_MAX);       \
        }

#define TEST_AF3_INIT_SPEC_VAL(NUM_SPEC_VALUE)                  \
        for (int i = 0; i < NUM_SPEC_VALUE; i++)                \
        {                                                       \
            input_data[i] = spec_value[i];                      \
        }                                                       \

#define TEST_AF4_INIT_SPEC_VAL(NUM_SPEC_VALUE)                            \
        for (int i = 0, j = 0; i < NUM_SPEC_VALUE * NUM_SPEC_VALUE; i++)  \
        {                                                                 \
            if (((i % NUM_SPEC_VALUE) == 0) && (i > 0))                   \
            {                                                             \
                j++;                                                      \
            }                                                             \
                                                                          \
            input_data1[i] = spec_value[i % NUM_SPEC_VALUE];              \
            input_data2[i] = spec_value[j];                               \
        }                                                                 \

#define TEST_AF3_INIT_INPUT(START_INDEX, ARR_LEN)                                 \
        uint32_t range_index0;                                                    \
        float lo0;                                                                \
        float hi0;                                                                \
                                                                                  \
        for (int i = START_INDEX; i < ARR_LEN; ++i)                               \
        {                                                                         \
            range_index0 = i % RANGE_CNT0;                                        \
            lo0 = ranges0[range_index0].lo;                                       \
            hi0 = ranges0[range_index0].hi;                                       \
                                                                                  \
            input_data[i] = lo0 + (hi0 - lo0)* ((float) rand()/ (float)RAND_MAX); \
        }                                                                         \

#define TEST_AF4_INIT_INPUT(START_INDEX, ARR_LEN)                                       \
        uint64_t range_index0;                                                          \
        uint64_t range_index1;                                                          \
        float lo0;                                                                      \
        float hi0;                                                                      \
        float lo1;                                                                      \
        float hi1;                                                                      \
        for (int i = START_INDEX; i < ARR_LEN; ++i)                                     \
        {                                                                               \
            range_index0 = i % RANGE_CNT0;                                              \
            lo0 = ranges0[range_index0].lo;                                             \
            hi0 = ranges0[range_index0].hi;                                             \
                                                                                        \
            range_index1 = i % RANGE_CNT0;                                              \
            lo1 = ranges1[range_index1].lo;                                             \
            hi1 = ranges1[range_index1].hi;                                             \
                                                                                        \
            input_data1[i] = lo0 + (hi0 - lo0) * ((float)rand() / (float)RAND_MAX);     \
            input_data2[i] = lo1 + (hi1 - lo1) * ((float)rand() / (float)RAND_MAX);     \
        }                                                                               \

#define BENCH_H1_INPUT_INIT_16B(ARR_LEN, RANGE_CNT, RAND_MAX)                                 \
        int16_t lo0, hi0;                                                                     \
        for (int i = 0; i < ARR_LEN; ++i)                                                     \
        {                                                                                     \
            uint32_t range_index0 = i % RANGE_CNT;                                            \
            lo0 = ranges[range_index0].lo;                                                    \
            hi0 = ranges[range_index0].hi;                                                    \
                                                                                              \
            a[i] = lo0 + ((int16_t) (((float) (hi0 - lo0)) * (qhmath_abs_f((float)rand() / (float)RAND_MAX))));  \
        }

#define BENCH_H1_INPUT_INIT_32B(ARR_LEN, RANGE_CNT, RAND_MAX)                                 \
        int32_t lo0, hi0;                                                                     \
        for (int i = 0; i < ARR_LEN; ++i)                                                     \
        {                                                                                     \
            uint32_t range_index0 = i % RANGE_CNT;                                            \
            lo0 = ranges[range_index0].lo;                                                    \
            hi0 = ranges[range_index0].hi;                                                    \
                                                                                              \
            a[i] = lo0 + ((int32_t) (((float) (hi0 - lo0)) * (qhmath_abs_f((float)rand() / (float)RAND_MAX))));  \
        }

#define BENCH_H1_INPUT_INIT_U16B(ARR_LEN, RANGE_CNT, RAND_MAX)                                 \
        uint16_t lo0, hi0;                                                                     \
        for (int i = 0; i < ARR_LEN; ++i)                                                     \
        {                                                                                     \
            uint32_t range_index0 = i % RANGE_CNT;                                            \
            lo0 = ranges[range_index0].lo;                                                    \
            hi0 = ranges[range_index0].hi;                                                    \
                                                                                              \
            a[i] = lo0 + ((uint16_t) (((float) (hi0 - lo0)) * (qhmath_abs_f((float)rand() / (float)RAND_MAX))));  \
        }

#define BENCH_H2_INPUT_INIT_16B(ARR_LEN, RANGE_CNT, RAND_MAX)                                 \
        int16_t lo0, hi0;                                                                     \
        for (int i = 0; i < ARR_LEN; ++i)                                                     \
        {                                                                                     \
            uint32_t range_index0 = i % RANGE_CNT;                                            \
            lo0 = ranges[range_index0].lo;                                                    \
            hi0 = ranges[range_index0].hi;                                                    \
                                                                                              \
            lo0 = ranges[range_index0].lo;                                                    \
            hi0 = ranges[range_index0].hi;                                                    \
            a0[i] = lo0 + ((int16_t) (((float) (hi0 - lo0)) * (qhmath_abs_f((float)rand() / (float)RAND_MAX))));  \
            a1[i] = lo0 + ((int16_t) (((float) (hi0 - lo0)) * (qhmath_abs_f((float)rand() / (float)RAND_MAX))));  \
        }

/* Macro for benchmark of function that returns half value and takes 2 half argument */
#define BENCH_TFUN_H2(FNAME, ARR_LEN)                                                                                     \
    void test_##FNAME(uint64_t n)                                                                                         \
    {                                                                                                                     \
        for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                            \
            r[j] = FNAME(a0[j], a1[j]);                                                                                           \
        measurement_start();                                                                                              \
        for (uint64_t i = 0; i < n; ++i)                                                                                  \
        {                                                                                                                 \
            for (uint32_t j = 0; j < ARR_LEN; ++j)                                                                             \
            {                                                                                                             \
                  r[j] = FNAME(a0[j], a1[j]);                                                                                               \
            }                                                                                                             \
        }                                                                                                                 \
        measurement_finish();                                                                                             \
        measurement_print_f(#FNAME, n, ARR_LEN);                                                                          \
    }

#endif //_QHMATH_TEST_H

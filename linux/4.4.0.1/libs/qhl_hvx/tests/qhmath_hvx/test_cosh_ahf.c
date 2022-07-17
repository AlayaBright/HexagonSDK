/**=============================================================================
@file
    test_cosh_af.c

@brief
    Accuracy test for qhmath_hvx_cosh_ahf function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>
#include "qhmath_hvx.h"
#include "qhblas_test.h"
#include <math.h>

static range_float_t ranges0[] =
{
    {  -40.0f,   40.0f }
};


static __fp16 input[ARR_LEN];
static __fp16 ref_output[ARR_LEN];
static __fp16 output[ARR_LEN];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))
// #define DUMPDATA

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = (__fp16)cosh((float)input[i]);
    }
}

inline static void generate_input_float(__fp16* input1, uint32_t length, range_float_t* range)
{
    uint32_t range_index = 0;
    __fp16 lo = range[range_index].lo;
    __fp16 hi = range[range_index].hi;
    for (uint32_t i = 0; i < length; ++i)
    {
        input1[i] = (__fp16)(lo + (hi - lo) * i / length);
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;
    float tolerance = 6.5;
#ifdef DUMPDATA
    char *output_filename = "outputs_cosh_ahf.txt";
    FILE *fp_output;
    fp_output = fopen(output_filename, "w");
#endif

    printf("Test for qhmath_hvx_cosh_ahf function:\n");

    // INIT_INPUT_FLOAT(input, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    generate_input_float(input, ARR_LEN, ranges0);

    generate_ref_output(ARR_LEN);

    for (uint32_t i = ARR_LEN; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);

        qhmath_hvx_cosh_ahf(input, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            float error = ref_output[j] - output[j];
            if (error < 0)
                error = -error;

            float error_percent = fabsf((ref_output[j]-output[j])/(ref_output[j]))*100.0f;

            if (error_percent > tolerance)
            {
                printf("Error:\tx = { %.7f } | "
                       "qhmath_hvx_cosh_ahf(x) = %.7e | OK = %.7e "
                       "| error_percent = %2.5f "
                       "| vector length = %u | i = %u\n",
                       input[j], output[j],
                       ref_output[j],
                       error_percent, i, j);
                ++fail;
            }
            else
            {
                ++pass;
            }
        }
    }

    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");
    printf("(%u tests passed, %u failed, with a tolerance of %.1f %%)\n", pass, fail, tolerance);

#ifdef DUMPDATA
    for (uint32_t j = 0; j < ARR_LEN; ++j)
    {
        fprintf(fp_output, "%.7f,%.7f,%.7f,0x%x,0x%x\n", input[j], output[j], ref_output[j], *(uint16_t*)&input[j], *(uint16_t*)&output[j]);
    }
    fclose(fp_output);
#endif

    return 0;
}
#else
int main(void)
{
    return 0;
}
#endif

/**=============================================================================
 @file
    test_modexp_af.c

 @brief
    Accuracy test for qhmath_modexp_af function.

 Copyright (c) 2019 Qualcomm Technologies Incorporated.
 All Rights Reserved. Qualcomm Proprietary and Confidential.
 =============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "qhmath.h"

int32_t ok_modexp_af(float_a8_t *input, float_a8_t *output, int32_t length,
        int32_t new_exponent);

#define TEST_DATA_CNT    1024

static int32_t test_new_exponent[] =
{ -25, -5, -1, 0, 1, 5, 25, };

#define TEST_NEW_EXPONENT_CNT (sizeof(test_new_exponent)/sizeof(*test_new_exponent))

static float_a8_t in_data[TEST_DATA_CNT] =
{ -0.0, 0.0, 1.0, 10.0, 100.0, 2.0, 4.0, 8.0, 16.0, 32.0, 256.0, 0.5, 0.25,
        256.5664, 987987.122, 1155.1232456, };

static float_a8_t ok_data[TEST_DATA_CNT];

static float_a8_t out_data[TEST_DATA_CNT];

static long test_executed = 0L;
static long fail_detected = 0L;

int main(void)
{
    int r;
    bool test_ok = true;

    // prepare input data
    for (int i = 16; i < TEST_DATA_CNT; ++i)
    {
        float lo = -9.99e20;
        float hi = +9.99e20;

        in_data[i] = lo + (float)(rand()) / ((float)RAND_MAX/(hi-lo));
    }

    for (int new_exponent_idx = 0; new_exponent_idx < TEST_NEW_EXPONENT_CNT;
            new_exponent_idx++)
    {
        int32_t new_exponent = test_new_exponent[new_exponent_idx];

        for (int test_cnt = 1; test_cnt <= TEST_DATA_CNT; test_cnt++)
        {
            if (!test_ok)
            {
                // break;
            }

            // printf("Test qhmath_modexp_af function with data array size= %d\n", test_cnt);

            test_executed++;

            r = ok_modexp_af(in_data, ok_data, test_cnt, new_exponent);

            if (r != 0)
            {
                printf("Error with input parameters while creating reference array\n");
                continue;
            }

            memset(out_data, 0, sizeof(out_data));

            r = qhmath_modexp_af(in_data, out_data, test_cnt, new_exponent);

            if (r == 0)
            {
                for (int i = 0; i < test_cnt; i++)
                {
                    if (out_data[i] != ok_data[i])
                    {
                        printf("ERROR on member index= %d / "
                                "test count= %d| "
                                "got: %e > should be: %e\n", i, test_cnt,
                                out_data[i], ok_data[i]);
                        fail_detected = fail_detected + 1;
                        test_ok = false;

                        break;
                    }
                }
            }
            else
            {
                printf("Error with input parameters! "
                        "r= %d | in/out | cnt= %d | new_exponent= %ld\n", r,
                        test_cnt, new_exponent);
                test_ok = false;
                fail_detected = fail_detected + 1;
            }
        }
    }

    if (test_ok)
    {
        printf("Test for qhmath_modexp_ah function:\n");
        printf("PASSED (%d tests passed, %d failed)\n",test_executed-fail_detected,fail_detected);
    }
    else
    {
      printf("Test for qhmath_modexp_ah function:\n");
      printf("FAILED (%d tests passed, %d failed)\n",test_executed-fail_detected,fail_detected);
    }

    return 0;
}

/* -------------------------------------------------------------- */

/** Reference function for qhmath_modexp_af assembly */

typedef union
{
    float value;
    struct
    {
        uint32_t mantissa :23;
        uint32_t exponent :8;
        uint32_t sign :1;
    };
    uint32_t bitval;
} fp_ieee754_t;

/** ok_modexp_af function generates valid data for comparing */
int32_t ok_modexp_af(float_a8_t *input, float_a8_t *output, int32_t length,
        int32_t new_exponent)
{
    if (length <= 0)
        return -1;

    new_exponent += 0x7F;

    if (new_exponent > 254)
        return -1;
    if (new_exponent < 0)
        return -1;

    for (int i = 0; i < length; ++i)
    {
        fp_ieee754_t x;

        x.value = input[i];

        ((fp_ieee754_t *) &x)->exponent = new_exponent;

        output[i] = x.value;
    }

    return 0;
}

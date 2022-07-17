/**=============================================================================
 @file
    test_abs_ah.c

 @brief
    Accuracy test for qhmath_abs_ah function.

 Copyright (c) 2019 Qualcomm Technologies Incorporated.
 All Rights Reserved. Qualcomm Proprietary and Confidential.
 =============================================================================**/

#define T_START     1
#define T_INC       1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "qhmath.h"

#define TEST_DATA_CNT 1024

static long test_passed = 0L;
static long fail_detected = 0L;

int16_t range_lo = -32765;
int16_t range_hi = 32765;

int16_a8_t in_data[TEST_DATA_CNT];
int16_a8_t ok_data[TEST_DATA_CNT];

int16_a8_t in_tmp[TEST_DATA_CNT];
int16_a8_t out_data[TEST_DATA_CNT];

/** The base function, used for assembler variant */
int32_t ok_abs_ah(int16_a8_t *input, int16_a8_t *output, uint32_t size)
{
    if (!input || !output)
    {
        return -1;
    }

    if (size == 0)
    {
        return -1;
    }

    for (uint32_t i = 0; i < size; i++)
    {
        int16_t actual_val = input[i];

        if (actual_val < 0)
            actual_val = -actual_val;

        output[i] = actual_val;
    }

    return 0;
}

const int16_t IN_TMP_TEST = ((int16_t)0xBABA);
const int16_t OUT_TMP_TEST = ((int16_t)0xE1E1);

int main(void)
{
    int r;
    bool test_ok = true;

    puts("Test qhmath_abs_ah function\n");

    // temporary input array
    for (int i = 0; i < TEST_DATA_CNT; i++)
    {
        // Generate random integer number in range
        in_data[i] = (int16_t)(range_lo + (rand() % (range_hi - range_lo + 1)));
    }

    r = ok_abs_ah(in_data, ok_data, TEST_DATA_CNT);
    if (r)
    {
        puts("Error while generating valid output for compare");
    }

    for (int test_cnt = T_START; test_cnt <= TEST_DATA_CNT; test_cnt += T_INC)
    {
        // printf("Test qhmath_abs_ah function: data count= %d\n", test_cnt);

        memset(in_tmp, (uint8_t)IN_TMP_TEST, TEST_DATA_CNT * sizeof(int16_t));
        memset(out_data, (uint8_t)OUT_TMP_TEST, TEST_DATA_CNT * sizeof(int16_t));
        memcpy(in_tmp, in_data, test_cnt * sizeof(int16_t));

        r = qhmath_abs_ah(in_tmp, out_data, test_cnt);

        if (r == 0)
        {

            for (int i = 0; i < TEST_DATA_CNT; i++)
            {
                test_passed++;
                if (i < test_cnt)
                {
                    if (out_data[i] != ok_data[i])
                    {
                        printf("ERROR [%d]/%d | "
                               "for: %d > got: %d > should be: %d\n",
                               i,
                               test_cnt, in_tmp[i], out_data[i], ok_data[i]);

                        test_ok = false;
                        fail_detected++;
                    }
                }
                else
                {
                    if ((int16_t)in_tmp[i] != IN_TMP_TEST)
                    {
                        printf("ERROR IN INPUT ARRAY OVERWRITE\n");
                        test_ok = false;
                        fail_detected++;
                    }
                    if ((int16_t)out_data[i] != OUT_TMP_TEST)
                    {
                        printf("ERROR WRITE MORE IN OUTPUT ARRAY !\n");
                        test_ok = false;
                        fail_detected++;
                    }
                }

                if (!test_ok)
                {
                    break;
                }
            }
        }
        else
        {
            puts("Error with input data!");
            test_ok = false;
            fail_detected++;
        }

        if (!test_ok)
        {
            for (int i = 0; i < test_cnt + 16; ++i)
            {
                printf("[%3d] %d > %d | \n", i, in_tmp[i], out_data[i]);
            }
            break;
        }
    }

    if (test_ok)
    {
        printf("Test for qhmath_abs_ah function:\n");
        printf("PASSED (%d tests passed, %d failed)\n", test_passed, fail_detected);
    }
    else
    {
        printf("Test for qhmath_abs_ah function:\n");
        printf("FAILED (%d tests passed, %d failed)\n", test_passed, fail_detected);
    }

    return 0;
}

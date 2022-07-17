/**=============================================================================
@file
    test_clipping_ah.c

@brief
    Accuracy test for qhmath_clipping_ah function.n.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#define T_START             1
#define T_INC               1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "qhmath.h"

#define TEST_DATA_CNT       1024

static long test_passed = 0L;
static long fail_detected = 0L;

int16_t low_level = 256;
int16_t high_level = 512;
int16_t lowest_data = -32000;
int16_t higest_data = +32000;

int16_a8_t in_data[TEST_DATA_CNT];
int16_a8_t ok_data[TEST_DATA_CNT];

int16_a8_t in_tmp[TEST_DATA_CNT];
int16_a8_t out_data[TEST_DATA_CNT];

/** The base clipping function, used for assembler variant */
int32_t ok_clipping_ah(int16_a8_t *input, int16_a8_t *output,
                       uint32_t size, int16_t lowest_val, int16_t highest_value)
{
    if (!input || !output)
    {
        return -1;
    }

    if (size == 0)
    {
        return -1;
    }

    if (lowest_val > highest_value)
    {
        return -1;
    }

    for (uint32_t i = 0; i < size; i++)
    {
        int16_t actual_val = input[i];

        if (lowest_val > actual_val)
            actual_val = lowest_val;
        else if (actual_val > highest_value)
            actual_val = highest_value;

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

    puts("Test qhmath_clipping_ah function\n");

    // temporary input array
    for (int i = 0; i < TEST_DATA_CNT; i++)
    {
        // Generate random integer number in range
        in_data[i] = (int16_t)(lowest_data + (rand() % (higest_data - lowest_data + 1)));
    }

    r = ok_clipping_ah(in_data, ok_data, TEST_DATA_CNT, low_level, high_level);
    if (r)
    {
        puts("Error while generating valid output for compare");
    }

    for (int test_cnt = T_START; test_cnt <= TEST_DATA_CNT; test_cnt += T_INC)
    {
        //        printf("Test qhmath_clipping_ah function: data count= %d | "
        //               "low_level= %d | high_level= %d\n",
        //               test_cnt, low_level,
        //               high_level);

        memset(in_tmp, (uint8_t)IN_TMP_TEST, TEST_DATA_CNT * sizeof(int16_t));
        memset(out_data, (uint8_t)OUT_TMP_TEST, TEST_DATA_CNT * sizeof(int16_t));
        memcpy(in_tmp, in_data, test_cnt * sizeof(int16_t));

        r = qhmath_clipping_ah(in_tmp, out_data, test_cnt, low_level,
                               high_level);

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
            // for (int i = 0; i < test_cnt + 16; ++i)
            // {
            //     printf("[%3d] %d > %d | \n", i, in_tmp[i], out_data[i]);
            // }
            break;
        }
    }

    if (test_ok)
    {
        printf("Test for qhmath_modexp_ah function:\n");
        printf("PASSED (%d tests passed, %d failed)\n",test_passed,fail_detected);
    }
    else
    {
      printf("Test for qhmath_modexp_ah function:\n");
      printf("FAILED (%d tests passed, %d failed)\n",test_passed,fail_detected);
    }

    return 0;
}

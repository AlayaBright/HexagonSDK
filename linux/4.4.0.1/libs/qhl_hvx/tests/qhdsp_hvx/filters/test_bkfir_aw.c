/**=============================================================================
@file
    test_bkfir_aw.c

@brief
    Accuracy test for qhdsp_hvx_bkfir_aw function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include <hexagon_types.h>

#include "qhdsp_hvx.h"
#include "qhblas_test.h"

#undef ARR_LEN
#define ARR_LEN 4096

static range_int32_t ranges0[] =
{
    { -800000, -10000  },
    { -10000,  -1000   },
    { -1000,    1000   },
    {  1000,    10000  },
    {  10000,   800000 }
};

#define MAX_TAPS_SIZE (1024)

static int32_t __attribute__((aligned(128))) input[ARR_LEN + MAX_TAPS_SIZE];
static int32_t __attribute__((aligned(128))) coeffs[MAX_TAPS_SIZE];
static int32_t __attribute__((aligned(128))) output[ARR_LEN];
static int32_t ref_output[ARR_LEN];

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))

static int32_t mult(int32_t x, int32_t y)
{
    int32_t res = ((int64_t)x * (int64_t)y + 0x40000000) >> 31;
    return (res == 0x80000000 ? 0x7fffffff : res);
}

static void bkfir(int32_t *xin, int32_t *coefs, uint32_t taps, uint32_t length, int32_t *yout)
{
  int32_t sum;
  for (uint32_t i = 0; i < length; i++)  {
      sum = 0;

      for (uint32_t j = 0; j < taps; j++) {
         sum += mult(coefs[j], xin[i+j]);
      }

      yout[i] = sum;
  }
}

int main(void)
{
    printf("Test for test_bkfir_aw function:\n");

    INIT_INPUT_INT32(input, ARR_LEN+MAX_TAPS_SIZE, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT32(coeffs, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    uint32_t pass = 0;
    uint32_t fail = 0;
    int32_t failed = 0;

    // Go through variable size of taps.
    for (uint32_t taps = 64; taps <= MAX_TAPS_SIZE; taps += 64)
    {
        failed = 0;

        // Clear buffers.
        memset(output, 0, sizeof(output));

        // Call functions.
        bkfir(input, coeffs, taps, ARR_LEN, ref_output);

        qhdsp_hvx_bkfir_aw(input, coeffs, taps, ARR_LEN, output);

        // Check outputs.
        for (uint32_t j = 0; j < ARR_LEN; ++j)
        {
            if (ref_output[j] != output[j])
            {
                failed = 1;

                printf("Error:\tOutput buffer | got = %ld | OK = %ld | "
                       "input = %ld | input length = %d | taps = %ld | "
                       "iter = %ld\n",
                       output[j], ref_output[j], input[j], ARR_LEN, taps, j);
            }
        }

        fail += failed;
        pass += !failed;
    }

    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");
    printf("(%lu tests passed, %lu failed)\n", pass, fail);

    return 0;
}

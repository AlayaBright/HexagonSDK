/**=============================================================================
@file
    test_bkfir_ah.c

@brief
    Accuracy test for qhdsp_hvx_bkfir_ah function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include <hexagon_types.h>

#include "qhdsp_hvx.h"
#include "qhblas_test.h"

static range_int16_t ranges0[] =
{
    { -1000, -100  },
    { -100,   100  },
    {  100,   1000 }
};

#define MAX_TAPS_SIZE (1024)

static int16_t __attribute__((aligned(128))) input[ARR_LEN + MAX_TAPS_SIZE];
static int16_t __attribute__((aligned(128))) coeffs[MAX_TAPS_SIZE];
static int16_t __attribute__((aligned(128))) output[ARR_LEN];
static int16_t ref_output[ARR_LEN];

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define mult_r(x, y)  Q6_R_asrh_R(Q6_R_mpy_RlRl_s1_rnd_sat(x, y))


static void bkfir(int16_t *xin, int16_t *coefs, uint32_t taps, uint32_t length, int16_t *yout)
{
  for (uint32_t i = 0; i < length; i++)  {
      int32_t sum = 0;
      for (uint32_t j = 0; j < taps; j++) {
         sum += coefs[j] * xin[i+j];
      }

      yout[i] = Q6_R_sath_R(Q6_P_asr_PR(sum, 15));
  }
}


int main(void)
{
    printf("Test for test_bkfir_ah function:\n");

    INIT_INPUT_INT16(input, ARR_LEN + MAX_TAPS_SIZE, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT16(coeffs, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

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
        bkfir(input, coeffs, taps, ARR_LEN-32+1, ref_output);

        qhdsp_hvx_bkfir_ah(input, coeffs, taps, ARR_LEN-32+1, output);

        // Check outputs.
        for (uint32_t j = 0; j < ARR_LEN-32+1; ++j)
        {
            if (ref_output[j] != output[j])
            {
                failed = 1;

                printf("Error:\tOutput buffer | got = %d | OK = %d | "
                       "input = %d | input length = %d | taps = %ld | "
                       "iter = %ld\n",
                       output[j], ref_output[j], input[j], ARR_LEN-32+1, taps, j);
            }
        }

        fail += failed;
        pass += !failed;
    }

    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");
    printf("(%lu tests passed, %lu failed)\n", pass, fail);

    return 0;
}

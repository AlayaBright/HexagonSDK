/**=============================================================================
@file
    test_vector_integrate_af.c

@brief
    Accuracy test for qhblas_hvx_vector_integrate_af function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

static range_float_t ranges0[] = {

    {-1000.0,    -100.0      },
    { -100.0,     100.0      },
    {  100.0,     1000.0     }

};

static float input_arr[ARR_LEN];
static float output[ARR_LEN];
static float ref_output[ARR_LEN];

static test_invalid_f test_data[] = {
    { input_arr, output, 0,       -1 },
    { input_arr, NULL,   ARR_LEN, -1 },
    { input_arr, NULL,   0,       -1 },
    { NULL,       output, ARR_LEN, -1 },
    { NULL,       output, 0,       -1 },
    { NULL,       NULL,   ARR_LEN, -1 },
    { NULL,       NULL,   0,       -1 },
    { input_arr, output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(range_float_t))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(test_invalid_f))

inline static void generate_ref_output(uint32_t size)
{
    float sum = 0;
    for (int32_t i = 0; i < size; ++i)
    {
        sum = sum + input_arr[i];
        ref_output[i] = (float)sum;
    }

}

int main(void)
{
    INIT_INPUT_FLOAT(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhblas_hvx_vector_integrate_af)

    TEST_INPUT_AF_T(qhblas_hvx_vector_integrate_af, 0.1)

    return 0;
}

#else /* __HEXAGON_ARCH__ >= 68 */
#include <stdio.h>
int main()
{
    printf("HVX float is not supported for this architecture.\n");
    return 0;
}
#endif /* __HEXAGON_ARCH__ >= 68 */

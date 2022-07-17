/**=============================================================================
@file
    test_cexp_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_cexp_f function.

 Copyright (c) 2019 Qualcomm Technologies Incorporated.
 All Rights Reserved. Qualcomm Proprietary and Confidential.
 =============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    {0.0f,0.0f},
    {-0.0f,0.0f},
    {0.0f,-0.0f},
    {-0.0f,-0.0f},
    {INFINITY,0.0f},
    {INFINITY,-0.0f},
    {-INFINITY,0.0f},
    {-INFINITY,-0.0f},
    {0.0f, INFINITY},
    {-0.0f,INFINITY},
    {0.0f, -INFINITY},
    {-0.0f,-INFINITY},
    {100.0f, INFINITY},
    {-100.0f, INFINITY},
    {100.0f, -INFINITY},
    {-100.0f, -INFINITY},
    {-INFINITY,2.0f},
    {-INFINITY,4.0f},
    {INFINITY,2.0f},
    {INFINITY,4.0f},
    {INFINITY,INFINITY},
    {INFINITY,-INFINITY},
    {-INFINITY,INFINITY},
    {-INFINITY,-INFINITY},
    {-INFINITY, NAN},
    {INFINITY, NAN},
    {NAN, 0.0f},
    {NAN, 1.0f},
    {NAN, INFINITY},
    {0.0f, NAN},
    {1.0f, NAN},
    {NAN, NAN},
    {0.7f, 1.2f},
    {-2.0f, -3.0f},
    {5.0f, 2.0f}
};

float complex test_expected_outputs[] =
{
    {1.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, -0.0f},
    {1.0f, -0.0f},
    {INFINITY,0.0},
    {INFINITY,-0.0},
    {0.0f, 0.0f},
    {0.0f, -0.0f},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {-0.0f, 0.0f},
    {-0.0f, -0.0f},
    {-INFINITY,INFINITY},
    {-INFINITY,-INFINITY},
    {INFINITY,NAN},
    {INFINITY,NAN},
    {0.0f, 0.0f},
    {0.0f, -0.0f},
    {0.0f, 0.0f},
    {INFINITY,NAN},
    {NAN, 0.0f},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {0.72969891f, + 1.8768962f},
    {-0.1339809f, - 0.01909852f},
    {-61.76166666f, 134.951703679f}
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_cexp_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}

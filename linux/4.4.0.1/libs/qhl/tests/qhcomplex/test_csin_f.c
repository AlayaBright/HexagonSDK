/**=============================================================================
@file
    test_csin_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_csin_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    {-2.0f, -3.0f},
    {0.7f, 1.2f},
    {NAN, NAN},
    {INFINITY, NAN},
    {-INFINITY, NAN},
    {10.0f, NAN},
    {NAN, -10.0f},
    {0.0f, NAN},
    {-0.0f, NAN},
    {NAN, 9.0f},
    {NAN, -9.0f},
    {NAN, INFINITY},
    {NAN, -INFINITY},
    {NAN, 0.0f},
    {NAN,-0.0f},
    {INFINITY,6.75f},
    {INFINITY,-6.75f},
    {-INFINITY,6.75f},
    {-INFINITY,-6.75f},
    {4.625f,INFINITY},
    { 4.625f,-INFINITY},
    {-4.625f,INFINITY},
    {-4.625f,-INFINITY},
    {INFINITY,INFINITY},
    {-INFINITY,INFINITY},
    {INFINITY,-INFINITY},
    {-INFINITY,-INFINITY},
    {INFINITY,0.0f},
    {-INFINITY,-0.0f},
    {-INFINITY,0.0f},
    {-INFINITY,-0.0f},
    {0.0f, INFINITY},
    {-0.0f, INFINITY},
    {0.0f,-INFINITY},
    {-0.0f,-INFINITY},
    {0.0f,0.0f},
    {-0.0f,0.0f},
    {0.0f,-0.0f},
    {-0.0f,-0.0f}
 };

 float complex test_expected_outputs[] =
 {
    {-9.1544991f,   4.1689069f},
    {1.16645634f, + 1.1544998f},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {0.0f,NAN},
    {-0.0f,NAN},
    {NAN,NAN},
    {NAN,NAN},
    {NAN, -INFINITY},
    {NAN, -INFINITY},
    {NAN, 0.0f},
    {NAN, 0.0f},
    {NAN,NAN},
    {NAN,NAN},
    {NAN,NAN},
    {NAN,NAN},
    {-INFINITY,-INFINITY },
    {-INFINITY,INFINITY },
    {INFINITY,-INFINITY },
    {INFINITY,INFINITY },
    {NAN,-INFINITY},
    {NAN,-INFINITY},
    {NAN,-INFINITY},
    {NAN,-INFINITY},
    {NAN,0.0},
    {NAN,0.0},
    {NAN,0.0},
    {NAN,0.0},
    {0.0f,INFINITY},
    {-0.0f,INFINITY},
    {0.0f,-INFINITY},
    {-0.0f,-INFINITY},
    {0.0f,0.0f},
    {-0.0f,0.0f},
    {0.0f,-0.0f},
    {-0.0f,-0.0f}
  };

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_csin_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}
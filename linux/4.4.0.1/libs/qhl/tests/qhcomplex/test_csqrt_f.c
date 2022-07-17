/**=============================================================================
 @file
    test_csqrt_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_csqrt_f function.

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
    {0.0f,-0.0f},
    {-0.0f,0.0f},
    {-0.0f,-0.0f},
    {-INFINITY,0.0f},
    {-INFINITY,6.0f},
    {-INFINITY,-0.0f},
    {-INFINITY,-6.0f},
    {INFINITY,0.0f},
    {INFINITY,6.0f},
    {INFINITY,-0.0f},
    {INFINITY,-6.0f},
    {0.0f, INFINITY},
    {4.0f, INFINITY},
    {INFINITY,INFINITY},
    {-0.0f, INFINITY},
    {-4.0f, INFINITY},
    {-INFINITY,INFINITY},
    {0.0f, -INFINITY},
    {4.0f, -INFINITY},
    {INFINITY,-INFINITY},
    {-0.0f, -INFINITY},
    {-4.0f, -INFINITY},
    {-INFINITY,-INFINITY},
    {-INFINITY, NAN},
    {INFINITY, NAN},
    {0.0f, NAN},
    {1.0f, NAN},
    {-0.0f, NAN},
    {-1.0f, NAN},
    {NAN,0.0},
    {NAN,8.0f},
    {NAN,-0.0f},
    {NAN,-8.0f},
    {NAN, NAN},
    {16.0f, -30.0f},
    {-1.0f, 0.0f},
    {0.0f, 2.0f},
    {119.0f, 120.0f},
    {0.7f, 1.2f},
    {2.0f, -3.0f},
    {-2.0f, 3.0f}

};

float complex test_expected_outputs[] =
{
    {0.0f, 0.0f},
    {0.0f, -0.0f},
    {0.0f, 0.0f},
    {0.0f, -0.0f},
    {0.0, INFINITY},
    {0.0, INFINITY},
    {0.0, -INFINITY},
    {0.0, -INFINITY},
    {INFINITY, 0.0},
    {INFINITY, 0.0},
    {INFINITY, -0.0},
    {INFINITY, -0.0},
    {INFINITY, INFINITY},
    {INFINITY, INFINITY},
    {INFINITY, INFINITY},
    {INFINITY, INFINITY},
    {INFINITY, INFINITY},
    {INFINITY, INFINITY},
    {INFINITY, -INFINITY},
    {INFINITY, -INFINITY},
    {INFINITY, -INFINITY},
    {INFINITY, -INFINITY},
    {INFINITY, -INFINITY},
    {INFINITY, -INFINITY},
    {NAN,-INFINITY},
    {INFINITY, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {5.0f,-3.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {12.0f, 5.0f},
    {1.022067610030026450706487883081139, + 0.58704531296356521154977678719838035},
    {1.6741492280355400404480393008490518216747086778839203667, - 0.89597747612983812471573375529004344104332419955493149324},
    {0.89597747612983812471573375529004348, 1.6741492280355400404480393008490519 }
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_csqrt_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}
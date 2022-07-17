/**=============================================================================
@file
    test_clog_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_clog_f function.

 Copyright (c) 2019 Qualcomm Technologies Incorporated.
 All Rights Reserved. Qualcomm Proprietary and Confidential.
 =============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    {-0.0f,0.0f},
    {-0.0f,-0.0f},
    {0.0f,0.0f},
    {0.0f,-0.0f},
    {-INFINITY,INFINITY},
    {-INFINITY,-INFINITY},
    {INFINITY,INFINITY},
    {INFINITY,-INFINITY},
    {0.0f, INFINITY},
    {3.0f, INFINITY},
    {-0.0f, INFINITY},
    {-3.0f, INFINITY},
    {0.0f,-INFINITY},
    {3.0f,-INFINITY},
    {-0.0f,-INFINITY},
    {-3.0f,-INFINITY},
    {-INFINITY,0.0f},
    {-INFINITY,1.0f},
    {-INFINITY,-0.0f},
    {-INFINITY,-1.0f},
    {INFINITY,0.0f},
    {INFINITY,1.0f},
    {INFINITY,-0.0f},
    {INFINITY,-1.0f},
    {INFINITY, NAN},
    {-INFINITY, NAN},
    {NAN, INFINITY},
    {NAN, -INFINITY},
    {0.0f, NAN},
    {3.0f, NAN},
    {-0.0f, NAN},
    {-3.0f, NAN},
    {NAN, 0.0f},
    {NAN, 5.0f},
    {NAN,-0.0f},
    {NAN,-5.0f},
    {NAN, NAN},
    {-2.0f, -3.0f},
    {5.0f, 2.0f},
    {30.0f, 30.0f},
    {500.0f, 0.1f},
    {-5.0f, 2.0f},
    {-30.0f, 30.0f},
    {-500.0f, 0.1f},
    {-5.0f, -2.0f},
    {-30.0f, -30.0f},
    {-500.0f, -0.1f},
    {5.0f, -2.0f},
    {30.0f, -30.0f}
};

float complex test_expected_outputs[] =
{
    {-INFINITY,M_PI},
    {-INFINITY,-M_PI},
    {-INFINITY,0.0},
    {-INFINITY,-0.0},
    {INFINITY,M_PI*(3.0/4.0)},
    {INFINITY,-M_PI*(3.0/4.0)},
    {INFINITY,M_PI*(1.0/4.0)},
    {INFINITY,-M_PI*(1.0/4.0)},
    {INFINITY,M_PI*(1.0/2.0)},
    {INFINITY,M_PI*(1.0/2.0)},
    {INFINITY,M_PI*(1.0/2.0)},
    {INFINITY,M_PI*(1.0/2.0)},
    {INFINITY,M_PI*(-1.0/2.0)},
    {INFINITY,M_PI*(-1.0/2.0)},
    {INFINITY,M_PI*(-1.0/2.0)},
    {INFINITY,M_PI*(-1.0/2.0)},
    {INFINITY,M_PI},
    {INFINITY,M_PI},
    {INFINITY,-M_PI},
    {INFINITY,-M_PI},
    {INFINITY,0.0},
    {INFINITY,0.0},
    {INFINITY,-0.0},
    {INFINITY,-0.0},
    {INFINITY,NAN},
    {INFINITY,NAN},
    {INFINITY,NAN},
    {INFINITY,NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {1.28247468f, - 2.1587990f},
    {1.68365f, 0.380506f},
    {3.747771f, 0.7853982f},
    {6.214608118f, 0.00019999f},
    {1.68365f, 2.761086f},
    {3.747771f, 2.356194f},
    {6.214608118f, 3.1413926536f},
    {1.68365f, - 2.761086f},
    {3.747771f,- 2.356194f},
    {6.214608118f, - 3.1413926536f},
    {1.68365f, - 0.380506f},
    {3.747771f, - 0.7853982f}
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_clog_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}

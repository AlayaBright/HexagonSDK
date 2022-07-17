/**=============================================================================
@file
    test_ctanh_f.c

@brief
    Testing qhcomplex_ctanh_f function for corner cases and accuracy.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    {  0.0f,      0.0f     },
    {  0.0f,     -0.0f     },
    { -0.0f,      0.0f     },
    { -0.0f,     -0.0f     },
    {  0.0f,      INFINITY },
    {  2.0f,      INFINITY },
    {  0.0f,      NAN      },
    {  5.0f,      NAN      },
    {  INFINITY,  0.0f     },
    {  INFINITY, -0.0f     },
    {  INFINITY,  INFINITY },
    {  INFINITY, -INFINITY },
    {  INFINITY,  NAN      },
    { -INFINITY,  NAN      },
    {  NAN,       0.0f     },
    {  NAN,      -0.0f     },
    {  NAN,       0.5f     },
    {  NAN,      -4.5f     },
    {  NAN,       NAN      },
    {  0.0f,      M_PI/4   },
    {  0.7f,      1.2f     },
    { -2.0f,     -3.0f     },
    { -2.0f,      0.001f   },
    { -2.0f,      10.0f    },
    { -2.0f,      95.0f    },
    { -2.0f,      300.0f   },
    {  80.0f,     80.0f    },
    { -80.0f,    -80.0f    },
    {  200.0f,    120.0f   },
    { -200.0f,   -120.0f   }
};

float complex test_expected_outputs[] =
{
    { 0.0000000f,  0.0000000f },
    { 0.0000000f, -0.0000000f },
    { -0.0000000f, 0.0000000f },
    {-0.0000000f, -0.0000000f },
    { 0.0000000f,  NAN        },
    { NAN,         NAN        },
    { 0.0000000f,  NAN        },
    { NAN,         NAN        },
    { 1.0000000f,  0.0000000f },
    { 1.0000000f, -0.0000000f },
    { 1.0000000f,  0.0000000f },
    { 1.0000000f, -0.0000000f },
    { 1.0000000f, -0.0000000f },
    { -1.0000000f,-0.0000000f },
    { NAN,         0.0000000f },
    { NAN,        -0.0000000f },
    { NAN,         NAN        },
    { NAN,         NAN        },
    { NAN,         NAN        },
    { 0.0000000f,  1.0000001f },
    { 1.3472198f,  0.4778641f },
    {-0.9653859f,  0.0098844f },
    {-0.9640276f,  0.0000707f },
    {-0.9846157f,  0.0329389f },
    {-0.9969088f,  0.0364499f },
    {-1.0372763f,  0.0016794f },
    { 1.0000000f,  0.0000000f },
    {-1.0000000f, -0.0000000f },
    { 1.0000000f,  0.0000000f },
    {-1.0000000f, -0.0000000f }
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_ctanh_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}
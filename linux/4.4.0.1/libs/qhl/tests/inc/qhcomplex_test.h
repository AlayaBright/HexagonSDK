/**=============================================================================
@file
    qhcomplex_test.h

@brief
    Macros and datatypes used in QHCOMPLEX tests.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHCOMPLEX_TEST_H
#define _QHCOMPLEX_TEST_H

#include "qhmath.h"
#include "qhcomplex.h"
#include "qhl_common.h"

/* Macro for testing accuracy of complex function that returns complex float value and takes 1 argument */
#define TEST_CMPLX_FUN_CF1(FNAME, TEST_INPUTS, TEST_INPUTS_CNT, TEST_EXPECTED_OUTPUTS)                                \
    uint32_t pass = 0;                                                                                                \
    uint32_t fail = 0;                                                                                                \
                                                                                                                      \
    printf("Accuracy test for %s function:\n", #FNAME);                                                               \
                                                                                                                      \
    for (uint32_t i = 0; i < TEST_INPUTS_CNT; i++)                                                                    \
    {                                                                                                                 \
        float complex x = TEST_INPUTS[i]; /* function input */                                                        \
        float complex r = FNAME(x); /* function output */                                                             \
        float complex t = TEST_EXPECTED_OUTPUTS[i]; /* expected output */                                             \
                                                                                                                      \
        if ((isequal(qhcomplex_creal_f(r),qhcomplex_creal_f(t)) && (isequal(qhcomplex_cimag_f(r),qhcomplex_cimag_f(t)))) == 0)            \
        {                                                                                                             \
            printf("Error:\tx = %.7f + %.7f*i | %s(x) = %.7f + %.7f*i | OK = %.7f + %.7f*i\n",                        \
                   qhcomplex_creal_f(x),qhcomplex_cimag_f(x), #FNAME,                                                           \
                   qhcomplex_creal_f(r),qhcomplex_cimag_f(r),qhcomplex_creal_f(t),qhcomplex_cimag_f(t));                                  \
                                                                                                                      \
            fail+=1;                                                                                                  \
        }                                                                                                             \
        else                                                                                                          \
        {                                                                                                             \
            pass+=1;                                                                                                  \
        }                                                                                                             \
    }                                                                                                                 \
                                                                                                                      \
    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                 \
    printf("(%lu tests passed, %lu failed)\n", pass, fail);

/* Macro for testing accuracy of complex function that returns float value and takes 1 argument */
#define TEST_CMPLX_FUN_F1(FNAME, TEST_INPUTS, TEST_INPUTS_CNT, TEST_EXPECTED_OUTPUTS)                                 \
    uint32_t pass = 0;                                                                                                \
    uint32_t fail = 0;                                                                                                \
                                                                                                                      \
    printf("Accuracy test for %s function:\n", #FNAME);                                                               \
                                                                                                                      \
    for (uint32_t i = 0; i < TEST_INPUTS_CNT; i++)                                                                    \
    {                                                                                                                 \
        float complex x = TEST_INPUTS[i]; /* function input */                                                        \
        float r = FNAME(x); /* function output */                                                                     \
        float t = TEST_EXPECTED_OUTPUTS[i]; /* expected output */                                                     \
                                                                                                                      \
        if (isequal(r, t) == 0)                                                                                       \
        {                                                                                                             \
            printf("Error:\tx = %.7f + %.7f*i | %s(x) = %.7f | OK = %.7f\n",                                          \
                    qhcomplex_creal_f(x), qhcomplex_cimag_f(x), #FNAME, r, t);                                                  \
                                                                                                                      \
            fail+=1;                                                                                                  \
        }                                                                                                             \
        else                                                                                                          \
        {                                                                                                             \
            pass+=1;                                                                                                  \
        }                                                                                                             \
    }                                                                                                                 \
                                                                                                                      \
    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                 \
    printf("(%lu tests passed, %lu failed)\n", pass, fail);

/* Macro for testing accuracy of complex function that returns complex float value and takes 2 arguments */
#define TEST_CMPLX_FUN_CF2(FNAME, TEST_INPUTS1, TEST_INPUTS2, TEST_INPUTS_CNT, TEST_EXPECTED_OUTPUTS)                 \
    uint32_t pass = 0;                                                                                                \
    uint32_t fail = 0;                                                                                                \
                                                                                                                      \
    printf("Accuracy test for %s function:\n", #FNAME);                                                               \
                                                                                                                      \
    for (uint32_t i = 0; i < TEST_INPUTS_CNT; i++)                                                                    \
    {                                                                                                                 \
        float complex x = TEST_INPUTS1[i]; /* function input, 1st parameter */                                        \
        float complex y = TEST_INPUTS2[i]; /* function input, 2nd parameter */                                        \
        float complex r = FNAME(x, y); /* function output */                                                          \
        float complex t = TEST_EXPECTED_OUTPUTS[i]; /* expected output */                                             \
                                                                                                                      \
        if ((isequal(qhcomplex_creal_f(r),qhcomplex_creal_f(t)) && (isequal(qhcomplex_cimag_f(r),qhcomplex_cimag_f(t)))) == 0)            \
        {                                                                                                             \
            printf("Error:\tx = %.7f + %.7f*i | y = %.7f + %.7f*i | %s(x,y) = %.7f + %.7f*i | OK = %.7f + %.7f*i\n",  \
                   qhcomplex_creal_f(x), qhcomplex_cimag_f(x), qhcomplex_creal_f(y),qhcomplex_cimag_f(y), #FNAME,                         \
                   qhcomplex_creal_f(r), qhcomplex_cimag_f(r),qhcomplex_creal_f(t), qhcomplex_cimag_f(t));                                \
                                                                                                                      \
            fail+=1;                                                                                                  \
        }                                                                                                             \
        else                                                                                                          \
        {                                                                                                             \
            pass+=1;                                                                                                  \
        }                                                                                                             \
    }                                                                                                                 \
                                                                                                                      \
    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");                                                                 \
    printf("(%lu tests passed, %lu failed)\n", pass, fail);

#endif //_QHCOMPLEX_TEST_H

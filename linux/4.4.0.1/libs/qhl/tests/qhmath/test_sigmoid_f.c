/**=============================================================================
  @file
     test_singmoid_f.c

  @brief
     Accuracy test for qhmath_sigmoid_f function

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs[] =
    {
    -INFINITY,
    -0.0,
    +0.0,
    INFINITY,
    NAN,
    -5.0,
    -4.9,
    -4.8,
    -4.7,
    -4.6,
    -4.5,
    -4.4,
    -4.3,
    -4.2,
    -4.1,
    -4.0,
    -3.9,
    -3.8,
    -3.7,
    -3.6,
    -3.5,
    -3.4,
    -3.3,
    -3.2,
    -3.1,
    -3.0,
    -2.9,
    -2.8,
    -2.7,
    -2.6,
    -2.5,
    -2.4,
    -2.3,
    -2.2,
    -2.1,
    -2.0,
    -1.9,
    -1.8,
    -1.7,
    -1.6,
    -1.5,
    -1.4,
    -1.3,
    -1.2,
    -1.1,
    -1.0,
    -0.9,
    -0.8,
    -0.7,
    -0.6,
    -0.5,
    -0.4,
    -0.3,
    -0.2,
    -0.1,
    0.0,
    0.1,
    0.2,
    0.3,
    0.4,
    0.5,
    0.6,
    0.7,
    0.8,
    0.9,
    1.0,
    1.1,
    1.2,
    1.3,
    1.4,
    1.5,
    1.6,
    1.7,
    1.8,
    1.9,
    2.0,
    2.1,
    2.2,
    2.3,
    2.4,
    2.5,
    2.6,
    2.7,
    2.8,
    2.9,
    3.0,
    3.1,
    3.2,
    3.3,
    3.4,
    3.5,
    3.6,
    3.7,
    3.8,
    3.9,
    4.0,
    4.1,
    4.2,
    4.3,
    4.4,
    4.5,
    4.6,
    4.7,
    4.8,
    4.9,
    5.0,
    };

float test_expected_outputs[] =
    {
    +0.0,
    0.5,
    0.5,
    +1.0,
    NAN,
    0.006692850924,
    0.007391541345,
    0.008162571153,
    0.009013298653,
    0.009951801867,
    0.01098694263,
    0.01212843498,
    0.01338691783,
    0.01477403169,
    0.01630249937,
    0.01798620996,
    0.01984030573,
    0.02188127094,
    0.02412702142,
    0.02659699358,
    0.02931223075,
    0.0322954647,
    0.03557118927,
    0.0391657228,
    0.04310725494,
    0.04742587318,
    0.05215356308,
    0.0573241759,
    0.06297335606,
    0.06913842034,
    0.07585818002,
    0.08317269649,
    0.09112296101,
    0.09975048912,
    0.1090968212,
    0.119202922,
    0.1301084744,
    0.1418510649,
    0.1544652651,
    0.1679816149,
    0.1824255238,
    0.1978161114,
    0.214165017,
    0.2314752165,
    0.2497398944,
    0.2689414214,
    0.2890504974,
    0.3100255189,
    0.3318122278,
    0.3543436938,
    0.3775406688,
    0.4013123399,
    0.4255574832,
    0.4501660027,
    0.4750208125,
    0.5,
    0.5249791875,
    0.5498339973,
    0.5744425168,
    0.5986876601,
    0.6224593312,
    0.6456563062,
    0.6681877722,
    0.6899744811,
    0.7109495026,
    0.7310585786,
    0.7502601056,
    0.7685247835,
    0.785834983,
    0.8021838885,
    0.8175744762,
    0.8320183851,
    0.8455347349,
    0.8581489351,
    0.8698915256,
    0.880797078,
    0.8909031788,
    0.9002495109,
    0.908877039,
    0.9168273035,
    0.92414182,
    0.9308615797,
    0.9370266439,
    0.9426758241,
    0.9478464369,
    0.9525741268,
    0.9568927451,
    0.9608342772,
    0.9644288107,
    0.9677045353,
    0.9706877693,
    0.9734030064,
    0.9758729786,
    0.9781187291,
    0.9801596943,
    0.98201379,
    0.9836975006,
    0.9852259683,
    0.9866130822,
    0.987871565,
    0.9890130574,
    0.9900481981,
    0.9909867013,
    0.9918374288,
    0.9926084587,
    0.9933071491,
    };
#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_FUN_F1(qhmath_sigmoid_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
    return 0;
}

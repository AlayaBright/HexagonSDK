/**=============================================================================
@file
   test_ahf_convert_soft_asf.c

@brief
    Accuracy test for qhmath_hvx_ahf_convert_soft_asf function

Copyright (c) 2020 - 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "hexagon_protos.h"
#include "hexagon_types.h"

#include "qhmath_hvx.h"
#include "qhmath_hvx_cvt.h"
#include <math.h>

#define SIZE (2560*64)

#define IN_TYPE  float
#define OUT_TYPE __fp16

int main() {
    int i;
    OUT_TYPE *hf_ref;
    OUT_TYPE *hf_test;
    IN_TYPE *arg;

    arg   =  (IN_TYPE *) memalign(128,sizeof(IN_TYPE) * SIZE);

    for(i = 0; i < SIZE; i++) {
        float lo =  -65504.0f;
        float hi = 65504.0f;
        arg[i] = (float)(lo + (hi - lo) * ((float)rand() / (float)RAND_MAX));
    }

    hf_ref = (OUT_TYPE *) memalign(128, sizeof(OUT_TYPE) * SIZE);
    hf_test = (OUT_TYPE *) memalign(128, sizeof(OUT_TYPE) * SIZE);

    // Use the compiler's built-in function to generate the reference data
    for(i = 0; i < SIZE; i++) {
        hf_ref[i] = (__fp16)arg[i];
    }

    qhmath_hvx_ahf_convert_soft_asf(arg, hf_test, SIZE);

    // compare bit accuracy of qfloat conversion to int16
    for(i = 0; i < SIZE; i++) {
        if (hf_ref[i] != hf_test[i]) {
            printf("Conversion: sf -> hf mismatches with reference implementation, FAIL!\n");
            break;
        }
    }
    printf("Conversion: sf -> hf  matches with reference implementation, PASS! \n");
    /*======================================================================*/
    free(arg);
    free(hf_test);
    free(hf_ref);

    return 0;
}


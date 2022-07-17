/**=============================================================================
@file
    test_aqf32_convert_aw.c

@brief
    Accuracy test for qhmath_hvx_aqf32_convert_aw function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>
#include "hexagon_protos.h"
#include "hexagon_types.h"

#include "qhmath_hvx.h"
#include "qhmath_hvx_cvt.h"
#include <math.h>


#define dfloat double
#define SIZE (65536 + 100) //every element in INT_TYPE
#define DEBUG 0

#define IN_TYPE  int32_t
#define OUT_TYPE int32_t



int main() {

    int i, j;
    qf16_t *qf16_ref;
    qf32_t *qf32_ref, *qf32_test;
    IN_TYPE *arg;
    int k =0;

    arg   =  (IN_TYPE *) memalign(128,sizeof(IN_TYPE) * SIZE);
    // Input Test data covering all the values that fit into 9-bit(mantissa) + 1 bit(Extra accuracy with full normalization)
    k = -512;
    for(j = 0, i = 0; i < SIZE; i++) {
        arg[j++] = k++;
        if(k == 512) {
            k = -512;
        }
    }

    qf16_ref = (qf16_t *) memalign(128, sizeof(qf16_t) * SIZE);
    qf32_ref = (qf32_t *) memalign(128, sizeof(qf32_t) * SIZE);
    qf32_test = (qf32_t *) memalign(128, sizeof(qf32_t) * SIZE);

    // Generate the Qfloat data using c reference code
    qhmath_hvx_aqf32_convert_aw_ref(arg, qf32_ref, SIZE);

    //Generate qf16_t from input using ref
    qhmath_hvx_aqf16_convert_aw_ref (arg, qf16_ref,  SIZE);

    // Convert qf16 t0 qf32
    qhmath_hvx_aqf32_convert_aqf16 (qf16_ref, qf32_test, SIZE);

    //compare bit accuracy  of qfloat16 conversion to qfloat32

    if(qhmath_hvx_cmp_qf32(qf32_ref, qf32_test, SIZE) < 14.0 ) {
       printf("Conversion: qf16 -> qf32 mismatches with reference implementation, FAIL!\n");
    } else {
       printf("Conversion: qf16 -> qf32  matches with reference implementation, PASS! \n");
    }
/*======================================================================*/
    free(arg);
    free(qf32_ref);
    free(qf32_test);
    free(qf16_ref);

    return(0);
}

#else // __HEXAGON_ARCH__ >= 68
#include <stdio.h>
int main()
{
    printf("HVX float is not supported for this architecture.\n");
    return 0;
}

#endif // __HEXAGON_ARCH__ >= 68

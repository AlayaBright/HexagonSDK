/**=============================================================================
@file
   test_auw_convert_aqf16.c

@brief
    Accuracy test for qhmath_hvx_auw_convert_aqf16 function

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

#define IN_TYPE  uint32_t
#define OUT_TYPE uint32_t


int main() {

    int i, j;
    qf16_t *qf_ref;
    OUT_TYPE *int_ref;
    OUT_TYPE *int_test;
    IN_TYPE *arg;
    int k =0;

    arg   =  (IN_TYPE *) memalign(128,sizeof(IN_TYPE) * SIZE);
    // Input Test data covering all the values that fit into 9-bit(mantissa) + 1 bit(Extra accuracy with full normalization)
    k = 0;
    for(j = 0, i = 0; i < SIZE; i++) {
        arg[j++] = k++;
        if(k == 1024) {
            k = 0;
        }
    }

    qf_ref = (qf16_t *) memalign(128, sizeof(qf16_t) * SIZE);
    int_test = (OUT_TYPE *) memalign(128, sizeof(OUT_TYPE) * SIZE);
    int_ref = (OUT_TYPE *) memalign(128, sizeof(OUT_TYPE) * SIZE);

    // Generate the Qfloat data using c reference code
    qhmath_hvx_aqf16_convert_auw_ref(arg, qf_ref, SIZE);

    // Use the Qfloat data to convert to uint32 using intrinsic code and reference code

    qhmath_hvx_auw_convert_aqf16_ref (qf_ref, int_ref, SIZE);
    qhmath_hvx_auw_convert_aqf16 (qf_ref, int_test, SIZE);

    //compare bit accuracy  of qfloat conversion to uint32

    if(qhmath_hvx_cmp_uw(int_ref, int_test, SIZE, 1)) {
       printf("Conversion: qf16 -> uint32 mismatches with reference implementation, FAIL!\n");
    } else {
       printf("Conversion: qf16 -> uint32  matches with reference implementation, PASS! \n");
    }
/*======================================================================*/
    free(arg);
    free(int_test);
    free(int_ref);
    free(qf_ref);

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

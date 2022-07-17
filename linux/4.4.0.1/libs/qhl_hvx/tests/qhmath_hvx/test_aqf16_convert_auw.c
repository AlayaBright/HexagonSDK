/**=============================================================================
@file
   test_aqf16_convert_auw.c

@brief
    Accuracy test for qhmath_hvx_aqf16_convert_auw function

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
#define SIZE (65536 + 100)  //every element in INT_TYPE
#define DEBUG 0

#define IN_TYPE  uint32_t
#define OUT_TYPE uint32_t


int main() {

    int i, j;
    qf16_t *qf_ref;
    qf16_t *qf_test;
    IN_TYPE *arg;
    int k;

    arg   =  (IN_TYPE *) memalign(128, sizeof(IN_TYPE) * SIZE);
    // Input Test data covering all the values that fit into 9-bit mantisaa
    k = 0;
    for(j = 0, i = 0; i < SIZE; i++) {
          arg[j++] = k++;
         if (k == 512) {
            k = 0;
         }
    }

    qf_ref =  (qf16_t *) memalign(128, sizeof(qf16_t) * SIZE);
    qf_test = (qf16_t *) memalign(128, sizeof(qf16_t) * SIZE);

    // Use the Qfloat data to convert to uint32 using intrinsic code and reference code

    qhmath_hvx_aqf16_convert_auw_ref(arg, qf_ref, SIZE);
    qhmath_hvx_aqf16_convert_auw(arg, qf_test, SIZE);

    //compare uint32 to qf16 conversion accuracy

    if(qhmath_hvx_cmp_qf16(qf_ref, qf_test, SIZE) < 10.0) {
       printf("Conversion: uint32 -> qf16 mismatches with reference implementation, FAIL!\n");
    } else {
       printf("Conversion: uint32 -> qf16 matches with reference implementation, PASS! \n");
    }
/*======================================================================*/
    free(arg);
    free(qf_ref);
    free(qf_test);

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

/**=============================================================================
@file
   test_aqf16_convert_auh.c

@brief
    Accuracy test for qhmath_hvx_aqf16_convert_auh function

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

#define IN_TYPE  uint16_t
#define OUT_TYPE uint16_t


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

    // Use the uint16 data to convert to qf16 using intrinsic code and reference code

    qhmath_hvx_aqf16_convert_auh_ref(arg, qf_ref, SIZE);
    qhmath_hvx_aqf16_convert_auh(arg, qf_test, SIZE);

    //compare bit accuracy  of uint16 conversion to qfloat16

    if(qhmath_hvx_cmp_qf16(qf_ref, qf_test, SIZE) < 10.0) {
       printf("Conversion: uint16 -> qf16 mismatches with reference implementation, FAIL!\n");
    } else {
       printf("Conversion: uint16 -> qf16 matches with reference implementation, PASS! \n");
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

/**=============================================================================
@file
    test_aqf32_convert_auw.c

@brief
    Accuracy test for qhmath_hvx_aqf32_convert_auw function

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
#define SIZE ( 65536 + 100)  //every element in INT_TYPE
#define DEBUG 0

#define IN_TYPE  uint32_t
#define OUT_TYPE uint32_t


int main() {

    int i, j;
    qf32_t *qf_ref;
    qf32_t *qf_test;
    IN_TYPE *arg;

    arg   =  (IN_TYPE *) memalign(128, sizeof(IN_TYPE) * SIZE);
    for(j = 0, i = 0; i < SIZE; i++) {
          arg[j++] = (i << 8) ^ (0xffff & rand());
    }

    qf_ref = (qf32_t *) memalign(128, sizeof(qf32_t) * SIZE);
    qf_test = (qf32_t *) memalign(128, sizeof(qf32_t) * SIZE);


    // Use the uint32 data to convert to qfloat using intrinsic code and reference code

    qhmath_hvx_aqf32_convert_auw_ref(arg, qf_ref, SIZE);
    qhmath_hvx_aqf32_convert_auw(arg, qf_test, SIZE);

    //compare bit accuracy  of uint32 conversion to qf32

    if(qhmath_hvx_cmp_qf32(qf_ref, qf_test, SIZE) < 23.0) {
       printf("Conversion: uint32 -> qf32 mismatches with reference implementation, FAIL!\n");
    } else {
       printf("Conversion: uint32 -> qf32 matches with reference implementation, PASS! \n");
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

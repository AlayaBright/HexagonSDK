/**=============================================================================
@file
    test_ab_convert_aqf32.c

@brief
    Accuracy test for qhmath_hvx_ab_convert_aqf32 function

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
#define SIZE ( 65536 + 100 )  //every element in INT_TYPE
#define DEBUG 0

#define IN_TYPE  int8_t
#define OUT_TYPE int8_t


int main() {

    int i, j;
    qf32_t *qf_ref;
    OUT_TYPE *int_ref;
    OUT_TYPE *int_test;
    IN_TYPE *arg;

    arg   =  (IN_TYPE *) memalign(128,sizeof(IN_TYPE) * SIZE);

    for(j = 0, i = 0; i < SIZE; i++) {
        arg[j++] = (i << 8) ^ (0xffff & rand());
    }
    qf_ref = (qf32_t *) memalign(128, sizeof(qf32_t) * SIZE);
    int_test = (OUT_TYPE *) memalign(128, sizeof(OUT_TYPE) * SIZE);
    int_ref = (OUT_TYPE *) memalign(128, sizeof(OUT_TYPE) * SIZE);

    // Generate the Qfloat data using c reference code
    qhmath_hvx_aqf32_convert_ab_ref(arg, qf_ref, SIZE);

    // Use the Qfloat data to convert to int16 using intrinsic code and reference code

    qhmath_hvx_ab_convert_aqf32_ref(qf_ref, int_ref, SIZE);
    qhmath_hvx_ab_convert_aqf32(qf_ref, int_test, SIZE);

    //compare bit accuracy  of qfloat conversion to int8

    if(qhmath_hvx_cmp_b(int_ref, int_test, SIZE, 1)) {
       printf("Conversion: qf32 -> int8 mismatches with reference implementation, FAIL!\n");
    } else {
       printf("Conversion: qf32 -> int8  matches with reference implementation, PASS! \n");
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

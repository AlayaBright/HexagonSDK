/**=============================================================================
@file
    test_aqf32_convert_asf.c

@brief
    Accuracy test for qhmath_hvx_aqf32_convert_asf function

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

#define IN_TYPE  float
#define OUT_TYPE float


int main() {

    int i, j;
    qf32_t *qf_test;
    IN_TYPE *arg, *test_dat;

    arg   =  (IN_TYPE *) memalign(128, sizeof(IN_TYPE) * SIZE);
    test_dat   =  (IN_TYPE *) memalign(128, sizeof(IN_TYPE) * SIZE);
    for(j = 0, i = 0; i < SIZE; i++) {
          arg[j++] = (i << 8) ^ (0xffff & rand());
    }

    qf_test = (qf32_t *) memalign(128, sizeof(qf32_t) * SIZE);


    // Use the float data to convert to qfloat

    qhmath_hvx_aqf32_convert_asf(arg, qf_test, SIZE);
    // Use the qfloat data to convert to float
    qhmath_hvx_asf_convert_aqf32(qf_test, test_dat, SIZE);
    int err_cnt = 0;

    for(j = 0; j < SIZE; j++) {
        if( (arg[j] - test_dat[j]) >= 1) {
            printf("mismatch at %d: Actual = %f and calculated = %f \n", j, arg[j], test_dat[j]);
            err_cnt++;
        }
    }

    if(err_cnt > 0) {
       printf("Conversion: float -> qf32 mismatches with reference implementation, FAIL!\n");
       printf("Conversion: qf32 -> float mismatches with reference implementation, FAIL!\n");
    } else {
       printf("Conversion: float -> qf32 matches with reference implementation, PASS! \n");
       printf("Conversion: qf32 -> float matches with reference implementation, PASS! \n");
    }
/*======================================================================*/
    free(arg);
    free(qf_test);
    free(test_dat);

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

/**=============================================================================
@file
    test_matrix_inverse2x2_ah.c

@brief
    Accuracy test for __qhblas_matrix_inverse2x2_ah function

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/
#include <stdio.h>
#include <stdlib.h>
#include <hexagon_protos.h>

#include "qhmath.h"
#include "qhblas.h"
#include "qhblas_test.h"

#undef MATRIX_N
#define MATRIX_N 2

#undef TOL
#define TOL 107374182                          // representing 0.1

range_uint16_t ranges0[] = {
    {INT16_MIN, -1000},
    {-1000, -100},
    {-100, 100},
    {100, 1000},
    {1000, INT16_MAX}
};

int16_t input_data[MATRIX_N*MATRIX_N];
int32_t output_data[MATRIX_N*MATRIX_N];
int32_t check_data[MATRIX_N*MATRIX_N];
int32_t number_of_errors;
int32_t check_flag;
int32_t w;
int32_t total_number_of_errors;
int32_t number_singular;


#define RANGE_CNT0         (sizeof(ranges0) / sizeof(range_uint16_t))

int main(void)
{
  total_number_of_errors = 0;
  number_singular = 0;
  for(w = 0; w < NUMBER_OF_MATRIX_ITERATIONS; w++)
  {
    // // initialize matrix contents
    TEST_AH3_INIT_INPUT(0, MATRIX_N*MATRIX_N);
    //
    check_flag = __qhblas_matrix_inverse2x2_ah(input_data, output_data);

    if(check_flag == 0)
    {
        MATRIX_MULTIPLY(input_data, output_data, check_data, MATRIX_N);

        VALIDATE_MATRIX_INVERSE_H(check_data, MATRIX_N, TOL);

        total_number_of_errors = total_number_of_errors + number_of_errors;
    }
    else
    {
        number_singular = number_singular + 1;
    }
  }
  if (number_singular == 0)
   {
     w = w + 1;
     for (int32_t i = 0; i < MATRIX_N*MATRIX_N; i++)
     {
       input_data[i] = (int16_t)(0.5 * 32768) ;
     }
     if (__qhblas_matrix_inverse2x2_ah(input_data, output_data) == -1)
     {
       number_singular = number_singular + 1;
     }
  }
  printf("Number of random matrixes [%d] | Matrix_size[%d x %d]|%s| Total number of %d errors| tolerance %.7d | number of singular matrix %d \n",w,MATRIX_N, MATRIX_N, (total_number_of_errors == 0) ? "PASSED!" : "FAILED!", total_number_of_errors, TOL, number_singular );

    return 0;
}

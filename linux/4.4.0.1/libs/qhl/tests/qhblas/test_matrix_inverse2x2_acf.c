/**=============================================================================
@file
    test_matrix_inverse2x2_acf.c

@brief
    Accuracy test for __qhblas_matrix_inverse2x2_acf function

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/
#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas.h"
#include "qhblas_test.h"

#undef MATRIX_N
#define MATRIX_N 2

range_float_t ranges0[] = {
    {-1000.0, -100.0},
    {-100.0, -1.0},
    {-1.0, 1.0},
    {1.0, 100.0},
    {100, 1000.0}
};

complex float input_data[MATRIX_N*MATRIX_N];
complex float output_data[MATRIX_N*MATRIX_N];
complex float check_data[MATRIX_N*MATRIX_N];
int32_t number_of_errors;
int32_t check_flag;
int32_t w;
int32_t total_number_of_errors;
int32_t number_singular;


#define RANGE_CNT0         (sizeof(ranges0) / sizeof(range_float_t))

int main(void)
{
  total_number_of_errors = 0;
  number_singular = 0;
  for(w = 0; w < NUMBER_OF_MATRIX_ITERATIONS; w++)
  {
    // // initialize matrix contents
    TEST_AF3_INIT_INPUT(0, MATRIX_N*MATRIX_N);
    //
    check_flag = __qhblas_matrix_inverse2x2_acf(input_data, output_data);

    if(check_flag == 0)
    {
        MATRIX_MULTIPLY(input_data, output_data, check_data, MATRIX_N);
        VALIDATE_MATRIX_INVERSE_F(check_data, MATRIX_N, TOL);
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
       input_data[i] = 1.0 + 0.0*I ;
     }
     if (__qhblas_matrix_inverse2x2_acf(input_data, output_data) == -1)
     {
       number_singular = number_singular + 1;
     }
  }
  printf("Number of random matrixes [%d] | Matrix_size[%d x %d]|%s| Total number of %d errors| tolerance %f | number of singular matrix %d \n",w,MATRIX_N, MATRIX_N, (total_number_of_errors == 0) ? "PASSED!" : "FAILED!", total_number_of_errors, TOL, number_singular );

    return 0;
}

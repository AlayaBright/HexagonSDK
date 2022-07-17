/**=============================================================================
@file
    test_matrix_integrate_aw.c

@brief
    Accuracy test for qhblas_hvx_matrix_integrate_aw function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

void create_input_data(int32_t* input_data, uint32_t m, uint32_t n)
{
    for (int32_t i = 0; i < m; i++) {
        for (int32_t j = 0; j < n; j++) {
            int32_t sign = ((rand()%2) == 0) ? 1 : -1;
            input_data[i*n+j] = (int32_t)(sign * (rand()>>15));
        }
    }
}

void create_output_data(int32_t* input_data, int32_t* output_data, uint32_t m, uint32_t n)
{
    int32_t sum = 0;
    for (uint32_t j = 0; j < n; j++) {
        sum += input_data[j];
        output_data[j] = sum;
    }

        for (uint32_t i = 1; i < m; i++)
        {
            sum = 0;
            for (uint32_t j = 0; j < n; j++) {
                sum += input_data[i*n+j];
                output_data[i*n+j] = (int32_t)(sum+output_data[(i-1)*n+j]);
            }
        }

}

int main(void)
{
    int32_t* output_data_hvx;
    int32_t* input_data;
    int32_t* output_data;
    int32_t passed = 0;
    int32_t failed = 0;
    uint32_t m,n;
    uint32_t marray[5] = {10, 128, 512, 1024, 1920};
    uint32_t narray[5] = {10, 200, 512, 780,  1440};

    printf("Test for qhblas_hvx_matrix_integrate_aw function:\n");

    for (int q = 0; q < 5; q++) {
        for (int p = 0; p < 5; p++) {
            m = marray[q];
            n = narray[p];

            output_data_hvx = malloc(m*n*sizeof(int32_t));
            input_data = malloc(m*n*sizeof(int32_t));
            output_data = malloc(m*n*sizeof(int32_t));
            create_input_data(input_data, m, n);
            create_output_data(input_data, output_data, m, n);
            qhblas_hvx_matrix_integrate_aw(input_data, output_data_hvx, m, n);

            passed += n*m;
            for (int32_t i = 0; i < m; i++) {
                for (int32_t j = 0; j < n; j++) {
                    if (output_data_hvx[i*n+j] != output_data[i*n+j])
                    {
                        printf("error on element i, j: %ld %ld output_data_hvx: %ld output_data %ld\n", i,j, output_data_hvx[i*n+j], output_data[i*n+j]);                        failed+=1;
                        passed+=-1;
                    }
                }

            }
            free(input_data);
            free(output_data);
            free(output_data_hvx);
    }
}

    printf("TEST MATRIX integrate 32b->32b %s | %ld passed | %ld failed\n", ((failed == 0) ? "PASSED!" : "FAILED!"), passed ,failed );
    return 0;
}

/**=============================================================================
@file
    test_matrix_transpose_aqf32.c

@brief
    Accuracy test for qhblas_hvx_matrix_transpose_aqf32 function

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"
#include "qhmath_hvx.h"
#include <hvx_internal.h>

void create_input_data(float* input_data, uint32_t m, uint32_t n)
{
    for (int32_t i = 0; i < m; i++) {
        for (int32_t j = 0; j < n; j++) {
            input_data[i*n+j] = j;
        }
    }
}

void create_output_data(float* input_data, float* output_data, uint32_t m, uint32_t n)
{
    for(int i=0; i<m; i++)
    {
        for(int j=0; j<n; j++)
        {
            *(output_data+m*j + i) = *(input_data+n*i+j);
        }
    }
}

static float convert_qf32_to_float_helper(qf32_t qf)
{
    uint32_t  __attribute__((aligned(128))) temp_in[32];
    float __attribute__((aligned(128))) temp_out[32];

    temp_in[0] = *(unsigned int*)&qf;
    HVX_Vector in = *(HVX_Vector*)temp_in;

    HVX_Vector out = Q6_Vsf_equals_Vqf32(in);

    *(HVX_Vector *)temp_out = out;
    return temp_out[0];
}

static qf32_t convert_float_to_qf32_helper(float sf)
{
    uint32_t  __attribute__((aligned(128))) temp_in[32];
    qf32_t __attribute__((aligned(128))) temp_out[32];

    temp_in[0] = *(unsigned int*)&sf;
    HVX_Vector in = *(HVX_Vector*)temp_in;

    HVX_Vector out = Q6_Vqf32_vadd_VsfVsf(in, Q6_V_vzero());

    *(HVX_Vector *)temp_out = out;
    return temp_out[0];
}

static void convert_qf32_to_float(qf32_t *in, float *out, int m, int n)
{
    for(int i=0; i<m; i++)
    {
        for(int j=0; j<n; j++)
        {
            out[i*n+j] = convert_qf32_to_float_helper(in[i*n+j]);
        }
    }
}

static void convert_float_to_qf32(float *in, qf32_t *out, int m, int n)
{
    for(int i=0; i<m; i++)
    {
        for(int j=0; j<n; j++)
        {
            out[i*n+j] = convert_float_to_qf32_helper(in[i*n+j]);
        }
    }
}

int main(void)
{
    qf32_t* input_data_qf32;
    float* input_data;
    qf32_t* output_data_hvx_qf32;
    float* output_data_hvx;
    float* output_data;
    float diff;
    float epsilon = 0.5;
    int32_t passed = 0;
    int32_t failed = 0;
    uint32_t m,n;

    uint32_t marray[4] = {1, 31, 32, 33};
    uint32_t narray[4] = {1, 15, 32, 128};
    printf("Test for qhblas_hvx_matrix_transpose_aqf32 function:\n");

    for (int q = 0; q < 4; q++) {
        for (int p = 0; p < 4; p++) {
            m = marray[q];
            n = narray[p];


            input_data_qf32 = malloc(m*n*sizeof(qf32_t));
            input_data = malloc(m*n*sizeof(float));
            output_data_hvx_qf32 = malloc( m*n*sizeof(qf32_t));
            output_data_hvx = malloc( m*n*sizeof(float));
            output_data = malloc( m*n*sizeof(float));

            create_input_data(input_data, m, n);
            create_output_data(input_data, output_data, m, n);

            convert_float_to_qf32(input_data, input_data_qf32, m, n);
            qhblas_hvx_matrix_transpose_aqf32(input_data_qf32, output_data_hvx_qf32, m, n);

            // Convert Output from qf32 to IEEE float
            convert_qf32_to_float(output_data_hvx_qf32, output_data_hvx, m, n);

            for (int32_t i = 0; i < m; i++) {
                for (int32_t j = 0; j < n; j++) {
                    diff = output_data_hvx[i*n+j] - output_data[i*n+j];
                    diff = (diff<0.0) ? -diff : diff;
                    if (diff > epsilon )
                    {
                        printf("error on element i, j: %ld %ld output_data_hvx: %f output_data %f\n", i,j, output_data_hvx[i*n+j], output_data[i*n+j]);
                        failed+=1;
                    }
                    else {
                        passed += 1;
                    }
                }
            }
            free(input_data);
            free(input_data_qf32);
            free(output_data);
            free(output_data_hvx);
            free(output_data_hvx_qf32);
        }
    }

    printf("TEST MATRIX transpose qf32->qf32 %s | %ld passed | %ld failed\n", ((failed == 0) ? "PASSED!" : "FAILED!"), passed ,failed );
    return 0;
}

#else /* __HEXAGON_ARCH__ >= 68 */
#include <stdio.h>
int main()
{
    printf("HVX qf32_t is not supported for this architecture.\n");
    return 0;
}
#endif /* __HEXAGON_ARCH__ >= 68 */

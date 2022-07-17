/**=============================================================================
@file
    test_matrix_integrate_ahf.c

@brief
    Accuracy test for qhblas_hvx_matrix_integrate_ahf function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"


#define BLOCK_SIZE       (8*1024/128)  // vector chunks
#define L2FETCH_AHEAD    (BLOCK_SIZE)


void create_input_data(__fp16* input_data, uint32_t m, uint32_t n)
{
    for (int32_t i = 0; i < m; i++) {
        for (int32_t j = 0; j < n; j++) {
            float sign = ((rand()%2) == 0) ? 1 : -1;
            input_data[i*n+j] = (__fp16)(sign * ((float)rand()/(float)RAND_MAX));
        }
    }
}

float saturate_to_fp16(float x)
{
    float temp;
    if (isinf(x))
    {
        temp = (x>(0)) ? 65504.0 : -65504.0;
    }
    else
    {
    temp = (x>(65504.0)) ? 65504 : x;
    temp = (x<(-65504.0)) ? -65504.0 : x;
    }
    return temp;
}

void create_output_data(__fp16* input_data, __fp16* output_data, uint32_t m, uint32_t n)
{
    float sum = 0;
    float temp = 0;
    for (uint32_t j = 0; j < n; j++) {
        sum += input_data[j];
        sum = saturate_to_fp16(sum);
        output_data[j] = (__fp16)sum;
    }

        for (uint32_t i = 1; i < m; i++)
        {
            sum = 0;
            for (uint32_t j = 0; j < n; j++) {
                sum += input_data[i*n+j];
                sum = saturate_to_fp16(sum);
                temp = (sum+output_data[(i-1)*n+j]);
                temp = saturate_to_fp16(temp);
                output_data[i*n+j] = (__fp16)temp;
            }
        }

}

int main(void)
{
    __fp16* output_data_hvx;
    __fp16* input_data;
    __fp16* output_data;
    float diff;
    float epsilon = 5000.0;
    int32_t passed = 0;
    int32_t failed = 0;
    uint32_t m,n;
    uint32_t marray[5] = {3, 512, 1024, 1920, 1440};
    uint32_t narray[5] = {3, 512, 720, 2560,  1080};

    printf("Test for qhblas_hvx_matrix_integrate_ahf function:\n");

            for (int q = 0; q < 5; q++) {
                for (int p = 0; p < 5; p++) {
                    m = marray[q];
                    n = narray[p];

                    output_data_hvx = malloc(m*n*sizeof(__fp16));
                    input_data = malloc(m*n*sizeof(__fp16));
                    output_data = malloc(m*n*sizeof(__fp16));
                    create_input_data(input_data, m, n);
                    create_output_data(input_data, output_data, m, n);
                    qhblas_hvx_matrix_integrate_ahf(input_data, output_data_hvx, m, n);

                    passed += n*m;
                    for (int32_t i = 0; i < m; i++) {
                        for (int32_t j = 0; j < n; j++) {
                            diff = output_data_hvx[i*n+j] - output_data[i*n+j];
                            diff = (diff<0.0) ? -diff : diff;
                            if (diff > epsilon )
                            {
                                printf("error on element i, j: %ld %ld output_data_hvx: %f output_data %f\n", i,j, output_data_hvx[i*n+j], output_data[i*n+j]);
                                failed+=1;
                                passed+=-1;
                            }
                        }
                    }
                    free(input_data);
                    free(output_data);
                    free(output_data_hvx);
            }
        }

    printf("TEST MATRIX integrate 16f->16f %s | %ld passed | %ld failed\n", ((failed == 0) ? "PASSED!" : "FAILED!"), passed ,failed );

    return 0;
}

#else /* __HEXAGON_ARCH__ >= 68 */
#include <stdio.h>
int main()
{
    printf("HVX float is not supported for this architecture.\n");
    return 0;
}
#endif /* __HEXAGON_ARCH__ >= 68 */

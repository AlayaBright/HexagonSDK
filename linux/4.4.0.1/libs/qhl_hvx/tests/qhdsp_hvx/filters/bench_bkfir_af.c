/**=============================================================================
@file
    bench_bkfir_af.c

@brief
    Benchmark for measuring performance of qhdsp_hvx_bkfir_af function.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>
#include <hexagon_cache.h>
#include "qhl_common.h"
#include "qhdsp_hvx.h"
#include <math.h>

#define VLEN 128

// generate randomly distributed float numbers in the range [lo, hi]
void init_data(float *input, uint32_t length, float lo, float hi)
{
    for (uint32_t i = 0; i < length; ++i)
    {
        input[i] = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
    }
}


int main(int argc, char* argv[])
{
    int32_t blockSize, numTaps;
    double cpc, cpm, uspc, uspm;

    /* -----------------------------------------------------*/
    /*  Get input parameters                                */
    /* -----------------------------------------------------*/
    if (argc != 3)
    {
        printf("Error: Usage is %s <blockSize> <numTaps>\n", argv[0]);
        return 1;
    }

    blockSize  = atoi(argv[1]);
    numTaps = atoi(argv[2]);

    /* -----------------------------------------------------*/
    /*  Allocate memory for input/output                    */
    /* -----------------------------------------------------*/
    float *input  = memalign(VLEN, (blockSize+numTaps)*sizeof(float));
    float *coefs = memalign(VLEN, numTaps*sizeof(float));
    float *output = memalign(VLEN, blockSize*sizeof(float));

    if (input == NULL || coefs == NULL || output == NULL)
    {
        printf("Error: Could not allocate Memory for image\n");
        return 1;
    }

    /* -----------------------------------------------------*/
    /*  Generate both the input and coefficients            */
    /* -----------------------------------------------------*/
    init_data((float *)coefs, numTaps, -1.0f, 1.0f);
    init_data((float *)input, blockSize+numTaps, -1.0f, 1.0f);

    /* -----------------------------------------------------*/
    /*  Call benchmark fuction                              */
    /* -----------------------------------------------------*/
    cpc = 0;
    uspc = 0;
    for (int32_t i = 0; i < NR_OF_ITERATIONS; ++i)
    {
        hexagon_buffer_cleaninv((const uint8_t *)input, (blockSize+numTaps)*sizeof(float));
        measurement_start();
        qhdsp_hvx_bkfir_af(input, coefs, numTaps, blockSize, output);
        measurement_finish();
        cpc += measurement_get_cycles();
        uspc += measurement_get_us();
    }

    cpc /= (double) NR_OF_ITERATIONS;
    uspc /= (double) NR_OF_ITERATIONS;
    cpm = cpc / (double) (blockSize);
    uspm = uspc / (double) (blockSize);
    printf("  %11.4f CPC %11.4f MPS | %11.4f CPM %11.4f MPSPM | blockSize %lu numTaps %lu\n",
           cpc, 1.0 / uspc, cpm, 1.0 / uspm, blockSize, numTaps);

    free(input);
    free(coefs);
    free(output);

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

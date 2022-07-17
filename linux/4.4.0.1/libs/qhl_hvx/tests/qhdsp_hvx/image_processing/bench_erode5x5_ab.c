/**=============================================================================
@file
    bench_erode5x5_ab.c

@brief
    Benchmark for measuring performance of qhdsp_hvx_erode5x5_ab function.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include <hexagon_cache.h>
#include "qhl_common.h"
#include "qhdsp_hvx.h"

#define VLEN 128

int main(int argc, char* argv[])
{
    int32_t width, height, stride;
    double cpc, cpm, uspc, uspm;
    FILE* fp;

    printf("Benchmark for qhdsp_hvx_erode5x5_ab function:\n");

    /* -----------------------------------------------------*/
    /*  Get input parameters                                */
    /* -----------------------------------------------------*/
    if (argc != 4)
    {
        printf("Error: Usage is %s <width> <height> <input.bin>\n", argv[0]);
        return 1;
    }

    width  = atoi(argv[1]);
    height = atoi(argv[2]);
    stride = (width + VLEN-1) & (-VLEN);  // make stride a multiple of HVX vector size

    /* -----------------------------------------------------*/
    /*  Allocate memory for input/output                    */
    /* -----------------------------------------------------*/
    uint8_t *input  = memalign(VLEN, stride*height*sizeof(uint8_t));
    uint8_t *output = memalign(VLEN, stride*height*sizeof(uint8_t));

    if (input == NULL || output == NULL)
    {
        printf("Error: Could not allocate Memory for image\n");
        return 1;
    }

    /* -----------------------------------------------------*/
    /*  Read image input from file                          */
    /* -----------------------------------------------------*/
    if((fp = fopen(argv[3], "rb")) < 0)
    {
        printf("Error: Cannot open %s for input\n", argv[3]);
        goto err;
    }

    for (int32_t i = 0; i < height; ++i)
    {
        if(fread(&input[i*stride], sizeof(int8_t), sizeof(uint8_t)*width, fp) != width)
        {
            printf("Error: Unable to read from %s\n", argv[3]);
            fclose(fp);
            goto err;
        }
    }
    fclose(fp);

    /* -----------------------------------------------------*/
    /*  Call benchmark fuction                              */
    /* -----------------------------------------------------*/
    cpc = 0;
    uspc = 0;
    for (int32_t i = 0; i < NR_OF_ITERATIONS; ++i)
    {
        hexagon_buffer_cleaninv(input, stride*height*sizeof(uint8_t));
        measurement_start();
        qhdsp_hvx_erode5x5_ab(input, stride, width, height, output, stride);
        measurement_finish();
        cpc += measurement_get_cycles();
        uspc += measurement_get_us();
    }

    cpc /= (double) NR_OF_ITERATIONS;
    uspc /= (double) NR_OF_ITERATIONS;
    cpm = cpc / (double) (width * height);
    uspm = uspc / (double) (width * height);
    printf("  %11.4f CPC %11.4f MPS | %11.4f CPM %11.4f MPSPM | image size = %lu %lu\n",
           cpc, 1.0 / uspc, cpm, 1.0 / uspm, width, height);
    free(input);
    free(output);

    return 0;

err:
    free(input);
    free(output);

    return 1;
}

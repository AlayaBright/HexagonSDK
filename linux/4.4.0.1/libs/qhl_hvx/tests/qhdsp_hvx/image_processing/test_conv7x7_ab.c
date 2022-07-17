/**=============================================================================
@file
    test_conv7x7_ab.c

@brief
    Accuracy test for qhdsp_hvx_conv7x7_ab function.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include <hexagon_cache.h>
#include "qhl_common.h"
#include "qhdsp_hvx.h"

#define VLEN 128

static void qhdsp_hvx_conv7x7_ab_ref(
    uint8_t  *input,
    int32_t  stride_i,
    uint32_t width,
    uint32_t height,
    int8_t   *mask,
    uint32_t shift,
    uint8_t  *output,
    int32_t  stride_o)
{
    if ((input == NULL) || (mask == NULL) || (output == NULL) || (height < 7)
        || ((uint32_t) input & 127) || ((uint32_t) output & 127)
        || (stride_i & 127) || (width & 127) || (stride_o & 127))
    {
        return;
    }

    for (int32_t i = 3; i < height-3; ++i)
    {
        for (int32_t j = 3; j < width-3; ++j)
        {
            int32_t sum = 0;

            for (int32_t k = 0; k < 7; ++k)
            {
                for (int32_t n = 0; n < 7; ++n)
                {
                    int16_t pixel = input[(i-3+k)*stride_i + (j-3+n)];
                    int16_t maskv = mask[k*7+n];
                    sum += pixel * maskv;
                }
            }

            sum = sum >> shift;

            if (sum < 0) sum = 0;
            if (sum > 255) sum = 255;

            output[i*stride_o + j] = sum;
        }
    }
}

static int8_t mask[49] =
{
    1, 1, 1,  -7, 1, 1, 1,
    2, 2, 2,  -8, 2, 2, 2,
    3, 3, 3,  -9, 3, 3, 3,
    4, 4, 4, -10, 4, 4, 4,
    1, 1, 1, -11, 1, 1, 1,
    2, 2, 2, -12, 2, 2, 2,
    3, 3, 3, -13, 3, 3, 3
};

int main(int argc, char* argv[])
{
    int32_t width, height, stride;
    FILE* fp;

    printf("Test for qhdsp_hvx_conv7x7_ab function:\n");

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
    uint8_t *output_ref = memalign(VLEN, stride*height*sizeof(uint8_t));

    if (input == NULL || output == NULL || output_ref == NULL)
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
    /*  Call test fuctions                                  */
    /* -----------------------------------------------------*/
    qhdsp_hvx_conv7x7_ab(input, stride, width, height, mask, 4, output, stride);
    qhdsp_hvx_conv7x7_ab_ref(input, stride, width, height, mask, 4, output_ref, stride);

    for (int32_t i = 3; i < height-3; ++i)
    {
        for (int32_t j = 3; j < width-3; ++j)
        {
            if (output[i*stride + j] != output_ref[i*stride + j])
            {
                printf("Error on %ld %ld:\t got = %d | OK = %d | image size = %lu %lu\n",
                       i, j, output[i*stride + j], output_ref[i*stride + j], width, height);
                goto err;
            }
        }
    }

    printf("PASSED\n");

    free(input);
    free(output);
    free(output_ref);

    return 0;

err:
    free(input);
    free(output);
    free(output_ref);

    return 1;
}

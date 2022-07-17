/**=============================================================================
@file
    test_bench_fxp_fft_complex.c

@brief
    Test and bench for complex 2^N FFT and IFFT routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/


#include <stdio.h>
#include <stdlib.h>
#if __hexagon__
#include <hexagon_protos.h>     // intrinsics
#else
#ifdef __ARM_ACLE
#include <arm_acle.h>
#endif /* __ARM_ACLE */
#include <malloc.h>
#include <stdint.h>
#endif

#if __hexagon__
#include "qhmath.h"
#include "qhcomplex.h"
#include "qhdsp.h"
#else
#include <math.h>
#include <complex.h>
#endif

#include "qhdsp_fft_internal.h"
#include "test_data/fft_fxp_complex_test_data.h"
#include "qhdsp_fft_complex.h"
#include "qhdsp_ifft_complex.h"
#include "qhdsp_common.h"
#include "qhl_common.h"

short complex *Input;
short complex *Output;
short complex *Output_ASM;
int32_t *Wtwiddles;
short complex *IFFT_output;
short complex *IFFT_output_ASM;

short complex *inputTestDataPtrs[] =
{
    (short complex *)fxp_fft_complex_test_x2,
    (short complex *)fxp_fft_complex_test_x4,
    (short complex *)fxp_fft_complex_test_x8,
    (short complex *)fxp_fft_complex_test_x16,
    (short complex *)fxp_fft_complex_test_x32,
    (short complex *)fxp_fft_complex_test_x64,
    (short complex *)fxp_fft_complex_test_x128,
    (short complex *)fxp_fft_complex_test_x256,
    (short complex *)fxp_fft_complex_test_x512,
    (short complex *)fxp_fft_complex_test_x1024,
    (short complex *)fxp_fft_complex_test_x2048,
    (short complex *)fxp_fft_complex_test_x4096
};

short complex *resTestDataPtrs[] =
{
    (short complex *)fxp_fft_complex_test_y2,
    (short complex *)fxp_fft_complex_test_y4,
    (short complex *)fxp_fft_complex_test_y8,
    (short complex *)fxp_fft_complex_test_y16,
    (short complex *)fxp_fft_complex_test_y32,
    (short complex *)fxp_fft_complex_test_y64,
    (short complex *)fxp_fft_complex_test_y128,
    (short complex *)fxp_fft_complex_test_y256,
    (short complex *)fxp_fft_complex_test_y512,
    (short complex *)fxp_fft_complex_test_y1024,
    (short complex *)fxp_fft_complex_test_y2048,
    (short complex *)fxp_fft_complex_test_y4096
};

void init(short complex *inputVector, short complex *inputData, int32_t nPoints, int32_t log2N)
{
    qhdsp_fft_gen_twiddles_complex_ach(Wtwiddles, nPoints, log2N);

    for(int32_t i=0; i<nPoints;i++)
    {
        inputData[i] = inputVector[i];
    }
}

void do_CPC_measurement_of_FFT_IFFT_func(void *funcPtr, short complex *inputData, int32_t nPoints, short complex *outputData)
{
    void (*benchFunction)();

    benchFunction = funcPtr;

    measurement_start();
    benchFunction(inputData, nPoints, Wtwiddles, outputData);
    measurement_finish();

    measurement_print();
}

void calculate_SNR(short complex* REF, short complex* DUT, int32_t nPoints)
{
    short xr, xi;
    short er, ei;

    short max_err;
    short max_err_re = 0;
    short max_err_im = 0;

    for(int32_t i=0; i<nPoints; i++)
    {
        xr = bf_extr(DUT[i],16,0);     // get real part of DUT
        xi = bf_extr(DUT[i],16,16);    // get imag part of DUT
        er = bf_extr(REF[i],16,0);     // get real part of REF
        ei = bf_extr(REF[i],16,16);    // get imag part of REF

        if(abs(xr-er) > max_err_re)
        {
            max_err_re = abs(xr-er);
        }

        if(abs(xi-ei) > max_err_im)
        {
            max_err_im = abs(xi-ei);
        }
    }

    max_err = max_err_im;
    if(max_err_re > max_err)
    {
        max_err = max_err_re;
    }

    if(max_err_re!=0)    // avoid log2(zero)
    {
        printf("SNR [accuracy]:\t\t%.1f dB (out of 96.0dB max)\n", 96 - 6*((LOG2((float)max_err))+1));
    }
    else
    {
        printf("SNR [accuracy]:\t\t96.0 dB (out of 96.0dB max)\n");        // max for 16-bit fixed-point (6dB rule)
    }
}

void allocate_aligned_buffers(int32_t nPoints)
{
    // void *memalign(size_t alignment, size_t size);
    Input = memalign(nPoints*sizeof(short complex), nPoints*sizeof(short complex));
    Output = memalign(nPoints*sizeof(short complex), nPoints*sizeof(short complex));        // has to be aligned to buffer size because of IFFT brev
                                                                                // (reusing this buffer as IFFT input buffer)
    Output_ASM = memalign(sizeof(short complex), nPoints*sizeof(short complex));
    Wtwiddles = memalign(sizeof(int32_t), 3*nPoints*sizeof(int32_t)/4);

    IFFT_output = memalign(sizeof(short complex), nPoints*sizeof(short complex));
    IFFT_output_ASM = memalign(sizeof(short complex), nPoints*sizeof(short complex));
}

void free_aligned_buffers()
{
    free(Input);
    free(Output);
    free(Output_ASM);
    free(Wtwiddles);

    free(IFFT_output);
    free(IFFT_output_ASM);
}

void bench(int32_t nPoints, int32_t log2N)
{
    allocate_aligned_buffers(nPoints);

    init(inputTestDataPtrs[log2N-1], Input, nPoints, log2N);        // generate twiddles + copy inputs


    printf("\n----------------------------------------\n");
    printf("Benchmark for FFT (complex), num points: %d\n", nPoints);
    printf("----------------------------------------\n");

    // -----------------------------------------------
    // FFT C
    printf("FFT C:\n");
    do_CPC_measurement_of_FFT_IFFT_func(c1dfft_h,Input, nPoints, Output);
    calculate_SNR(resTestDataPtrs[log2N-1], Output, nPoints);
    printf("----------------------------------------\n");
    // -----------------------------------------------
#if __hexagon__
    // -----------------------------------------------
    // FFT ASM
    printf("FFT ASM:\n");
    do_CPC_measurement_of_FFT_IFFT_func(qhdsp_c1dfft_ach, Input, nPoints, Output_ASM);
    calculate_SNR(resTestDataPtrs[log2N-1], Output_ASM, nPoints);
    printf("----------------------------------------\n");
    // -----------------------------------------------
#endif
    printf("\n----------------------------------------\n");
    printf("Benchmark for IFFT (complex), num points: %d\n", nPoints);
    printf("----------------------------------------\n");

    // -----------------------------------------------
    // IFFT C
    printf("IFFT C:\n");
    do_CPC_measurement_of_FFT_IFFT_func(c1difft_h, Output, nPoints, IFFT_output);
    calculate_SNR(Input, IFFT_output, nPoints);
    printf("----------------------------------------\n");
    // -----------------------------------------------
#if __hexagon__
    // -----------------------------------------------
    // IFFT asm
    printf("IFFT ASM:\n");
    do_CPC_measurement_of_FFT_IFFT_func(qhdsp_c1difft_ach, Output, nPoints, IFFT_output_ASM);
    calculate_SNR(Input, IFFT_output_ASM, nPoints);
    printf("----------------------------------------\n");
    // -----------------------------------------------
#endif
    free_aligned_buffers();
}

int main(int argc, char const *argv[])
{
    int32_t iWindow = 2;
#if __hexagon__
    performance_high();
#endif
    for(int32_t i=1; i<=12; i++)
    {
        bench(iWindow, i);
        iWindow *= 2;       // (int32_t)(pow(2,i))
        printf("\n");
    }
#if __hexagon__
    performance_normal();
#endif
    return 0;
}


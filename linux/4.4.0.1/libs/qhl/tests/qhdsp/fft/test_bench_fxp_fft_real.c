/**=============================================================================
@file
    test_bench_fxp_fft_real.c

@brief
    Test and bench for real 2^N FFT and IFFT routines.

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
#include "test_data/fft_fxp_real_test_data.h"
#include "qhdsp_fft_real.h"
#include "qhdsp_ifft_real.h"
#include "qhdsp_common.h"
#include "qhl_common.h"

int16_t *Input;
short complex *Output;
short complex *Output_ASM;
int32_t *Wtwiddle1;
int32_t *Wtwiddle2;
int16_t *IFFT_output;
int16_t *IFFT_output_ASM;

int16_t *inputTestDataPtrs[] =
{
    (int16_t *)fxp_fft_test_x2,
    (int16_t *)fxp_fft_test_x4,
    (int16_t *)fxp_fft_test_x8,
    (int16_t *)fxp_fft_test_x16,
    (int16_t *)fxp_fft_test_x32,
    (int16_t *)fxp_fft_test_x64,
    (int16_t *)fxp_fft_test_x128,
    (int16_t *)fxp_fft_test_x256,
    (int16_t *)fxp_fft_test_x512,
    (int16_t *)fxp_fft_test_x1024,
    (int16_t *)fxp_fft_test_x2048,
    (int16_t *)fxp_fft_test_x4096
};

short complex *resTestDataPtrs[] =
{
    (short complex *)fxp_fft_test_y2,
    (short complex *)fxp_fft_test_y4,
    (short complex *)fxp_fft_test_y8,
    (short complex *)fxp_fft_test_y16,
    (short complex *)fxp_fft_test_y32,
    (short complex *)fxp_fft_test_y64,
    (short complex *)fxp_fft_test_y128,
    (short complex *)fxp_fft_test_y256,
    (short complex *)fxp_fft_test_y512,
    (short complex *)fxp_fft_test_y1024,
    (short complex *)fxp_fft_test_y2048,
    (short complex *)fxp_fft_test_y4096
};

void init(int16_t *inputVector, int16_t *inputData, int32_t nPoints, int32_t log2N)
{
    qhdsp_fft_gen_twiddles_real_ach(Wtwiddle1, Wtwiddle2, nPoints, log2N);

    for(int32_t i=0; i<nPoints;i++)
    {
        inputData[i] = inputVector[i];
    }
}

void do_CPC_measurement_of_FFT_func(void *funcPtr, int16_t *inputData, int32_t nPoints, short complex *outputData)
{
    void (*benchFunction)();

    benchFunction = funcPtr;

    measurement_start();
    benchFunction(inputData, nPoints, Wtwiddle1, Wtwiddle2, outputData);
    measurement_finish();

    measurement_print();
}

void do_CPC_measurement_of_IFFT_func(void *funcPtr, short complex *inputData, int32_t nPoints, int16_t *outputData)
{
    void (*benchFunction)();

    benchFunction = funcPtr;

    measurement_start();
    benchFunction(inputData, nPoints, Wtwiddle1, Wtwiddle2, outputData);
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
    Input = memalign(nPoints*sizeof(int16_t), nPoints*sizeof(int16_t));
    Output = memalign(nPoints*sizeof(short complex), nPoints*sizeof(short complex));        // has to be aligned to buffer size because of IFFT brev
                                                                                            // (reusing this buffer as IFFT input buffer)
    Output_ASM = memalign(sizeof(short complex), nPoints*sizeof(short complex));
    Wtwiddle1 = memalign(sizeof(int32_t), 3*nPoints*sizeof(int32_t)/8);                     // 3*N/4 - for N point complex FFT -> 3*N/8 - for N/2 point
    Wtwiddle2 = memalign(sizeof(int32_t), nPoints/2*sizeof(int32_t));

    IFFT_output = memalign(sizeof(int16_t), nPoints*sizeof(int16_t));
    IFFT_output_ASM = memalign(sizeof(int16_t)*2, nPoints*sizeof(int16_t));
}

void free_aligned_buffers()
{
    free(Input);
    free(Output);
    free(Output_ASM);
    free(Wtwiddle1);
    free(Wtwiddle2);

    free(IFFT_output);
    free(IFFT_output_ASM);
}

void bench(int32_t nPoints, int32_t log2N)
{
    allocate_aligned_buffers(nPoints);

    init(inputTestDataPtrs[log2N-1], Input, nPoints, log2N);        // generate twiddles + copy inputs


    printf("\n----------------------------------------\n");
    printf("Benchmark for FFT (real), num points: %d\n", nPoints);
    printf("----------------------------------------\n");

    // -----------------------------------------------
    // FFT C
    printf("FFT C:\n");
    do_CPC_measurement_of_FFT_func(r1dfft_h, Input, nPoints, Output);
    calculate_SNR(resTestDataPtrs[log2N-1], Output, nPoints);
    printf("----------------------------------------\n");
    // -----------------------------------------------
#if __hexagon__
    // -----------------------------------------------
    // FFT ASM
    printf("FFT ASM:\n");
    do_CPC_measurement_of_FFT_func(qhdsp_ach_r1dfft_ah, Input, nPoints, Output_ASM);
    calculate_SNR(resTestDataPtrs[log2N-1], Output_ASM, nPoints);
    printf("----------------------------------------\n");
    // -----------------------------------------------
#endif
    printf("\n----------------------------------------\n");
    printf("Benchmark for IFFT (real), num points: %d\n", nPoints);
    printf("----------------------------------------\n");

    // -----------------------------------------------
    // IFFT C
    printf("IFFT C:\n");
    do_CPC_measurement_of_IFFT_func(r1difft_h, Output, nPoints, IFFT_output);
    calculate_SNR((short complex*)Input, (short complex*)IFFT_output, nPoints/2);         // nPoints/2 since we are treating nPoints real values
                                                                    // as nPoints/2 complex values - doesn't affect SNR
                                                                    // calculation really
    printf("----------------------------------------\n");
    // -----------------------------------------------
#if __hexagon__
    // -----------------------------------------------
    // IFFT asm
    printf("IFFT ASM:\n");
    do_CPC_measurement_of_IFFT_func(qhdsp_ah_r1difft_ach, Output, nPoints, IFFT_output_ASM);
    calculate_SNR((short complex*)Input, (short complex*)IFFT_output_ASM, nPoints/2);     // nPoints/2 since we are treating nPoints real values
                                                                    			  // as nPoints/2 complex values - doesn't affect SNR
                                                                    			  // calculation really
    printf("----------------------------------------\n");
#endif
    // -----------------------------------------------

    free_aligned_buffers();
}

int main(int argc, char const *argv[])
{
    int32_t iWindow = 2;

    performance_high();

    for(int32_t i=1; i<=12; i++)
    {
        bench(iWindow, i);
        iWindow *= 2;       // (int32_t)(pow(2,i))
        printf("\n");
    }

    performance_normal();

    return 0;
}


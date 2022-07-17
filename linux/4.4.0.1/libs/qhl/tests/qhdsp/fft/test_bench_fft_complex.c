/**=============================================================================
@file
    test_bench_fft_complex.c

@brief
    Test and bench for complex 2^N FFT and IFFT routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#if __hexagon__
#include <stdlib.h>
#include "qhmath.h"
#include "qhcomplex.h"
#else
#include <math.h>
#include <complex.h>
#include <malloc.h>
#endif
#include "qhdsp.h"
#include "qhdsp_fft_internal.h"
#include "test_data/fft_complex_test_data.h"
#include "qhdsp_fft_complex.h"
#include "qhdsp_ifft_complex.h"
#include "qhdsp_common.h"
#include "qhl_common.h"

float complex *Input;
float complex *Output;
float complex *Output_ASM;
float complex *Wtwiddles;
float complex *IFFT_output;
float complex *IFFT_output_ASM;

float complex* inputTestDataPtrs[] =
{
    (float complex *)fft_complex_test_x2,
    (float complex *)fft_complex_test_x4,
    (float complex *)fft_complex_test_x8,
    (float complex *)fft_complex_test_x16,
    (float complex *)fft_complex_test_x32,
    (float complex *)fft_complex_test_x64,
    (float complex *)fft_complex_test_x128,
    (float complex *)fft_complex_test_x256,
    (float complex *)fft_complex_test_x512,
    (float complex *)fft_complex_test_x1024,
    (float complex *)fft_complex_test_x2048,
    (float complex *)fft_complex_test_x4096
};

float complex* resTestDataPtrs[] =
{
    (float complex *)fft_complex_test_y2,
    (float complex *)fft_complex_test_y4,
    (float complex *)fft_complex_test_y8,
    (float complex *)fft_complex_test_y16,
    (float complex *)fft_complex_test_y32,
    (float complex *)fft_complex_test_y64,
    (float complex *)fft_complex_test_y128,
    (float complex *)fft_complex_test_y256,
    (float complex *)fft_complex_test_y512,
    (float complex *)fft_complex_test_y1024,
    (float complex *)fft_complex_test_y2048,
    (float complex *)fft_complex_test_y4096
};

void init(float complex *inputVector, float complex *inputData, int nPoints, int log2N)
{
    qhdsp_fft_gen_twiddles_complex_acf(Wtwiddles, nPoints, log2N);

    for(int i=0; i<nPoints;i++)
    {
        inputData[i] = inputVector[i];
    }
}

void do_CPC_measurement_of_FFT_IFFT_func(void *funcPtr, float complex *inputData, int nPoints, float complex *outputData)
{
    void (*benchFunction)();

    benchFunction = funcPtr;

    measurement_start();
    benchFunction(inputData, nPoints, Wtwiddles, outputData);
    measurement_finish();

    measurement_print();
}

void calculate_SNR(float complex* REF, float complex* DUT, int nPoints)
{
    double xr, xi;
    double er, ei, err, sig;

    sig = 0.0;
    err = 0.0;

    for(int i=0; i<nPoints; i++)
    {
        xr = creal_f(DUT[i]);   xi = cimag_f(DUT[i]);
        er = creal_f(REF[i]);   ei = cimag_f(REF[i]);

        sig += er * er + ei * ei;

        er -= xr;  ei -= xi;
        err += er * er + ei * ei;
    }

    printf("SNR [accuracy]:\t\t%.2f dB\n", 10 * LOG10(sig / err));
}

void allocate_aligned_buffers(int nPoints)
{
    // void *memalign(size_t alignment, size_t size);
    Input = memalign(nPoints*sizeof(float complex), nPoints*sizeof(float complex));
    Output = memalign(nPoints*sizeof(float complex), nPoints*sizeof(float complex));        // has to be aligned to buffer size because of IFFT brev
                                                                                            // (reusing this buffer as IFFT input buffer)
    Output_ASM = memalign(sizeof(float complex), nPoints*sizeof(float complex));
    Wtwiddles = memalign(sizeof(float complex), 3*nPoints*sizeof(float complex)/4);

    IFFT_output = memalign(sizeof(float complex), nPoints*sizeof(float complex));
    IFFT_output_ASM = memalign(sizeof(float complex), nPoints*sizeof(float complex));
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

void bench(int nPoints, int log2N)
{
    allocate_aligned_buffers(nPoints);

    init(inputTestDataPtrs[log2N-1], Input, nPoints, log2N);        // generate twiddles + copy inputs


    printf("\n----------------------------------------\n");
    printf("Benchmark for FFT (complex), num points: %d\n", nPoints);
    printf("----------------------------------------\n");

    // -----------------------------------------------
    // FFT C
    printf("FFT C:\n");
    do_CPC_measurement_of_FFT_IFFT_func(c1dfft_f,Input, nPoints, Output);
    calculate_SNR(resTestDataPtrs[log2N-1], Output, nPoints);
    printf("----------------------------------------\n");
    // -----------------------------------------------
#if __hexagon__
    // -----------------------------------------------
    // FFT ASM
    printf("FFT ASM:\n");
    do_CPC_measurement_of_FFT_IFFT_func(qhdsp_c1dfft_acf, Input, nPoints, Output_ASM);
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
    do_CPC_measurement_of_FFT_IFFT_func(c1difft_f, Output, nPoints, IFFT_output);
    calculate_SNR(Input, IFFT_output, nPoints);
    printf("----------------------------------------\n");
    // -----------------------------------------------
#if __hexagon__
    // -----------------------------------------------
    // IFFT asm
    printf("IFFT ASM:\n");
    do_CPC_measurement_of_FFT_IFFT_func(qhdsp_c1difft_acf, Output, nPoints, IFFT_output_ASM);
    calculate_SNR(Input, IFFT_output_ASM, nPoints);
    printf("----------------------------------------\n");
    // -----------------------------------------------
#endif
    free_aligned_buffers();
}

int main(int argc, char const *argv[])
{
    int iWindow = 2;
#if __hexagon__
    performance_high();
#endif
    for(int i=1; i<=12; i++)
    {
        bench(iWindow, i);
        iWindow *= 2;       // (int)(pow(2,i))
        printf("\n");
    }
#if __hexagon__
    performance_normal();
#endif
    return 0;
}

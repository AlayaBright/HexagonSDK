/**=============================================================================
@file
   qhdsp_test_common.c

@brief
   Common routines used internally in QHDSP testing.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include "qhdsp_test_common.h"

static uint8_t extract_l8(int16_t L_var1)
{
   uint8_t var2;

   var2 = (int16_t) (0x00ff & L_var1);

   return (uint8_t)(var2);
}

static uint8_t extract_h8(int16_t L_var1)
{
   uint8_t var2;

   var2 = (int16_t) (0x00ff & (L_var1 >> 8));

   return (uint8_t)(var2);
}

static int32_t L_extract_l32(int64_t LL_var1)
{
    int32_t L_var_out;

    L_var_out = (int32_t)LL_var1;

    return (L_var_out);
}

static int32_t L_extract_h32(int64_t LL_var1)
{
    int32_t L_var_out;

    L_var_out = (int32_t)(LL_var1 >> 32);

    return (L_var_out);
}

float calculate_SNR32(int complex* REF, int complex* DUT, uint32_t n_points)
{
    int32_t real_DUT, imag_DUT;
    int32_t real_REF, imag_REF;

    int32_t max_err;
    int32_t max_err_re = 0;
    int32_t max_err_im = 0;

    for(uint32_t i=0; i<n_points; i++)
    {
        real_DUT = L_extract_l32(DUT[i]);     // get real part of DUT
        imag_DUT = L_extract_h32(DUT[i]);     // get imag part of DUT
        real_REF = L_extract_l32(REF[i]);     // get real part of REF
        imag_REF = L_extract_h32(REF[i]);     // get imag part of REF

        if(abs(real_DUT-real_REF) > max_err_re)
        {
            max_err_re = abs(real_DUT-real_REF);
        }

        if(abs(imag_DUT-imag_REF) > max_err_im)
        {
            max_err_im = abs(imag_DUT-imag_REF);
        }
    }

    max_err = max_err_im;
    if(max_err_re > max_err)
    {
        max_err = max_err_re;
    }

    if(max_err_re!=0)    // avoid log2(zero)
    {
        return (float)(32*6 - 6*((qhmath_log2_f((float)max_err))+1));
    }
    else
    {
        return (float)(32*6);        // max for 32-bit fixed-point (6dB rule)
    }

    //printf("Max error: %ld\n", max_err);
}

float calculate_SNR8(const char complex* REF, char complex* DUT, uint32_t n_points)
{
    int32_t real_DUT, imag_DUT;
    int32_t real_REF, imag_REF;

    int32_t max_err;
    int32_t max_err_re = 0;
    int32_t max_err_im = 0;

    for(uint32_t i=0; i<n_points; i++)
    {
        real_DUT = extract_l8((int16_t)DUT[i]);     // get real part of DUT
        imag_DUT = extract_h8((int16_t)DUT[i]);     // get imag part of DUT
        real_REF = extract_l8((int16_t)REF[i]);     // get real part of REF
        imag_REF = extract_h8((int16_t)REF[i]);     // get imag part of REF

        if(abs(real_DUT-real_REF) > max_err_re)
        {
            max_err_re = abs(real_DUT-real_REF);
        }

        if(abs(imag_DUT-imag_REF) > max_err_im)
        {
            max_err_im = abs(imag_DUT-imag_REF);
        }
    }

    max_err = max_err_im;
    if(max_err_re > max_err)
    {
        max_err = max_err_re;
    }

    if(max_err_re!=0)    // avoid log2(zero)
    {
        return (float)(8*6 - 6*((qhmath_log2_f((float)max_err))+1));
    }
    else
    {
        return (float)(8*6);        // max for 8-bit fixed-point (6dB rule)
    }

    //printf("Max error: %ld\n", max_err);
}

float calculate_SNR_sf(float complex* REF, float complex* DUT, int nPoints)
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

    return 10 * LOG10(sig / err);

    //printf("SNR [accuracy]:\t\t%.2f dB\n", 10 * LOG10(sig / err));
}

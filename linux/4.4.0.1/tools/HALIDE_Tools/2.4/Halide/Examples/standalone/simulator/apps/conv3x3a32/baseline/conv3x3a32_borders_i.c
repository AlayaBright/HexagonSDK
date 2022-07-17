/* ===============================================================================================
 */
/*  QUALCOMM TECHNOLOGIES, INC. */
/*                                                                                                 */
/*  HEXAGON HVX Image/Video Processing Library */
/*                                                                                                 */
/* -----------------------------------------------------------------------------------------------
 */
/*          Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its
 * subsidiaries.           */
/*                           All Rights Reserved. */
/*                  QUALCOMM Confidential and Proprietary */
/* ===============================================================================================
 */

/*[===============================================================================================]*/
/*[ FUNCTION                                                               ]*/
/*[     conv3x3                                                            ]*/
/*[                                                                        ]*/
/*[-----------------------------------------------------------------------------------------------]*/
/*[ DESCRIPTION                                                            ]*/
/*[     This function applies a 3x3 kernel to filter a image.              ]*/
/*[     During the computation, 32bit accumulator is used.                 ]*/
/*[                                                                        ]*/
/*[-----------------------------------------------------------------------------------------------]*/
/*[ REVISION DATE                                                          ]*/
/*[     AUG-01-2014                                                        ]*/
/*[                                                                        ]*/
/*[===============================================================================================]*/
#include "hexagon_types.h"
#define VLEN 128
/* ===============================================================================================
 */
/*  Intrinsic C version of conv3x3().                                       */
/* =============================================================================================== */
static void conv3x3Per2Row(
    unsigned char *restrict inp,
    int stride,
    int width,
    int height,
    signed char *restrict mask,
    int shift,
    unsigned char *restrict outp,
    int y) {
    int i;
    HEXAGON_Vect32 __m2m1m0, __m5m4m3, __m8m7m6;
    HEXAGON_Vect32 *mask4 = (HEXAGON_Vect32 *)mask;

    HVX_Vector sline000, sline004, sline064;
    HVX_Vector sline100, sline104, sline164;
    HVX_Vector sline200, sline204, sline264;
    HVX_Vector sline300, sline304, sline364;
    HVX_Vector sSum20L, sSum31L, sSum20H, sSum31H;
    HVX_Vector sOut01, sOut01p, sOut11, sOut11p, sOut00, sOut10;

    HVX_VectorPair dline000, dline100, dline200, dline300;
    HVX_VectorPair dSum020, dSum031, dSum120, dSum131;

    __m2m1m0 = mask4[0];
    __m5m4m3 = mask4[1];
    __m8m7m6 = mask4[2];

    int p00, p01, p02, p10, p11, p12, p20, p21, p22, p30, p31, p32;

    HVX_Vector *iptr0 = (y == 0) ? (HVX_Vector *)(inp + 0 * stride) : (HVX_Vector *)(inp - 1 * stride);
    HVX_Vector *iptr1 = (HVX_Vector *)(inp + 0 * stride);
    HVX_Vector *iptr2 = (HVX_Vector *)(inp + 1 * stride);
    HVX_Vector *iptr3 = (y == height - 2) ? (HVX_Vector *)(inp + 1 * stride) : (HVX_Vector *)(inp + 2 * stride);
    HVX_Vector *optr0 = (HVX_Vector *)(outp + 0 * stride);
    HVX_Vector *optr1 = (HVX_Vector *)(outp + 1 * stride);

    p00 = *((char *)iptr0);
    p01 = p00;
    p02 = *((char *)iptr0 + 1);
    p10 = *((char *)iptr1);
    p11 = p10;
    p12 = *((char *)iptr1 + 1);
    p20 = *((char *)iptr2);
    p21 = p20;
    p22 = *((char *)iptr2 + 1);
    p30 = *((char *)iptr3);
    p31 = p30;
    p32 = *((char *)iptr3 + 1);

    sline000 = *iptr0++;
    sline100 = *iptr1++;
    sline200 = *iptr2++;
    sline300 = *iptr3++;

    int sum1 = p00 * mask[0] + p01 * mask[1] + p02 * mask[2] + p10 * mask[4] + p11 * mask[5] + p12 * mask[6] + p20 * mask[8] + p21 * mask[9] + p22 * mask[10];
    int sum2 = p10 * mask[0] + p11 * mask[1] + p12 * mask[2] + p20 * mask[4] + p21 * mask[5] + p22 * mask[6] + p30 * mask[8] + p31 * mask[9] + p32 * mask[10];
    sum1 = (sum1 >> shift);
    sum2 = (sum2 >> shift);

    if (sum1 < 0) sum1 = 0;
    if (sum1 > 255) sum1 = 255;
    if (sum2 < 0) sum2 = 0;
    if (sum2 > 255) sum2 = 255;

    sOut01p = Q6_V_vsplat_R(sum1 << 24);
    sOut11p = Q6_V_vsplat_R(sum2 << 24);

    for (i = width; i > VLEN; i -= VLEN) {
        sline064 = *iptr0++;
        sline164 = *iptr1++;
        sline264 = *iptr2++;
        sline364 = *iptr3++;

        sline004 = Q6_V_valign_VVI(sline064, sline000, 4);
        sline104 = Q6_V_valign_VVI(sline164, sline100, 4);
        sline204 = Q6_V_valign_VVI(sline264, sline200, 4);
        sline304 = Q6_V_valign_VVI(sline364, sline300, 4);

        dline000 = Q6_W_vcombine_VV(sline004, sline000);
        dline100 = Q6_W_vcombine_VV(sline104, sline100);
        dline200 = Q6_W_vcombine_VV(sline204, sline200);
        dline300 = Q6_W_vcombine_VV(sline304, sline300);

        dSum020 = Q6_Ww_vrmpy_WubRbI(dline000, __m2m1m0, 0);
        dSum031 = Q6_Ww_vrmpy_WubRbI(dline000, __m2m1m0, 1);

        dSum020 = Q6_Ww_vrmpyacc_WwWubRbI(dSum020, dline100, __m5m4m3, 0);
        dSum031 = Q6_Ww_vrmpyacc_WwWubRbI(dSum031, dline100, __m5m4m3, 1);

        dSum020 = Q6_Ww_vrmpyacc_WwWubRbI(dSum020, dline200, __m8m7m6, 0);
        dSum031 = Q6_Ww_vrmpyacc_WwWubRbI(dSum031, dline200, __m8m7m6, 1);

        sSum31L =
            Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum031), Q6_V_lo_W(dSum031), shift);
        sSum20L =
            Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum020), Q6_V_lo_W(dSum020), shift);
        sSum31L = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum031), Q6_V_lo_W(dSum031), shift);
        sSum20L = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum020), Q6_V_lo_W(dSum020), shift);
        sOut01 = Q6_Vub_vsat_VhVh(sSum31L, sSum20L);
        sOut00 = Q6_V_vlalign_VVI(sOut01, sOut01p, 1);
        *optr0++ = sOut00;

        dSum120 = Q6_Ww_vrmpy_WubRbI(dline100, __m2m1m0, 0);
        dSum131 = Q6_Ww_vrmpy_WubRbI(dline100, __m2m1m0, 1);

        dSum120 = Q6_Ww_vrmpyacc_WwWubRbI(dSum120, dline200, __m5m4m3, 0);
        dSum131 = Q6_Ww_vrmpyacc_WwWubRbI(dSum131, dline200, __m5m4m3, 1);

        dSum120 = Q6_Ww_vrmpyacc_WwWubRbI(dSum120, dline300, __m8m7m6, 0);
        dSum131 = Q6_Ww_vrmpyacc_WwWubRbI(dSum131, dline300, __m8m7m6, 1);

        sSum31H = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum131), Q6_V_lo_W(dSum131), shift);
        sSum20H = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum120), Q6_V_lo_W(dSum120), shift);
        sOut11 = Q6_Vub_vsat_VhVh(sSum31H, sSum20H);
        sOut10 = Q6_V_vlalign_VVI(sOut11, sOut11p, 1);
        *optr1++ = sOut10;

        sline000 = sline064;
        sline100 = sline164;
        sline200 = sline264;
        sline300 = sline364;

        sOut01p = sOut01;
        sOut11p = sOut11;
    }

    {

        int tmp = VLEN - i + 1;
        unsigned int end0 = *((char *)iptr0 - tmp);
        unsigned int end1 = *((char *)iptr1 - tmp);
        unsigned int end2 = *((char *)iptr2 - tmp);
        unsigned int end3 = *((char *)iptr3 - tmp);

        sline064 = Q6_V_vsplat_R(end0);
        sline164 = Q6_V_vsplat_R(end1);
        sline264 = Q6_V_vsplat_R(end2);
        sline364 = Q6_V_vsplat_R(end3);

        sline004 = Q6_V_valign_VVI(sline064, sline000, 4);
        sline104 = Q6_V_valign_VVI(sline164, sline100, 4);
        sline204 = Q6_V_valign_VVI(sline264, sline200, 4);
        sline304 = Q6_V_valign_VVI(sline364, sline300, 4);

        dline000 = Q6_W_vcombine_VV(sline004, sline000);
        dline100 = Q6_W_vcombine_VV(sline104, sline100);
        dline200 = Q6_W_vcombine_VV(sline204, sline200);
        dline300 = Q6_W_vcombine_VV(sline304, sline300);

        dSum020 = Q6_Ww_vrmpy_WubRbI(dline000, __m2m1m0, 0);
        dSum031 = Q6_Ww_vrmpy_WubRbI(dline000, __m2m1m0, 1);

        dSum020 = Q6_Ww_vrmpyacc_WwWubRbI(dSum020, dline100, __m5m4m3, 0);
        dSum031 = Q6_Ww_vrmpyacc_WwWubRbI(dSum031, dline100, __m5m4m3, 1);

        dSum020 = Q6_Ww_vrmpyacc_WwWubRbI(dSum020, dline200, __m8m7m6, 0);
        dSum031 = Q6_Ww_vrmpyacc_WwWubRbI(dSum031, dline200, __m8m7m6, 1);

        sSum31L = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum031), Q6_V_lo_W(dSum031), shift);
        sSum20L = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum020), Q6_V_lo_W(dSum020), shift);
        sOut01 = Q6_Vub_vsat_VhVh(sSum31L, sSum20L);
        sOut00 = Q6_V_vlalign_VVI(sOut01, sOut01p, tmp);
        *optr0++ = sOut00;

        dSum120 = Q6_Ww_vrmpy_WubRbI(dline100, __m2m1m0, 0);
        dSum131 = Q6_Ww_vrmpy_WubRbI(dline100, __m2m1m0, 1);

        dSum120 = Q6_Ww_vrmpyacc_WwWubRbI(dSum120, dline200, __m5m4m3, 0);
        dSum131 = Q6_Ww_vrmpyacc_WwWubRbI(dSum131, dline200, __m5m4m3, 1);

        dSum120 = Q6_Ww_vrmpyacc_WwWubRbI(dSum120, dline300, __m8m7m6, 0);
        dSum131 = Q6_Ww_vrmpyacc_WwWubRbI(dSum131, dline300, __m8m7m6, 1);

        sSum31H = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum131), Q6_V_lo_W(dSum131), shift);
        sSum20H = Q6_Vh_vasr_VwVwR_sat(Q6_V_hi_W(dSum120), Q6_V_lo_W(dSum120), shift);
        sOut11 = Q6_Vub_vsat_VhVh(sSum31H, sSum20H);
        sOut10 = Q6_V_vlalign_VVI(sOut11, sOut11p, tmp);
        *optr1++ = sOut10;

        sline000 = sline064;
        sline100 = sline164;
        sline200 = sline264;
        sline300 = sline364;

        sOut01p = sOut01;
        sOut11p = sOut11;
    }
}

/* =============================================================================================== */
int conv3x3a32_i(
    unsigned char *restrict inp,
    int stride,
    int width,
    int height,
    signed char *restrict mask,
    int shift,
    unsigned char *restrict outp) {
    int i;
    unsigned char *input = inp;
    unsigned char *output = outp;
    for (i = 0; i < height; i += 2) {
        conv3x3Per2Row(input, stride, width, height, mask, shift, output, i);
        input += 2 * stride;
        output += 2 * stride;
    }
    return 0;
}

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
/*[     gaussian5x5u8                                                      ]*/
/*[                                                                        ]*/
/*[-----------------------------------------------------------------------------------------------]*/
/*[ DESCRIPTION                                                            ]*/
/*[     This function performs gaussian blur on an image with a kxk kernel ]*/
/*[                                                                        ]*/
/*[-----------------------------------------------------------------------------------------------]*/
/*[ REVISION DATE                                                          ]*/
/*[     AUG-01-2014                                                        ]*/
/*[                                                                        ]*/
/*[===============================================================================================]*/
#include "hexagon_types.h"
#include "hvx.cfg.h"
/* ===============================================================================================
 */
/*  Intrinsic C version of gaussian5x5u8()                                  */
/* =============================================================================================== */
static void gaussian5x5u8PerRow(
    unsigned char *restrict src,
    int stride,
    int width,
    int height,
    unsigned char *restrict dst,
    int y) {
    int i;
    HEXAGON_Vect32 const4, const6;

    HVX_Vector sLine0, sLine1, sLine2, sLine3, sLine4;
    HVX_Vector sVsum0, sVsum1, sVsum2, sVsum3, sVsum4, sVsum5;
    HVX_Vector sVsum1a3, sVsum2a4, sSumE, sSumO;

    HVX_VectorPair dVsumv0, dVsumv1, dVsumv2;

    HVX_Vector *iptr0 = (HVX_Vector *)(src - 2 * stride);
    HVX_Vector *iptr1 = (HVX_Vector *)(src - 1 * stride);
    HVX_Vector *iptr2 = (HVX_Vector *)(src + 0 * stride);
    HVX_Vector *iptr3 = (HVX_Vector *)(src + 1 * stride);
    HVX_Vector *iptr4 = (HVX_Vector *)(src + 2 * stride);

    if (y == 0) {
        iptr0 = iptr2;
        iptr1 = iptr2;
    } else if (y == 1) {
        iptr0 = iptr1;
    } else if (y == height - 2) {
        iptr4 = iptr3;
    } else if (y == height - 1) {
        iptr3 = iptr2;
        iptr4 = iptr2;
    }

    HVX_Vector *optr = (HVX_Vector *)(dst);

    const4 = 0x04040404;
    const6 = 0x06060606;

    unsigned int start0 = *((char *)iptr0);
    unsigned int start1 = *((char *)iptr1);
    unsigned int start2 = *((char *)iptr2);
    unsigned int start3 = *((char *)iptr3);
    unsigned int start4 = *((char *)iptr4);
    unsigned int coln1 = (start0 + start4 + 4 * (start1 + start3) + 6 * start2) << 16;

    dVsumv0 = Q6_W_vcombine_VV(Q6_V_vsplat_R(coln1), Q6_V_vsplat_R(coln1));
    dVsumv1 = dVsumv0;

    sLine0 = *iptr0++;
    sLine1 = *iptr1++;
    sLine2 = *iptr2++;
    sLine3 = *iptr3++;
    sLine4 = *iptr4++;

    dVsumv2 = Q6_Wh_vadd_VubVub(sLine0, sLine4);
    dVsumv2 = Q6_Wh_vmpyacc_WhVubRb(dVsumv2, sLine2, const6);

    dVsumv2 = Q6_Wh_vmpaacc_WhWubRb(dVsumv2, Q6_W_vcombine_VV(sLine3, sLine1), const4);

    for (i = width; i > VLEN; i -= VLEN) {
        sLine0 = *iptr0++;
        sLine1 = *iptr1++;
        sLine2 = *iptr2++;
        sLine3 = *iptr3++;
        sLine4 = *iptr4++;

        dVsumv0 = dVsumv1;
        dVsumv1 = dVsumv2;
        dVsumv2 = Q6_Wh_vadd_VubVub(sLine0, sLine4);
        dVsumv2 = Q6_Wh_vmpyacc_WhVubRb(dVsumv2, sLine2, const6);
        dVsumv2 = Q6_Wh_vmpaacc_WhWubRb(dVsumv2, Q6_W_vcombine_VV(sLine3, sLine1), const4);

        sVsum0 = Q6_V_vlalign_VVI(Q6_V_lo_W(dVsumv1), Q6_V_lo_W(dVsumv0), 2);  // even(x-2)
        sVsum1 = Q6_V_vlalign_VVI(Q6_V_hi_W(dVsumv1), Q6_V_hi_W(dVsumv0), 2);  // odd(x-2) : even(x-1)
        sVsum2 = Q6_V_lo_W(dVsumv1);                                           // even(x) : odd(x-1)
        sVsum3 = Q6_V_hi_W(dVsumv1);                                           // odd(x) : even(x+1)
        sVsum4 = Q6_V_valign_VVI(Q6_V_lo_W(dVsumv2), Q6_V_lo_W(dVsumv1), 2);   // even(x+2) : odd(x+1)
        sVsum5 = Q6_V_valign_VVI(Q6_V_hi_W(dVsumv2), Q6_V_hi_W(dVsumv1), 2);   // odd(x+2)

        sVsum1a3 = Q6_Vh_vadd_VhVh(sVsum1, sVsum3);  // even(x-1) + even(x+1)
        sVsum2a4 = Q6_Vh_vadd_VhVh(sVsum2, sVsum4);  // odd(x-1) + odd(x+1)

        sSumE = Q6_Vh_vadd_VhVh(sVsum0, sVsum4);
        sSumE = Q6_Vw_vmpyiacc_VwVwRb(sSumE, sVsum2, const6);
        sSumE = Q6_Vw_vmpyiacc_VwVwRb(sSumE, sVsum1a3, const4);
        sSumO = Q6_Vh_vadd_VhVh(sVsum1, sVsum5);
        sSumO = Q6_Vw_vmpyiacc_VwVwRb(sSumO, sVsum3, const6);
        sSumO = Q6_Vw_vmpyiacc_VwVwRb(sSumO, sVsum2a4, const4);

        *optr++ = Q6_Vb_vshuffo_VbVb(sSumO, sSumE);
    }

    {

        int tmp = VLEN - i + 1;
        unsigned int end0 = *((char *)iptr0 - tmp);
        unsigned int end1 = *((char *)iptr1 - tmp);
        unsigned int end2 = *((char *)iptr2 - tmp);
        unsigned int end3 = *((char *)iptr3 - tmp);
        unsigned int end4 = *((char *)iptr4 - tmp);
        unsigned int cole1 = end0 + end4 + 4 * (end1 + end3) + 6 * end2;

        dVsumv0 = dVsumv1;
        dVsumv1 = dVsumv2;
        dVsumv2 = Q6_W_vcombine_VV(Q6_V_vsplat_R(cole1), Q6_V_vsplat_R(cole1));

        sVsum0 = Q6_V_vlalign_VVI(Q6_V_lo_W(dVsumv1), Q6_V_lo_W(dVsumv0), 2);
        sVsum1 = Q6_V_vlalign_VVI(Q6_V_hi_W(dVsumv1), Q6_V_hi_W(dVsumv0), 2);
        sVsum2 = Q6_V_lo_W(dVsumv1);
        sVsum3 = Q6_V_hi_W(dVsumv1);
        sVsum4 = Q6_V_valign_VVI(Q6_V_lo_W(dVsumv2), Q6_V_lo_W(dVsumv1), 2);
        sVsum5 = Q6_V_valign_VVI(Q6_V_hi_W(dVsumv2), Q6_V_hi_W(dVsumv1), 2);

        sVsum1a3 = Q6_Vh_vadd_VhVh(sVsum1, sVsum3);
        sVsum2a4 = Q6_Vh_vadd_VhVh(sVsum2, sVsum4);

        sSumE = Q6_Vh_vadd_VhVh(sVsum0, sVsum4);
        sSumE = Q6_Vw_vmpyiacc_VwVwRb(sSumE, sVsum2, const6);
        sSumE = Q6_Vw_vmpyiacc_VwVwRb(sSumE, sVsum1a3, const4);
        sSumO = Q6_Vh_vadd_VhVh(sVsum1, sVsum5);
        sSumO = Q6_Vw_vmpyiacc_VwVwRb(sSumO, sVsum3, const6);
        sSumO = Q6_Vw_vmpyiacc_VwVwRb(sSumO, sVsum2a4, const4);

        *optr++ = Q6_Vb_vshuffo_VbVb(sSumO, sSumE);
    }
}

/* =============================================================================================== */
int gaussian5x5u8_i(
    unsigned char *restrict src,
    int stride,
    int width,
    int height,
    unsigned char *restrict dst) {
    int y;
    unsigned char *inp = src;
    unsigned char *outp = dst;

    for (y = 0; y < height; y++) {
        gaussian5x5u8PerRow(inp, stride, width, height, outp, y);
        inp += stride;
        outp += stride;
    }

    return 0;
}

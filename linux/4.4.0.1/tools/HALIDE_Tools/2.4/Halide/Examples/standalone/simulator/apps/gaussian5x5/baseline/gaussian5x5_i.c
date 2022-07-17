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
    unsigned char *restrict dst) {
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
    HVX_Vector *optr = (HVX_Vector *)(dst);

    const4 = 0x04040404;
    const6 = 0x06060606;

    dVsumv0 = Q6_W_vcombine_VV(Q6_V_vzero(), Q6_V_vzero());
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
        //      sLine0 = *iptr0++;
        //      sLine1 = *iptr1++;
        //      sLine2 = *iptr2++;
        //      sLine3 = *iptr3++;
        //      sLine4 = *iptr4++;

        dVsumv0 = dVsumv1;
        dVsumv1 = dVsumv2;
        //      dVsumv2 = Q6_Wh_vadd_VubVub(sLine0,sLine4);
        //      dVsumv2 = Q6_Wh_vmpyacc_WhVubRb(dVsumv2,sLine2,const6);
        //      dVsumv2 = Q6_Wh_vmpaacc_WhWubRb(dVsumv2,Q6_W_vcombine_VV(sLine3,sLine1),const4);

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

    unsigned char *inp = src + 2 * stride;
    unsigned char *outp = dst + 2 * stride;

    for (y = 2; y < height - 2; y++) {
        gaussian5x5u8PerRow(inp, stride, width, outp);

        inp += stride;
        outp += stride;
    }

    return 0;
}

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
/*[     sobel                                                              ]*/
/*[                                                                        ]*/
/*[-----------------------------------------------------------------------------------------------]*/
/*[ DESCRIPTION                                                            ]*/
/*[     This function performs a soble operator on an image. The operator  ]*/
/*[     convolves the following two 3x3 kernels with the original image    ]*/
/*[     to calculate approximations of the derivatives for horizontal and  ]*/
/*[     vertical changes.                                                  ]*/
/*[         Horizontal:                                                    ]*/
/*[              1  2  1                                                   ]*/
/*[              0  0  0                                                   ]*/
/*[             -1 -2 -1                                                   ]*/
/*[                                                                        ]*/
/*[         Vertical:                                                      ]*/
/*[              1  0  -1                                                  ]*/
/*[              2  0  -2                                                  ]*/
/*[              1  0  -1                                                  ]*/
/*[                                                                        ]*/
/*[     At each point in the image, the resulting gradient is approximated ]*/
/*[     by combining horizontal and vertical gradient magnitudes.          ]*/
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
/*  Intrinsic C version of sobel()                                          */
/* =============================================================================================== */
static void sobelPer2Row(
    unsigned char *restrict inp,
    int stride,
    int width,
    int height,
    unsigned char *restrict outp,
    int y) {
    int i;
    HEXAGON_Vect32 c121, cn11, cn22;

    HVX_Vector sLine0v0, sLine0v1, sLine1v0, sLine1v1, sLine2v0, sLine2v1,
        sLine3v0, sLine3v1;
    HVX_Vector sX00, sX02, sX10, sX12, sX20, sX22, sX30, sX32;
    HVX_Vector sEdgeH0L, sEdgeH0H, sEdgeH1L, sEdgeH1H;
    HVX_Vector sEdgeV0L, sEdgeV0H, sEdgeV1L, sEdgeV1H;
    HVX_Vector sEdge0L, sEdge0H, sEdge1L, sEdge1H;

    HVX_VectorPair dX02X00, dX12X10, dX22X20, dX32X30;
    HVX_VectorPair dHsum00, dHsum02, dHsum10, dHsum12, dVsum0, dVsum1;

    c121 = 0x02010201;
    cn11 = 0xFF01FF01;
    cn22 = 0xFE02FE02;

    HVX_Vector *iptr0 = (y == 0) ? (HVX_Vector *)(inp + 0 * stride) : (HVX_Vector *)(inp - 1 * stride);
    HVX_Vector *iptr1 = (HVX_Vector *)(inp + 0 * stride);
    HVX_Vector *iptr2 = (HVX_Vector *)(inp + 1 * stride);
    HVX_Vector *iptr3 = (y == height - 2) ? (HVX_Vector *)(inp + 1 * stride) : (HVX_Vector *)(inp + 2 * stride);
    HVX_Vector *optr0 = (HVX_Vector *)(outp + 0 * stride);
    HVX_Vector *optr1 = (HVX_Vector *)(outp + 1 * stride);

    unsigned int start0 = *((char *)iptr0);
    unsigned int start1 = *((char *)iptr1);
    unsigned int start2 = *((char *)iptr2);
    unsigned int start3 = *((char *)iptr3);

    sLine0v0 = Q6_V_vsplat_R(start0 << 24);
    sLine1v0 = Q6_V_vsplat_R(start1 << 24);
    sLine2v0 = Q6_V_vsplat_R(start2 << 24);
    sLine3v0 = Q6_V_vsplat_R(start3 << 24);

    sLine0v1 = *iptr0;
    sLine1v1 = *iptr1;
    sLine2v1 = *iptr2;
    sLine3v1 = *iptr3;

    if (width > VLEN) {
        iptr0++;
        iptr1++;
        iptr2++;
        iptr3++;
    }

    for (i = width; i > 0; i -= VLEN) {
        sX00 = Q6_V_vlalign_VVI(sLine0v1, sLine0v0, 1);
        sX10 = Q6_V_vlalign_VVI(sLine1v1, sLine1v0, 1);
        sX20 = Q6_V_vlalign_VVI(sLine2v1, sLine2v0, 1);
        sX30 = Q6_V_vlalign_VVI(sLine3v1, sLine3v0, 1);

        sLine0v0 = sLine0v1;
        sLine1v0 = sLine1v1;
        sLine2v0 = sLine2v1;
        sLine3v0 = sLine3v1;

        if (i >= 2 * VLEN) {
            sLine0v1 = *iptr0++;
            sLine1v1 = *iptr1++;
            sLine2v1 = *iptr2++;
            sLine3v1 = *iptr3++;

            sX02 = Q6_V_valign_VVI(sLine0v1, sLine0v0, 1);
            sX12 = Q6_V_valign_VVI(sLine1v1, sLine1v0, 1);
            sX22 = Q6_V_valign_VVI(sLine2v1, sLine2v0, 1);
            sX32 = Q6_V_valign_VVI(sLine3v1, sLine3v0, 1);
        } else {
            int tmp = VLEN - i + 1;
            unsigned int end0 = *((char *)iptr0 - tmp);
            unsigned int end1 = *((char *)iptr1 - tmp);
            unsigned int end2 = *((char *)iptr2 - tmp);
            unsigned int end3 = *((char *)iptr3 - tmp);

            sLine0v1 = Q6_V_vsplat_R(end0);
            sLine1v1 = Q6_V_vsplat_R(end1);
            sLine2v1 = Q6_V_vsplat_R(end2);
            sLine3v1 = Q6_V_vsplat_R(end3);

            sX02 = Q6_V_valign_VVI(sLine0v1, sLine0v0, tmp);
            sX12 = Q6_V_valign_VVI(sLine1v1, sLine1v0, tmp);
            sX22 = Q6_V_valign_VVI(sLine2v1, sLine2v0, tmp);
            sX32 = Q6_V_valign_VVI(sLine3v1, sLine3v0, tmp);
        }

        dX02X00 = Q6_W_vcombine_VV(sX02, sX00);
        dX12X10 = Q6_W_vcombine_VV(sX12, sX10);
        dX22X20 = Q6_W_vcombine_VV(sX22, sX20);
        dX32X30 = Q6_W_vcombine_VV(sX32, sX30);

        // first output row
        dHsum00 = Q6_Wh_vtmpy_WubRb(dX02X00, c121);
        dHsum02 = Q6_Wh_vtmpy_WubRb(dX22X20, c121);
        sEdgeH0L = Q6_Vuh_vabsdiff_VhVh(Q6_V_lo_W(dHsum02), Q6_V_lo_W(dHsum00));
        sEdgeH0H = Q6_Vuh_vabsdiff_VhVh(Q6_V_hi_W(dHsum02), Q6_V_hi_W(dHsum00));

        dVsum0 = Q6_Wh_vsub_VubVub(sX00, sX02);
        dVsum0 = Q6_Wh_vmpaacc_WhWubRb(dVsum0, dX12X10, cn22);
        dVsum0 = Q6_Wh_vmpaacc_WhWubRb(dVsum0, dX22X20, cn11);
        sEdgeV0L = Q6_Vh_vabs_Vh(Q6_V_lo_W(dVsum0));
        sEdgeV0H = Q6_Vh_vabs_Vh(Q6_V_hi_W(dVsum0));

        sEdge0L = Q6_Vh_vadd_VhVh(sEdgeH0L, sEdgeV0L);
        sEdge0H = Q6_Vh_vadd_VhVh(sEdgeH0H, sEdgeV0H);
        *optr0++ = Q6_Vub_vsat_VhVh(sEdge0H, sEdge0L);

        // second output row
        dHsum10 = Q6_Wh_vtmpy_WubRb(dX12X10, c121);
        dHsum12 = Q6_Wh_vtmpy_WubRb(dX32X30, c121);
        sEdgeH1L = Q6_Vuh_vabsdiff_VhVh(Q6_V_lo_W(dHsum12), Q6_V_lo_W(dHsum10));
        ;
        sEdgeH1H = Q6_Vuh_vabsdiff_VhVh(Q6_V_hi_W(dHsum12), Q6_V_hi_W(dHsum10));
        ;

        dVsum1 = Q6_Wh_vsub_VubVub(sX10, sX12);
        dVsum1 = Q6_Wh_vmpaacc_WhWubRb(dVsum1, dX22X20, cn22);
        dVsum1 = Q6_Wh_vmpaacc_WhWubRb(dVsum1, dX32X30, cn11);
        sEdgeV1L = Q6_Vh_vabs_Vh(Q6_V_lo_W(dVsum1));
        sEdgeV1H = Q6_Vh_vabs_Vh(Q6_V_hi_W(dVsum1));

        sEdge1L = Q6_Vh_vadd_VhVh(sEdgeH1L, sEdgeV1L);
        sEdge1H = Q6_Vh_vadd_VhVh(sEdgeH1H, sEdgeV1H);
        *optr1++ = Q6_Vub_vsat_VhVh(sEdge1H, sEdge1L);
    }
}

/* =============================================================================================== */
int sobel_i(
    unsigned char *in,
    int stride,
    int width,
    int height,
    unsigned char *out) {
    int i;
    unsigned char *input = in;
    unsigned char *output = out;

    for (i = 0; i < height; i += 2) {
        sobelPer2Row(input, stride, width, height, output, i);
        input += 2 * stride;
        output += 2 * stride;
    }

    return 0;
}

/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#ifndef UBWCDMA_UTILS_H
#define UBWCDMA_UTILS_H

#include "dma_def.h"
#include "dma_types.h"
#include "dmaWrapper.h"

int getFrameSize(int width, int height, int stride, t_eDmaFmt frm, bool tf) {
    t_StDmaWrapper_FrameProp pStFrameProp;
    pStFrameProp.aAddr = 0;
    pStFrameProp.u16H = height;
    pStFrameProp.u16W = width;
    pStFrameProp.u16Stride = stride;
    int sz = nDmaWrapper_GetFramesize(frm, &pStFrameProp, tf);
    return sz;
};
#endif

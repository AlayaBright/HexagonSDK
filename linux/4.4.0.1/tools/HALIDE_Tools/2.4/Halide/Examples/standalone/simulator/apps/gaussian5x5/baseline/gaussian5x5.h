/***************************************************************************
* Copyright(c) 2008-2021 QUALCOMM Technologies Inc. and/or its subsidiaries.
* All Rights Reserved
* QUALCOMM Confidential and Proprietary 
****************************************************************************/

/*! \file gaussian.h
    \brief Brief description of file
*/

/*!
Performs 5x5 Gaussian blur on an image.

\param src pointer to input buffer
\param stride stride of input image
\param width  width of the image block to be processed
\param height height of the image block to be processed
\param dst pointer to output buffer

\details

\b Assembly \b Assumptions
 - \a src must be aligned by HVX vector size
 - \a dst must be aligned by HVX vector size
 - \a strides must be a multiple of HVX vector size
 - \a height must be a multiple of 2

\b Cycle-Count
 - TO ADD

\b Notes
 - None
*/

int gaussian5x5u8_i(
    unsigned char *src,
    int stride,
    int width,
    int height,
    unsigned char *dst);

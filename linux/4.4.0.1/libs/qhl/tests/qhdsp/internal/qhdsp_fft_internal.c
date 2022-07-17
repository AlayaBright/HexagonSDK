/**=============================================================================
@file
   qhdsp_fft_internal.c

@brief
   Common routines used internally in C implementations of FFT.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include "qhdsp_fft_internal.h"

int32_t bitrev(int32_t x, int32_t bits)
{
    int32_t i;
    int32_t y = 0;

    for (i = 0; i < bits; i++)
    {
        y = (y << 1) | (x & 1);
        x >>= 1;
    }

    return y;
}

void radix2_btfly_f(float complex *x)
{
    float complex a, b;

    a = x[0] + x[1];
    b = x[0] - x[1];

    x[0] = a;
    x[1] = b;

    return;
}

void radix4_btfly_f(float complex *x)
{
    float complex a, b, c, d;

    float d_r, d_i;

    a = x[0] + x[1];
    b = x[0] - x[1];
    c = x[2] + x[3];
    d = x[2] - x[3];

    // -j*d
    //d = (0 + 1i * (-1)) * d;

    /* optimizing this mathematically:
     -i * (a+bi) = -ai - bi^2 = b - ai
     in other words, updated real part is previous imag part
     while updated imag part is negated previous real part
     this is cheaper in terms of numbers cycles w.r.t. multiplication
    */

    d_r = cimag_f(d);
    d_i = -creal_f(d);

    d = d_r + d_i*I;

    x[0] = a + c;
    x[1] = b + d;
    x[2] = a - c;
    x[3] = b - d;

    return;
}

void ifft_radix4_btfly_f(float complex *x)
{
    float complex a, b, c, d;

    float d_r, d_i;

    a = x[0] + x[1];
    b = x[0] - x[1];
    c = x[2] + x[3];
    d = x[2] - x[3];

    // j*d
    //d = (0 + 1i * 1) * d;

    /* optimizing this mathematically:
     i * (a+bi) = ai + bi^2 = -b + ai
     in other words, updated real part is negated previous imag part
     while updated imag part is previous real part
     this is cheaper in terms of numbers cycles w.r.t. multiplication
    */

    d_r = -cimag_f(d);
    d_i = creal_f(d);

    d = d_r + d_i*I;

    x[0] = a + c;
    x[1] = b + d;
    x[2] = a - c;
    x[3] = b - d;

    return;
}

int32_t cavg( int32_t x, int32_t y )
{
    int32_t zr, zi;
    zr = (int32_t)real(x) + (int32_t)real(y);
    zi = (int32_t)imag(x) + (int32_t)imag(y);

#ifdef TWOS_COMPLEMENT_ROUNDING
    zr += 1;
    zi += 1;
#else        // convergent rounding
    zr +=  (zr>>1) & 1; 
    zi +=  (zi>>1) & 1;
#endif
    zr >>= 1;
    zi >>= 1;

    return create_complex((int16_t)zr, (int16_t)zi);
}

int32_t cnavg( int32_t x, int32_t y )
{
    int32_t zr, zi;
    zr = (int32_t)real(x) - (int32_t)real(y);
    zi = (int32_t)imag(x) - (int32_t)imag(y);
#ifdef TWOS_COMPLEMENT_ROUNDING
    zr += 1;
    zi += 1;
#else        // convergent rounding
    zr +=  (zr>>1) & 1;
    zi +=  (zi>>1) & 1;
#endif
    zr >>= 1;
    zi >>= 1;

    return create_complex(saturate(zr), saturate(zi));
}

void radix2_btfly_h( int32_t *x )
{
    int32_t a,b;
    a =  cavg(x[0],x[1]);
    b = cnavg(x[0],x[1]);

    x[0] = a;
    x[1] = b;

    return;
}

void radix4_btfly_h_qv3( int32_t *x )
{
    int32_t a,b,c,d;

    a =  cavg(x[0],x[1]);
    b = cnavg(x[0],x[1]);
    c =  cavg(x[2],x[3]);
    d = cnavg(x[2],x[3]);

    // j*b
    b = create_complex(negate(imag(b)), real(b));

    x[0] =  cavg(a,c);
    x[1] =  cavg(b,d);
    x[2] = cnavg(a,c);
    x[3] = cnavg(b,d);

    return;
}

void radix4_btfly_h( int32_t *x )
{
    int32_t a,b,c,d;

    a =  cavg(x[0],x[1]);
    b = cnavg(x[0],x[1]);
    c =  cavg(x[2],x[3]);
    d = cnavg(x[2],x[3]);

    // j*d
    d = create_complex(negate(imag(d)), real(d));

    x[0] =  cavg(a,c);
    x[1] = cnavg(b,d);
    x[2] = cnavg(a,c);
    x[3] =  cavg(b,d);

    return;
}

// scaling not done in order to return to Q15 after IFFT
void ifft_radix2_btfly_h( int32_t *x )
{
    int32_t a,b;
    a = cadd(x[0],x[1]);
    b = csub(x[0],x[1]);

    x[0] = a;
    x[1] = b;

    return;
}

// scaling not done in order to return to Q15 after IFFT
void ifft_radix4_btfly_h_qv3( int32_t *x )
{
    int32_t a,b,c,d;

    a = cadd(x[0],x[1]);
    b = csub(x[0],x[1]);
    c = cadd(x[2],x[3]);
    d = csub(x[2],x[3]);

    // -j*b
    b = create_complex(imag(b), negate(real(b)));

    x[0] = cadd(a,c);
    x[1] = cadd(b,d);
    x[2] = csub(a,c);
    x[3] = csub(b,d);

    return;
}

// scaling not done in order to return to Q15 after IFFT
void ifft_radix4_btfly_h( int32_t *x )
{
    int32_t a,b,c,d;

    a =  cadd(x[0],x[1]);
    b =  csub(x[0],x[1]);
    c =  cadd(x[2],x[3]);
    d =  csub(x[2],x[3]);

    // j*d
    d = create_complex(negate(imag(d)), real(d));

    x[0] = cadd(a,c);
    x[1] = cadd(b,d);
    x[2] = csub(a,c);
    x[3] = csub(b,d);

    return;
}

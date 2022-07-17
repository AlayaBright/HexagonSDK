#ifndef _IIR_COEFFS_H_H
#define _IIR_COEFFS_H_H

#define NUM_BIQUAD_CASCADES 5
#define NUM_IIR_COEFFS 5

// a0 coeff assumed to be 1 (all coeffs divided by a0)
int16_t iir_coeffs_2kHz_LP_h[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS+1] = {
    {
        0x01d7,    // b0    //   0.014401440347
        0x03af,    // b1    //   0.028802880693
        0x6882,    // a1    //  -1.632993161855 divided by -2.0
        0x01d7,    // b2    //   0.014401440347
        0xd3ce,    // a2    //   0.690598923241 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x01d7,    // b0    //   0.014401440347
        0x03af,    // b1    //   0.028802880693
        0x6882,    // a1    //  -1.632993161855 divided by -2.0
        0x01d7,    // b2    //   0.014401440347
        0xd3ce,    // a2    //   0.690598923241 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x01d7,    // b0    //   0.014401440347
        0x03af,    // b1    //   0.028802880693
        0x6882,    // a1    //  -1.632993161855 divided by -2.0
        0x01d7,    // b2    //   0.014401440347
        0xd3ce,    // a2    //   0.690598923241 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x01d7,    // b0    //   0.014401440347
        0x03af,    // b1    //   0.028802880693
        0x6882,    // a1    //  -1.632993161855 divided by -2.0
        0x01d7,    // b2    //   0.014401440347
        0xd3ce,    // a2    //   0.690598923241 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x01d7,    // b0    //   0.014401440347
        0x03af,    // b1    //   0.028802880693
        0x6882,    // a1    //  -1.632993161855 divided by -2.0
        0x01d7,    // b2    //   0.014401440347
        0xd3ce,    // a2    //   0.690598923241 divided by -2.0
        0x0000     // shift amount
    }
};

// a0 coeff assumed to be 1 (all coeffs divided by a0)
int16_t iir_coeffs_10kHz_HP_h[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS+1] = {
    {
        0x3881,    // b0    //   0.441450095681
        0x8efe,    // b1    //  -0.882900191362
        0x23f6,    // a1    //  -0.561899583630 divided by -2.0
        0x3881,    // b2    //   0.441450095681
        0xebbd,    // a2    //   0.316611461821 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x3881,    // b0    //   0.441450095681
        0x8efe,    // b1    //  -0.882900191362
        0x23f6,    // a1    //  -0.561899583630 divided by -2.0
        0x3881,    // b2    //   0.441450095681
        0xebbd,    // a2    //   0.316611461821 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x3881,    // b0    //   0.441450095681
        0x8efe,    // b1    //  -0.882900191362
        0x23f6,    // a1    //  -0.561899583630 divided by -2.0
        0x3881,    // b2    //   0.441450095681
        0xebbd,    // a2    //   0.316611461821 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x3881,    // b0    //   0.441450095681
        0x8efe,    // b1    //  -0.882900191362
        0x23f6,    // a1    //  -0.561899583630 divided by -2.0
        0x3881,    // b2    //   0.441450095681
        0xebbd,    // a2    //   0.316611461821 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x3881,    // b0    //   0.441450095681
        0x8efe,    // b1    //  -0.882900191362
        0x23f6,    // a1    //  -0.561899583630 divided by -2.0
        0x3881,    // b2    //   0.441450095681
        0xebbd,    // a2    //   0.316611461821 divided by -2.0
        0x0000     // shift amount
    }
};

#endif // _IIR_COEFFS_H_H
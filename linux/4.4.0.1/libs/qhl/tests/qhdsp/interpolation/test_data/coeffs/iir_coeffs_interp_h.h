#ifndef _IIR_COEFFS_INTERP_H_H
#define _IIR_COEFFS_INTERP_H_H

#define NUM_BIQUAD_CASCADES 5
#define NUM_IIR_COEFFS 5

// a0 coeff assumed to be 1 (all coeffs divided by a0)
int16_t iir_coeffs_24kHz_LP_h[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS+1] = {
    {
        0x1e7b,    // b0    //   0.238129151665
        0x3cf6,    // b1    //   0.476258303331
        0x1121,    // a1    //  -0.267689941602 divided by -2.0
        0x1e7b,    // b2    //   0.238129151665
        0xee05,    // a2    //   0.281005480603 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x1e7b,    // b0    //   0.238129151665
        0x3cf6,    // b1    //   0.476258303331
        0x1121,    // a1    //  -0.267689941602 divided by -2.0
        0x1e7b,    // b2    //   0.238129151665
        0xee05,    // a2    //   0.281005480603 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x1e7b,    // b0    //   0.238129151665
        0x3cf6,    // b1    //   0.476258303331
        0x1121,    // a1    //  -0.267689941602 divided by -2.0
        0x1e7b,    // b2    //   0.238129151665
        0xee05,    // a2    //   0.281005480603 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x1e7b,    // b0    //   0.238129151665
        0x3cf6,    // b1    //   0.476258303331
        0x1121,    // a1    //  -0.267689941602 divided by -2.0
        0x1e7b,    // b2    //   0.238129151665
        0xee05,    // a2    //   0.281005480603 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x1e7b,    // b0    //   0.238129151665
        0x3cf6,    // b1    //   0.476258303331
        0x1121,    // a1    //  -0.267689941602 divided by -2.0
        0x1e7b,    // b2    //   0.238129151665
        0xee05,    // a2    //   0.281005480603 divided by -2.0
        0x0000     // shift amount
    }
};

#endif // _IIR_COEFFS_INTERP_H_H
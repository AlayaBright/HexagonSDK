#ifndef _IIR_COEFFS_INTERP_F_H
#define _IIR_COEFFS_INTERP_F_H

#define NUM_BIQUAD_CASCADES 5
#define NUM_IIR_COEFFS 5

// a0 coeff assumed to be 1 (all coeffs divided by a0)
float iir_coeffs_24kHz_LP_f[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS] = {
    {
        0.238129,    // b0
        0.476258,    // b1
        -0.267690,    // a1
        0.238129,    // b2
        0.281005     // a2
    },
    {
        0.238129,    // b0
        0.476258,    // b1
        -0.267690,    // a1
        0.238129,    // b2
        0.281005     // a2
    },
    {
        0.238129,    // b0
        0.476258,    // b1
        -0.267690,    // a1
        0.238129,    // b2
        0.281005     // a2
    },
    {
        0.238129,    // b0
        0.476258,    // b1
        -0.267690,    // a1
        0.238129,    // b2
        0.281005     // a2
    },
    {
        0.238129,    // b0
        0.476258,    // b1
        -0.267690,    // a1
        0.238129,    // b2
        0.281005     // a2
    }
};

#endif // _IIR_COEFFS_INTERP_F_H
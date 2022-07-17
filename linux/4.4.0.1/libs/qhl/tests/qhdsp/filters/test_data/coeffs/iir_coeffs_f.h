#ifndef _IIR_COEFFS_F_H
#define _IIR_COEFFS_F_H

#define NUM_BIQUAD_CASCADES 5
#define NUM_IIR_COEFFS 5

// a0 coeff assumed to be 1 (all coeffs divided by a0)
float iir_coeffs_2kHz_LP_f[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS] = {
    {
        0.014401,    // b0
        0.028803,    // b1
        -1.632993,    // a1
        0.014401,    // b2
        0.690599     // a2
    },
    {
        0.014401,    // b0
        0.028803,    // b1
        -1.632993,    // a1
        0.014401,    // b2
        0.690599     // a2
    },
    {
        0.014401,    // b0
        0.028803,    // b1
        -1.632993,    // a1
        0.014401,    // b2
        0.690599     // a2
    },
    {
        0.014401,    // b0
        0.028803,    // b1
        -1.632993,    // a1
        0.014401,    // b2
        0.690599     // a2
    },
    {
        0.014401,    // b0
        0.028803,    // b1
        -1.632993,    // a1
        0.014401,    // b2
        0.690599     // a2
    }
};

// a0 coeff assumed to be 1 (all coeffs divided by a0)
float iir_coeffs_10kHz_HP_f[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS] = {
    {
        0.441450,    // b0
        -0.882900,    // b1
        -0.561900,    // a1
        0.441450,    // b2
        0.316611     // a2
    },
    {
        0.441450,    // b0
        -0.882900,    // b1
        -0.561900,    // a1
        0.441450,    // b2
        0.316611     // a2
    },
    {
        0.441450,    // b0
        -0.882900,    // b1
        -0.561900,    // a1
        0.441450,    // b2
        0.316611     // a2
    },
    {
        0.441450,    // b0
        -0.882900,    // b1
        -0.561900,    // a1
        0.441450,    // b2
        0.316611     // a2
    },
    {
        0.441450,    // b0
        -0.882900,    // b1
        -0.561900,    // a1
        0.441450,    // b2
        0.316611     // a2
    }
};

#endif // _IIR_COEFFS_F_H
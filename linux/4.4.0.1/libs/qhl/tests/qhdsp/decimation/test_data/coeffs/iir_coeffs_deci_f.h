#ifndef _IIR_COEFFS_DECI_F_H
#define _IIR_COEFFS_DECI_F_H

#define NUM_BIQUAD_CASCADES 5
#define NUM_IIR_COEFFS 5

// a0 coeff assumed to be 1 (all coeffs divided by a0)
float iir_coeffs_12kHz_LP_f[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS] = {
    {
        0.357339,    // b0
        0.714678,    // b1
        0.241509,    // a1
        0.357339,    // b2
        0.279082     // a2
    },
    {
        0.357339,    // b0
        0.714678,    // b1
        0.241509,    // a1
        0.357339,    // b2
        0.279082     // a2
    },
    {
        0.357339,    // b0
        0.714678,    // b1
        0.241509,    // a1
        0.357339,    // b2
        0.279082     // a2
    },
    {
        0.357339,    // b0
        0.714678,    // b1
        0.241509,    // a1
        0.357339,    // b2
        0.279082     // a2
    },
    {
        0.357339,    // b0
        0.714678,    // b1
        0.241509,    // a1
        0.357339,    // b2
        0.279082     // a2
    }
};

// a0 coeff assumed to be 1 (all coeffs divided by a0)
float iir_coeffs_6kHz_LP_f[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS] = {
    {
        0.129366,    // b0
        0.258732,    // b1
        -0.819417,    // a1
        0.129366,    // b2
        0.369911     // a2
    },
    {
        0.129366,    // b0
        0.258732,    // b1
        -0.819417,    // a1
        0.129366,    // b2
        0.369911     // a2
    },
    {
        0.129366,    // b0
        0.258732,    // b1
        -0.819417,    // a1
        0.129366,    // b2
        0.369911     // a2
    },
    {
        0.129366,    // b0
        0.258732,    // b1
        -0.819417,    // a1
        0.129366,    // b2
        0.369911     // a2
    },
    {
        0.129366,    // b0
        0.258732,    // b1
        -0.819417,    // a1
        0.129366,    // b2
        0.369911     // a2
    }
};

#endif // _IIR_COEFFS_DECI_F_H
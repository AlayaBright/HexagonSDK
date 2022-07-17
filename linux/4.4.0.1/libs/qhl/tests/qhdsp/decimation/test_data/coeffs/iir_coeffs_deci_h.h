#ifndef _IIR_COEFFS_DECI_H_H
#define _IIR_COEFFS_DECI_H_H

#define NUM_BIQUAD_CASCADES 5
#define NUM_IIR_COEFFS 5

// a0 coeff assumed to be 1 (all coeffs divided by a0)
int16_t iir_coeffs_12kHz_LP_h[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS+1] = {
    {
        0x2dbd,    // b0    //   0.357338830670
        0x5b7a,    // b1    //   0.714677661339
        0xf08c,    // a1    //   0.241508677065 divided by -2.0
        0x2dbd,    // b2    //   0.357338830670
        0xee24,    // a2    //   0.279082091742 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x2dbd,    // b0    //   0.357338830670
        0x5b7a,    // b1    //   0.714677661339
        0xf08c,    // a1    //   0.241508677065 divided by -2.0
        0x2dbd,    // b2    //   0.357338830670
        0xee24,    // a2    //   0.279082091742 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x2dbd,    // b0    //   0.357338830670
        0x5b7a,    // b1    //   0.714677661339
        0xf08c,    // a1    //   0.241508677065 divided by -2.0
        0x2dbd,    // b2    //   0.357338830670
        0xee24,    // a2    //   0.279082091742 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x2dbd,    // b0    //   0.357338830670
        0x5b7a,    // b1    //   0.714677661339
        0xf08c,    // a1    //   0.241508677065 divided by -2.0
        0x2dbd,    // b2    //   0.357338830670
        0xee24,    // a2    //   0.279082091742 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x2dbd,    // b0    //   0.357338830670
        0x5b7a,    // b1    //   0.714677661339
        0xf08c,    // a1    //   0.241508677065 divided by -2.0
        0x2dbd,    // b2    //   0.357338830670
        0xee24,    // a2    //   0.279082091742 divided by -2.0
        0x0000     // shift amount
    }
};

// a0 coeff assumed to be 1 (all coeffs divided by a0)
int16_t iir_coeffs_6kHz_LP_h[NUM_BIQUAD_CASCADES][NUM_IIR_COEFFS+1] = {
    {
        0x108f,    // b0    //   0.129366167650
        0x211e,    // b1    //   0.258732335299
        0x3471,    // a1    //  -0.819416521168 divided by -2.0
        0x108f,    // b2    //   0.129366167650
        0xe854,    // a2    //   0.369910851592 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x108f,    // b0    //   0.129366167650
        0x211e,    // b1    //   0.258732335299
        0x3471,    // a1    //  -0.819416521168 divided by -2.0
        0x108f,    // b2    //   0.129366167650
        0xe854,    // a2    //   0.369910851592 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x108f,    // b0    //   0.129366167650
        0x211e,    // b1    //   0.258732335299
        0x3471,    // a1    //  -0.819416521168 divided by -2.0
        0x108f,    // b2    //   0.129366167650
        0xe854,    // a2    //   0.369910851592 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x108f,    // b0    //   0.129366167650
        0x211e,    // b1    //   0.258732335299
        0x3471,    // a1    //  -0.819416521168 divided by -2.0
        0x108f,    // b2    //   0.129366167650
        0xe854,    // a2    //   0.369910851592 divided by -2.0
        0x0000     // shift amount
    },
    {
        0x108f,    // b0    //   0.129366167650
        0x211e,    // b1    //   0.258732335299
        0x3471,    // a1    //  -0.819416521168 divided by -2.0
        0x108f,    // b2    //   0.129366167650
        0xe854,    // a2    //   0.369910851592 divided by -2.0
        0x0000     // shift amount
    }
};

#endif // _IIR_COEFFS_DECI_H_H
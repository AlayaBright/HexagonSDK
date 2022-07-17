/**=============================================================================

@file
   qfe.c

@brief
    A qfloat16, qfloat32 emulation library providing APIs for
    basic arithmetic, conversion and math functions and
    utilities to compare accuracy across different data types.

Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include <hexagon_protos.h>

#include "qfe.h"
#include "qhmath_hvx.h"


////////////////////////////////////////////////////////////////////////////
// QFES operations: Operations consuming and producing single values

/* Conversion Functions */

_Float16 qfes_hf_equals_sf(float sf)
{
    HVX_Vector vzero = Q6_V_vzero();
    QFE_V   vin   = { .sf = sf };
    HVX_Vector vq32  = Q6_Vqf32_vadd_VsfVsf(vin.v, vzero);

    QFE_V vout = { .v = Q6_Vhf_equals_Wqf32(Q6_W_vcombine_VV(vq32,vq32)) };

    return vout.hf;
}

_Float16 qfes_hf_equals_qf16(qf16_t in)
{
    QFE_V vin  = { .qf16 = in };

    QFE_V vout = { .v = Q6_Vhf_equals_Vqf16(vin.v) };

    return vout.hf;
}

_Float16 qfes_hf_equals_qf32(qf32_t in)
{
    QFE_V vin     = { .qf32 = in };
    HVX_Vector vzero = Q6_V_vzero();
    HVX_Vector vq32  = Q6_Vqf32_vadd_Vqf32Vsf(vin.v, vzero);

    QFE_V vout = { .v = Q6_Vhf_equals_Wqf32(Q6_W_vcombine_VV(vq32,vq32)) };

    return vout.hf;
}

float qfes_sf_equals_hf(_Float16 hf)
{
    QFE_V vhf   = { .hf = hf };
    QFE_V vone  = { .hf = (_Float16) (0x3c00) };
    HVX_Vector vqf32 = Q6_V_lo_W(Q6_Wqf32_vmpy_VhfVhf(vhf.v, vone.v));

    QFE_V vout = { .v = Q6_Vsf_equals_Vqf32(vqf32) };

    return vout.sf;
}

float qfes_sf_equals_qf16(qf16_t in)
{
    QFE_V vin   = { .qf16 = in };
    QFE_V vone  = { .hf = (_Float16) (0x3c00) };
    HVX_Vector vqf32 = Q6_V_lo_W(Q6_Wqf32_vmpy_Vqf16Vhf(vin.v, vone.v));

    QFE_V vout = { .v = Q6_Vsf_equals_Vqf32(vqf32) };

    return vout.sf;
}

float qfes_sf_equals_qf32(qf32_t in)
{
    QFE_V vin  = { .qf32 = in };

    QFE_V vout = { .v = Q6_Vsf_equals_Vqf32(vin.v) };

    return vout.sf;
}

qf16_t qfes_qf16_equals_hf(_Float16 hf)
{
    QFE_V vin   = { .hf = hf };
    HVX_Vector vzero = Q6_V_vzero();

    QFE_V vout = { .v = Q6_Vqf16_vadd_VhfVhf(vin.v, vzero) };

    return vout.qf16;
}

qf16_t qfes_qf16_equals_sf(float sf)
{
    HVX_Vector vzero = Q6_V_vzero();  // zero in sf and hf
    QFE_V vin     = { .sf = sf };
    HVX_Vector vq32  = Q6_Vqf32_vadd_VsfVsf(vin.v, vzero);
    HVX_Vector vhf   = Q6_Vhf_equals_Wqf32(Q6_W_vcombine_VV(vq32, vq32));

    QFE_V vout = { .v = Q6_Vqf16_vadd_VhfVhf(vhf, vzero) };

    return vout.qf16;
}

qf16_t qfes_qf16_equals_qf32(qf32_t in)
{
    QFE_V vin     = { .qf32 = in };
    HVX_Vector vhf   = Q6_Vhf_equals_Wqf32(Q6_W_vcombine_VV(vin.v, vin.v));
    HVX_Vector vzero = Q6_V_vzero();

    QFE_V vout = { .v = Q6_Vqf16_vadd_VhfVhf(vhf, vzero) };

    return vout.qf16;
}

qf32_t qfes_qf32_equals_hf(_Float16 hf)
{
    QFE_V vin  = { .hf = hf };
    QFE_V vone = { .hf = (_Float16) (0x3c00) };

    QFE_V vout = { .v = Q6_V_lo_W(Q6_Wqf32_vmpy_VhfVhf(vin.v, vone.v)) };

    return vout.qf32;
}

qf32_t qfes_qf32_equals_sf(float sf)
{
    HVX_Vector vzero = Q6_V_vzero();
    QFE_V vin     = { .sf = sf };

    QFE_V vout = { .v = Q6_Vqf32_vadd_VsfVsf(vin.v, vzero) };

    return vout.qf32;
}

qf32_t qfes_qf32_equals_qf16(qf16_t in)
{
    QFE_V vin  = { .qf16 = in };
    QFE_V vone = { .hf = (_Float16) (0x3c00) };

    QFE_V vout = { .v = Q6_V_lo_W(Q6_Wqf32_vmpy_Vqf16Vhf(vin.v, vone.v)) };

    return vout.qf32;
}


/* Helper functions for displaying output */

void qfes_printf_qf16(qf16_t in) {
    float sf=qfes_sf_equals_qf16(in);
    printf("(%f,0x%x)\n",sf, 0xffff & *((uint32_t *)&in));
}

void qfes_strcat_qf16(char* string, qf16_t in) {
    float sf=qfes_sf_equals_qf16(in);
    char regString[128];
    sprintf(regString,"(%f,0x%x)",sf, 0xffff & *((uint32_t *)&in));
    strcat(string,regString);
}

void qfes_printf_qf32(qf32_t in) {
    float sf=qfes_sf_equals_qf32(in);
    printf("(%f,0x%x)\n",sf, *((uint32_t *)&in));
}

void qfes_strcat_qf32(char* string, qf32_t in) {
    float sf=qfes_sf_equals_qf32(in);
    char regString[128];
    sprintf(regString,"(%f,0x%x)",sf, *((uint32_t *)&in));
    strcat(string,regString);
}


/* Half-Precision Addition */

qf16_t qfes_qf16_add_qf16qf16(qf16_t a, qf16_t b) {

    QFE_V va = { .qf16 = a };
    QFE_V vb = { .qf16 = b };

    QFE_V vout = { .v = Q6_Vqf16_vadd_Vqf16Vqf16(va.v, vb.v) };

    return vout.qf16;
}

qf16_t qfes_qf16_add_qf16hf(qf16_t a, _Float16 b) {

    QFE_V va = { .qf16 = a };
    QFE_V vb = { .hf = b };

    QFE_V vout = { .v = Q6_Vqf16_vadd_Vqf16Vhf(va.v, vb.v) };

    return vout.qf16;
}

qf16_t qfes_qf16_add_hfhf(_Float16 a, _Float16 b) {

    QFE_V va = { .hf = a };
    QFE_V vb = { .hf = b };

    QFE_V vout = { .v = Q6_Vqf16_vadd_VhfVhf(va.v, vb.v) };

    return vout.qf16;
}


/* Single-Precision Addition */

qf32_t qfes_qf32_add_qf32qf32(qf32_t a, qf32_t b) {

    QFE_V va = { .qf32 = a };
    QFE_V vb = { .qf32 = b };

    QFE_V vout = { .v = Q6_Vqf32_vadd_Vqf32Vqf32(va.v, vb.v) };

    return vout.qf32;
}

qf32_t qfes_qf32_add_qf32sf(qf32_t a, float b) {

    QFE_V va = { .qf32 = a };
    QFE_V vb = { .sf = b };

    QFE_V vout = { .v = Q6_Vqf32_vadd_Vqf32Vsf(va.v, vb.v) };

    return vout.qf32;
}

qf32_t qfes_qf32_add_sfsf(float a, float b) {

    QFE_V va = { .sf = a };
    QFE_V vb = { .sf = b };

    QFE_V vout = { .v = Q6_Vqf32_vadd_VsfVsf(va.v, vb.v) };

    return vout.qf32;
}


/* Half-Precision Subtraction */

qf16_t qfes_qf16_sub_qf16qf16(qf16_t a, qf16_t b) {

    QFE_V va = { .qf16 = a };
    QFE_V vb = { .qf16 = b };

    QFE_V vout = { .v = Q6_Vqf16_vsub_Vqf16Vqf16(va.v, vb.v) };

    return vout.qf16;
}

qf16_t qfes_qf16_sub_qf16hf(qf16_t a, _Float16 b) {

    QFE_V va = { .qf16 = a };
    QFE_V vb = { .hf = b };

    QFE_V vout = { .v = Q6_Vqf16_vsub_Vqf16Vhf(va.v, vb.v) };

    return vout.qf16;
}

qf16_t qfes_qf16_sub_hfhf(_Float16 a, _Float16 b) {

    QFE_V va = { .hf = a };
    QFE_V vb = { .hf = b };

    QFE_V vout = { .v = Q6_Vqf16_vsub_VhfVhf(va.v, vb.v) };

    return vout.qf16;
}


/* Single-Precision Subtraction */

qf32_t qfes_qf32_sub_qf32qf32(qf32_t a, qf32_t b) {

    QFE_V va = { .qf32 = a };
    QFE_V vb = { .qf32 = b };

    QFE_V vout = { .v = Q6_Vqf32_vsub_Vqf32Vqf32(va.v, vb.v) };

    return vout.qf32;
}

qf32_t qfes_qf32_sub_qf32sf(qf32_t a, float b) {

    QFE_V va = { .qf32 = a };
    QFE_V vb = { .sf = b };

    QFE_V vout = { .v = Q6_Vqf32_vsub_Vqf32Vsf(va.v, vb.v) };

    return vout.qf32;
}

qf32_t qfes_qf32_sub_sfsf(float a, float b) {

    QFE_V va = { .sf = a };
    QFE_V vb = { .sf = b };

    QFE_V vout = { .v = Q6_Vqf32_vsub_VsfVsf(va.v, vb.v) };

    return vout.qf32;
}


/* Half-Precision Multiplication : 16-bit output */

qf16_t qfes_qf16_mpy_qf16qf16(qf16_t a, qf16_t b) {

    QFE_V va = { .qf16 = a };
    QFE_V vb = { .qf16 = b };

    QFE_V vout = { .v = Q6_Vqf16_vmpy_Vqf16Vqf16(va.v, vb.v) };

    return vout.qf16;
}

qf16_t qfes_qf16_mpy_qf16hf(qf16_t a, _Float16 b) {

    QFE_V va = { .qf16 = a };
    QFE_V vb = { .hf = b };

    QFE_V vout = { .v = Q6_Vqf16_vmpy_Vqf16Vhf(va.v, vb.v) };

    return vout.qf16;
}

qf16_t qfes_qf16_mpy_hfhf(_Float16 a, _Float16 b) {

    QFE_V va = { .hf = a };
    QFE_V vb = { .hf = b };

    QFE_V vout = { .v = Q6_Vqf16_vmpy_VhfVhf(va.v, vb.v) };

    return vout.qf16;
}


/* Half-Precision Multiplication : 32-bit output*/

qf32_t qfes_qf32_mpy_qf16qf16(qf16_t a, qf16_t b) {

    QFE_V va = { .qf16 = a };
    QFE_V vb = { .qf16 = b };

    QFE_V vout = { .v = Q6_V_lo_W(Q6_Wqf32_vmpy_Vqf16Vqf16(va.v, vb.v)) };

    return vout.qf32;
}

qf32_t qfes_qf32_mpy_qf16hf(qf16_t a, _Float16 b) {

    QFE_V va = { .qf16 = a };
    QFE_V vb = { .hf = b };

    QFE_V vout = { .v = Q6_V_lo_W(Q6_Wqf32_vmpy_Vqf16Vhf(va.v, vb.v)) };

    return vout.qf32;
}

qf32_t qfes_qf32_mpy_hfhf(_Float16 a, _Float16 b) {

    QFE_V va = { .hf = a };
    QFE_V vb = { .hf = b };

    QFE_V vout = { .v = Q6_V_lo_W(Q6_Wqf32_vmpy_VhfVhf(va.v, vb.v)) };

    return vout.qf32;
}


/* Single-Precision Multiplication  */

qf32_t qfes_qf32_mpy_qf32qf32(qf32_t a, qf32_t b) {

    QFE_V va = { .qf32 = a };
    QFE_V vb = { .qf32 = b };

    QFE_V vout = { .v = Q6_Vqf32_vmpy_Vqf32Vqf32(va.v, vb.v) };

    return vout.qf32;
}

qf32_t qfes_qf32_mpy_sfsf(float a, float b) {

    QFE_V va = { .sf = a };
    QFE_V vb = { .sf = b };

    QFE_V vout = { .v = Q6_Vqf32_vmpy_VsfVsf(va.v, vb.v) };

    return vout.qf32;
}

/* Half-Precision Division  */

qf16_t qfes_qf16_div_qf16qf16(qf16_t a, qf16_t b) {

    int num_args = 2;
    return qfes_qf16_generic_function((fptr_hf)qhmath_hvx_div_ahf, a, b, num_args);
}


/* Single-Precision Division */

qf32_t qfes_qf32_div_qf32qf32(qf32_t a, qf32_t b) {

    int num_args = 2;
    return qfes_qf32_generic_function((fptr_sf)qhmath_hvx_div_af, a, b, num_args);
}


/* Half-Precision Multiplicative Inverse */

qf16_t qfes_qf16_inv_qf16(qf16_t a) {

    int num_args = 1;
    return qfes_qf16_generic_function((fptr_hf)qhmath_hvx_inv_ahf, a, 0, num_args);
}


/* Single-Precision Multiplicative Inverse */

qf32_t qfes_qf32_inv_qf32(qf32_t a) {

    int num_args = 1;
    return qfes_qf32_generic_function((fptr_sf)qhmath_hvx_inv_af, a, 0, num_args);
}





////////////////////////////////////////////////////////////////////////////
// QFE operations: Operations consuming and producing QF values

void qfe_init_sf(float init_value, qf_t* out) {
    out->ref_df=init_value;
    out->ref_sf=init_value;
    out->ref_qf32=qfes_qf32_equals_sf(init_value);
    out->ref_qf16=qfes_qf16_equals_sf(init_value);

    out->actual_sf = init_value;
    out->actual_type = SF;
}

/* Conversion Functions  */

void qfe_hf_equals_sf(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_hf = qfes_hf_equals_sf(in.actual_sf);
    out->actual_type = HF;
}

void qfe_hf_equals_qf16(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_hf = qfes_hf_equals_qf16(in.actual_qf16);
    out->actual_type = HF;
}

void qfe_hf_equals_qf32(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_hf = qfes_hf_equals_qf32(in.actual_qf32);
    out->actual_type = HF;
}

void qfe_sf_equals_hf(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_sf = qfes_sf_equals_hf(in.actual_hf);
    out->actual_type = SF;
}

void qfe_sf_equals_qf16(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_sf = qfes_sf_equals_qf16(in.actual_qf16);
    out->actual_type = SF;
}

void qfe_sf_equals_qf32(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_sf = qfes_sf_equals_qf32(in.actual_qf32);
    out->actual_type = SF;
}

void qfe_qf16_equals_hf(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_qf16 = qfes_qf16_equals_hf(in.actual_hf);
    out->actual_type = QF16;
}

void qfe_qf16_equals_sf(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_qf16 = qfes_qf16_equals_sf(in.actual_sf);
    out->actual_type = QF16;
}

void qfe_qf16_equals_qf32(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_qf16 = qfes_qf16_equals_qf32(in.actual_qf32);
    out->actual_type = QF16;
}

void qfe_qf32_equals_hf(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_qf32 = qfes_qf32_equals_hf(in.actual_hf);
    out->actual_type = QF32;
}

void qfe_qf32_equals_sf(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_qf32 = qfes_qf32_equals_sf(in.actual_sf);
    out->actual_type = QF32;
}

void qfe_qf32_equals_qf16(qf_t in, qf_t* out) {
    out->ref_df=in.ref_df;
    out->ref_sf=in.ref_sf;
    out->ref_qf32=in.ref_qf32;
    out->ref_qf16=in.ref_qf16;

    out->actual_qf32 = qfes_qf32_equals_qf16(in.actual_qf16);
    out->actual_type = QF32;
}

/* Half-Precision Addition */

void qfe_qf16_add_qf16qf16(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df+b.ref_df;
    out->ref_sf=a.ref_sf+b.ref_sf;
    out->ref_qf32=qfes_qf32_add_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_add_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf16_add_qf16qf16(a.actual_qf16,b.actual_qf16);
    out->actual_type = QF16;
}

void qfe_qf16_add_qf16hf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df+b.ref_df;
    out->ref_sf=a.ref_sf+b.ref_sf;
    out->ref_qf32=qfes_qf32_add_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_add_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf16_add_qf16hf(a.actual_qf16,b.actual_hf);
    out->actual_type = QF16;
}

void qfe_qf16_add_hfhf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df+b.ref_df;
    out->ref_sf=a.ref_sf+b.ref_sf;
    out->ref_qf32=qfes_qf32_add_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_add_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf16_add_hfhf(a.actual_hf,b.actual_hf);
    out->actual_type = QF16;
}

/* Single-Precision Addition */

void qfe_qf32_add_qf32qf32(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df+b.ref_df;
    out->ref_sf=a.ref_sf+b.ref_sf;
    out->ref_qf32=qfes_qf32_add_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_add_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf32 = qfes_qf32_add_qf32qf32(a.actual_qf32,b.actual_qf32);
    out->actual_type = QF32;
}

void qfe_qf32_add_qf32sf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df+b.ref_df;
    out->ref_sf=a.ref_sf+b.ref_sf;
    out->ref_qf32=qfes_qf32_add_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_add_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf32 = qfes_qf32_add_qf32sf(a.actual_qf32,b.actual_sf);
    out->actual_type = QF32;
}

void qfe_qf32_add_sfsf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df+b.ref_df;
    out->ref_sf=a.ref_sf+b.ref_sf;
    out->ref_qf32=qfes_qf32_add_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_add_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf32 = qfes_qf32_add_sfsf(a.actual_sf,b.actual_sf);
    out->actual_type = QF32;
}

/* Half-Precision Subtraction */

void qfe_qf16_sub_qf16qf16(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df-b.ref_df;
    out->ref_sf=a.ref_sf-b.ref_sf;
    out->ref_qf32=qfes_qf32_sub_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_sub_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf16_sub_qf16qf16(a.actual_qf16,b.actual_qf16);
    out->actual_type = QF16;
}

void qfe_qf16_sub_qf16hf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df-b.ref_df;
    out->ref_sf=a.ref_sf-b.ref_sf;
    out->ref_qf32=qfes_qf32_sub_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_sub_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf16_sub_qf16hf(a.actual_qf16,b.actual_hf);
    out->actual_type = QF16;
}

void qfe_qf16_sub_hfhf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df-b.ref_df;
    out->ref_sf=a.ref_sf-b.ref_sf;
    out->ref_qf32=qfes_qf32_sub_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_sub_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf16_sub_hfhf(a.actual_hf,b.actual_hf);
    out->actual_type = QF16;
}

/* Single-Precision Subtraction */

void qfe_qf32_sub_qf32qf32(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df-b.ref_df;
    out->ref_sf=a.ref_sf-b.ref_sf;
    out->ref_qf32=qfes_qf32_sub_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_sub_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf32 = qfes_qf32_sub_qf32qf32(a.actual_qf32,b.actual_qf32);
    out->actual_type = QF32;
}

void qfe_qf32_sub_qf32sf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df-b.ref_df;
    out->ref_sf=a.ref_sf-b.ref_sf;
    out->ref_qf32=qfes_qf32_sub_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_sub_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf32 = qfes_qf32_sub_qf32sf(a.actual_qf32,b.actual_sf);
    out->actual_type = QF32;
}

void qfe_qf32_sub_sfsf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df-b.ref_df;
    out->ref_sf=a.ref_sf-b.ref_sf;
    out->ref_qf32=qfes_qf32_sub_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_sub_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf32 = qfes_qf32_sub_sfsf(a.actual_sf,b.actual_sf);
    out->actual_type = QF32;
}

/* Half-Precision Multiplication : 16-bit output */

void qfe_qf16_mpy_qf16qf16(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df*b.ref_df;
    out->ref_sf=a.ref_sf*b.ref_sf;
    out->ref_qf32=qfes_qf32_mpy_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_mpy_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf16_mpy_qf16qf16(a.actual_qf16,b.actual_qf16);
    out->actual_type = QF16;
}

void qfe_qf16_mpy_qf16hf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df*b.ref_df;
    out->ref_sf=a.ref_sf*b.ref_sf;
    out->ref_qf32=qfes_qf32_mpy_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_mpy_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf16_mpy_qf16hf(a.actual_qf16,b.actual_hf);
    out->actual_type = QF16;
}

void qfe_qf16_mpy_hfhf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df*b.ref_df;
    out->ref_sf=a.ref_sf*b.ref_sf;
    out->ref_qf32=qfes_qf32_mpy_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_mpy_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf16_mpy_hfhf(a.actual_hf,b.actual_hf);
    out->actual_type = QF16;
}

/* Half-Precision Multiplication : 32-bit output */

void qfe_qf32_mpy_qf16qf16(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df*b.ref_df;
    out->ref_sf=a.ref_sf*b.ref_sf;
    out->ref_qf32=qfes_qf32_mpy_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_mpy_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf32 = qfes_qf32_mpy_qf16qf16(a.actual_qf16,b.actual_qf16);
    out->actual_type = QF32;
}

void qfe_qf32_mpy_qf16hf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df*b.ref_df;
    out->ref_sf=a.ref_sf*b.ref_sf;
    out->ref_qf32=qfes_qf32_mpy_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_mpy_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf32_mpy_qf16hf(a.actual_qf16,b.actual_hf);
    out->actual_type = QF32;
}

void qfe_qf32_mpy_hfhf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df*b.ref_df;
    out->ref_sf=a.ref_sf*b.ref_sf;
    out->ref_qf32=qfes_qf32_mpy_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_mpy_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf16 = qfes_qf32_mpy_hfhf(a.actual_hf,b.actual_hf);
    out->actual_type = QF32;
}

/* Single-Precision Multiplication  */

void qfe_qf32_mpy_qf32qf32(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df*b.ref_df;
    out->ref_sf=a.ref_sf*b.ref_sf;
    out->ref_qf32=qfes_qf32_mpy_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_mpy_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf32 = qfes_qf32_mpy_qf32qf32(a.actual_qf32,b.actual_qf32);
    out->actual_type = QF32;
}

void qfe_qf32_mpy_sfsf(qf_t a, qf_t b, qf_t* out) {
    out->ref_df=a.ref_df*b.ref_df;
    out->ref_sf=a.ref_sf*b.ref_sf;
    out->ref_qf32=qfes_qf32_mpy_qf32qf32(a.ref_qf32,b.ref_qf32);
    out->ref_qf16=qfes_qf16_mpy_qf16qf16(a.ref_qf16,b.ref_qf16);

    out->actual_qf32 = qfes_qf32_mpy_sfsf(a.actual_sf,b.actual_sf);
    out->actual_type = QF32;
}

void qfe_qf16_div_qf16qf16(qf_t a, qf_t b, qf_t* out) {
    out->ref_df = a.ref_df/b.ref_df;
    out->ref_sf = a.ref_sf/b.ref_sf;
    out->ref_qf32 = qfes_qf32_div_qf32qf32(a.ref_qf32, b.ref_qf32);
    out->ref_qf16 = qfes_qf16_div_qf16qf16(a.ref_qf16, b.ref_qf16);

    out->actual_qf16 = qfes_qf16_div_qf16qf16(a.actual_qf16, b.actual_qf16);
    out->actual_type = QF16;
}

void qfe_qf32_div_qf32qf32(qf_t a, qf_t b, qf_t* out) {
    out->ref_df = a.ref_df/b.ref_df;
    out->ref_sf = a.ref_sf/b.ref_sf;
    out->ref_qf32 = qfes_qf32_div_qf32qf32(a.ref_qf32, b.ref_qf32);
    out->ref_qf16 = qfes_qf16_div_qf16qf16(a.ref_qf16, b.ref_qf16);

    out->actual_qf32 = qfes_qf32_div_qf32qf32(a.actual_qf32, b.actual_qf32);
    out->actual_type = QF32;
}

void qfe_qf16_inv_qf16(qf_t a, qf_t* out) {
    out->ref_df = 1.0/a.ref_df;
    out->ref_sf = 1.0/a.ref_sf;
    out->ref_qf32 = qfes_qf32_inv_qf32(a.ref_qf32);
    out->ref_qf16 = qfes_qf16_inv_qf16(a.ref_qf16);

    out->actual_qf16 = qfes_qf16_inv_qf16(a.actual_qf16);
    out->actual_type = QF16;
}

void qfe_qf32_inv_qf32(qf_t a, qf_t* out) {
    out->ref_df = 1.0/a.ref_df;
    out->ref_sf = 1.0/a.ref_sf;
    out->ref_qf32 = qfes_qf32_inv_qf32(a.ref_qf32);
    out->ref_qf16 = qfes_qf16_inv_qf16(a.ref_qf16);

    out->actual_qf32 = qfes_qf32_inv_qf32(a.actual_qf32);
    out->actual_type = QF32;
}


/* Helper functions for displaying output */

void qfe_printf(qf_t x) {
    float ref_qf32=qfes_sf_equals_qf32(x.ref_qf32);
    float ref_qf16=qfes_sf_equals_qf16(x.ref_qf16);

    float actual_value;
    uint32_t actual_representation;
    switch(x.actual_type) {
        case SF:
            actual_value=x.actual_sf;
            actual_representation = *(uint32_t *)&x.actual_sf;
            break;
        case HF:
            actual_value=qfes_sf_equals_hf(x.actual_hf);
            actual_representation = 0xffff&(*(uint32_t *)&x.actual_hf);
            break;
        case QF32:
            actual_value=qfes_sf_equals_qf32(x.actual_qf32);
            actual_representation = *(uint32_t *)&x.actual_qf32;
            break;
        case QF16:
            actual_value=qfes_sf_equals_qf16(x.actual_qf16);
            actual_representation = 0xffff&(*(uint32_t *)&x.actual_qf16);
            break;
    }
    float actual_error=actual_value-x.ref_df;

    printf("REFS -- sf:(%f,0x%x) err=%.1e | qf32:(%f,0x%x) err=%.1e | qf16:(%f,0x%x) err=%.1e || ACTUAL -- %s:(%f,0x%x) err=%.1e\n",
        x.ref_sf,*(uint32_t *)&x.ref_sf,x.ref_sf-x.ref_df,
        ref_qf32,*(uint32_t *)&x.ref_qf32,ref_qf32-x.ref_df,
        ref_qf16,*(uint16_t *)&x.ref_qf16,ref_qf16-x.ref_df,
        x.actual_type==SF ? "SF" : (x.actual_type==HF ? "HF" : (x.actual_type==QF32 ? "QF32" : "QF16") ),
        actual_value, actual_representation, actual_error
        );
}

void qfe_strcat(char* string, qf_t x) {
    char regString[128];

    float actual_value;
    uint32_t actual_representation;
    switch(x.actual_type) {
        case SF:
            actual_value=x.actual_sf;
            actual_representation = *(uint32_t *)&x.actual_sf;
            break;
        case HF:
            actual_value=qfes_sf_equals_hf(x.actual_hf);
            actual_representation = 0xffff&(*(uint32_t *)&x.actual_hf);
            break;
        case QF32:
            actual_value=qfes_sf_equals_qf32(x.actual_qf32);
            actual_representation = *(uint32_t *)&x.actual_qf32;
            break;
        case QF16:
            actual_value=qfes_sf_equals_qf16(x.actual_qf16);
            actual_representation = 0xffff&(*(uint32_t *)&x.actual_qf16);
            break;
    }

    sprintf(regString,"%s:(%f,0x%x)",
        x.actual_type==SF ? "SF" : (x.actual_type==HF ? "HF" : (x.actual_type==QF32 ? "QF32" : "QF16") ),
        actual_value, actual_representation);
    strcat(string,regString);
}


////////////////////////////////////////////////////////////////////////////
// Helper Functions


void* allocate_aligned_memory(int size, int alignment)
{
    void *ptr;
    #ifdef _MSC_VER
        ptr = _aligned_malloc(size, alignment);
    #else
        posix_memalign(&ptr, alignment, size);
    #endif

    return ptr;

}

void free_aligned_memory(void *ptr)
{
    if(ptr!=NULL)
    {
        #ifdef _MSC_VER
            _aligned_free(ptr);
        #else
            free(ptr);
        #endif
    }

    return;
}

qf16_t qfes_qf16_generic_function(fptr_hf qhmath_func_ptr, qf16_t a, qf16_t b, int num_args)
{

    int retVal;
    _ALIGNED_ATTRIBUTE_(128) _Float16 arr_a_hf[64];
    _ALIGNED_ATTRIBUTE_(128) _Float16 arr_b_hf[64];
    _ALIGNED_ATTRIBUTE_(128) _Float16 arr_out_hf[64];

    arr_a_hf[0] = qfes_hf_equals_qf16(a);
    arr_b_hf[0] = qfes_hf_equals_qf16(b);
    arr_out_hf[0] = 0;

    switch(num_args)
    {
        case 1:
            retVal = (*qhmath_func_ptr)(arr_a_hf, arr_out_hf, 64);
            break;

        case 2:
            retVal = (*qhmath_func_ptr)(arr_a_hf, arr_b_hf, arr_out_hf, 64);
    }

    if(retVal)
    {
        printf("ERROR: qhmath_hvx function returned 0x%x\n", retVal);
        return -1;
    }

    return qfes_qf16_equals_hf(arr_out_hf[0]);

}

qf32_t qfes_qf32_generic_function(fptr_sf qhmath_func_ptr, qf32_t a, qf32_t b, int num_args)
{

    int retVal;
    _ALIGNED_ATTRIBUTE_(128) float arr_a_sf[32];
    _ALIGNED_ATTRIBUTE_(128) float arr_b_sf[32];
    _ALIGNED_ATTRIBUTE_(128) float arr_out_sf[32];

    arr_a_sf[0] = qfes_sf_equals_qf32(a);
    arr_b_sf[0] = qfes_sf_equals_qf32(b);
    arr_out_sf[0] = 0;

    switch(num_args)
    {
        case 1:
            retVal = (*qhmath_func_ptr)(arr_a_sf, arr_out_sf, 32);
            break;

        case 2:
            retVal = (*qhmath_func_ptr)(arr_a_sf, arr_b_sf, arr_out_sf, 32);
    }

    if(retVal)
    {
        printf("ERROR: qhmath_hvx function returned 0x%x\n", retVal);
        return -1;
    }

    return qfes_qf32_equals_sf(arr_out_sf[0]);

}

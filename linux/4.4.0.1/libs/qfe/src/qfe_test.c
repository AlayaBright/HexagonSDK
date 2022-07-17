/**=============================================================================

@file
   qfe.c

Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/


#include <stdio.h>
#include <hexagon_protos.h>
#include <string.h>
#include "qfe.h"

void qf_single_type() {
    qf32_t a_qf32, b_qf32, out_qf32;

    a_qf32 = qfes_qf32_equals_sf(3.3);
    b_qf32 = qfes_qf32_equals_sf(1.1);
    out_qf32 = qfes_qf32_add_qf32qf32(a_qf32,b_qf32);

    printf("Output variable status after q32 add\n");
    qfes_printf_qf32(out_qf32);
    char myString[256];
    strcpy(myString,"One more time: ");
    qfes_strcat_qf32(myString,out_qf32);
    printf("%s.\n",myString);
}

void qf_generic_type() {
    qf_t a_qf, b_qf, out_qf;

    printf("TEST 1\n");

    qfe_init_sf(3.3,&a_qf);
    qfe_init_sf(1.1,&b_qf);

    printf("\nInput variable status after initialization to float\n");
    qfe_printf(a_qf);
    qfe_printf(b_qf);

    qfe_qf32_equals_sf(a_qf,&a_qf);
    qfe_qf32_equals_sf(b_qf,&b_qf);

    printf("\nInput variable status after conversion to qf32\n");
    qfe_printf(a_qf);
    qfe_printf(b_qf);

    qfe_qf32_add_qf32qf32(a_qf,b_qf,&out_qf);

    printf("\nOutput variable status after q32 add\n");
    qfe_printf(out_qf);

    printf("\nTEST 2\n");
    qfe_init_sf(0,&a_qf);
    qfe_init_sf(1.1,&b_qf);
    qfe_qf32_equals_sf(a_qf,&a_qf);
    qfe_qf32_equals_sf(b_qf,&b_qf);
    for (int i=0;i<100;i++) {
        qfe_qf32_add_qf32qf32(a_qf,b_qf,&a_qf);
    }
    qfe_printf(a_qf);

    char myString[512];
    strcpy(myString,"Or if you only care about appending the value to an existing string: ");
    qfe_strcat(myString,a_qf);
    printf("%s.\n",myString);

}


void qf_single_type_demo_all() {

    qf16_t a_qf16, b_qf16, out_qf16;
    qf32_t a_qf32, b_qf32, out_qf32;
    _Float16 a_hf, b_hf;
    float a_sf = 6.0, b_sf = 3.0;


    a_qf16 = qfes_qf16_equals_sf(a_sf);
    b_qf16 = qfes_qf16_equals_sf(b_sf);

    a_qf32 = qfes_qf32_equals_sf(a_sf);
    b_qf32 = qfes_qf32_equals_sf(b_sf);

    a_hf = qfes_hf_equals_sf(a_sf);
    b_hf = qfes_hf_equals_sf(b_sf);


    out_qf16 = qfes_qf16_add_qf16qf16(a_qf16, b_qf16);
    printf("qfes_qf16_add_qf16qf16 : %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);

    out_qf16 = qfes_qf16_add_qf16hf(a_qf16, b_hf);
    printf("qfes_qf16_add_qf16hf : %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);

    out_qf16 = qfes_qf16_add_hfhf(a_hf, b_hf);
    printf("qfes_qf16_add_hfhf : %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);


    out_qf32 = qfes_qf32_add_qf32qf32(a_qf32,b_qf32);
    printf("qfes_qf32_add_qf32qf32: %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);

    out_qf32 = qfes_qf32_add_qf32sf(a_qf32,b_sf);
    printf("qfes_qf32_add_qf32sf: %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);

    out_qf32 = qfes_qf32_add_sfsf(a_sf,b_sf);
    printf("qfes_qf32_add_sfsf: %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);


    out_qf16 = qfes_qf16_sub_qf16qf16(a_qf16, b_qf16);
    printf("qfes_qf16_sub_qf16qf16 : %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);

    out_qf16 = qfes_qf16_sub_qf16hf(a_qf16, b_hf);
    printf("qfes_qf16_sub_qf16hf : %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);

    out_qf16 = qfes_qf16_sub_hfhf(a_hf, b_hf);
    printf("qfes_qf16_sub_hfhf : %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);


    out_qf32 = qfes_qf32_sub_qf32qf32(a_qf32,b_qf32);
    printf("qfes_qf32_sub_qf32qf32: %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);

    out_qf32 = qfes_qf32_sub_qf32sf(a_qf32,b_sf);
    printf("qfes_qf32_sub_qf32sf: %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);

    out_qf32 = qfes_qf32_sub_sfsf(a_sf,b_sf);
    printf("qfes_qf32_sub_sfsf: %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);


    out_qf16 = qfes_qf16_mpy_qf16qf16(a_qf16, b_qf16);
    printf("qfes_qf16_mpy_qf16qf16 : %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);

    out_qf16 = qfes_qf16_mpy_qf16hf(a_qf16, b_hf);
    printf("qfes_qf16_mpy_qf16hf : %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);

    out_qf16 = qfes_qf16_mpy_hfhf(a_hf, b_hf);
    printf("qfes_qf16_mpy_hfhf : %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);


    out_qf32 = qfes_qf32_mpy_qf16qf16(a_qf16, b_qf16);
    printf("qfes_qf32_mpy_qf16qf16 : %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);

    out_qf32 = qfes_qf32_mpy_qf16hf(a_qf16, b_hf);
    printf("qfes_qf32_mpy_qf16hf : %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);

    out_qf32 = qfes_qf32_mpy_hfhf(a_hf, b_hf);
    printf("qfes_qf32_mpy_hfhf : %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);


    out_qf32 = qfes_qf32_mpy_qf32qf32(a_qf32,b_qf32);
    printf("qfes_qf32_mpy_qf32qf32: %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);

    out_qf32 = qfes_qf32_mpy_sfsf(a_sf,b_sf);
    printf("qfes_qf32_mpy_sfsf: %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);

    out_qf16 = qfes_qf16_div_qf16qf16(a_qf16, b_qf16);
    printf("qfes_qf16_div_qf16qf16: %f %f\n", a_sf, b_sf);
    qfes_printf_qf16(out_qf16);

    out_qf32 = qfes_qf32_div_qf32qf32(a_qf32, b_qf32);
    printf("qfes_qf32_div_qf32qf32: %f %f\n", a_sf, b_sf);
    qfes_printf_qf32(out_qf32);

    out_qf16 = qfes_qf16_inv_qf16(a_qf16);
    printf("qfes_qf16_inv_qf16: %f\n", a_sf);
    qfes_printf_qf16(out_qf16);

    out_qf32 = qfes_qf32_inv_qf32(a_qf32);
    printf("qfes_qf32_inv_qf32: %f\n", a_sf);
    qfes_printf_qf32(out_qf32);

    return;
}



void qf_generic_type_demo_all() {

    qf_t a_qf, b_qf, out_qf;

    qfe_init_sf(9.0,&a_qf);
    qfe_init_sf(3.0,&b_qf);
    printf("\nInput variable status after initialization to float\n");
    qfe_printf(a_qf);
    qfe_printf(b_qf);

    qfe_hf_equals_sf(a_qf,&a_qf);
    qfe_hf_equals_sf(b_qf,&b_qf);
    printf("\nInput variable status after conversion to hf\n");
    qfe_printf(a_qf);
    qfe_printf(b_qf);

    qfe_qf16_equals_sf(a_qf,&a_qf);
    qfe_qf16_equals_sf(b_qf,&b_qf);
    printf("\nInput variable status after conversion to qf16\n");
    qfe_printf(a_qf);
    qfe_printf(b_qf);

    qfe_qf32_equals_sf(a_qf,&a_qf);
    qfe_qf32_equals_sf(b_qf,&b_qf);
    printf("\nInput variable status after conversion to qf32\n");
    qfe_printf(a_qf);
    qfe_printf(b_qf);


    qfe_qf16_add_qf16qf16(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_add_qf16qf16 : \n");
    qfe_printf(out_qf);

    qfe_qf16_add_qf16hf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_add_qf16hf : \n");
    qfe_printf(out_qf);

    qfe_qf16_add_hfhf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_add_hfhf : \n");
    qfe_printf(out_qf);


    qfe_qf32_add_qf32qf32(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_add_qf32qf32: \n");
    qfe_printf(out_qf);

    qfe_qf32_add_qf32sf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_add_qf32sf: \n");
    qfe_printf(out_qf);

    qfe_qf32_add_sfsf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_add_sfsf: \n");
    qfe_printf(out_qf);


    qfe_qf16_sub_qf16qf16(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_sub_qfqf16 : \n");
    qfe_printf(out_qf);

    qfe_qf16_sub_qf16hf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_sub_qfhf : \n");
    qfe_printf(out_qf);

    qfe_qf16_sub_hfhf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_sub_qfhf : \n");
    qfe_printf(out_qf);


    qfe_qf32_sub_qf32qf32(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_sub_qfqf32: \n");
    qfe_printf(out_qf);

    qfe_qf32_sub_qf32sf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_sub_qfsf: \n");
    qfe_printf(out_qf);

    qfe_qf32_sub_sfsf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_sub_qfsf: \n");
    qfe_printf(out_qf);


    qfe_qf16_mpy_qf16qf16(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_mpy_qf16qf16 : \n");
    qfe_printf(out_qf);

    qfe_qf16_mpy_qf16hf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_mpy_qf16hf : \n");
    qfe_printf(out_qf);

    qfe_qf16_mpy_hfhf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_mpy_hfhf : \n");
    qfe_printf(out_qf);


    qfe_qf32_mpy_qf16qf16(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_mpy_qf16qf16 : \n");
    qfe_printf(out_qf);

    qfe_qf32_mpy_qf16hf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_mpy_qf16hf : \n");
    qfe_printf(out_qf);

    qfe_qf32_mpy_hfhf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_mpy_hfhf : \n");
    qfe_printf(out_qf);


    qfe_qf32_mpy_qf32qf32(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_mpy_qf32qf32: \n");
    qfe_printf(out_qf);

    qfe_qf32_mpy_sfsf(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_mpy_sfsf: \n");
    qfe_printf(out_qf);

    qfe_qf16_div_qf16qf16(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf16_div_qf16qf16 : \n");
    qfe_printf(out_qf);

    qfe_qf32_div_qf32qf32(a_qf,b_qf,&out_qf);
    printf("\nqfe_qf32_div_qf32qf32 : \n");
    qfe_printf(out_qf);

    qfe_qf16_inv_qf16(a_qf,&out_qf);
    printf("\nqfe_qf16_inv_qf16 : \n");
    qfe_printf(out_qf);

    qfe_qf32_inv_qf32(a_qf,&out_qf);
    printf("\nqfe_qf32_inv_qf32 : \n");
    qfe_printf(out_qf);


    return;
}


int main(int argc, char **argv)
{

    printf("---------------------------------------------------------------\n");
    printf("SCALAR TEST\n");
    qf_single_type();
    printf("---------------------------------------------------------------\n");
    printf("GENERIC TEST\n");
    qf_generic_type();

    printf("\n---------------------------------------------------------------\n");
    printf("RUN ALL SINGLE TYPE FUNCTIONS\n\n");
    qf_single_type_demo_all();

    printf("\n---------------------------------------------------------------\n");
    printf("RUN ALL GENERIC TYPE FUNCTIONS\n\n");
    qf_generic_type_demo_all();


    return 0;
}

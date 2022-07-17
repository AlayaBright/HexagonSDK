/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#include "HAP_farf.h"
#undef FARF_LOW
#define FARF_LOW 1
#undef FARF_HIGH
#define FARF_HIGH 1

extern "C" {

void halide_print(void *user_context, const char *msg) {
    FARF(HIGH, "halide_print %s\n", msg);
}

void halide_error(void *user_context, const char *msg) {
    FARF(LOW, "In halide_error\n");
    halide_print(user_context, msg);
}
}

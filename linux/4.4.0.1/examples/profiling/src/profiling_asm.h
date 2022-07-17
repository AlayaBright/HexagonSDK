/*==============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#ifndef PERFTESTS_ASM_H
#define PERFTESTS_ASM_H
#include "hexagon_types.h"
#include "hexagon_protos.h"
#ifdef __cplusplus
extern "C"
{
#endif

int profiling_loop_time(int num);
int profiling_loop_pcycles(int num);
HVX_Vector profiling_hvx_function(int num);


#ifdef __cplusplus
}
#endif

#endif    // PERFTESTS_ASM_H

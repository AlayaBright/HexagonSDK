/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

    .text
    .p2align 2
    .p2align 4,,15
    .globl profiling_loop_time
    .type   profiling_loop_time, @function
profiling_loop_time:
    {
        R1 = #0
    }
    {
        LOOP0(iter, R0)
    }
    {
        R0 = R1
        JUMPR R31
    }

    .falign
iter:
    {
        R1 = ADD(R1, #1)
    }:endloop0
    .size    profiling_loop_time, .-profiling_loop_time

    .text
    .p2align 2
    .globl profiling_loop_pcycles
    .type   profiling_loop_pcycles, @function
profiling_loop_pcycles:
    {
        R1 = #0
    }
    {
        LOOP0(iterate, R0)
    }

    {
        R0 = R1
        JUMPR R31
    }

    .falign
iterate:
    {
        R1 = ADD(R1, #1)
    }:endloop0
    .size    profiling_loop_pcycles, .-profiling_loop_pcycles


    .text
    .p2align 2
    .globl profiling_hvx_function
    .type   profiling_hvx_function, @function
profiling_hvx_function:
    {
        V0 = VSPLAT(R0)
        JUMPR R31
    }
    .size    profiling_hvx_function, .-profiling_hvx_function

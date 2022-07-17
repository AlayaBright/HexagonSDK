/**=============================================================================
@file
    getopt_custom.h

@brief
    Emulate getopt funtion using getopt_long_custom for non-Linux environment.

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _GETOPT_CUSTOM_H
#define _GETOPT_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "getopt_long_custom.h"

/* Store zero to suppress the error message for unrecognized options. */
extern int opterr;

/* Corresponds to an unrecognized option passed to getopt. */
extern int optopt;

/**
 * Custom version of C library function getopt() for non-Linux Environments (e.g. Hexagon and Windows).
 *
 * @param[in]   argc Argument count
 * @param[in]   argv Argument array
 * @param[in]   options_string  Legitimate option characters
 * @return      0   if long option
 *              1   if duplicate option with different parameter or
 *                     option that is supposed to carry a parameter but doesn't
 *              2   if duplicate option with same parameter as before
 *             '?'  cast to int of char output if invalid option
 *             'c'  cast to int of char if short option
 *             -1   end of input option
 */
int getopt(int argc, char * const argv[], const char *options_string);

#ifdef __cplusplus
}
#endif

#endif /* _GETOPT_CUSTOM_H */
/**=============================================================================
@file
    GETOPT_LONG_CUSTOM.h

@brief
    Emulate Linux getopt_long funtion for non-Linux environment.

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _GETOPT_LONG_CUSTOM_H
#define _GETOPT_LONG_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef optarg
/* Contains the argument value for the recognized options.
   Re-defining it for custom implementation. */
#define optarg optarg_custom
#endif

/* Contains the argument value for the recognized options. */
extern char* optarg_custom;

/* Structure containing the information for long options. */ 
struct long_options_string
{
    const char *name;
    int         has_arg;
    int        *flag;
    int         val;
};

/**
 * Custom version of C library function getopt_long() for non-Linux Environments (e.g. Hexagon and Windows).
 *
 * @param[in]   argc Argument count
 * @param[in]   argv Argument array
 * @param[in]   short_options_string Legitimate option characters
 * @param[in]   long_options_string Array of long options.
 * @param[out]  longindex Array option index.
 * @return      0   if long option
 *              1   if duplicate option with different parameter or
 *                     option that is supposed to carry a parameter but doesn't
 *              2   if duplicate option with same parameter as before
 *             '?'  cast to int of char output if invalid option
 *             'c'  cast to int of char if short option
 *             -1   end of input option
 */
int getopt_long_custom(int argc, char *const argv[], const char *short_options_string,
                        const struct long_options_string *options, int *longindex);


#ifdef __cplusplus
}
#endif

#endif /* _GETOPT_LONG_CUSTOM_H */

/**=============================================================================
Copyright (c) 2021 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary

This file contains custom getopt_long funtion for non-Linux environment.
=============================================================================**/


//==============================================================================
// Include Files
//==============================================================================

#include <stdio.h>
#include <string.h>
#include "getopt_long_custom.h"

char *optarg_custom;

/* Contains the list of long options, dummy for getopt_custom. */
static struct long_options_string long_options[] = {{0}};

/*==============================================================================
    LOCAL FUNCTION
==============================================================================*/
/**
 * Check if an option has already been set before.
 *
 * @param[in]   optionInput  Cast to int of char input.
 * @return      1 if same option with different argument,
 *              2 if duplicate option,
 *              optionInput otherwise.
 */
static int register_option(int optionInput)
{
    static int optionList[100];
    static char *paraList[100];
    static int optionIndex = 0;

    if (optionInput != 0)
    {
        if (optionIndex == 0)
        {
            optionList[0] = optionInput;
            paraList[0] = optarg_custom;
            optionIndex++;
            return optionInput;
        }
        for (int i = 0; i < optionIndex; i++)
        {
            if (optionList[i] == optionInput && paraList[i] != NULL)
            {
                if (strcmp(optarg_custom, paraList[i]) != 0)
                {
                    return 1;
                }
                else
                {
                    printf("Duplicate input -%c!\n", optionInput);
                    return 2;
                }
            }
            if (optionList[i] == optionInput && paraList[i] == NULL)
            {
                printf("Duplicate input -%c!\n", optionInput);
                return 2;
            }
        }
        optionList[optionIndex] = optionInput;
        paraList[optionIndex] = optarg_custom;
        optionIndex++;
        return optionInput;
    }
    else
    {
        return optionInput;
    }
}

/*==============================================================================
    GLOBAL FUNCTION
==============================================================================*/
int getopt_long_custom(int argc, char *const argv[], const char *short_options_string,
                        const struct long_options_string *options, int *longindex)
{
    static int index = 1;
    if (argc < 2 || argc > 99)
    {
        return 1;
    }
    if (index > argc - 1)
    {
        return -1; // end condition
    }
    size_t len = strlen(argv[index]);
    // FORMAT: -x & -x arg
    if (len == 2 && argv[index][0] == '-' && argv[index][1] != '-')
    {
        // check whether in short_options_string and arg
        char *shortOptAddr = strstr(short_options_string, &argv[index][1]);
        if (shortOptAddr == NULL)
        {
            printf("Invalid option: %s\n", argv[index]);
            index += 1;
            return '?'; // invalid option, stop and parse next option
        }
        if (shortOptAddr[1] == ':')
        {
            if (argv[index + 1] == NULL || argv[index + 1][0] == '-')
            {
                index += 1;
                return 1; // error code 1
            }
            optarg_custom = argv[index + 1];
            index += 2;
            return register_option(shortOptAddr[0]);
        }
        else
        {
            index++;
            optarg_custom = NULL;
            return register_option(argv[index - 1][1]);
        }
    }
    // FORMAT: --xyz & --xyz mm & --xyz=mm
    const struct long_options_string *p;
    int tmpIndex;
    char *tmpAddr;

    // get option with '='
    if (len > 3 && argv[index][0] == '-' && argv[index][1] == '-' && (tmpAddr = strchr(argv[index], '=')) != NULL)
    {
        if (strlen(tmpAddr) > 1)
        {
            for (p = options, tmpIndex = 0; p->name; p++, tmpIndex++)
            {
                if (!strncmp(p->name, argv[index] + 2, strlen(argv[index]) - strlen(tmpAddr) - 2) && (strlen(argv[index]) - strlen(tmpAddr) - 2) == strlen(p->name))
                {
                    *longindex = tmpIndex;
                    if (p->has_arg == 1)
                    {
                        optarg_custom = tmpAddr + 1;
                        index += 1;
                        return register_option(p->val);
                    }
                }
            }
        }
        else
            return 1;
    }
    // get option without '='
    else if (len > 3 && argv[index][0] == '-' && argv[index][1] == '-')
    {
        for (p = options, tmpIndex = 0; p->name; p++, tmpIndex++)
        {
            if (!strncmp(p->name, argv[index] + 2, len - 2) && (len - 2) == strlen(p->name))
            {
                *longindex = tmpIndex;
                if (p->has_arg == 1)
                {
                    if (argv[index + 1] == NULL || argv[index + 1][0] == '-')
                    {
                        index += 1;
                        return 1; // error code 1
                    }
                    optarg_custom = argv[index + 1];
                    index += 2;
                    return register_option(p->val);
                }
                else
                {
                    index++;
                    optarg_custom = NULL;
                    return register_option(p->val);
                }
            }
        }
        printf("Invalid option: %s\n", argv[index]);
        index++;
        return '?';
    }
    return 1;
}

int getopt(int argc, char * const argv[], const char *options_string) {
    int optind = 0;
    return getopt_long_custom(argc, argv, options_string, long_options, &optind);
}

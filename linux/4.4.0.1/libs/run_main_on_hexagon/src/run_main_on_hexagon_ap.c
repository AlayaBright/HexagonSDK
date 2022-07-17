/*==============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#define FARF_HIGH 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __LA_FLAG
#include <android/log.h>
#endif
#include "remote.h"
#include "run_main_on_hexagon.h"
#include "HAP_farf.h"

#define MAX_ARGS_LEN 2048
#define DEFAULT_STACK_SIZE 1024*256

char args_buf[MAX_ARGS_LEN];

static void print_usage(char* program)
{
    printf("Usage:\n\n"
    "  %s domain path <stack_size=> <args>\n\n"
    "  domain      : which Hexagon DSP to use, expressed as numeric domain id \n"
    "                supported domains: 0 (ADSP), 1 (MDSP), 2 (SDSP), 3 (CDSP)\n\n"
    "  path        : file path to .so that exports symbol that adheres to: \"int main(int argc, char *argv[])\"\n\n"
    "  stack_size= : optional argument to configure the stack size of run_main_on_hexagon thread that runs main()\n"
    "                default stack size is 256kb and stack size less than 256kb is not allowed \n\n"
    "  args        : optional string arguments to pass to main()\n\n"
    "  e.g. %s 0 test_main.so stack_size=0x400000 1 foo2 2 bar\n\n"
    "     above command runs main() of test_main.so in a thread of stack size 0x400000 with arguments \"1 foo2 2 bar\" on ADSP\n" , program, program);
}

int main(int argc, char* argv[])
{
    int nErr = 0;
    char* stack_arg;
    unsigned int stack_size = 0;
    int result = 0;
    int domain = -1;
    remote_handle64 handle64 = -1;
    char* uri;

    if (argc < 3) {
        print_usage(argv[0]);
        return -1;
    }

    // concat args into string to send over the wire
    {
        int i = 0;
        int len = 0;
        char* ptr;
        size_t arg_size;

        args_buf[0] = '\0';
        for (i = 2 /* skip my name and domain */, ptr = args_buf; i < argc; i++) {
            arg_size = strlen(argv[i]) + 1;
            len += (arg_size);
            if (len >= MAX_ARGS_LEN) {
                print_usage(argv[0]);
                return -1;
            }
            snprintf(ptr, arg_size, "%s", argv[i]);

            ptr += (arg_size - 1);
            *ptr++ = ' ';
        }
        *ptr = '\0';
    }

    // determine domain
    domain = atoi(argv[1]);
    if (domain == ADSP_DOMAIN_ID) {
        uri = run_main_on_hexagon_URI ADSP_DOMAIN;
    } else if (domain == CDSP_DOMAIN_ID) {
        uri = run_main_on_hexagon_URI CDSP_DOMAIN;
    } else if (domain == MDSP_DOMAIN_ID) {
        uri = run_main_on_hexagon_URI MDSP_DOMAIN;
    } else if (domain == SDSP_DOMAIN_ID) {
        uri = run_main_on_hexagon_URI SDSP_DOMAIN;
    } else {
        FARF(ERROR, "Unsupported domain %d", domain);
        return -1;
    }

    nErr = run_main_on_hexagon_open(uri, &handle64);
    if (nErr) {
        FARF(ERROR, "Domain %d failed to open (0x%X), uri: %s", domain, nErr, uri);
        return -1;
    }

    // read optional stack size from user
    stack_arg = strstr(args_buf, "stack_size=");
    if (stack_arg) {
        stack_arg += strlen("stack_size=");
        stack_size = (unsigned int)strtol(stack_arg, NULL, 0);
    } else {
        stack_size = DEFAULT_STACK_SIZE;
    }
    if (stack_size < DEFAULT_STACK_SIZE){
        FARF(HIGH, "Stack size less than 0x%X is not allowed",DEFAULT_STACK_SIZE);
        nErr = -1;
        goto bail;
    }

    FARF(HIGH, "RPC to Hexagon DSP with args: \"%s\"", args_buf);
    nErr = run_main_on_hexagon_doit(handle64, stack_size, args_buf, &result);

bail:
    if (nErr) {
        FARF(ERROR, "Error %d: Failed to call main() on DSP", nErr);
    } else if (result) {
        FARF(HIGH, "Error: Main on Hexagon DSP returned %d", result);
    } else {
        FARF(HIGH, "Successfully called main() on Hexagon DSP and received return value of 0.");
    }

    if (handle64) {
        if (run_main_on_hexagon_close(handle64)) {
            FARF(ERROR, "Failed to close handle");
        }
    }
    return nErr;
}

void HAP_debug(const char* msg, int level, const char* filename, int line)
{
#ifdef __LA_FLAG
    __android_log_print(level, "adsprpc", "%s:%d: %s", filename, line, msg);
#endif
    printf("%s :%d:%s\n", msg, line, filename);
}

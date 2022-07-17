/*==============================================================================
  Copyright (c) 2019, 2021 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "dlfcn.h"
#include "AEEStdErr.h"
#include "qurt.h"

#define FARF_ERROR 1
#define FARF_HIGH 1
#define FARF_MEDIUM 1
#include "HAP_farf.h"
#include "run_main_on_hexagon.h"

extern void qurt_process_cmdline_get(char *buf, unsigned buf_siz);

#define MAX_BUF_SIZE 2048
#define DEFAULT_STACK_SIZE 1024*256

const char* DELIM = "--";
char args_buf[MAX_BUF_SIZE];
unsigned int stack_size = DEFAULT_STACK_SIZE;

struct thread_context {
   char* args;
   int result;
};

// Find the string before "delim" from "str" string
char* find_string(char* str, const char* delim)
{
   char *p = NULL;
   p = strstr(str, delim);
   if (p != NULL) {
      *p = '\0';
   }
   return str;
}

// Find arguments to run_main executable from "str" string
int find_args(char* str)
{
   int nErr = AEE_SUCCESS;
   char* stack_arg = NULL;

   // Read optional stack size argument from user
   stack_arg = strstr(str, "stack_size=");
   if (stack_arg != NULL) {
      stack_arg += strlen("stack_size=");
      stack_size = (int)strtol(stack_arg, NULL, 0);
   }

   if (stack_size < DEFAULT_STACK_SIZE) {
      FARF(HIGH, "Stack size less than 0x%x is not allowed", DEFAULT_STACK_SIZE);
      nErr = AEE_EBADPARM;
      goto bail;
   }

bail:
   return nErr;
}

int main(void)
{
   int nErr = AEE_SUCCESS;
   struct thread_context context;
   char* args_to_doit = NULL, *args_to_sim = NULL;
   int status = -1; /* Return value from doit function running on DSP. */
   size_t len = 0;

   // Initialize the loader
   DL_vtbl vtbl = { sizeof(DL_vtbl), HAP_debug_v2 };
   char *builtin[] = { (char *)"libc.so", (char *)"libgcc.so" };
   if (0 == dlinitex(2, builtin, &vtbl)) {
      FARF(ERROR, "Failed to init loader");
      nErr = AEE_EFAILED;
      goto bail;
   }

   // Get the arguments
   args_buf[0] = '\0';
   qurt_process_cmdline_get(args_buf, MAX_BUF_SIZE);
   len = strlen(args_buf);
   if (len == 0) {
      FARF(ERROR, "Failed to get args from QuRT");
      nErr = AEE_EFAILED;
      goto bail;
   }
   FARF(HIGH, "Args from QuRT: %s", args_buf);

   // Save a copy of "Args from QuRT"
   args_to_doit = (char*)malloc(len+1);
   if (args_to_doit == NULL) {
      FARF(ERROR, "Malloc failed for user arguments");
      nErr = AEE_ENORPCMEMORY;
      goto bail;
   }

   if (strlcpy(args_to_doit, args_buf, len+1) > len) {
      FARF(ERROR, "Buffer is truncated while copying args_buf: %s", args_buf);
      nErr = AEE_EBUFFERTOOSMALL;
      goto bail;
   }

   // Find the string before "--" to find args to simulator executable
   args_to_sim = find_string(args_to_doit, DELIM);
   if (args_to_sim != NULL)
   {
      // Find the args to simulator executable
      nErr = find_args(args_to_sim);
      if (nErr != AEE_SUCCESS)
         goto bail;
   }

   // Find string after "--" to get the module and its arguments
   context.args = strstr(args_buf, DELIM);
   if (context.args != NULL) {
      context.args += strlen(DELIM);
   } else {
      context.args = "";
   }

   // Call the doit function on the DSP
   status = run_main_on_hexagon_doit(0, stack_size, context.args, &context.result);
   if ((int)status != AEE_SUCCESS) {
      FARF(ERROR, "Error 0x%x while attempting to call main() on dsp", status);
      nErr = status;
      goto bail;
   }
   FARF(HIGH, "Main() returned %d", context.result);

bail:
   if (args_to_doit)
      free(args_to_doit);
   if (nErr != AEE_SUCCESS) {
      FARF(ERROR, "run_main_on_hexagon returned 0x%x", nErr);
      return nErr;
   }
   return context.result;
}


/*==============================================================================
  Copyright (c) 2012-2013, 2020 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "verify.h"
#include "HAP_farf.h"

#include "calculator_plus.h"
#include "calculator_test.h"

#ifndef CALC_EXPORT
#define CALC_EXPORT
#endif /*CALC_EXPORT*/

#if defined(_WIN32) && !defined(_ARM_)
#include "ptl_remote_invoke.h"
CALC_EXPORT int init(pfn_getSymbol GetSymbol)
{
   return remote_invoke_stub_init(GetSymbol);
}
#endif

CALC_EXPORT int main(void)
{
   int nErr  =  0;
   int nPass =  0;

   VERIFY(0 == (nErr = calculator_test(ADSP_DOMAIN_ID)));
   nPass++;
   VERIFY(0 == (nErr = calculator_test(CDSP_DOMAIN_ID)));
   nPass++;

bail:
   printf("############################################################\n");
   printf("Summary Report \n");
   printf("############################################################\n");
   printf("Pass: %d\n", nPass);
   printf("Fail: %d\n", 2 - nPass);

   return nErr;
}

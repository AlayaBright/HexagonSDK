/*==============================================================================
  Copyright (c) 2021-22, Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "HAP_farf.h"
#include <math.h>
#include "gtest.h"
#include "gtest/gtest.h"

int gtest_open(const char*uri, remote_handle64* handle) {
   void *tptr = NULL;
  /* can be any value or ignored, rpc layer doesn't care
   * also ok
   * *handle = 0;
   * *handle = 0xdeadc0de;
   */
   tptr = (void *)malloc(1);
   *handle = (remote_handle64)tptr;
   assert(*handle);
   return 0;
}

/**
 * @param handle, the value returned by open
 * @retval, 0 for success, should always succeed
 */
int gtest_close(remote_handle64 handle) {
   if (handle)
      free((void*)handle);
   return 0;
}

// DSP-Side Square root function
double square_root_on_dsp(const double in_value) {
    return sqrt(in_value);
}

int gtest_square_root(remote_handle64 h, double input_val, double* output_val)
{
  *output_val = square_root_on_dsp(input_val);
  FARF(RUNTIME_HIGH, "===============     DSP: Square Root result %f ===============", *output_val);
  return 0;
}

// Test DSP-side Square-root function using gtest framework
TEST (square_root_test, verify_gtest_on_dsp) {
    EXPECT_EQ (18.0, square_root_on_dsp(324.0));
    EXPECT_EQ (25.0, square_root_on_dsp(625.0));
    EXPECT_EQ (50.0, square_root_on_dsp(2500.0));
    EXPECT_EQ (0.0 , square_root_on_dsp(0.0));
}

int gtest_verify_on_dsp(remote_handle64 h) {
    int retval = 0;
    int n_argc = 2;
    char *n_argv[] = {(char *) "gtest_all", (char *)"--gtest_filter=*", NULL};

    ::testing::InitGoogleTest(&n_argc, n_argv);
    retval = RUN_ALL_TESTS();

    if(retval) {
        FARF(RUNTIME_HIGH,"==== Error 0x%x: Tests failed =====\n", retval);
    }

    return retval;
}

/**================================================================================
 An example illustrating how to invoke the Qualcomm Hexagon Libraries HVX (QHL HVX)

 Copyright (c) 2021 Qualcomm Technologies Incorporated.
 All Rights Reserved. Qualcomm Proprietary and Confidential.
 ===============================================================================**/

#include "AEEStdErr.h"
#include <stdio.h>
#include <math.h>
#include "gtest/gtest.h"
#include "gtest.h"
#include "remote.h"

// Host Square Root function
double square_root(const double in_value){

    return sqrt(in_value);
}

// Test Host Square Root function using gtest framework
TEST(square_root_test, run_on_cpu) {
    EXPECT_EQ (18.0, square_root (324.0));
    EXPECT_EQ (25.0, square_root (625.0));
    EXPECT_EQ (50.0, square_root (2500.0));
    EXPECT_EQ (0.0 , square_root (0.0));
}

// Test DSP Square Root function using gtest framework
double input_value[4] = {324.0, 625.0, 2500.0, 0.0}, output_value[4] = {-1.0};
remote_handle64 handle = -1;

TEST (square_root_test, run_on_dsp) {
    ASSERT_EQ (0, gtest_square_root (handle, input_value[0], &output_value[0]));
    EXPECT_EQ (18.0, output_value[0]);
    ASSERT_EQ (0, gtest_square_root (handle, input_value[1], &output_value[1]));
    EXPECT_EQ (25.0, output_value[1]);
    ASSERT_EQ (0, gtest_square_root (handle, input_value[2], &output_value[2]));
    EXPECT_EQ (50.0, output_value[2]);
    ASSERT_EQ (0, gtest_square_root (handle, input_value[3], &output_value[3]));
    EXPECT_EQ (0.0, output_value[3]);
}

int main() {
    int ret_val = 0;
    int n_argc = 2;
    char *n_argv[] = {(char *) "gtest_all", (char *)"--gtest_filter=*", NULL};
    char *gtest_URI_Domain = NULL;
    // Open a handle on gtest RPC interface. Choose default URI for compute DSP.
    // See remote.idl for more options/information.
    gtest_URI_Domain = (char*)gtest_URI CDSP_DOMAIN; // try opening handle on CDSP.
    ret_val = gtest_open(gtest_URI_Domain, &handle);
    if (AEE_SUCCESS == ret_val) {
        printf("gtests running on HOST and DSP \n");
    } else {
        printf("Error 0x%x: unable to create fastrpc session on CDSP\n", ret_val);
        printf("gtests Running Locally \n");
        n_argv[0] = (char *)"gtest_on_host";
        n_argv[1] = (char *)"--gtest_filter=square_root_test.run_on_cpu";
    }
    ::testing::InitGoogleTest(&n_argc, n_argv);
    ret_val = RUN_ALL_TESTS();

    if(ret_val) {
        printf("ERROR 0x%x: Tests failed from gtest on CPU\n", ret_val);
    }

    ret_val = gtest_verify_on_dsp(handle);

    if(ret_val) {
        printf("ERROR 0x%x: Tests failed from gtest on DSP\n", ret_val);
    }

    /* Close the handle */
    if (handle != -1) {
        if (AEE_SUCCESS != (ret_val = gtest_close(handle))) {
            printf("ERROR 0x%x: Failed to close handle\n", ret_val);
        }
    }

  return ret_val;
}


/* ====================================================================== *
*  QUALCOMM TECHNOLOGIES, INC.                                            *
*                                                                         *
*  RunGenHost.cpp                                                         *
*                                                                         *
* ----------------------------------------------------------------------- *
*          Copyright (c) 2021 QUALCOMM TECHNOLOGIES Incorporated.         *
*                           All Rights Reserved.                          *
*                  QUALCOMM Confidential and Proprietary                  *
* ======================================================================= */
#include "rungen_interface.h"
#include <string>
#include <iostream>
#include <fstream>
#include "rpcmem.h"

#ifdef TEST_FORMAT
#include "test_report.h"
void print_test_report();
#endif

#define FASTRPC_THREAD_PARAMS (1)
#define CDSP_DOMAIN_ID    3

int main(int argc, char **argv) {

    struct remote_rpc_thread_params th;
    th.domain = CDSP_DOMAIN_ID;
    th.prio = -1;
    th.stack_size = 32*1024;
    if (&remote_session_control) {
        int set_stack_size = remote_session_control(FASTRPC_THREAD_PARAMS, (void*)&th, sizeof(th));
        if (set_stack_size != 0) {
            std::cout   << "Error: Couldn't set stack size for remote rpc session: "
                        << set_stack_size << "\n";
        }
    }

    int heapid = RPCMEM_HEAP_ID_SYSTEM;
    rpcmem_init();

    rungen_str *str_seq = (rungen_str *)rpcmem_alloc(heapid, RPCMEM_DEFAULT_FLAGS, argc * sizeof(rungen_str));
    for (int i = 0; i < argc; ++i) {
        str_seq[i].data = argv[i];
        str_seq[i].dataLen = strlen(argv[i]) + 1;
    }

    int set_perf_mode_turbo = rungen_set_hvx_perf_mode_turbo();
    if (set_perf_mode_turbo != 0) {
        std::cout   << "Error: Couldn't set perf mode to turbo: "
                    << set_perf_mode_turbo << "\n";
    }

    int power_on = rungen_power_on_hvx();
    if (power_on != 0) {
        std::cout   << "Error: Couldn't power on hvx: "
                    << power_on << "\n";
        abort();
    }

    std::cout << "Running pipeline... \n";
    rungen_main(const_cast<rungen_str *> (str_seq), argc);

    rungen_power_off_hvx();

    std::ifstream log_file(LOG_OUT_FILE);

    if (log_file.fail()) {
        std::cout   << "Cannot open rungen logfile: " << LOG_OUT_FILE
                    << "\nCheck DSP logs for failure info.\n";
    } else {
        std::cout << log_file.rdbuf();
#ifdef TEST_FORMAT
// TEST_FORMAT specified by tests in halide_test to print
// output in halide_test_tools friendly format.
        print_test_report();
#endif
    }

    rpcmem_free(str_seq);
    rpcmem_deinit();
    return 0;
}

#ifdef TEST_FORMAT
// Reads rungen output from log file and prints to console
// in test_report format required for halide_test.
// Will give a passing test report iff pipeline ran successfully
// and --parsable_output and --benchmarks=all were specified to the
// rungen executable.
void print_test_report() {
    std::ifstream log_file(LOG_OUT_FILE);
    std::string str;
    std::string::size_type n;
    double avg_time = 0;
    while(std::getline(log_file, str)) {
        n = str.find("BEST_TIME_MSEC_PER_ITER");
        if (n != std::string::npos) {
            avg_time = std::stod(str.substr(n + 25));
            break;
        }
    }
    if (avg_time != 0) {
        TestReport tr("rungen", avg_time, "milliseconds", Mode::Device_Standalone, Result::Pass);
        tr.print();
    } else {
        std::cout   << "Failed to read test run logs in test report format.\n";
        TestReport tr("rungen", avg_time, "milliseconds", Mode::Device_Standalone, Result::Fail);
        tr.print();
    }
}

#endif
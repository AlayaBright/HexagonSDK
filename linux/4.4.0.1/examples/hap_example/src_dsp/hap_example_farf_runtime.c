/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "AEEStdErr.h"

/*
 * Enable all FARF levels
 */
#define FARF_LOW    1
#define FARF_MEDIUM 1
#define FARF_HIGH   1
#define FARF_ERROR  1
#define FARF_FATAL  1


#include "HAP_farf.h"


/*
 * Illustrate the usage of FARF macro and Run-time logging options
 */


AEEResult hap_example_farf_runtime(remote_handle64 h, int runtime_logging_enable)
{

// HAP_setFARFRuntimeLoggingParams is unsupported on hexagon-sim
#if !defined(__hexagon__)

    if(runtime_logging_enable)
    {

        /*
         * Run-time logging is enabled using HAP_setFARFRuntimeLoggingParams(mask, file_array, num_files)
         * FARF logs with RUNTIME_ will appear in logcat
         */

        const char* file_name = "hap_example_farf_runtime.c";

        HAP_setFARFRuntimeLoggingParams(0x1f, &file_name, 1);

        FARF(LOW,   "Low    FARF message : Run-time logging enabled");
        FARF(MEDIUM,"Medium FARF message : Run-time logging enabled");
        FARF(HIGH,  "High   FARF message : Run-time logging enabled");
        FARF(ERROR, "Error  FARF message : Run-time logging enabled");
        FARF(FATAL, "Fatal  FARF message : Run-time logging enabled");

        FARF(RUNTIME_LOW,    "Runtime Low FARF message    - Run-time logging enabled");
        FARF(RUNTIME_MEDIUM, "Runtime Medium FARF message - Run-time logging enabled");
        FARF(RUNTIME_HIGH,   "Runtime High FARF message   - Run-time logging enabled");
        FARF(RUNTIME_ERROR,  "Runtime Error FARF message  - Run-time logging enabled");
        FARF(RUNTIME_FATAL,  "Runtime Fatal FARF message  - Run-time logging enabled");
    }
    else
    {
        /*
         * Run-time logging is disabled using HAP_setFARFRuntimeLoggingParams()
         * FARF logs with RUNTIME_ are not logged and will not appear in logcat
         */

        const char* file_name = "hap_example_farf_runtime.c";

        HAP_setFARFRuntimeLoggingParams(0, &file_name, 1);

        FARF(LOW,   "Low    FARF message : Run-time logging disabled");
        FARF(MEDIUM,"Medium FARF message : Run-time logging disabled");
        FARF(HIGH,  "High   FARF message : Run-time logging disabled");
        FARF(ERROR, "Error  FARF message : Run-time logging disabled");
        FARF(FATAL, "Fatal  FARF message : Run-time logging disabled");

        /*
         * Since run-time logging is disabled, these messages will not appear in logcat
         * but will appear in qxdm/mini-dm logs
         */
        FARF(RUNTIME_LOW,    "Runtime Low FARF message    - Run-time logging disabled");
        FARF(RUNTIME_MEDIUM, "Runtime Medium FARF message - Run-time logging disabled");
        FARF(RUNTIME_HIGH,   "Runtime High FARF message   - Run-time logging disabled");
        FARF(RUNTIME_ERROR,  "Runtime Error FARF message  - Run-time logging disabled");
        FARF(RUNTIME_FATAL,  "Runtime Fatal FARF message  - Run-time logging disabled");
    }

#endif

    return AEE_SUCCESS;
}

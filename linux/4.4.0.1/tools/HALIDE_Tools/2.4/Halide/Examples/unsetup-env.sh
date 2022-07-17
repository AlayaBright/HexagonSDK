#!/bin/bash
#
# /* ==================================================================================== */
# /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
# /*                           All Rights Reserved.                                       */
# /*                  QUALCOMM Confidential and Proprietary                               */
# /* ==================================================================================== */
#
# Usage: source unsetup-env.sh
#
#############################################################################
$(return >/dev/null 2>&1)
if [ "$?" -ne "0" ]; then
    echo "Warning: This script should be sourced, not executed."
fi

unset CURR_DIR
unset HALIDE_ROOT
unset SDK_ROOT
unset HEXAGON_SDK_VER
unset HEXAGON_SDK_ROOT
unset HEXAGON_TOOLS_VER
unset HEXAGON_TOOLS
unset BLD_TOP
unset HL_HEXAGON_SIM_REMOTE
unset Q6_VERSION
unset RUN_DEVICE_ID

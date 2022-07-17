#!/bin/bash -x
#
# /* ==================================================================================== */
# /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
# /*                           All Rights Reserved.                                       */
# /*                  QUALCOMM Confidential and Proprietary                               */
# /* ==================================================================================== */
#
# Halide setup script for explicitly specified SDK/Tools paths
#
$(return >/dev/null 2>&1)
if [ "$?" -ne "0" ]; then
    echo "Warning: This script should be sourced, not executed."
fi

export CURR_DIR=`pwd`
export HALIDE_ROOT=${HALIDE_ROOT:-$CURR_DIR/../}
export SDK_ROOT=${SDK_ROOT:-$HOME/Qualcomm}
export HEXAGON_SDK_VER=${HEXAGON_SDK_VER:-4.4}
export HEXAGON_SDK_ROOT=${HEXAGON_SDK_ROOT:-$SDK_ROOT/Hexagon_SDK/$HEXAGON_SDK_VER}
export HEXAGON_TOOLS_VER=${HEXAGON_TOOLS_VER:-8.4.12}
export HEXAGON_TOOLS=${HEXAGON_TOOLS:-$HEXAGON_SDK_ROOT/tools/HEXAGON_Tools/$HEXAGON_TOOLS_VER}

export HL_HEXAGON_SIM_REMOTE=${HL_HEXAGON_SIM_REMOTE:-$HALIDE_ROOT/bin/hexagon_sim_remote}
export Q6_VERSION=${Q6_VERSION:-65}

#BLD_TOP is the temporary work area where the examples will be built.
export BLD_TOP=${BLD_TOP:-$CURR_DIR/build}

export HALIDE_ROOT=`readlink -m $HALIDE_ROOT`
export HEXAGON_SDK_ROOT=`readlink -m $HEXAGON_SDK_ROOT`
export HEXAGON_TOOLS=`readlink -m $HEXAGON_TOOLS`
export HL_HEXAGON_SIM_REMOTE=`readlink -m $HL_HEXAGON_SIM_REMOTE`

if [ -z "${RUN_DEVICE_ID-}" ]; then
    devices=`adb devices | grep -v "devices" | awk '{print $1}' | tr '\r\n' ' '`
    export RUN_DEVICE_ID=${ANDROID_SERIAL:-$devices}
fi

echo HALIDE_ROOT=$HALIDE_ROOT
echo SDK_ROOT=$SDK_ROOT
echo HEXAGON_SDK_ROOT=$HEXAGON_SDK_ROOT
echo HEXAGON_TOOLS=$HEXAGON_TOOLS
echo HL_HEXAGON_SIM_REMOTE=$HL_HEXAGON_SIM_REMOTE
echo Q6_VERSION=$Q6_VERSION
echo RUN_DEVICE_ID="$RUN_DEVICE_ID"

# Report an error if any path doesn't exist
readlink -ve $HALIDE_ROOT > /dev/null
readlink -ve $HEXAGON_SDK_ROOT > /dev/null
readlink -ve $HEXAGON_TOOLS > /dev/null
readlink -ve $HL_HEXAGON_SIM_REMOTE > /dev/null

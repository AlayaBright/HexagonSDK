#!/bin/bash -x
#
# /* ==================================================================================== */
# /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
# /*                           All Rights Reserved.                                       */
# /*                  QUALCOMM Confidential and Proprietary                               */
# /* ==================================================================================== */
#
#
# Halide setup script for Hexagon_SDK/{VER}/tools/HALIDE_Tools/{VER} paths
#
$(return >/dev/null 2>&1)
if [ "$?" -ne "0" ]; then
    echo "Warning: This script should be sourced, not executed."
fi

export CURR_DIR=$PWD
cd ..
export HALIDE_ROOT=${HALIDE_ROOT:-$PWD}
cd ../../..
export HEXAGON_TOOLS_VER=${HEXAGON_TOOLS:-`ls HEXAGON_Tools | tail -1`}
cd ..
export HEXAGON_SDK_ROOT=${HEXAGON_SDK_ROOT:-$PWD}
export HEXAGON_TOOLS=${HEXAGON_TOOLS:-$HEXAGON_SDK_ROOT/tools/HEXAGON_Tools/$HEXAGON_TOOLS_VER}
cd ../..
export SDK_ROOT=${SDK_ROOT:-$PWD}
cd $CURR_DIR

source setup-env.sh

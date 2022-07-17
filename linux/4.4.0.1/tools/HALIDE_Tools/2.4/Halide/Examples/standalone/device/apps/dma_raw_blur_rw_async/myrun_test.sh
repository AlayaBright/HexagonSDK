#!/bin/bash
#
# /* ==================================================================================== */
# /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
# /*                           All Rights Reserved.                                       */
# /*                  QUALCOMM Confidential and Proprietary                               */
# /* ==================================================================================== */
#
export HL_NUM_THREADS=16
adb shell  HL_NUM_THREADS=16 ADSP_LIBRARY_PATH=/data/ /data/main-dma_raw_blur_rw_async.out 512 512 512 0 0 "/data/adb_raw.bin" "/data/adb_raw.bin"

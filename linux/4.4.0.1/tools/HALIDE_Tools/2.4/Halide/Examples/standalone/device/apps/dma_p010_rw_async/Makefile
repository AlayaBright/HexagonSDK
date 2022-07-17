#/* ============================================================================================== */
#/*  QUALCOMM TECHNOLOGIES, INC.                                                                   */
#/*                                                                                                */
#/*  HVX Intrinsics Examples with Boundary Conditions                                              */
#/*                                                                                                */
#/* ---------------------------------------------------------------------------------------------- */
#/*          Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.          */
#/*                           All Rights Reserved.                                                 */
#/*                  QUALCOMM Confidential and Proprietary                                         */
#/* ============================================================================================== */

CURR_DIR = $(shell pwd)
DEVICE_STANDALONE_TEST_TOP_DIR = ${CURR_DIR}/../..
TESTNAME = $(shell basename ${CURR_DIR})
TESTIMAGE1 = $(CURR_DIR)/../../Images/img11_p010_ubwc_hana.bin
TESTIMAGE2 = $(CURR_DIR)/../../Images/img11_p010.bin

FEATURES ?= hexagon_dma-hvx_128
Q6_VERSION = 66
ifeq ($(shell expr $$Q6_VERSION + 0 \> 66), 1)
  Q6_VERSION = $(shell echo $$Q6_VERSION)
endif

include $(DEVICE_STANDALONE_TEST_TOP_DIR)/apps/Makefile.common

TEST_ARGS=1024 1024 1024 4 0 0 "$(DEVICE_TEST_PATH)/img11_p010.bin" "$(DEVICE_TEST_PATH)/img11_p010.bin"

include $(DEVICE_STANDALONE_TEST_TOP_DIR)/apps/Makefile.rules


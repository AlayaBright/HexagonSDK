#/* ============================================================================================== */
#/*  QUALCOMM TECHNOLOGIES, INC.                                                                   */
#/*                                                                                                */
#/*  Halide Benchmarks/Examples in Device-standalone mode                                          */
#/*                                                                                                */
#/* ---------------------------------------------------------------------------------------------- */
#/*          Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.          */
#/*                           All Rights Reserved.                                                 */
#/*                  QUALCOMM Confidential and Proprietary                                         */
#/* ============================================================================================== */

include ../Makefile.common

HALIDE_TEST_TOP_DIR ?= $(shell pwd)/../../..
DEVICE_STANDALONE_TEST_TOP_DIR ?= $(HALIDE_TEST_TOP_DIR)/standalone/device
DEVICE_STANDALONE_UTILS_DIR = $(DEVICE_STANDALONE_TEST_TOP_DIR)/utils
BLD_TOP ?= $(DEVICE_STANDALONE_TEST_TOP_DIR)/build
UTILS_BLD_TOP ?= $(BLD_TOP)/standalone/device/utils
UTILS_BIN ?= $(UTILS_BLD_TOP)/bin

ARM_INCLUDES =  -I$(HEXAGON_SDK_ROOT)/ipc/fastrpc/rpcmem/inc -I$(HEXAGON_SDK_ROOT)/ipc/fastrpc/remote/ship/android
ARM_CXXFLAGS= -target aarch64-linux-android21 $(ARM_INCLUDES) -fsigned-char
ARM_LDFLAGS = -llog -fPIE -pie -l$(DSPTYPE)rpc -L$(HEXAGON_SDK_ROOT)/ipc/fastrpc/remote/ship/android_aarch64

.PHONY : all clean

all: $(UTILS_BIN)/libion_allocation.a

$(UTILS_BIN)/ion_allocation.o: $(DEVICE_STANDALONE_UTILS_DIR)/ion/ion_allocation.cpp $(DEVICE_STANDALONE_UTILS_DIR)/ion/ion_allocation.h
	mkdir -p $(@D)
	$(ARMCXX) $(ARM_CXXFLAGS) -O3  -c $< -o $@

$(UTILS_BIN)/libion_allocation.a: $(UTILS_BIN)/ion_allocation.o
	ar q $@ $^

clean:
	rm -rf $(UTILS_BIN)

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

include Makefile.common

HALIDE_TEST_TOP_DIR ?= $(shell pwd)/../..
DEVICE_STANDALONE_TEST_TOP_DIR ?= $(HALIDE_TEST_TOP_DIR)/standalone/device
DEVICE_STANDALONE_APPS_TOP_DIR ?= $(DEVICE_STANDALONE_TEST_TOP_DIR)/apps
DEVICE_STANDALONE_UTILS_DIR = $(DEVICE_STANDALONE_TEST_TOP_DIR)/utils

.PHONY : run all utils clean apps

run: utils apps

all: utils apps

apps:
	make -C $(DEVICE_STANDALONE_APPS_TOP_DIR) all

utils:
	make -C $(DEVICE_STANDALONE_UTILS_DIR) all

clean:
	make -C $(DEVICE_STANDALONE_APPS_TOP_DIR) clean

distclean:
	make -C $(DEVICE_STANDALONE_APPS_TOP_DIR) clean
	make -C $(DEVICE_STANDALONE_UTILS_DIR) clean

report_txt:
	make -C $(DEVICE_STANDALONE_APPS_TOP_DIR) report_txt

report_json:
	make -C $(DEVICE_STANDALONE_APPS_TOP_DIR) report_json

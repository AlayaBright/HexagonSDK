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
SIMULATOR_STANDALONE_TEST_TOP_DIR ?= $(HALIDE_TEST_TOP_DIR)/standalone/simulator
SIMULATOR_STANDALONE_APPS_TOP_DIR ?= $(SIMULATOR_STANDALONE_TEST_TOP_DIR)/apps

.PHONY : run all utils clean apps

run: apps

all: apps

apps:
	make -C $(SIMULATOR_STANDALONE_APPS_TOP_DIR) all

clean:
	make -C $(SIMULATOR_STANDALONE_APPS_TOP_DIR) clean

bmark-report:
	make -C $(SIMULATOR_STANDALONE_APPS_TOP_DIR) bmark-report

report_json:
	make -C $(SIMULATOR_STANDALONE_APPS_TOP_DIR) report_json

report_txt:
	make -C $(SIMULATOR_STANDALONE_APPS_TOP_DIR) report_txt

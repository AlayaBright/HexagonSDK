#/* ============================================================================================== */
#/*  QUALCOMM TECHNOLOGIES, INC.                                                                   */
#/*                                                                                                */
#/*  Halide Benchmarks/Examples in offload mode                                                    */
#/*                                                                                                */
#/* ---------------------------------------------------------------------------------------------- */
#/*          Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.          */
#/*                           All Rights Reserved.                                                 */
#/*                  QUALCOMM Confidential and Proprietary                                         */
#/* ============================================================================================== */

include Makefile.common

HALIDE_TEST_TOP_DIR ?= $(shell pwd)/..
OFFLOAD_TEST_TOP_DIR ?= $(HALIDE_TEST_TOP_DIR)/offload
OFFLOAD_APPS_TOP_DIR ?= $(OFFLOAD_TEST_TOP_DIR)/apps

.PHONY : run all utils clean apps report_txt

run: apps

all: apps

apps:
	make -C $(OFFLOAD_APPS_TOP_DIR) all

clean:
	make -C $(OFFLOAD_APPS_TOP_DIR) clean

report_txt:
	make -C $(OFFLOAD_APPS_TOP_DIR) report_txt

#/* ============================================================================================== */
#/*  QUALCOMM TECHNOLOGIES, INC.                                                                   */
#/*                                                                                                */
#/*  Halide Benchmarks/Examples                                                                    */
#/*                                                                                                */
#/* ---------------------------------------------------------------------------------------------- */
#/*          Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.          */
#/*                           All Rights Reserved.                                                 */
#/*                  QUALCOMM Confidential and Proprietary                                         */
#/* ============================================================================================== */

HALIDE_TEST_TOP_DIR ?= $(shell pwd)
OFFLOAD_TESTS_TOP_DIR ?= $(HALIDE_TEST_TOP_DIR)/offload
STANDALONE_TESTS_TOP_DIR ?= $(HALIDE_TEST_TOP_DIR)/standalone
BLD_TOP ?= $(shell pwd)
.PHONY : clean dev-offload sim-offload sim-standalone dev-standalone all

all: dev-offload sim-offload sim-standalone dev-standalone

clean:
	make -C $(OFFLOAD_TESTS_TOP_DIR) clean
	make -C $(STANDALONE_TESTS_TOP_DIR) clean

run:
	make -C $(OFFLOAD_TESTS_TOP_DIR) run
	make -C $(STANDALONE_TESTS_TOP_DIR) run

dev-offload:
	$(MAKE) -C $(OFFLOAD_TESTS_TOP_DIR)/apps run-arm-64-android

sim-offload:
	$(MAKE) -C $(OFFLOAD_TESTS_TOP_DIR)/apps run-host

dev-standalone:
	$(MAKE) -C $(STANDALONE_TESTS_TOP_DIR)/device run

sim-standalone:
	$(MAKE) -C $(STANDALONE_TESTS_TOP_DIR)/simulator run

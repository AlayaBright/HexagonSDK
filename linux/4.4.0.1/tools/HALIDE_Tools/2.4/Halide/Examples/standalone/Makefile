#/* ============================================================================================== */
#/*  QUALCOMM TECHNOLOGIES, INC.                                                                   */
#/*                                                                                                */
#/*  Halide Benchmarks/Examples in standalone mode                                                 */
#/*                                                                                                */
#/* ---------------------------------------------------------------------------------------------- */
#/*          Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.          */
#/*                           All Rights Reserved.                                                 */
#/*                  QUALCOMM Confidential and Proprietary                                         */
#/* ============================================================================================== */

HALIDE_TEST_TOP_DIR ?= $(shell pwd)/..
STANDALONE_TESTS_TOP_DIR ?= $(HALIDE_TEST_TOP_DIR)/standalone
DEVICE_STANDALONE_TESTS_TOP_DIR ?= $(STANDALONE_TESTS_TOP_DIR)/device
SIMULATOR_STANDALONE_TESTS_TOP_DIR ?= $(STANDALONE_TESTS_TOP_DIR)/simulator

.PHONY : clean run report_txt report_json


clean:
# At this level, when we say clean, we mean distclean for
#device-standalone mode tests
	make -C $(DEVICE_STANDALONE_TESTS_TOP_DIR) distclean
	make -C $(SIMULATOR_STANDALONE_TESTS_TOP_DIR) clean

run:
# run in device standalone will take care of building utils.
	make -C $(DEVICE_STANDALONE_TESTS_TOP_DIR) run
	make -C $(SIMULATOR_STANDALONE_TESTS_TOP_DIR) run

report_txt:
	make -C $(DEVICE_STANDALONE_TESTS_TOP_DIR) report_txt
	make -C $(SIMULATOR_STANDALONE_TESTS_TOP_DIR) report_txt

report_json:
	make -C $(DEVICE_STANDALONE_TESTS_TOP_DIR) report_json
	make -C $(SIMULATOR_STANDALONE_TESTS_TOP_DIR) report_json

#/* ======================================================================= */
#/*  QUALCOMM TECHNOLOGIES, INC.                                            */
#/*                                                                         */
#/*  3x3 Convolution                                                        */
#/*                                                                         */
#/* ----------------------------------------------------------------------- */
#/*          Copyright (c) 2016-2021 QUALCOMM TECHNOLOGIES Incorporated.    */
#/*                           All Rights Reserved.                          */
#/*                  QUALCOMM Confidential and Proprietary                  */
#/* ======================================================================= */

CURR_DIR = $(shell pwd)
DEVICE_STANDALONE_TEST_TOP_DIR = ${CURR_DIR}/../..
TESTNAME = $(shell basename ${CURR_DIR})

include $(DEVICE_STANDALONE_TEST_TOP_DIR)/apps/Makefile.common

# Redefine these values.
TEST_ARGS=1024 1024 128 1000

NUM_BATCHES ?= 10
BATCH_SIZE ?= 32
SAMPLES = $(BLD_DIR)/samples
AUTOTUNE_SCHEDULE_INCLUDES = $(BIN)/includes

include $(DEVICE_STANDALONE_TEST_TOP_DIR)/apps/Makefile.rules

.PHONY: autoschedule autotune

autoschedule: AUTOSCHEDULE_OPTS = auto_schedule=true -p $(HALIDE_ROOT)/lib/libauto_schedule.so -s Adams2019
autoschedule: run

autotune: $(AUTOTUNE_SCHEDULE_INCLUDES)/$(TESTNAME)_halide.schedule.h
	@echo "Autotuning complete"
	rm -f $(BIN)/$(TESTNAME)_generate $(BIN)/remote/lib$(TESTNAME)_skel.so
	# Benchmark the generated schedule file on device in standalone mode.
	HL_PERMIT_FAILED_UNROLL=1 USE_SCHEDULE=true SCHEDULE_INCLUDES=$(AUTOTUNE_SCHEDULE_INCLUDES) make run

$(AUTOTUNE_SCHEDULE_INCLUDES)/%_halide.schedule.h: $(BIN)/%_generate
	mkdir -p $(@D)
	$(HALIDE_ROOT)/bin/autotune_loop.py $^ $*_halide --target $(HALIDE_TARGET) --bld-dir $(BLD_DIR) --num-batches $(NUM_BATCHES) --batch-size $(BATCH_SIZE) -sdk $(HEXAGON_SDK_ROOT) --run-device-id $(RUN_DEVICE_ID)
	cp $(SAMPLES)/best.*.schedule.h $(AUTOTUNE_SCHEDULE_INCLUDES)/$*_halide.schedule.h



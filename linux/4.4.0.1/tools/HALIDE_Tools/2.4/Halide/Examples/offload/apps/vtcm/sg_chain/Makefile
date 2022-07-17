# /* ==================================================================================== */
# /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
# /*                           All Rights Reserved.                                       */
# /*                  QUALCOMM Confidential and Proprietary                               */
# /* ==================================================================================== */

include ../../../Makefile.common

CXXFLAGS-host ?=
CXXFLAGS-arm-64-android ?=

LDFLAGS-host ?= -lpthread -ldl -lm
LDFLAGS-arm-64-android ?= -llog -fPIE -pie
LDFLAGS-arm-64-profile-android ?= -llog -fPIE -pie

SG_CHAIN_DEVICE_ITERATIONS ?= 5
SIMULATOR_ITERATIONS ?= 1

SRC = $(shell pwd)
BLD_TOP ?= $(shell pwd)
BLD_DIR ?= $(BLD_TOP)/offload/vtcm/sg_chain
BIN ?= $(BLD_DIR)/bin
FEATURES ?= hvx_128
VTCM ?= true
SCHEDULING_OPTS = use_vtcm=${VTCM}
DEVICE_PATH ?= /data/tmp/vtcm/sg_chain

.PHONY : run all clean
CXXFLAGS += -I $(SRC)/../../../../include -I includes
GENERATOR_CXXFLAGS += -I includes

run: run-arm-64-android
all: run-arm-64-android run-host

$(BIN)/pipeline_generator: pipeline.cpp includes/process.h $(GENERATOR_DEPS)
	@mkdir -p $(@D)
	$(CXX) $(GENERATOR_CXXFLAGS) -g -fno-rtti $(filter-out %.h,$^) -o $@ $(GENERATOR_LDFLAGS)

$(BIN)/%/pipeline.o: $(BIN)/pipeline_generator
	@mkdir -p $(@D)
	$^ -g sg_chain -o $(BIN)/$* -e o,h -f pipeline target=$*-$(FEATURES) $(SCHEDULING_OPTS)

$(BIN)/process-%: process.cpp includes/process.h $(BIN)/%/pipeline.o
	$(CXX-$*) $(CXXFLAGS) $(CXXFLAGS-$*) -I $(BIN)/$* -Wall -O3 process.cpp $(BIN)/$*/pipeline.o -o $@ $(LDFLAGS-$*) $(DASH_D_DEFINES)

run-%-android: $(BIN)/process-%-android
	for DEVICE in $(RUN_DEVICE_ID); do \
		mkdir -p $(BIN)/$$DEVICE/; \
		rm -rf $(BIN)/$$DEVICE/* ; \
		echo "device_id:$$DEVICE" > $(BIN)/$$DEVICE/device.txt; \
		echo Result:Unknown > $(BIN)/$$DEVICE/result; \
		echo Mode:device-offload >> $(BIN)/$$DEVICE/result; \
		echo Name:vtcm/sg_chain >> $(BIN)/$$DEVICE/result; \
		echo "Features:$(FEATURES)" >> $(BIN)/$$DEVICE/result; \
		adb -s $$DEVICE shell mkdir -p $(DEVICE_PATH); \
		adb -s $$DEVICE push $(BIN)/process-$*-android $(DEVICE_PATH); \
		adb -s $$DEVICE shell chmod +x $(DEVICE_PATH)/process-$*-android; \
		adb -s $$DEVICE shell getprop ro.serialno; \
		adb -s $$DEVICE shell FEATURES=$(FEATURES) $(DEVICE_PATH)/process-$*-android $(SG_CHAIN_DEVICE_ITERATIONS) 2>&1 | tee $(BIN)/$$DEVICE/run.stdout ; \
	done

run-host: $(BIN)/process-host
	echo Result:Unknown > $(BIN)/result ; \
	echo Mode:simulator-offload >> $(BIN)/result ; \
	echo Name:vtcm/sg_chain >> $(BIN)/result ; \
	echo "Features:$(FEATURES)" >> $(BIN)/result ; \
	LD_LIBRARY_PATH=$(HALIDE_ROOT)/lib/host:$(HEXAGON_TOOLS)/Tools/lib/iss:$$LD_LIBRARY_PATH FEATURES=$(FEATURES) $(BIN)/process-host $(SIMULATOR_ITERATIONS) 2>&1 | tee $(BIN)/run.stdout

clean:
	rm -rf $(BIN)

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

include ../../Makefile.common
#include ../support/Makefile.inc

HEXAGON_BENCHMARKS_FILTERS ?= "conv3x3a16,dilate3x3,median3x3,gaussian5x5,sobel,morph_edge"

HEXAGON_BENCHMARKS_DEVICE_ITERATIONS ?= 10
HEXAGON_BENCHMARKS_SIMULATOR_ITERATIONS ?= 1

CXXFLAGS-host ?=
CXXFLAGS-arm-64-android ?=
CXXFLAGS +=  -I $(SRC)/../../../include

LDFLAGS-host ?= -lpthread -ldl -lm
LDFLAGS-arm-64-android ?= -llog -fPIE -pie
LDFLAGS-arm-64-profile-android ?= -llog -fPIE -pie
FEATURES ?= hvx_128

ifneq ($(FEATURES),)
	TARGET_FEATURES = -$(FEATURES)
else
	TARGET_FEATURES = $(FEATURES)
endif

SRC = $(shell pwd)
BLD_TOP ?= $(shell pwd)
BLD_DIR ?= $(BLD_TOP)/offload/hexagon_benchmarks
BIN ?= $(if $(filter $(USE_PYTHON),1),$(BLD_DIR)/bin_python,$(BLD_DIR)/bin)


CONV_DEPS = $(if $(filter $(USE_PYTHON),1),$(SRC)/conv3x3_generator.py,$(BIN)/conv3x3.generator)
DILATE_DEPS = $(if $(filter $(USE_PYTHON),1),$(SRC)/dilate3x3_generator.py,$(BIN)/dilate3x3.generator)
MEDIAN_DEPS = $(if $(filter $(USE_PYTHON),1),$(SRC)/median3x3_generator.py,$(BIN)/median3x3.generator)
GAUSSIAN_DEPS = $(if $(filter $(USE_PYTHON),1),$(SRC)/gaussian5x5_generator.py,$(BIN)/gaussian5x5.generator)
SOBEL_DEPS = $(if $(filter $(USE_PYTHON),1),$(SRC)/sobel_generator.py,$(BIN)/sobel.generator)
MORPH_EDGE_DEPS = $(if $(filter $(USE_PYTHON),1),$(SRC)/morph_edge_generator.py,$(BIN)/morph_edge.generator)

PYTHON_EXEC = LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(HALIDE_ROOT)/lib PYTHONPATH=$(PYTHONPATH):$(HALIDE_ROOT)/lib:$(HALIDE_ROOT)/tools $(PYTHON)
# empty string if running compiled generator, else $(PYTHON_EXEC)
EXEC_CMD = $(if $(filter $(USE_PYTHON),1),$(PYTHON_EXEC),)

FILTERS = $(shell echo "$(HEXAGON_BENCHMARKS_FILTERS)" | sed -e "s/,/ /g")
OBJS = $(patsubst %,$(BIN)/\%/%.o, $(FILTERS))

AUTOTUNE_DEPS = $(patsubst %,$(BIN)/%_autotune, $(FILTERS))

NUM_BATCHES ?= 10
BATCH_SIZE ?= 32

LINK_OBJS = $(patsubst %,$(BIN)/$$*/%.o, $(FILTERS))

UPPERCASE_FILTERS = $(shell echo $(FILTERS) | tr '[:lower:]' '[:upper:]')
DASH_D_DEFINES += $(patsubst %, -D%=1, $(UPPERCASE_FILTERS))

PARALLEL_SCHED ?= true
PREFETCH_SCHED ?= true
SCHEDULING_OPTS = use_parallel_sched=${PARALLEL_SCHED} use_prefetch_sched=${PREFETCH_SCHED}

.PRECIOUS: $(BIN)/%.generator

$(BIN)/%.generator : %_generator.cpp $(GENERATOR_DEPS)
	@mkdir -p $(@D)
	$(CXX) $(GENERATOR_CXXFLAGS) -O3 -g -fno-rtti $(USE_EXPORT_DYNAMIC) $(filter-out %.h,$^) -o $@ $(LDFLAGS) $(GENERATOR_LDFLAGS)

$(BIN)/%/conv3x3a16.o: $(CONV_DEPS)
	@mkdir -p $(@D)
	$(EXEC_CMD) $^ -g conv3x3 -o $(BIN)/$* -e $(EMIT_OPTIONS) -f conv3x3a16 target=$*$(TARGET_FEATURES) accumulator_type=int16 $(SCHEDULING_OPTS) \
	auto_schedule=$(AUTOSCHEDULE) $(AUTOSCHEDULE_OPTS)


$(BIN)/%/dilate3x3.o: $(DILATE_DEPS)
	@mkdir -p $(@D)
	$(EXEC_CMD) $^ -g dilate3x3 -o $(BIN)/$* -e $(EMIT_OPTIONS) -f dilate3x3 target=$*$(TARGET_FEATURES) $(SCHEDULING_OPTS) \
	auto_schedule=$(AUTOSCHEDULE) $(AUTOSCHEDULE_OPTS)

$(BIN)/%/median3x3.o: $(MEDIAN_DEPS)
	@mkdir -p $(@D)
	$(EXEC_CMD) $^ -g median3x3 -o $(BIN)/$* -e $(EMIT_OPTIONS) -f median3x3 target=$*$(TARGET_FEATURES) $(SCHEDULING_OPTS) \
	auto_schedule=$(AUTOSCHEDULE) $(AUTOSCHEDULE_OPTS)

$(BIN)/%/gaussian5x5.o: $(GAUSSIAN_DEPS)
	@mkdir -p $(@D)
	$(EXEC_CMD) $^ -g gaussian5x5 -o $(BIN)/$* -e $(EMIT_OPTIONS) -f gaussian5x5 target=$*$(TARGET_FEATURES) $(SCHEDULING_OPTS) \
	auto_schedule=$(AUTOSCHEDULE) $(AUTOSCHEDULE_OPTS)

$(BIN)/%/sobel.o: $(SOBEL_DEPS)
	@mkdir -p $(@D)
	$(EXEC_CMD) $^ -g sobel -o $(BIN)/$* -e $(EMIT_OPTIONS) -f sobel target=$*$(TARGET_FEATURES) $(SCHEDULING_OPTS) \
	auto_schedule=$(AUTOSCHEDULE) $(AUTOSCHEDULE_OPTS)

$(BIN)/%/morph_edge.o: $(MORPH_EDGE_DEPS)
	@mkdir -p $(@D)
	$(EXEC_CMD) $^ -g morph_edge -o $(BIN)/$* -e $(EMIT_OPTIONS) -f morph_edge target=$*$(TARGET_FEATURES) $(SCHEDULING_OPTS) \
	auto_schedule=$(AUTOSCHEDULE) $(AUTOSCHEDULE_OPTS)

$(BIN)/%/conv3x3a32.o: $(CONV_DEPS)
	@mkdir -p $(@D)
	$(EXEC_CMD) $^ -g conv3x3 -o $(BIN)/$* -e $(EMIT_OPTIONS) -f conv3x3a32 target=$*$(TARGET_FEATURES) accumulator_type=int32 $(SCHEDULING_OPTS) \
	auto_schedule=$(AUTOSCHEDULE) $(AUTOSCHEDULE_OPTS)

$(BIN)/%/filters.a : $(OBJS)
	ar q $(BIN)/$*/filters.a $^

$(BIN)/process-%: process.cpp $(BIN)/%/filters.a process.h
	@mkdir -p $(@D)
	$(CXX-$*) $(CXXFLAGS) $(CXXFLAGS-$*) -I $(BIN)/$*  $(DASH_D_DEFINES) -Wall -O0 -g process.cpp $(BIN)/$*/filters.a -o $(BIN)/process-$* $(LDFLAGS-$*)


$(BIN)/conv3x3a16_autotune: $(BIN)/conv3x3.generator
	$(HALIDE_ROOT)/bin/autotune_loop.py $< $(patsubst %.generator,%,$(<F)) -hr $(HALIDE_ROOT) -sdk $(HEXAGON_SDK_ROOT) -t arm-64-android-$(FEATURES) \
	--bld-dir $(@) -n $(NUM_BATCHES) -s $(BATCH_SIZE) -r $(RUN_DEVICE_ID) --generator-arg-sets accumulator_type=int16
	ln -s $(@)/samples/best.*.schedule.h $(BIN)/$(patsubst %.generator,%,$(<F)).schedule.h

$(BIN)/dilate3x3_autotune: $(BIN)/dilate3x3.generator
	$(HALIDE_ROOT)/bin/autotune_loop.py $< $(patsubst %.generator,%,$(<F)) -hr $(HALIDE_ROOT) -sdk $(HEXAGON_SDK_ROOT) -t arm-64-android-$(FEATURES) \
	--bld-dir $(@) -n $(NUM_BATCHES) -s $(BATCH_SIZE) -r $(RUN_DEVICE_ID)
	ln -s $(@)/samples/best.*.schedule.h $(BIN)/$(patsubst %.generator,%,$(<F)).schedule.h

$(BIN)/median3x3_autotune: $(BIN)/median3x3.generator
	$(HALIDE_ROOT)/bin/autotune_loop.py $< $(patsubst %.generator,%,$(<F)) -hr $(HALIDE_ROOT) -sdk $(HEXAGON_SDK_ROOT) -t arm-64-android-$(FEATURES) \
	--bld-dir $(@) -n $(NUM_BATCHES) -s $(BATCH_SIZE) -r $(RUN_DEVICE_ID)
	ln -s $(@)/samples/best.*.schedule.h $(BIN)/$(patsubst %.generator,%,$(<F)).schedule.h

$(BIN)/gaussian5x5_autotune: $(BIN)/gaussian5x5.generator
	$(HALIDE_ROOT)/bin/autotune_loop.py $< $(patsubst %.generator,%,$(<F)) -hr $(HALIDE_ROOT) -sdk $(HEXAGON_SDK_ROOT) -t arm-64-android-$(FEATURES) \
	--bld-dir $(@) -n $(NUM_BATCHES) -s $(BATCH_SIZE) -r $(RUN_DEVICE_ID)
	ln -s $(@)/samples/best.*.schedule.h $(BIN)/$(patsubst %.generator,%,$(<F)).schedule.h

$(BIN)/sobel_autotune: $(BIN)/sobel.generator
	$(HALIDE_ROOT)/bin/autotune_loop.py $< $(patsubst %.generator,%,$(<F)) -hr $(HALIDE_ROOT) -sdk $(HEXAGON_SDK_ROOT) -t arm-64-android-$(FEATURES) \
	--bld-dir $(@) -n $(NUM_BATCHES) -s $(BATCH_SIZE) -r $(RUN_DEVICE_ID)
	ln -s $(@)/samples/best.*.schedule.h $(BIN)/$(patsubst %.generator,%,$(<F)).schedule.h

$(BIN)/morph_edge_autotune: $(BIN)/morph_edge.generator
	$(HALIDE_ROOT)/bin/autotune_loop.py $< $(patsubst %.generator,%,$(<F)) -hr $(HALIDE_ROOT) -sdk $(HEXAGON_SDK_ROOT) -t arm-64-android-$(FEATURES) \
	--bld-dir $(@) -n $(NUM_BATCHES) -s $(BATCH_SIZE) -r $(RUN_DEVICE_ID)
	ln -s $(@)/samples/best.*.schedule.h $(BIN)/$(patsubst %.generator,%,$(<F)).schedule.h

$(BIN)/conv3x3a32_autotune: $(BIN)/conv3x3.generator
	$(HALIDE_ROOT)/bin/autotune_loop.py $< $(patsubst %.generator,%,$(<F)) -hr $(HALIDE_ROOT) -sdk $(HEXAGON_SDK_ROOT) -t arm-64-android-$(FEATURES) \
	--bld-dir $(@) -n $(NUM_BATCHES) -s $(BATCH_SIZE) -r $(RUN_DEVICE_ID) --generator-arg-sets accumulator_type=int32
	ln -s $(@)/samples/best.*.schedule.h $(BIN)/$(patsubst %.generator,%,$(<F)).schedule.h


# Autotunes and prints the result in TestReport format.
autotune: $(AUTOTUNE_DEPS)
	@echo "Autotuning complete"
	for DIR in $(AUTOTUNE_DEPS); do \
		cat $$DIR/samples/best.*.benchmark.txt; \
	done
	rm $(BIN)/*.generator
	# Benchmark the generated schedule files on device.
	HL_PERMIT_FAILED_UNROLL=1 HEXAGON_BENCHMARKS_FILTERS=$(HEXAGON_BENCHMARKS_FILTERS) USE_SCHEDULE=true SCHEDULE_INCLUDES=$(BIN) make run

#test: $(BIN)/host/process
#	$(BIN)/host/process -n $(ITERATIONS)

clean:
	rm -rf $(BIN)

.PHONY : run all clean autotune
run : all

all: run-arm-64-android #run-host

DEVICE_PATH ?= /data/tmp/hexagon_benchmarks
#DEVICE_ENV = "LD_LIBRARY_PATH=$(DEVICE_PATH):/vendor/lib64 ADSP_LIBRARY_PATH=\"$(DEVICE_PATH);/dsp\""

run-%-android: $(BIN)/process-%-android
	for DEVICE in $(RUN_DEVICE_ID); do \
		mkdir -p $(BIN)/$$DEVICE/; \
	    rm -rf $(BIN)/$$DEVICE/* ; \
	    echo "device_id:$$DEVICE" > $(BIN)/$$DEVICE/device.txt ; \
	    echo "Features:$(FEATURES)" > $(BIN)/$$DEVICE/result; \
	    echo Result:Unknown >> $(BIN)/$$DEVICE/result ; \
	    echo Mode:device-offload >> $(BIN)/$$DEVICE/result ; \
	    echo Name:hexagon_benchmarks >> $(BIN)/$$DEVICE/result ; \
	    echo "Filters:$(HEXAGON_BENCHMARKS_FILTERS)" >> $(BIN)/$$DEVICE/result ; \
	    adb -s $$DEVICE shell mkdir -p $(DEVICE_PATH) ; \
	    adb -s $$DEVICE push $(BIN)/process-$*-android $(DEVICE_PATH) ; \
	    adb -s $$DEVICE shell getprop ro.serialno; \
		adb -s $$DEVICE shell chmod +x $(DEVICE_PATH)/process-$*-android ; \
	    adb -s $$DEVICE shell cp /vendor/lib/rfsa/adsp/testsig*.so $(DEVICE_PATH)/; \
	    adb -s $$DEVICE shell cp /vendor/lib/rfsa/adsp/libhalide_hexagon_remote_skel.so $(DEVICE_PATH)/; \
	    adb -s $$DEVICE shell  ADSP_LIBRARY_PATH=${DEVICE_PATH} ${DEVICE_PATH}/process-$*-android -n $(HEXAGON_BENCHMARKS_DEVICE_ITERATIONS) 2>&1 | tee $(BIN)/$$DEVICE/run.stdout ; \
	done

run-host: $(BIN)/process-host
	echo Result:Unknown > $(BIN)/result
	echo "Features:$(FEATURES)" >> $(BIN)/result; \
	echo Mode:simulator-offload >> $(BIN)/result; \
	echo Name:hexagon_benchmarks >> $(BIN)/result; \
	echo "Filters:$(HEXAGON_BENCHMARKS_FILTERS)" >> $(BIN)/result; \
	LD_LIBRARY_PATH=$(HALIDE_ROOT)/lib/host:$(HEXAGON_TOOLS)/Tools/lib/iss:$$LD_LIBRARY_PATH $(BIN)/process-host -n $(HEXAGON_BENCHMARKS_SIMULATOR_ITERATIONS) 2>&1 | tee $(BIN)/run.stdout

.PHONY: report_txt

report_txt:
	for DEVICE in $(RUN_DEVICE_ID); do \
		if [ -d $(BIN)/$$DEVICE ]; then \
	 	   rm -rf $(BIN)/$$DEVICE/$(REPORT_TXT); \
	 	   if [[ -e $(BIN)/$$DEVICE/run.stdout && -e $(BIN)/$$DEVICE/result ]]; then \
	 	    	echo "FIXME: report_txt"; \
	 	   else \
	 	    	echo "run.stdout or result file not found; please run make run first"; \
	 	   fi; \
	    else \
	 	  echo "Test not run yet. Do make run"; \
	    fi; \
	done


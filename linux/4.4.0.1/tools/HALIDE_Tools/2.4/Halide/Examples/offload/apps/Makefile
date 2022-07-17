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


include ../Makefile.common
APPS ?= $(shell ls -l | grep '^d' | awk '{print $$9}')

.PHONY : clean run all

clean:
	for app in $(APPS); do \
		$(MAKE) -C $$app clean; \
	done

all: run

run:
	for app in $(APPS); do \
		$(MAKE) -C $$app run; \
	done

run-arm-64-android:
	for app in $(APPS); do \
		$(MAKE) -C $$app run-arm-64-android; \
	done

run-host:
	for app in $(APPS); do \
		$(MAKE) -C $$app run-host; \
	done

report_txt:
	for app in $(APPS); do \
		$(MAKE) -C $$app report_txt; \
	done

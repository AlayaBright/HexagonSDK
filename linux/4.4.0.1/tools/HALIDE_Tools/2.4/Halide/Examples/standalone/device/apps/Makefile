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
APPS ?= $(shell ls -l | grep '^d' | awk '{print $$9}') 

.PHONY : clean run bmark-report all

clean:
	for app in $(APPS); do \
		$(MAKE) -C $$app clean; \
	done

distclean:
	for app in $(APPS); do \
		$(MAKE) -C $$app distclean; \
	done

all: run bmark-report

run:
	for app in $(APPS); do \
		$(MAKE) -C $$app run; \
	done

report_txt:
	for app in $(APPS); do \
		$(MAKE) -C $$app report_txt; \
	done

report_json:
	for app in $(APPS); do \
		$(MAKE) -C $$app report_json; \
	done




#/* ============================================================================================== */
#/*  QUALCOMM TECHNOLOGIES, INC.                                                                   */
#/*                                                                                                */
#/*  Halide for HVX Image Processing Example                                                       */
#/*                                                                                                */
#/* ---------------------------------------------------------------------------------------------- */
#/*          Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.          */
#/*                           All Rights Reserved.                                                 */
#/*                  QUALCOMM Confidential and Proprietary                                         */
#/* ============================================================================================== */
SUBDIRS ?= $(shell ls -l | grep '^d' | awk '{print $$9}') 

.PHONY : clean run run-prof bmark-report all
all : run

distclean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm -rf utils/bin

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

run:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir run; \
	done

run-prof:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir run-prof; \
	done

baseline:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir baseline; \
	done

bmark-report:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir bmark-report; \
	done

report_json:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir report_json; \
	done

report_txt:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir report_txt; \
	done

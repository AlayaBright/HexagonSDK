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

VTCM_TESTS = gather \
             histogram \
             scatter \
             sg_chain \
             vtcm_alloc \
			 small_lut

.PHONY : clean run run-host run-arm-64-android
all : run

clean:
	for dir in $(VTCM_TESTS); do \
		$(MAKE) -C $$dir clean; \
	done

run:
	for dir in $(VTCM_TESTS); do \
		$(MAKE) -C $$dir run; \
	done

run-host:
	for dir in $(VTCM_TESTS); do \
		$(MAKE) -C $$dir run-host; \
	done

run-arm-64-android:
	for dir in $(VTCM_TESTS); do \
		$(MAKE) -C $$dir run-arm-64-android; \
	done

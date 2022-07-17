##################################################################
# Copyright (c) $Date: 2012/03/07 01:00:01 $ QUALCOMM INCORPORATED.
# All Rights Reserved.
# Modified by QUALCOMM INCORPORATED on $Date: 2012/03/07 01:00:01 $
##################################################################
#
# Linux makefile to build and run the QFloat example

all: clean build sim

TESTFILE = QFloat

CC  = hexagon-clang
CXX = hexagon-clang++

Q6VERSION := v68

CFLAGS = -m${Q6VERSION} -O2 -G0 -mhvx -mhvx-length=128B -Wall
OBJS = $(TESTFILE).o

build: $(OBJS)
	$(CC) -m${Q6VERSION} $(OBJS) -o $(TESTFILE).elf


%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.i
	$(CC) $(CFLAGS) -c $< -o $@

%.s:%.i
	$(CC) $(CFLAGS) -S $< -o $@

read:
	hexagon-llvm-readelf -e $(TESTFILE).elf


dump:
	hexagon-llvm-objdump -d -r $(TESTFILE).o

sim:
	hexagon-sim -m$(Q6VERSION) $(TESTFILE).elf

clean:
	rm -fr *.o $(TESTFILE).elf stats.txt pmu_statsfile.txt *.map  *.so *.s

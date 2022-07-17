.PHONY: all

Q6VERSION ?= v68
HEXCORE ?= HEX_CPU_V68
CC_COSIM ?= g++


RELEASE_DIR:=$(shell which hexagon-sim)
RELEASE_DIR:=$(shell dirname ${RELEASE_DIR})/..
INCLUDE_DIR := $(RELEASE_DIR)/include/iss/
LIB_DIR:= $(RELEASE_DIR)/lib/iss/
TEST_DIR:= $(shell pwd)/bin
COSIM_CFG_DIR:= $(shell pwd)/cosim_src
TEST_NAME:= TxRxTest
COSIM_TX:= txCosimCfg
COSIM_RX:= rxCosimCfg
TIMING := HEX_NO_TIMING 
MULTISIM := 1

all: clean source SysSim.exe run
source:
	$(MAKE) -f cosim.mk -C cosim_src/ mini_all MULTISIM=1

SysSim.exe: SysSim.cpp
	${CC_COSIM} -g -Wall -DHEX_CORE=${HEXCORE} -DTIMING=${TIMING} -I${INCLUDE_DIR} -I./ $^ -o $@ -L${LIB_DIR} -lwrapper -ldl

clean:
	rm -rf build/ bin/ *.txt SysSim.exe
	$(MAKE) -f cosim.mk -C cosim_src/ clean

run:
	export LD_LIBRARY_PATH=${LIB_DIR}; \
	./SysSim.exe ${TEST_DIR}/${TEST_NAME} ${COSIM_CFG_DIR}/${COSIM_TX} ${COSIM_CFG_DIR}/${COSIM_RX}

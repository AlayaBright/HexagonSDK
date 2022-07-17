#################################################################
# Copyright (c) \$Date\$ QUALCOMM INCORPORATED.
# All Rights Reserved.
# Modified by QUALCOMM INCORPORATED on \$Date\$
##################################################################

# 
# Top level makefile
# 

target: all
INSTALLPREFIX := /usr/local/hexagon/qc
Q6VERSION ?= v68
PORTNUM1=8080
PORTNUM2=8081
MULTISIM:=0

CC = hexagon-clang
COSIMNAME = TxRxCosim
RUN = hexagon-sim
RELEASE_DIR:=$(shell which ${RUN})
RELEASE_DIR:=$(shell dirname ${RELEASE_DIR})/../..
INCLUDE_DIR := $(RELEASE_DIR)/Tools/include/iss/


SYSTEM := $(shell uname)
ARCH := $(shell uname -m)
SRC := $(shell pwd)
SRC_TOP := $(SRC)/..
EXEDIR := $(SRC_TOP)/bin
BUILDDIR := $(SRC_TOP)/build

# Compiler to use
# g++ must be g++-5.4.0 or later. Please set LD_LIBRARY_PATH to point to
# the g++ distribution lib dir (typically $(g++-5.4.0-or-later-install-dir)/lib64
CC_COSIM ?= g++

AR := ar

DLLIB = -ldl
O_EXT := o
O_OUTPUT := -o
CFLAGS_COSIM += -fPIC -Wno-write-strings -fomit-frame-pointer -ffast-math -std=c++11 -g

TEST_DIR := $(SRC_TOP)/hexagon_src/
OBJDIR := $(SRC_TOP)/build/
xOBJDIR := $(OBJDIR)
BM_DIR := $(SRC_TOP)/cosim_src

CFLAGS_COSIM += -I./ -Wall
MDCFLAGS := $(CFLAGS_COSIM)
DBG_CFLAGS := -g -DDEBUG
NDBG_CFLAGS := -O4
BINDIR := $(SRC_TOP)/bin
LIBSUFF := so

ifneq (Linux, $(findstring Linux,$(SYSTEM)))
	ARCH=
	DLLIB = 
	O_EXT := obj
	O_OUTPUT := /Fo

	CURR_DIR := $(shell pwd)
	TMP_CURR_DIR := $(shell cygpath -w ${CURR_DIR})
	
	SRC_DIR := $(shell echo "$(TMP_CURR_DIR)" | sed 's/\\/\//g')
	TMP_SRC_DIR := $(shell echo $(SRC_DIR) | sed 's,^/cygdrive/\(.\),\1:,g')
	SRC_TOP := $(TMP_SRC_DIR)/..
	EXEDIR := $(SRC_TOP)/bin
	BUILDDIR := $(SRC_TOP)/build
	# Compiler to use
	CC_COSIM := cl
	CFLAGS_COSIM := /nologo /Zm1000 /EHsc /D "WIN32" -DVCPP -DLITTLE_ENDIAN  /TP /I.
	LINKFLAGS := /nologo /dll libwrapper.lib
	LIBPATH := $(RELEASE_DIR)/Tools/lib/iss/
	DBG_CFLAGS := /MTd /Od /Gm /ZI
	NDBG_CFLAGS := /MT /O2
	MDCFLAGS := -DWIN32 -DVCPP -DLITTLE_ENDIAN -I.
	TEST_DIR := $(SRC_TOP)/hexagon_src/
	OBJDIR := $(SRC_TOP)/build/
	BM_DIR := $(SRC_TOP)/cosim_src
	AR := ar
	BINDIR := $(SRC_TOP)/bin/
	LIBSUFF := dll
endif

TEST := TxRxTest

LIBBM = $(BINDIR)/$(COSIMNAME).$(LIBSUFF)


ifeq (Linux, $(findstring Linux,$(SYSTEM)))
	CFLAGS_COSIM += -I$(INCLUDE_DIR) -I$(BM_DIR)
	MDCFLAGS += -I$(INCLUDE_DIR) -I$(BM_DIR)
else
	CFLAGS_COSIM += /I`cygpath -w $(INCLUDE_DIR)` /I$(BM_DIR)
	MDCFLAGS += -I$(INCLUDE_DIR) -I$(BM_DIR) 
endif


ALLCFILES = $(wildcard *.cpp)
DFILES = $(patsubst %,$(OBJDIR)/%,$(ALLCFILES:.cpp=.d))
-include $(DFILES)
OFILES = $(patsubst %,$(OBJDIR)/%,$(ALLCFILES:.cpp=.$(O_EXT)))

all: clean mini_all
	#make -C . OPT_CFLAGS="$(NDBG_CFLAGS)" mini_all
ifeq (Linux, $(findstring Linux,$(SYSTEM)))
	${RUN} --cosim_file bdCosimCfg --m$(Q6VERSION) ${BINDIR}/${TEST} -- BD
else
	${RUN} --cosim_file bdCygCosimCfg --m$(Q6VERSION) ${BINDIR}/${TEST} -- BD
endif

mini_all: makedirs $(LIBBM) app cosimFile

debug:
	make -C . OPT_CFLAGS="$(DBG_CFLAGS)" BINDIR=$(BINDIR)/debug OBJDIR=$(OBJDIR)/debug mini_all

cosimFile:
	sed i${BINDIR}/ ${TEST_DIR}/bdCosimCfg > newCfg;\
	sed '$!N;s/\n//' newCfg > bdCosimCfg;\
	rm -f newCfg;
	sed i${BINDIR}/ ${TEST_DIR}/txCosimCfg > newCfg;\
	sed '$!N;s/\n//' newCfg > txCosimCfg;\
	rm -f newCfg;
	sed i${BINDIR}/ ${TEST_DIR}/rxCosimCfg > newCfg;\
	sed '$!N;s/\n//' newCfg > rxCosimCfg;\
	rm -f newCfg;
ifneq (Linux, $(findstring Linux,$(SYSTEM)))
	sed 's/\.so/\.dll/' bdCosimCfg > bdCygCosimCfg;\
	rm -f bdCosimCfg;
	sed 's/\.so/\.dll/' txCosimCfg > txCygCosimCfg;\
	rm -f bdCosimCfg;
	sed 's/\.so/\.dll/' rxCosimCfg > rxCygCosimCfg;\
	rm -f bdCosimCfg;
endif

app:
	$(CC) -m${Q6VERSION} -o ${BINDIR}/${TEST} ${TEST_DIR}/${TEST}.c -DMULTISIM=${MULTISIM}

makedirs:
	mkdir -p $(BINDIR) $(OBJDIR);

$(OBJDIR)/%.$(O_EXT): %.cpp
ifeq (Linux, $(findstring Linux,$(SYSTEM)))
	$(CC_COSIM) $(OPT_CFLAGS) $(CFLAGS_COSIM) -g -c $< $(O_OUTPUT)$@
else
	"`cygpath -u "$(VCINSTALLDIR)\\bin\\$(CC_COSIM).exe"`" $(OPT_CFLAGS) $(CFLAGS_COSIM) -c $< $(O_OUTPUT)$@
endif

ifeq (Linux, $(findstring Linux,$(SYSTEM)))
$(LIBBM): $(OFILES)
	$(CC_COSIM) -shared  -o $@ $+
else
$(LIBBM): $(OFILES)
	"`cygpath -u "$(VCINSTALLDIR)\\Bin\\link.exe"`" `cygpath -w $(OFILES)` $(LINKFLAGS) /libpath:"`cygpath -w $(LIBPATH)`"  /out:$(LIBBM)
endif

install:  $(LIBBM)
	mkdir -p $(INSTALLPREFIX)/lib/iss/$(Q6VERSION)

clean:
	rm -rf $(EXEDIR) $(BUILDDIR) $(DFILES) newCfg *CosimCfg *CygCosimCfg pmu_statsfile.txt stats.txt


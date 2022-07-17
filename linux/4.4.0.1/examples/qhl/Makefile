ifeq ($(OS),Windows_NT)
SHELL:=$(ComSpec)
else
SHELL=/bin/bash
endif

QHL_DIR = $(HEXAGON_SDK_ROOT)/libs/qhl
ifeq ($(OS),Windows_NT)
        QHL_DIR_BIN = $(QHL_DIR)/hexagon_ReleaseG_$(DEFAULT_TOOLS_VARIANT)_$(V)
        SCRIPT_EXT = cmd
        SCRIPT_PATH =
else
        QHL_DIR_BIN = $(QHL_DIR)/hexagon_ReleaseG_$(DEFAULT_TOOLS_VARIANT)_$(V)
        SCRIPT_EXT = bash
        SCRIPT_PATH = bash
endif

QHL_INC_DIRS := $(QHL_DIR)/inc/qhmath $(QHL_DIR)/inc/qhcomplex $(QHL_DIR)/inc/qhdsp $(QHL_DIR)/inc/qhblas
QHL_LIBS = $(QHL_DIR_BIN)/libqhdsp.a $(QHL_DIR_BIN)/libqhcomplex.a $(QHL_DIR_BIN)/libqhmath.a $(QHL_DIR_BIN)/libqhblas.a

ifndef HEXAGON_TOOLS_ROOT
HEXAGON_TOOLS_ROOT = $(DEFAULT_HEXAGON_TOOLS_ROOT)
endif

CC = $(HEXAGON_TOOLS_ROOT)/Tools/bin/hexagon-clang

ifeq ($(V),v68)
CFLAGS = -m$(V) -DRUN_ON_V68 -fPIC -O2 -G0 -g -Wall -Werror -c -Wno-cast-align -Wpointer-arith -Wno-missing-braces -Wno-strict-aliasing -fno-exceptions -fno-strict-aliasing -fno-zero-initialized-in-bss -fdata-sections -D__V_DYNAMIC__ -Wstrict-prototypes -Wnested-externs
else
CFLAGS = -m$(V) -fPIC -O2 -G0 -g -Wall -Werror -c -Wno-cast-align -Wpointer-arith -Wno-missing-braces -Wno-strict-aliasing -fno-exceptions -fno-strict-aliasing -fno-zero-initialized-in-bss -fdata-sections -D__V_DYNAMIC__ -Wstrict-prototypes -Wnested-externs
endif
LDFLAGS = -shared -m$(V) -G0 -Wl
INCLUDES = $(addprefix -I,$(QHL_INC_DIRS))

all: qhl_libs qhl_example.so

qhl_libs:
	cd ${QHL_DIR} && $(SCRIPT_PATH) cmake_build.$(SCRIPT_EXT) hexagon DSP_ARCH=${V}
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<  -o $@

%.so: %.o
	$(CC) $(LDFLAGS) -o $@ $< $(QHL_LIBS)

clean:
	rm -rf *.o *.so
	cd ${QHL_DIR} && rm -rf ${QHL_DIR_BIN}

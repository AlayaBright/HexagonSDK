ifeq ($(OS),Windows_NT)
SHELL=com.exe
else
SHELL=/bin/bash
endif

ifeq ($(V),v68)
    Q6VERSION = v68
else ifeq ($(V),v66)
    Q6VERSION = v66
else
    Q6VERSION = v65
endif

OUTPUT_DIR = output

INCLUDE_ROOT_DIR = ../inc

ifeq ($(OS),Windows_NT)
	LIBS_BIN_DIR = ../hexagon_ReleaseG_$(DEFAULT_TOOLS_VARIANT)_$(Q6VERSION)
        SCRIPT_EXT= cmd
        SCRIPT_PATH=
else
	LIBS_BIN_DIR = ../hexagon_ReleaseG_$(DEFAULT_TOOLS_VARIANT)_$(Q6VERSION)
       SCRIPT_EXT= bash
       SCRIPT_PATH= bash
endif

# Note: default source folder, this is only for debug purpose
SRC_FF = qhmath


all : info qhl_libs
	@$(MAKE) --no-print-directory compile SRC_FF=qhmath
	@$(MAKE) --no-print-directory compile SRC_FF=qhblas
	@$(MAKE) --no-print-directory compile SRC_FF=qhcomplex
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp/internal TARGETS=$(INTERNAL_OBJS)
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp/common TARGETS=$(COMMON_OBJS)
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp/decimation
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp/interpolation
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp/filters
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp/fft


CC = ${DEFAULT_HEXAGON_TOOLS_ROOT}/Tools/bin/hexagon-clang
AR = ${DEFAULT_HEXAGON_TOOLS_ROOT}/Tools/bin/hexagon-ar
QHL_DIR=../

INC_DIRS = qhmath qhcomplex qhdsp qhblas internal
LIBS = qhdsp qhcomplex qhblas qhmath
LIBS_HVX = qhdsp_hvx qhblas_hvx qhmath_hvx

INCLUDES = $(patsubst %, -I$(INCLUDE_ROOT_DIR)/%,$(INC_DIRS))
STATIC_LIBS = $(patsubst %, $(LIBS_BIN_DIR)/lib%.a,$(LIBS))
INCLUDES += -Iinc -Icommon
INCLUDES += -I../src/internal
INCLUDES += -I../../../incs -I../../../incs/HAP
INCLUDES += -I../../../incs/stddef

CFLAGS_SIM = -m$(Q6VERSION) -fPIC -fno-builtin -O2 -G0 -c -Wall -Werror -DBENCH_QHL $(INCLUDES)
CFLAGS_DSP = -m$(Q6VERSION) -fPIC -fno-builtin -O2 -G0 -c -Wall -Werror -DBENCH_QHL -DRUN_ON_ADSP $(INCLUDES)
LDFLAGS_SIM = -m$(Q6VERSION) -G0
LDFLAGS_DSP = -shared -m$(Q6VERSION) -G0 -Wl,-Bsymbolic-functions

SRC_FILES = $(wildcard $(SRC_FF)/*.c)

COMMON_OBJS := $(patsubst %.c,$(OUTPUT_DIR)/sim/obj/%.o,$(wildcard qhdsp/common/*.c))
INTERNAL_OBJS := $(patsubst %.c,$(OUTPUT_DIR)/sim/obj/%.o,$(wildcard qhdsp/internal/*.c))

IS_DSP = $(findstring qhdsp, $(SRC_FF))
ifeq ($(IS_DSP),qhdsp)
	STATIC_LIBS := $(COMMON_OBJS) $(INTERNAL_OBJS) $(STATIC_LIBS)
endif

LIBS_SIM = $(STATIC_LIBS) -lhexagon
LIBS_DSP = $(STATIC_LIBS)

OBJS_SIM := $(patsubst %.c,$(OUTPUT_DIR)/sim/obj/%.o,$(SRC_FILES))
OBJS_DSP := $(patsubst %.c,$(OUTPUT_DIR)/dsp/obj/%.o,$(SRC_FILES))
TARGETS_SIM := $(patsubst %.c,$(OUTPUT_DIR)/sim/%_qhl,$(SRC_FILES))
TARGETS_DSP := $(patsubst %.c,$(OUTPUT_DIR)/dsp/%_qhl.so,$(SRC_FILES))
TARGETS := $(TARGETS_SIM) $(TARGETS_DSP)

$(OUTPUT_DIR)/sim/obj/$(SRC_FF)/%.o : $(SRC_FF)/%.c
	@$(CC) $(CFLAGS_SIM) $< -o $@

$(OUTPUT_DIR)/dsp/obj/$(SRC_FF)/%.o : $(SRC_FF)/%.c
	@$(CC) $(CFLAGS_DSP) $< -o $@

$(OUTPUT_DIR)/sim/$(SRC_FF)/%_qhl : $(OUTPUT_DIR)/sim/obj/$(SRC_FF)/%.o
	@$(CC) $(LDFLAGS_SIM) -o $@ $< $(LIBS_SIM)
	@echo [Simulator] Build done: $@

$(OUTPUT_DIR)/dsp/$(SRC_FF)/%_qhl.so : $(OUTPUT_DIR)/dsp/obj/$(SRC_FF)/%.o
	@$(CC) $(LDFLAGS_DSP) -o $@ $< $(LIBS_DSP)
	@echo [DSP] Build done: $@

compile : checkdirs $(TARGETS)
	@echo Target done: $(SRC_FF)
	@echo ------------------------------

# --- directories

# Workaraund for Windows : create parents directories
ADIRS = $(OUTPUT_DIR) \
		$(OUTPUT_DIR)/sim $(OUTPUT_DIR)/sim/$(SRC_FF) $(OUTPUT_DIR)/sim/obj $(OUTPUT_DIR)/sim/obj/$(SRC_FF) \
		$(OUTPUT_DIR)/dsp $(OUTPUT_DIR)/dsp/$(SRC_FF) $(OUTPUT_DIR)/dsp/obj $(OUTPUT_DIR)/dsp/obj/$(SRC_FF)

ifeq ($(OS),Windows_NT)
	# MKDIR = mkdir -p # work with gow
	MKDIR = mkdir
	DIRS = $(subst /,\,$(ADIRS))
else
	MKDIR = mkdir -p
	DIRS = $(ADIRS)
endif

$(DIRS):
	-@$(MKDIR) $@

checkdirs: $(DIRS)

clean :
	rm -rf $(OUTPUT_DIR)

info :
	@echo Compile tests and benchmarks for QHL on $(Q6VERSION) Hexagon architecture
qhl_libs:
	cd ${QHL_DIR} && ${SCRIPT_PATH} cmake_build.${SCRIPT_EXT} hexagon DSP_ARCH=${Q6VERSION}
# Makefile debugging trick:
# call print-VARIABLE to see the runtime value of any variable
print-% :
	@echo '$*="$($*)"'

ifeq ($(OS),Windows_NT)
SHELL=com.exe
else
SHELL=/bin/bash
endif

ifeq ($(V),v68)
    Q6VERSION = v68
else
    Q6VERSION = v66
endif

OUTPUT_DIR = output

INCLUDE_ROOT_DIR = ../inc

ifeq ($(OS),Windows_NT)
	LIBS_BIN_DIR = ../../qhl/hexagon_ReleaseG_$(DEFAULT_TOOLS_VARIANT)_$(Q6VERSION)
	LIBS_HVX_BIN_DIR = ../hexagon_ReleaseG_$(DEFAULT_TOOLS_VARIANT)_$(Q6VERSION)
        SCRIPT_EXT= cmd
        SCRIPT_PATH=
else
	LIBS_BIN_DIR = ../../qhl/hexagon_ReleaseG_$(DEFAULT_TOOLS_VARIANT)_$(Q6VERSION)
	LIBS_HVX_BIN_DIR = ../hexagon_ReleaseG_$(DEFAULT_TOOLS_VARIANT)_$(Q6VERSION)
        SCRIPT_EXT= bash
        SCRIPT_PATH= bash
endif

# Note: default source folder, this is only for debug purpose
SRC_FF = qhmath_hvx


all : info qhl_libs qhl_hvx_libs
	@$(MAKE) --no-print-directory compile SRC_FF=common TARGETS=$(COMMON_OBJS)
	@$(MAKE) --no-print-directory compile SRC_FF=qhmath_hvx
	@$(MAKE) --no-print-directory compile SRC_FF=qhblas_hvx
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp_hvx
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp_hvx/image_processing
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp_hvx/lms
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp_hvx/fft
	@$(MAKE) --no-print-directory compile SRC_FF=qhdsp_hvx/filters


CC = ${DEFAULT_HEXAGON_TOOLS_ROOT}/Tools/bin/hexagon-clang
AR = ${DEFAULT_HEXAGON_TOOLS_ROOT}/Tools/bin/hexagon-ar
QHL_DIR=../../qhl
QHL_HVX_DIR=../

INC_DIRS = qhmath qhcomplex qhdsp qhblas internal
INC_DIRS += qhdsp_hvx qhblas_hvx qhmath_hvx
LIBS = qhdsp qhcomplex qhblas qhmath
LIBS_HVX = qhdsp_hvx qhblas_hvx qhmath_hvx

INCLUDES = $(patsubst %, -I$(INCLUDE_ROOT_DIR)/%,$(INC_DIRS))
STATIC_LIBS = $(patsubst %, $(LIBS_HVX_BIN_DIR)/lib%.a,$(LIBS_HVX))
STATIC_LIBS += $(patsubst %, $(LIBS_BIN_DIR)/lib%.a,$(LIBS))
INCLUDES += -Iinc -Icommon
INCLUDES += -Iqhdsp_hvx/fft/test_data
INCLUDES += -I../../../incs -I../../../incs/HAP
INCLUDES += -I../../../incs/stddef
INCLUDES += -I../../qhl/inc/qhblas -I../../qhl/inc/qhmath -I../../qhl/inc/qhcomplex

CFLAGS_SIM =  -mhvx -mhvx-length=128B -m$(Q6VERSION) -fPIC -fno-builtin -O2 -G0 -c -Wall -Werror -DBENCH_QHL $(INCLUDES)
CFLAGS_DSP =  -mhvx -mhvx-length=128B -m$(Q6VERSION) -fPIC -fno-builtin -O2 -G0 -c -Wall -Werror -DBENCH_QHL -DRUN_ON_ADSP $(INCLUDES)
LDFLAGS_SIM = -m$(Q6VERSION) -G0
LDFLAGS_DSP = -shared -m$(Q6VERSION) -G0 -Wl,-Bsymbolic-functions

SRC_FILES = $(wildcard $(SRC_FF)/*.c)

COMMON_OBJS := $(patsubst %.c,$(OUTPUT_DIR)/sim/obj/%.o,$(wildcard common/*.c))

ifeq ($(SRC_FF),qhdsp_hvx/fft)
	STATIC_LIBS := $(COMMON_OBJS) $(STATIC_LIBS)
endif

ifeq ($(SRC_FF), qhmath_hvx)
	STATIC_LIBS := $(COMMON_OBJS) $(STATIC_LIBS)
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
	@echo Compile tests and benchmarks for HVX on $(Q6VERSION) Hexagon architecture
qhl_libs:
	cd ${QHL_DIR} && ${SCRIPT_PATH} cmake_build.${SCRIPT_EXT} hexagon DSP_ARCH=${Q6VERSION}
qhl_hvx_libs:
	cd ${QHL_HVX_DIR} && ${SCRIPT_PATH} cmake_build.${SCRIPT_EXT} hexagon DSP_ARCH=${Q6VERSION}
# Makefile debugging trick:
# call print-VARIABLE to see the runtime value of any variable
print-% :
	@echo '$*="$($*)"'

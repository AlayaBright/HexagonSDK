#
#
# gcc options- -Wall = enable all warnings
#            - -g add debug symbols
#            - -I include folder path
#            - -L library folder path
#            - -l link libnative.a and libmath
#            - -o output file name
#
# cl options - /MD = place MSVCRT.lib into .obj file,
#            - /IC = include following files,
#            - /link = invoke linker and add following file,
#            - /out = generate output file.

ifndef SDK_SETUP_ENV
	$(error Error! SDK Environment not set up -> please run setup_sdk_env script from SDK root directory.)
endif


TEST_NAME = qfe_test
LIB_NAME = qfe
EXE = $(TEST_NAME).exe
INCLUDE_DIR := $(RELEASE_DIR)/include/iss/
#QHMATH_HVX_DIR := src/qhmath_hvx_src/
QHMATH_HVX_DIR := $(HEXAGON_SDK_ROOT)/libs/qhl_hvx/src/qhmath_hvx/
QHMATH_HVX_SRC_FILES := $(wildcard $(QHMATH_HVX_DIR)/*.c)
QHMATH_HVX_OBJ_FILES := $(patsubst $(QHMATH_HVX_DIR)/%.c,bin/%.o,$(QHMATH_HVX_SRC_FILES))
SYSTEM := $(shell uname)

ifeq (Linux, $(findstring Linux,$(SYSTEM)))
	RELEASE_DIR := $(shell dirname `which hexagon-sim`)/..
	#RELEASE_DIR:=$(shell dirname ${RELEASE_DIR})/..
	ARCH := $(shell arch)
	NM = nm -A $(RELEASE_DIR)/libnative/lib/libnative.a > libnative-calls.txt
	CC_LIB := gcc

	CFLAGS_LIB += -Wall -Wno-unused-function -c -g -D__HVX_ARCH__=68 -D__HVXDBL__=1\
				  -L$(RELEASE_DIR)/libnative/lib -lnative -lm

	CFLAGS_TEST += -Wall -Wno-unused-function -c -o bin/$(TEST_NAME).o src/$(TEST_NAME).c -g \
				   -D__HVX_ARCH__=68 -D__HVXDBL__=1 \
				   -L$(RELEASE_DIR)/libnative/lib -lnative -lm

	CFLAGS_LINK += bin/$(TEST_NAME).o -o bin/$(TEST_NAME)  -l$(LIB_NAME) \
				   -D__HVX_ARCH__=68 -D__HVXDBL__=1 \
				   -Lbin/ -L$(RELEASE_DIR)/libnative/lib -lnative -lm

	INCLUDES += -I./inc/ -I$(RELEASE_DIR)/libnative/include
else
	NM = dumpbin /ALL /OUT:libnative-calls.txt $(RELEASE_DIR)/libnative/lib/libnative.a
endif

all: make_bin_dir run


run: build_test
ifeq (Linux, $(findstring Linux,$(SYSTEM)))
	./bin/$(TEST_NAME)
endif


build_test : link
	$(CC_LIB) $(INCLUDES) $(CFLAGS_TEST)
	$(CC_LIB) $(INCLUDES) $(CFLAGS_LINK)


link: build_qfe
	ar rcs bin/lib$(LIB_NAME).a bin/*.o


build_qfe: $(QHMATH_HVX_OBJ_FILES)
	$(CC_LIB) -o bin/$(LIB_NAME).o src/$(LIB_NAME).c $(INCLUDES) $(CFLAGS_LIB)


bin/%.o: $(QHMATH_HVX_DIR)/%.c
	$(CC_LIB) -o $@ $^ $(INCLUDES) $(CFLAGS_LIB)


#	$(CC_LIB) -o bin/qhmath_hvx_div_af.o $(QHMATH_HVX_DIR)/qhmath_hvx_div_af.c $(INCLUDES)  $(CFLAGS_LIB)


make_bin_dir:
	mkdir -p ./bin/


clean:
ifeq (Linux, $(findstring Linux,$(SYSTEM)))
	rm -r -f ./bin/ libnative-calls.txt
else
	./make.cmd clean
endif


list:
	$(NM)

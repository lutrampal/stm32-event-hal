
include $(PROJECT_ROOT_DIR)/env.mk

SRC_FILES += $(shell find $(PLATFORM_DIRS) -maxdepth 1 -type f -name *.$(CXX_EXT)) \
  $(ROOT_SRC_DIR)/event_loop.cpp  $(COMPONENT_SRC_DIR)/logger.cpp $(DEVICE_SRC_DIR)/error_status.cpp

CPPUTEST_HOME ?= /usr/arm-none-eabi

LD_LIBRARIES = -lCppUTest

CXX_host ?= g++

TEST_SRC_FILES += \
  $(UNITTEST_SRC_DIR)/AllTests.cpp

UNITTEST_EXTRA_INC_PATHS += \
  -I$(CPPUTEST_HOME)/include \
  -I$(UNITTEST_ROOT)/ \
  -I$(PROJECT_ROOT_DIR)

CPPUTEST_CPPFLAGS += $(UNITTEST_EXTRA_INC_PATHS) $(DEFINES) -DINSIDE_UNITTEST
CPPUTEST_LDFLAGS += $(LFLAGS) 

export CPPUTEST_USE_EXTENSIONS=N
export CPPUTEST_USE_MEM_LEAK_DETECTION=N
export CPPUTEST_USE_GCOV=N
export CPPUTEST_USE_LONG_LONG=Y

CC_VERSION_OUTPUT ="$(shell $(CXX_host) -v 2>&1)"
CLANG_STR = clang
ifeq ($(findstring $(CLANG_STR),$(CC_VERSION_OUTPUT)),$(CLANG_STR))
COMPILER_SPECIFIC_WARNINGS += \
  -Wno-c++11-extensions \
  -Wno-c11-extensions \
  -Wno-c99-extensions \
  -Wno-covered-switch-default \
  -Wno-documentation \
  -Wno-documentation-unknown-command \
  -Wno-flexible-array-extensions \
  -Wno-gnu-variable-sized-type-not-at-end \
  -Wno-keyword-macro \
  -Wno-reserved-id-macro \
  -Wno-shorten-64-to-32 \
  -Wno-vla-extension
endif

CPPUTEST_WARNINGFLAGS += $(COMPILER_SPECIFIC_WARNINGS)
export CPPUTEST_WARNINGFLAGS

UNITTEST_RESULT_DIR = $(UNITTEST_BUILD_DIR)/$(COMPONENT_NAME)

export TEST_TARGET = $(UNITTEST_RESULT_DIR)/$(COMPONENT_NAME)_tests
export CPPUTEST_OBJS_DIR = $(UNITTEST_RESULT_DIR)/objs
export CPPUTEST_LIB_DIR = $(UNITTEST_RESULT_DIR)/lib

# Enable color!
CPPUTEST_EXE_FLAGS ?= "-c"

# run MakefileWorker.mk with the variables defined here
include MakefileWorker.mk

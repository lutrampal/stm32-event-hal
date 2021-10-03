
BUILD_DIR ?= $(PROJECT_ROOT_DIR)/build
BUILD_TYPE ?= debug
TARGET ?= $(BUILD_DIR)/example
MCU ?= cortex-m7
BOARD ?= stm32f7508_dk

LD_DIR = $(PROJECT_ROOT_DIR)/ld
LD_SCRIPT ?= $(LD_DIR)/stm32f7508-dk.ld

ARM_GDB_SERVER_PORT ?= 4242

CXX ?= arm-none-eabi-g++
OC ?= arm-none-eabi-objcopy
OS ?= arm-none-eabi-size
GDB ?= arm-none-eabi-gdb

WFLAGS ?= -Wall -Wpedantic -Wextra -Wno-unused-parameter
ARMFLAGS = -mcpu=$(MCU) -mthumb -mhard-float -mfloat-abi=hard -mfpu=fpv5-sp-d16
CXXFLAGS = -c -fexceptions -ffunction-sections -fdata-sections -std=c++17 \
	-specs=nosys.specs $(ARMFLAGS) $(WFLAGS)
LFLAGS = -T $(LD_SCRIPT) -Wl,--gc-sections -Wl,-L$(LD_DIR) \
	-specs=nosys.specs $(ARMFLAGS) $(WFLAGS) 

INCLUDES = -I$(PROJECT_ROOT_DIR)/include -I$(PROJECT_ROOT_DIR)/src/
ROOT_SRC_DIR = $(PROJECT_ROOT_DIR)/src
DEVICE_SRC_DIR = $(ROOT_SRC_DIR)/device
DRIVER_SRC_DIR = $(ROOT_SRC_DIR)/driver
COMPONENT_SRC_DIR = $(ROOT_SRC_DIR)/component
HARDWARE_SRC_DIR = $(ROOT_SRC_DIR)/hardware

ALL_SRC_DIRS = $(ROOT_SRC_DIR) $(DEVICE_SRC_DIR) $(DRIVER_SRC_DIR) $(COMPONENT_SRC_DIR) $(HARDWARE_SRC_DIR)
ALL_BUILD_DIRS = $(subst $(ROOT_SRC_DIR), $(BUILD_DIR), $(ALL_SRC_DIRS))
CXX_EXT = cpp
DEFINES ?= -DLOG_LEVEL=LOG_LEVEL_$(LOG_LEVEL)

ifeq ($(BUILD_TYPE),debug)
	CXXFLAGS += -O0 -g3
	DEFINES += -DDEBUG
else
	CXXFLAGS += -Os
endif

ifeq ($(BOARD),stm32f7508_dk)
	DEFINES += -DMCU_STM32F750 -DBOARD_STM32F7508_DK
	PLATFORM_DIRS = $(ROOT_SRC_DIR)/device/stm32f750 $(ROOT_SRC_DIR)/device/stm32f7508-dk
# Add here any other board
endif

ALL_SRC_DIRS += $(PLATFORM_DIRS)

CXX_SRC = $(shell find $(ALL_SRC_DIRS) -maxdepth 1 -type f -name *.$(CXX_EXT))
OBJS = $(patsubst $(ROOT_SRC_DIR)/%,$(BUILD_DIR)/%,$(CXX_SRC:.$(CXX_EXT)=.o))

DEPENDS := $(patsubst $(ROOT_SRC_DIR)/%,$(BUILD_DIR)/%,$(CXX_SRC:.$(CXX_EXT)=.d))

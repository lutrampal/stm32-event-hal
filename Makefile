
BUILD_DIR ?= ./build
BUILD_TYPE ?= debug
TARGET ?= $(BUILD_DIR)/example
MCU ?= cortex-m7
BOARD ?= stm32f7508_dk

LD_SCRIPT ?= ./ld/stm32f7508-dk.ld

ARM_GDB_SERVER_PORT ?= 4242

CXX ?= arm-none-eabi-g++
OC ?= arm-none-eabi-objcopy
OS ?= arm-none-eabi-size
GDB ?= arm-none-eabi-gdb

WFLAGS ?= -Wall -Wpedantic -Wextra -Wno-unused-parameter
ARMFLAGS = -mcpu=$(MCU) -mthumb -mhard-float -mfloat-abi=hard -mfpu=fpv5-sp-d16
CXXFLAGS = -c -fexceptions -ffunction-sections -fdata-sections -std=c++17 \
	-specs=nosys.specs $(ARMFLAGS) $(WFLAGS)
LFLAGS = -T $(LD_SCRIPT) -Wl,--gc-sections -Wl,-L./ld \
	-specs=nosys.specs $(ARMFLAGS) $(WFLAGS) 

INCLUDES = -I./include -I./src/
SRC_DIR = ./src
ALL_SRC_DIRS = $(SRC_DIR) ./src/device ./src/driver ./src/hardware ./src/component
ALL_BUILD_DIRS = $(subst $(SRC_DIR), $(BUILD_DIR), $(ALL_SRC_DIRS))
CXX_EXT = cpp
DEFINES ?=

ifeq ($(BUILD_TYPE),debug)
	CXXFLAGS += -O0 -g3
	DEFINES += -DDEBUG
else
	CXXFLAGS += -Os
endif

ifeq ($(BOARD),stm32f7508_dk)
	DEFINES += -DMCU_STM32F750 -DBOARD_STM32F7508_DK
	ALL_SRC_DIRS += ./src/device/stm32f750 ./src/device/stm32f7508-dk
# Add here any other board
endif


CXX_SRC = $(shell find $(ALL_SRC_DIRS) -maxdepth 1 -type f -name *.$(CXX_EXT))
OBJS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(CXX_SRC:.$(CXX_EXT)=.o))

DEPENDS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(CXX_SRC:.$(CXX_EXT)=.d))
-include $(DEPENDS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(CXX_EXT) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -MMD -MP -o $@ $<

$(TARGET).elf: $(OBJS)
	$(CXX) $^ $(LFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OC) -S -O binary $< $@
	$(OS) $<

$(BUILD_DIR):
	mkdir -p $(ALL_BUILD_DIRS)


.PHONY: all flash-n-debug clean

all: $(TARGET).bin

flash-n-debug: all
	$(GDB) $(TARGET).elf -ex 'target extended-remote :$(ARM_GDB_SERVER_PORT)' -ex load

clean:
	rm -rf $(BUILD_DIR)


export PROJECT_ROOT_DIR := $(abspath .)

include env.mk

-include $(DEPENDS)

$(BUILD_DIR)/%.o: $(ROOT_SRC_DIR)/%.$(CXX_EXT) | $(BUILD_DIR)
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

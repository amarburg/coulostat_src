PROJECT_OBJS = 
	
CFLAGS += -I. -Ifs -DUSER_PROVIDES_SYSTICK_HANDLER

# I believe this overrides a maple-provided rule
# 
$(BUILD_PATH)/%.o: %.c
	@mkdir -p $(dir $(@:%.o=%.d))
	$(SILENT_CC) $(CC) $(CFLAGS) $(LIBMAPLE_INCLUDES) $(WIRISH_INCLUDES)  -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $< 
	
$(BUILD_PATH)/%.o: %.cpp
	@mkdir -p $(dir $(@:%.o=%.d))
	$(SILENT_CXX) $(CXX) $(CFLAGS) $(CXXFLAGS) $(LIBMAPLE_INCLUDES) $(WIRISH_INCLUDES)  -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $< 

# main project target
$(BUILD_PATH)/main.o: main.cpp 
	$(SILENT_CXX) $(CXX) $(CFLAGS) $(CXXFLAGS) $(LIBMAPLE_INCLUDES) $(WIRISH_INCLUDES) -o $@ -c $< 

$(BUILD_PATH)/libmaple.a: $(BUILDDIRS) $(TGT_BIN)
	- rm -f $@
	$(AR) crv $(BUILD_PATH)/libmaple.a $(TGT_BIN)

library: $(BUILD_PATH)/libmaple.a

.PHONY: library

$(BUILD_PATH)/$(BOARD).elf: $(BUILDDIRS) $(TGT_BIN) $(BUILD_PATH)/main.o $(PROJECT_OBJS) 
	$(SILENT_LD) $(CXX) $(LDFLAGS) -o $@ $(TGT_BIN) $(PROJECT_OBJS) $(BUILD_PATH)/main.o 

$(BUILD_PATH)/$(BOARD).bin: $(BUILD_PATH)/$(BOARD).elf
	$(SILENT_OBJCOPY) $(OBJCOPY) -v -Obinary $(BUILD_PATH)/$(BOARD).elf $@ 1>/dev/null
	$(SILENT_DISAS) $(DISAS) -d $(BUILD_PATH)/$(BOARD).elf > $(BUILD_PATH)/$(BOARD).disas
	@echo " "
	@echo "Object file sizes:"
	@find $(BUILD_PATH) -iname *.o | xargs $(SIZE) -t > $(BUILD_PATH)/$(BOARD).sizes
	@cat $(BUILD_PATH)/$(BOARD).sizes
	@echo " "
	@echo "Final Size:"
	@$(SIZE) $<
	@echo $(MEMORY_TARGET) > $(BUILD_PATH)/build-type

$(BUILDDIRS):
	@mkdir -p $@

MSG_INFO:
	@echo "================================================================================"
	@echo ""
	@echo "  Build info:"
	@echo "     BOARD:          " $(BOARD)
	@echo "     MCU:            " $(MCU)
	@echo "     MEMORY_TARGET:  " $(MEMORY_TARGET)
	@echo ""
	@echo "  See 'make help' for all possible targets"
	@echo ""
	@echo "================================================================================"
	@echo


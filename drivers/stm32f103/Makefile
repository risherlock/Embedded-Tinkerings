TARGET = main
BUILD_DIR = build
DEBUG = 0

# CPU flags
FPU =
FLOAT-ABI =
CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
OPT = -Og

# C sources
C_SOURCES =  \
core/startup/crt.c \
core/startup/vectors_stm32f10x_md.c \
core/cmsis/core_support/core_cm3.c \
core/cmsis/device_support/system_stm32f10x.c \
libs/delay/delay.c \
libs/uart/usart.c \
libs/util/util.c

# CPU sources
CPP_SOURCES = \
libs/ina219/ina219.cpp \
main.cpp

# C/CPP includes
C_INCLUDES =  \
-Icore/cmsis/device_support \
-Icore/cmsis/core_support \
-Ilibs/uart \
-Ilibs/util \
-Ilibs/delay \
-Ilibs/ina219

# ASM sources
ASM_SOURCES =  \

# Macros for gcc
AS_DEFS =
C_DEFS = -DSTM32F10X_MD

# Includes
AS_INCLUDES =

# Link script
LDSCRIPT = core/linker/stm32f103xb_flash.ld

# Libraries
LIBS = -lc -lm -lnosys
LIBDIR =

## Stop Edit ##

# Compilers
GCC_PATH =
PREFIX = arm-none-eabi-
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
CX = $(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
CX = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
CPPFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions -std=c++11 -U__STRICT_ANSI__

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Link flags
LDFLAGS = $(MCU) -nostartfiles -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -u _printf_float,--no-warn-mismatch 

# Default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# List of C objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o))) 
vpath %.c $(sort $(dir $(C_SOURCES)))

# List of CPP objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

# List of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	$(CX) -c $(CPPFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CX) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@ #

# Flash the hex
flash: all
	openocd -f interface/stlink.cfg -f target/stm32f1x_clone.cfg -c "program $(BUILD_DIR)/$(TARGET).hex verify reset exit"

# Delete build directory
clean:
	rm -r -d $(BUILD_DIR)

# Dependencies
-include $(wildcard $(BUILD_DIR)/*.d)

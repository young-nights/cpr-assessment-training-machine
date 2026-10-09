# ============================================================
# CPR Head Board - STM32F103C6T6 Makefile
# Bare-metal, arm-none-eabi-gcc
#
# Usage:
#   make          - build .elf and .bin
#   make flash    - flash via OpenOCD (ST-Link)
#   make clean    - remove build artifacts
# ============================================================

# Toolchain
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

# Target
TARGET  = cpr-head-board

# Directories
BUILD   = build
SRCDIR  = Src
INCDIR  = Inc
DRVDIR  = Drivers
LDDIR   = Linker

# Sources
C_SRCS  = $(wildcard $(SRCDIR)/*.c)
ASM_SRCS = $(DRVDIR)/startup_stm32f103c6.s

# Includes
INCS    = -I$(INCDIR) -I$(DRVDIR)

# Linker script
LDSCRIPT = $(LDDIR)/stm32f103c6_flash.ld

# Compiler flags
CPU     = -mcpu=cortex-m3 -mthumb
CFLAGS  = $(CPU) $(INCS) -Wall -Wextra -O2 -g \
          -ffunction-sections -fdata-sections \
          -DSTM32F103x6
ASFLAGS = $(CPU) -x assembler-with-cpp

# Linker flags
LDFLAGS = $(CPU) -T$(LDSCRIPT) \
          -Wl,--gc-sections \
          -Wl,-Map=$(BUILD)/$(TARGET).map \
          --specs=nano.specs \
          --specs=nosys.specs

# Object files
OBJS    = $(C_SRCS:%.c=$(BUILD)/%.o) $(ASM_SRCS:%.s=$(BUILD)/%.o)

# ============================================================
# Rules
# ============================================================

.PHONY: all clean flash size

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin size

# Link
$(BUILD)/$(TARGET).elf: $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Binary
$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Size report
size: $(BUILD)/$(TARGET).elf
	$(SIZE) $<

# Compile C
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble
$(BUILD)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# Flash via OpenOCD (ST-Link)
flash: $(BUILD)/$(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program $(BUILD)/$(TARGET).bin 0x08000000 verify reset exit"

# Clean
clean:
	rm -rf $(BUILD)

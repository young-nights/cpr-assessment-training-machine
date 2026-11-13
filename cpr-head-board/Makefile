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

# Directories (anchored to this Makefile: build outputs always stay in
# this board folder regardless of the directory make is invoked from)
BOARD_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILD   = $(BOARD_DIR)build
SRCDIR  = $(BOARD_DIR)Src
INCDIR  = $(BOARD_DIR)Inc
DRVDIR  = $(BOARD_DIR)Drivers
LDDIR   = $(BOARD_DIR)Linker

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
OBJS    = $(patsubst $(SRCDIR)/%.c,$(BUILD)/Src/%.o,$(C_SRCS)) \
          $(patsubst $(DRVDIR)/%.s,$(BUILD)/Drivers/%.o,$(ASM_SRCS))

# Shell compatibility: cmd builtins on Windows, POSIX elsewhere, so the
# Makefile works from cmd / PowerShell / VSCode tasks / WSL alike.
ifeq ($(OS),Windows_NT)
SHELL       := cmd.exe
.SHELLFLAGS := /c
mkdir_p = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
rm_rf   = if exist $(subst /,\,$(1)) rmdir /s /q $(subst /,\,$(1))
else
mkdir_p = mkdir -p $(1)
rm_rf   = rm -rf $(1)
endif

# ============================================================
# Rules
# ============================================================

.PHONY: all clean flash size

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin size

# Link
$(BUILD)/$(TARGET).elf: $(OBJS)
	@$(call mkdir_p,$(dir $@))
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Binary
$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Size report
size: $(BUILD)/$(TARGET).elf
	$(SIZE) $<

# Compile C
$(BUILD)/Src/%.o: $(SRCDIR)/%.c
	@$(call mkdir_p,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble
$(BUILD)/Drivers/%.o: $(DRVDIR)/%.s
	@$(call mkdir_p,$(dir $@))
	$(AS) $(ASFLAGS) -c $< -o $@

# Flash via OpenOCD (ST-Link)
flash: $(BUILD)/$(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program $(BUILD)/$(TARGET).bin 0x08000000 verify reset exit"

# Clean
clean:
	$(call rm_rf,$(BUILD))

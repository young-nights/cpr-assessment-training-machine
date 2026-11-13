# ============================================================
# CPR Raster Board - STM32F103C6T6 Makefile
# Bare-metal, arm-none-eabi-gcc
#
# Usage:
#   make          - build .elf and .bin
#   make flash    - flash via OpenOCD (ST-Link)
#   make clean    - remove build artifacts
# ============================================================

# ---- 工具链 ----
# 全部使用 ARM 裸机交叉工具链，命名与 CMake 版一致
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-gcc      # 汇编也用 gcc 驱动，便于处理 .s 中的 C 预处理
OBJCOPY = arm-none-eabi-objcopy  # .elf -> .bin
SIZE    = arm-none-eabi-size     # 打印段大小

# ---- 目标名 ----
TARGET  = cpr-raster-board

# ---- 目录 ----
# 关键技巧：用 $(lastword $(MAKEFILE_LIST)) 取当前 Makefile 路径，
# abspath 转绝对路径，dir 取目录（带结尾斜杠），
# 这样无论从哪个目录调用 make，输出都固定落在本板卡的 build/ 下
BOARD_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILD   = $(BOARD_DIR)build
SRCDIR  = $(BOARD_DIR)Src
INCDIR  = $(BOARD_DIR)Inc
DRVDIR  = $(BOARD_DIR)Drivers
LDDIR   = $(BOARD_DIR)Linker

# ---- 源文件 ----
# C 源文件用 wildcard 自动收集 Src/ 下所有 .c（新增文件不用改 Makefile）
C_SRCS  = $(wildcard $(SRCDIR)/*.c)
# 汇编启动文件写死一个
ASM_SRCS = $(DRVDIR)/startup_stm32f103c6.s

# ---- 头文件搜索路径 ----
INCS    = -I$(INCDIR) -I$(DRVDIR)

# ---- 链接脚本 ----
LDSCRIPT = $(LDDIR)/stm32f103c6_flash.ld

# ---- 编译选项 ----
# CPU：Cortex-M3 + Thumb 指令集
CPU     = -mcpu=cortex-m3 -mthumb
# CFLAGS 与 CMake 版 target_compile_options 基本一一对应：
#   -Wall -Wextra 警告；-O2 -g 优化+调试；
#   -ffunction-sections -fdata-sections 配合 --gc-sections 裁剪；
#   -DSTM32F103x6 等价于 CMake 的 target_compile_definitions
CFLAGS  = $(CPU) $(INCS) -Wall -Wextra -O2 -g \
          -ffunction-sections -fdata-sections \
          -DSTM32F103x6
# 汇编选项：-x assembler-with-cpp 让 .s 先过 C 预处理器（支持 #include/#define）
ASFLAGS = $(CPU) -x assembler-with-cpp

# ---- 链接选项 ----
# 与 CMake 版 target_link_options 对应：
#   -T 链接脚本；--gc-sections 裁剪；-Map 生成 map；
#   nano.specs 精简 C 库；nosys.specs 提供空系统调用
LDFLAGS = $(CPU) -T$(LDSCRIPT) \
          -Wl,--gc-sections \
          -Wl,-Map=$(BUILD)/$(TARGET).map \
          --specs=nano.specs \
          --specs=nosys.specs

# ---- 目标文件列表 ----
# patsubst 做“路径+后缀”映射：
#   Src/xxx.c      -> build/Src/xxx.o
#   Drivers/xxx.s  -> build/Drivers/xxx.o
# 这样源码目录和输出目录分离，源码树保持干净
OBJS    = $(patsubst $(SRCDIR)/%.c,$(BUILD)/Src/%.o,$(C_SRCS)) \
          $(patsubst $(DRVDIR)/%.s,$(BUILD)/Drivers/%.o,$(ASM_SRCS))

# ---- 跨平台 shell 兼容 ----
# Windows_NT 是 Windows 上 make 常见的内置环境变量。
# Windows 下用 cmd.exe 并自定义 mkdir/rm 命令（cmd 没有 mkdir -p / rm -rf）；
# 其他平台用 POSIX 命令。这样同一份 Makefile 在 cmd/PowerShell/VSCode/WSL 都能跑。
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

# 声明伪目标，避免与同名文件冲突
.PHONY: all clean flash size

# 默认目标：先链接出 .elf，再生成 .bin，最后打印 size
all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin size

# ---- 链接 ----
# 依赖所有 .o，先确保输出目录存在，再调用 gcc 链接
$(BUILD)/$(TARGET).elf: $(OBJS)
	@$(call mkdir_p,$(dir $@))
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# ---- 生成 bin ----
# 对应 CMake 版 POST_BUILD 里的 objcopy 步骤
$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# ---- 打印大小 ----
# 对应 CMake 版 POST_BUILD 里的 size 步骤
size: $(BUILD)/$(TARGET).elf
	$(SIZE) $<

# ---- 编译 C ----
# 模式规则：Src/%.c -> build/Src/%.o
$(BUILD)/Src/%.o: $(SRCDIR)/%.c
	@$(call mkdir_p,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

# ---- 汇编 ----
# 模式规则：Drivers/%.s -> build/Drivers/%.o
$(BUILD)/Drivers/%.o: $(DRVDIR)/%.s
	@$(call mkdir_p,$(dir $@))
	$(CC) $(ASFLAGS) -c $< -o $@

# ---- 烧录 ----
# 对应 CMake 版 add_custom_target(flash)：
# 依赖 .bin，保证先构建再烧录；OpenOCD + ST-Link，烧到 0x08000000
flash: $(BUILD)/$(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program $(BUILD)/$(TARGET).bin 0x08000000 verify reset exit"

# ---- 清理 ----
# 对应 CMake 版没有显式 clean（CMake 用 cmake --build build --target clean）
clean:
	$(call rm_rf,$(BUILD))
# cpr-raster-board - STM32F103C6T6 bare-metal firmware (CMake)
#
# Build:
#   cmake -S . -B build -G Ninja
#   cmake --build build
#   cmake --build build --target flash    # OpenOCD + ST-Link
#
# Output stays in this board's build/ directory (see .vscode/settings.json).

# 要求 CMake 最低版本 3.16，低于此版本直接报错
cmake_minimum_required(VERSION 3.16)

# ---- 交叉编译环境声明（必须在 project() 之前设置）----
# 告诉 CMake 这是“裸机”目标，不是本机 Linux/Windows，避免它去探测本机编译器能力
set(CMAKE_SYSTEM_NAME Generic)
# 目标 CPU 架构：Cortex-M3（STM32F103 的内核）
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)
# 交叉编译时 CMake 默认会尝试链接一个可执行文件来测试编译器，
# 但裸机没有标准运行时/启动文件，链接会失败；改成只编译静态库来探测，绕过链接测试
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 指定 C 和汇编编译器为 arm-none-eabi-gcc（ARM 裸机工具链）
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
# objcopy：把 .elf 转成 .bin（烧录用）；size：打印各段大小
set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE FILEPATH "")
set(CMAKE_SIZE arm-none-eabi-size CACHE FILEPATH "")

# 定义工程名，并声明使用 C 和 ASM 两种语言（ASM 用于 startup 汇编文件）
project(cpr-raster-board C ASM)

# ---- 定义可执行目标 .elf，并列出所有源文件 ----
# Src/ 下是应用代码（main、GPIO、任务、两个 UART、时钟），
# Drivers/startup_stm32f103c6.s 是启动汇编（设置栈、向量表、跳 main）
add_executable(${PROJECT_NAME}.elf
    Src/main.c
    Src/raster_gpio.c
    Src/raster_task.c
    Src/raster_uart1.c
    Src/raster_uart2.c
    Src/syscalls.c
    Src/sys_clock.c
    Drivers/startup_stm32f103c6.s
)

# 头文件搜索路径：Inc/ 放项目头文件，Drivers/ 放启动文件/外设相关头文件
target_include_directories(${PROJECT_NAME}.elf PRIVATE Inc Drivers)
# 编译宏 STM32F103x6：让 CMSIS/外设头文件选中正确的芯片型号定义
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE STM32F103x6)
# 编译选项：
#   -mcpu=cortex-m3 -mthumb  指定内核与 Thumb 指令集
#   -Wall -Wextra            打开警告，帮助发现裸机代码里的隐患
#   -O2 -g                   优化级别 + 调试信息
#   -ffunction-sections -fdata-sections  每个函数/数据单独成段，配合链接器 --gc-sections 裁剪未用代码
target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m3 -mthumb -Wall -Wextra -O2 -g
    -ffunction-sections -fdata-sections)

# 链接选项：
#   -mcpu/-mthumb            链接阶段同样需要内核参数
#   -T...flash.ld            指定链接脚本（定义 Flash/RAM 布局、段放置）
#   --gc-sections            删除未被引用的段，减小固件体积
#   -Map=...                 生成 map 文件，便于分析内存占用
#   --specs=nano.specs       使用 newlib-nano 精简 C 库
#   --specs=nosys.specs      提供空实现的系统调用（裸机没有 OS，避免链接报错）
#                            （_write/_read/_close/_lseek 已由 Src/syscalls.c 接管）
#   --no-warn-rwx-segments   静默 RWX 段告警（.data 镜像在 flash 段，Cortex-M 无害）
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m3 -mthumb
    -T${CMAKE_CURRENT_SOURCE_DIR}/Linker/stm32f103c6_flash.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
    -Wl,--no-warn-rwx-segments
    --specs=nano.specs --specs=nosys.specs)

# 构建 .elf 之后自动执行的后续步骤（POST_BUILD）：
#   1) objcopy 把 .elf 转成纯二进制 .bin，用于烧录到 0x08000000
#   2) size 打印 text/data/bss 占用，方便检查是否超出 Flash/RAM
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:${PROJECT_NAME}.elf>
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating ${PROJECT_NAME}.bin")

# 自定义目标 flash：用 OpenOCD + ST-Link 把 .bin 烧到 Flash 起始地址 0x08000000，
# 烧完校验并复位运行。用法：cmake --build build --target flash
add_custom_target(flash
    COMMAND openocd -f interface/stlink.cfg -f target/stm32f1x.cfg
            -c "program ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin 0x08000000 verify reset exit"
    COMMENT "Flashing ${PROJECT_NAME}.bin via OpenOCD (ST-Link)")
# 保证 flash 前先构建出 .elf/.bin，避免烧到旧文件
add_dependencies(flash ${PROJECT_NAME}.elf)
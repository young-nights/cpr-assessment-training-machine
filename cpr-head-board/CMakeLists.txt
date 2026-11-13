# cpr-head-board - STM32F103C6T6 bare-metal firmware (CMake)
#
# Build:
#   cmake -S . -B build -G Ninja
#   cmake --build build
#   cmake --build build --target flash    # OpenOCD + ST-Link
#
# Output stays in this board's build/ directory (see .vscode/settings.json).

cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE FILEPATH "")
set(CMAKE_SIZE arm-none-eabi-size CACHE FILEPATH "")

project(cpr-head-board C ASM)

add_executable(${PROJECT_NAME}.elf
    Src/main.c
    Src/head_gpio.c
    Src/head_oled.c
    Src/head_uart.c
    Src/head_ws2812b.c
    Src/sys_clock.c
    Drivers/startup_stm32f103c6.s
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE Inc Drivers)
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE STM32F103x6)
target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m3 -mthumb -Wall -Wextra -O2 -g
    -ffunction-sections -fdata-sections)
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m3 -mthumb
    -T${CMAKE_CURRENT_SOURCE_DIR}/Linker/stm32f103c6_flash.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
    --specs=nano.specs --specs=nosys.specs)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:${PROJECT_NAME}.elf>
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating ${PROJECT_NAME}.bin")

add_custom_target(flash
    COMMAND openocd -f interface/stlink.cfg -f target/stm32f1x.cfg
            -c "program ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin 0x08000000 verify reset exit"
    COMMENT "Flashing ${PROJECT_NAME}.bin via OpenOCD (ST-Link)")
add_dependencies(flash ${PROJECT_NAME}.elf)

cmake_minimum_required(VERSION 3.22)

include(pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()



project(pico_memory)

pico_sdk_init()

# Run from RAM only. Flash is too slow for heavy overclocking.
set(PICO_DEFAULT_BINARY_TYPE copy_to_ram)

add_executable(pico_memory)

target_sources(pico_memory PRIVATE rom.c commands.c rambuf.c)



# Enable USB output, disable UART output.
pico_enable_stdio_usb(pico_memory 1)
pico_enable_stdio_uart(pico_memory 0)


target_link_libraries(pico_memory PRIVATE pico_stdlib pico_multicore hardware_flash hardware_sync pico_time)
pico_add_extra_outputs(pico_memory)

# Add url via pico_set_program_url.
#example_auto_set_url(pico_memory)

# Slow down flash during boot for heavy overclocking.
#pico_define_boot_stage2(slower_boot2 ${PICO_DEFAULT_BOOT_STAGE2_FILE})
#target_compile_definitions(slower_boot2 PRIVATE PICO_FLASH_SPI_CLKDIV=4)
#pico_set_boot_stage2(pico_memory slower_boot2)

# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "C:/Users/j_gab/Documents/Pico/pico-sdk")

#Intialise freeRTOS-Kernel from installed location
set(FREERTOS_KERNEL_PATH "C:/Users/j_gab/Documents/Pico/FreeRTOS-Kernel")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

#Pull in FreeRTOS-Kernel
include(FreeRTOS_Kernel_import.cmake)

project(poleFirmware C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adding subdirectories
add_subdirectory(lightManager)
add_subdirectory(supplyManager)

# Add executable. Default name is the project name, version 0.1
if(TARGET tinyusb_device)
    add_executable(poleFirmware poleFirmware.cpp )
    target_include_directories(poleFirmware PUBLIC ./)

    pico_set_program_name(poleFirmware "poleFirmware")
    pico_set_program_version(poleFirmware "0.1")

    pico_enable_stdio_uart(poleFirmware 0)
    pico_enable_stdio_usb(poleFirmware 1)

    # Add the standard library to the build
    target_link_libraries(poleFirmware 
        pico_stdlib
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap4
        pico_time
        hardware_pwm
        lightManager
        supplyManager
    )

    pico_add_extra_outputs(poleFirmware)

    elseif(PICO_ON_DEVICE)
        message(WARNING "Not building hello_usb because TinyUSB submodule is not initialized in the SDK")
    endif()


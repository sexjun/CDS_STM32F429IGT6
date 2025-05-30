cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME yehuo_STM32F429IGT6_base_preject)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
      ${CMAKE_SOURCE_DIR}/Core/Src/cds_gpio_exit.c
      ${CMAKE_SOURCE_DIR}/Core/Src/bsp_ds18b20.c
      ${CMAKE_SOURCE_DIR}/Core/Src/core_delay_dwt.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Add compile options
# Note: The -u _printf_float option is used to enable floating-point support in printf/scanf functions.
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -u _printf_float
)


# 打印编译器信息
message(STATUS "===== 编译器信息 =====")
message(STATUS "CMake版本: ${CMAKE_VERSION}")
message(STATUS "C编译器: ${CMAKE_C_COMPILER}")
message(STATUS "C++编译器: ${CMAKE_CXX_COMPILER}")
message(STATUS "编译器标识: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "编译器版本: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMake可执行文件路径: ${CMAKE_CMAKE_EXECUTABLE}")

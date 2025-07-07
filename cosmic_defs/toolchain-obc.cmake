# This example toolchain file describes the cross compiler to use for
# the target architecture indicated in the configuration file.

# In this sample application, the "cross" toolchain is configured to
# simply use the system native compiler with the "m32" switch to output
# 32-bit code on a 64-bit system.  This will not be necessary in 
# future revisions.

# Basic cross system configuration
SET(CMAKE_SYSTEM_NAME           Linux)
SET(CMAKE_SYSTEM_VERSION        1)
SET(CMAKE_SYSTEM_PROCESSOR      arm)

# Specify the cross compiler executables
# Typically these would be installed in a home directory or somewhere
# in /opt.  However in this example the system compiler is used.
SET(CMAKE_C_COMPILER            "${CMAKE_CURRENT_SOURCE_DIR}/../submodules/compiler/bin/arm-buildroot-linux-gnueabi-gcc")
SET(CMAKE_CXX_COMPILER          "${CMAKE_CURRENT_SOURCE_DIR}/../submodules/compiler/bin/arm-buildroot-linux-gnueabi-g++")

# Configure the find commands
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM   NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY   NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE   NEVER)

# These variable settings are specific to cFE/OSAL and determines which 
# abstraction layers are built when using this toolchain
SET(CFE_SYSTEM_PSPNAME      "pc-linux")
SET(OSAL_SYSTEM_BSPNAME     "generic-linux")
SET(OSAL_SYSTEM_OSTYPE      "posix")

add_definitions(-DCOSMIC_PLATFORM_OBC)


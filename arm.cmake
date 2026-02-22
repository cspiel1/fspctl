# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_SYSTEM_PROCESSOR "arm")

# specify the cross compiler
SET(CMAKE_C_COMPILER   $ENV{HOME}/dev/arm-toolchain/bin/arm-none-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER $ENV{HOME}/dev/arm-toolchain/bin/arm-none-linux-gnueabihf-g++)

# where is the target environment 
SET(CMAKE_SYSROOT  $ENV{HOME}/dev/arm-sysroot)
SET(CMAKE_FIND_ROOT_PATH  $ENV{HOME}/dev/arm-sysroot $ENV{HOME}/dev/arm-staging)
SET(CMAKE_PREFIX_PATH ${CMAKE_SYSROOT}/usr)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_FLAGS_INIT   "--sysroot=${CMAKE_SYSROOT} -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard")
set(CMAKE_CXX_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT} -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard")

# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_SYSTEM_PROCESSOR "arm")

# specify the cross compiler
SET(CMAKE_C_COMPILER   $ENV{HOME}/arm/.build/arm-cortex_a8-linux-gnueabihf/buildtools/bin/arm-cortex_a8-linux-gnueabihf-gcc)

# where is the target environment 
SET(CMAKE_SYSROOT  $ENV{HOME}/arm/rootfs)
SET(CMAKE_FIND_ROOT_PATH  $ENV{HOME}/arm/rootfs $ENV{HOME}/arm/staging)
set(ENV{PKG_CONFIG_PATH} "$ENV{HOME}/arm/staging/lib/pkgconfig")
set(ENV{LIBPATH} "$ENV{HOME}/arm/rootfs/usr/lib")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

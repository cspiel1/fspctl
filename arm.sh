#!/bin/bash
export PKG_CONFIG_SYSROOT_DIR=$HOME/dev/arm-sysroot
export PKG_CONFIG_LIBDIR=$HOME/dev/arm-sysroot/usr/lib/pkgconfig:$HOME/dev/arm-sysroot/usr/share/pkgconfig
cmake -B buildarm -DCMAKE_TOOLCHAIN_FILE=arm.cmake -S .
cmake --build buildarm

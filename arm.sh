#!/bin/bash
mkdir -p buildarm 
cd buildarm
cmake -DCMAKE_TOOLCHAIN_FILE=../arm.cmake ..
make

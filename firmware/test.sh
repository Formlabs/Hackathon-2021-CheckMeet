#!/bin/sh -uex

mkdir -p build
cd build
test -f build/CMakeCache.txt || cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
exec ./catch_firmware

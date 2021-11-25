#!/bin/sh -uex

MYDIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
SRC_DIR="${MYDIR}"
BUILD_DIR="${MYDIR}/build"

mkdir -p "${BUILD_DIR}"
test -f "${BUILD_DIR}/CMakeCache.txt" || cmake -S "${SRC_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug
cmake --build "${BUILD_DIR}"
(cd "${BUILD_DIR}" && ctest --output-on-failure)

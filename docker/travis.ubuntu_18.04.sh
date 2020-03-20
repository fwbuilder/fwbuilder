#!/bin/bash

set -e -x

mkdir -p /fwbuilder/build
cd /fwbuilder/build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
make install
QT_QPA_PLATFORM=vnc ctest --verbose

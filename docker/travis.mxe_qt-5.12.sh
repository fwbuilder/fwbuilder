#!/bin/bash

set -e -x

export PATH=$PATH:/opt/mxe/usr/bin

mkdir -p /fwbuilder/build
cd /fwbuilder/build
i686-w64-mingw32.shared-cmake ..
make -j$(nproc)

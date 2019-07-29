#!/bin/bash

set -e -x

mkdir -p /fwbuilder/build
cd /fwbuilder/build
cmake ..
make -j$(nproc)

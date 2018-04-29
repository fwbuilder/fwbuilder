#!/bin/bash

set -e -x

CXXFLAGS=-Wno-error=attributes ./autogen.sh
make -j$(nproc)

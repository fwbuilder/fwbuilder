#!/bin/bash

set -e -x

./autogen.sh
make -j$(nproc)

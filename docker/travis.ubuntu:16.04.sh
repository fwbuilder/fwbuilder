#!/bin/bash

set -e -x

qbs setup-toolchains --detect
qbs setup-qt $(which qmake) qt
qbs debug profile:qt

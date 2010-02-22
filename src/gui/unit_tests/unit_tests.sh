#!/bin/bash

commands=$@

for directory in `find . -maxdepth 1 -type d -regex '\./[A-Za-z0-9\-\_]*'`
do
    cd $directory
    [ ! -e Makefile ] && qmake -spec $QMAKESPEC
    $commands
    cd -
done;
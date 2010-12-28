#!/bin/bash

set +x

commands=$@

for directory in `find . -maxdepth 1 -type d -regex '\./[A-Za-z0-9\-\_]*'`
do
    echo "======================= $directory"
    cd $directory
    [ ! -e Makefile ] && qmake -spec $QMAKESPEC
    $commands || exit 1
    cd -
done


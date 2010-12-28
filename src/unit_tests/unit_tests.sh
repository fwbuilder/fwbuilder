#!/bin/bash

set +x

commands=$@

build() {
    directory=$1
    shift
    commands=$@
    cd $directory
    [ ! -e Makefile ] && qmake -spec $QMAKESPEC
    $commands || exit 1
    cd -
}

build main $commands

for directory in `find . -maxdepth 1 -type d -regex '\./[A-Za-z0-9\-\_]*'`
do
    echo "======================= $directory"
    build $directory $commands
done


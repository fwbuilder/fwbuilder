#!/bin/bash

action=$@

for directory in $(find . -name unit_tests)
do
   home=`pwd`
   cd $directory
   echo $directory
   qmake -spec $QMAKESPEC || exit 1
   $action || exit 1
   cd $home
done
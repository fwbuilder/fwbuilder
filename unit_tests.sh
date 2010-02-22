#!/bin/bash

action=$@

for directory in $(find . -name unit_tests)
do
   home=`pwd`
   cd $directory
   qmake -spec $QMAKESPEC
   $action
   cd $home
done
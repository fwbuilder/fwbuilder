#!/bin/sh

for d in */run-tests.sh
do
    (d=`dirname $d`; echo; echo "******* $d    "; cd $d && ./run-tests.sh)
done

#!/bin/sh

qmake
make 
echo "Copying RCS file into place"
mkdir -p RCS
cp zu.fwb_v RCS/zu.fwb,v
echo "Running the test"
./rcs_test && echo PASS || echo FAIL


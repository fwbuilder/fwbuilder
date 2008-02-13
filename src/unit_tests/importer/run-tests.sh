#!/bin/sh

qmake
make
echo "Running the test"
echo
echo "iosacl" && \
./importer_test iosacl < test_data/test1.conf > test1.result 2> test1.stderr
cmp test1.result test_data/test1.result &&  \
cmp test1.stderr test_data/test1.stderr

echo "iptables" && \
./importer_test iptables < test_data/iptables_test1.conf > iptables_test1.result 2> iptables_test1.stderr
cmp iptables_test1.result test_data/iptables_test1.result &&  \
cmp iptables_test1.stderr test_data/iptables_test1.stderr &&  \

echo PASS || echo FAIL


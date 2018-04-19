include(../tests_common.pri)

QT += testlib network gui widgets printsupport
TARGET = instDialogClusterTest
SOURCES += main_instDialogClusterTest.cpp \
           instDialogClusterTest.cpp
HEADERS += instDialogClusterTest.h

run_tests.commands = cp -f test.fwb test_work.fwb; \
                     ./${TARGET}; \
                     rm -f test_work.fwb

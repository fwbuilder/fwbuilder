include(../tests_common.pri)

QT += testlib network gui widgets printsupport
TARGET = newClusterDialogTest
SOURCES += main_newClusterDialogTest.cpp \
    newClusterDialogTest.cpp
HEADERS += newClusterDialogTest.h

run_tests.commands = cp -f test.fwb test_work.fwb; \
                     ./${TARGET}; \
                     rm -f test_work.fwb



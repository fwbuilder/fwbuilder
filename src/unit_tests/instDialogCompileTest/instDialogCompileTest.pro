include(../tests_common.pri)

QT += testlib network gui widgets printsupport
TARGET = instDialogCompileTest
SOURCES += main_instDialogCompileTest.cpp \
    instDialogCompileTest.cpp
HEADERS += instDialogCompileTest.h

run_tests.commands = cp -f test.fwb test_work.fwb; \
                     ./${TARGET}; \
                     rm -f test_work.fwb

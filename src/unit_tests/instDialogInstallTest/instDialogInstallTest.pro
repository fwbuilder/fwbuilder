include(../tests_common.pri)

QT += testlib network gui widgets printsupport
TARGET = instDialogInstallTest
SOURCES += main_instDialogInstallTest.cpp \
    instDialogInstallTest.cpp
HEADERS += instDialogInstallTest.h

run_tests.commands = cp -f test.fwb test_work.fwb; \
                     ./${TARGET}; \
                     rm -f test_work.fwb

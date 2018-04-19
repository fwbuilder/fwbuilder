include(../tests_common.pri)

QT += testlib network gui widgets printsupport
TARGET = ObjectManipulatorTest
SOURCES += main_ObjectManipulatorTest.cpp \
    ObjectManipulatorTest.cpp
HEADERS += ObjectManipulatorTest.h

run_tests.commands = cp -f test.fwb test_work.fwb; \
                     ./${TARGET}; \
                     rm -f test_work.fwb

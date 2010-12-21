include(../../../../qmake.inc)

QT -= core \
    gui

QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS
LIBS += $$CPPUNIT_LIBS

TARGET = InetAddrMaskTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    InetAddrMaskTest.cpp
HEADERS += InetAddrMaskTest.h
INCLUDEPATH += ../../..
LIBS += -L ../.. -lfwbuilder
run_tests.commands = echo "Running tests..." && ./${TARGET}
run_tests.depends = all
clean_tests.depends = clean
build_tests.depends = all
QMAKE_EXTRA_TARGETS += run_tests clean_tests build_tests

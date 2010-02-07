
include(../../../../qmake.inc)

QT -= core \
    gui
TARGET = AddressTableTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    AddressTableTest.cpp
HEADERS += AddressTableTest.h
INCLUDEPATH += ../../..
LIBS += -L ../.. -lcppunit -lfwbuilder
run.commands = echo "Running tests..." && ./${TARGET}
run.depends = all
QMAKE_EXTRA_TARGETS += run

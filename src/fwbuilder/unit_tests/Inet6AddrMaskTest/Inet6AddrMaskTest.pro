include(../../../../qmake.inc)

QT -= core \
    gui
TARGET = Inet6AddrMaskTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    Inet6AddrMaskTest.cpp
HEADERS += Inet6AddrMaskTest.h
INCLUDEPATH += ../../..
LIBS += -L ../.. -lcppunit -lfwbuilder
run.commands = echo "Running tests..." && ./${TARGET}
run.depends = all
QMAKE_EXTRA_TARGETS += run

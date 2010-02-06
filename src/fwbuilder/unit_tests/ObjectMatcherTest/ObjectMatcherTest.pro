
include(../../../../qmake.inc)

QT -= core \
    gui
TARGET = ObjectMatcherTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ObjectMatcherTest.cpp
HEADERS += ObjectMatcherTest.h
INCLUDEPATH += ../../..
LIBS += -lcppunit -lfwbuilder
run.commands = echo "Running tests..." && ./${TARGET}
run.depends = all
QMAKE_EXTRA_TARGETS += run

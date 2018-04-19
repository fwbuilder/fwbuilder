include(../tests_common.pri)

QT += testlib network gui widgets printsupport
TARGET = NetworkDialogTests
SOURCES += main_NetworkDialogTest.cpp \
    NetworkDialogTest.cpp
HEADERS += NetworkDialogTest.h

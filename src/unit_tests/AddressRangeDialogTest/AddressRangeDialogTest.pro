include(../tests_common.pri)

QT += testlib network gui
TARGET = AddressRangeDialogTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += AddressRangeDialogTest.cpp \
           main_AddressRangeDialogTest.cpp

HEADERS += AddressRangeDialogTest.h

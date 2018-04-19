include(../tests_common.pri)

QT += testlib network gui widgets printsupport
TARGET = CustomServiceDialogTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main_CustomServiceDialogTest.cpp \
    CustomServiceDialogTest.cpp

HEADERS += CustomServiceDialogTest.h

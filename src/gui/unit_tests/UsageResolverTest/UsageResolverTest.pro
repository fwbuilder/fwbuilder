# -------------------------------------------------
# Project created by QtCreator 2009-12-08T19:22:04
# -------------------------------------------------

include(../../../../qmake.inc)

QT += gui network

INCLUDEPATH += ../../.ui ../../../compiler_lib/
TARGET = UsageResolverTest
#CONFIG += console
#CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    UsageResolverTest.cpp \
    ../../UsageResolver.cpp \
    ../../FWBTree.cpp

HEADERS += UsageResolverTest.h ../../UsageResolver.h ../../FWBTree.h

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER
LIBS += -lgcov -lcppunit

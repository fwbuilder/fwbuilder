# -------------------------------------------------
# Project created by QtCreator 2009-12-08T19:22:04
# -------------------------------------------------

include(../../../../qmake.inc)

QT += gui network

INCLUDEPATH += ../../.ui ../../../compiler_lib/
TARGET = FWObjectDatabaseTest
#CONFIG += console
#CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    FWObjectDatabaseTest.cpp \
    ../../ObjectManipulator.cpp \
    ../../ObjectManipulator_create_new.cpp \
    ../../ObjectManipulator_ops.cpp \
    ../../ObjectManipulator_slots.cpp \
    ../../ObjectManipulator_tree_ops.cpp \
    ../../FWWindow.cpp \
    ../../FWWindowPrint.cpp \
    ../../FWWindow_editor.cpp \
    ../../FWWindow_wrappers.cpp

HEADERS += FWObjectDatabaseTest.h ../../ObjectManipulator.h ../../FWWindow.h

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER
LIBS += -lgcov -lcppunit

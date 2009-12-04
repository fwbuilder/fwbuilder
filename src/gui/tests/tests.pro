# -*- mode: makefile; tab-width: 4; -*-
# $Id: gui.pro 1987 2009-12-01 01:23:24Z vadim $

include(../../../qmake.inc)

CONFIG+=qtestlib
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += ../../iptlib \
    ../../pflib \
    ../../cisco_lib/ \
    ../../compiler_lib/

DEPENDPATH = ../../iptlib \
    ../../pflib \
    ../../cisco_lib/ \
    ../../compiler_lib
win32:LIBS += ../../iptlib/release/iptlib.lib \
    ../../pflib/release/fwbpf.lib \
    ../../cisco_lib/release/fwbcisco.lib \
    ../../compiler_lib/release/compilerdriver.lib
!win32:LIBS += ../../iptlib/libiptlib.a \
    ../../pflib/libfwbpf.a \
    ../../cisco_lib/libfwbcisco.a \
    ../../compiler_lib/libcompilerdriver.a
win32:PRE_TARGETDEPS = ../../iptlib/release/iptlib.lib \
    ../../pflib/release/fwbpf.lib \
    ../../cisco_lib/release/fwbcisco.lib \
    ../../compiler_lib/release/compilerdriver.lib
!win32:PRE_TARGETDEPS = ../../iptlib/libiptlib.a \
    ../../pflib/libfwbpf.a \
    ../../cisco_lib/libfwbcisco.a \
    ../../compiler_lib/libcompilerdriver.a

# Input
HEADERS += InterfacePropertiesTest.h #../global.h ../../../VERSION.h
#LIBRARIES += ../../../../../install_root/lib
SOURCES += InterfacePropertiesTest.cpp
#INCLUDES += ../../../../../install_root/includes
#QMAKE_LFLAGS += -L../../../../../install_root/lib -lfwbuilder

# -*- mode: makefile; tab-width: 4; -*-
# $Id$
TEMPLATE = app
LANGUAGE = C++
QT += network
TARGET = fwbuilder
include(../../qmake.inc)
exists(qmake.inc):include( qmake.inc)

SOURCES += main.cpp

win32:LIBS += ../libgui/libgui.lib
!win32:LIBS += ../libgui/libgui.a
INCLUDEPATH += ../libgui

INCLUDEPATH += $$ANTLR_INCLUDEPATH
LIBS += $$FWBPARSER_LIB \
    $$ANTLR_LIBS
DEFINES += $$ANTLR_DEFINES

# fwtransfer lib. Add this before adding -lQtDBus to LIBS below
LIBS += $$FWTRANSFER_LIB
contains( HAVE_QTDBUS, 1 ):unix { 
    !macx:QT += network \
        dbus
    macx:LIBS += -framework \
        QtDBus
}

# !macx:LIBS += -lQtDBus # workaround for QT += dbus not working with Qt < 4.4.0
INCLUDEPATH += ../common \
    ../iptlib \
    ../pflib \
    ../cisco_lib/ \
    ../compiler_lib/
DEPENDPATH = ../common \
    ../iptlib \
    ../pflib \
    ../cisco_lib/ \
    ../compiler_lib
win32:LIBS += ../common/release/common.lib \
    ../iptlib/release/iptlib.lib \
    ../pflib/release/fwbpf.lib \
    ../cisco_lib/release/fwbcisco.lib \
    ../compiler_lib/release/compilerdriver.lib
!win32:LIBS += ../common/libcommon.a \
    ../iptlib/libiptlib.a \
    ../pflib/libfwbpf.a \
    ../cisco_lib/libfwbcisco.a \
    ../compiler_lib/libcompilerdriver.a
win32:PRE_TARGETDEPS = ../common/release/common.lib \
    ../iptlib/release/iptlib.lib \
    ../pflib/release/fwbpf.lib \
    ../cisco_lib/release/fwbcisco.lib \
    ../compiler_lib/release/compilerdriver.lib \
    $$FWBPARSER_LIB
!win32:PRE_TARGETDEPS = ../common/libcommon.a \
    ../iptlib/libiptlib.a \
    ../pflib/libfwbpf.a \
    ../cisco_lib/libfwbcisco.a \
    ../compiler_lib/libcompilerdriver.a \
    $$FWBPARSER_LIB
macx:LIBS += -framework \
    Carbon
LIBS += $$LIBS_FWCOMPILER
#RESOURCES += ../libgui/MainRes.qrc
TRANSLATIONS = fwbuilder_ru.ts \
    fwbuilder_ja.ts \
    fwbuilder_en.ts
ja.path = $$PKGLOCALEDIR
ja.files = fwbuilder_ja.qm
ru.path = $$PKGLOCALEDIR
ru.files = fwbuilder_ru.qm
INSTALLS += ja ru


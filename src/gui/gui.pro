# -*- mode: makefile; tab-width: 4; -*-
# $Id$
TEMPLATE = app
LANGUAGE = C++
QT += network
TARGET = fwbuilder
include(../../qmake.inc)
exists(qmake.inc):include( qmake.inc)

SOURCES += main.cpp

# Arrange static libraries before dynamic ones in the linker command
# line.  libgui goes first
win32 {
    IMPORT_LIB = ../import/release/import.lib
    FWBPARSER_LIB = ../parsers/release/fwbparser.lib
    FWTRANSFER_LIB = ../fwtransfer/release/fwtransfer.lib
    STATIC_LIBS += ../libgui/release/gui.lib
}

!win32 {
    IMPORT_LIB = ../import/libimport.a
    FWBPARSER_LIB = ../parsers/libfwbparser.a
    FWTRANSFER_LIB = ../fwtransfer/libfwtransfer.a
    STATIC_LIBS += ../libgui/libgui.a
}

INCLUDEPATH += $$ANTLR_INCLUDEPATH
STATIC_LIBS += $$IMPORT_LIB $$FWBPARSER_LIB $$ANTLR_LIBS
DEFINES += $$ANTLR_DEFINES

# fwtransfer lib. Add this before adding -lQtDBus to LIBS below
STATIC_LIBS += $$FWTRANSFER_LIB
contains( HAVE_QTDBUS, 1 ):unix { 
    !macx:QT += network \
        dbus
    macx:STATIC_LIBS += -framework \
        QtDBus
}

# !macx:STATIC_LIBS += -lQtDBus # workaround for QT += dbus not working with Qt < 4.4.0
INCLUDEPATH +=  \
	../common \
    ../import \
    ../iptlib \
    ../pflib \
    ../cisco_lib/ \
    ../compiler_lib/ \
	../libgui \
	../libfwbuilder/src

win32:INCLUDEPATH += ../libgui/ui
!win32:INCLUDEPATH += ../libgui/.ui

DEPENDPATH =  \
	../common \
    ../import \
    ../iptlib \
    ../pflib \
    ../cisco_lib/ \
    ../compiler_lib \
	../libgui \
	../libfwbuilder/src/fwbuilder \
	../libfwbuilder/src/fwcompiler

win32:STATIC_LIBS += \
	../common/release/common.lib \
    ../iptlib/release/iptlib.lib \
    ../pflib/release/fwbpf.lib \
    ../cisco_lib/release/fwbcisco.lib \
    ../compiler_lib/release/compilerdriver.lib \
	../libfwbuilder/src/fwbuilder/release/fwbuilder.lib \
	../libfwbuilder/src/fwcompiler/release/fwcompiler.lib

!win32:STATIC_LIBS +=  \
	../common/libcommon.a \
    ../iptlib/libiptlib.a \
    ../pflib/libfwbpf.a \
    ../cisco_lib/libfwbcisco.a \
    ../compiler_lib/libcompilerdriver.a \
	../libfwbuilder/src/fwcompiler/libfwcompiler.a \
	../libfwbuilder/src/fwbuilder/libfwbuilder.a \

win32:PRE_TARGETDEPS = \
	../libgui/release/gui.lib \
	../common/release/common.lib \
    ../iptlib/release/iptlib.lib \
    ../pflib/release/fwbpf.lib \
    ../cisco_lib/release/fwbcisco.lib \
    ../compiler_lib/release/compilerdriver.lib \
	../libfwbuilder/src/fwbuilder/release/fwbuilder.lib \
	../libfwbuilder/src/fwcompiler/release/fwcompiler.lib \
    $$FWBPARSER_LIB \
    $$IMPORT_LIB

!win32:PRE_TARGETDEPS = \
    ../libgui/libgui.a \
    ../common/libcommon.a \
    ../iptlib/libiptlib.a \
    ../pflib/libfwbpf.a \
    ../cisco_lib/libfwbcisco.a \
    ../compiler_lib/libcompilerdriver.a \
    ../libfwbuilder/src/fwbuilder/libfwbuilder.a \
    ../libfwbuilder/src/fwcompiler/libfwcompiler.a \
    $$FWBPARSER_LIB \
    $$IMPORT_LIB

macx:STATIC_LIBS += -framework \
    Carbon
STATIC_LIBS += $$LIBS_FWCOMPILER

LIBS = $$STATIC_LIBS $$LIBS

#RESOURCES += ../libgui/MainRes.qrc
TRANSLATIONS = fwbuilder_ru.ts \
    fwbuilder_ja.ts \
    fwbuilder_en.ts
ja.path = $$PKGLOCALEDIR
ja.files = fwbuilder_ja.qm
ru.path = $$PKGLOCALEDIR
ru.files = fwbuilder_ru.qm
INSTALLS += ja ru


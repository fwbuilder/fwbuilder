#-*- mode: makefile; tab-width: 4; -*-
#
#
include(../../../qmake.inc)

SOURCES = transfer_secuwall.cpp

HEADERS = ../../../config.h

INCLUDEPATH += ../../libfwbuilder/src
DEPENDPATH  += ../../libfwbuilder/src

contains( HAVE_QTDBUS, 1 ):unix {
	!macx: QT += network dbus
	macx:  LIBS += -framework QtDBus
#!macx:LIBS += -lQtDBus # workaround for QT += dbus not working with Qt < 4.4.0
}

!win32 {
	QMAKE_COPY = ../../../install.sh -m 0755 -s
    LIBS  = ../../fwtransfer/libfwtransfer.a  \  # -lQtDBus
        ../../libfwbuilder/src/fwcompiler/libfwcompiler.a \
        ../../libfwbuilder/src/fwbuilder/libfwbuilder.a \
      $$LIBS
}

win32 {
    CONFIG += console
    LIBS  += ../../fwtransfer/release/fwtransfer.lib  \  # -lQtDBus
        ../../libfwbuilder/src/fwcompiler/release/fwcompiler.lib \
        ../../libfwbuilder/src/fwbuilder/release/fwbuilder.lib
}

TARGET = transfer_secuwall


#-*- mode: makefile; tab-width: 4; -*-
#
#
include(../../../qmake.inc)

SOURCES = transfer_secuwall.cpp

HEADERS = ../../../config.h

contains( HAVE_QTDBUS, 1 ):unix {
	!macx: QT += network dbus
	macx:  LIBS += -framework QtDBus
#!macx:LIBS += -lQtDBus # workaround for QT += dbus not working with Qt < 4.4.0
}

!win32 {
	QMAKE_COPY = ../../../install.sh -m 0755 -s
}

win32:CONFIG += console

TARGET = transfer_secuwall

LIBS  += ../$$FWTRANSFER_LIB  # -lQtDBus

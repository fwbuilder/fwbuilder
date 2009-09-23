#-*- mode: makefile; tab-width: 4; -*-
#
#
include(../../../qmake.inc)

SOURCES = transfer_secuwall.cpp

HEADERS = ../../../config.h

unix {
  !macx {
# workaround for QT += dbus not working atm
        LIBS += -lQtDBus
  }
}

!win32 {
	QMAKE_COPY = ../../../install.sh -m 0755 -s
}

win32:CONFIG += console

TARGET = transfer_secuwall

LIBS  += ../$$FWTRANSFER_LIB  # -lQtDBus

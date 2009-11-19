# -*- mode: makefile; tab-width: 4; -*-
TEMPLATE = app

unix {
	!macx: QT += network dbus
	macx:  LIBS += -framework QtDBus
}

SOURCES = qtdbus_test.cpp
TARGET = qtdbus_test

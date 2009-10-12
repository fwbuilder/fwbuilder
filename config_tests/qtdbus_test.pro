# -*- mode: makefile; tab-width: 4; -*-
TEMPLATE = app
unix {
  macx {
      LIBS += -framework QtDBus
  }
  !macx {
      # workaround for QT += dbus not working atm
      LIBS += -lQtDBus
  }
}
SOURCES = qtdbus_test.cpp
TARGET = qtdbus_test

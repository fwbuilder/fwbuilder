#-*- mode: makefile; tab-width: 4; -*-
#
#
#
include(../../../qmake.inc)
#
exists(qmake.inc) {
  include( qmake.inc)
}

QT             += network

HEADERS	+= ../../../config.h \
	../../gui/RCS.h \
        ../../gui/utils.h \
	../../gui/global.h

SOURCES	+=  rcs_test.cpp \
        ../../gui/utils.cpp \
        ../../gui/RCS.cpp


#
include(../../../qmake.inc)
#
exists(../qmake.inc) {
  include( ../qmake.inc)
}

TARGET = rcs_test

INCLUDEPATH += ../../.. ../../gui/

QMAKE_COPY    = echo

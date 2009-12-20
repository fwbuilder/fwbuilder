#-*- mode: makefile; tab-width: 4; -*-
#
#
#
include(../../../qmake.inc)
#
exists(qmake.inc) {
  include( qmake.inc)
}

QT += network

HEADERS +=  ../../../config.h \
	    ../../gui/utils_no_qt.h \
		../../gui/FWBTree.h \
	    ../../gui/Importer.h \
	    ../../gui/IOSImporter.h \
	    ../../gui/IPTImporter.h

SOURCES += importer_test.cpp \
	    ../../gui/utils_no_qt.cpp \
		../../gui/FWBTree.cpp \
	    ../../gui/Importer.cpp \
	    ../../gui/IOSImporter.cpp \
	    ../../gui/IOSImporterRun.cpp \
	    ../../gui/IPTImporter.cpp \
	    ../../gui/IPTImporterRun.cpp

TARGET = importer_test
#TARGETDEPS += unit_test

!win32:LIBS += ../../common/libcommon.a
!win32:PRE_TARGETDEPS = ../../common/libcommon.a

win32:CONFIG += console

win32:LIBS += ../../common/release/common.lib
win32:PRE_TARGETDEPS = ../../common/release/common.lib


contains( HAVE_ANTLR_RUNTIME, 1 ) {
	INCLUDEPATH	 += $$ANTLR_INCLUDEPATH
	LIBS		 += ../../parsers/libfwbparser.a  $$ANTLR_LIBS 
	DEFINES		 += $$ANTLR_DEFINES
}

INCLUDEPATH += ../../.. ../../gui/ ../../gui/.ui

QMAKE_COPY   = echo



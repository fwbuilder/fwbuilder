#-*- mode: makefile; tab-width: 4; -*-
#
include(../../../qmake.inc)
#
TEMPLATE= app
#
SOURCES	 = inet6addrmask_test.cpp

TARGET = inet6addrmask_test

QMAKE_COPY    = ../../../install.sh -m 0755 -s

win32:target.path =$$prefix/
unix:target.path  =$$prefix/usr/bin
macx:target.path  =$$prefix/

unix {
	CONFIG   -= qt
    CONFIG   += warn_on debug
    INCLUDEPATH += ../../

    LIBS     += -L../../fwbuilder -lfwbuilder
}

win32 {
    CONFIG   -= qt
    CONFIG   += console thread rtti stl warn_on release
    DEFINES  += LIBXML_STATIC LIBXSLT_STATIC LIBEXSLT_STATIC XMLSEC_STATIC
    INCLUDEPATH += Z:\include C:\local\include
    LIBS     += -LZ: -LC:\local\lib
    LIBS     += fwbuilder.lib ws2_32.lib pthreadVC.lib getopt.lib
    LIBS     += libxml2.lib libxslt.lib zlib.lib ssleay32.lib libeay32.lib
}


INSTALLS     -= target

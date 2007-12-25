#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE= app
#
VERSION = $$SO_VERSION
#
SOURCES  = 	ipAddressTest.cpp \
			main.cpp \
			removeObjectTest.cpp
#
TARGET  = test

win32:target.path =$$prefix/
unix:target.path  =$$prefix/usr/bin
macx:target.path  =$$prefix/

unix {
	CONFIG   -= qt
	CONFIG   += warn_on debug
	INCLUDEPATH += ../

	LIBS     += -L../fwbuilder -lfwbuilder -pthread -lxslt -lxml2 -lz -lm -lssl -lcrypto /usr/lib/libresolv.a
}

win32 {
	CONFIG   -= qt
    CONFIG   += thread rtti stl warn_on release
    DEFINES  += LIBXML_STATIC LIBXSLT_STATIC LIBEXSLT_STATIC XMLSEC_STATIC
    INCLUDEPATH += Z:\include C:\local\include
    LIBS     += -LZ: -LC:\local\lib
    LIBS     += fwbuilder.lib ws2_32.lib pthreadVC.lib getopt.lib
    LIBS     += libxml2.lib libxslt.lib zlib.lib ssleay32.lib libeay32.lib
}

INSTALLS   -= target

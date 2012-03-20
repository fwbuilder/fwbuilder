#-*- mode: makefile; tab-width: 4; -*-
#
######### libfwbuilder/qmake.inc.in
#
QTDIR = $$(QTDIR)
TEMPLATE = lib
DEFINES += $$(DEFINES)
INCLUDEPATH += .. ../.. $$(INCLUDEPATH)
DEPENDPATH  += .. ../..
LANGUAGE = C++

HAVE_CPPUNIT = 1
CPPUNIT_CFLAGS = 
CPPUNIT_LIBS = -lcppunit -ldl

unix {
    CONFIG += warn_on debug

    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj

    prefix = /usr/local

    QMAKE_CXX = /usr/bin/ccache  $$QMAKE_CXX

	# We put /usr/include into INCLUDEPATH to fix a problem compiling
	# fwbuilder/snmp.cpp on freebsd.
    INCLUDEPATH += /usr/include /usr/include/libxml2 
    LIBS += -pthread -lxml2  -L/usr/lib -lxslt -Wl,-Bsymbolic-functions -L/usr/lib -lnetsnmp  -lz -Wl,-Bsymbolic-functions -L/usr/lib -lnetsnmp -lutil 

    QMAKE_CFLAGS_DEBUG += -Wno-unused-parameter
    QMAKE_CFLAGS_RELEASE += -Wno-unused-parameter
    QMAKE_CXXFLAGS_DEBUG += -Wno-unused-parameter -g -O2
    QMAKE_CXXFLAGS_RELEASE += -Wno-unused-parameter -g -O2

    exec_prefix = @EXEC_PREFIX@
    DESTDIR =

    target.path = @LIBDIR@
    dtd.path = /usr/local/share/fwbuilder-5.1.0.3598/
    migration.path = /usr/local/share/fwbuilder-5.1.0.3598/migration
    doc.path = /usr/local/share/doc/fwbuilder-5.1.0.3598
}

QMAKE_CXXFLAGS_DEBUG += -D__STDC_FORMAT_MACROS
QMAKE_CXXFLAGS_RELEASE += -D__STDC_FORMAT_MACROS

exists(qmake2.inc) {
  include( qmake2.inc)
}

CONFIG -= qt

INSTALLS += target

#-*- mode: makefile; tab-width: 4; -*-
#
########## fwbuilder/qmake.inc.in
#
QTDIR = $$(QTDIR)
TEMPLATE = app
DEFINES += $$(DEFINES)
LANGUAGE = C++
UI_DIR = ui
MANDIR = /usr/local/share/man/
DOCDIR = /usr/local/share/doc/fwbuilder-5.1.0.3598

HAVE_QTDBUS = @HAVE_QTDBUS@
HAVE_CPPUNIT = 1
CPPUNIT_CFLAGS = 
CPPUNIT_LIBS = -lcppunit -ldl

# CONFIG -= nostrip
# CONFIG += release
CONFIG += debug

unix {
    PREFIX = /usr/local

    ANTLR_INCLUDEPATH =  /home/vadim/src/v4/build/source/fwbuilder/src/
    ANTLR_LIBS =  /home/vadim/src/v4/build/source/fwbuilder/src/antlr/libantlr.a

    QMAKE_CXX = /usr/bin/ccache  $$QMAKE_CXX

    INCLUDEPATH += .. ../..
    INCLUDEPATH += /usr/include/libxml2 
    LIBS += -pthread -lxml2  -L/usr/lib -lxslt -Wl,-Bsymbolic-functions -L/usr/lib -lnetsnmp  -lz -Wl,-Bsymbolic-functions -L/usr/lib -lnetsnmp -lutil 

    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj

    QMAKE_CFLAGS_DEBUG += -Wno-unused-parameter
    QMAKE_CFLAGS_RELEASE += -Wno-unused-parameter
    QMAKE_CXXFLAGS_DEBUG += -Wno-unused-parameter
    QMAKE_CXXFLAGS_RELEASE += -Wno-unused-parameter

    !macx {

        exec_prefix = @EXEC_PREFIX@
        DESTDIR =
        ICONSDIR = /usr/local/share//icons/hicolor/

        target.path = $$PREFIX/bin
        dtd.path = /usr/local/share/fwbuilder-5.1.0.3598/
        migration.path = /usr/local/share/fwbuilder-5.1.0.3598/migration
        doc.path = /usr/local/share/doc/fwbuilder-5.1.0.3598
        datadir.path   = /usr/local/share/

        res.path = /usr/local/share/fwbuilder-5.1.0.3598
        res_os.path = $$res.path/os/
        res_platform.path = $$res.path/platform/
        res_help_C.path = $$res.path/help/C
        res_help_en_US.path = $$res.path/help/en_US
        res_desktop.path = /usr/local/share//applications/
        res_configlets.path = $$res.path/configlets

        # INSTALLS += icns
        # LIBS += -lz -Wl,-Bsymbolic-functions -L/usr/lib -lnetsnmp -lutil 

        PKGLOCALEDIR = $$res.path/locale

        CONFIG += warn_on

#		DEFINES += __STDC_FORMAT_MACROS
    }
}

win32 {
    CONFIG -= debug
}

FWB_SIG = $$(FWB_SIG)
if (isEmpty(FWB_SIG)) { FWB_SIG=26932 }
DEFINES += FWB_SIG=$$FWB_SIG

QMAKE_CXXFLAGS_DEBUG += -D__STDC_FORMAT_MACROS
QMAKE_CXXFLAGS_RELEASE += -D__STDC_FORMAT_MACROS

exists(qmake2.inc) {
  include(qmake2.inc)
}

INSTALLS += target

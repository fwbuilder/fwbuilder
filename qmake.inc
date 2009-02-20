#-*- mode: makefile; tab-width: 4; -*-
#
######### fwbuilder/qmake.inc.in
#
QTDIR         = $$(QTDIR)

TEMPLATE      = app

SO_VERSION    = @LIBFWBUILDER_SOLIB_VERSION@

DEFINES      += $$(DEFINES)
LANGUAGE      = C++
UI_DIR        = ui
MANDIR        = /home/vadim/src/fwb3-branch-v3/install_root/share/man/
DOCDIR        = /home/vadim/src/fwb3-branch-v3/install_root/share/doc/fwbuilder-3.0.4

HAVE_ANTLR_RUNTIME = 1
HAVE_EXTERNAL_ANTLR = 0

unix {
    ANTLR_INCLUDEPATH =  /home/vadim/src/fwb3-branch-v3/source/fwbuilder/src/
    ANTLR_LIBS =  /home/vadim/src/fwb3-branch-v3/source/fwbuilder/src/antlr/libantlr.a
    FWBPARSER_LIB = ../parsers/libfwbparser.a
}


unix {
!macx {
	UI_DIR = .ui
	MOC_DIR = .moc
	OBJECTS_DIR = .obj

	PREFIX                  = /home/vadim/src/fwb3-branch-v3/install_root
	exec_prefix             = @EXEC_PREFIX@
	DESTDIR                 =
	ICONSDIR                = /home/vadim/src/fwb3-branch-v3/install_root/share//icons/hicolor/

    INCLUDEPATH  += .. ../.. $$(INCLUDEPATH) /home/vadim/src/fwb3-branch-v3/install_root/include/ /home/vadim/src/fwb3-branch-v3/install_root/include/fwb-3 /usr/include/libxml2

    LIBS_FWCOMPILER         = -L/home/vadim/src/fwb3-branch-v3/install_root/lib -Wl,-Bsymbolic-functions -pthread -lfwcompiler -lfwbuilder -lz -lxslt -lxml2 -lnetsnmp
    LIBS_FWBUILDER          = -L/home/vadim/src/fwb3-branch-v3/install_root/lib -Wl,-Bsymbolic-functions -pthread -lfwbuilder -lz -lxslt -lxml2 -lnetsnmp

	target.path             = $$PREFIX/bin
	dtd.path                = @TEMPLATE_DIR@/
	migration.path          = @TEMPLATE_DIR@/migration
	doc.path                = /home/vadim/src/fwb3-branch-v3/install_root/share/doc/fwbuilder-3.0.4
	datadir.path		    = /home/vadim/src/fwb3-branch-v3/install_root/share/


#	win32:target.path    = $$PREFIX/
#	unix:target.path     = $$PREFIX/share/fwbuilder/
#	macx:target.path     = $$PREFIX/

	res.path			= /home/vadim/src/fwb3-branch-v3/install_root/share/fwbuilder-3.0.4
	res_os.path 		= /home/vadim/src/fwb3-branch-v3/install_root/share/fwbuilder-3.0.4/os/
	res_platform.path 	= /home/vadim/src/fwb3-branch-v3/install_root/share/fwbuilder-3.0.4/platform/
	res_help.path 	    = /home/vadim/src/fwb3-branch-v3/install_root/share/fwbuilder-3.0.4/help/
	res_desktop.path	= /home/vadim/src/fwb3-branch-v3/install_root/share//applications/

    INSTALLS		   += res 
    INSTALLS		   += res_os
    INSTALLS		   += res_platform
#        INSTALLS   += icns
	LIBS                   += $$LIBS_FWBUILDER -lutil 

	PKGLOCALEDIR			= $$res.path/locale

	CONFIG                 += warn_on debug
	QMAKE_CFLAGS_DEBUG     += -Wno-unused-parameter
	QMAKE_CFLAGS_RELEASE   += -Wno-unused-parameter
	QMAKE_CXXFLAGS_DEBUG   += -Wno-unused-parameter -g -O2
	QMAKE_CXXFLAGS_RELEASE += -Wno-unused-parameter -g -O2
}
}

macx {
    PREFIX = /home/vadim/src/fwb3-branch-v3/install_root
}

win32 {
    CONFIG -= debug
}

exists(qmake2.inc) {
  include(qmake2.inc)
}

INSTALLS += target

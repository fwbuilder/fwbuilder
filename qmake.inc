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
MANDIR        = /usr/home/vadim/src/fwb3/install_root/man/
DOCDIR        = /usr/home/vadim/src/fwb3/install_root/share/doc/fwbuilder-3.0.2

HAVE_ANTLR_RUNTIME = 1
HAVE_EXTERNAL_ANTLR = 0

unix {
    ANTLR_INCLUDEPATH =  /usr/home/vadim/src/fwb3/source/fwbuilder/src/
    ANTLR_LIBS =  /usr/home/vadim/src/fwb3/source/fwbuilder/src/antlr/libantlr.a
    FWBPARSER_LIB = ../parsers/libfwbparser.a
}


unix {
!macx {
	UI_DIR = .ui
	MOC_DIR = .moc
	OBJECTS_DIR = .obj

	PREFIX                  = /usr/home/vadim/src/fwb3/install_root
	exec_prefix             = @EXEC_PREFIX@
	DESTDIR                 =
	ICONSDIR                = 

    INCLUDEPATH  += .. ../.. $$(INCLUDEPATH) /usr/home/vadim/src/fwb3/install_root/include/ /usr/home/vadim/src/fwb3/install_root/include/fwb-3 /usr/local/include /usr/local/include/libxml2

    LIBS_FWCOMPILER         = -L/usr/home/vadim/src/fwb3/install_root/lib -L/usr/local/lib -lfwcompiler -lfwbuilder -lxslt -lxml2 -lz -liconv -lm
    LIBS_FWBUILDER          = -L/usr/home/vadim/src/fwb3/install_root/lib -L/usr/local/lib -lfwbuilder -lxslt -lxml2 -lz -liconv -lm

	target.path             = $$PREFIX/bin
	dtd.path                = @TEMPLATE_DIR@/
	migration.path          = @TEMPLATE_DIR@/migration
	doc.path                = /usr/home/vadim/src/fwb3/install_root/share/doc/fwbuilder-3.0.2
	datadir.path		    = /usr/home/vadim/src/fwb3/install_root/share/


#	win32:target.path    = $$PREFIX/
#	unix:target.path     = $$PREFIX/share/fwbuilder/
#	macx:target.path     = $$PREFIX/

	res.path			= /usr/home/vadim/src/fwb3/install_root/share/fwbuilder-3.0.2
	res_os.path 		= /usr/home/vadim/src/fwb3/install_root/share/fwbuilder-3.0.2/os/
	res_platform.path 	= /usr/home/vadim/src/fwb3/install_root/share/fwbuilder-3.0.2/platform/
	res_help.path 	    = /usr/home/vadim/src/fwb3/install_root/share/fwbuilder-3.0.2/help/
	res_desktop.path	= /usr/home/vadim/src/fwb3/install_root/share//applications/

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

win32 {
    CONFIG -= debug
}

exists(qmake2.inc) {
  include(qmake2.inc)
}

INSTALLS += target

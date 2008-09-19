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
MANDIR        = /private/var/tmp/src/fwb3-branch-v3/install_root/share/man/
DOCDIR        = /private/var/tmp/src/fwb3-branch-v3/install_root/share/doc/fwbuilder-3.0.1

HAVE_ANTLR_RUNTIME = 1
HAVE_EXTERNAL_ANTLR = 0

unix {
    ANTLR_INCLUDEPATH =  /private/var/tmp/src/fwb3-branch-v3/source/fwbuilder/src/
    ANTLR_LIBS =  /private/var/tmp/src/fwb3-branch-v3/source/fwbuilder/src/antlr/libantlr.a
    FWBPARSER_LIB = ../parsers/libfwbparser.a
}


unix {
!macx {
	UI_DIR = .ui
	MOC_DIR = .moc
	OBJECTS_DIR = .obj

	PREFIX                  = /private/var/tmp/src/fwb3-branch-v3/install_root
	exec_prefix             = @EXEC_PREFIX@
	DESTDIR                 =
	ICONSDIR                = 

    INCLUDEPATH  += .. ../.. $$(INCLUDEPATH) /private/var/tmp/src/fwb3-branch-v3/install_root/include/ /private/var/tmp/src/fwb3-branch-v3/install_root/include/fwb-3 /usr/include/libxml2

    LIBS_FWCOMPILER         = -L/private/var/tmp/src/fwb3-branch-v3/install_root/lib -lfwcompiler -lfwbuilder -lxslt -lxml2 -lz -lpthread -licucore -lm -lnetsnmp
    LIBS_FWBUILDER          = -L/private/var/tmp/src/fwb3-branch-v3/install_root/lib -lfwbuilder -lxslt -lxml2 -lz -lpthread -licucore -lm -lnetsnmp

	target.path             = $$PREFIX/bin
	dtd.path                = @TEMPLATE_DIR@/
	migration.path          = @TEMPLATE_DIR@/migration
	doc.path                = /private/var/tmp/src/fwb3-branch-v3/install_root/share/doc/fwbuilder-3.0.1
	datadir.path		    = /private/var/tmp/src/fwb3-branch-v3/install_root/share/


#	win32:target.path    = $$PREFIX/
#	unix:target.path     = $$PREFIX/share/fwbuilder/
#	macx:target.path     = $$PREFIX/

	res.path			= ../Resources
	res_os.path 		= ../Resources/os/
	res_platform.path 	= ../Resources/platform/
	res_help.path 	    = ../Resources/help/
	res_desktop.path	= /private/var/tmp/src/fwb3-branch-v3/install_root/share//applications/

    INSTALLS		   += res 
    INSTALLS		   += res_os
    INSTALLS		   += res_platform
#        INSTALLS   += icns
	LIBS                   += $$LIBS_FWBUILDER  -L/sw/lib -lpoll

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

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
MANDIR        = /Users/vadim/src/fwb3-branch-v3/install_root/share/man/
DOCDIR        = /Users/vadim/src/fwb3-branch-v3/install_root/share/doc/fwbuilder-3.0.0

HAVE_ANTLR_RUNTIME = 1
HAVE_EXTERNAL_ANTLR = 0

unix {
    ANTLR_INCLUDEPATH =  /Users/vadim/src/fwb3-branch-v3/source/fwbuilder/src/
    ANTLR_LIBS =  /Users/vadim/src/fwb3-branch-v3/source/fwbuilder/src/antlr/libantlr.a
    FWBPARSER_LIB = ../parsers/libfwbparser.a
}


unix {
!macx {
	UI_DIR = .ui
	MOC_DIR = .moc
	OBJECTS_DIR = .obj

	PREFIX                  = /Users/vadim/src/fwb3-branch-v3/install_root
	exec_prefix             = @EXEC_PREFIX@
	DESTDIR                 =

    INCLUDEPATH  += .. ../.. $$(INCLUDEPATH) /Users/vadim/src/fwb3-branch-v3/install_root/include/ /Users/vadim/src/fwb3-branch-v3/install_root/include/fwb-3 /usr/include/libxml2

    LIBS_FWCOMPILER         = -L/Users/vadim/src/fwb3-branch-v3/install_root/lib -lfwcompiler -lfwbuilder -lxslt -lxml2 -lz -lpthread -licucore -lm -lnetsnmp
    LIBS_FWBUILDER          = -L/Users/vadim/src/fwb3-branch-v3/install_root/lib -lfwbuilder -lxslt -lxml2 -lz -lpthread -licucore -lm -lnetsnmp

	target.path             = $$PREFIX/bin
	dtd.path                = @TEMPLATE_DIR@/
	migration.path          = @TEMPLATE_DIR@/migration
	doc.path                = /Users/vadim/src/fwb3-branch-v3/install_root/share/doc/fwbuilder-3.0.0

#	win32:target.path    = $$PREFIX/
#	unix:target.path     = $$PREFIX/share/fwbuilder/
#	macx:target.path     = $$PREFIX/

	res.files           = src/res/objects_init.xml src/res/templates.xml src/res/resources.xml
	res.path			= ../Resources
	res_os.files        = src/res/os/*.xml
	res_os.path 		= ../Resources/os/
	res_platform.files  = src/res/platform/*.xml
	res_platform.path 	= ../Resources/platform/

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

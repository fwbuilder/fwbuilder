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
DOCDIR        = /home/vadim/src/fwb3-branch-v3/install_root/share/doc/fwbuilder-3.0.0

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

	INCLUDEPATH  += .. ../.. $$(INCLUDEPATH) /home/vadim/src/fwb3-branch-v3/install_root/include/ /home/vadim/src/fwb3-branch-v3/install_root/include/fwb-3 /usr/include/libxml2

    LIBS_FWCOMPILER         = -L/home/vadim/src/fwb3-branch-v3/install_root/lib -pthread -lfwcompiler -lfwbuilder -lxslt -lxml2 -lnetsnmp -lm -lwrap
    LIBS_FWBUILDER          = -L/home/vadim/src/fwb3-branch-v3/install_root/lib -pthread -lfwbuilder -lxslt -lxml2 -lnetsnmp -lm -lwrap

	target.path             = $$PREFIX/bin
	dtd.path                = @TEMPLATE_DIR@/
	migration.path          = @TEMPLATE_DIR@/migration
	doc.path                = /home/vadim/src/fwb3-branch-v3/install_root/share/doc/fwbuilder-3.0.0

#	win32:target.path    = $$PREFIX/
#	unix:target.path     = $$PREFIX/share/fwbuilder/
#	macx:target.path     = $$PREFIX/

	res.files           = src/res/objects_init.xml src/res/templates.xml src/res/resources.xml
	res.path			= /home/vadim/src/fwb3-branch-v3/install_root/share/fwbuilder-3.0.0
	res_os.files        = src/res/os/*.xml
	res_os.path 		= /home/vadim/src/fwb3-branch-v3/install_root/share/fwbuilder-3.0.0/os/
	res_platform.files  = src/res/platform/*.xml
	res_platform.path 	= /home/vadim/src/fwb3-branch-v3/install_root/share/fwbuilder-3.0.0/platform/

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

exists(qmake2.inc) {
  include( qmake2.inc)
}

INSTALLS   += target

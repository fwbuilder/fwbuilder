#-*- mode: makefile; tab-width: 4; -*-
#
######### libfwbuilder/qmake.inc.in
#
QTDIR         = $$(QTDIR)

TEMPLATE      = lib

SO_VERSION    = 8.0.0

DEFINES      += $$(DEFINES)
INCLUDEPATH  += .. ../.. $$(INCLUDEPATH)
LANGUAGE      = C++

unix {
!macx {
	UI_DIR = .ui
	MOC_DIR = .moc
	OBJECTS_DIR = .obj

    prefix = /home/vadim/src/fwb3-branch-v3/install_root
	exec_prefix = $$prefix
	DESTDIR =

	target.path = $$exec_prefix/lib
	dtd.path = /home/vadim/src/fwb3-branch-v3/install_root/share/libfwbuilder-3.0.0/
	migration.path = /home/vadim/src/fwb3-branch-v3/install_root/share/libfwbuilder-3.0.0/migration
	doc.path = /home/vadim/src/fwb3-branch-v3/install_root/share/doc/libfwbuilder-3.0.0
    script.path = $$prefix/bin

	CONFIG                 += warn_on debug
	QMAKE_CFLAGS_DEBUG     += -Wno-unused-parameter
	QMAKE_CFLAGS_RELEASE   += -Wno-unused-parameter
	QMAKE_CXXFLAGS_DEBUG   += -Wno-unused-parameter -g -O2
	QMAKE_CXXFLAGS_RELEASE += -Wno-unused-parameter -g -O2
}
}

unix {
	INCLUDEPATH += /usr/include /usr/include/libxml2  \
				   -pthread 
	LIBS += -pthread -L/usr/lib -lxml2  -lxslt -L/usr/lib -lnetsnmp -lm -lwrap -lwrap \
				    -L/usr/lib -lnetsnmp -lm -lwrap -lwrap 
}

exists(qmake2.inc) {
  include( qmake2.inc)
}

CONFIG -= qt

INSTALLS += target

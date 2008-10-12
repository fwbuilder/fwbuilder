#-*- mode: makefile; tab-width: 4; -*-
#
######### libfwbuilder/qmake.inc.in
#
QTDIR         = $$(QTDIR)

TEMPLATE      = lib

SO_VERSION    = 8.1.0

DEFINES      += $$(DEFINES)
INCLUDEPATH  += .. ../.. $$(INCLUDEPATH)
LANGUAGE      = C++

unix {
!macx {
	UI_DIR = .ui
	MOC_DIR = .moc
	OBJECTS_DIR = .obj

    prefix = /home/vadim/src/fwb3/install_root
	exec_prefix = $$prefix
	DESTDIR =

	target.path = $$exec_prefix/lib
	dtd.path = /home/vadim/src/fwb3/install_root/share/libfwbuilder-3.0.2/
	migration.path = /home/vadim/src/fwb3/install_root/share/libfwbuilder-3.0.2/migration
	doc.path = /home/vadim/src/fwb3/install_root/share/doc/libfwbuilder-3.0.2

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
	LIBS += -pthread -lxml2  -L/usr/lib -lxslt -Wl,-Bsymbolic-functions -L/usr/lib -lnetsnmp \
				    -lz -Wl,-Bsymbolic-functions -L/usr/lib -lnetsnmp 
}

exists(qmake2.inc) {
  include( qmake2.inc)
}

CONFIG -= qt

INSTALLS += target

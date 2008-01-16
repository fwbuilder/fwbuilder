#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = app

QMAKE_RUN_CC    = @echo
QMAKE_RUN_CXX   = @echo
QMAKE_LINK      = @echo
QMAKE_COPY      = ../../install.sh -c -m 0755
QMAKE_COPY_FILE = ../../install.sh -c -m 0755

TARGET          = libfwbuilder-config-3

!macx {
	target.path   = $$prefix/bin
}

macx {
	script.path   =$$prefix/bin
	script.files  =libfwbuilder-config-3

	INSTALLS     -= target
	INSTALLS     += script
}

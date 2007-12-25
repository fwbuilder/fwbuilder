#-*- mode: makefile; tab-width: 4; -*-
#

include(../../qmake.inc)

win32 {
	QMAKE_RUN_CC  = @echo
	QMAKE_RUN_CXX = @echo
	QMAKE_LINK    = @echo
}

unix {
	QMAKE_RUN_CC  = @true
	QMAKE_RUN_CXX = @true
	QMAKE_LINK    = @true
}

macx {
	QMAKE_RUN_CC  = @true
	QMAKE_RUN_CXX = @true
	QMAKE_LINK    = @true
}

TARGET        = tools

win32:tools.path    = $$target.path
unix:tools.path     = $$target.path
macx:tools.path     = $$target.path

tools.files         = fwb_install fwb_compile_all

INSTALLS   -= target
INSTALLS   += tools


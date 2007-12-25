#-*- mode: makefile; tab-width: 4; -*-
#

include(../qmake.inc)

TEMPLATE = app

QMAKE_RUN_CC  = @echo
QMAKE_RUN_CXX = @echo
QMAKE_LINK    = @echo

TARGET        = etc

win32:target.path    = $$prefix/
unix:target.path     = $$prefix/share/libfwbuilder-2.1/
macx:target.path     = $$prefix/

dtd.files            = fwbuilder.dtd

INSTALLS   -= target
INSTALLS   += dtd

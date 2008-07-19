#-*- mode: makefile; tab-width: 4; -*-
#

include(../../qmake.inc)

TEMPLATE = app

win32 {
  CONFIG -= embed_manifest_exe
  CONFIG -= debug
  QMAKE_RUN_CC  = echo
  QMAKE_RUN_CXX = echo
  QMAKE_LINK    = echo
}

!win32 {
  QMAKE_RUN_CC  = @echo > /dev/null
  QMAKE_RUN_CXX = @echo > /dev/null
  QMAKE_LINK    = @echo > /dev/null
}

TARGET = res

win32:target.path = $$PREFIX/
unix:target.path = $$PREFIX/share/fwbuilder/
macx:target.path = $$PREFIX/

res.files = objects_init.xml templates.xml resources.xml
res_os.files = os/*.xml
res_platform.files = platform/*.xml

INSTALLS -= target
INSTALLS += res 
INSTALLS += res_os
INSTALLS += res_platform

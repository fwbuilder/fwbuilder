# -*- mode: makefile; tab-width: 4; -*-
# $Id$
TEMPLATE = app
LANGUAGE = C++
QT += network
TARGET = fwbuilder
include(../../qmake.inc)
exists(qmake.inc):include( qmake.inc)

SOURCES += main.cpp

# Arrange static libraries before dynamic ones in the linker command
# line.  libgui goes first

IMPORT_LIB = ../import/$$BINARY_SUBDIR/libimport.a
FWBPARSER_LIB = ../parsers/$$BINARY_SUBDIR/libfwbparser.a
# FWTRANSFER_LIB = ../fwtransfer/$$BINARY_SUBDIR/libfwtransfer.a

INCLUDEPATH += $$ANTLR_INCLUDEPATH
DEFINES += $$ANTLR_DEFINES

STATIC_LIBS += ../libgui/$$BINARY_SUBDIR/libgui.a \
				$$IMPORT_LIB $$FWBPARSER_LIB $$ANTLR_LIBS

# fwtransfer lib. Add this before adding -lQtDBus to LIBS below
# STATIC_LIBS += $$FWTRANSFER_LIB

# contains( HAVE_QTDBUS, 1 ):unix { 
#     !macx:QT += network \
#         dbus
#     macx:STATIC_LIBS += -framework \
#         QtDBus
# }

# !macx:STATIC_LIBS += -lQtDBus # workaround for QT += dbus not working with Qt < 4.4.0

INCLUDEPATH +=  \
	../libgui \
	../compiler_lib \
	../libfwbuilder/src

win32:INCLUDEPATH += ../libgui/ui
!win32:INCLUDEPATH += ../libgui/.ui

DEPENDPATH +=  \
	../libgui \
	../compiler_lib \
	../libfwbuilder/src

OTHER_LIBS  = ../common/$$BINARY_SUBDIR/libcommon.a \
      ../iptlib/$$BINARY_SUBDIR/libiptlib.a \
      ../pflib/$$BINARY_SUBDIR/libfwbpf.a \
      ../cisco_lib/$$BINARY_SUBDIR/libfwbcisco.a \
      ../compiler_lib/$$BINARY_SUBDIR/libcompilerdriver.a \
      ../libfwbuilder/src/fwcompiler/$$BINARY_SUBDIR/libfwcompiler.a \
      ../libfwbuilder/src/fwbuilder/$$BINARY_SUBDIR/libfwbuilder.a

STATIC_LIBS += $$OTHER_LIBS

PRE_TARGETDEPS  = $$STATIC_LIBS

macx:STATIC_LIBS += -framework Carbon
STATIC_LIBS += $$LIBS_FWCOMPILER

LIBS = $$STATIC_LIBS $$LIBS

win32 {
    RC_FILE = fwbuilder-windows.rc
	win_ico.files = fwbuilder-windows.ico
	win_ico.path = $$PREFIX
	INSTALLS += win_ico
}

macx {
    QMAKE_INFO_PLIST = FwbuilderInfo.plist
    ICON = fwbuilder-macosx.icns
	SOURCES += main_mac.cpp
}

# TRANSLATIONS = fwbuilder_ru.ts fwbuilder_ja.ts fwbuilder_en.ts
# ja.path = $$PKGLOCALEDIR
# ja.files = fwbuilder_ja.qm
# ru.path = $$PKGLOCALEDIR
# ru.files = fwbuilder_ru.qm
# INSTALLS += ja ru


#-*- mode: makefile; tab-width: 4; -*-
#
######### fwbuilder/files/qmake2.inc
#


win32 {
    # Keep this as a absolute full path. For some reason, when
    # I leave it is up to qmake to define this variable, and it
    # makes it "xcopy /i /y /s /q", I get an error "Invalid number of parameters"
    # even though command line looks correct. Chances are, there is another
    # xcopy somewhere on the system that gets used, but I could not find it.
    # Setting this variable using full path works.
    QMAKE_COPY_DIR = C:\\Windows\\System32\\xcopy /I /Y /S /Q

    PREFIX = "c:/tmp/build_root"
    DOCDIR = "$$PREFIX/doc/fwbuilder"
    exec_prefix = "$$PREFIX"
    MANDIR = "$$PREFIX/man/"
    DESTDIR = ""

    doc.path    = $$DOCDIR

    target.path = $$PREFIX/
    res.path = $$PREFIX/resources
    res_os.path = $$res.path/os
    res_platform.path = $$res.path/platform
    res_help_C.path = $$res.path/help/C
    res_help_en_US.path = $$res.path/help/en_US
    res_configlets.path = $$res.path/configlets

    PKGLOCALEDIR = $$res.path/locale

    CONFIG += qt thread rtti stl warn_on release
    CONFIG -= debug

    BINARY_SUBDIR = release

    DEFINES += LIBXML_STATIC LIBXSLT_STATIC
    DEFINES += LIBEXSLT_STATIC XMLSEC_STATIC

    INCLUDEPATH += "../.."

    INCLUDEPATH += c:/local/include c:/local/include/libxml2 c:/MinGW/include

   LIBS += -L "c:/local/lib"
   LIBS += -l xslt -l xml2 -l iconv -l pthread -l netsnmp -l ws2_32

# workaround for QT += dbus not working atm
# Standard QT binary Windows distribution does not include QtDBus library
# Checked with 4.4.1 and 4.4.3 06/12/2009
#    LIBS += -lQtDBus

    target.path = $$PREFIX/bin

# on windows we use antlr DLL
    HAVE_ANTLR_RUNTIME = 1
    HAVE_EXTERNAL_ANTLR = 0
    antlr.path = $$PREFIX/lib

# Qt on windows does not have DBus framework support, turn parts of .pro files off
#    HAVE_QTDBUS = 

    ANTLR_INCLUDEPATH = ..
    ANTLR_LIBS = ../antlr/release/libantlr.a

# our parsers are linked as a library (code is in src/parsers)
    FWBPARSER_LIB = ../parsers/release/libfwbparser.a

# fwtransfer library for secuwall (and possibly other os/platform)
# override the name for windows
#    FWTRANSFER_LIB = ../fwtransfer/release/libfwtransfer.a
}

macx {

//	PREFIX = "$$DESTDIR"
	DOCDIR = "$$PREFIX/doc/"
	MANDIR = "$$PREFIX/share/man"

    # These do not really matter because we never run make install on Mac OS X
    target.path = $$PREFIX/
    res.path = $$PREFIX/resources/
    res_os.path = $$res.path/os
    res_platform.path = $$res.path/platform
    res_help_C.path = $$res.path/help/C
    res_help_en_US.path = $$res.path/help/en_US
    res_configlets.path = $$res.path/configlets

	icns.files = fwbuilder.icns
	icns.path = $$PREFIX/

	PKGLOCALEDIR = $$res.path/locale

# If I build on 10.6 without these, generated binary depends on
# /usr/lib/libxml2.2.dynlib that has compatibility version 10.0.0
# which does not work on 10.5 where its compatibility version is
# 9.0.0. Chances are, the same thing may be happening with other
# dependency libraries. Building on 10.6 with deployment target 10.5
# should solve the problem
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
	QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk

	CONFIG += qt thread rtti stl warn_on debug  # release

# not compiling for PPC because of problems on Snow Leopard
	CONFIG += x86 

# See http://trolltech.com/developer/task-tracker/index_html?method=entry&id=211852
# for why ppc architecture is commented out.
# If Trolltech does not fix it by the time of release v3.0, will have
# to figure out workaround (turn off precompiled headers ?)
#	CONFIG += ppc

	INCLUDEPATH += $(INSTALL_ROOT)/$$PREFIX/include/
	INCLUDEPATH += /usr/include/libxml2 /usr/include

	QMAKE_LIBS_QT =
	QMAKE_LIBS_QT_THREAD =

    # LIBS += $$LIBS_FWBUILDER
    LIBS += -lxml2 -lz
    LIBS += -lpthread -lssl -lcrypto

}


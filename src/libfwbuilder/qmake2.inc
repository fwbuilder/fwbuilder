#-*- mode: makefile; tab-width: 4; -*-
#
######### libfwbuilder/files/qmake2.inc
#


win32 {
    prefix = "c:/tmp/build_root"
    DOCDIR = "$$prefix/doc/"
    exec_prefix = "$$prefix"
    DESTDIR = ""

    target.path = "$$prefix/lib"
    dtd.path = "$$prefix/resources"
    migration.path = "$$prefix/resources/migration"
    doc.path = "$$prefix/doc/libfwbuilder"

    CONFIG -= debug
    CONFIG += thread rtti stl warn_on release
    CONFIG += staticlib

    DEFINES += LIBXML_STATIC LIBXSLT_STATIC 
    INCLUDEPATH += ../../../fwb-lm/include
    INCLUDEPATH += c:/local/include c:/local/include/libxml2 c:/MinGW/include
    LIBS += -LC:\local\lib
#    LIBS += ws2_32.lib pthreadVC.lib 
    LIBS += pthreadVC.lib 
    LIBS += libxml2.lib libxslt_a.lib libz.a  # zlib.lib
}

macx {
    prefix = "$$prefix"
    DOCDIR = "$$prefix/doc/"
    exec_prefix = "$$prefix"
    DESTDIR =

# these path definitions are not used since we do not run make install on Mac
# (see fwbuilder.def file where it is defined that we don't do that)
# But I need to assign these variables some value to make qmake happy.
    target.path = "$$prefix/lib"
    dtd.path = "$$prefix/"
    migration.path = "$$prefix/migration"
    doc.path = "$$prefix/doc/"

    CONFIG += x86 

# If I build on 10.6 without these, generated binary depends on
# /usr/lib/libxml2.2.dynlib that has compatibility version 10.0.0
# which does not work on 10.5 where its compatibility version is
# 9.0.0. Chances are, the same thing may be happening with other
# dependency libraries. Building on 10.6 with deployment target 10.5
# should solve the problem
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk

    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
}


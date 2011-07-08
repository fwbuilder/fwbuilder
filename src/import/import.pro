#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = lib
#

SOURCES = QStringListOperators.cpp \
	      PreImport.cpp \
          objectMaker.cpp \
		  objectSignature.cpp \
          addressObjectMaker.cpp \
          serviceObjectMaker.cpp \
          getProtoByName.cpp \
          getServByName.cpp \
          Importer.cpp \
          IOSImporter.cpp \
          IOSImporterRun.cpp \
          IPTImporter.cpp \
          IPTImporterRun.cpp \
          PIXImporter.cpp \
          PIXImporterNat.cpp \
          PIXImporterRun.cpp \
		  PFImporter.cpp \
          PFImporterRun.cpp \

HEADERS = QStringListOperators.h \
	      PreImport.h \
          objectMaker.h \
		  objectSignature.h \
          addressObjectMaker.h \
          serviceObjectMaker.h \
          getProtoByName.h \
          getServByName.h \
          Importer.h \
          IOSImporter.h \
          IPTImporter.h \
          PIXImporter.h \
		  PFImporter.h \
		  InterfaceSpec.h \
		  AddressSpec.h \
		  PortSpec.h \
		  IcmpSpec.h \
		  RouteSpec.h \


CONFIG += staticlib

# need to add "." for g++ on mingw
INCLUDEPATH += $$ANTLR_INCLUDEPATH ../libfwbuilder/src ../compiler_lib .
DEPENDPATH  += $$ANTLR_INCLUDEPATH ../libfwbuilder/src ../compiler_lib

LIBS += $$ANTLR_LIBS
DEFINES += $$ANTLR_DEFINES

TARGET   = import

INSTALLS -= target

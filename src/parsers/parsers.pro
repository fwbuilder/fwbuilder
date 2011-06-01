#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = lib
#

SOURCES	 =  IOSCfgLexer.cpp  \
			IOSCfgParser.cpp \
			IPTCfgLexer.cpp  \
			IPTCfgParser.cpp \
			PIXCfgLexer.cpp  \
			PIXCfgParser.cpp \
			PFCfgLexer.cpp  \
			PFCfgParser.cpp \
			IfconfigLinuxCfgLexer.cpp  \
			IfconfigLinuxCfgParser.cpp \
			IfconfigBSDCfgLexer.cpp  \
			IfconfigBSDCfgParser.cpp

HEADERS	 = ../../config.h    \
			IOSCfgLexer.hpp  \
			IOSCfgParser.hpp \
			IOSCfgParserTokenTypes.hpp \
			IPTCfgLexer.hpp  \
			IPTCfgParser.hpp \
			IPTCfgParserTokenTypes.hpp \
			PIXCfgLexer.hpp  \
			PIXCfgParser.hpp \
			PIXCfgParserTokenTypes.hpp \
			PFCfgLexer.hpp  \
			PFCfgParser.hpp \
			PFCfgParserTokenTypes.hpp \
			IfconfigLinuxCfgLexer.hpp  \
			IfconfigLinuxCfgParser.hpp \
			IfconfigLinuxCfgParserTokenTypes.hpp \
			IfconfigBSDCfgLexer.hpp  \
			IfconfigBSDCfgParser.hpp \
			IfconfigBSDCfgParserTokenTypes.hpp

CONFIG += staticlib

INCLUDEPATH += $$ANTLR_INCLUDEPATH ../import ../libfwbuilder/src
DEPENDPATH  += $$ANTLR_INCLUDEPATH ../import ../libfwbuilder/src

LIBS += $$ANTLR_LIBS
DEFINES += $$ANTLR_DEFINES

TARGET   = fwbparser

INSTALLS -= target

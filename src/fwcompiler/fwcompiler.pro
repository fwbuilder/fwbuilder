#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
VERSION = $$SO_VERSION
#
SOURCES  = 	BaseCompiler.cpp \
			Compiler.cpp \
			Compiler_helpers.cpp \
			Compiler_ops.cpp \
			Compiler_object_match.cpp \
			Preprocessor.cpp \
			NATCompiler.cpp \
			OSConfigurator.cpp \
			PolicyCompiler.cpp \
			RoutingCompiler.cpp

HEADERS  = 	BaseCompiler.h \
			Compiler.h \
			Preprocessor.h \
			NATCompiler.h \
			OSConfigurator.h \
			PolicyCompiler.h \
			RuleProcessor.h \
			RoutingCompiler.h

headers.files = $$HEADERS
headers.path  = "$$prefix/include/fwb-3/fwcompiler"

unix {
        LIBS += -L../fwbuilder -lfwbuilder
}

TARGET    = fwcompiler
# target.path = "$$prefix/lib"

INSTALLS += headers

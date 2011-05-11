#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = lib
#
SOURCES	 =  ANTLRUtil.cpp \
			ASTFactory.cpp \
			ASTNULLType.cpp \
			ASTRefCount.cpp \
			BaseAST.cpp \
			BitSet.cpp \
			CharBuffer.cpp \
			CharScanner.cpp \
			CommonAST.cpp \
			CommonASTWithHiddenTokens.cpp \
			CommonHiddenStreamToken.cpp \
			CommonToken.cpp \
			InputBuffer.cpp \
			LLkParser.cpp \
			MismatchedCharException.cpp \
			MismatchedTokenException.cpp \
			NoViableAltException.cpp \
			NoViableAltForCharException.cpp \
			Parser.cpp \
			RecognitionException.cpp \
			String.cpp \
			TokenBuffer.cpp \
			Token.cpp \
			TokenRefCount.cpp \
			TokenStreamBasicFilter.cpp \
			TokenStreamHiddenTokenFilter.cpp \
			TokenStreamRewriteEngine.cpp \
			TokenStreamSelector.cpp \
			TreeParser.cpp

#			dll.cpp \


HEADERS	 = ANTLRException.hpp \
			ANTLRUtil.hpp \
			ASTArray.hpp \
			ASTFactory.hpp \
			AST.hpp \
			ASTNULLType.hpp \
			ASTPair.hpp \
			ASTRefCount.hpp \
			BaseAST.hpp \
			BitSet.hpp \
			CharBuffer.hpp \
			CharInputBuffer.hpp \
			CharScanner.hpp \
			CharStreamException.hpp \
			CharStreamIOException.hpp \
			CircularQueue.hpp \
			CommonAST.hpp \
			CommonASTWithHiddenTokens.hpp \
			CommonHiddenStreamToken.hpp \
			CommonToken.hpp \
			config.hpp \
			InputBuffer.hpp \
			IOException.hpp \
			LexerSharedInputState.hpp \
			LLkParser.hpp \
			MismatchedCharException.hpp \
			MismatchedTokenException.hpp \
			NoViableAltException.hpp \
			NoViableAltForCharException.hpp \
			Parser.hpp \
			ParserSharedInputState.hpp \
			RecognitionException.hpp \
			RefCount.hpp \
			SemanticException.hpp \
			String.hpp \
			TokenBuffer.hpp \
			Token.hpp \
			TokenRefCount.hpp \
			TokenStreamBasicFilter.hpp \
			TokenStreamException.hpp \
			TokenStreamHiddenTokenFilter.hpp \
			TokenStream.hpp \
			TokenStreamIOException.hpp \
			TokenStreamRecognitionException.hpp \
			TokenStreamRetryException.hpp \
			TokenStreamRewriteEngine.hpp \
			TokenStreamSelector.hpp \
			TokenWithIndex.hpp \
			TreeParser.hpp \
			TreeParserSharedInputState.hpp

CONFIG += staticlib

INCLUDEPATH  += $$ANTLR_INCLUDEPATH
DEPENDPATH   += $$ANTLR_INCLUDEPATH
DEFINES		 += $$ANTLR_DEFINES

TARGET   = antlr

INSTALLS -= target

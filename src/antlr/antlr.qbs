import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "antlr"

    Depends { name: "Qt.core" }
    Depends { name: "cpp" }

    cpp.includePaths: base.concat(['..'])

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory, '..']
    }

    files: [
        "ANTLRUtil.cpp",
        "ASTFactory.cpp",
        "ASTNULLType.cpp",
        "ASTRefCount.cpp",
        "BaseAST.cpp",
        "BitSet.cpp",
        "CharBuffer.cpp",
        "CharScanner.cpp",
        "CommonAST.cpp",
        "CommonASTWithHiddenTokens.cpp",
        "CommonHiddenStreamToken.cpp",
        "CommonToken.cpp",
        "InputBuffer.cpp",
        "LLkParser.cpp",
        "MismatchedCharException.cpp",
        "MismatchedTokenException.cpp",
        "NoViableAltException.cpp",
        "NoViableAltForCharException.cpp",
        "Parser.cpp",
        "RecognitionException.cpp",
        "String.cpp",
        "TokenBuffer.cpp",
        "Token.cpp",
        "TokenRefCount.cpp",
        "TokenStreamBasicFilter.cpp",
        "TokenStreamHiddenTokenFilter.cpp",
        "TokenStreamRewriteEngine.cpp",
        "TokenStreamSelector.cpp",
        "TreeParser.cpp"
    ]
}

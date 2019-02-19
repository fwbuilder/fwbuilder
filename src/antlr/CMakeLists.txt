project(antlr)

set(antlr_srcs
    ANTLRUtil.cpp 
    ASTFactory.cpp 
    ASTNULLType.cpp 
    ASTRefCount.cpp 
    BaseAST.cpp 
    BitSet.cpp 
    CharBuffer.cpp 
    CharScanner.cpp 
    CommonAST.cpp 
    CommonASTWithHiddenTokens.cpp 
    CommonHiddenStreamToken.cpp 
    CommonToken.cpp 
    InputBuffer.cpp 
    LLkParser.cpp 
    MismatchedCharException.cpp 
    MismatchedTokenException.cpp 
    NoViableAltException.cpp 
    NoViableAltForCharException.cpp 
    Parser.cpp 
    RecognitionException.cpp 
    String.cpp 
    TokenBuffer.cpp 
    Token.cpp 
    TokenRefCount.cpp 
    TokenStreamBasicFilter.cpp 
    TokenStreamHiddenTokenFilter.cpp 
    TokenStreamRewriteEngine.cpp 
    TokenStreamSelector.cpp 
    TreeParser.cpp)

add_library(antlr STATIC ${antlr_srcs})

target_include_directories(antlr PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/..)

target_compile_options(antlr PRIVATE ${CXX_DEFAULT_FLAGS})


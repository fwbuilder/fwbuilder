#ifndef INC_PFCfgParser_hpp_
#define INC_PFCfgParser_hpp_

#line 25 "pf.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "PFImporter.h"

#line 11 "PFCfgParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090306): "pf.g" -> "PFCfgParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "PFCfgParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 32 "pf.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class PFImporter;

#line 28 "PFCfgParser.hpp"
#line 56 "pf.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 34 "PFCfgParser.hpp"
class CUSTOM_API PFCfgParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public PFCfgParserTokenTypes
{
#line 81 "pf.g"

// additional methods and members

    public:

    std::ostream *dbg;
    PFImporter *importer;

    /// Parser error-reporting function can be overridden in subclass
    virtual void reportError(const ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex)
    {
        importer->addMessageToLog("Parser error: " + ex.toString());
        std::cerr << ex.toString() << std::endl;
    }

    /// Parser error-reporting function can be overridden in subclass
    virtual void reportError(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser error: " + s);
        std::cerr << s << std::endl;
    }

    /// Parser warning-reporting function can be overridden in subclass
    virtual void reportWarning(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser warning: " + s);
        std::cerr << s << std::endl;
    }

#line 38 "PFCfgParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	PFCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	PFCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	PFCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	PFCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	PFCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return PFCfgParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return PFCfgParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return PFCfgParser::tokenNames;
	}
	public: void cfgfile();
	public: void comment();
	public: void pass_command();
	public: void drop_command();
	public: void timeout_command();
	public: void unknown_command();
	public: void rule_extended();
	public: void single_addr();
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 88;
#else
	enum {
		NUM_TOKENS = 88
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
};

#endif /*INC_PFCfgParser_hpp_*/

#ifndef INC_IfconfigBSDCfgParser_hpp_
#define INC_IfconfigBSDCfgParser_hpp_

#line 25 "ifconfig_bsd.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "IfconfigImporter.h"

#line 11 "IfconfigBSDCfgParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20100319): "ifconfig_bsd.g" -> "IfconfigBSDCfgParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "IfconfigBSDCfgParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 32 "ifconfig_bsd.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <iostream>
#include <sstream>

class IfconfigImporter;

#line 29 "IfconfigBSDCfgParser.hpp"
#line 57 "ifconfig_bsd.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 35 "IfconfigBSDCfgParser.hpp"
class CUSTOM_API IfconfigBSDCfgParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public IfconfigBSDCfgParserTokenTypes
{
#line 82 "ifconfig_bsd.g"

// additional methods and members

    public:

    std::ostream *dbg;
    IfconfigImporter *importer;

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

#line 39 "IfconfigBSDCfgParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	IfconfigBSDCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	IfconfigBSDCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	IfconfigBSDCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	IfconfigBSDCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	IfconfigBSDCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return IfconfigBSDCfgParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return IfconfigBSDCfgParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return IfconfigBSDCfgParser::tokenNames;
	}
	public: void cfgfile();
	public: void comment();
	public: void interface_line();
	public: void hwaddr_line();
	public: void inet_address();
	public: void inet6_address();
	public: void groups();
	public: void unknown_line();
	public: void groups_list();
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
	static const int NUM_TOKENS = 73;
#else
	enum {
		NUM_TOKENS = 73
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
};

#endif /*INC_IfconfigBSDCfgParser_hpp_*/

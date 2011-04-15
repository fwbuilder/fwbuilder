#ifndef INC_IOSCfgParser_hpp_
#define INC_IOSCfgParser_hpp_

#line 27 "iosacl.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "IOSImporter.h"

#line 11 "IOSCfgParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090306): "iosacl.g" -> "IOSCfgParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "IOSCfgParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 33 "iosacl.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class IOSImporter;

#line 28 "IOSCfgParser.hpp"
#line 57 "iosacl.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 34 "IOSCfgParser.hpp"
class CUSTOM_API IOSCfgParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public IOSCfgParserTokenTypes
{
#line 74 "iosacl.g"

// additional methods and members

    public:
    
    std::ostream *dbg;
    IOSImporter *importer;

    /// Parser error-reporting function can be overridden in subclass
    virtual void reportError(const ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex)
    {
        importer->addMessageToLog("Parser error: " + ex.toString());
    }

    /// Parser error-reporting function can be overridden in subclass
    virtual void reportError(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser error: " + s);
    }

    /// Parser warning-reporting function can be overridden in subclass
    virtual void reportWarning(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser warning: " + s);
    }
#line 38 "IOSCfgParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	IOSCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	IOSCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	IOSCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	IOSCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	IOSCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return IOSCfgParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return IOSCfgParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return IOSCfgParser::tokenNames;
	}
	public: void cfgfile();
	public: void comment();
	public: void version();
	public: void hostname();
	public: void ip_commands();
	public: void intrface();
	public: void controller();
	public: void vlan();
	public: void access_list_commands();
	public: void exit();
	public: void description();
	public: void shutdown();
	public: void certificate();
	public: void quit();
	public: void unknown_command();
	public: void ip_access_list_ext();
	public: void interface_known_ip_commands();
	public: void community_list_command();
	public: void ip_unused_command();
	public: void permit_std();
	public: void deny_std();
	public: void permit_ext();
	public: void deny_ext();
	public: void remark();
	public: void rule_ext();
	public: void rule_std();
	public: void ip_protocols();
	public: void hostaddr_ext();
	public: void time_range();
	public: void fragments();
	public: void log();
	public: void icmp_spec();
	public: void xoperator();
	public: void established();
	public: void hostaddr_std();
	public: void single_port_op();
	public: void port_range();
	public: void port_spec();
	public: void access_group_by_name();
	public: void access_group_by_number();
	public: void intf_address();
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
	static const int NUM_TOKENS = 80;
#else
	enum {
		NUM_TOKENS = 80
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
	static const unsigned long _tokenSet_5_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_9;
	static const unsigned long _tokenSet_10_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_10;
	static const unsigned long _tokenSet_11_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_11;
	static const unsigned long _tokenSet_12_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_12;
	static const unsigned long _tokenSet_13_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_13;
	static const unsigned long _tokenSet_14_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_14;
};

#endif /*INC_IOSCfgParser_hpp_*/

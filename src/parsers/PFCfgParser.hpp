#ifndef INC_PFCfgParser_hpp_
#define INC_PFCfgParser_hpp_

#line 25 "pf.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "PFImporter.h"

#line 11 "PFCfgParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20100319): "pf.g" -> "PFCfgParser.hpp"$ */
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
	public: void macro_definition();
	public: void altq_command();
	public: void antispoof_command();
	public: void queue_command();
	public: void set_command();
	public: void scrub_command();
	public: void nat_command();
	public: void rdr_command();
	public: void binat_command();
	public: void pass_command();
	public: void block_command();
	public: void timeout_command();
	public: void unknown_command();
	public: void rule_extended();
	public: void direction();
	public: void logging();
	public: void quick();
	public: void intrface();
	public: void address_family();
	public: void protospec();
	public: void hosts();
	public: void filteropts();
	public: void logopts();
	public: void logopt();
	public: void proto_name();
	public: void proto_number();
	public: void proto_list();
	public: void src_hosts_part();
	public: void src_port_part();
	public: void dst_hosts_part();
	public: void dst_port_part();
	public: void host();
	public: void host_list();
	public: void filteropt();
	public: void state();
	public: void queue();
	public: void unary_op();
	public: void binary_op();
	public: void op_list();
	public: void port_def();
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
	static const int NUM_TOKENS = 127;
#else
	enum {
		NUM_TOKENS = 127
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
	static const unsigned long _tokenSet_15_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_15;
	static const unsigned long _tokenSet_16_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_16;
	static const unsigned long _tokenSet_17_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_17;
	static const unsigned long _tokenSet_18_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_18;
	static const unsigned long _tokenSet_19_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_19;
	static const unsigned long _tokenSet_20_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_20;
	static const unsigned long _tokenSet_21_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_21;
	static const unsigned long _tokenSet_22_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_22;
	static const unsigned long _tokenSet_23_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_23;
	static const unsigned long _tokenSet_24_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_24;
};

#endif /*INC_PFCfgParser_hpp_*/

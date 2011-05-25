#ifndef INC_IPTCfgParser_hpp_
#define INC_IPTCfgParser_hpp_

#line 38 "iptables.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "IPTImporter.h"

#line 11 "IPTCfgParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090306): "iptables.g" -> "IPTCfgParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "IPTCfgParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 45 "iptables.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class IPTImporter;

#line 28 "IPTCfgParser.hpp"
#line 74 "iptables.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 34 "IPTCfgParser.hpp"
class CUSTOM_API IPTCfgParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public IPTCfgParserTokenTypes
{
#line 91 "iptables.g"

// additional methods and members

    public:
    
    std::ostream *dbg;
    IPTImporter *importer;

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

#line 38 "IPTCfgParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	IPTCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	IPTCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	IPTCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	IPTCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	IPTCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return IPTCfgParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return IPTCfgParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return IPTCfgParser::tokenNames;
	}
	public: void cfgfile();
	public: void comment();
	public: void start_table();
	public: void create_chain();
	public: void add_rule();
	public: void commit();
	public: void chain_def();
	public: void ipt_option();
	public: void negation();
	public: void module();
	public: void match_mark();
	public: void match_recent();
	public: void src();
	public: void dst();
	public: void i_intf();
	public: void o_intf();
	public: void proto();
	public: void target();
	public: void fragm();
	public: void icmp_type_spec();
	public: void basic_tcp_udp_port_spec();
	public: void multiport_tcp_udp_port_spec();
	public: void tcp_options();
	public: void match_limit();
	public: void match_limit_burst();
	public: void match_length();
	public: void match_iprange_src();
	public: void match_iprange_dst();
	public: void unknown_option();
	public: void unknown_parameter();
	public: void m_state();
	public: void m_mport();
	public: void m_icmp();
	public: void m_tcp();
	public: void m_udp();
	public: void m_limit();
	public: void m_length();
	public: void m_iprange();
	public: void m_comment();
	public: void m_pkttype();
	public: void m_unknown_module();
	public: void protocol_word();
	public: void target_options();
	public: void nat_spec();
	public: void redirect_spec();
	public: void nat_addr_range();
	public: void nat_port_def_with_range();
	public: void state_word();
	public: void m_mark();
	public: void limit_rate();
	public: void m_recent();
	public: void recent_opts();
	public: void recent_args_no_param();
	public: void recent_args_param();
	public: void length_spec();
	public: void pkt_type_spec();
	public: void port_def_with_range();
	public: void port_def_no_range();
	public: void port_def_with_incomplete_range();
	public: void syn();
	public: void tcp_flags();
	public: void tcp_option();
	public: void tcp_flag_word();
	public: void tcp_flags_list();
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
	static const int NUM_TOKENS = 146;
#else
	enum {
		NUM_TOKENS = 146
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

#endif /*INC_IPTCfgParser_hpp_*/

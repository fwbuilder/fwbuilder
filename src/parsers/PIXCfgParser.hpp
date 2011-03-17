#ifndef INC_PIXCfgParser_hpp_
#define INC_PIXCfgParser_hpp_

#line 25 "pix.g"

    // gets inserted before antlr generated includes in the header
    // file

#line 10 "PIXCfgParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090306): "pix.g" -> "PIXCfgParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "PIXCfgParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 30 "pix.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class PIXImporter;

#line 27 "PIXCfgParser.hpp"
#line 56 "pix.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 33 "PIXCfgParser.hpp"
class CUSTOM_API PIXCfgParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public PIXCfgParserTokenTypes
{
#line 73 "pix.g"

// additional methods and members

    public:
    
    std::ostream *dbg;
    PIXImporter *importer;
#line 37 "PIXCfgParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	PIXCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	PIXCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	PIXCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	PIXCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	PIXCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return PIXCfgParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return PIXCfgParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return PIXCfgParser::tokenNames;
	}
	public: void cfgfile();
	public: void comment();
	public: void version();
	public: void hostname();
	public: void ip_commands();
	public: void intrface();
	public: void vlan_interface();
	public: void switchport();
	public: void shutdown();
	public: void sec_level();
	public: void nameif();
	public: void controller();
	public: void access_list_commands();
	public: void exit();
	public: void certificate();
	public: void quit();
	public: void names_section();
	public: void name_entry();
	public: void named_object_network();
	public: void named_object_service();
	public: void crypto();
	public: void unknown_command();
	public: void ip_access_list_ext();
	public: void community_list_command();
	public: void named_object_network_parameters();
	public: void named_object_nat();
	public: void named_object_description();
	public: void host_addr();
	public: void range_addr();
	public: void subnet_addr();
	public: void named_object_service_parameters();
	public: void service_icmp();
	public: void service_icmp6();
	public: void service_tcp_udp();
	public: void service_other();
	public: void src_port_spec();
	public: void dst_port_spec();
	public: void xoperator();
	public: void permit_ext();
	public: void deny_ext();
	public: void remark();
	public: void rule_ext();
	public: void ip_protocols();
	public: void hostaddr_ext();
	public: void time_range();
	public: void fragments();
	public: void log();
	public: void icmp_spec();
	public: void established();
	public: void single_port_op();
	public: void port_range();
	public: void port_spec();
	public: void pair_of_ports_spec();
	public: void hostaddr_std();
	public: void interface_parameters();
	public: void intf_address();
	public: void interface_description();
	public: void interface_known_commands();
	public: void v6_ip_address();
	public: void v7_ip_address();
	public: void v6_dhcp_address();
	public: void v6_static_address();
	public: void v7_dhcp_address();
	public: void v7_static_address();
	public: void access_group_by_name();
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
	static const int NUM_TOKENS = 101;
#else
	enum {
		NUM_TOKENS = 101
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
};

#endif /*INC_PIXCfgParser_hpp_*/

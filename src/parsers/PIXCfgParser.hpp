#ifndef INC_PIXCfgParser_hpp_
#define INC_PIXCfgParser_hpp_

#line 25 "pix.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "PIXImporter.h"

#line 11 "PIXCfgParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090306): "pix.g" -> "PIXCfgParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "PIXCfgParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 32 "pix.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class PIXImporter;

#line 28 "PIXCfgParser.hpp"
#line 56 "pix.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 34 "PIXCfgParser.hpp"
class CUSTOM_API PIXCfgParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public PIXCfgParserTokenTypes
{
#line 81 "pix.g"

// additional methods and members

    public:

    std::ostream *dbg;
    PIXImporter *importer;

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

#line 38 "PIXCfgParser.hpp"
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
	public: void community_list_command();
	public: void unknown_ip_command();
	public: void intrface();
	public: void nameif_top_level();
	public: void intf_address();
	public: void controller();
	public: void access_list_commands();
	public: void ssh_command();
	public: void telnet_command();
	public: void http_command();
	public: void icmp_top_level_command();
	public: void nat_top_level_command();
	public: void global_top_level_command();
	public: void static_top_level_command();
	public: void access_group();
	public: void exit();
	public: void certificate();
	public: void quit();
	public: void names_section();
	public: void name_entry();
	public: void named_object_network();
	public: void named_object_service();
	public: void object_group_network();
	public: void object_group_service();
	public: void object_group_protocol();
	public: void object_group_icmp_8_0();
	public: void object_group_icmp_8_3();
	public: void crypto();
	public: void no_commands();
	public: void timeout_command();
	public: void dns_command();
	public: void service_top_level_command();
	public: void pim_top_level_command();
	public: void network_top_level_command();
	public: void unknown_command();
	public: void name_entry_ipv4();
	public: void name_entry_ipv6();
	public: void ip_protocol_names();
	public: void named_object_nat();
	public: void named_object_description();
	public: void named_object_network_parameters();
	public: void host_addr();
	public: void range_addr();
	public: void subnet_addr();
	public: void single_addr();
	public: void named_object_service_parameters();
	public: void service_icmp();
	public: void service_icmp6();
	public: void service_tcp_udp();
	public: void service_other();
	public: void service_unknown();
	public: void icmp_names();
	public: void src_port_spec();
	public: void dst_port_spec();
	public: void xoperator();
	public: void object_group_network_parameters();
	public: void object_group_description();
	public: void group_object();
	public: void network_object();
	public: void object_group_protocol_parameters();
	public: void protocol_object();
	public: void object_group_icmp_parameters();
	public: void icmp_object();
	public: void object_group_service_parameters();
	public: void service_object();
	public: void port_object();
	public: void permit_extended();
	public: void deny_extended();
	public: void permit_standard();
	public: void deny_standard();
	public: void remark();
	public: void rule_extended();
	public: void rule_standard();
	public: void hostaddr_expr();
	public: void ip_protocols();
	public: void time_range();
	public: void fragments();
	public: void log();
	public: void icmp_spec();
	public: void tcp_udp_rule_extended();
	public: void hostaddr_expr_1();
	public: void acl_tcp_udp_dst_port_spec();
	public: void hostaddr_expr_2();
	public: void acl_xoperator_dst();
	public: void established();
	public: void hostaddr_expr_3();
	public: void single_port_op();
	public: void port_range();
	public: void port_spec();
	public: void tcp_udp_port_spec();
	public: void pair_of_ports_spec();
	public: void tcp_udp_port_names();
	public: void interface_label();
	public: void interface_command_6();
	public: void interface_command_7();
	public: void pix6_interface_hw_speed();
	public: void interface_parameters();
	public: void vlan_interface();
	public: void sec_level();
	public: void nameif();
	public: void interface_description();
	public: void switchport();
	public: void shutdown();
	public: void interface_no_commands();
	public: void unsupported_interface_commands();
	public: void v6_ip_address();
	public: void v7_ip_address();
	public: void v6_dhcp_address();
	public: void v6_static_address();
	public: void v7_dhcp_address();
	public: void v7_static_address();
	public: void icmp_types_for_icmp_command();
	public: void nat_old_top_level_command();
	public: void nat_new_top_level_command();
	public: void nat_addr_match();
	public: void nat_command_last_parameters();
	public: void nat_and_static_command_common_last_parameters();
	public: void static_starts_with_hostaddr();
	public: void static_starts_with_tcp_udp();
	public: void static_mapped_addr_match();
	public: void static_real_addr_match();
	public: void static_command_common_last_parameters();
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
	static const int NUM_TOKENS = 197;
#else
	enum {
		NUM_TOKENS = 197
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
	static const unsigned long _tokenSet_25_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_25;
	static const unsigned long _tokenSet_26_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_26;
	static const unsigned long _tokenSet_27_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_27;
	static const unsigned long _tokenSet_28_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_28;
	static const unsigned long _tokenSet_29_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_29;
	static const unsigned long _tokenSet_30_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_30;
	static const unsigned long _tokenSet_31_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_31;
	static const unsigned long _tokenSet_32_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_32;
	static const unsigned long _tokenSet_33_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_33;
	static const unsigned long _tokenSet_34_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_34;
	static const unsigned long _tokenSet_35_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_35;
	static const unsigned long _tokenSet_36_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_36;
	static const unsigned long _tokenSet_37_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_37;
	static const unsigned long _tokenSet_38_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_38;
	static const unsigned long _tokenSet_39_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_39;
	static const unsigned long _tokenSet_40_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_40;
	static const unsigned long _tokenSet_41_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_41;
	static const unsigned long _tokenSet_42_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_42;
	static const unsigned long _tokenSet_43_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_43;
	static const unsigned long _tokenSet_44_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_44;
};

#endif /*INC_PIXCfgParser_hpp_*/

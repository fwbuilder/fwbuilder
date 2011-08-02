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

#include <iostream>
#include <sstream>

class PFImporter;

#line 29 "PFCfgParser.hpp"
#line 57 "pf.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 35 "PFCfgParser.hpp"
class CUSTOM_API PFCfgParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public PFCfgParserTokenTypes
{
#line 82 "pf.g"

// additional methods and members

    public:

    std::ostream *dbg;
    PFImporter *importer;

    /// Parser error-reporting function can be overridden in subclass
    virtual void reportError(const ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex)
    {
        importer->addMessageToLog("Parser error: " + ex.toString());
        importer->error_tracker->registerError("Parser error: " + ex.toString());
        std::cerr << ex.toString() << std::endl;
    }

    /// Parser error-reporting function can be overridden in subclass
    virtual void reportError(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser error: " + s);
        importer->error_tracker->registerError("Parser error: " + s);
        std::cerr << s << std::endl;
    }

    /// Parser warning-reporting function can be overridden in subclass
    virtual void reportWarning(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser warning: " + s);
        importer->error_tracker->registerError("Parser warning: " + s);
        std::cerr << s << std::endl;
    }

#line 39 "PFCfgParser.hpp"
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
	public: void include_command();
	public: void macro_definition();
	public: void altq_rule();
	public: void antispoof_rule();
	public: void queue_rule();
	public: void set_rule();
	public: void scrub_rule();
	public: void match_rule();
	public: void table_rule();
	public: void no_nat_rule();
	public: void nat_rule();
	public: void rdr_rule();
	public: void binat_rule();
	public: void pass_rule();
	public: void block_rule();
	public: void set_timeout();
	public: void set_ruleset_optimization();
	public: void set_optimization();
	public: void set_limit();
	public: void set_loginterface();
	public: void set_block_policy();
	public: void set_state_policy();
	public: void set_state_defaults();
	public: void set_require_order();
	public: void set_fingerprints();
	public: void set_skip();
	public: void set_debug();
	public: void set_reassemble();
	public: void set_hostid();
	public: void timeout_def();
	public: void timeout_def_list();
	public: void limit_def();
	public: void limit_def_list();
	public: void skip_def();
	public: void skip_list();
	public: void rule_extended();
	public: void tableaddr_spec();
	public: void logging();
	public: void intrface();
	public: void address_family();
	public: void protospec();
	public: void hosts();
	public: void tagged();
	public: void tag_clause();
	public: void redirhost();
	public: void redirhost_list();
	public: void portspec();
	public: void pooltype();
	public: void port_def();
	public: void block_return();
	public: void icmp_code_by_name();
	public: void direction();
	public: void quick_or_log();
	public: void route();
	public: void filteropts();
	public: void logopts();
	public: void logopt();
	public: void ifspec();
	public: void interface_list();
	public: void proto_def();
	public: void proto_name();
	public: void proto_number();
	public: void proto_list();
	public: void hosts_from();
	public: void hosts_to();
	public: void src_hosts_part();
	public: void src_port_part();
	public: void dst_hosts_part();
	public: void dst_port_part();
	public: void common_hosts_part();
	public: void host();
	public: void host_list();
	public: void route_to();
	public: void reply_to();
	public: void dup_to();
	public: void routehost();
	public: void routehost_list();
	public: void filteropt();
	public: void user_match();
	public: void group_match();
	public: void tcp_flags();
	public: void icmp_type();
	public: void icmp6_type();
	public: void state();
	public: void queue();
	public: void label();
	public: void match_rule_scrub_options();
	public: void scrub_options();
	public: void user_group_op();
	public: void user_group_op_list();
	public: void unary_op();
	public: void binary_op();
	public: void scrub_option();
	public: void scrub_option_list();
	public: void icmp_type_code();
	public: void icmp_list();
	public: void icmp_type_by_name();
	public: void port_op();
	public: void port_op_list();
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
	static const int NUM_TOKENS = 262;
#else
	enum {
		NUM_TOKENS = 262
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
	static const unsigned long _tokenSet_45_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_45;
	static const unsigned long _tokenSet_46_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_46;
	static const unsigned long _tokenSet_47_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_47;
	static const unsigned long _tokenSet_48_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_48;
	static const unsigned long _tokenSet_49_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_49;
	static const unsigned long _tokenSet_50_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_50;
	static const unsigned long _tokenSet_51_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_51;
	static const unsigned long _tokenSet_52_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_52;
	static const unsigned long _tokenSet_53_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_53;
	static const unsigned long _tokenSet_54_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_54;
	static const unsigned long _tokenSet_55_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_55;
	static const unsigned long _tokenSet_56_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_56;
	static const unsigned long _tokenSet_57_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_57;
	static const unsigned long _tokenSet_58_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_58;
	static const unsigned long _tokenSet_59_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_59;
	static const unsigned long _tokenSet_60_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_60;
	static const unsigned long _tokenSet_61_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_61;
	static const unsigned long _tokenSet_62_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_62;
};

#endif /*INC_PFCfgParser_hpp_*/

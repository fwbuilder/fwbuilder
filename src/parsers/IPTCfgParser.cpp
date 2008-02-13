/* $ANTLR 2.7.4: "iptables.g" -> "IPTCfgParser.cpp"$ */
#line 42 "iptables.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "IPTCfgParser.cpp"
#include "IPTCfgParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 48 "iptables.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#include "../gui/IPTImporter.h"
#include "fwbuilder/TCPService.h"

#include <algorithm>


#line 26 "IPTCfgParser.cpp"
#line 1 "iptables.g"
#line 28 "IPTCfgParser.cpp"
IPTCfgParser::IPTCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

IPTCfgParser::IPTCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
}

IPTCfgParser::IPTCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

IPTCfgParser::IPTCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
}

IPTCfgParser::IPTCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
}

void IPTCfgParser::cfgfile() {
	
	try {      // for error handling
		{ // ( ... )+
		int _cnt3=0;
		for (;;) {
			switch ( LA(1)) {
			case LINE_COMMENT:
			{
				comment();
				break;
			}
			case STAR:
			{
				start_table();
				break;
			}
			case COLON:
			{
				create_chain();
				break;
			}
			case ADD_RULE:
			{
				add_rule();
				break;
			}
			case COMMIT:
			{
				commit();
				break;
			}
			case NEWLINE:
			{
				match(NEWLINE);
				break;
			}
			default:
			{
				if ( _cnt3>=1 ) { goto _loop3; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt3++;
		}
		_loop3:;
		}  // ( ... )+
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_0);
	}
}

void IPTCfgParser::comment() {
	
	try {      // for error handling
		match(LINE_COMMENT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
}

void IPTCfgParser::start_table() {
	
	try {      // for error handling
		match(STAR);
		match(WORD);
#line 123 "iptables.g"
		
		importer->current_table = LT(0)->getText();
		*dbg << "TABLE " << LT(0)->getText() << std::endl;
		
#line 130 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
}

void IPTCfgParser::create_chain() {
	
	try {      // for error handling
		match(COLON);
		chain_def();
#line 142 "iptables.g"
		
		importer->newUnidirRuleSet(LT(0)->getText());
		*dbg << "NEW CHAIN " << LT(0)->getText() << std::endl;
		
#line 149 "IPTCfgParser.cpp"
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
			break;
		}
		case MINUS:
		{
			match(MINUS);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		{
		switch ( LA(1)) {
		case OPENING_SQUARE:
		{
			match(OPENING_SQUARE);
			match(INT_CONST);
			match(COLON);
			match(INT_CONST);
			match(CLOSING_SQUARE);
			break;
		}
		case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
		case NEWLINE:
		case LINE_COMMENT:
		case COMMIT:
		case STAR:
		case COLON:
		case ADD_RULE:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
}

void IPTCfgParser::add_rule() {
	
	try {      // for error handling
		match(ADD_RULE);
		chain_def();
#line 153 "iptables.g"
		
		// push previous rule
		*dbg << std::endl;
		importer->pushRule();
		// start new one
		if (importer->current_table=="nat")
		importer->newNATRule();
		else
		importer->newPolicyRule();
		importer->current_chain = LT(0)->getText();
		*dbg << "add_rule: line=" << LT(0)->getLine()
		<< " chain=" << LT(0)->getText();
		
#line 222 "IPTCfgParser.cpp"
		{ // ( ... )+
		int _cnt14=0;
		for (;;) {
			if ((_tokenSet_2.member(LA(1)))) {
				ipt_option();
			}
			else {
				if ( _cnt14>=1 ) { goto _loop14; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt14++;
		}
		_loop14:;
		}  // ( ... )+
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
}

void IPTCfgParser::commit() {
	
	try {      // for error handling
		match(COMMIT);
#line 111 "iptables.g"
		
		// push last rule
		importer->pushRule();
		*dbg << " COMMIT" << std::endl;
		// clear current table
		importer->current_table = "";
		
#line 258 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
}

void IPTCfgParser::chain_def() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case INPUT:
		{
			match(INPUT);
			break;
		}
		case FORWARD:
		{
			match(FORWARD);
			break;
		}
		case OUTPUT:
		{
			match(OUTPUT);
			break;
		}
		case PREROUTING:
		{
			match(PREROUTING);
			break;
		}
		case POSTROUTING:
		{
			match(POSTROUTING);
			break;
		}
		case WORD:
		{
			match(WORD);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void IPTCfgParser::ipt_option() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case OPT_MODULE:
		{
			module();
			break;
		}
		case OPT_SRC:
		{
			src();
			break;
		}
		case OPT_DST:
		{
			dst();
			break;
		}
		case OPT_IN_INTF:
		{
			i_intf();
			break;
		}
		case OPT_OUT_INTF:
		{
			o_intf();
			break;
		}
		case OPT_PROTO:
		{
			proto();
			break;
		}
		case OPT_TARGET:
		{
			target();
			break;
		}
		case OPT_FRAGM:
		{
			fragm();
			break;
		}
		case MATCH_ICMP_TYPE:
		{
			icmp_type_spec();
			break;
		}
		case MATCH_SRC_PORT:
		case MATCH_SRC_PORT_SHORT:
		case MATCH_DST_PORT:
		case MATCH_DST_PORT_SHORT:
		{
			basic_tcp_udp_port_spec();
			break;
		}
		case MATCH_SRC_MULTIPORT:
		case MATCH_SRC_MULTIPORT_SHORT:
		case MATCH_DST_MULTIPORT:
		case MATCH_DST_MULTIPORT_SHORT:
		{
			multiport_tcp_udp_port_spec();
			break;
		}
		case EXCLAMATION:
		case MATCH_SYN:
		case MATCH_TCP_FLAGS:
		case MATCH_TCP_OPTION:
		{
			tcp_options();
			break;
		}
		case MATCH_MARK:
		{
			match_mark();
			break;
		}
		case MATCH_LIMIT:
		{
			match_limit();
			break;
		}
		case MATCH_LIMIT_BURST:
		{
			match_limit_burst();
			break;
		}
		case MINUS:
		case UNSUPPORTED_OPTION:
		{
			unknown_option();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::module() {
	
	try {      // for error handling
		match(OPT_MODULE);
		{
		switch ( LA(1)) {
		case M_STATE:
		{
			m_state();
			break;
		}
		case M_MPORT:
		{
			m_mport();
			break;
		}
		case ICMP:
		{
			m_icmp();
			break;
		}
		case TCP:
		{
			m_tcp();
			break;
		}
		case UDP:
		{
			m_udp();
			break;
		}
		case M_MARK:
		{
			m_mark();
			break;
		}
		case M_LIMIT:
		{
			m_limit();
			break;
		}
		case WORD:
		{
			m_unknown_module();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::src() {
	
	try {      // for error handling
		match(OPT_SRC);
#line 275 "iptables.g"
		
		*dbg << " SRC=";
		
#line 493 "IPTCfgParser.cpp"
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
#line 280 "iptables.g"
			
			importer->src_neg = true;
			
#line 503 "IPTCfgParser.cpp"
			break;
		}
		case WORD:
		case IPV4:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		{
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
			break;
		}
		case IPV4:
		{
			match(IPV4);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 285 "iptables.g"
		
		importer->src_a = LT(0)->getText();
		*dbg << LT(0)->getText();
		
#line 541 "IPTCfgParser.cpp"
		{
		switch ( LA(1)) {
		case SLASH:
		{
			match(SLASH);
			{
			switch ( LA(1)) {
			case IPV4:
			{
				match(IPV4);
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 290 "iptables.g"
			
			importer->src_nm = LT(0)->getText();
			*dbg << "/" << LT(0)->getText();
			
#line 570 "IPTCfgParser.cpp"
			break;
		}
		case NEWLINE:
		case MINUS:
		case UNSUPPORTED_OPTION:
		case OPT_MODULE:
		case OPT_SRC:
		case EXCLAMATION:
		case OPT_DST:
		case OPT_IN_INTF:
		case OPT_OUT_INTF:
		case OPT_PROTO:
		case OPT_TARGET:
		case OPT_FRAGM:
		case MATCH_MARK:
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_SRC_MULTIPORT:
		case MATCH_SRC_MULTIPORT_SHORT:
		case MATCH_DST_MULTIPORT:
		case MATCH_DST_MULTIPORT_SHORT:
		case MATCH_ICMP_TYPE:
		case MATCH_SRC_PORT:
		case MATCH_SRC_PORT_SHORT:
		case MATCH_DST_PORT:
		case MATCH_DST_PORT_SHORT:
		case MATCH_SYN:
		case MATCH_TCP_FLAGS:
		case MATCH_TCP_OPTION:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::dst() {
	
	try {      // for error handling
		match(OPT_DST);
#line 299 "iptables.g"
		
		*dbg << " DST=";
		
#line 626 "IPTCfgParser.cpp"
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
#line 304 "iptables.g"
			
			importer->dst_neg = true;
			
#line 636 "IPTCfgParser.cpp"
			break;
		}
		case WORD:
		case IPV4:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		{
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
			break;
		}
		case IPV4:
		{
			match(IPV4);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 309 "iptables.g"
		
		importer->dst_a = LT(0)->getText();
		*dbg << LT(0)->getText();
		
#line 674 "IPTCfgParser.cpp"
		{
		switch ( LA(1)) {
		case SLASH:
		{
			match(SLASH);
			{
			switch ( LA(1)) {
			case IPV4:
			{
				match(IPV4);
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 314 "iptables.g"
			
			importer->dst_nm = LT(0)->getText();
			*dbg << "/" << LT(0)->getText();
			
#line 703 "IPTCfgParser.cpp"
			break;
		}
		case NEWLINE:
		case MINUS:
		case UNSUPPORTED_OPTION:
		case OPT_MODULE:
		case OPT_SRC:
		case EXCLAMATION:
		case OPT_DST:
		case OPT_IN_INTF:
		case OPT_OUT_INTF:
		case OPT_PROTO:
		case OPT_TARGET:
		case OPT_FRAGM:
		case MATCH_MARK:
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_SRC_MULTIPORT:
		case MATCH_SRC_MULTIPORT_SHORT:
		case MATCH_DST_MULTIPORT:
		case MATCH_DST_MULTIPORT_SHORT:
		case MATCH_ICMP_TYPE:
		case MATCH_SRC_PORT:
		case MATCH_SRC_PORT_SHORT:
		case MATCH_DST_PORT:
		case MATCH_DST_PORT_SHORT:
		case MATCH_SYN:
		case MATCH_TCP_FLAGS:
		case MATCH_TCP_OPTION:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::i_intf() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  i = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OPT_IN_INTF);
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
#line 325 "iptables.g"
			
			importer->intf_neg = true;
			
#line 765 "IPTCfgParser.cpp"
			break;
		}
		case WORD:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		i = LT(1);
		match(WORD);
#line 330 "iptables.g"
		
		importer->i_intf = LT(0)->getText();
		*dbg << " I_INTF=" << i->getText();
		
#line 785 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::o_intf() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  i = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OPT_OUT_INTF);
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
#line 340 "iptables.g"
			
			importer->intf_neg = true;
			
#line 808 "IPTCfgParser.cpp"
			break;
		}
		case WORD:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		i = LT(1);
		match(WORD);
#line 345 "iptables.g"
		
		importer->o_intf = LT(0)->getText();
		*dbg << " O_INTF=" << i->getText();
		
#line 828 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::proto() {
	
	try {      // for error handling
		match(OPT_PROTO);
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
#line 358 "iptables.g"
			
			importer->srv_neg = true;
			
#line 850 "IPTCfgParser.cpp"
			break;
		}
		case WORD:
		case INT_CONST:
		case TCP:
		case UDP:
		case ICMP:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		protocol_word();
#line 363 "iptables.g"
		
		std::string tmp_s = LT(0)->getText();
		importer->protocol.resize(tmp_s.size());
		std::transform(tmp_s.begin(), 
		tmp_s.end(),
		importer->protocol.begin(), 
		::tolower);
		*dbg << " PROTO=" << importer->protocol;
		
#line 878 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::target() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OPT_TARGET);
		t = LT(1);
		match(WORD);
#line 376 "iptables.g"
		
		importer->target = LT(0)->getText();
		*dbg << " TARGET=" << t->getText();
		
#line 899 "IPTCfgParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
				target_options();
			}
			else {
				goto _loop55;
			}
			
		}
		_loop55:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::fragm() {
	
	try {      // for error handling
		match(OPT_FRAGM);
#line 571 "iptables.g"
		
		importer->fragments = true;
		*dbg << " FRAGM";
		
#line 929 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::icmp_type_spec() {
	
	try {      // for error handling
		match(MATCH_ICMP_TYPE);
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
#line 717 "iptables.g"
			
			importer->icmp_spec = LT(0)->getText();
			*dbg << " ICMP_SPEC=" << LT(0)->getText();
			
#line 952 "IPTCfgParser.cpp"
			break;
		}
		case INT_CONST:
		{
			{
			match(INT_CONST);
#line 724 "iptables.g"
			
			importer->icmp_type = LT(0)->getText();
			importer->icmp_code = "-1";
			*dbg << " ICMP_TYPE=" << LT(0)->getText();
			
#line 965 "IPTCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
				match(INT_CONST);
#line 731 "iptables.g"
				
				importer->icmp_code = LT(0)->getText();
				*dbg << " ICMP_CODE=" << LT(0)->getText();
				
#line 977 "IPTCfgParser.cpp"
				break;
			}
			case NEWLINE:
			case MINUS:
			case UNSUPPORTED_OPTION:
			case OPT_MODULE:
			case OPT_SRC:
			case EXCLAMATION:
			case OPT_DST:
			case OPT_IN_INTF:
			case OPT_OUT_INTF:
			case OPT_PROTO:
			case OPT_TARGET:
			case OPT_FRAGM:
			case MATCH_MARK:
			case MATCH_LIMIT:
			case MATCH_LIMIT_BURST:
			case MATCH_SRC_MULTIPORT:
			case MATCH_SRC_MULTIPORT_SHORT:
			case MATCH_DST_MULTIPORT:
			case MATCH_DST_MULTIPORT_SHORT:
			case MATCH_ICMP_TYPE:
			case MATCH_SRC_PORT:
			case MATCH_SRC_PORT_SHORT:
			case MATCH_DST_PORT:
			case MATCH_DST_PORT_SHORT:
			case MATCH_SYN:
			case MATCH_TCP_FLAGS:
			case MATCH_TCP_OPTION:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::basic_tcp_udp_port_spec() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case MATCH_SRC_PORT:
		case MATCH_SRC_PORT_SHORT:
		{
			{
			switch ( LA(1)) {
			case MATCH_SRC_PORT:
			{
				match(MATCH_SRC_PORT);
				break;
			}
			case MATCH_SRC_PORT_SHORT:
			{
				match(MATCH_SRC_PORT_SHORT);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			{
			switch ( LA(1)) {
			case EXCLAMATION:
			{
				match(EXCLAMATION);
#line 792 "iptables.g"
				
				importer->srv_neg = true;
				
#line 1067 "IPTCfgParser.cpp"
				break;
			}
			case WORD:
			case INT_CONST:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			port_def_with_range();
#line 797 "iptables.g"
			
			importer->pushTmpPortSpecToSrcPortList();
			
#line 1086 "IPTCfgParser.cpp"
			break;
		}
		case MATCH_DST_PORT:
		case MATCH_DST_PORT_SHORT:
		{
			{
			switch ( LA(1)) {
			case MATCH_DST_PORT:
			{
				match(MATCH_DST_PORT);
				break;
			}
			case MATCH_DST_PORT_SHORT:
			{
				match(MATCH_DST_PORT_SHORT);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			{
			switch ( LA(1)) {
			case EXCLAMATION:
			{
				match(EXCLAMATION);
#line 804 "iptables.g"
				
				importer->srv_neg = true;
				
#line 1119 "IPTCfgParser.cpp"
				break;
			}
			case WORD:
			case INT_CONST:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			port_def_with_range();
#line 809 "iptables.g"
			
			importer->pushTmpPortSpecToDstPortList();
			
#line 1138 "IPTCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::multiport_tcp_udp_port_spec() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case MATCH_SRC_MULTIPORT:
		case MATCH_SRC_MULTIPORT_SHORT:
		{
			{
			{
			switch ( LA(1)) {
			case MATCH_SRC_MULTIPORT:
			{
				match(MATCH_SRC_MULTIPORT);
				break;
			}
			case MATCH_SRC_MULTIPORT_SHORT:
			{
				match(MATCH_SRC_MULTIPORT_SHORT);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 673 "iptables.g"
			
			importer->startSrcMultiPort();
			*dbg << " SRC MULTIPORT=";
			
#line 1186 "IPTCfgParser.cpp"
			port_def_no_range();
#line 678 "iptables.g"
			
			importer->pushTmpPortSpecToSrcPortList();
			
#line 1192 "IPTCfgParser.cpp"
			{ // ( ... )+
			int _cnt85=0;
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					port_def_no_range();
#line 682 "iptables.g"
					
					importer->pushTmpPortSpecToSrcPortList();
					
#line 1203 "IPTCfgParser.cpp"
				}
				else {
					if ( _cnt85>=1 ) { goto _loop85; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt85++;
			}
			_loop85:;
			}  // ( ... )+
			}
			break;
		}
		case MATCH_DST_MULTIPORT:
		case MATCH_DST_MULTIPORT_SHORT:
		{
			{
			{
			switch ( LA(1)) {
			case MATCH_DST_MULTIPORT:
			{
				match(MATCH_DST_MULTIPORT);
				break;
			}
			case MATCH_DST_MULTIPORT_SHORT:
			{
				match(MATCH_DST_MULTIPORT_SHORT);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 688 "iptables.g"
			
			importer->startDstMultiPort();
			*dbg << " DST MULTIPORT=";
			
#line 1243 "IPTCfgParser.cpp"
			port_def_no_range();
#line 693 "iptables.g"
			
			importer->pushTmpPortSpecToDstPortList();
			
#line 1249 "IPTCfgParser.cpp"
			{ // ( ... )+
			int _cnt89=0;
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					port_def_no_range();
#line 697 "iptables.g"
					
					importer->pushTmpPortSpecToDstPortList();
					
#line 1260 "IPTCfgParser.cpp"
				}
				else {
					if ( _cnt89>=1 ) { goto _loop89; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt89++;
			}
			_loop89:;
			}  // ( ... )+
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::tcp_options() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		case MATCH_SYN:
		{
			syn();
			break;
		}
		case MATCH_TCP_FLAGS:
		{
			tcp_flags();
			break;
		}
		case MATCH_TCP_OPTION:
		{
			tcp_option();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::match_mark() {
	
	try {      // for error handling
		match(MATCH_MARK);
		match(INT_CONST);
#line 623 "iptables.g"
		
		importer->match_mark = LT(0)->getText();
		*dbg << " MATCH MARK " << LT(0)->getText();
		
#line 1332 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::match_limit() {
	
	try {      // for error handling
		match(MATCH_LIMIT);
		limit_rate();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::match_limit_burst() {
	
	try {      // for error handling
		match(MATCH_LIMIT_BURST);
		match(INT_CONST);
#line 654 "iptables.g"
		
		importer->limit_burst = LT(0)->getText();
		*dbg << " LIMIT BURST " << LT(0)->getText();
		
#line 1364 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::unknown_option() {
	
	try {      // for error handling
		if ((LA(1) == MINUS) && (LA(2) == WORD)) {
			{
			match(MINUS);
			match(WORD);
#line 210 "iptables.g"
			
			importer->markCurrentRuleBad(
			std::string("Unknown option: -")+LT(0)->getText());
			*dbg << " UNKNOWN OPTION=-" << LT(0)->getText();
			
#line 1386 "IPTCfgParser.cpp"
			{
			switch ( LA(1)) {
			case WORD:
			case INT_CONST:
			case DIGIT:
			{
				unknown_parameter();
				break;
			}
			case NEWLINE:
			case MINUS:
			case UNSUPPORTED_OPTION:
			case OPT_MODULE:
			case OPT_SRC:
			case EXCLAMATION:
			case OPT_DST:
			case OPT_IN_INTF:
			case OPT_OUT_INTF:
			case OPT_PROTO:
			case OPT_TARGET:
			case OPT_FRAGM:
			case MATCH_MARK:
			case MATCH_LIMIT:
			case MATCH_LIMIT_BURST:
			case MATCH_SRC_MULTIPORT:
			case MATCH_SRC_MULTIPORT_SHORT:
			case MATCH_DST_MULTIPORT:
			case MATCH_DST_MULTIPORT_SHORT:
			case MATCH_ICMP_TYPE:
			case MATCH_SRC_PORT:
			case MATCH_SRC_PORT_SHORT:
			case MATCH_DST_PORT:
			case MATCH_DST_PORT_SHORT:
			case MATCH_SYN:
			case MATCH_TCP_FLAGS:
			case MATCH_TCP_OPTION:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			}
		}
		else if ((LA(1) == MINUS) && (LA(2) == MINUS)) {
			{
			{
			match(MINUS);
			match(MINUS);
			match(WORD);
			}
#line 220 "iptables.g"
			
			importer->markCurrentRuleBad(
			std::string("Unknown option: --")+LT(0)->getText());
			*dbg << " UNKNOWN OPTION=--" << LT(0)->getText();
			
#line 1447 "IPTCfgParser.cpp"
			{
			switch ( LA(1)) {
			case WORD:
			case INT_CONST:
			case DIGIT:
			{
				unknown_parameter();
				break;
			}
			case NEWLINE:
			case MINUS:
			case UNSUPPORTED_OPTION:
			case OPT_MODULE:
			case OPT_SRC:
			case EXCLAMATION:
			case OPT_DST:
			case OPT_IN_INTF:
			case OPT_OUT_INTF:
			case OPT_PROTO:
			case OPT_TARGET:
			case OPT_FRAGM:
			case MATCH_MARK:
			case MATCH_LIMIT:
			case MATCH_LIMIT_BURST:
			case MATCH_SRC_MULTIPORT:
			case MATCH_SRC_MULTIPORT_SHORT:
			case MATCH_DST_MULTIPORT:
			case MATCH_DST_MULTIPORT_SHORT:
			case MATCH_ICMP_TYPE:
			case MATCH_SRC_PORT:
			case MATCH_SRC_PORT_SHORT:
			case MATCH_DST_PORT:
			case MATCH_DST_PORT_SHORT:
			case MATCH_SYN:
			case MATCH_TCP_FLAGS:
			case MATCH_TCP_OPTION:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			}
		}
		else if ((LA(1) == UNSUPPORTED_OPTION)) {
			{
			match(UNSUPPORTED_OPTION);
#line 230 "iptables.g"
			
			importer->markCurrentRuleBad(
			std::string("Unknown option: ")+LT(0)->getText());
			*dbg << " UNKNOWN OPTION=" << LT(0)->getText();
			
#line 1504 "IPTCfgParser.cpp"
			{
			switch ( LA(1)) {
			case WORD:
			case INT_CONST:
			case DIGIT:
			{
				unknown_parameter();
				break;
			}
			case NEWLINE:
			case MINUS:
			case UNSUPPORTED_OPTION:
			case OPT_MODULE:
			case OPT_SRC:
			case EXCLAMATION:
			case OPT_DST:
			case OPT_IN_INTF:
			case OPT_OUT_INTF:
			case OPT_PROTO:
			case OPT_TARGET:
			case OPT_FRAGM:
			case MATCH_MARK:
			case MATCH_LIMIT:
			case MATCH_LIMIT_BURST:
			case MATCH_SRC_MULTIPORT:
			case MATCH_SRC_MULTIPORT_SHORT:
			case MATCH_DST_MULTIPORT:
			case MATCH_DST_MULTIPORT_SHORT:
			case MATCH_ICMP_TYPE:
			case MATCH_SRC_PORT:
			case MATCH_SRC_PORT_SHORT:
			case MATCH_DST_PORT:
			case MATCH_DST_PORT_SHORT:
			case MATCH_SYN:
			case MATCH_TCP_FLAGS:
			case MATCH_TCP_OPTION:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::unknown_parameter() {
	
	try {      // for error handling
		if ((LA(1) == INT_CONST || LA(1) == DIGIT) && (LA(2) == SLASH)) {
#line 240 "iptables.g"
			std::string s;
#line 1570 "IPTCfgParser.cpp"
			{
			{
			{
			switch ( LA(1)) {
			case DIGIT:
			{
				match(DIGIT);
#line 244 "iptables.g"
				s+=LT(0)->getText();
#line 1580 "IPTCfgParser.cpp"
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
#line 246 "iptables.g"
				s+=LT(0)->getText();
#line 1588 "IPTCfgParser.cpp"
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(SLASH);
#line 248 "iptables.g"
			s+=LT(0)->getText();
#line 1600 "IPTCfgParser.cpp"
			match(WORD);
#line 249 "iptables.g"
			s+=LT(0)->getText();
#line 1604 "IPTCfgParser.cpp"
			}
#line 251 "iptables.g"
			
			importer->markCurrentRuleBad(
			std::string("Unknown parameter: ")+s);
			*dbg << " UNKNOWN PARMETER=" << s;
			
#line 1612 "IPTCfgParser.cpp"
			}
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST || LA(1) == DIGIT) && (_tokenSet_4.member(LA(2)))) {
			{
			{
			switch ( LA(1)) {
			case DIGIT:
			{
				match(DIGIT);
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			case WORD:
			{
				match(WORD);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 260 "iptables.g"
			
			importer->markCurrentRuleBad(
			std::string("Unknown parameter: ")+LT(0)->getText());
			*dbg << " UNKNOWN PARMETER=" << LT(0)->getText();
			
#line 1646 "IPTCfgParser.cpp"
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::m_state() {
	
	try {      // for error handling
		match(M_STATE);
		match(MATCH_STATE);
#line 593 "iptables.g"
		
		importer->current_state = "";
		
#line 1670 "IPTCfgParser.cpp"
		state_word();
#line 597 "iptables.g"
		
		importer->current_state += LT(0)->getText();
		
#line 1676 "IPTCfgParser.cpp"
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
			state_word();
#line 602 "iptables.g"
			
			importer->current_state += std::string(",") + LT(0)->getText();
			
#line 1687 "IPTCfgParser.cpp"
			break;
		}
		case NEWLINE:
		case MINUS:
		case UNSUPPORTED_OPTION:
		case OPT_MODULE:
		case OPT_SRC:
		case EXCLAMATION:
		case OPT_DST:
		case OPT_IN_INTF:
		case OPT_OUT_INTF:
		case OPT_PROTO:
		case OPT_TARGET:
		case OPT_FRAGM:
		case MATCH_MARK:
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_SRC_MULTIPORT:
		case MATCH_SRC_MULTIPORT_SHORT:
		case MATCH_DST_MULTIPORT:
		case MATCH_DST_MULTIPORT_SHORT:
		case MATCH_ICMP_TYPE:
		case MATCH_SRC_PORT:
		case MATCH_SRC_PORT_SHORT:
		case MATCH_DST_PORT:
		case MATCH_DST_PORT_SHORT:
		case MATCH_SYN:
		case MATCH_TCP_FLAGS:
		case MATCH_TCP_OPTION:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 606 "iptables.g"
		
		*dbg << " STATE MATCH=" << importer->current_state;
		
#line 1730 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::m_mport() {
	
	try {      // for error handling
		match(M_MPORT);
#line 663 "iptables.g"
		
		*dbg << " MULTIPORT";
		
#line 1747 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::m_icmp() {
	
	try {      // for error handling
		match(ICMP);
#line 707 "iptables.g"
		
		importer->protocol = "icmp";
		*dbg << " ICMP";
		
#line 1765 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::m_tcp() {
	
	try {      // for error handling
		match(TCP);
#line 827 "iptables.g"
		
		importer->protocol = "tcp";
		*dbg << " TCP";
		
#line 1783 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::m_udp() {
	
	try {      // for error handling
		match(UDP);
#line 818 "iptables.g"
		
		importer->protocol = "udp";
		*dbg << " UDP";
		
#line 1801 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::m_mark() {
	
	try {      // for error handling
		match(M_MARK);
#line 615 "iptables.g"
		
		*dbg << " MARK";
		
#line 1818 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::m_limit() {
	
	try {      // for error handling
		match(M_LIMIT);
#line 632 "iptables.g"
		
		*dbg << " LIMIT";
		
#line 1835 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::m_unknown_module() {
	
	try {      // for error handling
		match(WORD);
#line 580 "iptables.g"
		
		*dbg << " UNKNOWN MODULE=" << LT(0)->getText();
		importer->markCurrentRuleBad(
		std::string("Unknown module: ")+LT(0)->getText());
		
#line 1854 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::protocol_word() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case TCP:
		{
			match(TCP);
			break;
		}
		case UDP:
		{
			match(UDP);
			break;
		}
		case ICMP:
		{
			match(ICMP);
			break;
		}
		case WORD:
		{
			match(WORD);
			break;
		}
		case INT_CONST:
		{
			match(INT_CONST);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::target_options() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case REJECT_WITH:
		{
			match(REJECT_WITH);
			match(WORD);
#line 387 "iptables.g"
			
			importer->action_params["reject_with"] = LT(0)->getText();
			*dbg << " REJECT WITH=" << LT(0)->getText();
			
#line 1921 "IPTCfgParser.cpp"
			break;
		}
		case LOG_PREFIX:
		{
			match(LOG_PREFIX);
			{
			switch ( LA(1)) {
			case WORD:
			{
				match(WORD);
				break;
			}
			case STRING:
			{
				match(STRING);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 393 "iptables.g"
			
			importer->action_params["log_prefix"] = LT(0)->getText();
			*dbg << " LOG PREFIX=" << LT(0)->getText();
			
#line 1950 "IPTCfgParser.cpp"
			break;
		}
		case LOG_TCP_SEQ:
		{
			match(LOG_TCP_SEQ);
#line 399 "iptables.g"
			
			importer->action_params["log_tcp_seq"] = LT(0)->getText();
			*dbg << " LOG TCP SEQUENCE=";
			
#line 1961 "IPTCfgParser.cpp"
			break;
		}
		case LOG_TCP_OPT:
		{
			match(LOG_TCP_OPT);
#line 405 "iptables.g"
			
			importer->action_params["log_tcp_options"] = LT(0)->getText();
			*dbg << " LOG TCP OPTIONS=";
			
#line 1972 "IPTCfgParser.cpp"
			break;
		}
		case LOG_IP_OPT:
		{
			match(LOG_IP_OPT);
#line 411 "iptables.g"
			
			importer->action_params["log_ip_options"] = LT(0)->getText();
			*dbg << " LOG IP OPTIONS=";
			
#line 1983 "IPTCfgParser.cpp"
			break;
		}
		case ULOG_PREFIX:
		{
			match(ULOG_PREFIX);
			{
			switch ( LA(1)) {
			case WORD:
			{
				match(WORD);
				break;
			}
			case STRING:
			{
				match(STRING);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 417 "iptables.g"
			
			importer->action_params["log_prefix"] = LT(0)->getText();
			*dbg << " ULOG PREFIX=" << LT(0)->getText();
			
#line 2012 "IPTCfgParser.cpp"
			break;
		}
		case LOG_LEVEL:
		{
			match(LOG_LEVEL);
			match(WORD);
#line 423 "iptables.g"
			
			importer->action_params["log_level"] = LT(0)->getText();
			*dbg << " LOG LEVEL=" << LT(0)->getText();
			
#line 2024 "IPTCfgParser.cpp"
			break;
		}
		case SET_MARK:
		{
			match(SET_MARK);
			match(INT_CONST);
#line 429 "iptables.g"
			
			importer->action_params["set_mark"] = LT(0)->getText();
			*dbg << " SET MARK=" << LT(0)->getText();
			
#line 2036 "IPTCfgParser.cpp"
			break;
		}
		case SAVE_MARK:
		{
			match(SAVE_MARK);
#line 445 "iptables.g"
			
			importer->action_params["connmark_save_mark"] = "--save-mark";
			*dbg << " SAVE MARK";
			
#line 2047 "IPTCfgParser.cpp"
			break;
		}
		case RESTORE_MARK:
		{
			match(RESTORE_MARK);
#line 451 "iptables.g"
			
			importer->action_params["connmark_restore_mark"] = "--restore-mark";
			*dbg << " RESTORE MARK";
			
#line 2058 "IPTCfgParser.cpp"
			break;
		}
		case CONTINUE:
		{
			match(CONTINUE);
#line 457 "iptables.g"
			
			importer->action_params["route_continue"] = "--continue";
			*dbg << " CONTINUE";
			
#line 2069 "IPTCfgParser.cpp"
			break;
		}
		case ROUTE_IIF:
		{
			match(ROUTE_IIF);
			match(WORD);
#line 463 "iptables.g"
			
			importer->action_params["route_iif"] = LT(0)->getText();
			*dbg << " ROUTE_IIF=" << LT(0)->getText();
			
#line 2081 "IPTCfgParser.cpp"
			break;
		}
		case ROUTE_OIF:
		{
			match(ROUTE_OIF);
			match(WORD);
#line 469 "iptables.g"
			
			importer->action_params["route_oif"] = LT(0)->getText();
			*dbg << " ROUTE_OIF=" << LT(0)->getText();
			
#line 2093 "IPTCfgParser.cpp"
			break;
		}
		case ROUTE_GW:
		{
			match(ROUTE_GW);
			match(IPV4);
#line 475 "iptables.g"
			
			importer->action_params["route_gw"] = LT(0)->getText();
			*dbg << " ROUTE_GW=" << LT(0)->getText();
			
#line 2105 "IPTCfgParser.cpp"
			break;
		}
		case ROUTE_TEE:
		{
			match(ROUTE_TEE);
#line 481 "iptables.g"
			
			importer->action_params["route_tee"] = "--tee";
			*dbg << " ROUTE_TEE";
			
#line 2116 "IPTCfgParser.cpp"
			break;
		}
		case TO_SOURCE:
		{
			match(TO_SOURCE);
#line 487 "iptables.g"
			
			*dbg << " TO-SOURCE";
			
#line 2126 "IPTCfgParser.cpp"
			nat_spec();
			break;
		}
		case TO_DESTINATION:
		{
			match(TO_DESTINATION);
#line 493 "iptables.g"
			
			*dbg << " TO-DESTINATION";
			
#line 2137 "IPTCfgParser.cpp"
			nat_spec();
			break;
		}
		case TO_PORTS:
		{
			match(TO_PORTS);
			redirect_spec();
			break;
		}
		case TO_NETMAP:
		{
			match(TO_NETMAP);
#line 501 "iptables.g"
			
			*dbg << " TO-NETMAP";
			
#line 2154 "IPTCfgParser.cpp"
			{
			match(IPV4);
#line 506 "iptables.g"
			
			importer->nat_addr1 = LT(0)->getText();
			importer->nat_addr2 = LT(0)->getText();
			*dbg << LT(0)->getText();
			
#line 2163 "IPTCfgParser.cpp"
			match(SLASH);
			{
			switch ( LA(1)) {
			case IPV4:
			{
				match(IPV4);
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 512 "iptables.g"
			
			importer->nat_nm = LT(0)->getText();
			*dbg << "/" << LT(0)->getText();
			
#line 2188 "IPTCfgParser.cpp"
			}
			break;
		}
		case CLAMP_MSS:
		{
			match(CLAMP_MSS);
#line 519 "iptables.g"
			
			importer->action_params["clamp-mss-to-pmtu"] = "--clamp-mss-to-pmtu";
			*dbg << " TO-NETMAP";
			
#line 2200 "IPTCfgParser.cpp"
			break;
		}
		default:
			if ((LA(1) == SET_TOS) && (LA(2) == HEX_CONST)) {
				match(SET_TOS);
				match(HEX_CONST);
#line 435 "iptables.g"
				
				*dbg << " SET TOS=" << LT(0)->getText() << "(unsupported)";
				
#line 2211 "IPTCfgParser.cpp"
			}
			else if ((LA(1) == SET_TOS) && (LA(2) == WORD)) {
				match(SET_TOS);
				match(WORD);
#line 440 "iptables.g"
				
				*dbg << " SET TOS=" << LT(0)->getText() << "(unsupported)";
				
#line 2220 "IPTCfgParser.cpp"
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

void IPTCfgParser::nat_spec() {
	
	try {      // for error handling
		nat_addr_range();
		{
		switch ( LA(1)) {
		case COLON:
		{
			match(COLON);
			nat_port_def_with_range();
			break;
		}
		case NEWLINE:
		case MINUS:
		case UNSUPPORTED_OPTION:
		case OPT_MODULE:
		case OPT_SRC:
		case EXCLAMATION:
		case OPT_DST:
		case OPT_IN_INTF:
		case OPT_OUT_INTF:
		case OPT_PROTO:
		case OPT_TARGET:
		case REJECT_WITH:
		case LOG_PREFIX:
		case LOG_TCP_SEQ:
		case LOG_TCP_OPT:
		case LOG_IP_OPT:
		case ULOG_PREFIX:
		case LOG_LEVEL:
		case SET_MARK:
		case SET_TOS:
		case SAVE_MARK:
		case RESTORE_MARK:
		case CONTINUE:
		case ROUTE_IIF:
		case ROUTE_OIF:
		case ROUTE_GW:
		case ROUTE_TEE:
		case TO_SOURCE:
		case TO_DESTINATION:
		case TO_PORTS:
		case TO_NETMAP:
		case CLAMP_MSS:
		case OPT_FRAGM:
		case MATCH_MARK:
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_SRC_MULTIPORT:
		case MATCH_SRC_MULTIPORT_SHORT:
		case MATCH_DST_MULTIPORT:
		case MATCH_DST_MULTIPORT_SHORT:
		case MATCH_ICMP_TYPE:
		case MATCH_SRC_PORT:
		case MATCH_SRC_PORT_SHORT:
		case MATCH_DST_PORT:
		case MATCH_DST_PORT_SHORT:
		case MATCH_SYN:
		case MATCH_TCP_FLAGS:
		case MATCH_TCP_OPTION:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 530 "iptables.g"
		
		*dbg << " "
		<< importer->nat_addr1
		<< "-"
		<< importer->nat_addr2
		<< ":"
		<< importer->nat_port_range_start
		<< "-"
		<< importer->nat_port_range_end;
		
#line 2315 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

void IPTCfgParser::redirect_spec() {
	
	try {      // for error handling
		nat_port_def_with_range();
#line 558 "iptables.g"
		
		*dbg << " TO-PORTS "
		<< importer->nat_addr1
		<< "-"
		<< importer->nat_addr2
		<< ":"
		<< importer->nat_port_range_start
		<< importer->nat_port_range_end;
		
#line 2338 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

void IPTCfgParser::nat_addr_range() {
	
	try {      // for error handling
		match(IPV4);
#line 545 "iptables.g"
		
		importer->nat_port_range_start = "";
		importer->nat_port_range_end = "";
		importer->nat_addr1 = LT(0)->getText();
		importer->nat_addr2 = LT(0)->getText();
		
#line 2358 "IPTCfgParser.cpp"
		{
		if ((LA(1) == MINUS) && (LA(2) == IPV4)) {
			match(MINUS);
			match(IPV4);
#line 552 "iptables.g"
			importer->nat_addr2 = LT(0)->getText();
#line 2365 "IPTCfgParser.cpp"
		}
		else if ((_tokenSet_7.member(LA(1))) && (_tokenSet_8.member(LA(2)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_7);
	}
}

void IPTCfgParser::nat_port_def_with_range() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
			break;
		}
		case INT_CONST:
		{
			match(INT_CONST);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 773 "iptables.g"
		
		importer->nat_port_range_start = LT(0)->getText();
		importer->nat_port_range_end = LT(0)->getText();
		*dbg << " PORT=" << LT(0)->getText();
		
#line 2409 "IPTCfgParser.cpp"
		{
		if ((LA(1) == MINUS) && (LA(2) == WORD || LA(2) == INT_CONST)) {
			match(MINUS);
			{
			switch ( LA(1)) {
			case WORD:
			{
				match(WORD);
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 780 "iptables.g"
			
			importer->nat_port_range_end = LT(0)->getText();
			*dbg << ":" << LT(0)->getText();
			
#line 2436 "IPTCfgParser.cpp"
		}
		else if ((_tokenSet_6.member(LA(1))) && (_tokenSet_8.member(LA(2)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

void IPTCfgParser::state_word() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case INVALID:
		{
			match(INVALID);
			break;
		}
		case NEW:
		{
			match(NEW);
			break;
		}
		case ESTABLISHED:
		{
			match(ESTABLISHED);
			break;
		}
		case RELATED:
		{
			match(RELATED);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_9);
	}
}

void IPTCfgParser::limit_rate() {
	
	try {      // for error handling
		match(INT_CONST);
#line 643 "iptables.g"
		importer->limit_val = LT(0)->getText();
#line 2498 "IPTCfgParser.cpp"
		match(SLASH);
		match(WORD);
#line 645 "iptables.g"
		importer->limit_suffix = LT(0)->getText();
#line 2503 "IPTCfgParser.cpp"
#line 646 "iptables.g"
		
		*dbg << " MATCH LIMIT "
		<< importer->limit_val << "/"
		<< importer->limit_suffix;
		
#line 2510 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::port_def_no_range() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
			break;
		}
		case INT_CONST:
		{
			match(INT_CONST);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 743 "iptables.g"
		
		importer->tmp_port_range_start = LT(0)->getText();
		importer->tmp_port_range_end = LT(0)->getText();
		*dbg << " PORT=" << LT(0)->getText();
		
#line 2546 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_9);
	}
}

void IPTCfgParser::port_def_with_range() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
			break;
		}
		case INT_CONST:
		{
			match(INT_CONST);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 754 "iptables.g"
		
		importer->tmp_port_range_start = LT(0)->getText();
		importer->tmp_port_range_end = LT(0)->getText();
		*dbg << " PORT=" << LT(0)->getText();
		
#line 2582 "IPTCfgParser.cpp"
		{
		switch ( LA(1)) {
		case COLON:
		{
			match(COLON);
			{
			switch ( LA(1)) {
			case WORD:
			{
				match(WORD);
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 761 "iptables.g"
			
			importer->tmp_port_range_end = LT(0)->getText();
			*dbg << ":" << LT(0)->getText();
			
#line 2611 "IPTCfgParser.cpp"
			break;
		}
		case NEWLINE:
		case MINUS:
		case UNSUPPORTED_OPTION:
		case OPT_MODULE:
		case OPT_SRC:
		case EXCLAMATION:
		case OPT_DST:
		case OPT_IN_INTF:
		case OPT_OUT_INTF:
		case OPT_PROTO:
		case OPT_TARGET:
		case OPT_FRAGM:
		case MATCH_MARK:
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_SRC_MULTIPORT:
		case MATCH_SRC_MULTIPORT_SHORT:
		case MATCH_DST_MULTIPORT:
		case MATCH_DST_MULTIPORT_SHORT:
		case MATCH_ICMP_TYPE:
		case MATCH_SRC_PORT:
		case MATCH_SRC_PORT_SHORT:
		case MATCH_DST_PORT:
		case MATCH_DST_PORT_SHORT:
		case MATCH_SYN:
		case MATCH_TCP_FLAGS:
		case MATCH_TCP_OPTION:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::syn() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
#line 842 "iptables.g"
			
			importer->srv_neg = true;
			
#line 2670 "IPTCfgParser.cpp"
			break;
		}
		case MATCH_SYN:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(MATCH_SYN);
#line 847 "iptables.g"
		
		importer->tcp_flags_mask.clear();
		importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::SYN);
		importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::RST);
		importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::ACK);
		
		importer->tcp_flags_comp.clear();
		importer->tcp_flags_comp.push_back(libfwbuilder::TCPService::SYN);
		
#line 2694 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::tcp_flags() {
	
	try {      // for error handling
		match(MATCH_TCP_FLAGS);
		tcp_flags_list();
#line 898 "iptables.g"
		
		importer->tcp_flags_mask = importer->tmp_tcp_flags_list;
		importer->tmp_tcp_flags_list.clear();
		
#line 2713 "IPTCfgParser.cpp"
		tcp_flags_list();
#line 903 "iptables.g"
		
		importer->tcp_flags_comp = importer->tmp_tcp_flags_list;
		importer->tmp_tcp_flags_list.clear();
		*dbg << " TCP FLAGS=";
		std::list<int>::iterator i;
		for (i=importer->tcp_flags_mask.begin();
		i!=importer->tcp_flags_mask.end(); ++i)
		*dbg << *i << "|";
		*dbg << " ";
		for (i=importer->tcp_flags_comp.begin();
		i!=importer->tcp_flags_comp.end(); ++i)
		*dbg << *i << "|";
		
#line 2729 "IPTCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::tcp_option() {
	
	try {      // for error handling
		match(MATCH_TCP_OPTION);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			match(NUMBER);
			break;
		}
		case EXCLAMATION:
		{
			match(EXCLAMATION);
			match(NUMBER);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
}

void IPTCfgParser::tcp_flag_word() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case SYN:
		{
			match(SYN);
#line 860 "iptables.g"
			importer->tmp_tcp_flag_code = libfwbuilder::TCPService::SYN;
#line 2779 "IPTCfgParser.cpp"
			break;
		}
		case ACK:
		{
			match(ACK);
#line 862 "iptables.g"
			importer->tmp_tcp_flag_code = libfwbuilder::TCPService::ACK;
#line 2787 "IPTCfgParser.cpp"
			break;
		}
		case FIN:
		{
			match(FIN);
#line 864 "iptables.g"
			importer->tmp_tcp_flag_code = libfwbuilder::TCPService::FIN;
#line 2795 "IPTCfgParser.cpp"
			break;
		}
		case RST:
		{
			match(RST);
#line 866 "iptables.g"
			importer->tmp_tcp_flag_code = libfwbuilder::TCPService::RST;
#line 2803 "IPTCfgParser.cpp"
			break;
		}
		case URG:
		{
			match(URG);
#line 868 "iptables.g"
			importer->tmp_tcp_flag_code = libfwbuilder::TCPService::URG;
#line 2811 "IPTCfgParser.cpp"
			break;
		}
		case PSH:
		{
			match(PSH);
#line 870 "iptables.g"
			importer->tmp_tcp_flag_code = libfwbuilder::TCPService::PSH;
#line 2819 "IPTCfgParser.cpp"
			break;
		}
		case ALL:
		{
			match(ALL);
#line 872 "iptables.g"
			importer->tmp_tcp_flag_code = 99;
#line 2827 "IPTCfgParser.cpp"
			break;
		}
		case NONE:
		{
			match(NONE);
#line 874 "iptables.g"
			importer->tmp_tcp_flag_code = 98;
#line 2835 "IPTCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_10);
	}
}

void IPTCfgParser::tcp_flags_list() {
	
	try {      // for error handling
#line 879 "iptables.g"
		
		importer->tmp_tcp_flags_list.clear();
		importer->tmp_tcp_flag_code = 0;
		
#line 2860 "IPTCfgParser.cpp"
		tcp_flag_word();
#line 884 "iptables.g"
		
		importer->tmp_tcp_flags_list.push_back(importer->tmp_tcp_flag_code);
		
#line 2866 "IPTCfgParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				tcp_flag_word();
#line 889 "iptables.g"
				
				importer->tmp_tcp_flags_list.push_back(
				importer->tmp_tcp_flag_code);
				
#line 2877 "IPTCfgParser.cpp"
			}
			else {
				goto _loop120;
			}
			
		}
		_loop120:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_11);
	}
}

void IPTCfgParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* IPTCfgParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"NEWLINE",
	"LINE_COMMENT",
	"\"COMMIT\"",
	"STAR",
	"WORD",
	"\"INPUT\"",
	"\"FORWARD\"",
	"\"OUTPUT\"",
	"\"PREROUTING\"",
	"\"POSTROUTING\"",
	"COLON",
	"MINUS",
	"OPENING_SQUARE",
	"INT_CONST",
	"CLOSING_SQUARE",
	"ADD_RULE",
	"UNSUPPORTED_OPTION",
	"DIGIT",
	"SLASH",
	"OPT_MODULE",
	"OPT_SRC",
	"EXCLAMATION",
	"IPV4",
	"OPT_DST",
	"OPT_IN_INTF",
	"OPT_OUT_INTF",
	"\"tcp\"",
	"\"udp\"",
	"\"icmp\"",
	"OPT_PROTO",
	"OPT_TARGET",
	"REJECT_WITH",
	"LOG_PREFIX",
	"STRING",
	"LOG_TCP_SEQ",
	"LOG_TCP_OPT",
	"LOG_IP_OPT",
	"ULOG_PREFIX",
	"LOG_LEVEL",
	"SET_MARK",
	"SET_TOS",
	"HEX_CONST",
	"SAVE_MARK",
	"RESTORE_MARK",
	"CONTINUE",
	"ROUTE_IIF",
	"ROUTE_OIF",
	"ROUTE_GW",
	"ROUTE_TEE",
	"TO_SOURCE",
	"TO_DESTINATION",
	"TO_PORTS",
	"TO_NETMAP",
	"CLAMP_MSS",
	"OPT_FRAGM",
	"\"INVALID\"",
	"\"NEW\"",
	"\"ESTABLISHED\"",
	"\"RELATED\"",
	"\"state\"",
	"MATCH_STATE",
	"COMMA",
	"\"mark\"",
	"MATCH_MARK",
	"\"limit\"",
	"MATCH_LIMIT",
	"MATCH_LIMIT_BURST",
	"\"multiport\"",
	"MATCH_SRC_MULTIPORT",
	"MATCH_SRC_MULTIPORT_SHORT",
	"MATCH_DST_MULTIPORT",
	"MATCH_DST_MULTIPORT_SHORT",
	"MATCH_ICMP_TYPE",
	"MATCH_SRC_PORT",
	"MATCH_SRC_PORT_SHORT",
	"MATCH_DST_PORT",
	"MATCH_DST_PORT_SHORT",
	"MATCH_SYN",
	"\"SYN\"",
	"\"ACK\"",
	"\"FIN\"",
	"\"RST\"",
	"\"URG\"",
	"\"PSH\"",
	"\"ALL\"",
	"\"NONE\"",
	"MATCH_TCP_FLAGS",
	"MATCH_TCP_OPTION",
	"NUMBER",
	"Whitespace",
	"NEG_INT_CONST",
	"HEXDIGIT",
	"SECONDS",
	"SET",
	"RSOURCE",
	"ULOG_QTHR",
	"ULOG_NLG",
	"ULOG_CPR",
	"NUMBER_SIGN",
	"PERCENT",
	"AMPERSAND",
	"APOSTROPHE",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"PLUS",
	"DOT",
	"SEMICOLON",
	"LESS_THAN",
	"EQUALS",
	"GREATER_THAN",
	"QUESTION",
	"COMMERCIAL_AT",
	"CARET",
	"UNDERLINE",
	"OPENING_BRACE",
	"CLOSING_BRACE",
	"TILDE",
	0
};

const unsigned long IPTCfgParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long IPTCfgParser::_tokenSet_1_data_[] = { 540914UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT "COMMIT" STAR COLON ADD_RULE 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long IPTCfgParser::_tokenSet_2_data_[] = { 999325696UL, 67108870UL, 201588584UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST OPT_IN_INTF 
// OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_MARK MATCH_LIMIT MATCH_LIMIT_BURST 
// MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT 
// MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT 
// MATCH_SYN MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long IPTCfgParser::_tokenSet_3_data_[] = { 999325952UL, 67108870UL, 201588584UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_MARK MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT 
// MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT 
// MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long IPTCfgParser::_tokenSet_4_data_[] = { 999325712UL, 67108870UL, 201588584UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_MARK MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT 
// MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT 
// MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long IPTCfgParser::_tokenSet_5_data_[] = { 0UL, 67100632UL, 0UL, 0UL };
// REJECT_WITH LOG_PREFIX LOG_TCP_SEQ LOG_TCP_OPT LOG_IP_OPT ULOG_PREFIX 
// LOG_LEVEL SET_MARK SET_TOS SAVE_MARK RESTORE_MARK CONTINUE ROUTE_IIF 
// ROUTE_OIF ROUTE_GW ROUTE_TEE TO_SOURCE TO_DESTINATION TO_PORTS TO_NETMAP 
// CLAMP_MSS 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long IPTCfgParser::_tokenSet_6_data_[] = { 999325712UL, 134209502UL, 201588584UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET REJECT_WITH LOG_PREFIX 
// LOG_TCP_SEQ LOG_TCP_OPT LOG_IP_OPT ULOG_PREFIX LOG_LEVEL SET_MARK SET_TOS 
// SAVE_MARK RESTORE_MARK CONTINUE ROUTE_IIF ROUTE_OIF ROUTE_GW ROUTE_TEE 
// TO_SOURCE TO_DESTINATION TO_PORTS TO_NETMAP CLAMP_MSS OPT_FRAGM MATCH_MARK 
// MATCH_LIMIT MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS 
// MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_6(_tokenSet_6_data_,8);
const unsigned long IPTCfgParser::_tokenSet_7_data_[] = { 999342096UL, 134209502UL, 201588584UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE COLON MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION 
// OPT_DST OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET REJECT_WITH LOG_PREFIX 
// LOG_TCP_SEQ LOG_TCP_OPT LOG_IP_OPT ULOG_PREFIX LOG_LEVEL SET_MARK SET_TOS 
// SAVE_MARK RESTORE_MARK CONTINUE ROUTE_IIF ROUTE_OIF ROUTE_GW ROUTE_TEE 
// TO_SOURCE TO_DESTINATION TO_PORTS TO_NETMAP CLAMP_MSS OPT_FRAGM MATCH_MARK 
// MATCH_LIMIT MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS 
// MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_7(_tokenSet_7_data_,8);
const unsigned long IPTCfgParser::_tokenSet_8_data_[] = { 4290429426UL, 2281701375UL, 536870908UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT "COMMIT" STAR WORD COLON MINUS INT_CONST ADD_RULE 
// UNSUPPORTED_OPTION DIGIT OPT_MODULE OPT_SRC EXCLAMATION IPV4 OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF "tcp" "udp" "icmp" OPT_PROTO OPT_TARGET REJECT_WITH 
// LOG_PREFIX STRING LOG_TCP_SEQ LOG_TCP_OPT LOG_IP_OPT ULOG_PREFIX LOG_LEVEL 
// SET_MARK SET_TOS HEX_CONST SAVE_MARK RESTORE_MARK CONTINUE ROUTE_IIF 
// ROUTE_OIF ROUTE_GW ROUTE_TEE TO_SOURCE TO_DESTINATION TO_PORTS TO_NETMAP 
// CLAMP_MSS OPT_FRAGM "state" "mark" MATCH_MARK "limit" MATCH_LIMIT MATCH_LIMIT_BURST 
// "multiport" MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT 
// MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT 
// MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN "SYN" "ACK" "FIN" "RST" 
// "URG" "PSH" "ALL" "NONE" MATCH_TCP_FLAGS MATCH_TCP_OPTION NUMBER 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_8(_tokenSet_8_data_,8);
const unsigned long IPTCfgParser::_tokenSet_9_data_[] = { 999325712UL, 67108870UL, 201588586UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM COMMA MATCH_MARK 
// MATCH_LIMIT MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS 
// MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_9(_tokenSet_9_data_,8);
const unsigned long IPTCfgParser::_tokenSet_10_data_[] = { 999325712UL, 67108870UL, 268435306UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM COMMA MATCH_MARK 
// MATCH_LIMIT MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN "SYN" 
// "ACK" "FIN" "RST" "URG" "PSH" "ALL" "NONE" MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_10(_tokenSet_10_data_,8);
const unsigned long IPTCfgParser::_tokenSet_11_data_[] = { 999325712UL, 67108870UL, 268435304UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_MARK MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT 
// MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT 
// MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN "SYN" "ACK" "FIN" "RST" 
// "URG" "PSH" "ALL" "NONE" MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_11(_tokenSet_11_data_,8);



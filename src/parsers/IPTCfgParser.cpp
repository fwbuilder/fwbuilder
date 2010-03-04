/* $ANTLR 2.7.7 (20090306): "iptables.g" -> "IPTCfgParser.cpp"$ */
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::comment() {
	
	try {      // for error handling
		match(LINE_COMMENT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::start_table() {
	
	try {      // for error handling
		match(STAR);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 123 "iptables.g"
			
			importer->current_table = LT(0)->getText();
			*dbg << "TABLE " << LT(0)->getText() << std::endl;
			
#line 137 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::create_chain() {
	
	try {      // for error handling
		match(COLON);
		chain_def();
		if ( inputState->guessing==0 ) {
#line 142 "iptables.g"
			
			importer->newUnidirRuleSet(LT(0)->getText());
			*dbg << "NEW CHAIN " << LT(0)->getText() << std::endl;
			
#line 161 "IPTCfgParser.cpp"
		}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::add_rule() {
	
	try {      // for error handling
		match(ADD_RULE);
		chain_def();
		if ( inputState->guessing==0 ) {
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
			
#line 239 "IPTCfgParser.cpp"
		}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::commit() {
	
	try {      // for error handling
		match(COMMIT);
		if ( inputState->guessing==0 ) {
#line 111 "iptables.g"
			
			// push last rule
			importer->pushRule();
			*dbg << " COMMIT" << std::endl;
			// clear current table
			importer->current_table = "";
			
#line 280 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_3);
		} else {
			throw;
		}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
		case M_COMMENT:
		{
			m_comment();
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::src() {
	
	try {      // for error handling
		match(OPT_SRC);
		if ( inputState->guessing==0 ) {
#line 276 "iptables.g"
			
			*dbg << " SRC=";
			
#line 534 "IPTCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
			if ( inputState->guessing==0 ) {
#line 281 "iptables.g"
				
				importer->src_neg = true;
				
#line 546 "IPTCfgParser.cpp"
			}
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
		if ( inputState->guessing==0 ) {
#line 286 "iptables.g"
			
			importer->src_a = LT(0)->getText();
			*dbg << LT(0)->getText();
			
#line 586 "IPTCfgParser.cpp"
		}
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
			if ( inputState->guessing==0 ) {
#line 291 "iptables.g"
				
				importer->src_nm = LT(0)->getText();
				*dbg << "/" << LT(0)->getText();
				
#line 617 "IPTCfgParser.cpp"
			}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::dst() {
	
	try {      // for error handling
		match(OPT_DST);
		if ( inputState->guessing==0 ) {
#line 300 "iptables.g"
			
			*dbg << " DST=";
			
#line 678 "IPTCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
			if ( inputState->guessing==0 ) {
#line 305 "iptables.g"
				
				importer->dst_neg = true;
				
#line 690 "IPTCfgParser.cpp"
			}
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
		if ( inputState->guessing==0 ) {
#line 310 "iptables.g"
			
			importer->dst_a = LT(0)->getText();
			*dbg << LT(0)->getText();
			
#line 730 "IPTCfgParser.cpp"
		}
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
			if ( inputState->guessing==0 ) {
#line 315 "iptables.g"
				
				importer->dst_nm = LT(0)->getText();
				*dbg << "/" << LT(0)->getText();
				
#line 761 "IPTCfgParser.cpp"
			}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
			if ( inputState->guessing==0 ) {
#line 326 "iptables.g"
				
				importer->intf_neg = true;
				
#line 828 "IPTCfgParser.cpp"
			}
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
		if ( inputState->guessing==0 ) {
#line 331 "iptables.g"
			
			importer->i_intf = LT(0)->getText();
			*dbg << " I_INTF=" << i->getText();
			
#line 850 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
			if ( inputState->guessing==0 ) {
#line 341 "iptables.g"
				
				importer->intf_neg = true;
				
#line 878 "IPTCfgParser.cpp"
			}
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
		if ( inputState->guessing==0 ) {
#line 346 "iptables.g"
			
			importer->o_intf = LT(0)->getText();
			*dbg << " O_INTF=" << i->getText();
			
#line 900 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
			if ( inputState->guessing==0 ) {
#line 359 "iptables.g"
				
				importer->srv_neg = true;
				
#line 927 "IPTCfgParser.cpp"
			}
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
		if ( inputState->guessing==0 ) {
#line 364 "iptables.g"
			
			std::string tmp_s = LT(0)->getText();
			importer->protocol.resize(tmp_s.size());
			std::transform(tmp_s.begin(), 
			tmp_s.end(),
			importer->protocol.begin(), 
			::tolower);
			*dbg << " PROTO=" << importer->protocol;
			
#line 957 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::target() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OPT_TARGET);
		t = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 377 "iptables.g"
			
			importer->target = LT(0)->getText();
			*dbg << " TARGET=" << t->getText();
			
#line 983 "IPTCfgParser.cpp"
		}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::fragm() {
	
	try {      // for error handling
		match(OPT_FRAGM);
		if ( inputState->guessing==0 ) {
#line 582 "iptables.g"
			
			importer->fragments = true;
			*dbg << " FRAGM";
			
#line 1018 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
			if ( inputState->guessing==0 ) {
#line 737 "iptables.g"
				
				importer->icmp_spec = LT(0)->getText();
				*dbg << " ICMP_SPEC=" << LT(0)->getText();
				
#line 1046 "IPTCfgParser.cpp"
			}
			break;
		}
		case INT_CONST:
		{
			{
			match(INT_CONST);
			if ( inputState->guessing==0 ) {
#line 744 "iptables.g"
				
				importer->icmp_type = LT(0)->getText();
				importer->icmp_code = "-1";
				*dbg << " ICMP_TYPE=" << LT(0)->getText();
				
#line 1061 "IPTCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
				match(INT_CONST);
				if ( inputState->guessing==0 ) {
#line 751 "iptables.g"
					
					importer->icmp_code = LT(0)->getText();
					*dbg << " ICMP_CODE=" << LT(0)->getText();
					
#line 1075 "IPTCfgParser.cpp"
				}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
				if ( inputState->guessing==0 ) {
#line 835 "iptables.g"
					
					importer->srv_neg = true;
					
#line 1170 "IPTCfgParser.cpp"
				}
				break;
			}
			case WORD:
			case COLON:
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
			{
			switch ( LA(1)) {
			case WORD:
			case INT_CONST:
			{
				port_def_with_range();
				break;
			}
			case COLON:
			{
				port_def_with_incomplete_range();
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 840 "iptables.g"
				
				importer->pushTmpPortSpecToSrcPortList();
				
#line 1210 "IPTCfgParser.cpp"
			}
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
				if ( inputState->guessing==0 ) {
#line 847 "iptables.g"
					
					importer->srv_neg = true;
					
#line 1245 "IPTCfgParser.cpp"
				}
				break;
			}
			case WORD:
			case COLON:
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
			{
			switch ( LA(1)) {
			case WORD:
			case INT_CONST:
			{
				port_def_with_range();
				break;
			}
			case COLON:
			{
				port_def_with_incomplete_range();
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 852 "iptables.g"
				
				importer->pushTmpPortSpecToDstPortList();
				
#line 1285 "IPTCfgParser.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
			if ( inputState->guessing==0 ) {
#line 693 "iptables.g"
				
				importer->startSrcMultiPort();
				*dbg << " SRC MULTIPORT=";
				
#line 1338 "IPTCfgParser.cpp"
			}
			port_def_no_range();
			if ( inputState->guessing==0 ) {
#line 698 "iptables.g"
				
				importer->pushTmpPortSpecToSrcPortList();
				
#line 1346 "IPTCfgParser.cpp"
			}
			{ // ( ... )+
			int _cnt89=0;
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					port_def_no_range();
					if ( inputState->guessing==0 ) {
#line 702 "iptables.g"
						
						importer->pushTmpPortSpecToSrcPortList();
						
#line 1359 "IPTCfgParser.cpp"
					}
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
			if ( inputState->guessing==0 ) {
#line 708 "iptables.g"
				
				importer->startDstMultiPort();
				*dbg << " DST MULTIPORT=";
				
#line 1401 "IPTCfgParser.cpp"
			}
			port_def_no_range();
			if ( inputState->guessing==0 ) {
#line 713 "iptables.g"
				
				importer->pushTmpPortSpecToDstPortList();
				
#line 1409 "IPTCfgParser.cpp"
			}
			{ // ( ... )+
			int _cnt93=0;
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					port_def_no_range();
					if ( inputState->guessing==0 ) {
#line 717 "iptables.g"
						
						importer->pushTmpPortSpecToDstPortList();
						
#line 1422 "IPTCfgParser.cpp"
					}
				}
				else {
					if ( _cnt93>=1 ) { goto _loop93; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt93++;
			}
			_loop93:;
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::tcp_options() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
			if ( inputState->guessing==0 ) {
#line 881 "iptables.g"
				importer->srv_neg = true;
#line 1464 "IPTCfgParser.cpp"
			}
			break;
		}
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
		{
		switch ( LA(1)) {
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::match_mark() {
	
	try {      // for error handling
		match(MATCH_MARK);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 634 "iptables.g"
			
			importer->match_mark = LT(0)->getText();
			*dbg << " MATCH MARK " << LT(0)->getText();
			
#line 1525 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::match_limit() {
	
	try {      // for error handling
		match(MATCH_LIMIT);
		limit_rate();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::match_limit_burst() {
	
	try {      // for error handling
		match(MATCH_LIMIT_BURST);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 665 "iptables.g"
			
			importer->limit_burst = LT(0)->getText();
			*dbg << " LIMIT BURST " << LT(0)->getText();
			
#line 1565 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::unknown_option() {
	
	try {      // for error handling
		if ((LA(1) == MINUS) && (LA(2) == WORD)) {
			{
			match(MINUS);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 210 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown option: -")+LT(0)->getText());
				*dbg << " UNKNOWN OPTION=-" << LT(0)->getText();
				
#line 1592 "IPTCfgParser.cpp"
			}
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
			if ( inputState->guessing==0 ) {
#line 220 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown option: --")+LT(0)->getText());
				*dbg << " UNKNOWN OPTION=--" << LT(0)->getText();
				
#line 1655 "IPTCfgParser.cpp"
			}
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
			if ( inputState->guessing==0 ) {
#line 230 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown option: ")+LT(0)->getText());
				*dbg << " UNKNOWN OPTION=" << LT(0)->getText();
				
#line 1714 "IPTCfgParser.cpp"
			}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::unknown_parameter() {
#line 239 "iptables.g"
	std::string s;
#line 1781 "IPTCfgParser.cpp"
	
	try {      // for error handling
		if ((LA(1) == INT_CONST || LA(1) == DIGIT) && (LA(2) == SLASH)) {
			{
			{
			{
			switch ( LA(1)) {
			case DIGIT:
			{
				match(DIGIT);
				if ( inputState->guessing==0 ) {
#line 245 "iptables.g"
					s+=LT(0)->getText();
#line 1795 "IPTCfgParser.cpp"
				}
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
				if ( inputState->guessing==0 ) {
#line 247 "iptables.g"
					s+=LT(0)->getText();
#line 1805 "IPTCfgParser.cpp"
				}
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(SLASH);
			if ( inputState->guessing==0 ) {
#line 249 "iptables.g"
				s+=LT(0)->getText();
#line 1819 "IPTCfgParser.cpp"
			}
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 250 "iptables.g"
				s+=LT(0)->getText();
#line 1825 "IPTCfgParser.cpp"
			}
			}
			if ( inputState->guessing==0 ) {
#line 252 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown parameter: ")+s);
				*dbg << " UNKNOWN PARMETER=" << s;
				
#line 1835 "IPTCfgParser.cpp"
			}
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
			if ( inputState->guessing==0 ) {
#line 261 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown parameter: ")+LT(0)->getText());
				*dbg << " UNKNOWN PARMETER=" << LT(0)->getText();
				
#line 1871 "IPTCfgParser.cpp"
			}
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_state() {
	
	try {      // for error handling
		match(M_STATE);
		match(MATCH_STATE);
		if ( inputState->guessing==0 ) {
#line 604 "iptables.g"
			
			importer->current_state = "";
			
#line 1900 "IPTCfgParser.cpp"
		}
		state_word();
		if ( inputState->guessing==0 ) {
#line 608 "iptables.g"
			
			importer->current_state += LT(0)->getText();
			
#line 1908 "IPTCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
			state_word();
			if ( inputState->guessing==0 ) {
#line 613 "iptables.g"
				
				importer->current_state += std::string(",") + LT(0)->getText();
				
#line 1921 "IPTCfgParser.cpp"
			}
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
		if ( inputState->guessing==0 ) {
#line 617 "iptables.g"
			
			*dbg << " STATE MATCH=" << importer->current_state;
			
#line 1966 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_mport() {
	
	try {      // for error handling
		match(M_MPORT);
		if ( inputState->guessing==0 ) {
#line 674 "iptables.g"
			
			*dbg << " MULTIPORT";
			
#line 1988 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_icmp() {
	
	try {      // for error handling
		match(ICMP);
		if ( inputState->guessing==0 ) {
#line 727 "iptables.g"
			
			importer->protocol = "icmp";
			*dbg << " ICMP";
			
#line 2011 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_tcp() {
	
	try {      // for error handling
		match(TCP);
		if ( inputState->guessing==0 ) {
#line 870 "iptables.g"
			
			importer->protocol = "tcp";
			*dbg << " TCP";
			
#line 2034 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_udp() {
	
	try {      // for error handling
		match(UDP);
		if ( inputState->guessing==0 ) {
#line 861 "iptables.g"
			
			importer->protocol = "udp";
			*dbg << " UDP";
			
#line 2057 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_mark() {
	
	try {      // for error handling
		match(M_MARK);
		if ( inputState->guessing==0 ) {
#line 626 "iptables.g"
			
			*dbg << " MARK";
			
#line 2079 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_limit() {
	
	try {      // for error handling
		match(M_LIMIT);
		if ( inputState->guessing==0 ) {
#line 643 "iptables.g"
			
			*dbg << " LIMIT";
			
#line 2101 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_comment() {
	
	try {      // for error handling
		match(M_COMMENT);
		match(MATCH_COMMENT);
		match(STRING);
		if ( inputState->guessing==0 ) {
#line 682 "iptables.g"
			
			*dbg << " COMMENT=" << LT(0)->getText();
			
#line 2125 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_unknown_module() {
	
	try {      // for error handling
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 591 "iptables.g"
			
			*dbg << " UNKNOWN MODULE=" << LT(0)->getText();
			importer->markCurrentRuleBad(
			std::string("Unknown module: ")+LT(0)->getText());
			
#line 2149 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
			if ( inputState->guessing==0 ) {
#line 388 "iptables.g"
				
				importer->action_params["reject_with"] = LT(0)->getText();
				*dbg << " REJECT WITH=" << LT(0)->getText();
				
#line 2224 "IPTCfgParser.cpp"
			}
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
			if ( inputState->guessing==0 ) {
#line 394 "iptables.g"
				
				importer->action_params["log_prefix"] = LT(0)->getText();
				*dbg << " LOG PREFIX=" << LT(0)->getText();
				
#line 2255 "IPTCfgParser.cpp"
			}
			break;
		}
		case LOG_TCP_SEQ:
		{
			match(LOG_TCP_SEQ);
			if ( inputState->guessing==0 ) {
#line 400 "iptables.g"
				
				importer->action_params["log_tcp_seq"] = LT(0)->getText();
				*dbg << " LOG TCP SEQUENCE=";
				
#line 2268 "IPTCfgParser.cpp"
			}
			break;
		}
		case LOG_TCP_OPT:
		{
			match(LOG_TCP_OPT);
			if ( inputState->guessing==0 ) {
#line 406 "iptables.g"
				
				importer->action_params["log_tcp_options"] = LT(0)->getText();
				*dbg << " LOG TCP OPTIONS=";
				
#line 2281 "IPTCfgParser.cpp"
			}
			break;
		}
		case LOG_IP_OPT:
		{
			match(LOG_IP_OPT);
			if ( inputState->guessing==0 ) {
#line 412 "iptables.g"
				
				importer->action_params["log_ip_options"] = LT(0)->getText();
				*dbg << " LOG IP OPTIONS=";
				
#line 2294 "IPTCfgParser.cpp"
			}
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
			if ( inputState->guessing==0 ) {
#line 418 "iptables.g"
				
				importer->action_params["log_prefix"] = LT(0)->getText();
				*dbg << " ULOG PREFIX=" << LT(0)->getText();
				
#line 2325 "IPTCfgParser.cpp"
			}
			break;
		}
		case LOG_LEVEL:
		{
			match(LOG_LEVEL);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 424 "iptables.g"
				
				importer->action_params["log_level"] = LT(0)->getText();
				*dbg << " LOG LEVEL=" << LT(0)->getText();
				
#line 2339 "IPTCfgParser.cpp"
			}
			break;
		}
		case SET_MARK:
		{
			match(SET_MARK);
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			case HEX_CONST:
			{
				match(HEX_CONST);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 430 "iptables.g"
				
				importer->action_params["set_mark"] = LT(0)->getText();
				*dbg << " SET MARK=" << LT(0)->getText();
				
#line 2370 "IPTCfgParser.cpp"
			}
			break;
		}
		case SAVE_MARK:
		{
			match(SAVE_MARK);
			if ( inputState->guessing==0 ) {
#line 448 "iptables.g"
				
				importer->action_params["connmark_save_mark"] = "--save-mark";
				*dbg << " SAVE MARK";
				
#line 2383 "IPTCfgParser.cpp"
			}
			break;
		}
		case RESTORE_MARK:
		{
			match(RESTORE_MARK);
			if ( inputState->guessing==0 ) {
#line 454 "iptables.g"
				
				importer->action_params["connmark_restore_mark"] = "--restore-mark";
				*dbg << " RESTORE MARK";
				
#line 2396 "IPTCfgParser.cpp"
			}
			break;
		}
		case CONTINUE:
		{
			match(CONTINUE);
			if ( inputState->guessing==0 ) {
#line 460 "iptables.g"
				
				importer->action_params["route_continue"] = "--continue";
				*dbg << " CONTINUE";
				
#line 2409 "IPTCfgParser.cpp"
			}
			break;
		}
		case ROUTE_IIF:
		{
			match(ROUTE_IIF);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 466 "iptables.g"
				
				importer->action_params["route_iif"] = LT(0)->getText();
				*dbg << " ROUTE_IIF=" << LT(0)->getText();
				
#line 2423 "IPTCfgParser.cpp"
			}
			break;
		}
		case ROUTE_OIF:
		{
			match(ROUTE_OIF);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 472 "iptables.g"
				
				importer->action_params["route_oif"] = LT(0)->getText();
				*dbg << " ROUTE_OIF=" << LT(0)->getText();
				
#line 2437 "IPTCfgParser.cpp"
			}
			break;
		}
		case ROUTE_GW:
		{
			match(ROUTE_GW);
			match(IPV4);
			if ( inputState->guessing==0 ) {
#line 478 "iptables.g"
				
				importer->action_params["route_gw"] = LT(0)->getText();
				*dbg << " ROUTE_GW=" << LT(0)->getText();
				
#line 2451 "IPTCfgParser.cpp"
			}
			break;
		}
		case ROUTE_TEE:
		{
			match(ROUTE_TEE);
			if ( inputState->guessing==0 ) {
#line 484 "iptables.g"
				
				importer->action_params["route_tee"] = "--tee";
				*dbg << " ROUTE_TEE";
				
#line 2464 "IPTCfgParser.cpp"
			}
			break;
		}
		case TO_SOURCE:
		{
			match(TO_SOURCE);
			if ( inputState->guessing==0 ) {
#line 490 "iptables.g"
				
				*dbg << " TO-SOURCE";
				
#line 2476 "IPTCfgParser.cpp"
			}
			nat_spec();
			break;
		}
		case TO_DESTINATION:
		{
			match(TO_DESTINATION);
			if ( inputState->guessing==0 ) {
#line 496 "iptables.g"
				
				*dbg << " TO-DESTINATION";
				
#line 2489 "IPTCfgParser.cpp"
			}
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
			if ( inputState->guessing==0 ) {
#line 504 "iptables.g"
				
				*dbg << " TO-NETMAP";
				
#line 2508 "IPTCfgParser.cpp"
			}
			{
			match(IPV4);
			if ( inputState->guessing==0 ) {
#line 509 "iptables.g"
				
				importer->nat_addr1 = LT(0)->getText();
				importer->nat_addr2 = LT(0)->getText();
				*dbg << LT(0)->getText();
				
#line 2519 "IPTCfgParser.cpp"
			}
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
			if ( inputState->guessing==0 ) {
#line 515 "iptables.g"
				
				importer->nat_nm = LT(0)->getText();
				*dbg << "/" << LT(0)->getText();
				
#line 2546 "IPTCfgParser.cpp"
			}
			}
			break;
		}
		case CLAMP_MSS:
		{
			match(CLAMP_MSS);
			if ( inputState->guessing==0 ) {
#line 522 "iptables.g"
				
				importer->action_params["clamp-mss-to-pmtu"] = "--clamp-mss-to-pmtu";
				*dbg << " TO-NETMAP";
				
#line 2560 "IPTCfgParser.cpp"
			}
			break;
		}
		default:
			if ((LA(1) == SET_TOS) && (LA(2) == HEX_CONST)) {
				match(SET_TOS);
				match(HEX_CONST);
				if ( inputState->guessing==0 ) {
#line 436 "iptables.g"
					
					importer->action_params["set_tos"] = LT(0)->getText();
					*dbg << " SET TOS=" << LT(0)->getText();
					
#line 2574 "IPTCfgParser.cpp"
				}
			}
			else if ((LA(1) == SET_TOS) && (LA(2) == WORD)) {
				match(SET_TOS);
				match(WORD);
				if ( inputState->guessing==0 ) {
#line 442 "iptables.g"
					
					importer->action_params["set_tos"] = LT(0)->getText();
					*dbg << " SET TOS=" << LT(0)->getText();
					
#line 2586 "IPTCfgParser.cpp"
				}
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_6);
		} else {
			throw;
		}
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
		if ( inputState->guessing==0 ) {
#line 533 "iptables.g"
			
			*dbg << " "
			<< importer->nat_addr1
			<< "-"
			<< importer->nat_addr2
			<< ":"
			<< importer->nat_port_range_start
			<< "-"
			<< importer->nat_port_range_end;
			
#line 2686 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_6);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::redirect_spec() {
	
	try {      // for error handling
		nat_port_def_with_range();
		if ( inputState->guessing==0 ) {
#line 568 "iptables.g"
			
			*dbg << " TO-PORTS "
			<< importer->nat_addr1
			<< "-"
			<< importer->nat_addr2
			<< ":"
			<< importer->nat_port_range_start
			<< "-"
			<< importer->nat_port_range_end;
			
#line 2715 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_6);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::nat_addr_range() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		bool synPredMatched67 = false;
		if (((LA(1) == IPV4) && (LA(2) == MINUS))) {
			int _m67 = mark();
			synPredMatched67 = true;
			inputState->guessing++;
			try {
				{
				match(IPV4);
				match(MINUS);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched67 = false;
			}
			rewind(_m67);
			inputState->guessing--;
		}
		if ( synPredMatched67 ) {
			{
			match(IPV4);
			match(MINUS);
			s = LT(1);
			match(IPV4);
			if ( inputState->guessing==0 ) {
#line 549 "iptables.g"
				
				importer->nat_port_range_start = "";
				importer->nat_port_range_end = "";
				importer->nat_addr1 = LT(0)->getText();
				importer->nat_addr2 = s->getText();
				
#line 2763 "IPTCfgParser.cpp"
			}
			}
		}
		else if ((LA(1) == IPV4) && (_tokenSet_7.member(LA(2)))) {
			match(IPV4);
			if ( inputState->guessing==0 ) {
#line 558 "iptables.g"
				
				importer->nat_port_range_start = "";
				importer->nat_port_range_end = "";
				importer->nat_addr1 = LT(0)->getText();
				importer->nat_addr2 = LT(0)->getText();
				
#line 2777 "IPTCfgParser.cpp"
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_7);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::nat_port_def_with_range() {
	
	try {      // for error handling
		bool synPredMatched110 = false;
		if (((LA(1) == WORD || LA(1) == INT_CONST) && (LA(2) == MINUS))) {
			int _m110 = mark();
			synPredMatched110 = true;
			inputState->guessing++;
			try {
				{
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
				match(MINUS);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched110 = false;
			}
			rewind(_m110);
			inputState->guessing--;
		}
		if ( synPredMatched110 ) {
			{
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
			if ( inputState->guessing==0 ) {
#line 810 "iptables.g"
				
				importer->nat_port_range_start = LT(0)->getText();
				importer->nat_port_range_end = LT(0)->getText();
				*dbg << " PORT=" << LT(0)->getText();
				
#line 2859 "IPTCfgParser.cpp"
			}
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
			if ( inputState->guessing==0 ) {
#line 816 "iptables.g"
				
				importer->nat_port_range_end = LT(0)->getText();
				*dbg << ":" << LT(0)->getText();
				
#line 2886 "IPTCfgParser.cpp"
			}
			}
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST) && (_tokenSet_6.member(LA(2)))) {
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
			if ( inputState->guessing==0 ) {
#line 823 "iptables.g"
				
				importer->nat_port_range_start = LT(0)->getText();
				importer->nat_port_range_end = LT(0)->getText();
				*dbg << " PORT=" << LT(0)->getText();
				
#line 2916 "IPTCfgParser.cpp"
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_6);
		} else {
			throw;
		}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_8);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::limit_rate() {
	
	try {      // for error handling
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 654 "iptables.g"
			importer->limit_val = LT(0)->getText();
#line 2983 "IPTCfgParser.cpp"
		}
		match(SLASH);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 656 "iptables.g"
			importer->limit_suffix = LT(0)->getText();
#line 2990 "IPTCfgParser.cpp"
		}
		if ( inputState->guessing==0 ) {
#line 657 "iptables.g"
			
			*dbg << " MATCH LIMIT "
			<< importer->limit_val << "/"
			<< importer->limit_suffix;
			
#line 2999 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
		if ( inputState->guessing==0 ) {
#line 763 "iptables.g"
			
			importer->tmp_port_range_start = LT(0)->getText();
			importer->tmp_port_range_end = LT(0)->getText();
			*dbg << " PORT=" << LT(0)->getText();
			
#line 3040 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_8);
		} else {
			throw;
		}
	}
}

/****************************************************************
 * port definition that allows for port range. That parser should
 * recognize constructs
 *
 * port1         ---> range_start = range_end = port1
 * port1:port2   ---> range_start = port1  range_end = port2
 * port1:        ---> range_start = port1  range_end = 65535
 * :port2        ---> range_start = 0      range_end = port2
 */
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
		if ( inputState->guessing==0 ) {
#line 781 "iptables.g"
			
			importer->tmp_port_range_start = LT(0)->getText();
			importer->tmp_port_range_end = LT(0)->getText();
			*dbg << " PORT=" << LT(0)->getText();
			
#line 3090 "IPTCfgParser.cpp"
		}
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
			if ( inputState->guessing==0 ) {
#line 788 "iptables.g"
				
				importer->tmp_port_range_end = LT(0)->getText();
				*dbg << ":" << LT(0)->getText();
				
#line 3151 "IPTCfgParser.cpp"
			}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::port_def_with_incomplete_range() {
	
	try {      // for error handling
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
		if ( inputState->guessing==0 ) {
#line 797 "iptables.g"
			
			importer->tmp_port_range_start = "0";
			importer->tmp_port_range_end = LT(0)->getText();
			*dbg << "PORT 0:" << LT(0)->getText();
			
#line 3231 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::syn() {
	
	try {      // for error handling
		match(MATCH_SYN);
		if ( inputState->guessing==0 ) {
#line 886 "iptables.g"
			
			importer->tcp_flags_mask.clear();
			importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::SYN);
			importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::RST);
			importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::ACK);
			
			importer->tcp_flags_comp.clear();
			importer->tcp_flags_comp.push_back(libfwbuilder::TCPService::SYN);
			
#line 3259 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::tcp_flags() {
	
	try {      // for error handling
		match(MATCH_TCP_FLAGS);
		tcp_flags_list();
		if ( inputState->guessing==0 ) {
#line 937 "iptables.g"
			
			importer->tcp_flags_mask = importer->tmp_tcp_flags_list;
			importer->tmp_tcp_flags_list.clear();
			
#line 3283 "IPTCfgParser.cpp"
		}
		tcp_flags_list();
		if ( inputState->guessing==0 ) {
#line 942 "iptables.g"
			
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
			
#line 3301 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::tcp_option() {
	
	try {      // for error handling
		match(MATCH_TCP_OPTION);
		{
		switch ( LA(1)) {
		case INT_CONST:
		{
			match(INT_CONST);
			break;
		}
		case EXCLAMATION:
		{
			match(EXCLAMATION);
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::tcp_flag_word() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case SYN:
		{
			match(SYN);
			if ( inputState->guessing==0 ) {
#line 899 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::SYN;
#line 3359 "IPTCfgParser.cpp"
			}
			break;
		}
		case ACK:
		{
			match(ACK);
			if ( inputState->guessing==0 ) {
#line 901 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::ACK;
#line 3369 "IPTCfgParser.cpp"
			}
			break;
		}
		case FIN:
		{
			match(FIN);
			if ( inputState->guessing==0 ) {
#line 903 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::FIN;
#line 3379 "IPTCfgParser.cpp"
			}
			break;
		}
		case RST:
		{
			match(RST);
			if ( inputState->guessing==0 ) {
#line 905 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::RST;
#line 3389 "IPTCfgParser.cpp"
			}
			break;
		}
		case URG:
		{
			match(URG);
			if ( inputState->guessing==0 ) {
#line 907 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::URG;
#line 3399 "IPTCfgParser.cpp"
			}
			break;
		}
		case PSH:
		{
			match(PSH);
			if ( inputState->guessing==0 ) {
#line 909 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::PSH;
#line 3409 "IPTCfgParser.cpp"
			}
			break;
		}
		case ALL:
		{
			match(ALL);
			if ( inputState->guessing==0 ) {
#line 911 "iptables.g"
				importer->tmp_tcp_flag_code = 99;
#line 3419 "IPTCfgParser.cpp"
			}
			break;
		}
		case NONE:
		{
			match(NONE);
			if ( inputState->guessing==0 ) {
#line 913 "iptables.g"
				importer->tmp_tcp_flag_code = 98;
#line 3429 "IPTCfgParser.cpp"
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::tcp_flags_list() {
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 918 "iptables.g"
			
			importer->tmp_tcp_flags_list.clear();
			importer->tmp_tcp_flag_code = 0;
			
#line 3459 "IPTCfgParser.cpp"
		}
		tcp_flag_word();
		if ( inputState->guessing==0 ) {
#line 923 "iptables.g"
			
			importer->tmp_tcp_flags_list.push_back(importer->tmp_tcp_flag_code);
			
#line 3467 "IPTCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				tcp_flag_word();
				if ( inputState->guessing==0 ) {
#line 928 "iptables.g"
					
					importer->tmp_tcp_flags_list.push_back(
					importer->tmp_tcp_flag_code);
					
#line 3480 "IPTCfgParser.cpp"
				}
			}
			else {
				goto _loop132;
			}
			
		}
		_loop132:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_10);
		} else {
			throw;
		}
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
	"HEX_CONST",
	"SET_TOS",
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
	"\"comment\"",
	"MATCH_COMMENT",
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
	"Whitespace",
	"IPV6",
	"MAC_ADDRESS",
	"NEG_INT_CONST",
	"HEXDIGIT",
	"NUM_3DIGIT",
	"NUM_HEX_4DIGIT",
	"NUMBER",
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
const unsigned long IPTCfgParser::_tokenSet_2_data_[] = { 999325696UL, 67108870UL, 806354024UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST OPT_IN_INTF 
// OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_MARK MATCH_LIMIT MATCH_LIMIT_BURST 
// MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT 
// MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT 
// MATCH_SYN MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long IPTCfgParser::_tokenSet_3_data_[] = { 999325952UL, 67108870UL, 806354024UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_MARK MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT 
// MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT 
// MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long IPTCfgParser::_tokenSet_4_data_[] = { 999325712UL, 67108870UL, 806354024UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_MARK MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT 
// MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT 
// MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long IPTCfgParser::_tokenSet_5_data_[] = { 0UL, 67104728UL, 0UL, 0UL };
// REJECT_WITH LOG_PREFIX LOG_TCP_SEQ LOG_TCP_OPT LOG_IP_OPT ULOG_PREFIX 
// LOG_LEVEL SET_MARK SET_TOS SAVE_MARK RESTORE_MARK CONTINUE ROUTE_IIF 
// ROUTE_OIF ROUTE_GW ROUTE_TEE TO_SOURCE TO_DESTINATION TO_PORTS TO_NETMAP 
// CLAMP_MSS 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long IPTCfgParser::_tokenSet_6_data_[] = { 999325712UL, 134213598UL, 806354024UL, 0UL, 0UL, 0UL, 0UL, 0UL };
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
const unsigned long IPTCfgParser::_tokenSet_7_data_[] = { 999342096UL, 134213598UL, 806354024UL, 0UL, 0UL, 0UL, 0UL, 0UL };
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
const unsigned long IPTCfgParser::_tokenSet_8_data_[] = { 999325712UL, 67108870UL, 806354026UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM COMMA MATCH_MARK 
// MATCH_LIMIT MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS 
// MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_8(_tokenSet_8_data_,8);
const unsigned long IPTCfgParser::_tokenSet_9_data_[] = { 999325712UL, 67108870UL, 1073740906UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM COMMA MATCH_MARK 
// MATCH_LIMIT MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN "SYN" 
// "ACK" "FIN" "RST" "URG" "PSH" "ALL" "NONE" MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_9(_tokenSet_9_data_,8);
const unsigned long IPTCfgParser::_tokenSet_10_data_[] = { 999325712UL, 67108870UL, 1073740904UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_MARK MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT 
// MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT 
// MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN "SYN" "ACK" "FIN" "RST" 
// "URG" "PSH" "ALL" "NONE" MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_10(_tokenSet_10_data_,8);



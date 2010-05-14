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
#include "fwbuilder/Logger.h"

#include <algorithm>


#line 28 "IPTCfgParser.cpp"
#line 1 "iptables.g"
#line 30 "IPTCfgParser.cpp"
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
#line 125 "iptables.g"
			
			if (!importer->current_table.empty())
			{
			// we did not see COMMIT
			*(importer->logger) << "********************************\n";
			*(importer->logger) << "Missing COMMIT for the table " << importer->current_table << "\n";
			*(importer->logger) << "Perhaps the file is broken ?" << "\n";
			*(importer->logger) << "********************************\n";
			
			*dbg << "Missing COMMIT for the table " << importer->current_table;
			*dbg << "Perhaps the file is broken ?";
			
			// push last rule
			importer->pushRule();
			// clear current table
			importer->current_table = "";
			}
			importer->current_table = LT(0)->getText();
			*dbg << "TABLE " << LT(0)->getText() << std::endl;
			
#line 155 "IPTCfgParser.cpp"
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
#line 160 "iptables.g"
			
			importer->newUnidirRuleSet(LT(0)->getText());
			*dbg << "NEW CHAIN " << LT(0)->getText() << std::endl;
			
#line 179 "IPTCfgParser.cpp"
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
#line 171 "iptables.g"
			
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
			
#line 257 "IPTCfgParser.cpp"
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
#line 113 "iptables.g"
			
			// push last rule
			importer->pushRule();
			*dbg << " COMMIT" << std::endl;
			// clear current table
			importer->current_table = "";
			
#line 298 "IPTCfgParser.cpp"
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
		case MATCH_LENGTH:
		{
			match_length();
			break;
		}
		case MINUS:
		case UNSUPPORTED_OPTION:
		{
			unknown_option();
			break;
		}
		default:
			if ((LA(1) == OPT_MODULE) && (_tokenSet_4.member(LA(2)))) {
				module();
			}
			else if ((LA(1) == OPT_MODULE) && (LA(2) == M_MARK)) {
				match_mark();
			}
			else if ((LA(1) == OPT_MODULE) && (LA(2) == M_RECENT)) {
				match_recent();
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
			recover(ex,_tokenSet_5);
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
		case M_LIMIT:
		{
			m_limit();
			break;
		}
		case M_LENGTH:
		{
			m_length();
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
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::match_mark() {
	
	try {      // for error handling
		match(OPT_MODULE);
		m_mark();
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
			if ( inputState->guessing==0 ) {
#line 659 "iptables.g"
				importer->neg_match_mark = true;
#line 560 "IPTCfgParser.cpp"
			}
			break;
		}
		case MATCH_MARK:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(MATCH_MARK);
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
#line 661 "iptables.g"
			
			importer->match_mark = LT(0)->getText();
			*dbg << " MATCH MARK " << LT(0)->getText();
			
#line 599 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::match_recent() {
	
	try {      // for error handling
		match(OPT_MODULE);
		m_recent();
		{ // ( ... )+
		int _cnt87=0;
		for (;;) {
			if (((LA(1) >= MATCH_RECENT_SET && LA(1) <= MATCH_RECENT_HITCOUNT))) {
				recent_opts();
			}
			else {
				if ( _cnt87>=1 ) { goto _loop87; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt87++;
		}
		_loop87:;
		}  // ( ... )+
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::src() {
	
	try {      // for error handling
		match(OPT_SRC);
		if ( inputState->guessing==0 ) {
#line 301 "iptables.g"
			
			*dbg << " SRC=";
			
#line 651 "IPTCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
			if ( inputState->guessing==0 ) {
#line 306 "iptables.g"
				
				importer->src_neg = true;
				
#line 663 "IPTCfgParser.cpp"
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
#line 311 "iptables.g"
			
			importer->src_a = LT(0)->getText();
			*dbg << LT(0)->getText();
			
#line 703 "IPTCfgParser.cpp"
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
#line 316 "iptables.g"
				
				importer->src_nm = LT(0)->getText();
				*dbg << "/" << LT(0)->getText();
				
#line 734 "IPTCfgParser.cpp"
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
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_LENGTH:
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
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::dst() {
	
	try {      // for error handling
		match(OPT_DST);
		if ( inputState->guessing==0 ) {
#line 325 "iptables.g"
			
			*dbg << " DST=";
			
#line 795 "IPTCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
			if ( inputState->guessing==0 ) {
#line 330 "iptables.g"
				
				importer->dst_neg = true;
				
#line 807 "IPTCfgParser.cpp"
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
#line 335 "iptables.g"
			
			importer->dst_a = LT(0)->getText();
			*dbg << LT(0)->getText();
			
#line 847 "IPTCfgParser.cpp"
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
#line 340 "iptables.g"
				
				importer->dst_nm = LT(0)->getText();
				*dbg << "/" << LT(0)->getText();
				
#line 878 "IPTCfgParser.cpp"
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
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_LENGTH:
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
			recover(ex,_tokenSet_5);
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
#line 351 "iptables.g"
				
				importer->intf_neg = true;
				
#line 945 "IPTCfgParser.cpp"
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
#line 356 "iptables.g"
			
			importer->i_intf = LT(0)->getText();
			*dbg << " I_INTF=" << i->getText();
			
#line 967 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
#line 366 "iptables.g"
				
				importer->intf_neg = true;
				
#line 995 "IPTCfgParser.cpp"
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
#line 371 "iptables.g"
			
			importer->o_intf = LT(0)->getText();
			*dbg << " O_INTF=" << i->getText();
			
#line 1017 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
#line 384 "iptables.g"
				
				importer->srv_neg = true;
				
#line 1044 "IPTCfgParser.cpp"
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
#line 389 "iptables.g"
			
			std::string tmp_s = LT(0)->getText();
			importer->protocol.resize(tmp_s.size());
			std::transform(tmp_s.begin(), 
			tmp_s.end(),
			importer->protocol.begin(), 
			::tolower);
			*dbg << " PROTO=" << importer->protocol;
			
#line 1074 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
#line 402 "iptables.g"
			
			importer->target = LT(0)->getText();
			*dbg << " TARGET=" << t->getText();
			
#line 1100 "IPTCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_6.member(LA(1)))) {
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
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::fragm() {
	
	try {      // for error handling
		match(OPT_FRAGM);
		if ( inputState->guessing==0 ) {
#line 607 "iptables.g"
			
			importer->fragments = true;
			*dbg << " FRAGM";
			
#line 1135 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
#line 820 "iptables.g"
				
				importer->icmp_spec = LT(0)->getText();
				*dbg << " ICMP_SPEC=" << LT(0)->getText();
				
#line 1163 "IPTCfgParser.cpp"
			}
			break;
		}
		case INT_CONST:
		{
			{
			match(INT_CONST);
			if ( inputState->guessing==0 ) {
#line 827 "iptables.g"
				
				importer->icmp_type = LT(0)->getText();
				importer->icmp_code = "-1";
				*dbg << " ICMP_TYPE=" << LT(0)->getText();
				
#line 1178 "IPTCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
				match(INT_CONST);
				if ( inputState->guessing==0 ) {
#line 834 "iptables.g"
					
					importer->icmp_code = LT(0)->getText();
					*dbg << " ICMP_CODE=" << LT(0)->getText();
					
#line 1192 "IPTCfgParser.cpp"
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
			case MATCH_LIMIT:
			case MATCH_LIMIT_BURST:
			case MATCH_LENGTH:
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
			recover(ex,_tokenSet_5);
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
#line 918 "iptables.g"
					
					importer->srv_neg = true;
					
#line 1287 "IPTCfgParser.cpp"
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
#line 923 "iptables.g"
				
				importer->pushTmpPortSpecToSrcPortList();
				
#line 1327 "IPTCfgParser.cpp"
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
#line 930 "iptables.g"
					
					importer->srv_neg = true;
					
#line 1362 "IPTCfgParser.cpp"
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
#line 935 "iptables.g"
				
				importer->pushTmpPortSpecToDstPortList();
				
#line 1402 "IPTCfgParser.cpp"
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
			recover(ex,_tokenSet_5);
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
#line 776 "iptables.g"
				
				importer->startSrcMultiPort();
				*dbg << " SRC MULTIPORT=";
				
#line 1455 "IPTCfgParser.cpp"
			}
			port_def_no_range();
			if ( inputState->guessing==0 ) {
#line 781 "iptables.g"
				
				importer->pushTmpPortSpecToSrcPortList();
				
#line 1463 "IPTCfgParser.cpp"
			}
			{ // ( ... )+
			int _cnt104=0;
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					port_def_no_range();
					if ( inputState->guessing==0 ) {
#line 785 "iptables.g"
						
						importer->pushTmpPortSpecToSrcPortList();
						
#line 1476 "IPTCfgParser.cpp"
					}
				}
				else {
					if ( _cnt104>=1 ) { goto _loop104; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt104++;
			}
			_loop104:;
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
#line 791 "iptables.g"
				
				importer->startDstMultiPort();
				*dbg << " DST MULTIPORT=";
				
#line 1518 "IPTCfgParser.cpp"
			}
			port_def_no_range();
			if ( inputState->guessing==0 ) {
#line 796 "iptables.g"
				
				importer->pushTmpPortSpecToDstPortList();
				
#line 1526 "IPTCfgParser.cpp"
			}
			{ // ( ... )+
			int _cnt108=0;
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					port_def_no_range();
					if ( inputState->guessing==0 ) {
#line 800 "iptables.g"
						
						importer->pushTmpPortSpecToDstPortList();
						
#line 1539 "IPTCfgParser.cpp"
					}
				}
				else {
					if ( _cnt108>=1 ) { goto _loop108; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt108++;
			}
			_loop108:;
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
			recover(ex,_tokenSet_5);
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
#line 964 "iptables.g"
				importer->srv_neg = true;
#line 1581 "IPTCfgParser.cpp"
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
			recover(ex,_tokenSet_5);
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
			recover(ex,_tokenSet_5);
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
#line 692 "iptables.g"
			
			importer->limit_burst = LT(0)->getText();
			*dbg << " LIMIT BURST " << LT(0)->getText();
			
#line 1658 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::match_length() {
	
	try {      // for error handling
		match(MATCH_LENGTH);
		length_spec();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
#line 232 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown option: -")+LT(0)->getText());
				*dbg << " UNKNOWN OPTION=-" << LT(0)->getText();
				
#line 1701 "IPTCfgParser.cpp"
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
			case MATCH_LIMIT:
			case MATCH_LIMIT_BURST:
			case MATCH_LENGTH:
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
#line 242 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown option: --")+LT(0)->getText());
				*dbg << " UNKNOWN OPTION=--" << LT(0)->getText();
				
#line 1764 "IPTCfgParser.cpp"
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
			case MATCH_LIMIT:
			case MATCH_LIMIT_BURST:
			case MATCH_LENGTH:
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
#line 252 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown option: ")+LT(0)->getText());
				*dbg << " UNKNOWN OPTION=" << LT(0)->getText();
				
#line 1823 "IPTCfgParser.cpp"
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
			case MATCH_LIMIT:
			case MATCH_LIMIT_BURST:
			case MATCH_LENGTH:
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
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::unknown_parameter() {
#line 261 "iptables.g"
	std::string s;
#line 1890 "IPTCfgParser.cpp"
	
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
#line 267 "iptables.g"
					s+=LT(0)->getText();
#line 1904 "IPTCfgParser.cpp"
				}
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
				if ( inputState->guessing==0 ) {
#line 269 "iptables.g"
					s+=LT(0)->getText();
#line 1914 "IPTCfgParser.cpp"
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
#line 271 "iptables.g"
				s+=LT(0)->getText();
#line 1928 "IPTCfgParser.cpp"
			}
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 272 "iptables.g"
				s+=LT(0)->getText();
#line 1934 "IPTCfgParser.cpp"
			}
			}
			if ( inputState->guessing==0 ) {
#line 274 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown parameter: ")+s);
				*dbg << " UNKNOWN PARMETER=" << s;
				
#line 1944 "IPTCfgParser.cpp"
			}
			}
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST || LA(1) == DIGIT) && (_tokenSet_5.member(LA(2)))) {
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
#line 283 "iptables.g"
				
				importer->markCurrentRuleBad(
				std::string("Unknown parameter: ")+LT(0)->getText());
				*dbg << " UNKNOWN PARMETER=" << LT(0)->getText();
				
#line 1980 "IPTCfgParser.cpp"
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
			recover(ex,_tokenSet_5);
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
#line 629 "iptables.g"
			
			importer->current_state = "";
			
#line 2009 "IPTCfgParser.cpp"
		}
		state_word();
		if ( inputState->guessing==0 ) {
#line 633 "iptables.g"
			
			importer->current_state += LT(0)->getText();
			
#line 2017 "IPTCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
			state_word();
			if ( inputState->guessing==0 ) {
#line 638 "iptables.g"
				
				importer->current_state += std::string(",") + LT(0)->getText();
				
#line 2030 "IPTCfgParser.cpp"
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
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_LENGTH:
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
#line 642 "iptables.g"
			
			*dbg << " STATE MATCH=" << importer->current_state;
			
#line 2075 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_mport() {
	
	try {      // for error handling
		match(M_MPORT);
		if ( inputState->guessing==0 ) {
#line 757 "iptables.g"
			
			*dbg << " MULTIPORT";
			
#line 2097 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_icmp() {
	
	try {      // for error handling
		match(ICMP);
		if ( inputState->guessing==0 ) {
#line 810 "iptables.g"
			
			importer->protocol = "icmp";
			*dbg << " ICMP";
			
#line 2120 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_tcp() {
	
	try {      // for error handling
		match(TCP);
		if ( inputState->guessing==0 ) {
#line 953 "iptables.g"
			
			importer->protocol = "tcp";
			*dbg << " TCP";
			
#line 2143 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_udp() {
	
	try {      // for error handling
		match(UDP);
		if ( inputState->guessing==0 ) {
#line 944 "iptables.g"
			
			importer->protocol = "udp";
			*dbg << " UDP";
			
#line 2166 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_limit() {
	
	try {      // for error handling
		match(M_LIMIT);
		if ( inputState->guessing==0 ) {
#line 670 "iptables.g"
			
			*dbg << " LIMIT";
			
#line 2188 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_length() {
	
	try {      // for error handling
		match(M_LENGTH);
		if ( inputState->guessing==0 ) {
#line 734 "iptables.g"
			
			*dbg << " LENGTH";
			
#line 2210 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
#line 765 "iptables.g"
			
			*dbg << " COMMENT=" << LT(0)->getText();
			
#line 2234 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_unknown_module() {
	
	try {      // for error handling
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 616 "iptables.g"
			
			*dbg << " UNKNOWN MODULE=" << LT(0)->getText();
			importer->markCurrentRuleBad(
			std::string("Unknown module: ")+LT(0)->getText());
			
#line 2258 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
			recover(ex,_tokenSet_5);
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
#line 413 "iptables.g"
				
				importer->action_params["reject_with"] = LT(0)->getText();
				*dbg << " REJECT WITH=" << LT(0)->getText();
				
#line 2333 "IPTCfgParser.cpp"
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
#line 419 "iptables.g"
				
				importer->action_params["log_prefix"] = LT(0)->getText();
				*dbg << " LOG PREFIX=" << LT(0)->getText();
				
#line 2364 "IPTCfgParser.cpp"
			}
			break;
		}
		case LOG_TCP_SEQ:
		{
			match(LOG_TCP_SEQ);
			if ( inputState->guessing==0 ) {
#line 425 "iptables.g"
				
				importer->action_params["log_tcp_seq"] = LT(0)->getText();
				*dbg << " LOG TCP SEQUENCE=";
				
#line 2377 "IPTCfgParser.cpp"
			}
			break;
		}
		case LOG_TCP_OPT:
		{
			match(LOG_TCP_OPT);
			if ( inputState->guessing==0 ) {
#line 431 "iptables.g"
				
				importer->action_params["log_tcp_options"] = LT(0)->getText();
				*dbg << " LOG TCP OPTIONS=";
				
#line 2390 "IPTCfgParser.cpp"
			}
			break;
		}
		case LOG_IP_OPT:
		{
			match(LOG_IP_OPT);
			if ( inputState->guessing==0 ) {
#line 437 "iptables.g"
				
				importer->action_params["log_ip_options"] = LT(0)->getText();
				*dbg << " LOG IP OPTIONS=";
				
#line 2403 "IPTCfgParser.cpp"
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
#line 443 "iptables.g"
				
				importer->action_params["log_prefix"] = LT(0)->getText();
				*dbg << " ULOG PREFIX=" << LT(0)->getText();
				
#line 2434 "IPTCfgParser.cpp"
			}
			break;
		}
		case LOG_LEVEL:
		{
			match(LOG_LEVEL);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 449 "iptables.g"
				
				importer->action_params["log_level"] = LT(0)->getText();
				*dbg << " LOG LEVEL=" << LT(0)->getText();
				
#line 2448 "IPTCfgParser.cpp"
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
#line 455 "iptables.g"
				
				importer->action_params["set_mark"] = LT(0)->getText();
				*dbg << " SET MARK=" << LT(0)->getText();
				
#line 2479 "IPTCfgParser.cpp"
			}
			break;
		}
		case SAVE_MARK:
		{
			match(SAVE_MARK);
			if ( inputState->guessing==0 ) {
#line 473 "iptables.g"
				
				importer->action_params["connmark_save_mark"] = "--save-mark";
				*dbg << " SAVE MARK";
				
#line 2492 "IPTCfgParser.cpp"
			}
			break;
		}
		case RESTORE_MARK:
		{
			match(RESTORE_MARK);
			if ( inputState->guessing==0 ) {
#line 479 "iptables.g"
				
				importer->action_params["connmark_restore_mark"] = "--restore-mark";
				*dbg << " RESTORE MARK";
				
#line 2505 "IPTCfgParser.cpp"
			}
			break;
		}
		case CONTINUE:
		{
			match(CONTINUE);
			if ( inputState->guessing==0 ) {
#line 485 "iptables.g"
				
				importer->action_params["route_continue"] = "--continue";
				*dbg << " CONTINUE";
				
#line 2518 "IPTCfgParser.cpp"
			}
			break;
		}
		case ROUTE_IIF:
		{
			match(ROUTE_IIF);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 491 "iptables.g"
				
				importer->action_params["route_iif"] = LT(0)->getText();
				*dbg << " ROUTE_IIF=" << LT(0)->getText();
				
#line 2532 "IPTCfgParser.cpp"
			}
			break;
		}
		case ROUTE_OIF:
		{
			match(ROUTE_OIF);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 497 "iptables.g"
				
				importer->action_params["route_oif"] = LT(0)->getText();
				*dbg << " ROUTE_OIF=" << LT(0)->getText();
				
#line 2546 "IPTCfgParser.cpp"
			}
			break;
		}
		case ROUTE_GW:
		{
			match(ROUTE_GW);
			match(IPV4);
			if ( inputState->guessing==0 ) {
#line 503 "iptables.g"
				
				importer->action_params["route_gw"] = LT(0)->getText();
				*dbg << " ROUTE_GW=" << LT(0)->getText();
				
#line 2560 "IPTCfgParser.cpp"
			}
			break;
		}
		case ROUTE_TEE:
		{
			match(ROUTE_TEE);
			if ( inputState->guessing==0 ) {
#line 509 "iptables.g"
				
				importer->action_params["route_tee"] = "--tee";
				*dbg << " ROUTE_TEE";
				
#line 2573 "IPTCfgParser.cpp"
			}
			break;
		}
		case TO_SOURCE:
		{
			match(TO_SOURCE);
			if ( inputState->guessing==0 ) {
#line 515 "iptables.g"
				
				*dbg << " TO-SOURCE";
				
#line 2585 "IPTCfgParser.cpp"
			}
			nat_spec();
			break;
		}
		case TO_DESTINATION:
		{
			match(TO_DESTINATION);
			if ( inputState->guessing==0 ) {
#line 521 "iptables.g"
				
				*dbg << " TO-DESTINATION";
				
#line 2598 "IPTCfgParser.cpp"
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
#line 529 "iptables.g"
				
				*dbg << " TO-NETMAP";
				
#line 2617 "IPTCfgParser.cpp"
			}
			{
			match(IPV4);
			if ( inputState->guessing==0 ) {
#line 534 "iptables.g"
				
				importer->nat_addr1 = LT(0)->getText();
				importer->nat_addr2 = LT(0)->getText();
				*dbg << LT(0)->getText();
				
#line 2628 "IPTCfgParser.cpp"
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
#line 540 "iptables.g"
				
				importer->nat_nm = LT(0)->getText();
				*dbg << "/" << LT(0)->getText();
				
#line 2655 "IPTCfgParser.cpp"
			}
			}
			break;
		}
		case CLAMP_MSS:
		{
			match(CLAMP_MSS);
			if ( inputState->guessing==0 ) {
#line 547 "iptables.g"
				
				importer->action_params["clamp-mss-to-pmtu"] = "--clamp-mss-to-pmtu";
				*dbg << " TO-NETMAP";
				
#line 2669 "IPTCfgParser.cpp"
			}
			break;
		}
		default:
			if ((LA(1) == SET_TOS) && (LA(2) == HEX_CONST)) {
				match(SET_TOS);
				match(HEX_CONST);
				if ( inputState->guessing==0 ) {
#line 461 "iptables.g"
					
					importer->action_params["set_tos"] = LT(0)->getText();
					*dbg << " SET TOS=" << LT(0)->getText();
					
#line 2683 "IPTCfgParser.cpp"
				}
			}
			else if ((LA(1) == SET_TOS) && (LA(2) == WORD)) {
				match(SET_TOS);
				match(WORD);
				if ( inputState->guessing==0 ) {
#line 467 "iptables.g"
					
					importer->action_params["set_tos"] = LT(0)->getText();
					*dbg << " SET TOS=" << LT(0)->getText();
					
#line 2695 "IPTCfgParser.cpp"
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
			recover(ex,_tokenSet_7);
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
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_LENGTH:
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
#line 558 "iptables.g"
			
			*dbg << " "
			<< importer->nat_addr1
			<< "-"
			<< importer->nat_addr2
			<< ":"
			<< importer->nat_port_range_start
			<< "-"
			<< importer->nat_port_range_end;
			
#line 2795 "IPTCfgParser.cpp"
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

void IPTCfgParser::redirect_spec() {
	
	try {      // for error handling
		nat_port_def_with_range();
		if ( inputState->guessing==0 ) {
#line 593 "iptables.g"
			
			*dbg << " TO-PORTS "
			<< importer->nat_addr1
			<< "-"
			<< importer->nat_addr2
			<< ":"
			<< importer->nat_port_range_start
			<< "-"
			<< importer->nat_port_range_end;
			
#line 2824 "IPTCfgParser.cpp"
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
#line 574 "iptables.g"
				
				importer->nat_port_range_start = "";
				importer->nat_port_range_end = "";
				importer->nat_addr1 = LT(0)->getText();
				importer->nat_addr2 = s->getText();
				
#line 2872 "IPTCfgParser.cpp"
			}
			}
		}
		else if ((LA(1) == IPV4) && (_tokenSet_8.member(LA(2)))) {
			match(IPV4);
			if ( inputState->guessing==0 ) {
#line 583 "iptables.g"
				
				importer->nat_port_range_start = "";
				importer->nat_port_range_end = "";
				importer->nat_addr1 = LT(0)->getText();
				importer->nat_addr2 = LT(0)->getText();
				
#line 2886 "IPTCfgParser.cpp"
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
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

void IPTCfgParser::nat_port_def_with_range() {
	
	try {      // for error handling
		bool synPredMatched125 = false;
		if (((LA(1) == WORD || LA(1) == INT_CONST) && (LA(2) == MINUS))) {
			int _m125 = mark();
			synPredMatched125 = true;
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
				synPredMatched125 = false;
			}
			rewind(_m125);
			inputState->guessing--;
		}
		if ( synPredMatched125 ) {
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
#line 893 "iptables.g"
				
				importer->nat_port_range_start = LT(0)->getText();
				importer->nat_port_range_end = LT(0)->getText();
				*dbg << " PORT=" << LT(0)->getText();
				
#line 2968 "IPTCfgParser.cpp"
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
#line 899 "iptables.g"
				
				importer->nat_port_range_end = LT(0)->getText();
				*dbg << ":" << LT(0)->getText();
				
#line 2995 "IPTCfgParser.cpp"
			}
			}
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST) && (_tokenSet_7.member(LA(2)))) {
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
#line 906 "iptables.g"
				
				importer->nat_port_range_start = LT(0)->getText();
				importer->nat_port_range_end = LT(0)->getText();
				*dbg << " PORT=" << LT(0)->getText();
				
#line 3025 "IPTCfgParser.cpp"
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
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_mark() {
	
	try {      // for error handling
		match(M_MARK);
		if ( inputState->guessing==0 ) {
#line 651 "iptables.g"
			
			*dbg << " MARK";
			
#line 3094 "IPTCfgParser.cpp"
		}
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

void IPTCfgParser::limit_rate() {
	
	try {      // for error handling
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 681 "iptables.g"
			importer->limit_val = LT(0)->getText();
#line 3114 "IPTCfgParser.cpp"
		}
		match(SLASH);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 683 "iptables.g"
			importer->limit_suffix = LT(0)->getText();
#line 3121 "IPTCfgParser.cpp"
		}
		if ( inputState->guessing==0 ) {
#line 684 "iptables.g"
			
			*dbg << " MATCH LIMIT "
			<< importer->limit_val << "/"
			<< importer->limit_suffix;
			
#line 3130 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::m_recent() {
	
	try {      // for error handling
		match(M_RECENT);
		if ( inputState->guessing==0 ) {
#line 701 "iptables.g"
			
			*dbg << " RECENT";
			
#line 3152 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_11);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::recent_opts() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case MATCH_RECENT_SET:
		case MATCH_RECENT_RCHECK:
		case MATCH_RECENT_UPDATE:
		case MATCH_RECENT_REMOVE:
		case MATCH_RECENT_RTTL:
		case RSOURCE:
		case MATCH_RECENT_RDEST:
		{
			recent_args_no_param();
			break;
		}
		case MATCH_RECENT_NAME:
		case MATCH_RECENT_SECONDS:
		case MATCH_RECENT_HITCOUNT:
		{
			recent_args_param();
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
			recover(ex,_tokenSet_12);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::recent_args_no_param() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case MATCH_RECENT_SET:
		{
			match(MATCH_RECENT_SET);
			break;
		}
		case MATCH_RECENT_RCHECK:
		{
			match(MATCH_RECENT_RCHECK);
			break;
		}
		case MATCH_RECENT_UPDATE:
		{
			match(MATCH_RECENT_UPDATE);
			break;
		}
		case MATCH_RECENT_REMOVE:
		{
			match(MATCH_RECENT_REMOVE);
			break;
		}
		case MATCH_RECENT_RTTL:
		{
			match(MATCH_RECENT_RTTL);
			break;
		}
		case RSOURCE:
		{
			match(RSOURCE);
			break;
		}
		case MATCH_RECENT_RDEST:
		{
			match(MATCH_RECENT_RDEST);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 722 "iptables.g"
			importer->recent_match += LT(0)->getText() + " ";
#line 3252 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_12);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::recent_args_param() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case MATCH_RECENT_NAME:
		{
			match(MATCH_RECENT_NAME);
			break;
		}
		case MATCH_RECENT_SECONDS:
		{
			match(MATCH_RECENT_SECONDS);
			break;
		}
		case MATCH_RECENT_HITCOUNT:
		{
			match(MATCH_RECENT_HITCOUNT);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 726 "iptables.g"
			importer->recent_match += LT(0)->getText() + " ";
#line 3294 "IPTCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
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
#line 728 "iptables.g"
			importer->recent_match += LT(0)->getText() + " ";
#line 3317 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_12);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::length_spec() {
	
	try {      // for error handling
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 745 "iptables.g"
			importer->length_spec = LT(0)->getText();
#line 3337 "IPTCfgParser.cpp"
		}
		match(COLON);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 747 "iptables.g"
			importer->length_spec += ":";
			importer->length_spec += LT(0)->getText();
#line 3345 "IPTCfgParser.cpp"
		}
		if ( inputState->guessing==0 ) {
#line 749 "iptables.g"
			
			*dbg << " MATCH LENGTH " << importer->length_spec;
			
#line 3352 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
#line 846 "iptables.g"
			
			importer->tmp_port_range_start = LT(0)->getText();
			importer->tmp_port_range_end = LT(0)->getText();
			*dbg << " PORT=" << LT(0)->getText();
			
#line 3393 "IPTCfgParser.cpp"
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
#line 864 "iptables.g"
			
			importer->tmp_port_range_start = LT(0)->getText();
			importer->tmp_port_range_end = LT(0)->getText();
			*dbg << " PORT=" << LT(0)->getText();
			
#line 3443 "IPTCfgParser.cpp"
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
			case MATCH_LIMIT:
			case MATCH_LIMIT_BURST:
			case MATCH_LENGTH:
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
#line 871 "iptables.g"
				
				importer->tmp_port_range_end = LT(0)->getText();
				*dbg << ":" << LT(0)->getText();
				
#line 3504 "IPTCfgParser.cpp"
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
		case MATCH_LIMIT:
		case MATCH_LIMIT_BURST:
		case MATCH_LENGTH:
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
			recover(ex,_tokenSet_5);
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
#line 880 "iptables.g"
			
			importer->tmp_port_range_start = "0";
			importer->tmp_port_range_end = LT(0)->getText();
			*dbg << "PORT 0:" << LT(0)->getText();
			
#line 3584 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::syn() {
	
	try {      // for error handling
		match(MATCH_SYN);
		if ( inputState->guessing==0 ) {
#line 969 "iptables.g"
			
			importer->tcp_flags_mask.clear();
			importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::SYN);
			importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::RST);
			importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::ACK);
			
			importer->tcp_flags_comp.clear();
			importer->tcp_flags_comp.push_back(libfwbuilder::TCPService::SYN);
			
#line 3612 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
#line 1020 "iptables.g"
			
			importer->tcp_flags_mask = importer->tmp_tcp_flags_list;
			importer->tmp_tcp_flags_list.clear();
			
#line 3636 "IPTCfgParser.cpp"
		}
		tcp_flags_list();
		if ( inputState->guessing==0 ) {
#line 1025 "iptables.g"
			
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
			
#line 3654 "IPTCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
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
			recover(ex,_tokenSet_5);
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
#line 982 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::SYN;
#line 3712 "IPTCfgParser.cpp"
			}
			break;
		}
		case ACK:
		{
			match(ACK);
			if ( inputState->guessing==0 ) {
#line 984 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::ACK;
#line 3722 "IPTCfgParser.cpp"
			}
			break;
		}
		case FIN:
		{
			match(FIN);
			if ( inputState->guessing==0 ) {
#line 986 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::FIN;
#line 3732 "IPTCfgParser.cpp"
			}
			break;
		}
		case RST:
		{
			match(RST);
			if ( inputState->guessing==0 ) {
#line 988 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::RST;
#line 3742 "IPTCfgParser.cpp"
			}
			break;
		}
		case URG:
		{
			match(URG);
			if ( inputState->guessing==0 ) {
#line 990 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::URG;
#line 3752 "IPTCfgParser.cpp"
			}
			break;
		}
		case PSH:
		{
			match(PSH);
			if ( inputState->guessing==0 ) {
#line 992 "iptables.g"
				importer->tmp_tcp_flag_code = libfwbuilder::TCPService::PSH;
#line 3762 "IPTCfgParser.cpp"
			}
			break;
		}
		case ALL:
		{
			match(ALL);
			if ( inputState->guessing==0 ) {
#line 994 "iptables.g"
				importer->tmp_tcp_flag_code = 99;
#line 3772 "IPTCfgParser.cpp"
			}
			break;
		}
		case NONE:
		{
			match(NONE);
			if ( inputState->guessing==0 ) {
#line 996 "iptables.g"
				importer->tmp_tcp_flag_code = 98;
#line 3782 "IPTCfgParser.cpp"
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
			recover(ex,_tokenSet_13);
		} else {
			throw;
		}
	}
}

void IPTCfgParser::tcp_flags_list() {
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 1001 "iptables.g"
			
			importer->tmp_tcp_flags_list.clear();
			importer->tmp_tcp_flag_code = 0;
			
#line 3812 "IPTCfgParser.cpp"
		}
		tcp_flag_word();
		if ( inputState->guessing==0 ) {
#line 1006 "iptables.g"
			
			importer->tmp_tcp_flags_list.push_back(importer->tmp_tcp_flag_code);
			
#line 3820 "IPTCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				tcp_flag_word();
				if ( inputState->guessing==0 ) {
#line 1011 "iptables.g"
					
					importer->tmp_tcp_flags_list.push_back(
					importer->tmp_tcp_flag_code);
					
#line 3833 "IPTCfgParser.cpp"
				}
			}
			else {
				goto _loop147;
			}
			
		}
		_loop147:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_14);
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
	"\"recent\"",
	"MATCH_RECENT_SET",
	"MATCH_RECENT_RCHECK",
	"MATCH_RECENT_UPDATE",
	"MATCH_RECENT_REMOVE",
	"MATCH_RECENT_RTTL",
	"RSOURCE",
	"MATCH_RECENT_RDEST",
	"MATCH_RECENT_NAME",
	"MATCH_RECENT_SECONDS",
	"MATCH_RECENT_HITCOUNT",
	"\"length\"",
	"MATCH_LENGTH",
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

const unsigned long IPTCfgParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_0(_tokenSet_0_data_,6);
const unsigned long IPTCfgParser::_tokenSet_1_data_[] = { 540914UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT "COMMIT" STAR COLON ADD_RULE 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_1(_tokenSet_1_data_,6);
const unsigned long IPTCfgParser::_tokenSet_2_data_[] = { 999325696UL, 67108870UL, 4287103072UL, 1537UL, 0UL, 0UL, 0UL, 0UL };
// MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST OPT_IN_INTF 
// OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_LIMIT MATCH_LIMIT_BURST 
// MATCH_LENGTH MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT 
// MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT 
// MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long IPTCfgParser::_tokenSet_3_data_[] = { 999325952UL, 67108870UL, 4287103072UL, 1537UL, 0UL, 0UL, 0UL, 0UL };
// WORD MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_LENGTH MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS 
// MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long IPTCfgParser::_tokenSet_4_data_[] = { 3221225728UL, 2147483649UL, 3407888UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD "tcp" "udp" "icmp" "state" "limit" "length" "multiport" "comment" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long IPTCfgParser::_tokenSet_5_data_[] = { 999325712UL, 67108870UL, 4287103072UL, 1537UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_LENGTH MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS 
// MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_5(_tokenSet_5_data_,8);
const unsigned long IPTCfgParser::_tokenSet_6_data_[] = { 0UL, 67104728UL, 0UL, 0UL, 0UL, 0UL };
// REJECT_WITH LOG_PREFIX LOG_TCP_SEQ LOG_TCP_OPT LOG_IP_OPT ULOG_PREFIX 
// LOG_LEVEL SET_MARK SET_TOS SAVE_MARK RESTORE_MARK CONTINUE ROUTE_IIF 
// ROUTE_OIF ROUTE_GW ROUTE_TEE TO_SOURCE TO_DESTINATION TO_PORTS TO_NETMAP 
// CLAMP_MSS 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_6(_tokenSet_6_data_,6);
const unsigned long IPTCfgParser::_tokenSet_7_data_[] = { 999325712UL, 134213598UL, 4287103072UL, 1537UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET REJECT_WITH LOG_PREFIX 
// LOG_TCP_SEQ LOG_TCP_OPT LOG_IP_OPT ULOG_PREFIX LOG_LEVEL SET_MARK SET_TOS 
// SAVE_MARK RESTORE_MARK CONTINUE ROUTE_IIF ROUTE_OIF ROUTE_GW ROUTE_TEE 
// TO_SOURCE TO_DESTINATION TO_PORTS TO_NETMAP CLAMP_MSS OPT_FRAGM MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_LENGTH MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS 
// MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_7(_tokenSet_7_data_,8);
const unsigned long IPTCfgParser::_tokenSet_8_data_[] = { 999342096UL, 134213598UL, 4287103072UL, 1537UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE COLON MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION 
// OPT_DST OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET REJECT_WITH LOG_PREFIX 
// LOG_TCP_SEQ LOG_TCP_OPT LOG_IP_OPT ULOG_PREFIX LOG_LEVEL SET_MARK SET_TOS 
// SAVE_MARK RESTORE_MARK CONTINUE ROUTE_IIF ROUTE_OIF ROUTE_GW ROUTE_TEE 
// TO_SOURCE TO_DESTINATION TO_PORTS TO_NETMAP CLAMP_MSS OPT_FRAGM MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_LENGTH MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS 
// MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_8(_tokenSet_8_data_,8);
const unsigned long IPTCfgParser::_tokenSet_9_data_[] = { 999325712UL, 67108870UL, 4287103074UL, 1537UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM COMMA MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_LENGTH MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN MATCH_TCP_FLAGS 
// MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_9(_tokenSet_9_data_,8);
const unsigned long IPTCfgParser::_tokenSet_10_data_[] = { 33554432UL, 0UL, 8UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EXCLAMATION MATCH_MARK 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_10(_tokenSet_10_data_,8);
const unsigned long IPTCfgParser::_tokenSet_11_data_[] = { 0UL, 0UL, 261888UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// MATCH_RECENT_SET MATCH_RECENT_RCHECK MATCH_RECENT_UPDATE MATCH_RECENT_REMOVE 
// MATCH_RECENT_RTTL RSOURCE MATCH_RECENT_RDEST MATCH_RECENT_NAME MATCH_RECENT_SECONDS 
// MATCH_RECENT_HITCOUNT 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_11(_tokenSet_11_data_,8);
const unsigned long IPTCfgParser::_tokenSet_12_data_[] = { 999325712UL, 67108870UL, 4287364960UL, 1537UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_RECENT_SET MATCH_RECENT_RCHECK MATCH_RECENT_UPDATE 
// MATCH_RECENT_REMOVE MATCH_RECENT_RTTL RSOURCE MATCH_RECENT_RDEST MATCH_RECENT_NAME 
// MATCH_RECENT_SECONDS MATCH_RECENT_HITCOUNT MATCH_LENGTH MATCH_SRC_MULTIPORT 
// MATCH_SRC_MULTIPORT_SHORT MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT 
// MATCH_ICMP_TYPE MATCH_SRC_PORT MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT 
// MATCH_SYN MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_12(_tokenSet_12_data_,8);
const unsigned long IPTCfgParser::_tokenSet_13_data_[] = { 999325712UL, 67108870UL, 4287103074UL, 2047UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM COMMA MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_LENGTH MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN "SYN" 
// "ACK" "FIN" "RST" "URG" "PSH" "ALL" "NONE" MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_13(_tokenSet_13_data_,8);
const unsigned long IPTCfgParser::_tokenSet_14_data_[] = { 999325712UL, 67108870UL, 4287103072UL, 2047UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE MINUS UNSUPPORTED_OPTION OPT_MODULE OPT_SRC EXCLAMATION OPT_DST 
// OPT_IN_INTF OPT_OUT_INTF OPT_PROTO OPT_TARGET OPT_FRAGM MATCH_LIMIT 
// MATCH_LIMIT_BURST MATCH_LENGTH MATCH_SRC_MULTIPORT MATCH_SRC_MULTIPORT_SHORT 
// MATCH_DST_MULTIPORT MATCH_DST_MULTIPORT_SHORT MATCH_ICMP_TYPE MATCH_SRC_PORT 
// MATCH_SRC_PORT_SHORT MATCH_DST_PORT MATCH_DST_PORT_SHORT MATCH_SYN "SYN" 
// "ACK" "FIN" "RST" "URG" "PSH" "ALL" "NONE" MATCH_TCP_FLAGS MATCH_TCP_OPTION 
const ANTLR_USE_NAMESPACE(antlr)BitSet IPTCfgParser::_tokenSet_14(_tokenSet_14_data_,8);



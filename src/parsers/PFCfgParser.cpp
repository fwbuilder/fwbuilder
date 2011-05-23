/* $ANTLR 2.7.7 (20090306): "pf.g" -> "PFCfgParser.cpp"$ */
#line 42 "pf.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "PFCfgParser.cpp"
#include "PFCfgParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 48 "pf.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#line 20 "PFCfgParser.cpp"
#line 1 "pf.g"
#line 22 "PFCfgParser.cpp"
PFCfgParser::PFCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

PFCfgParser::PFCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
}

PFCfgParser::PFCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

PFCfgParser::PFCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
}

PFCfgParser::PFCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
}

void PFCfgParser::cfgfile() {
	Tracer traceInOut(this, "cfgfile");
	
	try {      // for error handling
		{ // ( ... )*
		for (;;) {
			switch ( LA(1)) {
			case LINE_COMMENT:
			{
				comment();
				break;
			}
			case ALTQ:
			{
				altq_command();
				break;
			}
			case QUEUE:
			{
				queue_command();
				break;
			}
			case SET:
			{
				set_command();
				break;
			}
			case SCRUB:
			{
				scrub_command();
				break;
			}
			case NAT:
			{
				nat_command();
				break;
			}
			case RDR:
			{
				rdr_command();
				break;
			}
			case BINAT:
			{
				binat_command();
				break;
			}
			case PASS:
			{
				pass_command();
				break;
			}
			case BLOCK:
			{
				block_command();
				break;
			}
			case TIMEOUT:
			{
				timeout_command();
				break;
			}
			case NEWLINE:
			{
				match(NEWLINE);
				break;
			}
			default:
				if ((LA(1) == WORD) && (LA(2) == EQUAL)) {
					macro_definition();
				}
				else if ((LA(1) == WORD) && (_tokenSet_0.member(LA(2)))) {
					unknown_command();
				}
			else {
				goto _loop3;
			}
			}
		}
		_loop3:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::comment() {
	Tracer traceInOut(this, "comment");
	
	try {      // for error handling
		match(LINE_COMMENT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::macro_definition() {
	Tracer traceInOut(this, "macro_definition");
	
	try {      // for error handling
		match(WORD);
		match(EQUAL);
#line 149 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		consumeUntil(NEWLINE);
		
#line 160 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::altq_command() {
	Tracer traceInOut(this, "altq_command");
	
	try {      // for error handling
		match(ALTQ);
#line 158 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'altq' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 181 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::queue_command() {
	Tracer traceInOut(this, "queue_command");
	
	try {      // for error handling
		match(QUEUE);
#line 169 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'queue' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 202 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::set_command() {
	Tracer traceInOut(this, "set_command");
	
	try {      // for error handling
		match(SET);
#line 180 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'set' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 223 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::scrub_command() {
	Tracer traceInOut(this, "scrub_command");
	
	try {      // for error handling
		match(SCRUB);
#line 191 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'scrub' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 244 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::nat_command() {
	Tracer traceInOut(this, "nat_command");
	
	try {      // for error handling
		match(NAT);
#line 202 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'nat' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 265 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::rdr_command() {
	Tracer traceInOut(this, "rdr_command");
	
	try {      // for error handling
		match(RDR);
#line 224 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'rdr' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 286 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::binat_command() {
	Tracer traceInOut(this, "binat_command");
	
	try {      // for error handling
		match(BINAT);
#line 213 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'binat' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 307 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::pass_command() {
	Tracer traceInOut(this, "pass_command");
	
	try {      // for error handling
		match(PASS);
#line 258 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "pass";
		*dbg << LT(1)->getLine() << ":" << " pass ";
		
#line 328 "PFCfgParser.cpp"
		rule_extended();
		match(NEWLINE);
#line 266 "pf.g"
		
		importer->setInterfaceAndDirectionForRuleSet(
		"", importer->iface, importer->direction);
		importer->pushRule();
		
#line 337 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::block_command() {
	Tracer traceInOut(this, "block_command");
	
	try {      // for error handling
		match(BLOCK);
#line 274 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "block";
		*dbg << LT(1)->getLine() << ":" << " block   ";
		
#line 358 "PFCfgParser.cpp"
		rule_extended();
		match(NEWLINE);
#line 282 "pf.g"
		
		importer->setInterfaceAndDirectionForRuleSet(
		"", importer->iface, importer->direction);
		importer->pushRule();
		
#line 367 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::timeout_command() {
	Tracer traceInOut(this, "timeout_command");
	
	try {      // for error handling
		match(TIMEOUT);
#line 235 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'timeout' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 388 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::unknown_command() {
	Tracer traceInOut(this, "unknown_command");
	
	try {      // for error handling
		match(WORD);
#line 247 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		consumeUntil(NEWLINE);
		
#line 407 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::rule_extended() {
	Tracer traceInOut(this, "rule_extended");
	
	try {      // for error handling
		direction();
		{
		switch ( LA(1)) {
		case LOG:
		{
			logging();
			break;
		}
		case NEWLINE:
		case ALL:
		case TO:
		case QUICK:
		case ON:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
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
		case QUICK:
		{
			quick();
			break;
		}
		case NEWLINE:
		case ALL:
		case TO:
		case ON:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
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
		case ON:
		{
			intrface();
			break;
		}
		case NEWLINE:
		case ALL:
		case TO:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
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
		case ALL:
		case TO:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		{
			{
			switch ( LA(1)) {
			case INET:
			case INET6:
			{
				address_family();
				break;
			}
			case ALL:
			case TO:
			case PROTO:
			case FROM:
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
			case PROTO:
			{
				protospec();
				break;
			}
			case ALL:
			case TO:
			case FROM:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			hosts();
			filteropts();
			break;
		}
		case NEWLINE:
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
		recover(ex,_tokenSet_2);
	}
}

void PFCfgParser::direction() {
	Tracer traceInOut(this, "direction");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IN:
		{
			match(IN);
			break;
		}
		case OUT:
		{
			match(OUT);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 303 "pf.g"
		
		importer->direction = LT(0)->getText();
		
#line 588 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
}

void PFCfgParser::logging() {
	Tracer traceInOut(this, "logging");
	
	try {      // for error handling
		match(LOG);
		logopts();
#line 309 "pf.g"
		
		importer->logging = true;
		
#line 606 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_4);
	}
}

void PFCfgParser::quick() {
	Tracer traceInOut(this, "quick");
	
	try {      // for error handling
		match(QUICK);
#line 326 "pf.g"
		
		importer->quick = true;
		
#line 623 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_5);
	}
}

void PFCfgParser::intrface() {
	Tracer traceInOut(this, "intrface");
	
	try {      // for error handling
		match(ON);
		match(WORD);
#line 332 "pf.g"
		
		importer->iface = LT(0)->getText();
		importer->newInterface(importer->iface);
		
#line 642 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_6);
	}
}

void PFCfgParser::address_family() {
	Tracer traceInOut(this, "address_family");
	
	try {      // for error handling
		switch ( LA(1)) {
		case INET:
		{
			match(INET);
			break;
		}
		case INET6:
		{
			match(INET6);
#line 339 "pf.g"
			
			importer->address_family = LT(0)->getText();
			
#line 667 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_7);
	}
}

void PFCfgParser::protospec() {
	Tracer traceInOut(this, "protospec");
	
	try {      // for error handling
		match(PROTO);
		{
		switch ( LA(1)) {
		case IP:
		case ICMP:
		case IGMP:
		case TCP:
		case UDP:
		case RDP:
		case RSVP:
		case GRE:
		case ESP:
		case AH:
		case EIGRP:
		case OSPF:
		case IPIP:
		case VRRP:
		case L2TP:
		case ISIS:
		{
			proto_name();
			break;
		}
		case INT_CONST:
		{
			proto_number();
			break;
		}
		case OPENING_BRACE:
		{
			proto_list();
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
		recover(ex,_tokenSet_8);
	}
}

void PFCfgParser::hosts() {
	Tracer traceInOut(this, "hosts");
	
	try {      // for error handling
		switch ( LA(1)) {
		case ALL:
		{
			match(ALL);
			break;
		}
		case TO:
		case FROM:
		{
			{
			{
			switch ( LA(1)) {
			case FROM:
			{
				match(FROM);
				{
				switch ( LA(1)) {
				case WORD:
				case OPENING_BRACE:
				case ANY:
				case SELF:
				case EXCLAMATION:
				case IPV4:
				case IPV6:
				{
					src_hosts_part();
					break;
				}
				case TO:
				case PORT:
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
				case PORT:
				{
					src_port_part();
					break;
				}
				case TO:
				{
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				break;
			}
			case TO:
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
			match(TO);
			{
			switch ( LA(1)) {
			case WORD:
			case OPENING_BRACE:
			case ANY:
			case SELF:
			case EXCLAMATION:
			case IPV4:
			case IPV6:
			{
				dst_hosts_part();
				break;
			}
			case NEWLINE:
			case QUEUE:
			case COMMA:
			case NO:
			case KEEP:
			case MODULATE:
			case SYNPROXY:
			case PORT:
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
			case PORT:
			{
				dst_port_part();
				break;
			}
			case NEWLINE:
			case QUEUE:
			case COMMA:
			case NO:
			case KEEP:
			case MODULATE:
			case SYNPROXY:
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
		recover(ex,_tokenSet_9);
	}
}

void PFCfgParser::filteropts() {
	Tracer traceInOut(this, "filteropts");
	
	try {      // for error handling
		filteropt();
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				filteropt();
			}
			else {
				goto _loop65;
			}
			
		}
		_loop65:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_2);
	}
}

void PFCfgParser::logopts() {
	Tracer traceInOut(this, "logopts");
	
	try {      // for error handling
		logopt();
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				logopt();
			}
			else {
				goto _loop29;
			}
			
		}
		_loop29:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_4);
	}
}

void PFCfgParser::logopt() {
	Tracer traceInOut(this, "logopt");
	
	try {      // for error handling
		switch ( LA(1)) {
		case ALL:
		{
			match(ALL);
			break;
		}
		case USER:
		{
			match(USER);
			break;
		}
		case TO:
		{
			match(TO);
			match(WORD);
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
		recover(ex,_tokenSet_10);
	}
}

void PFCfgParser::proto_name() {
	Tracer traceInOut(this, "proto_name");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IP:
		{
			match(IP);
			break;
		}
		case ICMP:
		{
			match(ICMP);
			break;
		}
		case IGMP:
		{
			match(IGMP);
			break;
		}
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
		case RDP:
		{
			match(RDP);
			break;
		}
		case RSVP:
		{
			match(RSVP);
			break;
		}
		case GRE:
		{
			match(GRE);
			break;
		}
		case ESP:
		{
			match(ESP);
			break;
		}
		case AH:
		{
			match(AH);
			break;
		}
		case EIGRP:
		{
			match(EIGRP);
			break;
		}
		case OSPF:
		{
			match(OSPF);
			break;
		}
		case IPIP:
		{
			match(IPIP);
			break;
		}
		case VRRP:
		{
			match(VRRP);
			break;
		}
		case L2TP:
		{
			match(L2TP);
			break;
		}
		case ISIS:
		{
			match(ISIS);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 356 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 1054 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_8);
	}
}

void PFCfgParser::proto_number() {
	Tracer traceInOut(this, "proto_number");
	
	try {      // for error handling
		match(INT_CONST);
#line 362 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 1071 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_8);
	}
}

void PFCfgParser::proto_list() {
	Tracer traceInOut(this, "proto_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		protospec();
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				protospec();
			}
			else {
				goto _loop41;
			}
			
		}
		_loop41:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_8);
	}
}

void PFCfgParser::src_hosts_part() {
	Tracer traceInOut(this, "src_hosts_part");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case ANY:
		{
			match(ANY);
#line 397 "pf.g"
			
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>("0.0.0.0", "0.0.0.0"));
			
#line 1120 "PFCfgParser.cpp"
			break;
		}
		case SELF:
		{
			match(SELF);
#line 403 "pf.g"
			
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>("self", "255.255.255.255"));
			
#line 1131 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case EXCLAMATION:
		case IPV4:
		case IPV6:
		{
			host();
			break;
		}
		case OPENING_BRACE:
		{
			host_list();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 412 "pf.g"
		
		importer->src_neg = importer->tmp_neg;
		importer->src_group.splice(importer->src_group.begin(),
		importer->tmp_group);
		
#line 1159 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_11);
	}
}

void PFCfgParser::src_port_part() {
	Tracer traceInOut(this, "src_port_part");
	
	try {      // for error handling
		match(PORT);
		{
		if ((_tokenSet_12.member(LA(1))) && (LA(2) == WORD || LA(2) == TO || LA(2) == INT_CONST)) {
			unary_op();
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST) && ((LA(2) >= EXCEPT_RANGE && LA(2) <= COLON))) {
			binary_op();
		}
		else if ((LA(1) == OPENING_BRACE)) {
			op_list();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
#line 540 "pf.g"
		
		importer->src_port_group.splice(importer->src_port_group.begin(),
		importer->tmp_port_group);
		
#line 1192 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_13);
	}
}

void PFCfgParser::dst_hosts_part() {
	Tracer traceInOut(this, "dst_hosts_part");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case ANY:
		{
			match(ANY);
#line 422 "pf.g"
			
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>("0.0.0.0", "0.0.0.0"));
			
#line 1214 "PFCfgParser.cpp"
			break;
		}
		case SELF:
		{
			match(SELF);
#line 428 "pf.g"
			
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>("self", "255.255.255.255"));
			
#line 1225 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case EXCLAMATION:
		case IPV4:
		case IPV6:
		{
			host();
			break;
		}
		case OPENING_BRACE:
		{
			host_list();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 437 "pf.g"
		
		importer->dst_neg = importer->tmp_neg;
		importer->dst_group.splice(importer->src_group.begin(),
		importer->tmp_group);
		
#line 1253 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_14);
	}
}

void PFCfgParser::dst_port_part() {
	Tracer traceInOut(this, "dst_port_part");
	
	try {      // for error handling
		match(PORT);
		{
		if ((_tokenSet_12.member(LA(1))) && (_tokenSet_15.member(LA(2)))) {
			unary_op();
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST) && ((LA(2) >= EXCEPT_RANGE && LA(2) <= COLON))) {
			binary_op();
		}
		else if ((LA(1) == OPENING_BRACE)) {
			op_list();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
#line 548 "pf.g"
		
		importer->dst_port_group.splice(importer->dst_port_group.begin(),
		importer->tmp_port_group);
		
#line 1286 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_9);
	}
}

void PFCfgParser::host() {
	Tracer traceInOut(this, "host");
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXCLAMATION:
		{
			match(EXCLAMATION);
#line 448 "pf.g"
			
			importer->tmp_neg = true;
			
#line 1311 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case IPV4:
		case IPV6:
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
		case IPV4:
		case IPV6:
		{
			{
			switch ( LA(1)) {
			case IPV4:
			{
				h = LT(1);
				match(IPV4);
				break;
			}
			case IPV6:
			{
				v6 = LT(1);
				match(IPV6);
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
			case SLASH:
			{
				match(SLASH);
				{
				switch ( LA(1)) {
				case IPV4:
				{
					nm = LT(1);
					match(IPV4);
					break;
				}
				case INT_CONST:
				{
					nm6 = LT(1);
					match(INT_CONST);
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				break;
			}
			case NEWLINE:
			case QUEUE:
			case COMMA:
			case TO:
			case CLOSING_BRACE:
			case NO:
			case KEEP:
			case MODULATE:
			case SYNPROXY:
			case PORT:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 454 "pf.g"
			
			if (v6)
			{
			importer->addMessageToLog(
			QString("Warning: IPv6 import is not supported. "));
			consumeUntil(NEWLINE);
			} else
			{
			std::string addr = "0.0.0.0";
			std::string netm = "255.255.255.255";
			if (h) addr = h->getText();
			if (nm) netm = nm->getText();
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>(addr, netm));
			}
			
#line 1414 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		{
			match(WORD);
#line 472 "pf.g"
			
			// This should be an interface name
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>(
			LT(0)->getText(), "255.255.255.255"));
			
#line 1427 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_16);
	}
}

void PFCfgParser::host_list() {
	Tracer traceInOut(this, "host_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		host();
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				host();
			}
			else {
				goto _loop62;
			}
			
		}
		_loop62:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_17);
	}
}

void PFCfgParser::filteropt() {
	Tracer traceInOut(this, "filteropt");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		{
			state();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case COMMA:
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
		case QUEUE:
		{
			queue();
			break;
		}
		case NEWLINE:
		case COMMA:
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
		recover(ex,_tokenSet_18);
	}
}

void PFCfgParser::state() {
	Tracer traceInOut(this, "state");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case NO:
		{
			match(NO);
			break;
		}
		case KEEP:
		{
			match(KEEP);
			break;
		}
		case MODULATE:
		{
			match(MODULATE);
			break;
		}
		case SYNPROXY:
		{
			match(SYNPROXY);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 515 "pf.g"
		
		importer->state_op = LT(0)->getText();
		
#line 1557 "PFCfgParser.cpp"
		match(STATE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_19);
	}
}

void PFCfgParser::queue() {
	Tracer traceInOut(this, "queue");
	
	try {      // for error handling
		match(QUEUE);
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
#line 524 "pf.g"
			importer->queue += LT(0)->getText();
#line 1578 "PFCfgParser.cpp"
			break;
		}
		case OPENING_PAREN:
		{
			match(OPENING_PAREN);
#line 526 "pf.g"
			importer->queue += "(";
#line 1586 "PFCfgParser.cpp"
			match(WORD);
#line 527 "pf.g"
			importer->queue += LT(0)->getText();
#line 1590 "PFCfgParser.cpp"
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
#line 529 "pf.g"
					importer->queue += ",";
#line 1597 "PFCfgParser.cpp"
					match(WORD);
#line 530 "pf.g"
					importer->queue += LT(0)->getText();
#line 1601 "PFCfgParser.cpp"
				}
				else {
					goto _loop74;
				}
				
			}
			_loop74:;
			} // ( ... )*
			match(CLOSING_PAREN);
#line 532 "pf.g"
			importer->queue += ")";
#line 1613 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_18);
	}
}

void PFCfgParser::unary_op() {
	Tracer traceInOut(this, "unary_op");
	
	try {      // for error handling
#line 555 "pf.g"
		
		std::string op = "=";
		
#line 1637 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case EQUAL:
		case NOT_EQUAL:
		case LESS_THAN:
		case LESS_OR_EQUAL_THAN:
		case GREATER_THAN:
		case GREATER_OR_EQUAL_THAN:
		{
			{
			switch ( LA(1)) {
			case EQUAL:
			{
				match(EQUAL);
				break;
			}
			case NOT_EQUAL:
			{
				match(NOT_EQUAL);
				break;
			}
			case LESS_THAN:
			{
				match(LESS_THAN);
				break;
			}
			case LESS_OR_EQUAL_THAN:
			{
				match(LESS_OR_EQUAL_THAN);
				break;
			}
			case GREATER_THAN:
			{
				match(GREATER_THAN);
				break;
			}
			case GREATER_OR_EQUAL_THAN:
			{
				match(GREATER_OR_EQUAL_THAN);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 572 "pf.g"
			
			op = LT(0)->getText();
			
#line 1689 "PFCfgParser.cpp"
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
		port_def();
#line 577 "pf.g"
		
		std::vector<std::string> tuple;
		tuple.push_back(op);
		tuple.push_back(importer->tmp_port_def);
		importer->tmp_port_group.push_back(tuple);
		
#line 1711 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_20);
	}
}

void PFCfgParser::binary_op() {
	Tracer traceInOut(this, "binary_op");
	
	try {      // for error handling
#line 586 "pf.g"
		
		std::string op;
		std::string arg1;
		std::vector<std::string> tuple;
		
#line 1729 "PFCfgParser.cpp"
		port_def();
#line 592 "pf.g"
		
		arg1 = importer->tmp_port_def;
		
#line 1735 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case EXCEPT_RANGE:
		{
			match(EXCEPT_RANGE);
			break;
		}
		case INSIDE_RANGE:
		{
			match(INSIDE_RANGE);
			break;
		}
		case COLON:
		{
			match(COLON);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 602 "pf.g"
		
		op = LT(0)->getText();
		
#line 1763 "PFCfgParser.cpp"
		port_def();
#line 606 "pf.g"
		
		tuple.push_back(op);
		tuple.push_back(arg1);
		tuple.push_back(importer->tmp_port_def);
		importer->tmp_port_group.push_back(tuple);
		
#line 1772 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_20);
	}
}

void PFCfgParser::op_list() {
	Tracer traceInOut(this, "op_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		{
		if ((_tokenSet_12.member(LA(1))) && (_tokenSet_21.member(LA(2)))) {
			unary_op();
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST) && ((LA(2) >= EXCEPT_RANGE && LA(2) <= COLON))) {
			binary_op();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				{
				if ((_tokenSet_12.member(LA(1))) && (_tokenSet_21.member(LA(2)))) {
					unary_op();
				}
				else if ((LA(1) == WORD || LA(1) == INT_CONST) && ((LA(2) >= EXCEPT_RANGE && LA(2) <= COLON))) {
					binary_op();
				}
				else {
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				
				}
			}
			else {
				goto _loop90;
			}
			
		}
		_loop90:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_22);
	}
}

void PFCfgParser::port_def() {
	Tracer traceInOut(this, "port_def");
	
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
#line 616 "pf.g"
		
		importer->tmp_port_def = LT(0)->getText();
		
#line 1855 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_23);
	}
}

void PFCfgParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* PFCfgParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"NEWLINE",
	"LINE_COMMENT",
	"WORD",
	"EQUAL",
	"\"altq\"",
	"\"queue\"",
	"\"set\"",
	"\"scrub\"",
	"\"nat\"",
	"\"binat\"",
	"\"rdr\"",
	"\"timeout\"",
	"\"pass\"",
	"\"block\"",
	"\"in\"",
	"\"out\"",
	"\"log\"",
	"COMMA",
	"ALL",
	"USER",
	"\"to\"",
	"\"quick\"",
	"\"on\"",
	"\"inet\"",
	"\"inet6\"",
	"\"proto\"",
	"\"ip\"",
	"\"icmp\"",
	"\"igmp\"",
	"\"tcp\"",
	"\"udp\"",
	"\"rdp\"",
	"\"rsvp\"",
	"\"gre\"",
	"\"esp\"",
	"\"ah\"",
	"\"eigrp\"",
	"\"ospf\"",
	"\"ipip\"",
	"\"vrrp\"",
	"\"l2tp\"",
	"\"isis\"",
	"INT_CONST",
	"OPENING_BRACE",
	"CLOSING_BRACE",
	"\"from\"",
	"\"any\"",
	"SELF",
	"EXCLAMATION",
	"IPV4",
	"IPV6",
	"SLASH",
	"\"no\"",
	"\"keep\"",
	"\"modulate\"",
	"\"synproxy\"",
	"\"state\"",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"\"port\"",
	"\"!=\"",
	"LESS_THAN",
	"\"<=\"",
	"GREATER_THAN",
	"\">=\"",
	"\"<>\"",
	"\"><\"",
	"COLON",
	"\"exit\"",
	"\"quit\"",
	"\"interface\"",
	"\"icmp6\"",
	"\"igrp\"",
	"\"ipsec\"",
	"\"nos\"",
	"\"pcp\"",
	"\"pim\"",
	"\"pptp\"",
	"\"rip\"",
	"\"snp\"",
	"\"host\"",
	"\"range\"",
	"\"alerts\"",
	"\"critical\"",
	"\"debugging\"",
	"\"emergencies\"",
	"\"errors\"",
	"\"informational\"",
	"\"notifications\"",
	"\"warnings\"",
	"\"disable\"",
	"\"inactive\"",
	"\"->\"",
	"Whitespace",
	"HEX_CONST",
	"NUMBER",
	"NEG_INT_CONST",
	"DIGIT",
	"HEXDIGIT",
	"NUMBER_ADDRESS_OR_WORD",
	"STRING",
	"PIPE_CHAR",
	"NUMBER_SIGN",
	"PERCENT",
	"AMPERSAND",
	"APOSTROPHE",
	"STAR",
	"PLUS",
	"MINUS",
	"DOT",
	"SEMICOLON",
	"QUESTION",
	"COMMERCIAL_AT",
	"OPENING_SQUARE",
	"CLOSING_SQUARE",
	"CARET",
	"UNDERLINE",
	"TILDE",
	"EXLAMATION",
	0
};

const unsigned long PFCfgParser::_tokenSet_0_data_[] = { 262002UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "altq" "queue" "set" "scrub" "nat" "binat" 
// "rdr" "timeout" "pass" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long PFCfgParser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long PFCfgParser::_tokenSet_2_data_[] = { 16UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long PFCfgParser::_tokenSet_3_data_[] = { 1062207504UL, 131072UL, 0UL, 0UL };
// NEWLINE "log" ALL "to" "quick" "on" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long PFCfgParser::_tokenSet_4_data_[] = { 1061158928UL, 131072UL, 0UL, 0UL };
// NEWLINE ALL "to" "quick" "on" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long PFCfgParser::_tokenSet_5_data_[] = { 1027604496UL, 131072UL, 0UL, 0UL };
// NEWLINE ALL "to" "on" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long PFCfgParser::_tokenSet_6_data_[] = { 960495632UL, 131072UL, 0UL, 0UL };
// NEWLINE ALL "to" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long PFCfgParser::_tokenSet_7_data_[] = { 557842432UL, 131072UL, 0UL, 0UL };
// ALL "to" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long PFCfgParser::_tokenSet_8_data_[] = { 23068672UL, 196608UL, 0UL, 0UL };
// COMMA ALL "to" CLOSING_BRACE "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long PFCfgParser::_tokenSet_9_data_[] = { 2097680UL, 251658240UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "no" "keep" "modulate" "synproxy" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_9(_tokenSet_9_data_,4);
const unsigned long PFCfgParser::_tokenSet_10_data_[] = { 1063256080UL, 131072UL, 0UL, 0UL };
// NEWLINE COMMA ALL "to" "quick" "on" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_10(_tokenSet_10_data_,4);
const unsigned long PFCfgParser::_tokenSet_11_data_[] = { 16777216UL, 2147483648UL, 0UL, 0UL };
// "to" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_11(_tokenSet_11_data_,4);
const unsigned long PFCfgParser::_tokenSet_12_data_[] = { 192UL, 16384UL, 31UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD EQUAL INT_CONST "!=" LESS_THAN "<=" GREATER_THAN ">=" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_12(_tokenSet_12_data_,8);
const unsigned long PFCfgParser::_tokenSet_13_data_[] = { 16777216UL, 0UL, 0UL, 0UL };
// "to" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_13(_tokenSet_13_data_,4);
const unsigned long PFCfgParser::_tokenSet_14_data_[] = { 2097680UL, 2399141888UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "no" "keep" "modulate" "synproxy" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_14(_tokenSet_14_data_,4);
const unsigned long PFCfgParser::_tokenSet_15_data_[] = { 2097744UL, 251674624UL, 0UL, 0UL };
// NEWLINE WORD "queue" COMMA INT_CONST "no" "keep" "modulate" "synproxy" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_15(_tokenSet_15_data_,4);
const unsigned long PFCfgParser::_tokenSet_16_data_[] = { 18874896UL, 2399207424UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" CLOSING_BRACE "no" "keep" "modulate" "synproxy" 
// "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_16(_tokenSet_16_data_,4);
const unsigned long PFCfgParser::_tokenSet_17_data_[] = { 18874896UL, 2399141888UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" "no" "keep" "modulate" "synproxy" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_17(_tokenSet_17_data_,4);
const unsigned long PFCfgParser::_tokenSet_18_data_[] = { 2097168UL, 0UL, 0UL, 0UL };
// NEWLINE COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_18(_tokenSet_18_data_,4);
const unsigned long PFCfgParser::_tokenSet_19_data_[] = { 2097680UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_19(_tokenSet_19_data_,4);
const unsigned long PFCfgParser::_tokenSet_20_data_[] = { 18874896UL, 251723776UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" CLOSING_BRACE "no" "keep" "modulate" "synproxy" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_20(_tokenSet_20_data_,4);
const unsigned long PFCfgParser::_tokenSet_21_data_[] = { 2097216UL, 81920UL, 0UL, 0UL };
// WORD COMMA INT_CONST CLOSING_BRACE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_21(_tokenSet_21_data_,4);
const unsigned long PFCfgParser::_tokenSet_22_data_[] = { 18874896UL, 251658240UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" "no" "keep" "modulate" "synproxy" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_22(_tokenSet_22_data_,4);
const unsigned long PFCfgParser::_tokenSet_23_data_[] = { 18874896UL, 251723776UL, 224UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" CLOSING_BRACE "no" "keep" "modulate" "synproxy" 
// "<>" "><" COLON 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_23(_tokenSet_23_data_,8);



/* $ANTLR 2.7.7 (20100319): "pf.g" -> "PFCfgParser.cpp"$ */
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
			case ANTISPOOF:
			{
				antispoof_command();
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
#line 151 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		consumeUntil(NEWLINE);
		
#line 165 "PFCfgParser.cpp"
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
#line 171 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'altq' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 186 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::antispoof_command() {
	Tracer traceInOut(this, "antispoof_command");
	
	try {      // for error handling
		match(ANTISPOOF);
#line 160 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'antispoof' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 207 "PFCfgParser.cpp"
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
#line 182 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'queue' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 228 "PFCfgParser.cpp"
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
#line 193 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'set' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 249 "PFCfgParser.cpp"
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
#line 204 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'scrub' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 270 "PFCfgParser.cpp"
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
#line 215 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'nat' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 291 "PFCfgParser.cpp"
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
#line 237 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'rdr' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 312 "PFCfgParser.cpp"
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
#line 226 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'binat' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 333 "PFCfgParser.cpp"
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
#line 271 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "pass";
		*dbg << LT(1)->getLine() << ":" << " pass ";
		
#line 354 "PFCfgParser.cpp"
		rule_extended();
		match(NEWLINE);
#line 279 "pf.g"
		
		importer->setInterfaceAndDirectionForRuleSet(
		"", importer->iface, importer->direction);
		importer->pushRule();
		
#line 363 "PFCfgParser.cpp"
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
#line 287 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "block";
		*dbg << LT(1)->getLine() << ":" << " block   ";
		
#line 384 "PFCfgParser.cpp"
		rule_extended();
		match(NEWLINE);
#line 295 "pf.g"
		
		importer->setInterfaceAndDirectionForRuleSet(
		"", importer->iface, importer->direction);
		importer->pushRule();
		
#line 393 "PFCfgParser.cpp"
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
#line 248 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'timeout' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 414 "PFCfgParser.cpp"
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
#line 260 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		consumeUntil(NEWLINE);
		
#line 433 "PFCfgParser.cpp"
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
#line 316 "pf.g"
		
		importer->direction = LT(0)->getText();
		
#line 614 "PFCfgParser.cpp"
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
#line 322 "pf.g"
		
		importer->logging = true;
		
#line 632 "PFCfgParser.cpp"
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
#line 339 "pf.g"
		
		importer->quick = true;
		
#line 649 "PFCfgParser.cpp"
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
#line 345 "pf.g"
		
		importer->iface = LT(0)->getText();
		importer->newInterface(importer->iface);
		
#line 668 "PFCfgParser.cpp"
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
#line 352 "pf.g"
			
			importer->address_family = LT(0)->getText();
			
#line 693 "PFCfgParser.cpp"
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
				goto _loop66;
			}
			
		}
		_loop66:;
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
				goto _loop30;
			}
			
		}
		_loop30:;
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
#line 369 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 1080 "PFCfgParser.cpp"
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
#line 375 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 1097 "PFCfgParser.cpp"
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
				goto _loop42;
			}
			
		}
		_loop42:;
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
#line 410 "pf.g"
			
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>("0.0.0.0", "0.0.0.0"));
			
#line 1146 "PFCfgParser.cpp"
			break;
		}
		case SELF:
		{
			match(SELF);
#line 416 "pf.g"
			
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>("self", "255.255.255.255"));
			
#line 1157 "PFCfgParser.cpp"
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
#line 425 "pf.g"
		
		importer->src_neg = importer->tmp_neg;
		importer->src_group.splice(importer->src_group.begin(),
		importer->tmp_group);
		
#line 1185 "PFCfgParser.cpp"
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
		if ((_tokenSet_12.member(LA(1))) && (_tokenSet_13.member(LA(2)))) {
			unary_op();
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST || LA(1) == PORT_RANGE) && ((LA(2) >= EXCEPT_RANGE && LA(2) <= COLON))) {
			binary_op();
		}
		else if ((LA(1) == OPENING_BRACE)) {
			op_list();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
#line 553 "pf.g"
		
		importer->src_port_group.splice(importer->src_port_group.begin(),
		importer->tmp_port_group);
		
#line 1218 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_14);
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
#line 435 "pf.g"
			
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>("0.0.0.0", "0.0.0.0"));
			
#line 1240 "PFCfgParser.cpp"
			break;
		}
		case SELF:
		{
			match(SELF);
#line 441 "pf.g"
			
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>("self", "255.255.255.255"));
			
#line 1251 "PFCfgParser.cpp"
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
#line 450 "pf.g"
		
		importer->dst_neg = importer->tmp_neg;
		importer->dst_group.splice(importer->src_group.begin(),
		importer->tmp_group);
		
#line 1279 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_15);
	}
}

void PFCfgParser::dst_port_part() {
	Tracer traceInOut(this, "dst_port_part");
	
	try {      // for error handling
		match(PORT);
		{
		if ((_tokenSet_12.member(LA(1))) && (_tokenSet_16.member(LA(2)))) {
			unary_op();
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST || LA(1) == PORT_RANGE) && ((LA(2) >= EXCEPT_RANGE && LA(2) <= COLON))) {
			binary_op();
		}
		else if ((LA(1) == OPENING_BRACE)) {
			op_list();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
#line 561 "pf.g"
		
		importer->dst_port_group.splice(importer->dst_port_group.begin(),
		importer->tmp_port_group);
		
#line 1312 "PFCfgParser.cpp"
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
#line 461 "pf.g"
			
			importer->tmp_neg = true;
			
#line 1337 "PFCfgParser.cpp"
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
#line 467 "pf.g"
			
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
			
#line 1440 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		{
			match(WORD);
#line 485 "pf.g"
			
			// This should be an interface name
			importer->tmp_group.push_back(
			std::pair<std::string, std::string>(
			LT(0)->getText(), "255.255.255.255"));
			
#line 1453 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_17);
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
				goto _loop63;
			}
			
		}
		_loop63:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_18);
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
		recover(ex,_tokenSet_19);
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
#line 528 "pf.g"
		
		importer->state_op = LT(0)->getText();
		
#line 1583 "PFCfgParser.cpp"
		match(STATE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_20);
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
#line 537 "pf.g"
			importer->queue += LT(0)->getText();
#line 1604 "PFCfgParser.cpp"
			break;
		}
		case OPENING_PAREN:
		{
			match(OPENING_PAREN);
#line 539 "pf.g"
			importer->queue += "(";
#line 1612 "PFCfgParser.cpp"
			match(WORD);
#line 540 "pf.g"
			importer->queue += LT(0)->getText();
#line 1616 "PFCfgParser.cpp"
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
#line 542 "pf.g"
					importer->queue += ",";
#line 1623 "PFCfgParser.cpp"
					match(WORD);
#line 543 "pf.g"
					importer->queue += LT(0)->getText();
#line 1627 "PFCfgParser.cpp"
				}
				else {
					goto _loop75;
				}
				
			}
			_loop75:;
			} // ( ... )*
			match(CLOSING_PAREN);
#line 545 "pf.g"
			importer->queue += ")";
#line 1639 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_19);
	}
}

void PFCfgParser::unary_op() {
	Tracer traceInOut(this, "unary_op");
	
	try {      // for error handling
#line 568 "pf.g"
		
		std::string op = "=";
		
#line 1663 "PFCfgParser.cpp"
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
#line 585 "pf.g"
			
			op = LT(0)->getText();
			
#line 1715 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case INT_CONST:
		case PORT_RANGE:
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
#line 590 "pf.g"
		
		std::vector<std::string> tuple;
		tuple.push_back(op);
		tuple.push_back(importer->tmp_port_def);
		importer->tmp_port_group.push_back(tuple);
		
#line 1738 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_21);
	}
}

void PFCfgParser::binary_op() {
	Tracer traceInOut(this, "binary_op");
	
	try {      // for error handling
#line 599 "pf.g"
		
		std::string op;
		std::string arg1;
		std::vector<std::string> tuple;
		
#line 1756 "PFCfgParser.cpp"
		port_def();
#line 605 "pf.g"
		
		arg1 = importer->tmp_port_def;
		
#line 1762 "PFCfgParser.cpp"
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
#line 615 "pf.g"
		
		op = LT(0)->getText();
		
#line 1790 "PFCfgParser.cpp"
		port_def();
#line 619 "pf.g"
		
		tuple.push_back(op);
		tuple.push_back(arg1);
		tuple.push_back(importer->tmp_port_def);
		importer->tmp_port_group.push_back(tuple);
		
#line 1799 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_21);
	}
}

void PFCfgParser::op_list() {
	Tracer traceInOut(this, "op_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		{
		if ((_tokenSet_12.member(LA(1))) && (_tokenSet_22.member(LA(2)))) {
			unary_op();
		}
		else if ((LA(1) == WORD || LA(1) == INT_CONST || LA(1) == PORT_RANGE) && ((LA(2) >= EXCEPT_RANGE && LA(2) <= COLON))) {
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
				if ((_tokenSet_12.member(LA(1))) && (_tokenSet_22.member(LA(2)))) {
					unary_op();
				}
				else if ((LA(1) == WORD || LA(1) == INT_CONST || LA(1) == PORT_RANGE) && ((LA(2) >= EXCEPT_RANGE && LA(2) <= COLON))) {
					binary_op();
				}
				else {
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				
				}
			}
			else {
				goto _loop91;
			}
			
		}
		_loop91:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_23);
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
		case PORT_RANGE:
		{
			match(PORT_RANGE);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 629 "pf.g"
		
		importer->tmp_port_def = LT(0)->getText();
		
#line 1887 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_24);
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
	"\"antispoof\"",
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
	"PORT_RANGE",
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
	"HEX_DIGIT",
	"DIGIT",
	"NUM_3DIGIT",
	"NUM_HEX_4DIGIT",
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

const unsigned long PFCfgParser::_tokenSet_0_data_[] = { 524146UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "scrub" 
// "nat" "binat" "rdr" "timeout" "pass" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long PFCfgParser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long PFCfgParser::_tokenSet_2_data_[] = { 16UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long PFCfgParser::_tokenSet_3_data_[] = { 2124414992UL, 262144UL, 0UL, 0UL };
// NEWLINE "log" ALL "to" "quick" "on" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long PFCfgParser::_tokenSet_4_data_[] = { 2122317840UL, 262144UL, 0UL, 0UL };
// NEWLINE ALL "to" "quick" "on" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long PFCfgParser::_tokenSet_5_data_[] = { 2055208976UL, 262144UL, 0UL, 0UL };
// NEWLINE ALL "to" "on" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long PFCfgParser::_tokenSet_6_data_[] = { 1920991248UL, 262144UL, 0UL, 0UL };
// NEWLINE ALL "to" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long PFCfgParser::_tokenSet_7_data_[] = { 1115684864UL, 262144UL, 0UL, 0UL };
// ALL "to" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long PFCfgParser::_tokenSet_8_data_[] = { 46137344UL, 393216UL, 0UL, 0UL };
// COMMA ALL "to" CLOSING_BRACE "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long PFCfgParser::_tokenSet_9_data_[] = { 4195344UL, 503316480UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "no" "keep" "modulate" "synproxy" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_9(_tokenSet_9_data_,4);
const unsigned long PFCfgParser::_tokenSet_10_data_[] = { 2126512144UL, 262144UL, 0UL, 0UL };
// NEWLINE COMMA ALL "to" "quick" "on" "inet" "inet6" "proto" "from" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_10(_tokenSet_10_data_,4);
const unsigned long PFCfgParser::_tokenSet_11_data_[] = { 33554432UL, 0UL, 1UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "to" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_11(_tokenSet_11_data_,8);
const unsigned long PFCfgParser::_tokenSet_12_data_[] = { 192UL, 32768UL, 574UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD EQUAL INT_CONST "!=" LESS_THAN "<=" GREATER_THAN ">=" PORT_RANGE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_12(_tokenSet_12_data_,8);
const unsigned long PFCfgParser::_tokenSet_13_data_[] = { 33554496UL, 32768UL, 512UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD "to" INT_CONST PORT_RANGE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_13(_tokenSet_13_data_,8);
const unsigned long PFCfgParser::_tokenSet_14_data_[] = { 33554432UL, 0UL, 0UL, 0UL };
// "to" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_14(_tokenSet_14_data_,4);
const unsigned long PFCfgParser::_tokenSet_15_data_[] = { 4195344UL, 503316480UL, 1UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "no" "keep" "modulate" "synproxy" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_15(_tokenSet_15_data_,8);
const unsigned long PFCfgParser::_tokenSet_16_data_[] = { 4195408UL, 503349248UL, 512UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD "queue" COMMA INT_CONST "no" "keep" "modulate" "synproxy" 
// PORT_RANGE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_16(_tokenSet_16_data_,8);
const unsigned long PFCfgParser::_tokenSet_17_data_[] = { 37749776UL, 503447552UL, 1UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" CLOSING_BRACE "no" "keep" "modulate" "synproxy" 
// "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_17(_tokenSet_17_data_,8);
const unsigned long PFCfgParser::_tokenSet_18_data_[] = { 37749776UL, 503316480UL, 1UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" "no" "keep" "modulate" "synproxy" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_18(_tokenSet_18_data_,8);
const unsigned long PFCfgParser::_tokenSet_19_data_[] = { 4194320UL, 0UL, 0UL, 0UL };
// NEWLINE COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_19(_tokenSet_19_data_,4);
const unsigned long PFCfgParser::_tokenSet_20_data_[] = { 4195344UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_20(_tokenSet_20_data_,4);
const unsigned long PFCfgParser::_tokenSet_21_data_[] = { 37749776UL, 503447552UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" CLOSING_BRACE "no" "keep" "modulate" "synproxy" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_21(_tokenSet_21_data_,4);
const unsigned long PFCfgParser::_tokenSet_22_data_[] = { 4194368UL, 163840UL, 512UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA INT_CONST CLOSING_BRACE PORT_RANGE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_22(_tokenSet_22_data_,8);
const unsigned long PFCfgParser::_tokenSet_23_data_[] = { 37749776UL, 503316480UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" "no" "keep" "modulate" "synproxy" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_23(_tokenSet_23_data_,4);
const unsigned long PFCfgParser::_tokenSet_24_data_[] = { 37749776UL, 503447552UL, 448UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "to" CLOSING_BRACE "no" "keep" "modulate" "synproxy" 
// "<>" "><" COLON 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_24(_tokenSet_24_data_,8);



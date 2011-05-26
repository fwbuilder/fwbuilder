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
			case TABLE:
			{
				table_command();
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
#line 153 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		consumeUntil(NEWLINE);
		
#line 170 "PFCfgParser.cpp"
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
#line 173 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'altq' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 191 "PFCfgParser.cpp"
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
#line 162 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'antispoof' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 212 "PFCfgParser.cpp"
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
#line 184 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'queue' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 233 "PFCfgParser.cpp"
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
#line 195 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'set' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 254 "PFCfgParser.cpp"
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
#line 206 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'scrub' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 275 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::table_command() {
	Tracer traceInOut(this, "table_command");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  file = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(TABLE);
#line 218 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		
#line 295 "PFCfgParser.cpp"
		match(LESS_THAN);
		name = LT(1);
		match(WORD);
		match(GREATER_THAN);
		{
		switch ( LA(1)) {
		case PERSIST:
		{
			match(PERSIST);
			break;
		}
		case CONST:
		case COUNTERS:
		case FILE:
		case OPENING_BRACE:
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
		case CONST:
		{
			match(CONST);
			break;
		}
		case COUNTERS:
		case FILE:
		case OPENING_BRACE:
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
		case COUNTERS:
		{
			match(COUNTERS);
			break;
		}
		case FILE:
		case OPENING_BRACE:
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
		case FILE:
		{
			match(FILE);
			file = LT(1);
			match(STRING);
#line 230 "pf.g"
			
			importer->newAddressTableObject(
			name->getText(), file->getText());
			
#line 369 "PFCfgParser.cpp"
			break;
		}
		case OPENING_BRACE:
		{
			match(OPENING_BRACE);
			tableaddr_spec();
			{ // ( ... )*
			for (;;) {
				if ((_tokenSet_2.member(LA(1)))) {
					{
					switch ( LA(1)) {
					case COMMA:
					{
						match(COMMA);
						break;
					}
					case WORD:
					case EXLAMATION:
					case SELF:
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
					tableaddr_spec();
				}
				else {
					goto _loop18;
				}
				
			}
			_loop18:;
			} // ( ... )*
			match(CLOSING_BRACE);
#line 242 "pf.g"
			
			importer->newAddressTableObject(
			name->getText(), importer->tmp_group);
			
#line 414 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::nat_command() {
	Tracer traceInOut(this, "nat_command");
	
	try {      // for error handling
		match(NAT);
#line 287 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'nat' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 443 "PFCfgParser.cpp"
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
#line 309 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'rdr' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 464 "PFCfgParser.cpp"
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
#line 298 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'binat' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 485 "PFCfgParser.cpp"
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
#line 343 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "pass";
		*dbg << LT(1)->getLine() << ":" << " pass ";
		
#line 506 "PFCfgParser.cpp"
		rule_extended();
		match(NEWLINE);
#line 351 "pf.g"
		
		importer->pushRule();
		
#line 513 "PFCfgParser.cpp"
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
#line 357 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "block";
		*dbg << LT(1)->getLine() << ":" << " block   ";
		
#line 534 "PFCfgParser.cpp"
		rule_extended();
		match(NEWLINE);
#line 365 "pf.g"
		
		importer->pushRule();
		
#line 541 "PFCfgParser.cpp"
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
#line 320 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'timeout' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 562 "PFCfgParser.cpp"
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
#line 332 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		consumeUntil(NEWLINE);
		
#line 581 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::tableaddr_spec() {
	Tracer traceInOut(this, "tableaddr_spec");
#line 249 "pf.g"
	AddressSpec as;
#line 593 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 250 "pf.g"
			as.neg = true;
#line 603 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case SELF:
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
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
#line 253 "pf.g"
			
			as.at = AddressSpec::INTERFACE_NAME;
			as.address = LT(0)->getText();
			
#line 628 "PFCfgParser.cpp"
			break;
		}
		case SELF:
		{
			match(SELF);
#line 259 "pf.g"
			
			as.at = AddressSpec::SPECIAL_ADDRESS;
			as.address = "self";
			
#line 639 "PFCfgParser.cpp"
			break;
		}
		case IPV4:
		{
			match(IPV4);
#line 265 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 650 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 271 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 660 "PFCfgParser.cpp"
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
#line 275 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 683 "PFCfgParser.cpp"
				break;
			}
			case WORD:
			case COMMA:
			case CLOSING_BRACE:
			case EXLAMATION:
			case SELF:
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
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 280 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 713 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
}

void PFCfgParser::rule_extended() {
	Tracer traceInOut(this, "rule_extended");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IN:
		case OUT:
		{
			direction();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case LOG:
		case ALL:
		case TO:
		case QUICK:
		case ON:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		case LOG:
		{
			logging();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case ALL:
		case TO:
		case QUICK:
		case ON:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		case QUEUE:
		case ALL:
		case TO:
		case ON:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		case QUEUE:
		case ALL:
		case TO:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		case ROUTE_TO:
		case REPLY_TO:
		{
			route();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case ALL:
		case TO:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		case INET:
		case INET6:
		{
			address_family();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case ALL:
		case TO:
		case PROTO:
		case FROM:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		case NEWLINE:
		case QUEUE:
		case ALL:
		case TO:
		case FROM:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		if ((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2)))) {
			hosts();
		}
		else if ((_tokenSet_6.member(LA(1))) && (_tokenSet_7.member(LA(2)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		{
		switch ( LA(1)) {
		case QUEUE:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
		{
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
		recover(ex,_tokenSet_8);
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
#line 383 "pf.g"
		
		importer->direction = LT(0)->getText();
		
#line 1046 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_9);
	}
}

void PFCfgParser::logging() {
	Tracer traceInOut(this, "logging");
	
	try {      // for error handling
		match(LOG);
		{
		switch ( LA(1)) {
		case OPENING_PAREN:
		{
			logopts();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case ALL:
		case TO:
		case QUICK:
		case ON:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 390 "pf.g"
		
		importer->logging = true;
		
#line 1101 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_10);
	}
}

void PFCfgParser::quick() {
	Tracer traceInOut(this, "quick");
	
	try {      // for error handling
		match(QUICK);
#line 412 "pf.g"
		
		importer->quick = true;
		
#line 1118 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_11);
	}
}

void PFCfgParser::intrface() {
	Tracer traceInOut(this, "intrface");
	
	try {      // for error handling
		match(ON);
		{
		switch ( LA(1)) {
		case WORD:
		case EXLAMATION:
		{
			ifspec();
			break;
		}
		case OPENING_BRACE:
		{
			interface_list();
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
		recover(ex,_tokenSet_12);
	}
}

void PFCfgParser::route() {
	Tracer traceInOut(this, "route");
	
	try {      // for error handling
		switch ( LA(1)) {
		case ROUTE_TO:
		{
			route_to();
			break;
		}
		case REPLY_TO:
		{
			reply_to();
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
		recover(ex,_tokenSet_13);
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
#line 442 "pf.g"
			
			importer->address_family = LT(0)->getText();
			
#line 1201 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_14);
	}
}

void PFCfgParser::protospec() {
	Tracer traceInOut(this, "protospec");
	
	try {      // for error handling
		match(PROTO);
		proto_def();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_4);
	}
}

void PFCfgParser::hosts() {
	Tracer traceInOut(this, "hosts");
	
	try {      // for error handling
		switch ( LA(1)) {
		case ALL:
		{
			match(ALL);
#line 485 "pf.g"
			
			importer->src_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			importer->dst_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			
#line 1244 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
		case QUEUE:
		case TO:
		case FROM:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
		{
			{
			switch ( LA(1)) {
			case FROM:
			{
				hosts_from();
				break;
			}
			case NEWLINE:
			case QUEUE:
			case TO:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
			case NO:
			case KEEP:
			case MODULATE:
			case SYNPROXY:
			case LABEL:
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
			case TO:
			{
				hosts_to();
				break;
			}
			case NEWLINE:
			case QUEUE:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
			case NO:
			case KEEP:
			case MODULATE:
			case SYNPROXY:
			case LABEL:
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
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_6);
	}
}

void PFCfgParser::filteropts() {
	Tracer traceInOut(this, "filteropts");
	
	try {      // for error handling
		filteropt();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_15.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case QUEUE:
				case FLAGS:
				case ICMP_TYPE:
				case ICMP6_TYPE:
				case TAGGED:
				case TAG:
				case NO:
				case KEEP:
				case MODULATE:
				case SYNPROXY:
				case LABEL:
				{
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				filteropt();
			}
			else {
				goto _loop106;
			}
			
		}
		_loop106:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_8);
	}
}

void PFCfgParser::logopts() {
	Tracer traceInOut(this, "logopts");
	
	try {      // for error handling
		match(OPENING_PAREN);
		logopt();
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
#line 399 "pf.g"
				importer->logopts += ",";
#line 1396 "PFCfgParser.cpp"
				logopt();
			}
			else {
				goto _loop47;
			}
			
		}
		_loop47:;
		} // ( ... )*
		match(CLOSING_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_10);
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
#line 406 "pf.g"
			
			importer->logopts += LT(0)->getText();
			
#line 1437 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_16);
	}
}

void PFCfgParser::ifspec() {
	Tracer traceInOut(this, "ifspec");
#line 420 "pf.g"
	InterfaceSpec is;
#line 1456 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 421 "pf.g"
			is.neg = true;
#line 1466 "PFCfgParser.cpp"
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
		match(WORD);
#line 423 "pf.g"
		
		is.name = LT(0)->getText();
		importer->iface_group.push_back(is);
		importer->newInterface(is.name);
		
#line 1486 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_17);
	}
}

void PFCfgParser::interface_list() {
	Tracer traceInOut(this, "interface_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		ifspec();
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == WORD || LA(1) == COMMA || LA(1) == EXLAMATION)) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case WORD:
				case EXLAMATION:
				{
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				ifspec();
			}
			else {
				goto _loop57;
			}
			
		}
		_loop57:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_12);
	}
}

void PFCfgParser::proto_def() {
	Tracer traceInOut(this, "proto_def");
	
	try {      // for error handling
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
		recover(ex,_tokenSet_18);
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
#line 462 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 1683 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_18);
	}
}

void PFCfgParser::proto_number() {
	Tracer traceInOut(this, "proto_number");
	
	try {      // for error handling
		match(INT_CONST);
#line 468 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 1700 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_18);
	}
}

void PFCfgParser::proto_list() {
	Tracer traceInOut(this, "proto_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		proto_def();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_19.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case OPENING_BRACE:
				case INT_CONST:
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
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				proto_def();
			}
			else {
				goto _loop68;
			}
			
		}
		_loop68:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_18);
	}
}

void PFCfgParser::hosts_from() {
	Tracer traceInOut(this, "hosts_from");
	
	try {      // for error handling
		match(FROM);
		{
		switch ( LA(1)) {
		case WORD:
		case LESS_THAN:
		case OPENING_BRACE:
		case EXLAMATION:
		case SELF:
		case IPV4:
		case URPF_FAILED:
		case ANY:
		case NO_ROUTE:
		case IPV6:
		{
			src_hosts_part();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case TO:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		case NEWLINE:
		case QUEUE:
		case TO:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		recover(ex,_tokenSet_20);
	}
}

void PFCfgParser::hosts_to() {
	Tracer traceInOut(this, "hosts_to");
	
	try {      // for error handling
		match(TO);
		{
		switch ( LA(1)) {
		case WORD:
		case LESS_THAN:
		case OPENING_BRACE:
		case EXLAMATION:
		case SELF:
		case IPV4:
		case ANY:
		case NO_ROUTE:
		case IPV6:
		{
			dst_hosts_part();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
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
		recover(ex,_tokenSet_6);
	}
}

void PFCfgParser::src_hosts_part() {
	Tracer traceInOut(this, "src_hosts_part");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case WORD:
		case LESS_THAN:
		case OPENING_BRACE:
		case EXLAMATION:
		case SELF:
		case IPV4:
		case ANY:
		case NO_ROUTE:
		case IPV6:
		{
			common_hosts_part();
			break;
		}
		case URPF_FAILED:
		{
			match(URPF_FAILED);
#line 508 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::SPECIAL_ADDRESS, false,
			"urpf-failed", ""));
			
#line 1953 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 514 "pf.g"
		
		importer->src_neg = importer->tmp_neg;
		importer->src_group.splice(importer->src_group.begin(),
		importer->tmp_group);
		
#line 1968 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_21);
	}
}

void PFCfgParser::src_port_part() {
	Tracer traceInOut(this, "src_port_part");
	
	try {      // for error handling
		match(PORT);
		{
		switch ( LA(1)) {
		case WORD:
		case EQUAL:
		case LESS_THAN:
		case GREATER_THAN:
		case EXLAMATION:
		case INT_CONST:
		{
			port_op();
			break;
		}
		case OPENING_BRACE:
		{
			port_op_list();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 800 "pf.g"
		
		importer->src_port_group.splice(importer->src_port_group.begin(),
		importer->tmp_port_group);
		
#line 2009 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_20);
	}
}

void PFCfgParser::dst_hosts_part() {
	Tracer traceInOut(this, "dst_hosts_part");
	
	try {      // for error handling
		common_hosts_part();
#line 523 "pf.g"
		
		importer->dst_neg = importer->tmp_neg;
		importer->dst_group.splice(importer->dst_group.begin(),
		importer->tmp_group);
		
#line 2028 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_22);
	}
}

void PFCfgParser::dst_port_part() {
	Tracer traceInOut(this, "dst_port_part");
	
	try {      // for error handling
		match(PORT);
		{
		switch ( LA(1)) {
		case WORD:
		case EQUAL:
		case LESS_THAN:
		case GREATER_THAN:
		case EXLAMATION:
		case INT_CONST:
		{
			port_op();
			break;
		}
		case OPENING_BRACE:
		{
			port_op_list();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 808 "pf.g"
		
		importer->dst_port_group.splice(importer->dst_port_group.begin(),
		importer->tmp_port_group);
		
#line 2069 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_6);
	}
}

void PFCfgParser::common_hosts_part() {
	Tracer traceInOut(this, "common_hosts_part");
	
	try {      // for error handling
		switch ( LA(1)) {
		case ANY:
		{
			match(ANY);
#line 532 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			
#line 2090 "PFCfgParser.cpp"
			break;
		}
		case NO_ROUTE:
		{
			match(NO_ROUTE);
#line 538 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::SPECIAL_ADDRESS, false, "no-route", ""));
			
#line 2101 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case LESS_THAN:
		case EXLAMATION:
		case SELF:
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
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_21);
	}
}

void PFCfgParser::host() {
	Tracer traceInOut(this, "host");
	ANTLR_USE_NAMESPACE(antlr)RefToken  tn = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 548 "pf.g"
	AddressSpec as;
#line 2136 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 549 "pf.g"
			as.neg = true;
#line 2146 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case LESS_THAN:
		case SELF:
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
		case WORD:
		{
			match(WORD);
#line 552 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_NAME;
			as.address = LT(0)->getText();
			
#line 2174 "PFCfgParser.cpp"
			break;
		}
		case SELF:
		{
			match(SELF);
#line 559 "pf.g"
			
			as.at = AddressSpec::SPECIAL_ADDRESS;
			as.address = "self";
			
#line 2185 "PFCfgParser.cpp"
			break;
		}
		case IPV6:
		{
			match(IPV6);
#line 565 "pf.g"
			
			importer->addMessageToLog(
			QString("Error: IPv6 import is not supported. "));
			consumeUntil(NEWLINE);
			
#line 2197 "PFCfgParser.cpp"
			break;
		}
		case IPV4:
		{
			match(IPV4);
#line 572 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 2208 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 578 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 2218 "PFCfgParser.cpp"
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
#line 582 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 2241 "PFCfgParser.cpp"
				break;
			}
			case NEWLINE:
			case QUEUE:
			case COMMA:
			case CLOSING_BRACE:
			case TO:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
			case NO:
			case KEEP:
			case MODULATE:
			case SYNPROXY:
			case LABEL:
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
			break;
		}
		case LESS_THAN:
		{
			match(LESS_THAN);
			tn = LT(1);
			match(WORD);
			match(GREATER_THAN);
#line 588 "pf.g"
			
			as.at = AddressSpec::TABLE;
			as.address = tn->getText();
			
#line 2282 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 593 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 2295 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_23);
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
				goto _loop89;
			}
			
		}
		_loop89:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_21);
	}
}

void PFCfgParser::route_to() {
	Tracer traceInOut(this, "route_to");
	
	try {      // for error handling
		match(ROUTE_TO);
		{
		switch ( LA(1)) {
		case OPENING_PAREN:
		{
			routehost();
			break;
		}
		case OPENING_BRACE:
		{
			routehost_list();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 615 "pf.g"
		
		importer->route_type = PFImporter::ROUTE_TO;
		
#line 2357 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_13);
	}
}

void PFCfgParser::reply_to() {
	Tracer traceInOut(this, "reply_to");
	
	try {      // for error handling
		match(REPLY_TO);
		{
		switch ( LA(1)) {
		case OPENING_PAREN:
		{
			routehost();
			break;
		}
		case OPENING_BRACE:
		{
			routehost_list();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 622 "pf.g"
		
		importer->route_type = PFImporter::REPLY_TO;
		
#line 2392 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_13);
	}
}

void PFCfgParser::routehost() {
	Tracer traceInOut(this, "routehost");
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 627 "pf.g"
	RouteSpec rs;
#line 2408 "PFCfgParser.cpp"
	
	try {      // for error handling
		match(OPENING_PAREN);
		match(WORD);
#line 629 "pf.g"
		rs.iface = LT(0)->getText();
#line 2415 "PFCfgParser.cpp"
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
		case CLOSING_PAREN:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 631 "pf.g"
		
		if (v6)
		{
		importer->addMessageToLog(
		QString("Error: IPv6 import is not supported. "));
		consumeUntil(NEWLINE);
		} else
		{
		if (h) rs.address = h->getText();
		if (nm) rs.netmask = nm->getText();
		importer->route_group.push_back(rs);
		}
		
#line 2487 "PFCfgParser.cpp"
		match(CLOSING_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_24);
	}
}

void PFCfgParser::routehost_list() {
	Tracer traceInOut(this, "routehost_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		routehost();
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA || LA(1) == OPENING_PAREN)) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case OPENING_PAREN:
				{
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				routehost();
			}
			else {
				goto _loop102;
			}
			
		}
		_loop102:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_13);
	}
}

void PFCfgParser::filteropt() {
	Tracer traceInOut(this, "filteropt");
	
	try {      // for error handling
		switch ( LA(1)) {
		case FLAGS:
		{
			tcp_flags();
			break;
		}
		case ICMP_TYPE:
		{
			icmp_type();
			break;
		}
		case ICMP6_TYPE:
		{
			icmp6_type();
			break;
		}
		case TAGGED:
		{
			tagged();
			break;
		}
		case TAG:
		{
			tag_clause();
			break;
		}
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		{
			state();
			break;
		}
		case QUEUE:
		{
			queue();
			break;
		}
		case LABEL:
		{
			label();
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
		recover(ex,_tokenSet_25);
	}
}

void PFCfgParser::tcp_flags() {
	Tracer traceInOut(this, "tcp_flags");
	ANTLR_USE_NAMESPACE(antlr)RefToken  check = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  mask = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(FLAGS);
		{
		switch ( LA(1)) {
		case ANY:
		{
			match(ANY);
#line 688 "pf.g"
			
			importer->flags_check = "any";
			importer->flags_mask = "all";
			
#line 2616 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case SLASH:
		{
			{
			switch ( LA(1)) {
			case WORD:
			{
				check = LT(1);
				match(WORD);
				break;
			}
			case SLASH:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(SLASH);
			{
			switch ( LA(1)) {
			case WORD:
			{
				mask = LT(1);
				match(WORD);
				break;
			}
			case NEWLINE:
			case QUEUE:
			case COMMA:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
			case NO:
			case KEEP:
			case MODULATE:
			case SYNPROXY:
			case LABEL:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 694 "pf.g"
			
			if (check)
			importer->flags_check = check->getText();
			else
			importer->flags_check = "any";
			if (mask)
			importer->flags_mask = mask->getText();
			else
			importer->flags_mask = "all";
			
#line 2682 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_25);
	}
}

void PFCfgParser::icmp_type() {
	Tracer traceInOut(this, "icmp_type");
	
	try {      // for error handling
		match(ICMP_TYPE);
		{
		switch ( LA(1)) {
		case WORD:
		case INT_CONST:
		{
			icmp_type_code();
			break;
		}
		case OPENING_BRACE:
		{
			icmp_list();
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
		recover(ex,_tokenSet_25);
	}
}

void PFCfgParser::icmp6_type() {
	Tracer traceInOut(this, "icmp6_type");
	
	try {      // for error handling
		match(ICMP6_TYPE);
#line 740 "pf.g"
		
		importer->addMessageToLog(
		QString("Error: ICMP6 import is not supported. "));
		consumeUntil(NEWLINE);
		
#line 2740 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_25);
	}
}

void PFCfgParser::tagged() {
	Tracer traceInOut(this, "tagged");
	
	try {      // for error handling
		match(TAGGED);
		match(WORD);
#line 749 "pf.g"
		
		importer->tagged = LT(0)->getText();
		
#line 2758 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_25);
	}
}

void PFCfgParser::tag_clause() {
	Tracer traceInOut(this, "tag_clause");
	
	try {      // for error handling
		match(TAG);
		match(WORD);
#line 756 "pf.g"
		
		importer->tag = LT(0)->getText();
		
#line 2776 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_25);
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
#line 771 "pf.g"
		
		importer->state_op = LT(0)->getText();
		
#line 2820 "PFCfgParser.cpp"
		match(STATE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_25);
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
#line 780 "pf.g"
			importer->queue += LT(0)->getText();
#line 2841 "PFCfgParser.cpp"
			break;
		}
		case OPENING_PAREN:
		{
			match(OPENING_PAREN);
			match(WORD);
#line 783 "pf.g"
			importer->queue += LT(0)->getText();
#line 2850 "PFCfgParser.cpp"
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
#line 785 "pf.g"
					importer->queue += ",";
#line 2857 "PFCfgParser.cpp"
					match(WORD);
#line 786 "pf.g"
					importer->queue += LT(0)->getText();
#line 2861 "PFCfgParser.cpp"
				}
				else {
					goto _loop130;
				}
				
			}
			_loop130:;
			} // ( ... )*
			match(CLOSING_PAREN);
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
		recover(ex,_tokenSet_25);
	}
}

void PFCfgParser::label() {
	Tracer traceInOut(this, "label");
	
	try {      // for error handling
		match(LABEL);
		match(STRING);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_25);
	}
}

void PFCfgParser::icmp_type_code() {
	Tracer traceInOut(this, "icmp_type_code");
#line 716 "pf.g"
	std::string icmp_type, icmp_code;
#line 2903 "PFCfgParser.cpp"
	
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
#line 717 "pf.g"
		icmp_type = LT(0)->getText();
#line 2926 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case ICMP_CODE:
		{
			match(ICMP_CODE);
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
#line 719 "pf.g"
			icmp_code = LT(0)->getText();
#line 2952 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
		case WORD:
		case QUEUE:
		case COMMA:
		case CLOSING_BRACE:
		case INT_CONST:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
		case NO:
		case KEEP:
		case MODULATE:
		case SYNPROXY:
		case LABEL:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 721 "pf.g"
		
		importer->icmp_type_code_group.push_back(
		str_tuple(icmp_type, icmp_code));
		
#line 2985 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_26);
	}
}

void PFCfgParser::icmp_list() {
	Tracer traceInOut(this, "icmp_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		icmp_type_code();
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == WORD || LA(1) == COMMA || LA(1) == INT_CONST)) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
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
				icmp_type_code();
			}
			else {
				goto _loop121;
			}
			
		}
		_loop121:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_25);
	}
}

void PFCfgParser::port_op() {
	Tracer traceInOut(this, "port_op");
#line 840 "pf.g"
	PortSpec ps;
#line 3041 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EQUAL:
		case LESS_THAN:
		case GREATER_THAN:
		case EXLAMATION:
		{
			unary_port_op();
#line 842 "pf.g"
			ps.port_op = importer->tmp_port_op;
#line 3054 "PFCfgParser.cpp"
			port_def();
#line 844 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = importer->tmp_port_def;
			
#line 3061 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case INT_CONST:
		{
			port_def();
#line 850 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = ps.port1;
			ps.port_op = "=";
			
#line 3074 "PFCfgParser.cpp"
			{
			if ((LA(1) == LESS_THAN || LA(1) == GREATER_THAN || LA(1) == COLON) && (_tokenSet_27.member(LA(2)))) {
				binary_port_op();
#line 856 "pf.g"
				ps.port_op = importer->tmp_port_op;
#line 3080 "PFCfgParser.cpp"
				port_def();
#line 857 "pf.g"
				ps.port2 = LT(0)->getText();
#line 3084 "PFCfgParser.cpp"
			}
			else if ((_tokenSet_28.member(LA(1))) && (_tokenSet_29.member(LA(2)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
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
#line 860 "pf.g"
		
		importer->tmp_port_group.push_back(ps);
		
#line 3105 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_28);
	}
}

void PFCfgParser::port_op_list() {
	Tracer traceInOut(this, "port_op_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		port_op();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_30.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case WORD:
				case EQUAL:
				case LESS_THAN:
				case GREATER_THAN:
				case EXLAMATION:
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
				port_op();
			}
			else {
				goto _loop147;
			}
			
		}
		_loop147:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_20);
	}
}

void PFCfgParser::unary_port_op() {
	Tracer traceInOut(this, "unary_port_op");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EQUAL:
		{
			match(EQUAL);
#line 816 "pf.g"
			importer->tmp_port_op = "=";
#line 3172 "PFCfgParser.cpp"
			break;
		}
		case EXLAMATION:
		{
			match(EXLAMATION);
			match(EQUAL);
#line 818 "pf.g"
			importer->tmp_port_op = "!=";
#line 3181 "PFCfgParser.cpp"
			break;
		}
		default:
			if ((LA(1) == LESS_THAN) && (LA(2) == WORD || LA(2) == INT_CONST)) {
				match(LESS_THAN);
#line 820 "pf.g"
				importer->tmp_port_op = "<";
#line 3189 "PFCfgParser.cpp"
			}
			else if ((LA(1) == LESS_THAN) && (LA(2) == EQUAL)) {
				match(LESS_THAN);
				match(EQUAL);
#line 822 "pf.g"
				importer->tmp_port_op = "<=";
#line 3196 "PFCfgParser.cpp"
			}
			else if ((LA(1) == GREATER_THAN) && (LA(2) == WORD || LA(2) == INT_CONST)) {
				match(GREATER_THAN);
#line 824 "pf.g"
				importer->tmp_port_op = ">";
#line 3202 "PFCfgParser.cpp"
			}
			else if ((LA(1) == GREATER_THAN) && (LA(2) == EQUAL)) {
				match(GREATER_THAN);
				match(EQUAL);
#line 826 "pf.g"
				importer->tmp_port_op = ">=";
#line 3209 "PFCfgParser.cpp"
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_31);
	}
}

void PFCfgParser::binary_port_op() {
	Tracer traceInOut(this, "binary_port_op");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case LESS_THAN:
		{
			match(LESS_THAN);
			match(GREATER_THAN);
#line 832 "pf.g"
			importer->tmp_port_op = "<>";
#line 3235 "PFCfgParser.cpp"
			break;
		}
		case GREATER_THAN:
		{
			match(GREATER_THAN);
			match(LESS_THAN);
#line 834 "pf.g"
			importer->tmp_port_op = "><";
#line 3244 "PFCfgParser.cpp"
			break;
		}
		case COLON:
		{
			match(COLON);
#line 836 "pf.g"
			importer->tmp_port_op = ":";
#line 3252 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_31);
	}
}

void PFCfgParser::port_def() {
	Tracer traceInOut(this, "port_def");
	
	try {      // for error handling
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
			break;
		}
		case INT_CONST:
		{
			match(INT_CONST);
#line 867 "pf.g"
			
			importer->tmp_port_def = LT(0)->getText();
			
#line 3285 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_32);
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
	"\"table\"",
	"LESS_THAN",
	"GREATER_THAN",
	"\"persist\"",
	"\"const\"",
	"COUNTERS",
	"\"file\"",
	"STRING",
	"OPENING_BRACE",
	"COMMA",
	"CLOSING_BRACE",
	"EXLAMATION",
	"\"self\"",
	"IPV4",
	"SLASH",
	"INT_CONST",
	"\"nat\"",
	"\"binat\"",
	"\"rdr\"",
	"\"timeout\"",
	"\"pass\"",
	"\"block\"",
	"\"in\"",
	"\"out\"",
	"\"log\"",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"\"all\"",
	"\"user\"",
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
	"\"from\"",
	"\"urpf-failed\"",
	"\"any\"",
	"\"no-route\"",
	"IPV6",
	"\"route-to\"",
	"\"reply-to\"",
	"\"flags\"",
	"\"icmp-type\"",
	"\"code\"",
	"\"icmp6-type\"",
	"\"tagged\"",
	"\"tag\"",
	"\"no\"",
	"\"keep\"",
	"\"modulate\"",
	"\"synproxy\"",
	"\"state\"",
	"\"label\"",
	"\"port\"",
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
	"HEX_DIGIT",
	"DIGIT",
	"NUM_3DIGIT",
	"NUM_HEX_4DIGIT",
	"NUMBER_ADDRESS_OR_WORD",
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
	"DOUBLE_QUOTE",
	0
};

const unsigned long PFCfgParser::_tokenSet_0_data_[] = { 3758112626UL, 7UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "scrub" 
// "table" "nat" "binat" "rdr" "timeout" "pass" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_0(_tokenSet_0_data_,6);
const unsigned long PFCfgParser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_1(_tokenSet_1_data_,6);
const unsigned long PFCfgParser::_tokenSet_2_data_[] = { 121634880UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA EXLAMATION "self" IPV4 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_2(_tokenSet_2_data_,6);
const unsigned long PFCfgParser::_tokenSet_3_data_[] = { 130023488UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA CLOSING_BRACE EXLAMATION "self" IPV4 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_3(_tokenSet_3_data_,6);
const unsigned long PFCfgParser::_tokenSet_4_data_[] = { 1040UL, 1280UL, 392577UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "all" "to" "from" "flags" "icmp-type" "icmp6-type" "tagged" 
// "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long PFCfgParser::_tokenSet_5_data_[] = { 4285562738UL, 1095UL, 1047966UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "scrub" 
// "table" LESS_THAN STRING OPENING_BRACE COMMA EXLAMATION "self" IPV4 
// SLASH INT_CONST "nat" "binat" "rdr" "timeout" "pass" "block" OPENING_PAREN 
// "to" "urpf-failed" "any" "no-route" IPV6 "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "state" "label" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_5(_tokenSet_5_data_,8);
const unsigned long PFCfgParser::_tokenSet_6_data_[] = { 1040UL, 0UL, 392576UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "flags" "icmp-type" "icmp6-type" "tagged" "tag" "no" 
// "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_6(_tokenSet_6_data_,8);
const unsigned long PFCfgParser::_tokenSet_7_data_[] = { 4168105842UL, 71UL, 523652UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "scrub" 
// "table" STRING OPENING_BRACE COMMA SLASH INT_CONST "nat" "binat" "rdr" 
// "timeout" "pass" "block" OPENING_PAREN "any" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "state" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_7(_tokenSet_7_data_,8);
const unsigned long PFCfgParser::_tokenSet_8_data_[] = { 16UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_8(_tokenSet_8_data_,6);
const unsigned long PFCfgParser::_tokenSet_9_data_[] = { 1040UL, 64800UL, 392673UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "log" "all" "to" "quick" "on" "inet" "inet6" "proto" 
// "from" "route-to" "reply-to" "flags" "icmp-type" "icmp6-type" "tagged" 
// "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_9(_tokenSet_9_data_,8);
const unsigned long PFCfgParser::_tokenSet_10_data_[] = { 1040UL, 64768UL, 392673UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "all" "to" "quick" "on" "inet" "inet6" "proto" "from" 
// "route-to" "reply-to" "flags" "icmp-type" "icmp6-type" "tagged" "tag" 
// "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_10(_tokenSet_10_data_,8);
const unsigned long PFCfgParser::_tokenSet_11_data_[] = { 1040UL, 62720UL, 392673UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "all" "to" "on" "inet" "inet6" "proto" "from" "route-to" 
// "reply-to" "flags" "icmp-type" "icmp6-type" "tagged" "tag" "no" "keep" 
// "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_11(_tokenSet_11_data_,8);
const unsigned long PFCfgParser::_tokenSet_12_data_[] = { 1040UL, 58624UL, 392673UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "all" "to" "inet" "inet6" "proto" "from" "route-to" 
// "reply-to" "flags" "icmp-type" "icmp6-type" "tagged" "tag" "no" "keep" 
// "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_12(_tokenSet_12_data_,8);
const unsigned long PFCfgParser::_tokenSet_13_data_[] = { 1040UL, 58624UL, 392577UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "all" "to" "inet" "inet6" "proto" "from" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_13(_tokenSet_13_data_,8);
const unsigned long PFCfgParser::_tokenSet_14_data_[] = { 1040UL, 34048UL, 392577UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "all" "to" "proto" "from" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_14(_tokenSet_14_data_,8);
const unsigned long PFCfgParser::_tokenSet_15_data_[] = { 4195328UL, 0UL, 392576UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "queue" COMMA "flags" "icmp-type" "icmp6-type" "tagged" "tag" "no" "keep" 
// "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_15(_tokenSet_15_data_,8);
const unsigned long PFCfgParser::_tokenSet_16_data_[] = { 4194304UL, 128UL, 0UL, 0UL, 0UL, 0UL };
// COMMA CLOSING_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_16(_tokenSet_16_data_,6);
const unsigned long PFCfgParser::_tokenSet_17_data_[] = { 29361232UL, 58624UL, 392673UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD "queue" COMMA CLOSING_BRACE EXLAMATION "all" "to" "inet" 
// "inet6" "proto" "from" "route-to" "reply-to" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_17(_tokenSet_17_data_,8);
const unsigned long PFCfgParser::_tokenSet_18_data_[] = { 283116560UL, 4294903040UL, 392577UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" OPENING_BRACE COMMA CLOSING_BRACE INT_CONST "all" "to" 
// "ip" "icmp" "igmp" "tcp" "udp" "rdp" "rsvp" "gre" "esp" "ah" "eigrp" 
// "ospf" "ipip" "vrrp" "l2tp" "isis" "from" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_18(_tokenSet_18_data_,8);
const unsigned long PFCfgParser::_tokenSet_19_data_[] = { 274726912UL, 4294901760UL, 0UL, 0UL, 0UL, 0UL };
// OPENING_BRACE COMMA INT_CONST "ip" "icmp" "igmp" "tcp" "udp" "rdp" "rsvp" 
// "gre" "esp" "ah" "eigrp" "ospf" "ipip" "vrrp" "l2tp" "isis" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_19(_tokenSet_19_data_,6);
const unsigned long PFCfgParser::_tokenSet_20_data_[] = { 1040UL, 1024UL, 392576UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "to" "flags" "icmp-type" "icmp6-type" "tagged" "tag" 
// "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_20(_tokenSet_20_data_,8);
const unsigned long PFCfgParser::_tokenSet_21_data_[] = { 1040UL, 1024UL, 916864UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "to" "flags" "icmp-type" "icmp6-type" "tagged" "tag" 
// "no" "keep" "modulate" "synproxy" "label" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_21(_tokenSet_21_data_,8);
const unsigned long PFCfgParser::_tokenSet_22_data_[] = { 1040UL, 0UL, 916864UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "flags" "icmp-type" "icmp6-type" "tagged" "tag" "no" 
// "keep" "modulate" "synproxy" "label" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_22(_tokenSet_22_data_,8);
const unsigned long PFCfgParser::_tokenSet_23_data_[] = { 12583952UL, 1024UL, 916864UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA CLOSING_BRACE "to" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" "port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_23(_tokenSet_23_data_,8);
const unsigned long PFCfgParser::_tokenSet_24_data_[] = { 12583952UL, 58688UL, 392577UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA CLOSING_BRACE OPENING_PAREN "all" "to" "inet" 
// "inet6" "proto" "from" "flags" "icmp-type" "icmp6-type" "tagged" "tag" 
// "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_24(_tokenSet_24_data_,8);
const unsigned long PFCfgParser::_tokenSet_25_data_[] = { 4195344UL, 0UL, 392576UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "flags" "icmp-type" "icmp6-type" "tagged" "tag" 
// "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_25(_tokenSet_25_data_,8);
const unsigned long PFCfgParser::_tokenSet_26_data_[] = { 281019472UL, 0UL, 392576UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD "queue" COMMA CLOSING_BRACE INT_CONST "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_26(_tokenSet_26_data_,8);
const unsigned long PFCfgParser::_tokenSet_27_data_[] = { 268484672UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD LESS_THAN GREATER_THAN INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_27(_tokenSet_27_data_,6);
const unsigned long PFCfgParser::_tokenSet_28_data_[] = { 297845968UL, 1024UL, 392576UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD EQUAL "queue" LESS_THAN GREATER_THAN COMMA CLOSING_BRACE 
// EXLAMATION INT_CONST "to" "flags" "icmp-type" "icmp6-type" "tagged" 
// "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_28(_tokenSet_28_data_,8);
const unsigned long PFCfgParser::_tokenSet_29_data_[] = { 4293984242UL, 1095UL, 2096540UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD EQUAL "antispoof" "altq" "queue" "set" 
// "scrub" "table" LESS_THAN GREATER_THAN STRING OPENING_BRACE COMMA CLOSING_BRACE 
// EXLAMATION "self" IPV4 SLASH INT_CONST "nat" "binat" "rdr" "timeout" 
// "pass" "block" OPENING_PAREN "to" "any" "no-route" IPV6 "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "state" 
// "label" "port" COLON 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_29(_tokenSet_29_data_,8);
const unsigned long PFCfgParser::_tokenSet_30_data_[] = { 289456320UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD EQUAL LESS_THAN GREATER_THAN COMMA EXLAMATION INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_30(_tokenSet_30_data_,6);
const unsigned long PFCfgParser::_tokenSet_31_data_[] = { 268435520UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_31(_tokenSet_31_data_,6);
const unsigned long PFCfgParser::_tokenSet_32_data_[] = { 297845968UL, 1024UL, 1441152UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD EQUAL "queue" LESS_THAN GREATER_THAN COMMA CLOSING_BRACE 
// EXLAMATION INT_CONST "to" "flags" "icmp-type" "icmp6-type" "tagged" 
// "tag" "no" "keep" "modulate" "synproxy" "label" COLON 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_32(_tokenSet_32_data_,8);



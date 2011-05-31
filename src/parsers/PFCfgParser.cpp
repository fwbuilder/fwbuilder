/* $ANTLR 2.7.7 (20090306): "pf.g" -> "PFCfgParser.cpp"$ */
#line 43 "pf.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "PFCfgParser.cpp"
#include "PFCfgParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 49 "pf.g"

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
			case WORD:
			{
				macro_definition();
				break;
			}
			case ALTQ:
			{
				altq_rule();
				break;
			}
			case ANTISPOOF:
			{
				antispoof_rule();
				break;
			}
			case QUEUE:
			{
				queue_rule();
				break;
			}
			case SET:
			{
				set_rule();
				break;
			}
			case SCRUB:
			{
				scrub_rule();
				break;
			}
			case TABLE:
			{
				table_rule();
				break;
			}
			case NAT:
			{
				nat_rule();
				break;
			}
			case RDR:
			{
				rdr_rule();
				break;
			}
			case BINAT:
			{
				binat_rule();
				break;
			}
			case PASS:
			{
				pass_rule();
				break;
			}
			case BLOCK:
			{
				block_rule();
				break;
			}
			case TIMEOUT:
			{
				timeout_rule();
				break;
			}
			case NEWLINE:
			{
				match(NEWLINE);
				break;
			}
			default:
			{
				goto _loop3;
			}
			}
		}
		_loop3:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void PFCfgParser::comment() {
	Tracer traceInOut(this, "comment");
	
	try {      // for error handling
		match(LINE_COMMENT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::macro_definition() {
	Tracer traceInOut(this, "macro_definition");
	
	try {      // for error handling
		match(WORD);
		match(EQUAL);
#line 154 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		consumeUntil(NEWLINE);
		
#line 169 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::altq_rule() {
	Tracer traceInOut(this, "altq_rule");
	
	try {      // for error handling
		match(ALTQ);
#line 174 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->error_tracker->registerError(
		QString("import of 'altq' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 190 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::antispoof_rule() {
	Tracer traceInOut(this, "antispoof_rule");
	
	try {      // for error handling
		match(ANTISPOOF);
#line 163 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'antispoof' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 211 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::queue_rule() {
	Tracer traceInOut(this, "queue_rule");
	
	try {      // for error handling
		match(QUEUE);
#line 185 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->error_tracker->registerError(
		QString("import of 'queue' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 232 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_rule() {
	Tracer traceInOut(this, "set_rule");
	
	try {      // for error handling
		match(SET);
#line 196 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'set' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 253 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::scrub_rule() {
	Tracer traceInOut(this, "scrub_rule");
	
	try {      // for error handling
		match(SCRUB);
#line 207 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'scrub' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 274 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::table_rule() {
	Tracer traceInOut(this, "table_rule");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  file = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(TABLE);
#line 219 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		
#line 294 "PFCfgParser.cpp"
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
#line 231 "pf.g"
			
			importer->newAddressTableObject(
			name->getText(), file->getText());
			
#line 368 "PFCfgParser.cpp"
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
#line 243 "pf.g"
			
			importer->newAddressTableObject(
			name->getText(), importer->tmp_group);
			
#line 413 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::nat_rule() {
	Tracer traceInOut(this, "nat_rule");
	
	try {      // for error handling
		match(NAT);
#line 314 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newNATRule();
		importer->action = "nat";
		*dbg << LT(1)->getLine() << ":" << " nat ";
		
#line 442 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case PASS:
		{
			match(PASS);
#line 323 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat pass' commands is not supported."));
			
#line 453 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case LOG:
			{
				logging();
				break;
			}
			case EXLAMATION:
			case MINUS:
			case ALL:
			case TO:
			case ON:
			case INET:
			case INET6:
			case PROTO:
			case FROM:
			case TAGGED:
			case TAG:
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
		case EXLAMATION:
		case MINUS:
		case ALL:
		case TO:
		case ON:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case TAGGED:
		case TAG:
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
		case EXLAMATION:
		case MINUS:
		case ALL:
		case TO:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case TAGGED:
		case TAG:
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
		case EXLAMATION:
		case MINUS:
		case ALL:
		case TO:
		case PROTO:
		case FROM:
		case TAGGED:
		case TAG:
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
		case EXLAMATION:
		case MINUS:
		case ALL:
		case TO:
		case FROM:
		case TAGGED:
		case TAG:
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
		{
		switch ( LA(1)) {
		case EXLAMATION:
		case TAGGED:
		{
			tagged();
			break;
		}
		case MINUS:
		case TAG:
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
		case TAG:
		{
			tag_clause();
#line 336 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat ... tag' commands is not supported."));
			
#line 607 "PFCfgParser.cpp"
			break;
		}
		case MINUS:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(MINUS);
		match(GREATER_THAN);
		{
		switch ( LA(1)) {
		case WORD:
		case IPV4:
		case OPENING_PAREN:
		{
			redirhost();
			break;
		}
		case OPENING_BRACE:
		{
			redirhost_list();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 343 "pf.g"
		
		importer->nat_group = importer->tmp_group;
		
#line 646 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case PORT:
		{
			portspec();
#line 348 "pf.g"
			
			importer->nat_port_group = importer->tmp_port_group;
			
#line 656 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
		case STATIC_PORT:
		case BITMASK:
		case RANDOM:
		case SOURCE_HASH:
		case ROUND_ROBIN:
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
		case BITMASK:
		case RANDOM:
		case SOURCE_HASH:
		case ROUND_ROBIN:
		{
			pooltype();
			break;
		}
		case NEWLINE:
		case STATIC_PORT:
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
		case STATIC_PORT:
		{
			match(STATIC_PORT);
#line 354 "pf.g"
			importer->nat_rule_opt_2 = "static-port";
#line 702 "PFCfgParser.cpp"
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
#line 356 "pf.g"
		
		importer->pushRule();
		
#line 719 "PFCfgParser.cpp"
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::rdr_rule() {
	Tracer traceInOut(this, "rdr_rule");
	
	try {      // for error handling
		match(RDR);
#line 495 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'rdr' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 741 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::binat_rule() {
	Tracer traceInOut(this, "binat_rule");
	
	try {      // for error handling
		match(BINAT);
#line 484 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->error_tracker->registerError(
		QString("import of 'binat' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 762 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::pass_rule() {
	Tracer traceInOut(this, "pass_rule");
	
	try {      // for error handling
		match(PASS);
#line 529 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "pass";
		*dbg << LT(1)->getLine() << ":" << " pass ";
		
#line 783 "PFCfgParser.cpp"
		rule_extended();
#line 537 "pf.g"
		
		importer->pushRule();
		
#line 789 "PFCfgParser.cpp"
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::block_rule() {
	Tracer traceInOut(this, "block_rule");
	
	try {      // for error handling
		match(BLOCK);
#line 544 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "block";
		*dbg << LT(1)->getLine() << ":" << " block   ";
		
#line 811 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case DROP:
		case RETURN:
		case RETURN_RST:
		case RETURN_ICMP:
		case RETURN_ICMP6:
		{
			block_return();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case EXLAMATION:
		case IN:
		case OUT:
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
		rule_extended();
#line 553 "pf.g"
		
		importer->pushRule();
		
#line 863 "PFCfgParser.cpp"
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::timeout_rule() {
	Tracer traceInOut(this, "timeout_rule");
	
	try {      // for error handling
		match(TIMEOUT);
#line 506 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'timeout' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 885 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::tableaddr_spec() {
	Tracer traceInOut(this, "tableaddr_spec");
#line 250 "pf.g"
	AddressSpec as;
#line 897 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 251 "pf.g"
			as.neg = true;
#line 907 "PFCfgParser.cpp"
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
#line 254 "pf.g"
			
			as.at = AddressSpec::INTERFACE_NAME;
			as.address = LT(0)->getText();
			
#line 932 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case COLON:
			{
				match(COLON);
				{
				switch ( LA(1)) {
				case NETWORK:
				{
					match(NETWORK);
#line 262 "pf.g"
					
					as.at = AddressSpec::INTERFACE_NETWORK;
					
#line 947 "PFCfgParser.cpp"
					break;
				}
				case BROADCAST:
				{
					match(BROADCAST);
#line 267 "pf.g"
					
					as.at = AddressSpec::INTERFACE_BROADCAST;
					
#line 957 "PFCfgParser.cpp"
					break;
				}
				case PEER:
				{
					match(PEER);
#line 272 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:peer' is not supported."));
					
#line 968 "PFCfgParser.cpp"
					break;
				}
				case INT_CONST:
				{
					match(INT_CONST);
#line 278 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:0' is not supported."));
					
#line 979 "PFCfgParser.cpp"
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
		case SELF:
		{
			match(SELF);
#line 286 "pf.g"
			
			as.at = AddressSpec::SPECIAL_ADDRESS;
			as.address = "self";
			
#line 1015 "PFCfgParser.cpp"
			break;
		}
		case IPV4:
		{
			match(IPV4);
#line 292 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 1026 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 298 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 1036 "PFCfgParser.cpp"
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
#line 302 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 1059 "PFCfgParser.cpp"
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
#line 307 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 1089 "PFCfgParser.cpp"
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
		{
		switch ( LA(1)) {
		case OPENING_PAREN:
		{
			logopts();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case EXLAMATION:
		case MINUS:
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
#line 619 "pf.g"
		
		importer->logging = true;
		
#line 1146 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_4);
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
		recover(ex,_tokenSet_5);
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
#line 671 "pf.g"
			
			importer->address_family = LT(0)->getText();
			
#line 1202 "PFCfgParser.cpp"
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

void PFCfgParser::protospec() {
	Tracer traceInOut(this, "protospec");
	
	try {      // for error handling
		match(PROTO);
		proto_def();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_7);
	}
}

void PFCfgParser::hosts() {
	Tracer traceInOut(this, "hosts");
	
	try {      // for error handling
		switch ( LA(1)) {
		case ALL:
		{
			match(ALL);
#line 714 "pf.g"
			
			importer->src_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			importer->dst_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			
#line 1245 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
		case QUEUE:
		case EXLAMATION:
		case MINUS:
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
			case EXLAMATION:
			case MINUS:
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
			case EXLAMATION:
			case MINUS:
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
		recover(ex,_tokenSet_8);
	}
}

void PFCfgParser::tagged() {
	Tracer traceInOut(this, "tagged");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 1008 "pf.g"
			importer->tagged_neg = true;
#line 1351 "PFCfgParser.cpp"
			break;
		}
		case TAGGED:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(TAGGED);
		match(WORD);
#line 1010 "pf.g"
		
		importer->tagged = LT(0)->getText();
		
#line 1370 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_9);
	}
}

void PFCfgParser::tag_clause() {
	Tracer traceInOut(this, "tag_clause");
	
	try {      // for error handling
		match(TAG);
		match(WORD);
#line 1017 "pf.g"
		
		importer->tag = LT(0)->getText();
		
#line 1388 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_9);
	}
}

void PFCfgParser::redirhost() {
	Tracer traceInOut(this, "redirhost");
#line 367 "pf.g"
	AddressSpec as;
#line 1400 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IPV4:
		{
			match(IPV4);
#line 370 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 1413 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 376 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 1423 "PFCfgParser.cpp"
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
#line 380 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 1446 "PFCfgParser.cpp"
				break;
			}
			case NEWLINE:
			case WORD:
			case COMMA:
			case CLOSING_BRACE:
			case IPV4:
			case STATIC_PORT:
			case OPENING_PAREN:
			case PORT:
			case BITMASK:
			case RANDOM:
			case SOURCE_HASH:
			case ROUND_ROBIN:
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
		case OPENING_PAREN:
		{
			match(OPENING_PAREN);
			match(WORD);
#line 387 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_NAME;
			as.address = LT(0)->getText();
			
#line 1482 "PFCfgParser.cpp"
			match(CLOSING_PAREN);
			break;
		}
		case WORD:
		{
			match(WORD);
#line 395 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_NAME;
			as.address = LT(0)->getText();
			
#line 1495 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 401 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 1508 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_10);
	}
}

void PFCfgParser::redirhost_list() {
	Tracer traceInOut(this, "redirhost_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		redirhost();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_11.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case WORD:
				case IPV4:
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
				redirhost();
			}
			else {
				goto _loop45;
			}
			
		}
		_loop45:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_12);
	}
}

void PFCfgParser::portspec() {
	Tracer traceInOut(this, "portspec");
#line 426 "pf.g"
	PortSpec ps;
#line 1565 "PFCfgParser.cpp"
	
	try {      // for error handling
		match(PORT);
		port_def();
#line 429 "pf.g"
		
		ps.port1 = importer->tmp_port_def;
		ps.port2 = ps.port1;
		ps.port_op = "=";
		
#line 1576 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case COLON:
		{
			match(COLON);
#line 435 "pf.g"
			ps.port_op = ":";
#line 1584 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case STAR:
			{
				match(STAR);
#line 437 "pf.g"
				ps.port2 = "65535";
#line 1592 "PFCfgParser.cpp"
				break;
			}
			case WORD:
			case INT_CONST:
			{
				port_def();
#line 439 "pf.g"
				ps.port2 = importer->tmp_port_def;
#line 1601 "PFCfgParser.cpp"
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
		case STATIC_PORT:
		case BITMASK:
		case RANDOM:
		case SOURCE_HASH:
		case ROUND_ROBIN:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 442 "pf.g"
		
		importer->tmp_port_group.push_back(ps);
		
#line 1631 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_13);
	}
}

void PFCfgParser::pooltype() {
	Tracer traceInOut(this, "pooltype");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case BITMASK:
		{
			match(BITMASK);
#line 456 "pf.g"
			importer->nat_rule_opt_1 = "bitmask";
#line 1650 "PFCfgParser.cpp"
			break;
		}
		case RANDOM:
		{
			match(RANDOM);
#line 458 "pf.g"
			importer->nat_rule_opt_1 = "random";
#line 1658 "PFCfgParser.cpp"
			break;
		}
		case SOURCE_HASH:
		{
			match(SOURCE_HASH);
#line 460 "pf.g"
			importer->nat_rule_opt_1 = "source-hash";
#line 1666 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case HEX_KEY:
			{
				match(HEX_KEY);
#line 463 "pf.g"
				
				importer->error_tracker->registerError(
				QString("import of 'nat' commands with 'source-hash hex-key' "
				"option is not supported"));
				
#line 1678 "PFCfgParser.cpp"
				break;
			}
			case STRING_KEY:
			{
				match(STRING_KEY);
#line 470 "pf.g"
				
				importer->error_tracker->registerError(
				QString("import of 'nat' commands with 'source-hash string-key' "
				"option is not supported"));
				
#line 1690 "PFCfgParser.cpp"
				break;
			}
			case NEWLINE:
			case STATIC_PORT:
			case STICKY_ADDRESS:
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
		case ROUND_ROBIN:
		{
			match(ROUND_ROBIN);
#line 477 "pf.g"
			importer->nat_rule_opt_1 = "round-robin";
#line 1712 "PFCfgParser.cpp"
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
		case STICKY_ADDRESS:
		{
			match(STICKY_ADDRESS);
			break;
		}
		case NEWLINE:
		case STATIC_PORT:
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
		recover(ex,_tokenSet_14);
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
#line 1128 "pf.g"
		
		importer->tmp_port_def = LT(0)->getText();
		
#line 1772 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_15);
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
		case EXLAMATION:
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
		case EXLAMATION:
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
		case EXLAMATION:
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
		case EXLAMATION:
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
		case EXLAMATION:
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
		case EXLAMATION:
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
		case EXLAMATION:
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
		if ((_tokenSet_16.member(LA(1))) && (_tokenSet_17.member(LA(2)))) {
			hosts();
		}
		else if ((_tokenSet_18.member(LA(1))) && (_tokenSet_19.member(LA(2)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		{
		switch ( LA(1)) {
		case QUEUE:
		case EXLAMATION:
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
		recover(ex,_tokenSet_20);
	}
}

void PFCfgParser::block_return() {
	Tracer traceInOut(this, "block_return");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case DROP:
		{
			match(DROP);
#line 561 "pf.g"
			importer->block_action_params.push_back("drop");
#line 2098 "PFCfgParser.cpp"
			break;
		}
		case RETURN:
		{
			match(RETURN);
#line 563 "pf.g"
			importer->block_action_params.push_back("return");
#line 2106 "PFCfgParser.cpp"
			break;
		}
		case RETURN_RST:
		{
			match(RETURN_RST);
#line 565 "pf.g"
			importer->block_action_params.push_back("return-rst");
#line 2114 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case TTL:
			{
				match(TTL);
				match(INT_CONST);
#line 568 "pf.g"
				
				importer->error_tracker->registerError(
				QString("Import of \"block return-rst ttl number\" is not supported. "));
				
#line 2126 "PFCfgParser.cpp"
				break;
			}
			case NEWLINE:
			case QUEUE:
			case EXLAMATION:
			case IN:
			case OUT:
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
			break;
		}
		case RETURN_ICMP:
		{
			match(RETURN_ICMP);
#line 574 "pf.g"
			importer->block_action_params.push_back("return-icmp");
#line 2171 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case OPENING_PAREN:
			{
				match(OPENING_PAREN);
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
#line 578 "pf.g"
				importer->block_action_params.push_back(LT(0)->getText());
#line 2197 "PFCfgParser.cpp"
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
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
#line 582 "pf.g"
					
					importer->error_tracker->registerError(
					QString("Import of \"block return-icmp (icmp_code, icmp6_code)\" is not supported"));
					
#line 2226 "PFCfgParser.cpp"
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
				match(CLOSING_PAREN);
				break;
			}
			case NEWLINE:
			case QUEUE:
			case EXLAMATION:
			case IN:
			case OUT:
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
			break;
		}
		case RETURN_ICMP6:
		{
			match(RETURN_ICMP6);
#line 591 "pf.g"
			
			importer->error_tracker->registerError(
			QString("Import of \"block return-icmp6\" is not supported"));
			importer->block_action_params.push_back("return-icmp");
			
#line 2288 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_21);
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
#line 612 "pf.g"
		
		importer->direction = LT(0)->getText();
		
#line 2330 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_22);
	}
}

void PFCfgParser::quick() {
	Tracer traceInOut(this, "quick");
	
	try {      // for error handling
		match(QUICK);
#line 641 "pf.g"
		
		importer->quick = true;
		
#line 2347 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_23);
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
		recover(ex,_tokenSet_24);
	}
}

void PFCfgParser::filteropts() {
	Tracer traceInOut(this, "filteropts");
	
	try {      // for error handling
		filteropt();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_25.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case QUEUE:
				case EXLAMATION:
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
				goto _loop141;
			}
			
		}
		_loop141:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_20);
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
#line 628 "pf.g"
				importer->logopts += ",";
#line 2446 "PFCfgParser.cpp"
				logopt();
			}
			else {
				goto _loop82;
			}
			
		}
		_loop82:;
		} // ( ... )*
		match(CLOSING_PAREN);
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
#line 635 "pf.g"
			
			importer->logopts += LT(0)->getText();
			
#line 2487 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_26);
	}
}

void PFCfgParser::ifspec() {
	Tracer traceInOut(this, "ifspec");
#line 649 "pf.g"
	InterfaceSpec is;
#line 2506 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 650 "pf.g"
			is.neg = true;
#line 2516 "PFCfgParser.cpp"
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
#line 652 "pf.g"
		
		is.name = LT(0)->getText();
		importer->iface_group.push_back(is);
		importer->newInterface(is.name);
		
#line 2536 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_27);
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
				goto _loop92;
			}
			
		}
		_loop92:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_5);
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
		recover(ex,_tokenSet_28);
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
#line 691 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 2733 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_28);
	}
}

void PFCfgParser::proto_number() {
	Tracer traceInOut(this, "proto_number");
	
	try {      // for error handling
		match(INT_CONST);
#line 697 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 2750 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_28);
	}
}

void PFCfgParser::proto_list() {
	Tracer traceInOut(this, "proto_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		proto_def();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_29.member(LA(1)))) {
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
				goto _loop103;
			}
			
		}
		_loop103:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_28);
	}
}

void PFCfgParser::hosts_from() {
	Tracer traceInOut(this, "hosts_from");
	
	try {      // for error handling
		match(FROM);
		src_hosts_part();
		{
		switch ( LA(1)) {
		case PORT:
		{
			src_port_part();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case EXLAMATION:
		case MINUS:
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
		recover(ex,_tokenSet_30);
	}
}

void PFCfgParser::hosts_to() {
	Tracer traceInOut(this, "hosts_to");
	
	try {      // for error handling
		match(TO);
		dst_hosts_part();
		{
		switch ( LA(1)) {
		case PORT:
		{
			dst_port_part();
			break;
		}
		case NEWLINE:
		case QUEUE:
		case EXLAMATION:
		case MINUS:
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
		recover(ex,_tokenSet_8);
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
#line 737 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::SPECIAL_ADDRESS, false,
			"urpf-failed", ""));
			
#line 2933 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 743 "pf.g"
		
		importer->src_neg = importer->tmp_neg;
		importer->src_group.splice(importer->src_group.begin(),
		importer->tmp_group);
		
#line 2948 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_31);
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
#line 1061 "pf.g"
		
		importer->src_port_group.splice(importer->src_port_group.begin(),
		importer->tmp_port_group);
		
#line 2989 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_30);
	}
}

void PFCfgParser::dst_hosts_part() {
	Tracer traceInOut(this, "dst_hosts_part");
	
	try {      // for error handling
		common_hosts_part();
#line 752 "pf.g"
		
		importer->dst_neg = importer->tmp_neg;
		importer->dst_group.splice(importer->dst_group.begin(),
		importer->tmp_group);
		
#line 3008 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_32);
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
#line 1069 "pf.g"
		
		importer->dst_port_group.splice(importer->dst_port_group.begin(),
		importer->tmp_port_group);
		
#line 3049 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_8);
	}
}

void PFCfgParser::common_hosts_part() {
	Tracer traceInOut(this, "common_hosts_part");
	
	try {      // for error handling
		switch ( LA(1)) {
		case ANY:
		{
			match(ANY);
#line 761 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			
#line 3070 "PFCfgParser.cpp"
			break;
		}
		case NO_ROUTE:
		{
			match(NO_ROUTE);
#line 767 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::SPECIAL_ADDRESS, false, "no-route", ""));
			
#line 3081 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_31);
	}
}

void PFCfgParser::host() {
	Tracer traceInOut(this, "host");
	ANTLR_USE_NAMESPACE(antlr)RefToken  tn = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 777 "pf.g"
	AddressSpec as;
#line 3116 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 778 "pf.g"
			as.neg = true;
#line 3126 "PFCfgParser.cpp"
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
#line 781 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_NAME;
			as.address = LT(0)->getText();
			
#line 3154 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case COLON:
			{
				match(COLON);
				{
				switch ( LA(1)) {
				case NETWORK:
				{
					match(NETWORK);
#line 790 "pf.g"
					
					as.at = AddressSpec::INTERFACE_NETWORK;
					
#line 3169 "PFCfgParser.cpp"
					break;
				}
				case BROADCAST:
				{
					match(BROADCAST);
#line 795 "pf.g"
					
					as.at = AddressSpec::INTERFACE_BROADCAST;
					
#line 3179 "PFCfgParser.cpp"
					break;
				}
				case PEER:
				{
					match(PEER);
#line 800 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:peer' is not supported."));
					
#line 3190 "PFCfgParser.cpp"
					break;
				}
				case INT_CONST:
				{
					match(INT_CONST);
#line 806 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:0' is not supported."));
					
#line 3201 "PFCfgParser.cpp"
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
			case CLOSING_BRACE:
			case EXLAMATION:
			case MINUS:
			case PORT:
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
			break;
		}
		case SELF:
		{
			match(SELF);
#line 814 "pf.g"
			
			as.at = AddressSpec::SPECIAL_ADDRESS;
			as.address = "self";
			
#line 3249 "PFCfgParser.cpp"
			break;
		}
		case IPV6:
		{
			match(IPV6);
#line 820 "pf.g"
			
			importer->addMessageToLog(QString("IPv6 import is not supported. "));
			consumeUntil(NEWLINE);
			
#line 3260 "PFCfgParser.cpp"
			break;
		}
		case IPV4:
		{
			match(IPV4);
#line 826 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 3271 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 832 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 3281 "PFCfgParser.cpp"
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
#line 836 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 3304 "PFCfgParser.cpp"
				break;
			}
			case NEWLINE:
			case QUEUE:
			case COMMA:
			case CLOSING_BRACE:
			case EXLAMATION:
			case MINUS:
			case PORT:
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
			break;
		}
		case LESS_THAN:
		{
			match(LESS_THAN);
			tn = LT(1);
			match(WORD);
			match(GREATER_THAN);
#line 842 "pf.g"
			
			as.at = AddressSpec::TABLE;
			as.address = tn->getText();
			
#line 3347 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 847 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 3360 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_33);
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
				goto _loop124;
			}
			
		}
		_loop124:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_31);
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
#line 869 "pf.g"
		
		importer->route_type = PFImporter::ROUTE_TO;
		
#line 3422 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_24);
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
#line 876 "pf.g"
		
		importer->route_type = PFImporter::REPLY_TO;
		
#line 3457 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_24);
	}
}

void PFCfgParser::routehost() {
	Tracer traceInOut(this, "routehost");
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 881 "pf.g"
	RouteSpec rs;
#line 3473 "PFCfgParser.cpp"
	
	try {      // for error handling
		match(OPENING_PAREN);
		match(WORD);
#line 883 "pf.g"
		rs.iface = LT(0)->getText();
#line 3480 "PFCfgParser.cpp"
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
#line 885 "pf.g"
		
		if (v6)
		{
		importer->addMessageToLog(QString("IPv6 import is not supported. "));
		consumeUntil(NEWLINE);
		} else
		{
		if (h) rs.address = h->getText();
		if (nm) rs.netmask = nm->getText();
		importer->route_group.push_back(rs);
		}
		
#line 3551 "PFCfgParser.cpp"
		match(CLOSING_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_34);
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
				goto _loop137;
			}
			
		}
		_loop137:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_24);
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
		case EXLAMATION:
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
		recover(ex,_tokenSet_35);
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
#line 941 "pf.g"
			
			importer->flags_check = "none";
			importer->flags_mask = "none";
			
#line 3681 "PFCfgParser.cpp"
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
			case EXLAMATION:
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
#line 947 "pf.g"
			
			if (check)
			importer->flags_check = check->getText();
			else
			importer->flags_check = "any";
			if (mask)
			importer->flags_mask = mask->getText();
			else
			importer->flags_mask = "all";
			
#line 3748 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_35);
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
		recover(ex,_tokenSet_35);
	}
}

void PFCfgParser::icmp6_type() {
	Tracer traceInOut(this, "icmp6_type");
	
	try {      // for error handling
		match(ICMP6_TYPE);
#line 1001 "pf.g"
		
		importer->addMessageToLog(QString("ICMP6 import is not supported. "));
		consumeUntil(NEWLINE);
		
#line 3805 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_35);
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
#line 1032 "pf.g"
		
		importer->state_op = LT(0)->getText();
		
#line 3849 "PFCfgParser.cpp"
		match(STATE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_35);
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
#line 1041 "pf.g"
			importer->queue += LT(0)->getText();
#line 3870 "PFCfgParser.cpp"
			break;
		}
		case OPENING_PAREN:
		{
			match(OPENING_PAREN);
			match(WORD);
#line 1044 "pf.g"
			importer->queue += LT(0)->getText();
#line 3879 "PFCfgParser.cpp"
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
#line 1046 "pf.g"
					importer->queue += ",";
#line 3886 "PFCfgParser.cpp"
					match(WORD);
#line 1047 "pf.g"
					importer->queue += LT(0)->getText();
#line 3890 "PFCfgParser.cpp"
				}
				else {
					goto _loop166;
				}
				
			}
			_loop166:;
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
		recover(ex,_tokenSet_35);
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
		recover(ex,_tokenSet_35);
	}
}

void PFCfgParser::icmp_type_code() {
	Tracer traceInOut(this, "icmp_type_code");
#line 969 "pf.g"
	IcmpSpec is;
#line 3932 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
#line 971 "pf.g"
			is.icmp_type_name = LT(0)->getText();
#line 3942 "PFCfgParser.cpp"
			break;
		}
		case INT_CONST:
		{
			match(INT_CONST);
#line 973 "pf.g"
			is.icmp_type_int = LT(0)->getText();
#line 3950 "PFCfgParser.cpp"
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
		case ICMP_CODE:
		{
			match(ICMP_CODE);
			{
			switch ( LA(1)) {
			case WORD:
			{
				match(WORD);
#line 978 "pf.g"
				is.icmp_code_name = LT(0)->getText();
#line 3971 "PFCfgParser.cpp"
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
#line 980 "pf.g"
				is.icmp_code_int = LT(0)->getText();
#line 3979 "PFCfgParser.cpp"
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
		case WORD:
		case QUEUE:
		case COMMA:
		case CLOSING_BRACE:
		case EXLAMATION:
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
#line 983 "pf.g"
		
		importer->icmp_type_code_group.push_back(is);
		
#line 4020 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_36);
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
				goto _loop156;
			}
			
		}
		_loop156:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_35);
	}
}

void PFCfgParser::port_op() {
	Tracer traceInOut(this, "port_op");
#line 1101 "pf.g"
	PortSpec ps;
#line 4076 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EQUAL:
		case LESS_THAN:
		case GREATER_THAN:
		case EXLAMATION:
		{
			unary_port_op();
#line 1103 "pf.g"
			ps.port_op = importer->tmp_port_op;
#line 4089 "PFCfgParser.cpp"
			port_def();
#line 1105 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = importer->tmp_port_def;
			
#line 4096 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case INT_CONST:
		{
			port_def();
#line 1111 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = ps.port1;
			ps.port_op = "=";
			
#line 4109 "PFCfgParser.cpp"
			{
			if ((LA(1) == LESS_THAN || LA(1) == GREATER_THAN || LA(1) == COLON) && (_tokenSet_37.member(LA(2)))) {
				binary_port_op();
#line 1117 "pf.g"
				ps.port_op = importer->tmp_port_op;
#line 4115 "PFCfgParser.cpp"
				port_def();
#line 1118 "pf.g"
				ps.port2 = LT(0)->getText();
#line 4119 "PFCfgParser.cpp"
			}
			else if ((_tokenSet_38.member(LA(1))) && (_tokenSet_39.member(LA(2)))) {
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
#line 1121 "pf.g"
		
		importer->tmp_port_group.push_back(ps);
		
#line 4140 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_38);
	}
}

void PFCfgParser::port_op_list() {
	Tracer traceInOut(this, "port_op_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		port_op();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_40.member(LA(1)))) {
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
				goto _loop184;
			}
			
		}
		_loop184:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_30);
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
#line 1077 "pf.g"
			importer->tmp_port_op = "=";
#line 4207 "PFCfgParser.cpp"
			break;
		}
		case EXLAMATION:
		{
			match(EXLAMATION);
			match(EQUAL);
#line 1079 "pf.g"
			importer->tmp_port_op = "!=";
#line 4216 "PFCfgParser.cpp"
			break;
		}
		default:
			if ((LA(1) == LESS_THAN) && (LA(2) == WORD || LA(2) == INT_CONST)) {
				match(LESS_THAN);
#line 1081 "pf.g"
				importer->tmp_port_op = "<";
#line 4224 "PFCfgParser.cpp"
			}
			else if ((LA(1) == LESS_THAN) && (LA(2) == EQUAL)) {
				match(LESS_THAN);
				match(EQUAL);
#line 1083 "pf.g"
				importer->tmp_port_op = "<=";
#line 4231 "PFCfgParser.cpp"
			}
			else if ((LA(1) == GREATER_THAN) && (LA(2) == WORD || LA(2) == INT_CONST)) {
				match(GREATER_THAN);
#line 1085 "pf.g"
				importer->tmp_port_op = ">";
#line 4237 "PFCfgParser.cpp"
			}
			else if ((LA(1) == GREATER_THAN) && (LA(2) == EQUAL)) {
				match(GREATER_THAN);
				match(EQUAL);
#line 1087 "pf.g"
				importer->tmp_port_op = ">=";
#line 4244 "PFCfgParser.cpp"
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_41);
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
#line 1093 "pf.g"
			importer->tmp_port_op = "<>";
#line 4270 "PFCfgParser.cpp"
			break;
		}
		case GREATER_THAN:
		{
			match(GREATER_THAN);
			match(LESS_THAN);
#line 1095 "pf.g"
			importer->tmp_port_op = "><";
#line 4279 "PFCfgParser.cpp"
			break;
		}
		case COLON:
		{
			match(COLON);
#line 1097 "pf.g"
			importer->tmp_port_op = ":";
#line 4287 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_41);
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
	"COLON",
	"\"network\"",
	"\"broadcast\"",
	"\"peer\"",
	"INT_CONST",
	"\"self\"",
	"IPV4",
	"SLASH",
	"\"nat\"",
	"\"pass\"",
	"MINUS",
	"\"static-port\"",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"\"port\"",
	"STAR",
	"\"bitmask\"",
	"\"random\"",
	"\"source-hash\"",
	"\"hex-key\"",
	"\"string-key\"",
	"\"round-robin\"",
	"\"sticky-address\"",
	"\"binat\"",
	"\"rdr\"",
	"\"timeout\"",
	"\"block\"",
	"\"drop\"",
	"\"return\"",
	"\"return-rst\"",
	"TTL",
	"\"return-icmp\"",
	"RETURN_ICMP6",
	"\"in\"",
	"\"out\"",
	"\"log\"",
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
	"PLUS",
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

const unsigned long PFCfgParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_0(_tokenSet_0_data_,6);
const unsigned long PFCfgParser::_tokenSet_1_data_[] = { 16242UL, 983046UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "scrub" 
// "table" "nat" "pass" "binat" "rdr" "timeout" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_1(_tokenSet_1_data_,6);
const unsigned long PFCfgParser::_tokenSet_2_data_[] = { 3242197056UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA EXLAMATION "self" IPV4 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_2(_tokenSet_2_data_,6);
const unsigned long PFCfgParser::_tokenSet_3_data_[] = { 3250585664UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA CLOSING_BRACE EXLAMATION "self" IPV4 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_3(_tokenSet_3_data_,6);
const unsigned long PFCfgParser::_tokenSet_4_data_[] = { 16778256UL, 2684354568UL, 3156213791UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION MINUS "all" "to" "quick" "on" "inet" "inet6" 
// "proto" "from" "route-to" "reply-to" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long PFCfgParser::_tokenSet_5_data_[] = { 16778256UL, 2684354568UL, 3156213788UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION MINUS "all" "to" "inet" "inet6" "proto" "from" 
// "route-to" "reply-to" "flags" "icmp-type" "icmp6-type" "tagged" "tag" 
// "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_5(_tokenSet_5_data_,8);
const unsigned long PFCfgParser::_tokenSet_6_data_[] = { 16778256UL, 2684354568UL, 2954887184UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION MINUS "all" "to" "proto" "from" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_6(_tokenSet_6_data_,8);
const unsigned long PFCfgParser::_tokenSet_7_data_[] = { 16778256UL, 2684354568UL, 2954887168UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION MINUS "all" "to" "from" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_7(_tokenSet_7_data_,8);
const unsigned long PFCfgParser::_tokenSet_8_data_[] = { 16778256UL, 8UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION MINUS "flags" "icmp-type" "icmp6-type" "tagged" 
// "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_8(_tokenSet_8_data_,8);
const unsigned long PFCfgParser::_tokenSet_9_data_[] = { 20972560UL, 8UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA EXLAMATION MINUS "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_9(_tokenSet_9_data_,8);
const unsigned long PFCfgParser::_tokenSet_10_data_[] = { 2160066640UL, 20144UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD COMMA CLOSING_BRACE IPV4 "static-port" OPENING_PAREN "port" 
// "bitmask" "random" "source-hash" "round-robin" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_10(_tokenSet_10_data_,6);
const unsigned long PFCfgParser::_tokenSet_11_data_[] = { 2151678016UL, 32UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA IPV4 OPENING_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_11(_tokenSet_11_data_,6);
const unsigned long PFCfgParser::_tokenSet_12_data_[] = { 16UL, 20112UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "static-port" "port" "bitmask" "random" "source-hash" "round-robin" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_12(_tokenSet_12_data_,6);
const unsigned long PFCfgParser::_tokenSet_13_data_[] = { 16UL, 19984UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "static-port" "bitmask" "random" "source-hash" "round-robin" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_13(_tokenSet_13_data_,6);
const unsigned long PFCfgParser::_tokenSet_14_data_[] = { 16UL, 16UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "static-port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_14(_tokenSet_14_data_,6);
const unsigned long PFCfgParser::_tokenSet_15_data_[] = { 599835856UL, 2147503640UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD EQUAL "queue" LESS_THAN GREATER_THAN COMMA CLOSING_BRACE 
// EXLAMATION COLON INT_CONST MINUS "static-port" "bitmask" "random" "source-hash" 
// "round-robin" "to" "flags" "icmp-type" "icmp6-type" "tagged" "tag" "no" 
// "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_15(_tokenSet_15_data_,8);
const unsigned long PFCfgParser::_tokenSet_16_data_[] = { 16778256UL, 2684354560UL, 2954887168UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION "all" "to" "from" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_16(_tokenSet_16_data_,8);
const unsigned long PFCfgParser::_tokenSet_17_data_[] = { 3782246258UL, 983079UL, 3015704576UL, 255UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "scrub" 
// "table" LESS_THAN STRING OPENING_BRACE COMMA EXLAMATION INT_CONST "self" 
// IPV4 SLASH "nat" "pass" OPENING_PAREN "binat" "rdr" "timeout" "block" 
// "urpf-failed" "any" "no-route" IPV6 "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "state" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_17(_tokenSet_17_data_,8);
const unsigned long PFCfgParser::_tokenSet_18_data_[] = { 16778256UL, 0UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION "flags" "icmp-type" "icmp6-type" "tagged" 
// "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_18(_tokenSet_18_data_,8);
const unsigned long PFCfgParser::_tokenSet_19_data_[] = { 561004402UL, 983079UL, 2961178624UL, 255UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "scrub" 
// "table" STRING OPENING_BRACE COMMA EXLAMATION INT_CONST SLASH "nat" 
// "pass" OPENING_PAREN "binat" "rdr" "timeout" "block" "any" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "state" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_19(_tokenSet_19_data_,8);
const unsigned long PFCfgParser::_tokenSet_20_data_[] = { 16UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_20(_tokenSet_20_data_,6);
const unsigned long PFCfgParser::_tokenSet_21_data_[] = { 16778256UL, 3154116608UL, 3156213791UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION "in" "out" "log" "all" "to" "quick" "on" 
// "inet" "inet6" "proto" "from" "route-to" "reply-to" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_21(_tokenSet_21_data_,8);
const unsigned long PFCfgParser::_tokenSet_22_data_[] = { 16778256UL, 2952790016UL, 3156213791UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION "log" "all" "to" "quick" "on" "inet" "inet6" 
// "proto" "from" "route-to" "reply-to" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_22(_tokenSet_22_data_,8);
const unsigned long PFCfgParser::_tokenSet_23_data_[] = { 16778256UL, 2684354560UL, 3156213790UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION "all" "to" "on" "inet" "inet6" "proto" "from" 
// "route-to" "reply-to" "flags" "icmp-type" "icmp6-type" "tagged" "tag" 
// "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_23(_tokenSet_23_data_,8);
const unsigned long PFCfgParser::_tokenSet_24_data_[] = { 16778256UL, 2684354560UL, 2954887196UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION "all" "to" "inet" "inet6" "proto" "from" 
// "flags" "icmp-type" "icmp6-type" "tagged" "tag" "no" "keep" "modulate" 
// "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_24(_tokenSet_24_data_,8);
const unsigned long PFCfgParser::_tokenSet_25_data_[] = { 20972544UL, 0UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// "queue" COMMA EXLAMATION "flags" "icmp-type" "icmp6-type" "tagged" "tag" 
// "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_25(_tokenSet_25_data_,8);
const unsigned long PFCfgParser::_tokenSet_26_data_[] = { 4194304UL, 64UL, 0UL, 0UL, 0UL, 0UL };
// COMMA CLOSING_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_26(_tokenSet_26_data_,6);
const unsigned long PFCfgParser::_tokenSet_27_data_[] = { 29361232UL, 2684354568UL, 3156213788UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD "queue" COMMA CLOSING_BRACE EXLAMATION MINUS "all" "to" 
// "inet" "inet6" "proto" "from" "route-to" "reply-to" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_27(_tokenSet_27_data_,8);
const unsigned long PFCfgParser::_tokenSet_28_data_[] = { 568329232UL, 2684354568UL, 2956984288UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" OPENING_BRACE COMMA CLOSING_BRACE EXLAMATION INT_CONST 
// MINUS "all" "to" "ip" "icmp" "igmp" "tcp" "udp" "rdp" "rsvp" "gre" "esp" 
// "ah" "eigrp" "ospf" "ipip" "vrrp" "l2tp" "isis" "from" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_28(_tokenSet_28_data_,8);
const unsigned long PFCfgParser::_tokenSet_29_data_[] = { 543162368UL, 0UL, 2097120UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// OPENING_BRACE COMMA INT_CONST "ip" "icmp" "igmp" "tcp" "udp" "rdp" "rsvp" 
// "gre" "esp" "ah" "eigrp" "ospf" "ipip" "vrrp" "l2tp" "isis" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_29(_tokenSet_29_data_,8);
const unsigned long PFCfgParser::_tokenSet_30_data_[] = { 16778256UL, 2147483656UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION MINUS "to" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_30(_tokenSet_30_data_,8);
const unsigned long PFCfgParser::_tokenSet_31_data_[] = { 16778256UL, 2147483784UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION MINUS "port" "to" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_31(_tokenSet_31_data_,8);
const unsigned long PFCfgParser::_tokenSet_32_data_[] = { 16778256UL, 136UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" EXLAMATION MINUS "port" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_32(_tokenSet_32_data_,8);
const unsigned long PFCfgParser::_tokenSet_33_data_[] = { 29361168UL, 2147483784UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA CLOSING_BRACE EXLAMATION MINUS "port" "to" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_33(_tokenSet_33_data_,8);
const unsigned long PFCfgParser::_tokenSet_34_data_[] = { 29361168UL, 2684354592UL, 2954887196UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA CLOSING_BRACE EXLAMATION OPENING_PAREN "all" "to" 
// "inet" "inet6" "proto" "from" "flags" "icmp-type" "icmp6-type" "tagged" 
// "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_34(_tokenSet_34_data_,8);
const unsigned long PFCfgParser::_tokenSet_35_data_[] = { 20972560UL, 0UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA EXLAMATION "flags" "icmp-type" "icmp6-type" "tagged" 
// "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_35(_tokenSet_35_data_,8);
const unsigned long PFCfgParser::_tokenSet_36_data_[] = { 566232144UL, 0UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD "queue" COMMA CLOSING_BRACE EXLAMATION INT_CONST "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_36(_tokenSet_36_data_,8);
const unsigned long PFCfgParser::_tokenSet_37_data_[] = { 536920128UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD LESS_THAN GREATER_THAN INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_37(_tokenSet_37_data_,6);
const unsigned long PFCfgParser::_tokenSet_38_data_[] = { 566281424UL, 2147483656UL, 2952790016UL, 191UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD EQUAL "queue" LESS_THAN GREATER_THAN COMMA CLOSING_BRACE 
// EXLAMATION INT_CONST MINUS "to" "flags" "icmp-type" "icmp6-type" "tagged" 
// "tag" "no" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_38(_tokenSet_38_data_,8);
const unsigned long PFCfgParser::_tokenSet_39_data_[] = { 3824222194UL, 2148466735UL, 3011510272UL, 255UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD EQUAL "antispoof" "altq" "queue" "set" 
// "scrub" "table" LESS_THAN GREATER_THAN STRING OPENING_BRACE COMMA CLOSING_BRACE 
// EXLAMATION COLON INT_CONST "self" IPV4 SLASH "nat" "pass" MINUS OPENING_PAREN 
// "binat" "rdr" "timeout" "block" "to" "any" "no-route" IPV6 "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "no" "keep" "modulate" "synproxy" "state" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_39(_tokenSet_39_data_,8);
const unsigned long PFCfgParser::_tokenSet_40_data_[] = { 557891776UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD EQUAL LESS_THAN GREATER_THAN COMMA EXLAMATION INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_40(_tokenSet_40_data_,6);
const unsigned long PFCfgParser::_tokenSet_41_data_[] = { 536870976UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_41(_tokenSet_41_data_,6);



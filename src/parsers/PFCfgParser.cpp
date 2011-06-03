/* $ANTLR 2.7.7 (20100319): "pf.g" -> "PFCfgParser.cpp"$ */
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
			case MATCH:
			{
				match_rule();
				break;
			}
			case TABLE:
			{
				table_rule();
				break;
			}
			case NO:
			{
				no_nat_rule();
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
#line 156 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		consumeUntil(NEWLINE);
		
#line 174 "PFCfgParser.cpp"
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
#line 176 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->error_tracker->registerError(
		QString("import of 'altq' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 195 "PFCfgParser.cpp"
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
#line 165 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Warning: import of 'antispoof' commands has not been implemented yet."));
		consumeUntil(NEWLINE);
		
#line 216 "PFCfgParser.cpp"
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
#line 187 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->error_tracker->registerError(
		QString("import of 'queue' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 237 "PFCfgParser.cpp"
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
#line 198 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		
#line 255 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case TIMEOUT:
		{
			set_timeout();
			break;
		}
		case 13:
		{
			set_ruleset_optimization();
			break;
		}
		case LITERAL_optimization:
		{
			set_optimization();
			break;
		}
		case LITERAL_limit:
		{
			set_limit();
			break;
		}
		case LITERAL_loginterface:
		{
			set_loginterface();
			break;
		}
		case 22:
		{
			set_block_policy();
			break;
		}
		case 25:
		{
			set_state_policy();
			break;
		}
		case 28:
		{
			set_state_defaults();
			break;
		}
		case 29:
		{
			set_require_order();
			break;
		}
		case LITERAL_fingerprints:
		{
			set_fingerprints();
			break;
		}
		case LITERAL_skip:
		{
			set_skip();
			break;
		}
		case LITERAL_debug:
		{
			set_debug();
			break;
		}
		case LITERAL_reassemble:
		{
			set_reassemble();
			break;
		}
		case LITERAL_hostid:
		{
			set_hostid();
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

void PFCfgParser::scrub_rule() {
	Tracer traceInOut(this, "scrub_rule");
	
	try {      // for error handling
		match(SCRUB);
#line 481 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "scrub";
		*dbg << LT(1)->getLine() << ":" << " scrub ";
		
#line 354 "PFCfgParser.cpp"
		rule_extended();
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::match_rule() {
	Tracer traceInOut(this, "match_rule");
	
	try {      // for error handling
		match(MATCH);
#line 496 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "match";
		*dbg << LT(1)->getLine() << ":" << " match ";
		
#line 377 "PFCfgParser.cpp"
		rule_extended();
#line 504 "pf.g"
		
		if ( ! importer->scrub_rule) importer->pushRule();
		
#line 383 "PFCfgParser.cpp"
		match(NEWLINE);
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
#line 513 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		
#line 404 "PFCfgParser.cpp"
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
		case OPENING_BRACE:
		case CONST:
		case COUNTERS:
		case FILE:
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
		case OPENING_BRACE:
		case COUNTERS:
		case FILE:
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
		case OPENING_BRACE:
		case FILE:
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
#line 525 "pf.g"
			
			importer->newAddressTableObject(
			name->getText(), file->getText());
			
#line 478 "PFCfgParser.cpp"
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
					goto _loop51;
				}
				
			}
			_loop51:;
			} // ( ... )*
			match(CLOSING_BRACE);
#line 537 "pf.g"
			
			importer->newAddressTableObject(
			name->getText(), importer->tmp_group);
			
#line 523 "PFCfgParser.cpp"
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

void PFCfgParser::no_nat_rule() {
	Tracer traceInOut(this, "no_nat_rule");
	
	try {      // for error handling
		match(NO);
#line 610 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newNATRule();
		importer->action = "nonat";
		*dbg << LT(1)->getLine() << ":" << " nonat ";
		
#line 552 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
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
#line 627 "pf.g"
		
		if ( importer->action != "nonat" )
		{
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newNATRule();
		importer->action = "nat";
		*dbg << LT(1)->getLine() << ":" << " nat ";
		}
		
#line 594 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case PASS:
		{
			match(PASS);
#line 639 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat pass' commands is not supported."));
			
#line 605 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case LOG:
			{
				logging();
				break;
			}
			case NEWLINE:
			case ON:
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
			break;
		}
		case NEWLINE:
		case ON:
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
		case ON:
		{
			intrface();
			break;
		}
		case NEWLINE:
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
		case NEWLINE:
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
		case NEWLINE:
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
		case NEWLINE:
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
#line 652 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat ... tag' commands is not supported."));
			
#line 765 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
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
		{
		switch ( LA(1)) {
		case MINUS:
		{
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
#line 660 "pf.g"
			
			importer->nat_group = importer->tmp_group;
			
#line 809 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case PORT:
			{
				portspec();
#line 665 "pf.g"
				
				importer->nat_port_group = importer->tmp_port_group;
				
#line 819 "PFCfgParser.cpp"
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
#line 671 "pf.g"
				importer->nat_rule_opt_2 = "static-port";
#line 865 "PFCfgParser.cpp"
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
#line 674 "pf.g"
		
		importer->pushRule();
		
#line 894 "PFCfgParser.cpp"
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
#line 683 "pf.g"
		
		if ( importer->action != "nonat" )
		{
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newNATRule();
		importer->action = "rdr";
		*dbg << LT(1)->getLine() << ":" << " rdr ";
		}
		
#line 919 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case PASS:
		{
			match(PASS);
#line 695 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat pass' commands is not supported."));
			
#line 930 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case LOG:
			{
				logging();
				break;
			}
			case NEWLINE:
			case ON:
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
			break;
		}
		case NEWLINE:
		case ON:
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
		case ON:
		{
			intrface();
			break;
		}
		case NEWLINE:
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
		case NEWLINE:
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
		case NEWLINE:
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
		case NEWLINE:
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
#line 708 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat ... tag' commands is not supported."));
			
#line 1090 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
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
		{
		switch ( LA(1)) {
		case MINUS:
		{
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
#line 716 "pf.g"
			
			importer->nat_group = importer->tmp_group;
			
#line 1134 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case PORT:
			{
				portspec();
#line 721 "pf.g"
				
				importer->nat_port_group = importer->tmp_port_group;
				
#line 1144 "PFCfgParser.cpp"
				break;
			}
			case NEWLINE:
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
#line 727 "pf.g"
		
		importer->pushRule();
		
#line 1197 "PFCfgParser.cpp"
		match(NEWLINE);
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
#line 859 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->error_tracker->registerError(
		QString("import of 'binat' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1219 "PFCfgParser.cpp"
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
#line 882 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "pass";
		*dbg << LT(1)->getLine() << ":" << " pass ";
		
#line 1240 "PFCfgParser.cpp"
		rule_extended();
#line 890 "pf.g"
		
		importer->pushRule();
		
#line 1246 "PFCfgParser.cpp"
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
#line 897 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "block";
		*dbg << LT(1)->getLine() << ":" << " block   ";
		
#line 1268 "PFCfgParser.cpp"
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
		case ON:
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case IN:
		case OUT:
		case LOG:
		case ALL:
		case TO:
		case QUICK:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
#line 906 "pf.g"
		
		importer->pushRule();
		
#line 1328 "PFCfgParser.cpp"
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_timeout() {
	Tracer traceInOut(this, "set_timeout");
	
	try {      // for error handling
		match(TIMEOUT);
		{
		switch ( LA(1)) {
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
		case 49:
		case LITERAL_frag:
		case LITERAL_interval:
		case 52:
		case 53:
		case 54:
		{
			timeout_def();
			break;
		}
		case OPENING_BRACE:
		{
			timeout_def_list();
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

void PFCfgParser::set_ruleset_optimization() {
	Tracer traceInOut(this, "set_ruleset_optimization");
	
	try {      // for error handling
		match(13);
#line 241 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'set ruleset-optimization' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1398 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_optimization() {
	Tracer traceInOut(this, "set_optimization");
	
	try {      // for error handling
		match(LITERAL_optimization);
		{
		switch ( LA(1)) {
		case LITERAL_aggressive:
		{
			match(LITERAL_aggressive);
			break;
		}
		case LITERAL_conservative:
		{
			match(LITERAL_conservative);
			break;
		}
		case 17:
		{
			match(17);
			break;
		}
		case LITERAL_normal:
		{
			match(LITERAL_normal);
			break;
		}
		case LITERAL_satellite:
		{
			match(LITERAL_satellite);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 263 "pf.g"
		importer->set_optimization = LT(0)->getText();
#line 1446 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_limit() {
	Tracer traceInOut(this, "set_limit");
	
	try {      // for error handling
		match(LITERAL_limit);
		{
		switch ( LA(1)) {
		case LITERAL_frags:
		case LITERAL_states:
		case 61:
		case LITERAL_tables:
		case 63:
		{
			limit_def();
			break;
		}
		case OPENING_BRACE:
		{
			limit_def_list();
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

void PFCfgParser::set_loginterface() {
	Tracer traceInOut(this, "set_loginterface");
	
	try {      // for error handling
		match(LITERAL_loginterface);
#line 274 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'set loginterface' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1501 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_block_policy() {
	Tracer traceInOut(this, "set_block_policy");
	
	try {      // for error handling
		match(22);
		{
		switch ( LA(1)) {
		case DROP:
		{
			match(DROP);
			break;
		}
		case RETURN:
		{
			match(RETURN);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 286 "pf.g"
		importer->set_block_policy = LT(0)->getText();
#line 1534 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_state_policy() {
	Tracer traceInOut(this, "set_state_policy");
	
	try {      // for error handling
		match(25);
		{
		switch ( LA(1)) {
		case 26:
		{
			match(26);
			break;
		}
		case LITERAL_floating:
		{
			match(LITERAL_floating);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 292 "pf.g"
		importer->set_state_policy = LT(0)->getText();
#line 1567 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_state_defaults() {
	Tracer traceInOut(this, "set_state_defaults");
	
	try {      // for error handling
		match(28);
#line 298 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'set state-defaults' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1588 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_require_order() {
	Tracer traceInOut(this, "set_require_order");
	
	try {      // for error handling
		match(29);
#line 310 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'set require-order' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1609 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_fingerprints() {
	Tracer traceInOut(this, "set_fingerprints");
	
	try {      // for error handling
		match(LITERAL_fingerprints);
#line 322 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'set fingerprints' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1630 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_skip() {
	Tracer traceInOut(this, "set_skip");
	
	try {      // for error handling
		match(LITERAL_skip);
		match(ON);
		match(WORD);
#line 334 "pf.g"
		
		importer->set_skip_on = LT(0)->getText();
		
#line 1649 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_debug() {
	Tracer traceInOut(this, "set_debug");
	
	try {      // for error handling
		match(LITERAL_debug);
		match(WORD);
#line 342 "pf.g"
		
		importer->set_debug = LT(0)->getText();
		
#line 1667 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_reassemble() {
	Tracer traceInOut(this, "set_reassemble");
	
	try {      // for error handling
		match(LITERAL_reassemble);
#line 350 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'set reassemble' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1688 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::set_hostid() {
	Tracer traceInOut(this, "set_hostid");
	
	try {      // for error handling
		match(LITERAL_hostid);
#line 362 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'set hostid' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1709 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::timeout_def() {
	Tracer traceInOut(this, "timeout_def");
#line 380 "pf.g"
	std::string timeout_name, timeout_value;
#line 1721 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case 36:
		{
			match(36);
			break;
		}
		case 37:
		{
			match(37);
			break;
		}
		case 38:
		{
			match(38);
			break;
		}
		case 39:
		{
			match(39);
			break;
		}
		case 40:
		{
			match(40);
			break;
		}
		case 41:
		{
			match(41);
			break;
		}
		case 42:
		{
			match(42);
			break;
		}
		case 43:
		{
			match(43);
			break;
		}
		case 44:
		{
			match(44);
			break;
		}
		case 45:
		{
			match(45);
			break;
		}
		case 46:
		{
			match(46);
			break;
		}
		case 47:
		{
			match(47);
			break;
		}
		case 48:
		{
			match(48);
			break;
		}
		case 49:
		{
			match(49);
			break;
		}
		case LITERAL_frag:
		{
			match(LITERAL_frag);
			break;
		}
		case LITERAL_interval:
		{
			match(LITERAL_interval);
			break;
		}
		case 52:
		{
			match(52);
			break;
		}
		case 53:
		{
			match(53);
			break;
		}
		case 54:
		{
			match(54);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 421 "pf.g"
		
		timeout_name = LT(0)->getText();
		
#line 1831 "PFCfgParser.cpp"
		match(INT_CONST);
#line 425 "pf.g"
		
		timeout_value = LT(0)->getText();
		importer->timeouts.push_back(
		std::pair<std::string, std::string>(timeout_name, timeout_value));
		
#line 1839 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
}

void PFCfgParser::timeout_def_list() {
	Tracer traceInOut(this, "timeout_def_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		timeout_def();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case 36:
				case 37:
				case 38:
				case 39:
				case 40:
				case 41:
				case 42:
				case 43:
				case 44:
				case 45:
				case 46:
				case 47:
				case 48:
				case 49:
				case LITERAL_frag:
				case LITERAL_interval:
				case 52:
				case 53:
				case 54:
				{
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				timeout_def();
			}
			else {
				goto _loop35;
			}
			
		}
		_loop35:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::limit_def() {
	Tracer traceInOut(this, "limit_def");
#line 443 "pf.g"
	std::string limit_name, limit_value;
#line 1912 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case LITERAL_frags:
		{
			match(LITERAL_frags);
			break;
		}
		case LITERAL_states:
		{
			match(LITERAL_states);
			break;
		}
		case 61:
		{
			match(61);
			break;
		}
		case LITERAL_tables:
		{
			match(LITERAL_tables);
			break;
		}
		case 63:
		{
			match(63);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 456 "pf.g"
		
		limit_name = LT(0)->getText();
		
#line 1952 "PFCfgParser.cpp"
		match(INT_CONST);
#line 460 "pf.g"
		
		limit_value = LT(0)->getText();
		importer->limits.push_back(
		std::pair<std::string, std::string>(limit_name, limit_value));
		
#line 1960 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_5);
	}
}

void PFCfgParser::limit_def_list() {
	Tracer traceInOut(this, "limit_def_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		limit_def();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_6.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case LITERAL_frags:
				case LITERAL_states:
				case 61:
				case LITERAL_tables:
				case 63:
				{
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				limit_def();
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
		recover(ex,_tokenSet_1);
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
		case ON:
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case LOG:
		case ALL:
		case TO:
		case QUICK:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		case ON:
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case ALL:
		case TO:
		case QUICK:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		case ON:
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case ALL:
		case TO:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case ALL:
		case TO:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case ROUTE_TO:
		case REPLY_TO:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case ALL:
		case TO:
		case INET:
		case INET6:
		case PROTO:
		case FROM:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case ALL:
		case TO:
		case PROTO:
		case FROM:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case ALL:
		case TO:
		case FROM:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		if ((_tokenSet_7.member(LA(1))) && (_tokenSet_8.member(LA(2)))) {
			hosts();
		}
		else if ((_tokenSet_9.member(LA(1))) && (_tokenSet_10.member(LA(2)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		{
		switch ( LA(1)) {
		case QUEUE:
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		recover(ex,_tokenSet_11);
	}
}

void PFCfgParser::tableaddr_spec() {
	Tracer traceInOut(this, "tableaddr_spec");
#line 544 "pf.g"
	AddressSpec as;
#line 2390 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 545 "pf.g"
			as.neg = true;
#line 2400 "PFCfgParser.cpp"
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
#line 548 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = LT(0)->getText();
			
#line 2426 "PFCfgParser.cpp"
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
#line 557 "pf.g"
					
					as.at = AddressSpec::INTERFACE_NETWORK;
					
#line 2441 "PFCfgParser.cpp"
					break;
				}
				case BROADCAST:
				{
					match(BROADCAST);
#line 562 "pf.g"
					
					as.at = AddressSpec::INTERFACE_BROADCAST;
					
#line 2451 "PFCfgParser.cpp"
					break;
				}
				case PEER:
				{
					match(PEER);
#line 567 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:peer' is not supported."));
					
#line 2462 "PFCfgParser.cpp"
					break;
				}
				case INT_CONST:
				{
					match(INT_CONST);
#line 573 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:0' is not supported."));
					
#line 2473 "PFCfgParser.cpp"
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
#line 581 "pf.g"
			
			as.at = AddressSpec::SPECIAL_ADDRESS;
			as.address = "self";
			
#line 2509 "PFCfgParser.cpp"
			break;
		}
		case IPV4:
		{
			match(IPV4);
#line 587 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 2520 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 593 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 2530 "PFCfgParser.cpp"
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
#line 597 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 2553 "PFCfgParser.cpp"
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
#line 602 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 2583 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_12);
	}
}

void PFCfgParser::logging() {
	Tracer traceInOut(this, "logging");
	
	try {      // for error handling
		match(LOG);
		{
		if ((LA(1) == OPENING_PAREN) && ((LA(2) >= ALL && LA(2) <= TO))) {
			logopts();
		}
		else if ((_tokenSet_13.member(LA(1))) && (_tokenSet_14.member(LA(2)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
#line 972 "pf.g"
		
		importer->logging = true;
		
#line 2611 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_13);
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
		recover(ex,_tokenSet_15);
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
#line 1024 "pf.g"
			
			importer->address_family = LT(0)->getText();
			
#line 2667 "PFCfgParser.cpp"
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

void PFCfgParser::protospec() {
	Tracer traceInOut(this, "protospec");
	
	try {      // for error handling
		match(PROTO);
		proto_def();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_17);
	}
}

void PFCfgParser::hosts() {
	Tracer traceInOut(this, "hosts");
	
	try {      // for error handling
		switch ( LA(1)) {
		case ALL:
		{
			match(ALL);
#line 1067 "pf.g"
			
			importer->src_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			importer->dst_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			
#line 2710 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
		case QUEUE:
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case MINUS:
		case OPENING_PAREN:
		case TO:
		case FROM:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
			case LITERAL_reassemble:
			case SCRUB:
			case EXLAMATION:
			case NO:
			case MINUS:
			case OPENING_PAREN:
			case TO:
			case LITERAL_fragment:
			case 141:
			case 142:
			case 143:
			case 144:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
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
			case LITERAL_reassemble:
			case SCRUB:
			case EXLAMATION:
			case NO:
			case MINUS:
			case OPENING_PAREN:
			case LITERAL_fragment:
			case 141:
			case 142:
			case 143:
			case 144:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
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
		recover(ex,_tokenSet_18);
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
#line 1572 "pf.g"
			importer->tagged_neg = true;
#line 2840 "PFCfgParser.cpp"
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
#line 1574 "pf.g"
		
		importer->tagged = LT(0)->getText();
		
#line 2859 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_19);
	}
}

void PFCfgParser::tag_clause() {
	Tracer traceInOut(this, "tag_clause");
	
	try {      // for error handling
		match(TAG);
		match(WORD);
#line 1581 "pf.g"
		
		importer->tag = LT(0)->getText();
		
#line 2877 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_19);
	}
}

void PFCfgParser::redirhost() {
	Tracer traceInOut(this, "redirhost");
#line 738 "pf.g"
	AddressSpec as;
#line 2889 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IPV4:
		{
			match(IPV4);
#line 741 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 2902 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 747 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 2912 "PFCfgParser.cpp"
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
#line 751 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 2935 "PFCfgParser.cpp"
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
#line 758 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = LT(0)->getText();
			
#line 2971 "PFCfgParser.cpp"
			match(CLOSING_PAREN);
			break;
		}
		case WORD:
		{
			match(WORD);
#line 766 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = LT(0)->getText();
			
#line 2984 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 772 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 2997 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_20);
	}
}

void PFCfgParser::redirhost_list() {
	Tracer traceInOut(this, "redirhost_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		redirhost();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_21.member(LA(1)))) {
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
				goto _loop93;
			}
			
		}
		_loop93:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_22);
	}
}

void PFCfgParser::portspec() {
	Tracer traceInOut(this, "portspec");
#line 797 "pf.g"
	PortSpec ps;
#line 3054 "PFCfgParser.cpp"
	
	try {      // for error handling
		match(PORT);
		{
		switch ( LA(1)) {
		case WORD:
		case INT_CONST:
		{
			port_def();
#line 801 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = ps.port1;
			ps.port_op = "=";
			
#line 3070 "PFCfgParser.cpp"
			break;
		}
		case IPV6:
		{
			match(IPV6);
#line 810 "pf.g"
			
			ps.setFromPortRange(LT(0)->getText());
			
#line 3080 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case STAR:
			{
				match(STAR);
#line 814 "pf.g"
				ps.port2 = "65535";
#line 3088 "PFCfgParser.cpp"
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
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 817 "pf.g"
		
		importer->tmp_port_group.push_back(ps);
		
#line 3118 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_23);
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
#line 831 "pf.g"
			importer->nat_rule_opt_1 = "bitmask";
#line 3137 "PFCfgParser.cpp"
			break;
		}
		case RANDOM:
		{
			match(RANDOM);
#line 833 "pf.g"
			importer->nat_rule_opt_1 = "random";
#line 3145 "PFCfgParser.cpp"
			break;
		}
		case SOURCE_HASH:
		{
			match(SOURCE_HASH);
#line 835 "pf.g"
			importer->nat_rule_opt_1 = "source-hash";
#line 3153 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case HEX_KEY:
			{
				match(HEX_KEY);
#line 838 "pf.g"
				
				importer->error_tracker->registerError(
				QString("import of 'nat' commands with 'source-hash hex-key' "
				"option is not supported"));
				
#line 3165 "PFCfgParser.cpp"
				break;
			}
			case STRING_KEY:
			{
				match(STRING_KEY);
#line 845 "pf.g"
				
				importer->error_tracker->registerError(
				QString("import of 'nat' commands with 'source-hash string-key' "
				"option is not supported"));
				
#line 3177 "PFCfgParser.cpp"
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
#line 852 "pf.g"
			importer->nat_rule_opt_1 = "round-robin";
#line 3199 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_24);
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
#line 1720 "pf.g"
		
		importer->tmp_port_def = LT(0)->getText();
		
#line 3259 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_25);
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
#line 914 "pf.g"
			importer->block_action_params.push_back("drop");
#line 3278 "PFCfgParser.cpp"
			break;
		}
		case RETURN:
		{
			match(RETURN);
#line 916 "pf.g"
			importer->block_action_params.push_back("return");
#line 3286 "PFCfgParser.cpp"
			break;
		}
		case RETURN_RST:
		{
			match(RETURN_RST);
#line 918 "pf.g"
			importer->block_action_params.push_back("return-rst");
#line 3294 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case TTL:
			{
				match(TTL);
				match(INT_CONST);
#line 921 "pf.g"
				
				importer->error_tracker->registerError(
				QString("Import of \"block return-rst ttl number\" is not supported. "));
				
#line 3306 "PFCfgParser.cpp"
				break;
			}
			case NEWLINE:
			case QUEUE:
			case ON:
			case LITERAL_reassemble:
			case SCRUB:
			case EXLAMATION:
			case NO:
			case OPENING_PAREN:
			case IN:
			case OUT:
			case LOG:
			case ALL:
			case TO:
			case QUICK:
			case INET:
			case INET6:
			case PROTO:
			case FROM:
			case ROUTE_TO:
			case REPLY_TO:
			case LITERAL_fragment:
			case 141:
			case 142:
			case 143:
			case 144:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
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
#line 927 "pf.g"
			importer->block_action_params.push_back("return-icmp");
#line 3359 "PFCfgParser.cpp"
			{
			if ((LA(1) == OPENING_PAREN) && (_tokenSet_26.member(LA(2)))) {
				match(OPENING_PAREN);
				{
				switch ( LA(1)) {
				case 172:
				case 173:
				case 174:
				case 175:
				case LITERAL_needfrag:
				case LITERAL_srcfail:
				case 178:
				case 179:
				case LITERAL_isolate:
				case 181:
				case 182:
				case 183:
				case 184:
				case 185:
				case 186:
				case 187:
				case 188:
				case 189:
				case 190:
				case 191:
				case 192:
				case 193:
				case LITERAL_transit:
				case LITERAL_reassemb:
				case LITERAL_badhead:
				case LITERAL_optmiss:
				case LITERAL_badlen:
				case 199:
				case 200:
				case 201:
				{
					icmp_code_by_name();
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
#line 931 "pf.g"
				importer->block_action_params.push_back(LT(0)->getText());
#line 3412 "PFCfgParser.cpp"
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					{
					switch ( LA(1)) {
					case 172:
					case 173:
					case 174:
					case 175:
					case LITERAL_needfrag:
					case LITERAL_srcfail:
					case 178:
					case 179:
					case LITERAL_isolate:
					case 181:
					case 182:
					case 183:
					case 184:
					case 185:
					case 186:
					case 187:
					case 188:
					case 189:
					case 190:
					case 191:
					case 192:
					case 193:
					case LITERAL_transit:
					case LITERAL_reassemb:
					case LITERAL_badhead:
					case LITERAL_optmiss:
					case LITERAL_badlen:
					case 199:
					case 200:
					case 201:
					{
						icmp_code_by_name();
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
#line 935 "pf.g"
					
					importer->error_tracker->registerError(
					QString("Import of \"block return-icmp (icmp_code, icmp6_code)\" is not supported"));
					
#line 3470 "PFCfgParser.cpp"
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
			}
			else if ((_tokenSet_27.member(LA(1))) && (_tokenSet_28.member(LA(2)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			break;
		}
		case RETURN_ICMP6:
		{
			match(RETURN_ICMP6);
#line 944 "pf.g"
			
			importer->error_tracker->registerError(
			QString("Import of \"block return-icmp6\" is not supported"));
			importer->block_action_params.push_back("return-icmp");
			
#line 3503 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_27);
	}
}

void PFCfgParser::icmp_code_by_name() {
	Tracer traceInOut(this, "icmp_code_by_name");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case 172:
		{
			match(172);
			break;
		}
		case 173:
		{
			match(173);
			break;
		}
		case 174:
		{
			match(174);
			break;
		}
		case 175:
		{
			match(175);
			break;
		}
		case LITERAL_needfrag:
		{
			match(LITERAL_needfrag);
			break;
		}
		case LITERAL_srcfail:
		{
			match(LITERAL_srcfail);
			break;
		}
		case 178:
		{
			match(178);
			break;
		}
		case 179:
		{
			match(179);
			break;
		}
		case LITERAL_isolate:
		{
			match(LITERAL_isolate);
			break;
		}
		case 181:
		{
			match(181);
			break;
		}
		case 182:
		{
			match(182);
			break;
		}
		case 183:
		{
			match(183);
			break;
		}
		case 184:
		{
			match(184);
			break;
		}
		case 185:
		{
			match(185);
			break;
		}
		case 186:
		{
			match(186);
			break;
		}
		case 187:
		{
			match(187);
			break;
		}
		case 188:
		{
			match(188);
			break;
		}
		case 189:
		{
			match(189);
			break;
		}
		case 190:
		{
			match(190);
			break;
		}
		case 191:
		{
			match(191);
			break;
		}
		case 192:
		{
			match(192);
			break;
		}
		case 193:
		{
			match(193);
			break;
		}
		case LITERAL_transit:
		{
			match(LITERAL_transit);
			break;
		}
		case LITERAL_reassemb:
		{
			match(LITERAL_reassemb);
			break;
		}
		case LITERAL_badhead:
		{
			match(LITERAL_badhead);
			break;
		}
		case LITERAL_optmiss:
		{
			match(LITERAL_optmiss);
			break;
		}
		case LITERAL_badlen:
		{
			match(LITERAL_badlen);
			break;
		}
		case 199:
		{
			match(199);
			break;
		}
		case 200:
		{
			match(200);
			break;
		}
		case 201:
		{
			match(201);
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
		recover(ex,_tokenSet_29);
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
#line 965 "pf.g"
		
		importer->direction = LT(0)->getText();
		
#line 3714 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_30);
	}
}

void PFCfgParser::quick() {
	Tracer traceInOut(this, "quick");
	
	try {      // for error handling
		match(QUICK);
#line 994 "pf.g"
		
		importer->quick = true;
		
#line 3731 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_31);
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
		recover(ex,_tokenSet_32);
	}
}

void PFCfgParser::filteropts() {
	Tracer traceInOut(this, "filteropts");
	
	try {      // for error handling
		filteropt();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_33.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case QUEUE:
				case LITERAL_reassemble:
				case SCRUB:
				case EXLAMATION:
				case NO:
				case OPENING_PAREN:
				case LITERAL_fragment:
				case 141:
				case 142:
				case 143:
				case 144:
				case FLAGS:
				case ICMP_TYPE:
				case ICMP6_TYPE:
				case TAGGED:
				case TAG:
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
				goto _loop187;
			}
			
		}
		_loop187:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_11);
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
#line 981 "pf.g"
				importer->logopts += ",";
#line 3838 "PFCfgParser.cpp"
				logopt();
			}
			else {
				goto _loop128;
			}
			
		}
		_loop128:;
		} // ( ... )*
		match(CLOSING_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_13);
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
#line 988 "pf.g"
			
			importer->logopts += LT(0)->getText();
			
#line 3879 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_34);
	}
}

void PFCfgParser::ifspec() {
	Tracer traceInOut(this, "ifspec");
#line 1002 "pf.g"
	InterfaceSpec is;
#line 3898 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 1003 "pf.g"
			is.neg = true;
#line 3908 "PFCfgParser.cpp"
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
#line 1005 "pf.g"
		
		is.name = LT(0)->getText();
		importer->iface_group.push_back(is);
		importer->newInterface(is.name);
		
#line 3928 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_35);
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
				goto _loop138;
			}
			
		}
		_loop138:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_15);
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
		recover(ex,_tokenSet_36);
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
#line 1044 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 4125 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_36);
	}
}

void PFCfgParser::proto_number() {
	Tracer traceInOut(this, "proto_number");
	
	try {      // for error handling
		match(INT_CONST);
#line 1050 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 4142 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_36);
	}
}

void PFCfgParser::proto_list() {
	Tracer traceInOut(this, "proto_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		proto_def();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_37.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case INT_CONST:
				case OPENING_BRACE:
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
				goto _loop149;
			}
			
		}
		_loop149:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_36);
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
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case MINUS:
		case OPENING_PAREN:
		case TO:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		recover(ex,_tokenSet_38);
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
		case LITERAL_reassemble:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case MINUS:
		case OPENING_PAREN:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
		recover(ex,_tokenSet_18);
	}
}

void PFCfgParser::src_hosts_part() {
	Tracer traceInOut(this, "src_hosts_part");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case WORD:
		case OPENING_BRACE:
		case LESS_THAN:
		case EXLAMATION:
		case SELF:
		case IPV4:
		case OPENING_PAREN:
		case IPV6:
		case ANY:
		case NO_ROUTE:
		{
			common_hosts_part();
			break;
		}
		case URPF_FAILED:
		{
			match(URPF_FAILED);
#line 1090 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::SPECIAL_ADDRESS, false,
			"urpf-failed", ""));
			
#line 4342 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 1096 "pf.g"
		
		importer->src_neg = importer->tmp_neg;
		importer->src_group.splice(importer->src_group.begin(),
		importer->tmp_group);
		
#line 4357 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_39);
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
		case INT_CONST:
		case LESS_THAN:
		case GREATER_THAN:
		case EXLAMATION:
		{
			port_op();
			break;
		}
		case OPENING_BRACE:
		{
			port_op_list();
			break;
		}
		case IPV6:
		{
			match(IPV6);
#line 1633 "pf.g"
			
			PortSpec ps;
			ps.setFromPortRange(LT(0)->getText());
			importer->tmp_port_group.push_back(ps);
			
#line 4396 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 1639 "pf.g"
		
		importer->src_port_group.splice(importer->src_port_group.begin(),
		importer->tmp_port_group);
		
#line 4410 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_38);
	}
}

void PFCfgParser::dst_hosts_part() {
	Tracer traceInOut(this, "dst_hosts_part");
	
	try {      // for error handling
		common_hosts_part();
#line 1105 "pf.g"
		
		importer->dst_neg = importer->tmp_neg;
		importer->dst_group.splice(importer->dst_group.begin(),
		importer->tmp_group);
		
#line 4429 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_40);
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
		case INT_CONST:
		case LESS_THAN:
		case GREATER_THAN:
		case EXLAMATION:
		{
			port_op();
			break;
		}
		case OPENING_BRACE:
		{
			port_op_list();
			break;
		}
		case IPV6:
		{
			match(IPV6);
#line 1655 "pf.g"
			
			PortSpec ps;
			ps.setFromPortRange(LT(0)->getText());
			importer->tmp_port_group.push_back(ps);
			
#line 4468 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 1661 "pf.g"
		
		importer->dst_port_group.splice(importer->dst_port_group.begin(),
		importer->tmp_port_group);
		
#line 4482 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_18);
	}
}

void PFCfgParser::common_hosts_part() {
	Tracer traceInOut(this, "common_hosts_part");
	
	try {      // for error handling
		switch ( LA(1)) {
		case ANY:
		{
			match(ANY);
#line 1114 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			
#line 4503 "PFCfgParser.cpp"
			break;
		}
		case NO_ROUTE:
		{
			match(NO_ROUTE);
#line 1120 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::SPECIAL_ADDRESS, false, "no-route", ""));
			
#line 4514 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case LESS_THAN:
		case EXLAMATION:
		case SELF:
		case IPV4:
		case OPENING_PAREN:
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
		recover(ex,_tokenSet_39);
	}
}

void PFCfgParser::host() {
	Tracer traceInOut(this, "host");
	ANTLR_USE_NAMESPACE(antlr)RefToken  tn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1130 "pf.g"
	AddressSpec as;
#line 4551 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 1131 "pf.g"
			as.neg = true;
#line 4561 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case LESS_THAN:
		case SELF:
		case IPV4:
		case OPENING_PAREN:
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
#line 1134 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = LT(0)->getText();
			
#line 4590 "PFCfgParser.cpp"
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
#line 1143 "pf.g"
					
					as.at = AddressSpec::INTERFACE_NETWORK;
					
#line 4605 "PFCfgParser.cpp"
					break;
				}
				case BROADCAST:
				{
					match(BROADCAST);
#line 1148 "pf.g"
					
					as.at = AddressSpec::INTERFACE_BROADCAST;
					
#line 4615 "PFCfgParser.cpp"
					break;
				}
				case PEER:
				{
					match(PEER);
#line 1153 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:peer' is not supported."));
					
#line 4626 "PFCfgParser.cpp"
					break;
				}
				case INT_CONST:
				{
					match(INT_CONST);
#line 1159 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:0' is not supported."));
					
#line 4637 "PFCfgParser.cpp"
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
			case LITERAL_reassemble:
			case COMMA:
			case CLOSING_BRACE:
			case SCRUB:
			case EXLAMATION:
			case NO:
			case MINUS:
			case OPENING_PAREN:
			case PORT:
			case TO:
			case LITERAL_fragment:
			case 141:
			case 142:
			case 143:
			case 144:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
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
#line 1167 "pf.g"
			
			as.at = AddressSpec::SPECIAL_ADDRESS;
			as.address = "self";
			
#line 4693 "PFCfgParser.cpp"
			break;
		}
		case IPV6:
		{
			match(IPV6);
#line 1173 "pf.g"
			
			importer->error_tracker->registerError(
			QString("IPv6 import is not supported. "));
			consumeUntil(NEWLINE);
			
#line 4705 "PFCfgParser.cpp"
			break;
		}
		case IPV4:
		{
			match(IPV4);
#line 1180 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 4716 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 1186 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 4726 "PFCfgParser.cpp"
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
#line 1190 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 4749 "PFCfgParser.cpp"
				break;
			}
			case NEWLINE:
			case QUEUE:
			case LITERAL_reassemble:
			case COMMA:
			case CLOSING_BRACE:
			case SCRUB:
			case EXLAMATION:
			case NO:
			case MINUS:
			case OPENING_PAREN:
			case PORT:
			case TO:
			case LITERAL_fragment:
			case 141:
			case 142:
			case 143:
			case 144:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
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
#line 1196 "pf.g"
			
			as.at = AddressSpec::TABLE;
			as.address = tn->getText();
			
#line 4800 "PFCfgParser.cpp"
			break;
		}
		case OPENING_PAREN:
		{
			match(OPENING_PAREN);
			in = LT(1);
			match(WORD);
			match(CLOSING_PAREN);
#line 1202 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = in->getText();
			
#line 4815 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 1208 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 4828 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_41);
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
				goto _loop170;
			}
			
		}
		_loop170:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_39);
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
#line 1230 "pf.g"
		
		importer->route_type = PFImporter::ROUTE_TO;
		
#line 4890 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_32);
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
#line 1237 "pf.g"
		
		importer->route_type = PFImporter::REPLY_TO;
		
#line 4925 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_32);
	}
}

void PFCfgParser::routehost() {
	Tracer traceInOut(this, "routehost");
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1242 "pf.g"
	RouteSpec rs;
#line 4941 "PFCfgParser.cpp"
	
	try {      // for error handling
		match(OPENING_PAREN);
		match(WORD);
#line 1244 "pf.g"
		rs.iface = LT(0)->getText();
#line 4948 "PFCfgParser.cpp"
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
#line 1246 "pf.g"
		
		if (v6)
		{
		importer->error_tracker->registerError(
		QString("IPv6 import is not supported. "));
		consumeUntil(NEWLINE);
		} else
		{
		if (h) rs.address = h->getText();
		if (nm) rs.netmask = nm->getText();
		importer->route_group.push_back(rs);
		}
		
#line 5020 "PFCfgParser.cpp"
		match(CLOSING_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_42);
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
				goto _loop183;
			}
			
		}
		_loop183:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_32);
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
		case SCRUB:
		{
			match_rule_scrub_options();
			break;
		}
		case LITERAL_reassemble:
		case OPENING_PAREN:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		{
			scrub_options();
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
		recover(ex,_tokenSet_43);
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
#line 1383 "pf.g"
			
			importer->flags_check = "none";
			importer->flags_mask = "none";
			
#line 5166 "PFCfgParser.cpp"
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
			case LITERAL_reassemble:
			case COMMA:
			case SCRUB:
			case EXLAMATION:
			case NO:
			case OPENING_PAREN:
			case LITERAL_fragment:
			case 141:
			case 142:
			case 143:
			case 144:
			case FLAGS:
			case ICMP_TYPE:
			case ICMP6_TYPE:
			case TAGGED:
			case TAG:
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
#line 1389 "pf.g"
			
			if (check)
			importer->flags_check = check->getText();
			else
			importer->flags_check = "any";
			if (mask)
			importer->flags_mask = mask->getText();
			else
			importer->flags_mask = "all";
			
#line 5241 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_43);
	}
}

void PFCfgParser::icmp_type() {
	Tracer traceInOut(this, "icmp_type");
	
	try {      // for error handling
		match(ICMP_TYPE);
		{
		switch ( LA(1)) {
		case LITERAL_skip:
		case INT_CONST:
		case LITERAL_echorep:
		case LITERAL_unreach:
		case LITERAL_squench:
		case LITERAL_redir:
		case LITERAL_althost:
		case LITERAL_echoreq:
		case LITERAL_routeradv:
		case LITERAL_routersol:
		case LITERAL_timex:
		case LITERAL_paramprob:
		case LITERAL_timereq:
		case LITERAL_timerep:
		case LITERAL_inforeq:
		case LITERAL_inforep:
		case LITERAL_maskreq:
		case LITERAL_maskrep:
		case LITERAL_trace:
		case LITERAL_dataconv:
		case LITERAL_mobredir:
		case 167:
		case 168:
		case LITERAL_mobregreq:
		case LITERAL_mobregrep:
		case LITERAL_photuris:
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
		recover(ex,_tokenSet_43);
	}
}

void PFCfgParser::icmp6_type() {
	Tracer traceInOut(this, "icmp6_type");
	
	try {      // for error handling
		match(ICMP6_TYPE);
#line 1564 "pf.g"
		
		importer->error_tracker->registerError(
		QString("ICMP6 import is not supported. "));
		consumeUntil(NEWLINE);
		
#line 5323 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_43);
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
#line 1596 "pf.g"
		
		importer->state_op = LT(0)->getText();
		
#line 5367 "PFCfgParser.cpp"
		match(STATE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_43);
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
#line 1605 "pf.g"
			importer->queue += LT(0)->getText();
#line 5388 "PFCfgParser.cpp"
			break;
		}
		case OPENING_PAREN:
		{
			match(OPENING_PAREN);
			match(WORD);
#line 1608 "pf.g"
			importer->queue += LT(0)->getText();
#line 5397 "PFCfgParser.cpp"
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
#line 1610 "pf.g"
					importer->queue += ",";
#line 5404 "PFCfgParser.cpp"
					match(WORD);
#line 1611 "pf.g"
					importer->queue += LT(0)->getText();
#line 5408 "PFCfgParser.cpp"
				}
				else {
					goto _loop226;
				}
				
			}
			_loop226:;
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
		recover(ex,_tokenSet_43);
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
		recover(ex,_tokenSet_43);
	}
}

void PFCfgParser::match_rule_scrub_options() {
	Tracer traceInOut(this, "match_rule_scrub_options");
	
	try {      // for error handling
		match(SCRUB);
		scrub_options();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_43);
	}
}

void PFCfgParser::scrub_options() {
	Tracer traceInOut(this, "scrub_options");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case LITERAL_reassemble:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		{
			scrub_option();
			break;
		}
		case OPENING_PAREN:
		{
			scrub_option_list();
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
		recover(ex,_tokenSet_43);
	}
}

void PFCfgParser::scrub_option() {
	Tracer traceInOut(this, "scrub_option");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case LITERAL_fragment:
		{
			match(LITERAL_fragment);
			{
			switch ( LA(1)) {
			case LITERAL_reassemble:
			{
				match(LITERAL_reassemble);
				break;
			}
			case LITERAL_crop:
			{
				match(LITERAL_crop);
				break;
			}
			case 140:
			{
				match(140);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 1335 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple("fragment", LT(0)->getText()));
			importer->scrub_rule = true;
			
#line 5531 "PFCfgParser.cpp"
			break;
		}
		case LITERAL_reassemble:
		{
			match(LITERAL_reassemble);
			match(TCP);
#line 1342 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple("reassemble", "tcp"));
			importer->scrub_rule = true;
			
#line 5544 "PFCfgParser.cpp"
			break;
		}
		case 141:
		{
			match(141);
#line 1349 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple(LT(0)->getText(), ""));
			importer->scrub_rule = true;
			
#line 5556 "PFCfgParser.cpp"
			break;
		}
		case 142:
		{
			match(142);
			match(INT_CONST);
#line 1356 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple("min-ttl", LT(0)->getText()));
			importer->scrub_rule = true;
			
#line 5569 "PFCfgParser.cpp"
			break;
		}
		case 143:
		{
			match(143);
			match(INT_CONST);
#line 1363 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple("max-mss", LT(0)->getText()));
			importer->scrub_rule = true;
			
#line 5582 "PFCfgParser.cpp"
			break;
		}
		case 144:
		{
			match(144);
#line 1370 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple(LT(0)->getText(), ""));
			importer->scrub_rule = true;
			
#line 5594 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_44);
	}
}

void PFCfgParser::scrub_option_list() {
	Tracer traceInOut(this, "scrub_option_list");
	
	try {      // for error handling
		match(OPENING_PAREN);
		scrub_option();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_45.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case LITERAL_reassemble:
				case LITERAL_fragment:
				case 141:
				case 142:
				case 143:
				case 144:
				{
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
				scrub_option();
			}
			else {
				goto _loop195;
			}
			
		}
		_loop195:;
		} // ( ... )*
		match(CLOSING_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_43);
	}
}

void PFCfgParser::icmp_type_code() {
	Tracer traceInOut(this, "icmp_type_code");
#line 1411 "pf.g"
	IcmpSpec is;
#line 5662 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case LITERAL_skip:
		case LITERAL_echorep:
		case LITERAL_unreach:
		case LITERAL_squench:
		case LITERAL_redir:
		case LITERAL_althost:
		case LITERAL_echoreq:
		case LITERAL_routeradv:
		case LITERAL_routersol:
		case LITERAL_timex:
		case LITERAL_paramprob:
		case LITERAL_timereq:
		case LITERAL_timerep:
		case LITERAL_inforeq:
		case LITERAL_inforep:
		case LITERAL_maskreq:
		case LITERAL_maskrep:
		case LITERAL_trace:
		case LITERAL_dataconv:
		case LITERAL_mobredir:
		case 167:
		case 168:
		case LITERAL_mobregreq:
		case LITERAL_mobregrep:
		case LITERAL_photuris:
		{
			icmp_type_by_name();
#line 1413 "pf.g"
			is.icmp_type_name = LT(0)->getText();
#line 5696 "PFCfgParser.cpp"
			break;
		}
		case INT_CONST:
		{
			match(INT_CONST);
#line 1415 "pf.g"
			is.icmp_type_int = LT(0)->getText();
#line 5704 "PFCfgParser.cpp"
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
			case 172:
			case 173:
			case 174:
			case 175:
			case LITERAL_needfrag:
			case LITERAL_srcfail:
			case 178:
			case 179:
			case LITERAL_isolate:
			case 181:
			case 182:
			case 183:
			case 184:
			case 185:
			case 186:
			case 187:
			case 188:
			case 189:
			case 190:
			case 191:
			case 192:
			case 193:
			case LITERAL_transit:
			case LITERAL_reassemb:
			case LITERAL_badhead:
			case LITERAL_optmiss:
			case LITERAL_badlen:
			case 199:
			case 200:
			case 201:
			{
				icmp_code_by_name();
#line 1421 "pf.g"
				is.icmp_code_name = LT(0)->getText();
#line 5754 "PFCfgParser.cpp"
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
#line 1423 "pf.g"
				is.icmp_code_int = LT(0)->getText();
#line 5762 "PFCfgParser.cpp"
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
		case LITERAL_skip:
		case LITERAL_reassemble:
		case INT_CONST:
		case COMMA:
		case CLOSING_BRACE:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case LITERAL_fragment:
		case 141:
		case 142:
		case 143:
		case 144:
		case FLAGS:
		case ICMP_TYPE:
		case LITERAL_echorep:
		case LITERAL_unreach:
		case LITERAL_squench:
		case LITERAL_redir:
		case LITERAL_althost:
		case LITERAL_echoreq:
		case LITERAL_routeradv:
		case LITERAL_routersol:
		case LITERAL_timex:
		case LITERAL_paramprob:
		case LITERAL_timereq:
		case LITERAL_timerep:
		case LITERAL_inforeq:
		case LITERAL_inforep:
		case LITERAL_maskreq:
		case LITERAL_maskrep:
		case LITERAL_trace:
		case LITERAL_dataconv:
		case LITERAL_mobredir:
		case 167:
		case 168:
		case LITERAL_mobregreq:
		case LITERAL_mobregrep:
		case LITERAL_photuris:
		case ICMP6_TYPE:
		case TAGGED:
		case TAG:
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
#line 1426 "pf.g"
		
		importer->icmp_type_code_group.push_back(is);
		
#line 5835 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_46);
	}
}

void PFCfgParser::icmp_list() {
	Tracer traceInOut(this, "icmp_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		icmp_type_code();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_47.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case LITERAL_skip:
				case INT_CONST:
				case LITERAL_echorep:
				case LITERAL_unreach:
				case LITERAL_squench:
				case LITERAL_redir:
				case LITERAL_althost:
				case LITERAL_echoreq:
				case LITERAL_routeradv:
				case LITERAL_routersol:
				case LITERAL_timex:
				case LITERAL_paramprob:
				case LITERAL_timereq:
				case LITERAL_timerep:
				case LITERAL_inforeq:
				case LITERAL_inforep:
				case LITERAL_maskreq:
				case LITERAL_maskrep:
				case LITERAL_trace:
				case LITERAL_dataconv:
				case LITERAL_mobredir:
				case 167:
				case 168:
				case LITERAL_mobregreq:
				case LITERAL_mobregrep:
				case LITERAL_photuris:
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
				goto _loop216;
			}
			
		}
		_loop216:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_43);
	}
}

void PFCfgParser::icmp_type_by_name() {
	Tracer traceInOut(this, "icmp_type_by_name");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case LITERAL_echorep:
		{
			match(LITERAL_echorep);
			break;
		}
		case LITERAL_unreach:
		{
			match(LITERAL_unreach);
			break;
		}
		case LITERAL_squench:
		{
			match(LITERAL_squench);
			break;
		}
		case LITERAL_redir:
		{
			match(LITERAL_redir);
			break;
		}
		case LITERAL_althost:
		{
			match(LITERAL_althost);
			break;
		}
		case LITERAL_echoreq:
		{
			match(LITERAL_echoreq);
			break;
		}
		case LITERAL_routeradv:
		{
			match(LITERAL_routeradv);
			break;
		}
		case LITERAL_routersol:
		{
			match(LITERAL_routersol);
			break;
		}
		case LITERAL_timex:
		{
			match(LITERAL_timex);
			break;
		}
		case LITERAL_paramprob:
		{
			match(LITERAL_paramprob);
			break;
		}
		case LITERAL_timereq:
		{
			match(LITERAL_timereq);
			break;
		}
		case LITERAL_timerep:
		{
			match(LITERAL_timerep);
			break;
		}
		case LITERAL_inforeq:
		{
			match(LITERAL_inforeq);
			break;
		}
		case LITERAL_inforep:
		{
			match(LITERAL_inforep);
			break;
		}
		case LITERAL_maskreq:
		{
			match(LITERAL_maskreq);
			break;
		}
		case LITERAL_maskrep:
		{
			match(LITERAL_maskrep);
			break;
		}
		case LITERAL_trace:
		{
			match(LITERAL_trace);
			break;
		}
		case LITERAL_dataconv:
		{
			match(LITERAL_dataconv);
			break;
		}
		case LITERAL_mobredir:
		{
			match(LITERAL_mobredir);
			break;
		}
		case 167:
		{
			match(167);
			break;
		}
		case 168:
		{
			match(168);
			break;
		}
		case LITERAL_mobregreq:
		{
			match(LITERAL_mobregreq);
			break;
		}
		case LITERAL_mobregrep:
		{
			match(LITERAL_mobregrep);
			break;
		}
		case LITERAL_skip:
		{
			match(LITERAL_skip);
			break;
		}
		case LITERAL_photuris:
		{
			match(LITERAL_photuris);
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
		recover(ex,_tokenSet_48);
	}
}

void PFCfgParser::port_op() {
	Tracer traceInOut(this, "port_op");
#line 1693 "pf.g"
	PortSpec ps;
#line 6059 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EQUAL:
		case LESS_THAN:
		case GREATER_THAN:
		case EXLAMATION:
		{
			unary_port_op();
#line 1695 "pf.g"
			ps.port_op = importer->tmp_port_op;
#line 6072 "PFCfgParser.cpp"
			port_def();
#line 1697 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = importer->tmp_port_def;
			
#line 6079 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case INT_CONST:
		{
			port_def();
#line 1703 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = ps.port1;
			ps.port_op = "=";
			
#line 6092 "PFCfgParser.cpp"
			{
			if ((LA(1) == LESS_THAN || LA(1) == GREATER_THAN || LA(1) == COLON) && (_tokenSet_49.member(LA(2)))) {
				binary_port_op();
#line 1709 "pf.g"
				ps.port_op = importer->tmp_port_op;
#line 6098 "PFCfgParser.cpp"
				port_def();
#line 1710 "pf.g"
				ps.port2 = LT(0)->getText();
#line 6102 "PFCfgParser.cpp"
			}
			else if ((_tokenSet_50.member(LA(1))) && (_tokenSet_51.member(LA(2)))) {
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
#line 1713 "pf.g"
		
		importer->tmp_port_group.push_back(ps);
		
#line 6123 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_50);
	}
}

void PFCfgParser::port_op_list() {
	Tracer traceInOut(this, "port_op_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		port_op();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_52.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					break;
				}
				case WORD:
				case EQUAL:
				case INT_CONST:
				case LESS_THAN:
				case GREATER_THAN:
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
				port_op();
			}
			else {
				goto _loop244;
			}
			
		}
		_loop244:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_38);
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
#line 1669 "pf.g"
			importer->tmp_port_op = "=";
#line 6190 "PFCfgParser.cpp"
			break;
		}
		case EXLAMATION:
		{
			match(EXLAMATION);
			match(EQUAL);
#line 1671 "pf.g"
			importer->tmp_port_op = "!=";
#line 6199 "PFCfgParser.cpp"
			break;
		}
		default:
			if ((LA(1) == LESS_THAN) && (LA(2) == WORD || LA(2) == INT_CONST)) {
				match(LESS_THAN);
#line 1673 "pf.g"
				importer->tmp_port_op = "<";
#line 6207 "PFCfgParser.cpp"
			}
			else if ((LA(1) == LESS_THAN) && (LA(2) == EQUAL)) {
				match(LESS_THAN);
				match(EQUAL);
#line 1675 "pf.g"
				importer->tmp_port_op = "<=";
#line 6214 "PFCfgParser.cpp"
			}
			else if ((LA(1) == GREATER_THAN) && (LA(2) == WORD || LA(2) == INT_CONST)) {
				match(GREATER_THAN);
#line 1677 "pf.g"
				importer->tmp_port_op = ">";
#line 6220 "PFCfgParser.cpp"
			}
			else if ((LA(1) == GREATER_THAN) && (LA(2) == EQUAL)) {
				match(GREATER_THAN);
				match(EQUAL);
#line 1679 "pf.g"
				importer->tmp_port_op = ">=";
#line 6227 "PFCfgParser.cpp"
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_53);
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
#line 1685 "pf.g"
			importer->tmp_port_op = "<>";
#line 6253 "PFCfgParser.cpp"
			break;
		}
		case GREATER_THAN:
		{
			match(GREATER_THAN);
			match(LESS_THAN);
#line 1687 "pf.g"
			importer->tmp_port_op = "><";
#line 6262 "PFCfgParser.cpp"
			break;
		}
		case COLON:
		{
			match(COLON);
#line 1689 "pf.g"
			importer->tmp_port_op = ":";
#line 6270 "PFCfgParser.cpp"
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
		recover(ex,_tokenSet_53);
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
	"\"timeout\"",
	"\"ruleset-optimization\"",
	"\"optimization\"",
	"\"aggressive\"",
	"\"conservative\"",
	"\"high-latency\"",
	"\"normal\"",
	"\"satellite\"",
	"\"limit\"",
	"\"loginterface\"",
	"\"block-policy\"",
	"\"drop\"",
	"\"return\"",
	"\"state-policy\"",
	"\"if-bound\"",
	"\"floating\"",
	"\"state-defaults\"",
	"\"require-order\"",
	"\"fingerprints\"",
	"\"skip\"",
	"\"on\"",
	"\"debug\"",
	"\"reassemble\"",
	"\"hostid\"",
	"\"tcp.first\"",
	"\"tcp.opening\"",
	"\"tcp.established\"",
	"\"tcp.closing\"",
	"\"tcp.finwait\"",
	"\"tcp.closed\"",
	"\"udp.first\"",
	"\"udp.single\"",
	"\"udp.multiple\"",
	"\"icmp.first\"",
	"\"icmp.error\"",
	"\"other.first\"",
	"\"other.single\"",
	"\"other.multiple\"",
	"\"frag\"",
	"\"interval\"",
	"\"src.track\"",
	"\"adaptive.start\"",
	"\"adaptive.end\"",
	"INT_CONST",
	"OPENING_BRACE",
	"COMMA",
	"CLOSING_BRACE",
	"\"frags\"",
	"\"states\"",
	"\"src-nodes\"",
	"\"tables\"",
	"\"tables-entries\"",
	"\"scrub\"",
	"\"match\"",
	"\"table\"",
	"LESS_THAN",
	"GREATER_THAN",
	"\"persist\"",
	"\"const\"",
	"COUNTERS",
	"\"file\"",
	"STRING",
	"EXLAMATION",
	"COLON",
	"\"network\"",
	"\"broadcast\"",
	"\"peer\"",
	"\"self\"",
	"IPV4",
	"SLASH",
	"\"no\"",
	"\"nat\"",
	"\"pass\"",
	"MINUS",
	"\"static-port\"",
	"\"rdr\"",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"\"port\"",
	"IPV6",
	"STAR",
	"\"bitmask\"",
	"\"random\"",
	"\"source-hash\"",
	"\"hex-key\"",
	"\"string-key\"",
	"\"round-robin\"",
	"\"sticky-address\"",
	"\"binat\"",
	"\"block\"",
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
	"\"route-to\"",
	"\"reply-to\"",
	"\"fragment\"",
	"\"crop\"",
	"\"drop-ovl\"",
	"\"no-df\"",
	"\"min-ttl\"",
	"\"max-mss\"",
	"\"random-id\"",
	"\"flags\"",
	"\"icmp-type\"",
	"\"code\"",
	"\"echorep\"",
	"\"unreach\"",
	"\"squench\"",
	"\"redir\"",
	"\"althost\"",
	"\"echoreq\"",
	"\"routeradv\"",
	"\"routersol\"",
	"\"timex\"",
	"\"paramprob\"",
	"\"timereq\"",
	"\"timerep\"",
	"\"inforeq\"",
	"\"inforep\"",
	"\"maskreq\"",
	"\"maskrep\"",
	"\"trace\"",
	"\"dataconv\"",
	"\"mobredir\"",
	"\"ipv6-where\"",
	"\"ipv6-here\"",
	"\"mobregreq\"",
	"\"mobregrep\"",
	"\"photuris\"",
	"\"net-unr\"",
	"\"host-unr\"",
	"\"proto-unr\"",
	"\"port-unr\"",
	"\"needfrag\"",
	"\"srcfail\"",
	"\"net-unk\"",
	"\"host-unk\"",
	"\"isolate\"",
	"\"net-prohib\"",
	"\"host-prohib\"",
	"\"net-tos\"",
	"\"host-tos\"",
	"\"filter-prohib\"",
	"\"host-preced\"",
	"\"cutoff-preced\"",
	"\"redir-net\"",
	"\"redir-host\"",
	"\"redir-tos-net\"",
	"\"redir-tos-host\"",
	"\"normal-adv\"",
	"\"common-adv\"",
	"\"transit\"",
	"\"reassemb\"",
	"\"badhead\"",
	"\"optmiss\"",
	"\"badlen\"",
	"\"unknown-ind\"",
	"\"auth-fail\"",
	"\"decrypt-fail\"",
	"\"icmp6-type\"",
	"\"tagged\"",
	"\"tag\"",
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

const unsigned long PFCfgParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_0(_tokenSet_0_data_,10);
const unsigned long PFCfgParser::_tokenSet_1_data_[] = { 3954UL, 0UL, 10223623UL, 48UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "scrub" 
// "match" "table" "no" "nat" "pass" "rdr" "binat" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_1(_tokenSet_1_data_,10);
const unsigned long PFCfgParser::_tokenSet_2_data_[] = { 64UL, 33554432UL, 99328UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA EXLAMATION "self" IPV4 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_2(_tokenSet_2_data_,10);
const unsigned long PFCfgParser::_tokenSet_3_data_[] = { 3954UL, 109051888UL, 10223623UL, 48UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "tcp.first" 
// "tcp.opening" "tcp.established" "tcp.closing" "tcp.finwait" "tcp.closed" 
// "udp.first" "udp.single" "udp.multiple" "icmp.first" "icmp.error" "other.first" 
// "other.single" "other.multiple" "frag" "interval" "src.track" "adaptive.start" 
// "adaptive.end" COMMA CLOSING_BRACE "scrub" "match" "table" "no" "nat" 
// "pass" "rdr" "binat" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_3(_tokenSet_3_data_,10);
const unsigned long PFCfgParser::_tokenSet_4_data_[] = { 0UL, 41943024UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "tcp.first" "tcp.opening" "tcp.established" "tcp.closing" "tcp.finwait" 
// "tcp.closed" "udp.first" "udp.single" "udp.multiple" "icmp.first" "icmp.error" 
// "other.first" "other.single" "other.multiple" "frag" "interval" "src.track" 
// "adaptive.start" "adaptive.end" COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_4(_tokenSet_4_data_,10);
const unsigned long PFCfgParser::_tokenSet_5_data_[] = { 3954UL, 4261412864UL, 10223623UL, 48UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" COMMA 
// CLOSING_BRACE "frags" "states" "src-nodes" "tables" "tables-entries" 
// "scrub" "match" "table" "no" "nat" "pass" "rdr" "binat" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_5(_tokenSet_5_data_,10);
const unsigned long PFCfgParser::_tokenSet_6_data_[] = { 0UL, 4194304000UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// COMMA "frags" "states" "src-nodes" "tables" "tables-entries" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_6(_tokenSet_6_data_,10);
const unsigned long PFCfgParser::_tokenSet_7_data_[] = { 1040UL, 4UL, 17040385UL, 40960UL, 517136UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN "all" 
// "to" "from" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_7(_tokenSet_7_data_,16);
const unsigned long PFCfgParser::_tokenSet_8_data_[] = { 2147487602UL, 58720260UL, 161449487UL, 8388656UL, 4294442208UL, 4095UL, 261120UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "skip" 
// "reassemble" INT_CONST OPENING_BRACE COMMA "scrub" "match" "table" LESS_THAN 
// STRING EXLAMATION "self" IPV4 SLASH "no" "nat" "pass" "rdr" OPENING_PAREN 
// IPV6 "binat" "block" "tcp" "urpf-failed" "any" "no-route" "fragment" 
// "crop" "drop-ovl" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "echorep" "unreach" "squench" "redir" "althost" "echoreq" "routeradv" 
// "routersol" "timex" "paramprob" "timereq" "timerep" "inforeq" "inforep" 
// "maskreq" "maskrep" "trace" "dataconv" "mobredir" "ipv6-where" "ipv6-here" 
// "mobregreq" "mobregrep" "photuris" "icmp6-type" "tagged" "tag" "keep" 
// "modulate" "synproxy" "state" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_8(_tokenSet_8_data_,16);
const unsigned long PFCfgParser::_tokenSet_9_data_[] = { 1040UL, 4UL, 17040385UL, 0UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_9(_tokenSet_9_data_,16);
const unsigned long PFCfgParser::_tokenSet_10_data_[] = { 2147487602UL, 58720260UL, 27133447UL, 8388656UL, 4294442048UL, 4095UL, 261120UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "skip" 
// "reassemble" INT_CONST OPENING_BRACE COMMA "scrub" "match" "table" STRING 
// EXLAMATION SLASH "no" "nat" "pass" "rdr" OPENING_PAREN "binat" "block" 
// "tcp" "any" "fragment" "crop" "drop-ovl" "no-df" "min-ttl" "max-mss" 
// "random-id" "flags" "icmp-type" "echorep" "unreach" "squench" "redir" 
// "althost" "echoreq" "routeradv" "routersol" "timex" "paramprob" "timereq" 
// "timerep" "inforeq" "inforep" "maskreq" "maskrep" "trace" "dataconv" 
// "mobredir" "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" "photuris" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "state" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_10(_tokenSet_10_data_,16);
const unsigned long PFCfgParser::_tokenSet_11_data_[] = { 16UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_11(_tokenSet_11_data_,10);
const unsigned long PFCfgParser::_tokenSet_12_data_[] = { 64UL, 100663296UL, 99328UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA CLOSING_BRACE EXLAMATION "self" IPV4 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_12(_tokenSet_12_data_,10);
const unsigned long PFCfgParser::_tokenSet_13_data_[] = { 1040UL, 5UL, 19137537UL, 1024000UL, 517904UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "on" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "all" "to" "quick" "inet" "inet6" "proto" "from" "route-to" "reply-to" 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_13(_tokenSet_13_data_,16);
const unsigned long PFCfgParser::_tokenSet_14_data_[] = { 2147487602UL, 58720261UL, 163546655UL, 4294877232UL, 4294443007UL, 4095UL, 261120UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "skip" 
// "on" "reassemble" INT_CONST OPENING_BRACE COMMA "scrub" "match" "table" 
// LESS_THAN GREATER_THAN STRING EXLAMATION "self" IPV4 SLASH "no" "nat" 
// "pass" MINUS "rdr" OPENING_PAREN IPV6 "binat" "block" "all" "to" "inet" 
// "inet6" "proto" "ip" "icmp" "igmp" "tcp" "udp" "rdp" "rsvp" "gre" "esp" 
// "ah" "eigrp" "ospf" "ipip" "vrrp" "l2tp" "isis" "from" "urpf-failed" 
// "any" "no-route" "route-to" "reply-to" "fragment" "crop" "drop-ovl" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "echorep" 
// "unreach" "squench" "redir" "althost" "echoreq" "routeradv" "routersol" 
// "timex" "paramprob" "timereq" "timerep" "inforeq" "inforep" "maskreq" 
// "maskrep" "trace" "dataconv" "mobredir" "ipv6-where" "ipv6-here" "mobregreq" 
// "mobregrep" "photuris" "icmp6-type" "tagged" "tag" "keep" "modulate" 
// "synproxy" "state" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_14(_tokenSet_14_data_,16);
const unsigned long PFCfgParser::_tokenSet_15_data_[] = { 1040UL, 4UL, 19137537UL, 958464UL, 517904UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "all" "to" "inet" "inet6" "proto" "from" "route-to" "reply-to" "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_15(_tokenSet_15_data_,16);
const unsigned long PFCfgParser::_tokenSet_16_data_[] = { 1040UL, 4UL, 19137537UL, 565248UL, 517136UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "all" "to" "proto" "from" "fragment" "no-df" "min-ttl" "max-mss" "random-id" 
// "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_16(_tokenSet_16_data_,16);
const unsigned long PFCfgParser::_tokenSet_17_data_[] = { 1040UL, 4UL, 19137537UL, 40960UL, 517136UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "all" "to" "from" "fragment" "no-df" "min-ttl" "max-mss" "random-id" 
// "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_17(_tokenSet_17_data_,16);
const unsigned long PFCfgParser::_tokenSet_18_data_[] = { 1040UL, 4UL, 19137537UL, 0UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_18(_tokenSet_18_data_,16);
const unsigned long PFCfgParser::_tokenSet_19_data_[] = { 1040UL, 33554436UL, 19137537UL, 0UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" COMMA "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_19(_tokenSet_19_data_,16);
const unsigned long PFCfgParser::_tokenSet_20_data_[] = { 80UL, 100663296UL, 3846242304UL, 4UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD COMMA CLOSING_BRACE IPV4 "static-port" OPENING_PAREN "port" 
// "bitmask" "random" "source-hash" "round-robin" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_20(_tokenSet_20_data_,10);
const unsigned long PFCfgParser::_tokenSet_21_data_[] = { 64UL, 33554432UL, 16842752UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA IPV4 OPENING_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_21(_tokenSet_21_data_,10);
const unsigned long PFCfgParser::_tokenSet_22_data_[] = { 16UL, 0UL, 3829399552UL, 4UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "static-port" "port" "bitmask" "random" "source-hash" "round-robin" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_22(_tokenSet_22_data_,10);
const unsigned long PFCfgParser::_tokenSet_23_data_[] = { 16UL, 0UL, 3762290688UL, 4UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "static-port" "bitmask" "random" "source-hash" "round-robin" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_23(_tokenSet_23_data_,10);
const unsigned long PFCfgParser::_tokenSet_24_data_[] = { 16UL, 0UL, 4194304UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "static-port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_24(_tokenSet_24_data_,10);
const unsigned long PFCfgParser::_tokenSet_25_data_[] = { 1232UL, 109051908UL, 3781430297UL, 32772UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD EQUAL "queue" "reassemble" INT_CONST COMMA CLOSING_BRACE 
// "scrub" LESS_THAN GREATER_THAN EXLAMATION COLON "no" MINUS "static-port" 
// OPENING_PAREN "bitmask" "random" "source-hash" "round-robin" "to" "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_25(_tokenSet_25_data_,16);
const unsigned long PFCfgParser::_tokenSet_26_data_[] = { 0UL, 8388608UL, 0UL, 0UL, 0UL, 4294963200UL, 1023UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// INT_CONST "net-unr" "host-unr" "proto-unr" "port-unr" "needfrag" "srcfail" 
// "net-unk" "host-unk" "isolate" "net-prohib" "host-prohib" "net-tos" 
// "host-tos" "filter-prohib" "host-preced" "cutoff-preced" "redir-net" 
// "redir-host" "redir-tos-net" "redir-tos-host" "normal-adv" "common-adv" 
// "transit" "reassemb" "badhead" "optmiss" "badlen" "unknown-ind" "auth-fail" 
// "decrypt-fail" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_26(_tokenSet_26_data_,16);
const unsigned long PFCfgParser::_tokenSet_27_data_[] = { 1040UL, 5UL, 17040385UL, 1031168UL, 517904UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "on" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN 
// "in" "out" "log" "all" "to" "quick" "inet" "inet6" "proto" "from" "route-to" 
// "reply-to" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_27(_tokenSet_27_data_,16);
const unsigned long PFCfgParser::_tokenSet_28_data_[] = { 2147487602UL, 58720261UL, 161449487UL, 4294946864UL, 4294443007UL, 4095UL, 261120UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "skip" 
// "on" "reassemble" INT_CONST OPENING_BRACE COMMA "scrub" "match" "table" 
// LESS_THAN STRING EXLAMATION "self" IPV4 SLASH "no" "nat" "pass" "rdr" 
// OPENING_PAREN IPV6 "binat" "block" "log" "all" "to" "quick" "inet" "inet6" 
// "proto" "ip" "icmp" "igmp" "tcp" "udp" "rdp" "rsvp" "gre" "esp" "ah" 
// "eigrp" "ospf" "ipip" "vrrp" "l2tp" "isis" "from" "urpf-failed" "any" 
// "no-route" "route-to" "reply-to" "fragment" "crop" "drop-ovl" "no-df" 
// "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "echorep" "unreach" 
// "squench" "redir" "althost" "echoreq" "routeradv" "routersol" "timex" 
// "paramprob" "timereq" "timerep" "inforeq" "inforep" "maskreq" "maskrep" 
// "trace" "dataconv" "mobredir" "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" 
// "photuris" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "state" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_28(_tokenSet_28_data_,16);
const unsigned long PFCfgParser::_tokenSet_29_data_[] = { 2147484688UL, 109051908UL, 50594817UL, 0UL, 4294435840UL, 4095UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "skip" "reassemble" INT_CONST COMMA CLOSING_BRACE "scrub" 
// EXLAMATION "no" OPENING_PAREN CLOSING_PAREN "fragment" "no-df" "min-ttl" 
// "max-mss" "random-id" "flags" "icmp-type" "echorep" "unreach" "squench" 
// "redir" "althost" "echoreq" "routeradv" "routersol" "timex" "paramprob" 
// "timereq" "timerep" "inforeq" "inforep" "maskreq" "maskrep" "trace" 
// "dataconv" "mobredir" "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" 
// "photuris" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_29(_tokenSet_29_data_,16);
const unsigned long PFCfgParser::_tokenSet_30_data_[] = { 1040UL, 5UL, 17040385UL, 1028096UL, 517904UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "on" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN 
// "log" "all" "to" "quick" "inet" "inet6" "proto" "from" "route-to" "reply-to" 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_30(_tokenSet_30_data_,16);
const unsigned long PFCfgParser::_tokenSet_31_data_[] = { 1040UL, 5UL, 17040385UL, 958464UL, 517904UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "on" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN 
// "all" "to" "inet" "inet6" "proto" "from" "route-to" "reply-to" "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_31(_tokenSet_31_data_,16);
const unsigned long PFCfgParser::_tokenSet_32_data_[] = { 1040UL, 4UL, 17040385UL, 958464UL, 517136UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN "all" 
// "to" "inet" "inet6" "proto" "from" "fragment" "no-df" "min-ttl" "max-mss" 
// "random-id" "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" 
// "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_32(_tokenSet_32_data_,16);
const unsigned long PFCfgParser::_tokenSet_33_data_[] = { 1024UL, 33554436UL, 17040385UL, 0UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "queue" "reassemble" COMMA "scrub" EXLAMATION "no" OPENING_PAREN "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_33(_tokenSet_33_data_,16);
const unsigned long PFCfgParser::_tokenSet_34_data_[] = { 0UL, 33554432UL, 33554432UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// COMMA CLOSING_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_34(_tokenSet_34_data_,10);
const unsigned long PFCfgParser::_tokenSet_35_data_[] = { 1104UL, 100663300UL, 19137537UL, 958464UL, 517904UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD "queue" "reassemble" COMMA CLOSING_BRACE "scrub" EXLAMATION 
// "no" MINUS OPENING_PAREN "all" "to" "inet" "inet6" "proto" "from" "route-to" 
// "reply-to" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_35(_tokenSet_35_data_,16);
const unsigned long PFCfgParser::_tokenSet_36_data_[] = { 1040UL, 125829124UL, 19137537UL, 4293959680UL, 517151UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" INT_CONST OPENING_BRACE COMMA CLOSING_BRACE 
// "scrub" EXLAMATION "no" MINUS OPENING_PAREN "all" "to" "ip" "icmp" "igmp" 
// "tcp" "udp" "rdp" "rsvp" "gre" "esp" "ah" "eigrp" "ospf" "ipip" "vrrp" 
// "l2tp" "isis" "from" "fragment" "no-df" "min-ttl" "max-mss" "random-id" 
// "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_36(_tokenSet_36_data_,16);
const unsigned long PFCfgParser::_tokenSet_37_data_[] = { 0UL, 58720256UL, 0UL, 4293918720UL, 15UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// INT_CONST OPENING_BRACE COMMA "ip" "icmp" "igmp" "tcp" "udp" "rdp" "rsvp" 
// "gre" "esp" "ah" "eigrp" "ospf" "ipip" "vrrp" "l2tp" "isis" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_37(_tokenSet_37_data_,12);
const unsigned long PFCfgParser::_tokenSet_38_data_[] = { 1040UL, 4UL, 19137537UL, 32768UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "to" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_38(_tokenSet_38_data_,16);
const unsigned long PFCfgParser::_tokenSet_39_data_[] = { 1040UL, 4UL, 86246401UL, 32768UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "port" "to" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_39(_tokenSet_39_data_,16);
const unsigned long PFCfgParser::_tokenSet_40_data_[] = { 1040UL, 4UL, 86246401UL, 0UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "port" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_40(_tokenSet_40_data_,16);
const unsigned long PFCfgParser::_tokenSet_41_data_[] = { 1040UL, 100663300UL, 86246401UL, 32768UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" COMMA CLOSING_BRACE "scrub" EXLAMATION 
// "no" MINUS OPENING_PAREN "port" "to" "fragment" "no-df" "min-ttl" "max-mss" 
// "random-id" "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" 
// "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_41(_tokenSet_41_data_,16);
const unsigned long PFCfgParser::_tokenSet_42_data_[] = { 1040UL, 100663300UL, 17040385UL, 958464UL, 517136UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" COMMA CLOSING_BRACE "scrub" EXLAMATION 
// "no" OPENING_PAREN "all" "to" "inet" "inet6" "proto" "from" "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_42(_tokenSet_42_data_,16);
const unsigned long PFCfgParser::_tokenSet_43_data_[] = { 1040UL, 33554436UL, 17040385UL, 0UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" COMMA "scrub" EXLAMATION "no" OPENING_PAREN 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_43(_tokenSet_43_data_,16);
const unsigned long PFCfgParser::_tokenSet_44_data_[] = { 1040UL, 33554436UL, 50594817UL, 0UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" COMMA "scrub" EXLAMATION "no" OPENING_PAREN 
// CLOSING_PAREN "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_44(_tokenSet_44_data_,16);
const unsigned long PFCfgParser::_tokenSet_45_data_[] = { 0UL, 33554436UL, 0UL, 0UL, 123904UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "reassemble" COMMA "fragment" "no-df" "min-ttl" "max-mss" "random-id" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_45(_tokenSet_45_data_,12);
const unsigned long PFCfgParser::_tokenSet_46_data_[] = { 2147484688UL, 109051908UL, 17040385UL, 0UL, 4294435840UL, 4095UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "skip" "reassemble" INT_CONST COMMA CLOSING_BRACE "scrub" 
// EXLAMATION "no" OPENING_PAREN "fragment" "no-df" "min-ttl" "max-mss" 
// "random-id" "flags" "icmp-type" "echorep" "unreach" "squench" "redir" 
// "althost" "echoreq" "routeradv" "routersol" "timex" "paramprob" "timereq" 
// "timerep" "inforeq" "inforep" "maskreq" "maskrep" "trace" "dataconv" 
// "mobredir" "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" "photuris" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_46(_tokenSet_46_data_,16);
const unsigned long PFCfgParser::_tokenSet_47_data_[] = { 2147483648UL, 41943040UL, 0UL, 0UL, 4293918720UL, 4095UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "skip" INT_CONST COMMA "echorep" "unreach" "squench" "redir" "althost" 
// "echoreq" "routeradv" "routersol" "timex" "paramprob" "timereq" "timerep" 
// "inforeq" "inforep" "maskreq" "maskrep" "trace" "dataconv" "mobredir" 
// "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" "photuris" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_47(_tokenSet_47_data_,12);
const unsigned long PFCfgParser::_tokenSet_48_data_[] = { 2147484688UL, 109051908UL, 17040385UL, 0UL, 4294960128UL, 4095UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "skip" "reassemble" INT_CONST COMMA CLOSING_BRACE "scrub" 
// EXLAMATION "no" OPENING_PAREN "fragment" "no-df" "min-ttl" "max-mss" 
// "random-id" "flags" "icmp-type" "code" "echorep" "unreach" "squench" 
// "redir" "althost" "echoreq" "routeradv" "routersol" "timex" "paramprob" 
// "timereq" "timerep" "inforeq" "inforep" "maskreq" "maskrep" "trace" 
// "dataconv" "mobredir" "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" 
// "photuris" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_48(_tokenSet_48_data_,16);
const unsigned long PFCfgParser::_tokenSet_49_data_[] = { 64UL, 8388608UL, 24UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD INT_CONST LESS_THAN GREATER_THAN 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_49(_tokenSet_49_data_,10);
const unsigned long PFCfgParser::_tokenSet_50_data_[] = { 1232UL, 109051908UL, 19137561UL, 32768UL, 517120UL, 0UL, 195584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD EQUAL "queue" "reassemble" INT_CONST COMMA CLOSING_BRACE 
// "scrub" LESS_THAN GREATER_THAN EXLAMATION "no" MINUS OPENING_PAREN "to" 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_50(_tokenSet_50_data_,16);
const unsigned long PFCfgParser::_tokenSet_51_data_[] = { 2147487730UL, 125829124UL, 163548703UL, 8421424UL, 4294442176UL, 4095UL, 261120UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD EQUAL "antispoof" "altq" "queue" "set" 
// "skip" "reassemble" INT_CONST OPENING_BRACE COMMA CLOSING_BRACE "scrub" 
// "match" "table" LESS_THAN GREATER_THAN STRING EXLAMATION COLON "self" 
// IPV4 SLASH "no" "nat" "pass" MINUS "rdr" OPENING_PAREN IPV6 "binat" 
// "block" "to" "tcp" "any" "no-route" "fragment" "crop" "drop-ovl" "no-df" 
// "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "echorep" "unreach" 
// "squench" "redir" "althost" "echoreq" "routeradv" "routersol" "timex" 
// "paramprob" "timereq" "timerep" "inforeq" "inforep" "maskreq" "maskrep" 
// "trace" "dataconv" "mobredir" "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" 
// "photuris" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "state" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_51(_tokenSet_51_data_,16);
const unsigned long PFCfgParser::_tokenSet_52_data_[] = { 192UL, 41943040UL, 1048UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD EQUAL INT_CONST COMMA LESS_THAN GREATER_THAN EXLAMATION 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_52(_tokenSet_52_data_,10);
const unsigned long PFCfgParser::_tokenSet_53_data_[] = { 64UL, 8388608UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_53(_tokenSet_53_data_,10);



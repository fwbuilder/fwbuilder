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
#line 497 "pf.g"
		
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
#line 512 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "match";
		*dbg << LT(1)->getLine() << ":" << " match ";
		
#line 377 "PFCfgParser.cpp"
		rule_extended();
#line 520 "pf.g"
		
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
#line 529 "pf.g"
		
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
		case NEWLINE:
		case OPENING_BRACE:
		case CONST_WORD:
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
		case CONST_WORD:
		{
			match(CONST_WORD);
#line 541 "pf.g"
			
			importer->addMessageToLog(
			QString("Warning: attribute \"const\" will be dropped from table configuration since this attribute is not supported at this time"));
			
#line 440 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
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
#line 548 "pf.g"
			
			importer->addMessageToLog(
			QString("Warning: attribute \"counters\" will be dropped from table configuration since this attribute is not supported at this time"));
			
#line 466 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
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
#line 555 "pf.g"
			
			importer->newAddressTableObject(
			name->getText(), file->getText());
			
#line 493 "PFCfgParser.cpp"
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
					case INT_CONST:
					case EXLAMATION:
					case SELF:
					case IPV4:
					case NUMBER:
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
					goto _loop57;
				}
				
			}
			_loop57:;
			} // ( ... )*
			match(CLOSING_BRACE);
#line 567 "pf.g"
			
			importer->newAddressTableObject(
			name->getText(), importer->tmp_group);
			
#line 540 "PFCfgParser.cpp"
			break;
		}
		case NEWLINE:
		{
			match(NEWLINE);
#line 573 "pf.g"
			
			// Special case: table definition without file name or list of addresses.
			// Create run-time AddressTable object with name but no file spec.
			importer->newAddressTableObject(name->getText(), "");
			
#line 552 "PFCfgParser.cpp"
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
#line 648 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newNATRule();
		importer->action = "nonat";
		*dbg << LT(1)->getLine() << ":" << " nonat ";
		
#line 581 "PFCfgParser.cpp"
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
#line 665 "pf.g"
		
		if ( importer->action != "nonat" )
		{
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newNATRule();
		importer->action = "nat";
		*dbg << LT(1)->getLine() << ":" << " nat ";
		}
		
#line 623 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case PASS:
		{
			match(PASS);
#line 677 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat pass' commands is not supported."));
			
#line 634 "PFCfgParser.cpp"
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
#line 690 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat ... tag' commands is not supported."));
			
#line 794 "PFCfgParser.cpp"
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
#line 698 "pf.g"
			
			importer->nat_group = importer->tmp_group;
			
#line 838 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case PORT:
			{
				portspec();
#line 703 "pf.g"
				
				importer->nat_port_group = importer->tmp_port_group;
				
#line 848 "PFCfgParser.cpp"
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
#line 709 "pf.g"
				importer->nat_rule_opt_2 = "static-port";
#line 894 "PFCfgParser.cpp"
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
#line 712 "pf.g"
		
		importer->pushRule();
		
#line 923 "PFCfgParser.cpp"
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
#line 721 "pf.g"
		
		if ( importer->action != "nonat" )
		{
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newNATRule();
		importer->action = "rdr";
		*dbg << LT(1)->getLine() << ":" << " rdr ";
		}
		
#line 948 "PFCfgParser.cpp"
		{
		switch ( LA(1)) {
		case PASS:
		{
			match(PASS);
#line 733 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat pass' commands is not supported."));
			
#line 959 "PFCfgParser.cpp"
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
#line 746 "pf.g"
			
			importer->error_tracker->registerError(
			QString("import of 'nat ... tag' commands is not supported."));
			
#line 1119 "PFCfgParser.cpp"
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
#line 754 "pf.g"
			
			importer->nat_group = importer->tmp_group;
			
#line 1163 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case PORT:
			{
				portspec();
#line 759 "pf.g"
				
				importer->nat_port_group = importer->tmp_port_group;
				
#line 1173 "PFCfgParser.cpp"
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
#line 765 "pf.g"
		
		importer->pushRule();
		
#line 1226 "PFCfgParser.cpp"
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
#line 897 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->error_tracker->registerError(
		QString("import of 'binat' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1248 "PFCfgParser.cpp"
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
#line 920 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "pass";
		*dbg << LT(1)->getLine() << ":" << " pass ";
		
#line 1269 "PFCfgParser.cpp"
		rule_extended();
#line 928 "pf.g"
		
		importer->pushRule();
		
#line 1275 "PFCfgParser.cpp"
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
#line 935 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->newPolicyRule();
		importer->action = "block";
		*dbg << LT(1)->getLine() << ":" << " block   ";
		
#line 1297 "PFCfgParser.cpp"
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
		case IN_WORD:
		case OUT_WORD:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
#line 944 "pf.g"
		
		importer->pushRule();
		
#line 1357 "PFCfgParser.cpp"
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
		case 50:
		case 51:
		case 52:
		case LITERAL_frag:
		case LITERAL_interval:
		case 55:
		case 56:
		case 57:
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
		
#line 1427 "PFCfgParser.cpp"
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
#line 1475 "PFCfgParser.cpp"
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
		
#line 1530 "PFCfgParser.cpp"
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
#line 1563 "PFCfgParser.cpp"
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
#line 1596 "PFCfgParser.cpp"
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
		
#line 1617 "PFCfgParser.cpp"
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
		
#line 1638 "PFCfgParser.cpp"
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
		
#line 1659 "PFCfgParser.cpp"
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
		skip_def();
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
#line 358 "pf.g"
		
		importer->set_debug = LT(0)->getText();
		
#line 1691 "PFCfgParser.cpp"
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
#line 366 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'set reassemble' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1712 "PFCfgParser.cpp"
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
#line 378 "pf.g"
		
		importer->clear();
		importer->setCurrentLineNumber(LT(0)->getLine());
		importer->addMessageToLog(
		QString("Error: import of 'set hostid' commands is not supported."));
		consumeUntil(NEWLINE);
		
#line 1733 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::timeout_def() {
	Tracer traceInOut(this, "timeout_def");
#line 396 "pf.g"
	std::string timeout_name, timeout_value;
#line 1745 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
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
		case 50:
		{
			match(50);
			break;
		}
		case 51:
		{
			match(51);
			break;
		}
		case 52:
		{
			match(52);
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
		case 55:
		{
			match(55);
			break;
		}
		case 56:
		{
			match(56);
			break;
		}
		case 57:
		{
			match(57);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 437 "pf.g"
		
		timeout_name = LT(0)->getText();
		
#line 1855 "PFCfgParser.cpp"
		match(INT_CONST);
#line 441 "pf.g"
		
		timeout_value = LT(0)->getText();
		importer->timeouts.push_back(
		std::pair<std::string, std::string>(timeout_name, timeout_value));
		
#line 1863 "PFCfgParser.cpp"
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
				case 50:
				case 51:
				case 52:
				case LITERAL_frag:
				case LITERAL_interval:
				case 55:
				case 56:
				case 57:
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

void PFCfgParser::limit_def() {
	Tracer traceInOut(this, "limit_def");
#line 459 "pf.g"
	std::string limit_name, limit_value;
#line 1936 "PFCfgParser.cpp"
	
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
#line 472 "pf.g"
		
		limit_name = LT(0)->getText();
		
#line 1976 "PFCfgParser.cpp"
		match(INT_CONST);
#line 476 "pf.g"
		
		limit_value = LT(0)->getText();
		importer->limits.push_back(
		std::pair<std::string, std::string>(limit_name, limit_value));
		
#line 1984 "PFCfgParser.cpp"
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
				goto _loop47;
			}
			
		}
		_loop47:;
		} // ( ... )*
		match(CLOSING_BRACE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::skip_def() {
	Tracer traceInOut(this, "skip_def");
	
	try {      // for error handling
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
#line 338 "pf.g"
			importer->set_skip_on.push_back(LT(0)->getText());
#line 2049 "PFCfgParser.cpp"
			break;
		}
		case OPENING_BRACE:
		{
			skip_list();
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
		recover(ex,_tokenSet_1);
	}
}

void PFCfgParser::skip_list() {
	Tracer traceInOut(this, "skip_list");
	
	try {      // for error handling
		match(OPENING_BRACE);
		match(WORD);
#line 347 "pf.g"
		importer->set_skip_on.push_back(LT(0)->getText());
#line 2077 "PFCfgParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == WORD || LA(1) == COMMA)) {
				{ // ( ... )*
				for (;;) {
					if ((LA(1) == COMMA)) {
						match(COMMA);
					}
					else {
						goto _loop31;
					}
					
				}
				_loop31:;
				} // ( ... )*
				match(WORD);
#line 350 "pf.g"
				importer->set_skip_on.push_back(LT(0)->getText());
#line 2096 "PFCfgParser.cpp"
			}
			else {
				goto _loop32;
			}
			
		}
		_loop32:;
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
		case IN_WORD:
		case OUT_WORD:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
#line 581 "pf.g"
	AddressSpec as;
#line 2488 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 582 "pf.g"
			as.neg = true;
#line 2498 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case INT_CONST:
		case SELF:
		case IPV4:
		case NUMBER:
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
#line 585 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = LT(0)->getText();
			
#line 2526 "PFCfgParser.cpp"
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
#line 594 "pf.g"
					
					as.at = AddressSpec::INTERFACE_NETWORK;
					
#line 2541 "PFCfgParser.cpp"
					break;
				}
				case BROADCAST:
				{
					match(BROADCAST);
#line 599 "pf.g"
					
					as.at = AddressSpec::INTERFACE_BROADCAST;
					
#line 2551 "PFCfgParser.cpp"
					break;
				}
				case PEER:
				{
					match(PEER);
#line 604 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:peer' is not supported."));
					
#line 2562 "PFCfgParser.cpp"
					break;
				}
				case INT_CONST:
				{
					match(INT_CONST);
#line 610 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:0' is not supported."));
					
#line 2573 "PFCfgParser.cpp"
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
			case INT_CONST:
			case EXLAMATION:
			case SELF:
			case IPV4:
			case NUMBER:
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
#line 618 "pf.g"
			
			as.at = AddressSpec::SPECIAL_ADDRESS;
			as.address = "self";
			
#line 2611 "PFCfgParser.cpp"
			break;
		}
		case INT_CONST:
		case IPV4:
		case NUMBER:
		{
			{
			switch ( LA(1)) {
			case IPV4:
			{
				match(IPV4);
				break;
			}
			case NUMBER:
			{
				match(NUMBER);
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
#line 625 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 2646 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 631 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 2656 "PFCfgParser.cpp"
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
#line 635 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 2679 "PFCfgParser.cpp"
				break;
			}
			case WORD:
			case COMMA:
			case CLOSING_BRACE:
			case INT_CONST:
			case EXLAMATION:
			case SELF:
			case IPV4:
			case NUMBER:
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
#line 640 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 2711 "PFCfgParser.cpp"
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
#line 1010 "pf.g"
		
		importer->logging = true;
		
#line 2739 "PFCfgParser.cpp"
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
#line 1062 "pf.g"
			
			importer->address_family = LT(0)->getText();
			
#line 2795 "PFCfgParser.cpp"
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
#line 1105 "pf.g"
			
			importer->src_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			importer->dst_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			
#line 2838 "PFCfgParser.cpp"
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
		case 142:
		case 143:
		case 144:
		case 145:
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
			case 142:
			case 143:
			case 144:
			case 145:
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
			case 142:
			case 143:
			case 144:
			case 145:
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
#line 1610 "pf.g"
			importer->tagged_neg = true;
#line 2968 "PFCfgParser.cpp"
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
#line 1612 "pf.g"
		
		importer->tagged = LT(0)->getText();
		
#line 2987 "PFCfgParser.cpp"
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
#line 1619 "pf.g"
		
		importer->tag = LT(0)->getText();
		
#line 3005 "PFCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_19);
	}
}

void PFCfgParser::redirhost() {
	Tracer traceInOut(this, "redirhost");
#line 776 "pf.g"
	AddressSpec as;
#line 3017 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IPV4:
		{
			match(IPV4);
#line 779 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 3030 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 785 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 3040 "PFCfgParser.cpp"
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
#line 789 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 3063 "PFCfgParser.cpp"
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
#line 796 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = LT(0)->getText();
			
#line 3099 "PFCfgParser.cpp"
			match(CLOSING_PAREN);
			break;
		}
		case WORD:
		{
			match(WORD);
#line 804 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = LT(0)->getText();
			
#line 3112 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 810 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 3125 "PFCfgParser.cpp"
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
				goto _loop100;
			}
			
		}
		_loop100:;
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
#line 835 "pf.g"
	PortSpec ps;
#line 3182 "PFCfgParser.cpp"
	
	try {      // for error handling
		match(PORT);
		{
		switch ( LA(1)) {
		case WORD:
		case INT_CONST:
		{
			port_def();
#line 839 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = ps.port1;
			ps.port_op = "=";
			
#line 3198 "PFCfgParser.cpp"
			break;
		}
		case IPV6:
		{
			match(IPV6);
#line 848 "pf.g"
			
			ps.setFromPortRange(LT(0)->getText());
			
#line 3208 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case STAR:
			{
				match(STAR);
#line 852 "pf.g"
				ps.port2 = "65535";
#line 3216 "PFCfgParser.cpp"
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
#line 855 "pf.g"
		
		importer->tmp_port_group.push_back(ps);
		
#line 3246 "PFCfgParser.cpp"
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
#line 869 "pf.g"
			importer->nat_rule_opt_1 = "bitmask";
#line 3265 "PFCfgParser.cpp"
			break;
		}
		case RANDOM:
		{
			match(RANDOM);
#line 871 "pf.g"
			importer->nat_rule_opt_1 = "random";
#line 3273 "PFCfgParser.cpp"
			break;
		}
		case SOURCE_HASH:
		{
			match(SOURCE_HASH);
#line 873 "pf.g"
			importer->nat_rule_opt_1 = "source-hash";
#line 3281 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case HEX_KEY:
			{
				match(HEX_KEY);
#line 876 "pf.g"
				
				importer->error_tracker->registerError(
				QString("import of 'nat' commands with 'source-hash hex-key' "
				"option is not supported"));
				
#line 3293 "PFCfgParser.cpp"
				break;
			}
			case STRING_KEY:
			{
				match(STRING_KEY);
#line 883 "pf.g"
				
				importer->error_tracker->registerError(
				QString("import of 'nat' commands with 'source-hash string-key' "
				"option is not supported"));
				
#line 3305 "PFCfgParser.cpp"
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
#line 890 "pf.g"
			importer->nat_rule_opt_1 = "round-robin";
#line 3327 "PFCfgParser.cpp"
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
#line 1758 "pf.g"
		
		importer->tmp_port_def = LT(0)->getText();
		
#line 3387 "PFCfgParser.cpp"
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
#line 952 "pf.g"
			importer->block_action_params.push_back("drop");
#line 3406 "PFCfgParser.cpp"
			break;
		}
		case RETURN:
		{
			match(RETURN);
#line 954 "pf.g"
			importer->block_action_params.push_back("return");
#line 3414 "PFCfgParser.cpp"
			break;
		}
		case RETURN_RST:
		{
			match(RETURN_RST);
#line 956 "pf.g"
			importer->block_action_params.push_back("return-rst");
#line 3422 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case TTL:
			{
				match(TTL);
				match(INT_CONST);
#line 959 "pf.g"
				
				importer->error_tracker->registerError(
				QString("Import of \"block return-rst ttl number\" is not supported. "));
				
#line 3434 "PFCfgParser.cpp"
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
			case IN_WORD:
			case OUT_WORD:
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
			case 142:
			case 143:
			case 144:
			case 145:
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
#line 965 "pf.g"
			importer->block_action_params.push_back("return-icmp");
#line 3487 "PFCfgParser.cpp"
			{
			if ((LA(1) == OPENING_PAREN) && (_tokenSet_26.member(LA(2)))) {
				match(OPENING_PAREN);
				{
				switch ( LA(1)) {
				case 173:
				case 174:
				case 175:
				case 176:
				case LITERAL_needfrag:
				case LITERAL_srcfail:
				case 179:
				case 180:
				case LITERAL_isolate:
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
				case 194:
				case LITERAL_transit:
				case LITERAL_reassemb:
				case LITERAL_badhead:
				case LITERAL_optmiss:
				case LITERAL_badlen:
				case 200:
				case 201:
				case 202:
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
#line 969 "pf.g"
				importer->block_action_params.push_back(LT(0)->getText());
#line 3540 "PFCfgParser.cpp"
				{
				switch ( LA(1)) {
				case COMMA:
				{
					match(COMMA);
					{
					switch ( LA(1)) {
					case 173:
					case 174:
					case 175:
					case 176:
					case LITERAL_needfrag:
					case LITERAL_srcfail:
					case 179:
					case 180:
					case LITERAL_isolate:
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
					case 194:
					case LITERAL_transit:
					case LITERAL_reassemb:
					case LITERAL_badhead:
					case LITERAL_optmiss:
					case LITERAL_badlen:
					case 200:
					case 201:
					case 202:
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
#line 973 "pf.g"
					
					importer->error_tracker->registerError(
					QString("Import of \"block return-icmp (icmp_code, icmp6_code)\" is not supported"));
					
#line 3598 "PFCfgParser.cpp"
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
#line 982 "pf.g"
			
			importer->error_tracker->registerError(
			QString("Import of \"block return-icmp6\" is not supported"));
			importer->block_action_params.push_back("return-icmp");
			
#line 3631 "PFCfgParser.cpp"
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
		case 176:
		{
			match(176);
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
		case 179:
		{
			match(179);
			break;
		}
		case 180:
		{
			match(180);
			break;
		}
		case LITERAL_isolate:
		{
			match(LITERAL_isolate);
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
		case 194:
		{
			match(194);
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
		case 202:
		{
			match(202);
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
		case IN_WORD:
		{
			match(IN_WORD);
			break;
		}
		case OUT_WORD:
		{
			match(OUT_WORD);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 1003 "pf.g"
		
		importer->direction = LT(0)->getText();
		
#line 3842 "PFCfgParser.cpp"
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
#line 1032 "pf.g"
		
		importer->quick = true;
		
#line 3859 "PFCfgParser.cpp"
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
				case 142:
				case 143:
				case 144:
				case 145:
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
				goto _loop194;
			}
			
		}
		_loop194:;
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
#line 1019 "pf.g"
				importer->logopts += ",";
#line 3966 "PFCfgParser.cpp"
				logopt();
			}
			else {
				goto _loop135;
			}
			
		}
		_loop135:;
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
#line 1026 "pf.g"
			
			importer->logopts += LT(0)->getText();
			
#line 4007 "PFCfgParser.cpp"
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
#line 1040 "pf.g"
	InterfaceSpec is;
#line 4026 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 1041 "pf.g"
			is.neg = true;
#line 4036 "PFCfgParser.cpp"
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
#line 1043 "pf.g"
		
		is.name = LT(0)->getText();
		importer->iface_group.push_back(is);
		importer->newInterface(is.name);
		
#line 4056 "PFCfgParser.cpp"
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
				goto _loop145;
			}
			
		}
		_loop145:;
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
#line 1082 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 4253 "PFCfgParser.cpp"
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
#line 1088 "pf.g"
		
		importer->proto_list.push_back(LT(0)->getText());
		
#line 4270 "PFCfgParser.cpp"
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
				goto _loop156;
			}
			
		}
		_loop156:;
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
		case 142:
		case 143:
		case 144:
		case 145:
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
		case 142:
		case 143:
		case 144:
		case 145:
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
#line 1128 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::SPECIAL_ADDRESS, false,
			"urpf-failed", ""));
			
#line 4470 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 1134 "pf.g"
		
		importer->src_neg = importer->tmp_neg;
		importer->src_group.splice(importer->src_group.begin(),
		importer->tmp_group);
		
#line 4485 "PFCfgParser.cpp"
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
#line 1671 "pf.g"
			
			PortSpec ps;
			ps.setFromPortRange(LT(0)->getText());
			importer->tmp_port_group.push_back(ps);
			
#line 4524 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 1677 "pf.g"
		
		importer->src_port_group.splice(importer->src_port_group.begin(),
		importer->tmp_port_group);
		
#line 4538 "PFCfgParser.cpp"
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
#line 1143 "pf.g"
		
		importer->dst_neg = importer->tmp_neg;
		importer->dst_group.splice(importer->dst_group.begin(),
		importer->tmp_group);
		
#line 4557 "PFCfgParser.cpp"
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
#line 1693 "pf.g"
			
			PortSpec ps;
			ps.setFromPortRange(LT(0)->getText());
			importer->tmp_port_group.push_back(ps);
			
#line 4596 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 1699 "pf.g"
		
		importer->dst_port_group.splice(importer->dst_port_group.begin(),
		importer->tmp_port_group);
		
#line 4610 "PFCfgParser.cpp"
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
#line 1152 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
			
#line 4631 "PFCfgParser.cpp"
			break;
		}
		case NO_ROUTE:
		{
			match(NO_ROUTE);
#line 1158 "pf.g"
			
			importer->tmp_group.push_back(
			AddressSpec(AddressSpec::SPECIAL_ADDRESS, false, "no-route", ""));
			
#line 4642 "PFCfgParser.cpp"
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
#line 1168 "pf.g"
	AddressSpec as;
#line 4679 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXLAMATION:
		{
			match(EXLAMATION);
#line 1169 "pf.g"
			as.neg = true;
#line 4689 "PFCfgParser.cpp"
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
#line 1172 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = LT(0)->getText();
			
#line 4718 "PFCfgParser.cpp"
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
#line 1181 "pf.g"
					
					as.at = AddressSpec::INTERFACE_NETWORK;
					
#line 4733 "PFCfgParser.cpp"
					break;
				}
				case BROADCAST:
				{
					match(BROADCAST);
#line 1186 "pf.g"
					
					as.at = AddressSpec::INTERFACE_BROADCAST;
					
#line 4743 "PFCfgParser.cpp"
					break;
				}
				case PEER:
				{
					match(PEER);
#line 1191 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:peer' is not supported."));
					
#line 4754 "PFCfgParser.cpp"
					break;
				}
				case INT_CONST:
				{
					match(INT_CONST);
#line 1197 "pf.g"
					
					importer->error_tracker->registerError(
					QString("import of 'interface:0' is not supported."));
					
#line 4765 "PFCfgParser.cpp"
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
			case LITERAL_reassemble:
			case SCRUB:
			case EXLAMATION:
			case NO:
			case MINUS:
			case OPENING_PAREN:
			case PORT:
			case TO:
			case LITERAL_fragment:
			case 142:
			case 143:
			case 144:
			case 145:
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
#line 1205 "pf.g"
			
			as.at = AddressSpec::SPECIAL_ADDRESS;
			as.address = "self";
			
#line 4821 "PFCfgParser.cpp"
			break;
		}
		case IPV6:
		{
			match(IPV6);
#line 1211 "pf.g"
			
			importer->error_tracker->registerError(
			QString("IPv6 import is not supported. "));
			consumeUntil(NEWLINE);
			
#line 4833 "PFCfgParser.cpp"
			break;
		}
		case IPV4:
		{
			match(IPV4);
#line 1218 "pf.g"
			
			as.at = AddressSpec::HOST_ADDRESS;
			as.address = LT(0)->getText();
			
#line 4844 "PFCfgParser.cpp"
			{
			switch ( LA(1)) {
			case SLASH:
			{
				match(SLASH);
#line 1224 "pf.g"
				
				as.at = AddressSpec::NETWORK_ADDRESS;
				
#line 4854 "PFCfgParser.cpp"
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
#line 1228 "pf.g"
				
				as.netmask = LT(0)->getText(); 
				
#line 4877 "PFCfgParser.cpp"
				break;
			}
			case NEWLINE:
			case QUEUE:
			case COMMA:
			case CLOSING_BRACE:
			case LITERAL_reassemble:
			case SCRUB:
			case EXLAMATION:
			case NO:
			case MINUS:
			case OPENING_PAREN:
			case PORT:
			case TO:
			case LITERAL_fragment:
			case 142:
			case 143:
			case 144:
			case 145:
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
#line 1234 "pf.g"
			
			as.at = AddressSpec::TABLE;
			as.address = tn->getText();
			
#line 4928 "PFCfgParser.cpp"
			break;
		}
		case OPENING_PAREN:
		{
			match(OPENING_PAREN);
			in = LT(1);
			match(WORD);
			match(CLOSING_PAREN);
#line 1240 "pf.g"
			
			// interface name or domain/host name
			as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
			as.address = in->getText();
			
#line 4943 "PFCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 1246 "pf.g"
		
		importer->tmp_group.push_back(as);
		
#line 4956 "PFCfgParser.cpp"
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
				goto _loop177;
			}
			
		}
		_loop177:;
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
#line 1268 "pf.g"
		
		importer->route_type = PFImporter::ROUTE_TO;
		
#line 5018 "PFCfgParser.cpp"
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
#line 1275 "pf.g"
		
		importer->route_type = PFImporter::REPLY_TO;
		
#line 5053 "PFCfgParser.cpp"
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
#line 1280 "pf.g"
	RouteSpec rs;
#line 5069 "PFCfgParser.cpp"
	
	try {      // for error handling
		match(OPENING_PAREN);
		match(WORD);
#line 1282 "pf.g"
		rs.iface = LT(0)->getText();
#line 5076 "PFCfgParser.cpp"
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
#line 1284 "pf.g"
		
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
		
#line 5148 "PFCfgParser.cpp"
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
				goto _loop190;
			}
			
		}
		_loop190:;
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
		case 142:
		case 143:
		case 144:
		case 145:
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
#line 1421 "pf.g"
			
			importer->flags_check = "none";
			importer->flags_mask = "none";
			
#line 5294 "PFCfgParser.cpp"
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
			case LITERAL_reassemble:
			case SCRUB:
			case EXLAMATION:
			case NO:
			case OPENING_PAREN:
			case LITERAL_fragment:
			case 142:
			case 143:
			case 144:
			case 145:
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
#line 1427 "pf.g"
			
			if (check)
			importer->flags_check = check->getText();
			else
			importer->flags_check = "any";
			if (mask)
			importer->flags_mask = mask->getText();
			else
			importer->flags_mask = "all";
			
#line 5369 "PFCfgParser.cpp"
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
		case 168:
		case 169:
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
#line 1602 "pf.g"
		
		importer->error_tracker->registerError(
		QString("ICMP6 import is not supported. "));
		consumeUntil(NEWLINE);
		
#line 5451 "PFCfgParser.cpp"
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
#line 1634 "pf.g"
		
		importer->state_op = LT(0)->getText();
		
#line 5495 "PFCfgParser.cpp"
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
#line 1643 "pf.g"
			importer->queue += LT(0)->getText();
#line 5516 "PFCfgParser.cpp"
			break;
		}
		case OPENING_PAREN:
		{
			match(OPENING_PAREN);
			match(WORD);
#line 1646 "pf.g"
			importer->queue += LT(0)->getText();
#line 5525 "PFCfgParser.cpp"
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
#line 1648 "pf.g"
					importer->queue += ",";
#line 5532 "PFCfgParser.cpp"
					match(WORD);
#line 1649 "pf.g"
					importer->queue += LT(0)->getText();
#line 5536 "PFCfgParser.cpp"
				}
				else {
					goto _loop233;
				}
				
			}
			_loop233:;
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
		case 142:
		case 143:
		case 144:
		case 145:
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
			case 141:
			{
				match(141);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 1373 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple("fragment", LT(0)->getText()));
			importer->scrub_rule = true;
			
#line 5659 "PFCfgParser.cpp"
			break;
		}
		case LITERAL_reassemble:
		{
			match(LITERAL_reassemble);
			match(TCP);
#line 1380 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple("reassemble", "tcp"));
			importer->scrub_rule = true;
			
#line 5672 "PFCfgParser.cpp"
			break;
		}
		case 142:
		{
			match(142);
#line 1387 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple(LT(0)->getText(), ""));
			importer->scrub_rule = true;
			
#line 5684 "PFCfgParser.cpp"
			break;
		}
		case 143:
		{
			match(143);
			match(INT_CONST);
#line 1394 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple("min-ttl", LT(0)->getText()));
			importer->scrub_rule = true;
			
#line 5697 "PFCfgParser.cpp"
			break;
		}
		case 144:
		{
			match(144);
			match(INT_CONST);
#line 1401 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple("max-mss", LT(0)->getText()));
			importer->scrub_rule = true;
			
#line 5710 "PFCfgParser.cpp"
			break;
		}
		case 145:
		{
			match(145);
#line 1408 "pf.g"
			
			importer->scrub_options.push_back(
			str_tuple(LT(0)->getText(), ""));
			importer->scrub_rule = true;
			
#line 5722 "PFCfgParser.cpp"
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
				case 142:
				case 143:
				case 144:
				case 145:
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
				goto _loop202;
			}
			
		}
		_loop202:;
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
#line 1449 "pf.g"
	IcmpSpec is;
#line 5790 "PFCfgParser.cpp"
	
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
		case 168:
		case 169:
		case LITERAL_mobregreq:
		case LITERAL_mobregrep:
		case LITERAL_photuris:
		{
			icmp_type_by_name();
#line 1451 "pf.g"
			is.icmp_type_name = LT(0)->getText();
#line 5824 "PFCfgParser.cpp"
			break;
		}
		case INT_CONST:
		{
			match(INT_CONST);
#line 1453 "pf.g"
			is.icmp_type_int = LT(0)->getText();
#line 5832 "PFCfgParser.cpp"
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
			case 173:
			case 174:
			case 175:
			case 176:
			case LITERAL_needfrag:
			case LITERAL_srcfail:
			case 179:
			case 180:
			case LITERAL_isolate:
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
			case 194:
			case LITERAL_transit:
			case LITERAL_reassemb:
			case LITERAL_badhead:
			case LITERAL_optmiss:
			case LITERAL_badlen:
			case 200:
			case 201:
			case 202:
			{
				icmp_code_by_name();
#line 1459 "pf.g"
				is.icmp_code_name = LT(0)->getText();
#line 5882 "PFCfgParser.cpp"
				break;
			}
			case INT_CONST:
			{
				match(INT_CONST);
#line 1461 "pf.g"
				is.icmp_code_int = LT(0)->getText();
#line 5890 "PFCfgParser.cpp"
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
		case COMMA:
		case CLOSING_BRACE:
		case LITERAL_reassemble:
		case INT_CONST:
		case SCRUB:
		case EXLAMATION:
		case NO:
		case OPENING_PAREN:
		case LITERAL_fragment:
		case 142:
		case 143:
		case 144:
		case 145:
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
		case 168:
		case 169:
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
#line 1464 "pf.g"
		
		importer->icmp_type_code_group.push_back(is);
		
#line 5963 "PFCfgParser.cpp"
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
				case 168:
				case 169:
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
				goto _loop223;
			}
			
		}
		_loop223:;
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
		case 168:
		{
			match(168);
			break;
		}
		case 169:
		{
			match(169);
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
#line 1731 "pf.g"
	PortSpec ps;
#line 6187 "PFCfgParser.cpp"
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EQUAL:
		case LESS_THAN:
		case GREATER_THAN:
		case EXLAMATION:
		{
			unary_port_op();
#line 1733 "pf.g"
			ps.port_op = importer->tmp_port_op;
#line 6200 "PFCfgParser.cpp"
			port_def();
#line 1735 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = importer->tmp_port_def;
			
#line 6207 "PFCfgParser.cpp"
			break;
		}
		case WORD:
		case INT_CONST:
		{
			port_def();
#line 1741 "pf.g"
			
			ps.port1 = importer->tmp_port_def;
			ps.port2 = ps.port1;
			ps.port_op = "=";
			
#line 6220 "PFCfgParser.cpp"
			{
			if ((LA(1) == LESS_THAN || LA(1) == GREATER_THAN || LA(1) == COLON) && (_tokenSet_49.member(LA(2)))) {
				binary_port_op();
#line 1747 "pf.g"
				ps.port_op = importer->tmp_port_op;
#line 6226 "PFCfgParser.cpp"
				port_def();
#line 1748 "pf.g"
				ps.port2 = LT(0)->getText();
#line 6230 "PFCfgParser.cpp"
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
#line 1751 "pf.g"
		
		importer->tmp_port_group.push_back(ps);
		
#line 6251 "PFCfgParser.cpp"
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
				goto _loop251;
			}
			
		}
		_loop251:;
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
#line 1707 "pf.g"
			importer->tmp_port_op = "=";
#line 6318 "PFCfgParser.cpp"
			break;
		}
		case EXLAMATION:
		{
			match(EXLAMATION);
			match(EQUAL);
#line 1709 "pf.g"
			importer->tmp_port_op = "!=";
#line 6327 "PFCfgParser.cpp"
			break;
		}
		default:
			if ((LA(1) == LESS_THAN) && (LA(2) == WORD || LA(2) == INT_CONST)) {
				match(LESS_THAN);
#line 1711 "pf.g"
				importer->tmp_port_op = "<";
#line 6335 "PFCfgParser.cpp"
			}
			else if ((LA(1) == LESS_THAN) && (LA(2) == EQUAL)) {
				match(LESS_THAN);
				match(EQUAL);
#line 1713 "pf.g"
				importer->tmp_port_op = "<=";
#line 6342 "PFCfgParser.cpp"
			}
			else if ((LA(1) == GREATER_THAN) && (LA(2) == WORD || LA(2) == INT_CONST)) {
				match(GREATER_THAN);
#line 1715 "pf.g"
				importer->tmp_port_op = ">";
#line 6348 "PFCfgParser.cpp"
			}
			else if ((LA(1) == GREATER_THAN) && (LA(2) == EQUAL)) {
				match(GREATER_THAN);
				match(EQUAL);
#line 1717 "pf.g"
				importer->tmp_port_op = ">=";
#line 6355 "PFCfgParser.cpp"
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
#line 1723 "pf.g"
			importer->tmp_port_op = "<>";
#line 6381 "PFCfgParser.cpp"
			break;
		}
		case GREATER_THAN:
		{
			match(GREATER_THAN);
			match(LESS_THAN);
#line 1725 "pf.g"
			importer->tmp_port_op = "><";
#line 6390 "PFCfgParser.cpp"
			break;
		}
		case COLON:
		{
			match(COLON);
#line 1727 "pf.g"
			importer->tmp_port_op = ":";
#line 6398 "PFCfgParser.cpp"
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
	"OPENING_BRACE",
	"COMMA",
	"CLOSING_BRACE",
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
	"\"counters\"",
	"\"file\"",
	"STRING",
	"EXLAMATION",
	"COLON",
	"\"network\"",
	"\"broadcast\"",
	"\"peer\"",
	"\"self\"",
	"IPV4",
	"NUMBER",
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
const unsigned long PFCfgParser::_tokenSet_1_data_[] = { 3954UL, 0UL, 20447239UL, 96UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "scrub" 
// "match" "table" "no" "nat" "pass" "rdr" "binat" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_1(_tokenSet_1_data_,10);
const unsigned long PFCfgParser::_tokenSet_2_data_[] = { 64UL, 67108868UL, 230400UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA INT_CONST EXLAMATION "self" IPV4 NUMBER 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_2(_tokenSet_2_data_,10);
const unsigned long PFCfgParser::_tokenSet_3_data_[] = { 3954UL, 67108748UL, 20447239UL, 96UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" COMMA 
// CLOSING_BRACE "tcp.first" "tcp.opening" "tcp.established" "tcp.closing" 
// "tcp.finwait" "tcp.closed" "udp.first" "udp.single" "udp.multiple" "icmp.first" 
// "icmp.error" "other.first" "other.single" "other.multiple" "frag" "interval" 
// "src.track" "adaptive.start" "adaptive.end" "scrub" "match" "table" 
// "no" "nat" "pass" "rdr" "binat" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_3(_tokenSet_3_data_,10);
const unsigned long PFCfgParser::_tokenSet_4_data_[] = { 0UL, 67108740UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// COMMA "tcp.first" "tcp.opening" "tcp.established" "tcp.closing" "tcp.finwait" 
// "tcp.closed" "udp.first" "udp.single" "udp.multiple" "icmp.first" "icmp.error" 
// "other.first" "other.single" "other.multiple" "frag" "interval" "src.track" 
// "adaptive.start" "adaptive.end" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_4(_tokenSet_4_data_,10);
const unsigned long PFCfgParser::_tokenSet_5_data_[] = { 3954UL, 4160749580UL, 20447239UL, 96UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" COMMA 
// CLOSING_BRACE "frags" "states" "src-nodes" "tables" "tables-entries" 
// "scrub" "match" "table" "no" "nat" "pass" "rdr" "binat" "block" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_5(_tokenSet_5_data_,10);
const unsigned long PFCfgParser::_tokenSet_6_data_[] = { 0UL, 4160749572UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// COMMA "frags" "states" "src-nodes" "tables" "tables-entries" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_6(_tokenSet_6_data_,10);
const unsigned long PFCfgParser::_tokenSet_7_data_[] = { 1040UL, 32UL, 34079745UL, 81920UL, 1034272UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN "all" 
// "to" "from" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_7(_tokenSet_7_data_,16);
const unsigned long PFCfgParser::_tokenSet_8_data_[] = { 2147487602UL, 67108902UL, 322799119UL, 16777312UL, 4293917120UL, 8191UL, 522240UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "skip" 
// OPENING_BRACE COMMA "reassemble" INT_CONST "scrub" "match" "table" LESS_THAN 
// STRING EXLAMATION "self" IPV4 SLASH "no" "nat" "pass" "rdr" OPENING_PAREN 
// IPV6 "binat" "block" "tcp" "urpf-failed" "any" "no-route" "fragment" 
// "crop" "drop-ovl" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "echorep" "unreach" "squench" "redir" "althost" "echoreq" "routeradv" 
// "routersol" "timex" "paramprob" "timereq" "timerep" "inforeq" "inforep" 
// "maskreq" "maskrep" "trace" "dataconv" "mobredir" "ipv6-where" "ipv6-here" 
// "mobregreq" "mobregrep" "photuris" "icmp6-type" "tagged" "tag" "keep" 
// "modulate" "synproxy" "state" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_8(_tokenSet_8_data_,16);
const unsigned long PFCfgParser::_tokenSet_9_data_[] = { 1040UL, 32UL, 34079745UL, 0UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_9(_tokenSet_9_data_,16);
const unsigned long PFCfgParser::_tokenSet_10_data_[] = { 2147487602UL, 67108902UL, 54265351UL, 16777312UL, 4293916800UL, 8191UL, 522240UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "skip" 
// OPENING_BRACE COMMA "reassemble" INT_CONST "scrub" "match" "table" STRING 
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
const unsigned long PFCfgParser::_tokenSet_12_data_[] = { 64UL, 67108876UL, 230400UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA CLOSING_BRACE INT_CONST EXLAMATION "self" IPV4 NUMBER 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_12(_tokenSet_12_data_,10);
const unsigned long PFCfgParser::_tokenSet_13_data_[] = { 1040UL, 33UL, 38274049UL, 2048000UL, 1035808UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "on" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "all" "to" "quick" "inet" "inet6" "proto" "from" "route-to" "reply-to" 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_13(_tokenSet_13_data_,16);
const unsigned long PFCfgParser::_tokenSet_14_data_[] = { 2147487602UL, 67108903UL, 326993439UL, 4294787168UL, 4293918719UL, 8191UL, 522240UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "skip" 
// "on" OPENING_BRACE COMMA "reassemble" INT_CONST "scrub" "match" "table" 
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
const unsigned long PFCfgParser::_tokenSet_15_data_[] = { 1040UL, 32UL, 38274049UL, 1916928UL, 1035808UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "all" "to" "inet" "inet6" "proto" "from" "route-to" "reply-to" "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_15(_tokenSet_15_data_,16);
const unsigned long PFCfgParser::_tokenSet_16_data_[] = { 1040UL, 32UL, 38274049UL, 1130496UL, 1034272UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "all" "to" "proto" "from" "fragment" "no-df" "min-ttl" "max-mss" "random-id" 
// "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_16(_tokenSet_16_data_,16);
const unsigned long PFCfgParser::_tokenSet_17_data_[] = { 1040UL, 32UL, 38274049UL, 81920UL, 1034272UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "all" "to" "from" "fragment" "no-df" "min-ttl" "max-mss" "random-id" 
// "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_17(_tokenSet_17_data_,16);
const unsigned long PFCfgParser::_tokenSet_18_data_[] = { 1040UL, 32UL, 38274049UL, 0UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_18(_tokenSet_18_data_,16);
const unsigned long PFCfgParser::_tokenSet_19_data_[] = { 1040UL, 36UL, 38274049UL, 0UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_19(_tokenSet_19_data_,16);
const unsigned long PFCfgParser::_tokenSet_20_data_[] = { 80UL, 12UL, 3397451776UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD COMMA CLOSING_BRACE IPV4 "static-port" OPENING_PAREN "port" 
// "bitmask" "random" "source-hash" "round-robin" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_20(_tokenSet_20_data_,10);
const unsigned long PFCfgParser::_tokenSet_21_data_[] = { 64UL, 4UL, 33619968UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD COMMA IPV4 OPENING_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_21(_tokenSet_21_data_,10);
const unsigned long PFCfgParser::_tokenSet_22_data_[] = { 16UL, 0UL, 3363831808UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "static-port" "port" "bitmask" "random" "source-hash" "round-robin" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_22(_tokenSet_22_data_,10);
const unsigned long PFCfgParser::_tokenSet_23_data_[] = { 16UL, 0UL, 3229614080UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "static-port" "bitmask" "random" "source-hash" "round-robin" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_23(_tokenSet_23_data_,10);
const unsigned long PFCfgParser::_tokenSet_24_data_[] = { 16UL, 0UL, 8388608UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "static-port" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_24(_tokenSet_24_data_,10);
const unsigned long PFCfgParser::_tokenSet_25_data_[] = { 1232UL, 67108908UL, 3267890201UL, 65545UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD EQUAL "queue" COMMA CLOSING_BRACE "reassemble" INT_CONST 
// "scrub" LESS_THAN GREATER_THAN EXLAMATION COLON "no" MINUS "static-port" 
// OPENING_PAREN "bitmask" "random" "source-hash" "round-robin" "to" "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_25(_tokenSet_25_data_,16);
const unsigned long PFCfgParser::_tokenSet_26_data_[] = { 0UL, 67108864UL, 0UL, 0UL, 0UL, 4294959104UL, 2047UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// INT_CONST "net-unr" "host-unr" "proto-unr" "port-unr" "needfrag" "srcfail" 
// "net-unk" "host-unk" "isolate" "net-prohib" "host-prohib" "net-tos" 
// "host-tos" "filter-prohib" "host-preced" "cutoff-preced" "redir-net" 
// "redir-host" "redir-tos-net" "redir-tos-host" "normal-adv" "common-adv" 
// "transit" "reassemb" "badhead" "optmiss" "badlen" "unknown-ind" "auth-fail" 
// "decrypt-fail" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_26(_tokenSet_26_data_,16);
const unsigned long PFCfgParser::_tokenSet_27_data_[] = { 1040UL, 33UL, 34079745UL, 2062336UL, 1035808UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "on" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN 
// "in" "out" "log" "all" "to" "quick" "inet" "inet6" "proto" "from" "route-to" 
// "reply-to" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_27(_tokenSet_27_data_,16);
const unsigned long PFCfgParser::_tokenSet_28_data_[] = { 2147487602UL, 67108903UL, 322799119UL, 4294926432UL, 4293918719UL, 8191UL, 522240UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD "antispoof" "altq" "queue" "set" "skip" 
// "on" OPENING_BRACE COMMA "reassemble" INT_CONST "scrub" "match" "table" 
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
const unsigned long PFCfgParser::_tokenSet_29_data_[] = { 2147484688UL, 67108908UL, 101188609UL, 0UL, 4293904384UL, 8191UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "skip" COMMA CLOSING_BRACE "reassemble" INT_CONST "scrub" 
// EXLAMATION "no" OPENING_PAREN CLOSING_PAREN "fragment" "no-df" "min-ttl" 
// "max-mss" "random-id" "flags" "icmp-type" "echorep" "unreach" "squench" 
// "redir" "althost" "echoreq" "routeradv" "routersol" "timex" "paramprob" 
// "timereq" "timerep" "inforeq" "inforep" "maskreq" "maskrep" "trace" 
// "dataconv" "mobredir" "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" 
// "photuris" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_29(_tokenSet_29_data_,16);
const unsigned long PFCfgParser::_tokenSet_30_data_[] = { 1040UL, 33UL, 34079745UL, 2056192UL, 1035808UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "on" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN 
// "log" "all" "to" "quick" "inet" "inet6" "proto" "from" "route-to" "reply-to" 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_30(_tokenSet_30_data_,16);
const unsigned long PFCfgParser::_tokenSet_31_data_[] = { 1040UL, 33UL, 34079745UL, 1916928UL, 1035808UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "on" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN 
// "all" "to" "inet" "inet6" "proto" "from" "route-to" "reply-to" "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_31(_tokenSet_31_data_,16);
const unsigned long PFCfgParser::_tokenSet_32_data_[] = { 1040UL, 32UL, 34079745UL, 1916928UL, 1034272UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN "all" 
// "to" "inet" "inet6" "proto" "from" "fragment" "no-df" "min-ttl" "max-mss" 
// "random-id" "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" 
// "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_32(_tokenSet_32_data_,16);
const unsigned long PFCfgParser::_tokenSet_33_data_[] = { 1024UL, 36UL, 34079745UL, 0UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "queue" COMMA "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_33(_tokenSet_33_data_,16);
const unsigned long PFCfgParser::_tokenSet_34_data_[] = { 0UL, 4UL, 67108864UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// COMMA CLOSING_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_34(_tokenSet_34_data_,10);
const unsigned long PFCfgParser::_tokenSet_35_data_[] = { 1104UL, 44UL, 38274049UL, 1916928UL, 1035808UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD "queue" COMMA CLOSING_BRACE "reassemble" "scrub" EXLAMATION 
// "no" MINUS OPENING_PAREN "all" "to" "inet" "inet6" "proto" "from" "route-to" 
// "reply-to" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_35(_tokenSet_35_data_,16);
const unsigned long PFCfgParser::_tokenSet_36_data_[] = { 1040UL, 67108910UL, 38274049UL, 4292952064UL, 1034303UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" OPENING_BRACE COMMA CLOSING_BRACE "reassemble" INT_CONST 
// "scrub" EXLAMATION "no" MINUS OPENING_PAREN "all" "to" "ip" "icmp" "igmp" 
// "tcp" "udp" "rdp" "rsvp" "gre" "esp" "ah" "eigrp" "ospf" "ipip" "vrrp" 
// "l2tp" "isis" "from" "fragment" "no-df" "min-ttl" "max-mss" "random-id" 
// "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_36(_tokenSet_36_data_,16);
const unsigned long PFCfgParser::_tokenSet_37_data_[] = { 0UL, 67108870UL, 0UL, 4292870144UL, 31UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// OPENING_BRACE COMMA INT_CONST "ip" "icmp" "igmp" "tcp" "udp" "rdp" "rsvp" 
// "gre" "esp" "ah" "eigrp" "ospf" "ipip" "vrrp" "l2tp" "isis" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_37(_tokenSet_37_data_,12);
const unsigned long PFCfgParser::_tokenSet_38_data_[] = { 1040UL, 32UL, 38274049UL, 65536UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "to" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_38(_tokenSet_38_data_,16);
const unsigned long PFCfgParser::_tokenSet_39_data_[] = { 1040UL, 32UL, 172491777UL, 65536UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "port" "to" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_39(_tokenSet_39_data_,16);
const unsigned long PFCfgParser::_tokenSet_40_data_[] = { 1040UL, 32UL, 172491777UL, 0UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "reassemble" "scrub" EXLAMATION "no" MINUS OPENING_PAREN 
// "port" "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_40(_tokenSet_40_data_,16);
const unsigned long PFCfgParser::_tokenSet_41_data_[] = { 1040UL, 44UL, 172491777UL, 65536UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA CLOSING_BRACE "reassemble" "scrub" EXLAMATION 
// "no" MINUS OPENING_PAREN "port" "to" "fragment" "no-df" "min-ttl" "max-mss" 
// "random-id" "flags" "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" 
// "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_41(_tokenSet_41_data_,16);
const unsigned long PFCfgParser::_tokenSet_42_data_[] = { 1040UL, 44UL, 34079745UL, 1916928UL, 1034272UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA CLOSING_BRACE "reassemble" "scrub" EXLAMATION 
// "no" OPENING_PAREN "all" "to" "inet" "inet6" "proto" "from" "fragment" 
// "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" "icmp6-type" 
// "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_42(_tokenSet_42_data_,16);
const unsigned long PFCfgParser::_tokenSet_43_data_[] = { 1040UL, 36UL, 34079745UL, 0UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_43(_tokenSet_43_data_,16);
const unsigned long PFCfgParser::_tokenSet_44_data_[] = { 1040UL, 36UL, 101188609UL, 0UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" COMMA "reassemble" "scrub" EXLAMATION "no" OPENING_PAREN 
// CLOSING_PAREN "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" 
// "icmp-type" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_44(_tokenSet_44_data_,16);
const unsigned long PFCfgParser::_tokenSet_45_data_[] = { 0UL, 36UL, 0UL, 0UL, 247808UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// COMMA "reassemble" "fragment" "no-df" "min-ttl" "max-mss" "random-id" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_45(_tokenSet_45_data_,12);
const unsigned long PFCfgParser::_tokenSet_46_data_[] = { 2147484688UL, 67108908UL, 34079745UL, 0UL, 4293904384UL, 8191UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "skip" COMMA CLOSING_BRACE "reassemble" INT_CONST "scrub" 
// EXLAMATION "no" OPENING_PAREN "fragment" "no-df" "min-ttl" "max-mss" 
// "random-id" "flags" "icmp-type" "echorep" "unreach" "squench" "redir" 
// "althost" "echoreq" "routeradv" "routersol" "timex" "paramprob" "timereq" 
// "timerep" "inforeq" "inforep" "maskreq" "maskrep" "trace" "dataconv" 
// "mobredir" "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" "photuris" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_46(_tokenSet_46_data_,16);
const unsigned long PFCfgParser::_tokenSet_47_data_[] = { 2147483648UL, 67108868UL, 0UL, 0UL, 4292870144UL, 8191UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "skip" COMMA INT_CONST "echorep" "unreach" "squench" "redir" "althost" 
// "echoreq" "routeradv" "routersol" "timex" "paramprob" "timereq" "timerep" 
// "inforeq" "inforep" "maskreq" "maskrep" "trace" "dataconv" "mobredir" 
// "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" "photuris" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_47(_tokenSet_47_data_,12);
const unsigned long PFCfgParser::_tokenSet_48_data_[] = { 2147484688UL, 67108908UL, 34079745UL, 0UL, 4294952960UL, 8191UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "queue" "skip" COMMA CLOSING_BRACE "reassemble" INT_CONST "scrub" 
// EXLAMATION "no" OPENING_PAREN "fragment" "no-df" "min-ttl" "max-mss" 
// "random-id" "flags" "icmp-type" "code" "echorep" "unreach" "squench" 
// "redir" "althost" "echoreq" "routeradv" "routersol" "timex" "paramprob" 
// "timereq" "timerep" "inforeq" "inforep" "maskreq" "maskrep" "trace" 
// "dataconv" "mobredir" "ipv6-where" "ipv6-here" "mobregreq" "mobregrep" 
// "photuris" "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" 
// "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_48(_tokenSet_48_data_,16);
const unsigned long PFCfgParser::_tokenSet_49_data_[] = { 64UL, 67108864UL, 24UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD INT_CONST LESS_THAN GREATER_THAN 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_49(_tokenSet_49_data_,10);
const unsigned long PFCfgParser::_tokenSet_50_data_[] = { 1232UL, 67108908UL, 38274073UL, 65536UL, 1034240UL, 0UL, 391168UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD EQUAL "queue" COMMA CLOSING_BRACE "reassemble" INT_CONST 
// "scrub" LESS_THAN GREATER_THAN EXLAMATION "no" MINUS OPENING_PAREN "to" 
// "fragment" "no-df" "min-ttl" "max-mss" "random-id" "flags" "icmp-type" 
// "icmp6-type" "tagged" "tag" "keep" "modulate" "synproxy" "label" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_50(_tokenSet_50_data_,16);
const unsigned long PFCfgParser::_tokenSet_51_data_[] = { 2147487730UL, 67108910UL, 326995487UL, 16842848UL, 4293917056UL, 8191UL, 522240UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE LINE_COMMENT WORD EQUAL "antispoof" "altq" "queue" "set" 
// "skip" OPENING_BRACE COMMA CLOSING_BRACE "reassemble" INT_CONST "scrub" 
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
const unsigned long PFCfgParser::_tokenSet_52_data_[] = { 192UL, 67108868UL, 1048UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD EQUAL COMMA INT_CONST LESS_THAN GREATER_THAN EXLAMATION 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_52(_tokenSet_52_data_,10);
const unsigned long PFCfgParser::_tokenSet_53_data_[] = { 64UL, 67108864UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgParser::_tokenSet_53(_tokenSet_53_data_,10);



/* $ANTLR 2.7.4: "iosacl.g" -> "IOSCfgParser.cpp"$ */
#line 42 "iosacl.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "IOSCfgParser.cpp"
#include "IOSCfgParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 48 "iosacl.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#include "../gui/IOSImporter.h"

#line 22 "IOSCfgParser.cpp"
#line 1 "iosacl.g"
#line 24 "IOSCfgParser.cpp"
IOSCfgParser::IOSCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

IOSCfgParser::IOSCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
}

IOSCfgParser::IOSCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

IOSCfgParser::IOSCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
}

IOSCfgParser::IOSCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
}

void IOSCfgParser::cfgfile() {
	
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
			case IOSVERSION:
			{
				version();
				break;
			}
			case HOSTNAME:
			{
				hostname();
				break;
			}
			case IP:
			{
				ip_commands();
				break;
			}
			case INTRFACE:
			{
				intrface();
				break;
			}
			case VLAN:
			{
				vlan();
				break;
			}
			case ACCESS_LIST:
			{
				access_list_commands();
				break;
			}
			case EXIT:
			{
				exit();
				break;
			}
			case DESCRIPTION:
			{
				description();
				break;
			}
			case SHUTDOWN:
			{
				shutdown();
				break;
			}
			case WORD:
			{
				unknown_command();
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
			consume();
			consumeUntil(_tokenSet_0);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::comment() {
	
	try {      // for error handling
		match(LINE_COMMENT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_1);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::version() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  v = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(IOSVERSION);
		v = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 126 "iosacl.g"
			
			*dbg << "VERSION " << v->getText() << std::endl;
			
#line 166 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::hostname() {
	
	try {      // for error handling
		match(HOSTNAME);
		{
		switch ( LA(1)) {
		case STRING:
		{
			match(STRING);
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
#line 133 "iosacl.g"
			
			importer->setHostName( LT(0)->getText() );
			*dbg << "HOSTNAME "
			<< "LT0=" << LT(0)->getText()
			<< std::endl;
			
#line 210 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::ip_commands() {
	
	try {      // for error handling
		match(IP);
		{
		switch ( LA(1)) {
		case ACCESS_LIST:
		{
			ip_access_list_ext();
			break;
		}
		case ACCESS_GROUP:
		case ADDRESS:
		{
			interface_known_ip_commands();
			break;
		}
		case WORD:
		{
			unknown_command();
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
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::intrface() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(INTRFACE);
		in = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 443 "iosacl.g"
			
			importer->newInterface( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE: " << in->getText() << std::endl;
			
#line 278 "IOSCfgParser.cpp"
		}
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::vlan() {
	
	try {      // for error handling
		match(VLAN);
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
#line 434 "iosacl.g"
			
			importer->clearCurrentInterface();
			consumeUntil(NEWLINE);
			
#line 321 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::access_list_commands() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  acl_num = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_LIST);
		acl_num = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 148 "iosacl.g"
			
			importer->newUnidirRuleSet( std::string("acl_") + acl_num->getText() );
			*dbg << acl_num->getLine() << ":"
			<< " ACL #" << acl_num->getText() << " ";
			
#line 349 "IOSCfgParser.cpp"
		}
		{
		if ((LA(1) == PERMIT) && (LA(2) == IPV4 || LA(2) == ANY)) {
			permit_std();
		}
		else if ((LA(1) == DENY) && (LA(2) == IPV4 || LA(2) == ANY)) {
			deny_std();
		}
		else if ((LA(1) == PERMIT) && (_tokenSet_3.member(LA(2)))) {
			permit_ext();
		}
		else if ((LA(1) == DENY) && (_tokenSet_3.member(LA(2)))) {
			deny_ext();
		}
		else if ((LA(1) == REMARK)) {
			remark();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::exit() {
	
	try {      // for error handling
		match(EXIT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::description() {
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 454 "iosacl.g"
			
			*dbg << LT(1)->getLine() << ":";
			std::string descr;
			while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
			{
			descr += LT(1)->getText() + " ";
			consume();
			}
			importer->addInterfaceComment( descr );
			*dbg << " INTERFACE DESCRIPTION " << descr << std::endl;
			//consumeUntil(NEWLINE);
			
#line 418 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::shutdown() {
	
	try {      // for error handling
		match(SHUTDOWN);
		if ( inputState->guessing==0 ) {
#line 487 "iosacl.g"
			
			*dbg<< LT(1)->getLine() << ":"
			<< " INTERFACE SHUTDOWN " << std::endl;
			
#line 442 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::unknown_command() {
	
	try {      // for error handling
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 119 "iosacl.g"
			
			consumeUntil(NEWLINE);
			
#line 465 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::ip_access_list_ext() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_LIST);
		match(EXTENDED);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 169 "iosacl.g"
			
			importer->newUnidirRuleSet( name->getText() );
			*dbg << name->getLine() << ":"
			<< " ACL ext " << name->getText() << std::endl;
			
#line 494 "IOSCfgParser.cpp"
		}
		match(NEWLINE);
		{ // ( ... )+
		int _cnt14=0;
		for (;;) {
			switch ( LA(1)) {
			case PERMIT:
			{
				permit_ext();
				break;
			}
			case DENY:
			{
				deny_ext();
				break;
			}
			case REMARK:
			{
				remark();
				break;
			}
			default:
				if ((LA(1) == LINE_COMMENT) && (_tokenSet_1.member(LA(2)))) {
					comment();
				}
				else if ((LA(1) == NEWLINE) && (_tokenSet_1.member(LA(2)))) {
					match(NEWLINE);
				}
			else {
				if ( _cnt14>=1 ) { goto _loop14; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt14++;
		}
		_loop14:;
		}  // ( ... )+
		if ( inputState->guessing==0 ) {
#line 186 "iosacl.g"
			
			*dbg << LT(0)->getLine() << ":"
			<< " ACL end" << std::endl << std::endl;
			
#line 537 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::interface_known_ip_commands() {
	
	try {      // for error handling
		{
		if ((LA(1) == ACCESS_GROUP) && (LA(2) == WORD)) {
			access_group_by_name();
		}
		else if ((LA(1) == ACCESS_GROUP) && (LA(2) == INT_CONST)) {
			access_group_by_number();
		}
		else if ((LA(1) == ADDRESS)) {
			intf_address();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::permit_std() {
	
	try {      // for error handling
		match(PERMIT);
		if ( inputState->guessing==0 ) {
#line 219 "iosacl.g"
			
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 593 "IOSCfgParser.cpp"
		}
		rule_std();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 225 "iosacl.g"
			
			importer->pushRule();
			
#line 602 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::deny_std() {
	
	try {      // for error handling
		match(DENY);
		if ( inputState->guessing==0 ) {
#line 231 "iosacl.g"
			
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 627 "IOSCfgParser.cpp"
		}
		rule_std();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 237 "iosacl.g"
			
			importer->pushRule();
			
#line 636 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::permit_ext() {
	
	try {      // for error handling
		match(PERMIT);
		if ( inputState->guessing==0 ) {
#line 194 "iosacl.g"
			
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 661 "IOSCfgParser.cpp"
		}
		rule_ext();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 200 "iosacl.g"
			
			importer->pushRule();
			
#line 670 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_1);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::deny_ext() {
	
	try {      // for error handling
		match(DENY);
		if ( inputState->guessing==0 ) {
#line 206 "iosacl.g"
			
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 695 "IOSCfgParser.cpp"
		}
		rule_ext();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 212 "iosacl.g"
			
			importer->pushRule();
			
#line 704 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_1);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::remark() {
	
	try {      // for error handling
		match(REMARK);
		if ( inputState->guessing==0 ) {
#line 472 "iosacl.g"
			
			*dbg << LT(1)->getLine() << ":";
			std::string rem;
			while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
			{
			rem += LT(1)->getText() + " ";
			consume();
			}
			importer->addRuleComment( rem );
			*dbg << " REMARK " << rem << std::endl;
			//consumeUntil(NEWLINE);
			
#line 736 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_1);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::rule_ext() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IP:
		case WORD:
		{
			ip_protocols();
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 247 "iosacl.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 763 "IOSCfgParser.cpp"
			}
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 248 "iosacl.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 769 "IOSCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case TIME_RANGE:
			{
				time_range();
				break;
			}
			case NEWLINE:
			case LOG:
			case LOG_INPUT:
			case FRAGMENTS:
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
			case FRAGMENTS:
			{
				fragments();
				break;
			}
			case NEWLINE:
			case LOG:
			case LOG_INPUT:
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
			case LOG_INPUT:
			{
				log();
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
		case ICMP:
		{
			match(ICMP);
			if ( inputState->guessing==0 ) {
#line 254 "iosacl.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 839 "IOSCfgParser.cpp"
			}
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 258 "iosacl.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 845 "IOSCfgParser.cpp"
			}
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 259 "iosacl.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 851 "IOSCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case WORD:
			case INT_CONST:
			{
				icmp_spec();
				break;
			}
			case NEWLINE:
			case LOG:
			case LOG_INPUT:
			case FRAGMENTS:
			case TIME_RANGE:
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
			case TIME_RANGE:
			{
				time_range();
				break;
			}
			case NEWLINE:
			case LOG:
			case LOG_INPUT:
			case FRAGMENTS:
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
			case FRAGMENTS:
			{
				fragments();
				break;
			}
			case NEWLINE:
			case LOG:
			case LOG_INPUT:
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
			case LOG_INPUT:
			{
				log();
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
		case TCP:
		case UDP:
		{
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
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 266 "iosacl.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 961 "IOSCfgParser.cpp"
			}
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 270 "iosacl.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 967 "IOSCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case P_EQ:
			case P_GT:
			case P_LT:
			case P_NEQ:
			case P_RANGE:
			{
				xoperator();
				if ( inputState->guessing==0 ) {
#line 271 "iosacl.g"
					importer->SaveTmpPortToSrc();
#line 981 "IOSCfgParser.cpp"
				}
				break;
			}
			case HOST:
			case IPV4:
			case ANY:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 272 "iosacl.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 1001 "IOSCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case P_EQ:
			case P_GT:
			case P_LT:
			case P_NEQ:
			case P_RANGE:
			{
				xoperator();
				if ( inputState->guessing==0 ) {
#line 273 "iosacl.g"
					importer->SaveTmpPortToDst();
#line 1015 "IOSCfgParser.cpp"
				}
				break;
			}
			case NEWLINE:
			case LOG:
			case LOG_INPUT:
			case ESTABLISHED:
			case FRAGMENTS:
			case TIME_RANGE:
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
			case ESTABLISHED:
			{
				established();
				break;
			}
			case NEWLINE:
			case LOG:
			case LOG_INPUT:
			case FRAGMENTS:
			case TIME_RANGE:
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
			case TIME_RANGE:
			{
				time_range();
				break;
			}
			case NEWLINE:
			case LOG:
			case LOG_INPUT:
			case FRAGMENTS:
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
			case FRAGMENTS:
			{
				fragments();
				break;
			}
			case NEWLINE:
			case LOG:
			case LOG_INPUT:
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
			case LOG_INPUT:
			{
				log();
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
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 279 "iosacl.g"
			
			*dbg << std::endl;
			
#line 1125 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::rule_std() {
	
	try {      // for error handling
		{
		hostaddr_std();
		if ( inputState->guessing==0 ) {
#line 287 "iosacl.g"
			importer->SaveTmpAddrToSrc(); *dbg << "(std) ";
#line 1147 "IOSCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case LOG:
		case LOG_INPUT:
		{
			log();
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
		if ( inputState->guessing==0 ) {
#line 290 "iosacl.g"
			
			*dbg << std::endl;
			
#line 1173 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::ip_protocols() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IP:
		{
			match(IP);
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
#line 298 "iosacl.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << "protocol " << LT(0)->getText() << " ";
			
#line 1214 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::hostaddr_ext() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case HOST:
		{
			{
			match(HOST);
			h = LT(1);
			match(IPV4);
			}
			if ( inputState->guessing==0 ) {
#line 350 "iosacl.g"
				
				importer->tmp_a = h->getText();
				importer->tmp_nm = "0.0.0.0";
				*dbg << h->getText() << "/0.0.0.0";
				
#line 1249 "IOSCfgParser.cpp"
			}
			break;
		}
		case IPV4:
		{
			{
			a = LT(1);
			match(IPV4);
			m = LT(1);
			match(IPV4);
			}
			if ( inputState->guessing==0 ) {
#line 357 "iosacl.g"
				
				importer->tmp_a = a->getText();
				importer->tmp_nm = m->getText();
				*dbg << a->getText() << "/" << m->getText();
				
#line 1268 "IOSCfgParser.cpp"
			}
			break;
		}
		case ANY:
		{
			match(ANY);
			if ( inputState->guessing==0 ) {
#line 364 "iosacl.g"
				
				importer->tmp_a = "0.0.0.0";
				importer->tmp_nm = "0.0.0.0";
				*dbg << "0.0.0.0/0.0.0.0";
				
#line 1282 "IOSCfgParser.cpp"
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
			consume();
			consumeUntil(_tokenSet_6);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::time_range() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  tr_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(TIME_RANGE);
		tr_name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 416 "iosacl.g"
			
			importer->time_range_name = tr_name->getText();
			*dbg << "time_range " << tr_name->getText() << " ";
			
#line 1316 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_7);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::fragments() {
	
	try {      // for error handling
		match(FRAGMENTS);
		if ( inputState->guessing==0 ) {
#line 409 "iosacl.g"
			
			importer->fragments = true;
			*dbg << "fragments ";
			
#line 1340 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_8);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::log() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case LOG:
		{
			match(LOG);
			break;
		}
		case LOG_INPUT:
		{
			match(LOG_INPUT);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 395 "iosacl.g"
			
			importer->logging = true;
			*dbg << "logging ";
			
#line 1381 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::icmp_spec() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_code = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_word = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case INT_CONST:
		{
			{
			icmp_type = LT(1);
			match(INT_CONST);
			icmp_code = LT(1);
			match(INT_CONST);
			}
			if ( inputState->guessing==0 ) {
#line 306 "iosacl.g"
				
				importer->icmp_type = icmp_type->getText();
				importer->icmp_code = icmp_code->getText();
				importer->icmp_spec = "";
				*dbg << icmp_type->getText() << " "
				<< icmp_code->getText() << " ";
				
#line 1420 "IOSCfgParser.cpp"
			}
			break;
		}
		case WORD:
		{
			icmp_word = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 315 "iosacl.g"
				
				importer->icmp_spec = icmp_word->getText();
				*dbg << icmp_word->getText() << " ";
				
#line 1434 "IOSCfgParser.cpp"
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
			consume();
			consumeUntil(_tokenSet_9);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::xoperator() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case P_EQ:
		case P_GT:
		case P_LT:
		case P_NEQ:
		{
			single_port_op();
			break;
		}
		case P_RANGE:
		{
			port_range();
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
			consume();
			consumeUntil(_tokenSet_10);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::established() {
	
	try {      // for error handling
		match(ESTABLISHED);
		if ( inputState->guessing==0 ) {
#line 402 "iosacl.g"
			
			importer->established = true;
			*dbg << "established ";
			
#line 1500 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_9);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::hostaddr_std() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		if ((LA(1) == IPV4) && (LA(2) == NEWLINE || LA(2) == LOG || LA(2) == LOG_INPUT)) {
			{
			h = LT(1);
			match(IPV4);
			}
			if ( inputState->guessing==0 ) {
#line 373 "iosacl.g"
				
				importer->tmp_a = h->getText();
				importer->tmp_nm = "0.0.0.0";
				*dbg << h->getText() << "/0.0.0.0";
				
#line 1532 "IOSCfgParser.cpp"
			}
		}
		else if ((LA(1) == IPV4) && (LA(2) == IPV4)) {
			{
			a = LT(1);
			match(IPV4);
			m = LT(1);
			match(IPV4);
			}
			if ( inputState->guessing==0 ) {
#line 380 "iosacl.g"
				
				importer->tmp_a = a->getText();
				importer->tmp_nm = m->getText();
				*dbg << a->getText() << "/" << m->getText();
				
#line 1549 "IOSCfgParser.cpp"
			}
		}
		else if ((LA(1) == ANY)) {
			match(ANY);
			if ( inputState->guessing==0 ) {
#line 387 "iosacl.g"
				
				importer->tmp_a = "0.0.0.0";
				importer->tmp_nm = "0.0.0.0";
				*dbg << "0.0.0.0/0.0.0.0";
				
#line 1561 "IOSCfgParser.cpp"
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_8);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::single_port_op() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case P_EQ:
		{
			match(P_EQ);
			break;
		}
		case P_GT:
		{
			match(P_GT);
			break;
		}
		case P_LT:
		{
			match(P_LT);
			break;
		}
		case P_NEQ:
		{
			match(P_NEQ);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 326 "iosacl.g"
			
			importer->tmp_port_op = LT(0)->getText();
			*dbg << LT(0)->getText() << " ";
			
#line 1617 "IOSCfgParser.cpp"
		}
		port_spec();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_10);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::port_range() {
	
	try {      // for error handling
		match(P_RANGE);
		if ( inputState->guessing==0 ) {
#line 334 "iosacl.g"
			
			importer->tmp_port_op = LT(0)->getText();
			*dbg << LT(0)->getText() << " ";
			
#line 1642 "IOSCfgParser.cpp"
		}
		port_spec();
		port_spec();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_10);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::port_spec() {
	
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
#line 342 "iosacl.g"
			
			importer->tmp_port_spec += (std::string(" ") + LT(0)->getText());
			*dbg << LT(0)->getText() << " ";
			
#line 1685 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_11);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::access_group_by_name() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  acln = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dir = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_GROUP);
		acln = LT(1);
		match(WORD);
		dir = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 513 "iosacl.g"
			
			importer->setInterfaceAndDirectionForRuleSet(
			acln->getText(),
			"",
			dir->getText() );
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE: ACL '" << acln->getText() << "'"
			<< " " << dir->getText() << std::endl;
			
#line 1720 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::access_group_by_number() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  acln = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dir = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_GROUP);
		acln = LT(1);
		match(INT_CONST);
		dir = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 528 "iosacl.g"
			
			importer->setInterfaceAndDirectionForRuleSet(
			std::string("acl_") + acln->getText(),
			"",
			dir->getText() );
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE: ACL '" << acln->getText() << "'"
			<< " " << dir->getText() << std::endl;
			
#line 1755 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::intf_address() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ADDRESS);
		a = LT(1);
		match(IPV4);
		m = LT(1);
		match(IPV4);
		{
		switch ( LA(1)) {
		case SECONDARY:
		{
			s = LT(1);
			match(SECONDARY);
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
		if ( inputState->guessing==0 ) {
#line 540 "iosacl.g"
			
			importer->addInterfaceAddress(a->getText(), m->getText());
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << a->getText()
			<< "/" << m->getText() << " ";
			if (s)
			{
			*dbg << s->getText();
			}
			*dbg <<  std::endl;
			
#line 1811 "IOSCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_4);
		} else {
			throw;
		}
	}
}

void IOSCfgParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* IOSCfgParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"NEWLINE",
	"\"ip\"",
	"WORD",
	"\"version\"",
	"NUMBER",
	"\"hostname\"",
	"STRING",
	"\"access-list\"",
	"INT_CONST",
	"\"extended\"",
	"\"permit\"",
	"\"deny\"",
	"\"icmp\"",
	"\"tcp\"",
	"\"udp\"",
	"\"eq\"",
	"\"gt\"",
	"\"lt\"",
	"\"neq\"",
	"\"range\"",
	"\"host\"",
	"IPV4",
	"\"any\"",
	"\"log\"",
	"\"log-input\"",
	"\"established\"",
	"\"fragments\"",
	"\"time-range\"",
	"\"vlan\"",
	"\"interface\"",
	"\"description\"",
	"\"remark\"",
	"\"shutdown\"",
	"\"access-group\"",
	"\"address\"",
	"\"secondary\"",
	"\"exit\"",
	"LINE_COMMENT",
	"\"standard\"",
	"Whitespace",
	"HEX_CONST",
	"NEG_INT_CONST",
	"DIGIT",
	"HEXDIGIT",
	"NUMBER_SIGN",
	"PERCENT",
	"AMPERSAND",
	"APOSTROPHE",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"STAR",
	"PLUS",
	"COMMA",
	"MINUS",
	"DOT",
	"SLASH",
	"COLON",
	"SEMICOLON",
	"LESS_THAN",
	"EQUALS",
	"GREATER_THAN",
	"QUESTION",
	"COMMERCIAL_AT",
	"OPENING_SQUARE",
	"CLOSING_SQUARE",
	"CARET",
	"UNDERLINE",
	"OPENING_BRACE",
	"CLOSING_BRACE",
	"TILDE",
	0
};

const unsigned long IOSCfgParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long IOSCfgParser::_tokenSet_1_data_[] = { 51954UL, 799UL, 0UL, 0UL };
// EOF NEWLINE "ip" WORD "version" "hostname" "access-list" "permit" "deny" 
// "vlan" "interface" "description" "remark" "shutdown" "exit" LINE_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long IOSCfgParser::_tokenSet_2_data_[] = { 2802UL, 791UL, 0UL, 0UL };
// EOF NEWLINE "ip" WORD "version" "hostname" "access-list" "vlan" "interface" 
// "description" "shutdown" "exit" LINE_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long IOSCfgParser::_tokenSet_3_data_[] = { 458848UL, 0UL, 0UL, 0UL };
// "ip" WORD "icmp" "tcp" "udp" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long IOSCfgParser::_tokenSet_4_data_[] = { 16UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long IOSCfgParser::_tokenSet_5_data_[] = { 117440512UL, 0UL, 0UL, 0UL };
// "host" IPV4 "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long IOSCfgParser::_tokenSet_6_data_[] = { 4294447184UL, 0UL, 0UL, 0UL };
// NEWLINE WORD INT_CONST "eq" "gt" "lt" "neq" "range" "host" IPV4 "any" 
// "log" "log-input" "established" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long IOSCfgParser::_tokenSet_7_data_[] = { 1476395024UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long IOSCfgParser::_tokenSet_8_data_[] = { 402653200UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long IOSCfgParser::_tokenSet_9_data_[] = { 3623878672UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_9(_tokenSet_9_data_,4);
const unsigned long IOSCfgParser::_tokenSet_10_data_[] = { 4278190096UL, 0UL, 0UL, 0UL };
// NEWLINE "host" IPV4 "any" "log" "log-input" "established" "fragments" 
// "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_10(_tokenSet_10_data_,4);
const unsigned long IOSCfgParser::_tokenSet_11_data_[] = { 4278194256UL, 0UL, 0UL, 0UL };
// NEWLINE WORD INT_CONST "host" IPV4 "any" "log" "log-input" "established" 
// "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IOSCfgParser::_tokenSet_11(_tokenSet_11_data_,4);



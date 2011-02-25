/* $ANTLR 2.7.7 (20090306): "pix.g" -> "PIXCfgParser.cpp"$ */
#line 40 "pix.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "PIXCfgParser.cpp"
#include "PIXCfgParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 46 "pix.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#include "../libgui/PIXImporter.h"

#line 22 "PIXCfgParser.cpp"
#line 1 "pix.g"
#line 24 "PIXCfgParser.cpp"
PIXCfgParser::PIXCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

PIXCfgParser::PIXCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
}

PIXCfgParser::PIXCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

PIXCfgParser::PIXCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
}

PIXCfgParser::PIXCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
}

void PIXCfgParser::cfgfile() {
	
	try {      // for error handling
		{ // ( ... )+
		int _cnt3=0;
		for (;;) {
			switch ( LA(1)) {
			case LINE_COMMENT:
			case COLON_COMMENT:
			{
				comment();
				break;
			}
			case PIX_WORD:
			case ASA_WORD:
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
			case CONTROLLER:
			{
				controller();
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
			case CERTIFICATE:
			{
				certificate();
				break;
			}
			case QUIT:
			{
				quit();
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
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::comment() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case LINE_COMMENT:
		{
			match(LINE_COMMENT);
			break;
		}
		case COLON_COMMENT:
		{
			match(COLON_COMMENT);
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

void PIXCfgParser::version() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case PIX_WORD:
		{
			match(PIX_WORD);
			break;
		}
		case ASA_WORD:
		{
			match(ASA_WORD);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(VERSION_WORD);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 150 "pix.g"
			
			importer->setDiscoveredVersion(LT(0)->getText());
			*dbg << "VERSION " << LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 211 "PIXCfgParser.cpp"
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

void PIXCfgParser::hostname() {
	
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
#line 159 "pix.g"
			
			importer->setHostName( LT(0)->getText() );
			*dbg << "HOSTNAME "
			<< "LT0=" << LT(0)->getText()
			<< std::endl;
			
#line 254 "PIXCfgParser.cpp"
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

void PIXCfgParser::ip_commands() {
	
	try {      // for error handling
		match(IP);
		{
		switch ( LA(1)) {
		case ACCESS_LIST:
		{
			ip_access_list_ext();
			break;
		}
		case SHUTDOWN:
		case ADDRESS:
		case SWITCHPORT:
		{
			interface_known_commands();
			break;
		}
		case COMMUNITY_LIST:
		{
			community_list_command();
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::intrface() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(INTRFACE);
		in = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 420 "pix.g"
			
			importer->newInterface( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE: " << in->getText() << std::endl;
			
#line 326 "PIXCfgParser.cpp"
		}
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

void PIXCfgParser::controller() {
	
	try {      // for error handling
		match(CONTROLLER);
		if ( inputState->guessing==0 ) {
#line 411 "pix.g"
			
			importer->clearCurrentInterface();
			consumeUntil(NEWLINE);
			
#line 350 "PIXCfgParser.cpp"
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

void PIXCfgParser::access_list_commands() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  acl_num = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_LIST);
		acl_num = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 174 "pix.g"
			
			importer->newUnidirRuleSet( std::string("acl_") + acl_num->getText() );
			*dbg << acl_num->getLine() << ":"
			<< " ACL #" << acl_num->getText() << " ";
			
#line 377 "PIXCfgParser.cpp"
		}
		{
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

void PIXCfgParser::exit() {
	
	try {      // for error handling
		match(EXIT);
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

void PIXCfgParser::description() {
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 440 "pix.g"
			
			*dbg << LT(1)->getLine() << ":";
			std::string descr;
			while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
			{
			descr += LT(1)->getText() + " ";
			consume();
			}
			importer->addInterfaceComment( descr );
			*dbg << " DESCRIPTION " << descr << std::endl;
			//consumeUntil(NEWLINE);
			
#line 446 "PIXCfgParser.cpp"
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

void PIXCfgParser::shutdown() {
	
	try {      // for error handling
		match(SHUTDOWN);
		if ( inputState->guessing==0 ) {
#line 475 "pix.g"
			
			*dbg<< LT(1)->getLine() << ":"
			<< " INTERFACE SHUTDOWN " << std::endl;
			
#line 469 "PIXCfgParser.cpp"
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

void PIXCfgParser::certificate() {
	
	try {      // for error handling
		match(CERTIFICATE);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 142 "pix.g"
			
			consumeUntil(NEWLINE);
			consumeUntil(QUIT);
			
#line 493 "PIXCfgParser.cpp"
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

void PIXCfgParser::quit() {
	
	try {      // for error handling
		match(QUIT);
		if ( inputState->guessing==0 ) {
#line 121 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 515 "PIXCfgParser.cpp"
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

void PIXCfgParser::unknown_command() {
	
	try {      // for error handling
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 135 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 537 "PIXCfgParser.cpp"
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

void PIXCfgParser::ip_access_list_ext() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_LIST);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 191 "pix.g"
			
			importer->newUnidirRuleSet( name->getText() );
			*dbg << name->getLine() << ":"
			<< " ACL ext " << name->getText() << std::endl;
			
#line 564 "PIXCfgParser.cpp"
		}
		{
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
		case LINE_COMMENT:
		case COLON_COMMENT:
		{
			comment();
			break;
		}
		case REMARK:
		{
			remark();
			break;
		}
		case NEWLINE:
		{
			match(NEWLINE);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 207 "pix.g"
			
			*dbg << LT(0)->getLine() << ":"
			<< " ACL line end" << std::endl << std::endl;
			
#line 606 "PIXCfgParser.cpp"
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

void PIXCfgParser::interface_known_commands() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case ADDRESS:
		{
			intf_address();
			break;
		}
		case SWITCHPORT:
		{
			switchport();
			break;
		}
		case SHUTDOWN:
		{
			shutdown();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
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

void PIXCfgParser::community_list_command() {
	
	try {      // for error handling
		match(COMMUNITY_LIST);
		if ( inputState->guessing==0 ) {
#line 128 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 666 "PIXCfgParser.cpp"
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

void PIXCfgParser::permit_ext() {
	
	try {      // for error handling
		match(PERMIT);
		if ( inputState->guessing==0 ) {
#line 215 "pix.g"
			
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 690 "PIXCfgParser.cpp"
		}
		rule_ext();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 221 "pix.g"
			
			importer->pushRule();
			
#line 699 "PIXCfgParser.cpp"
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

void PIXCfgParser::deny_ext() {
	
	try {      // for error handling
		match(DENY);
		if ( inputState->guessing==0 ) {
#line 227 "pix.g"
			
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 723 "PIXCfgParser.cpp"
		}
		rule_ext();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 233 "pix.g"
			
			importer->pushRule();
			
#line 732 "PIXCfgParser.cpp"
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

void PIXCfgParser::remark() {
	
	try {      // for error handling
		match(REMARK);
		if ( inputState->guessing==0 ) {
#line 460 "pix.g"
			
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
			
#line 763 "PIXCfgParser.cpp"
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

void PIXCfgParser::rule_ext() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IP:
		case WORD:
		{
			ip_protocols();
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 243 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 789 "PIXCfgParser.cpp"
			}
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 244 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 795 "PIXCfgParser.cpp"
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
#line 250 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 865 "PIXCfgParser.cpp"
			}
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 254 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 871 "PIXCfgParser.cpp"
			}
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 255 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 877 "PIXCfgParser.cpp"
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
#line 262 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 987 "PIXCfgParser.cpp"
			}
			hostaddr_ext();
			if ( inputState->guessing==0 ) {
#line 266 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 993 "PIXCfgParser.cpp"
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
#line 267 "pix.g"
					importer->SaveTmpPortToSrc();
#line 1007 "PIXCfgParser.cpp"
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
#line 268 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 1027 "PIXCfgParser.cpp"
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
#line 269 "pix.g"
					importer->SaveTmpPortToDst();
#line 1041 "PIXCfgParser.cpp"
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
#line 275 "pix.g"
			
			*dbg << std::endl;
			
#line 1151 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::ip_protocols() {
	
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
#line 283 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << "protocol " << LT(0)->getText() << " ";
			
#line 1191 "PIXCfgParser.cpp"
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

void PIXCfgParser::hostaddr_ext() {
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
#line 335 "pix.g"
				
				importer->tmp_a = h->getText();
				importer->tmp_nm = "0.0.0.0";
				*dbg << h->getText() << "/0.0.0.0";
				
#line 1225 "PIXCfgParser.cpp"
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
#line 342 "pix.g"
				
				importer->tmp_a = a->getText();
				importer->tmp_nm = m->getText();
				*dbg << a->getText() << "/" << m->getText();
				
#line 1244 "PIXCfgParser.cpp"
			}
			break;
		}
		case ANY:
		{
			match(ANY);
			if ( inputState->guessing==0 ) {
#line 349 "pix.g"
				
				importer->tmp_a = "0.0.0.0";
				importer->tmp_nm = "0.0.0.0";
				*dbg << "0.0.0.0/0.0.0.0";
				
#line 1258 "PIXCfgParser.cpp"
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

void PIXCfgParser::time_range() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  tr_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(TIME_RANGE);
		tr_name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 401 "pix.g"
			
			importer->time_range_name = tr_name->getText();
			*dbg << "time_range " << tr_name->getText() << " ";
			
#line 1291 "PIXCfgParser.cpp"
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

void PIXCfgParser::fragments() {
	
	try {      // for error handling
		match(FRAGMENTS);
		if ( inputState->guessing==0 ) {
#line 394 "pix.g"
			
			importer->fragments = true;
			*dbg << "fragments ";
			
#line 1314 "PIXCfgParser.cpp"
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

void PIXCfgParser::log() {
	
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
#line 380 "pix.g"
			
			importer->logging = true;
			*dbg << "logging ";
			
#line 1354 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::icmp_spec() {
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
#line 291 "pix.g"
				
				importer->icmp_type = icmp_type->getText();
				importer->icmp_code = icmp_code->getText();
				importer->icmp_spec = "";
				*dbg << icmp_type->getText() << " "
				<< icmp_code->getText() << " ";
				
#line 1392 "PIXCfgParser.cpp"
			}
			break;
		}
		case WORD:
		{
			icmp_word = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 300 "pix.g"
				
				importer->icmp_spec = icmp_word->getText();
				*dbg << icmp_word->getText() << " ";
				
#line 1406 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_7);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::xoperator() {
	
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
			recover(ex,_tokenSet_8);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::established() {
	
	try {      // for error handling
		match(ESTABLISHED);
		if ( inputState->guessing==0 ) {
#line 387 "pix.g"
			
			importer->established = true;
			*dbg << "established ";
			
#line 1470 "PIXCfgParser.cpp"
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

void PIXCfgParser::single_port_op() {
	
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
#line 311 "pix.g"
			
			importer->tmp_port_op = LT(0)->getText();
			*dbg << LT(0)->getText() << " ";
			
#line 1520 "PIXCfgParser.cpp"
		}
		port_spec();
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

void PIXCfgParser::port_range() {
	
	try {      // for error handling
		match(P_RANGE);
		if ( inputState->guessing==0 ) {
#line 319 "pix.g"
			
			importer->tmp_port_op = LT(0)->getText();
			*dbg << LT(0)->getText() << " ";
			
#line 1544 "PIXCfgParser.cpp"
		}
		port_spec();
		port_spec();
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

void PIXCfgParser::port_spec() {
	
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
#line 327 "pix.g"
			
			importer->tmp_port_spec += (std::string(" ") + LT(0)->getText());
			*dbg << LT(0)->getText() << " ";
			
#line 1586 "PIXCfgParser.cpp"
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

void PIXCfgParser::hostaddr_std() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		if ((LA(1) == IPV4) && (LA(2) == ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE)) {
			{
			h = LT(1);
			match(IPV4);
			}
			if ( inputState->guessing==0 ) {
#line 358 "pix.g"
				
				importer->tmp_a = h->getText();
				importer->tmp_nm = "0.0.0.0";
				*dbg << h->getText() << "/0.0.0.0";
				
#line 1617 "PIXCfgParser.cpp"
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
#line 365 "pix.g"
				
				importer->tmp_a = a->getText();
				importer->tmp_nm = m->getText();
				*dbg << a->getText() << "/" << m->getText();
				
#line 1634 "PIXCfgParser.cpp"
			}
		}
		else if ((LA(1) == ANY)) {
			match(ANY);
			if ( inputState->guessing==0 ) {
#line 372 "pix.g"
				
				importer->tmp_a = "0.0.0.0";
				importer->tmp_nm = "0.0.0.0";
				*dbg << "0.0.0.0/0.0.0.0";
				
#line 1646 "PIXCfgParser.cpp"
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
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

void PIXCfgParser::nameif() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(NAMEIF);
		in = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 429 "pix.g"
			
			importer->addInterfaceLabel( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE LABEL: " << in->getText() << std::endl;
			
#line 1678 "PIXCfgParser.cpp"
		}
		match(NEWLINE);
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

void PIXCfgParser::intf_address() {
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
#line 502 "pix.g"
			
			importer->addInterfaceAddress(a->getText(), m->getText());
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << a->getText()
			<< "/" << m->getText() << " ";
			if (s)
			{
			*dbg << s->getText();
			}
			*dbg <<  std::endl;
			
#line 1734 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::switchport() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  vlan_num = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SWITCHPORT);
		match(ACCESS);
		match(VLAN);
		vlan_num = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 516 "pix.g"
			
			
#line 1760 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::access_group_by_name() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  acln = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dir = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  intf_label = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_GROUP);
		acln = LT(1);
		match(WORD);
		dir = LT(1);
		match(WORD);
		match(INTRFACE);
		intf_label = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 523 "pix.g"
			
			importer->setInterfaceAndDirectionForRuleSet(
			acln->getText(),
			intf_label->getText(),
			dir->getText() );
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE: ACL '" << acln->getText() << "'"
			<< " " << intf_label->getText()
			<< " " << dir->getText() << std::endl;
			
#line 1799 "PIXCfgParser.cpp"
		}
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

void PIXCfgParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* PIXCfgParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"NEWLINE",
	"\"ip\"",
	"\"quit\"",
	"\"community-list\"",
	"WORD",
	"\"certificate\"",
	"\"PIX\"",
	"\"ASA\"",
	"\"Version\"",
	"NUMBER",
	"\"hostname\"",
	"STRING",
	"\"access-list\"",
	"INT_CONST",
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
	"\"controller\"",
	"\"interface\"",
	"\"nameif\"",
	"\"description\"",
	"\"remark\"",
	"\"shutdown\"",
	"\"address\"",
	"\"secondary\"",
	"\"switchport\"",
	"\"access\"",
	"\"vlan\"",
	"\"access-group\"",
	"\"exit\"",
	"LINE_COMMENT",
	"COLON_COMMENT",
	"\"extended\"",
	"\"standard\"",
	"Whitespace",
	"HEX_CONST",
	"NEG_INT_CONST",
	"DIGIT",
	"HEXDIGIT",
	"PIPE_CHAR",
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

const unsigned long PIXCfgParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long PIXCfgParser::_tokenSet_1_data_[] = { 85874UL, 459440UL, 0UL, 0UL };
// EOF NEWLINE "ip" "quit" WORD "certificate" "PIX" "ASA" "hostname" "access-list" 
// "controller" "interface" "description" "shutdown" "exit" LINE_COMMENT 
// COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long PIXCfgParser::_tokenSet_2_data_[] = { 16UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long PIXCfgParser::_tokenSet_3_data_[] = { 1879048192UL, 0UL, 0UL, 0UL };
// "host" IPV4 "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long PIXCfgParser::_tokenSet_4_data_[] = { 4286710032UL, 15UL, 0UL, 0UL };
// NEWLINE WORD INT_CONST "eq" "gt" "lt" "neq" "range" "host" IPV4 "any" 
// "log" "log-input" "established" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long PIXCfgParser::_tokenSet_5_data_[] = { 2147483664UL, 5UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long PIXCfgParser::_tokenSet_6_data_[] = { 2147483664UL, 1UL, 0UL, 0UL };
// NEWLINE "log" "log-input" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long PIXCfgParser::_tokenSet_7_data_[] = { 2147483664UL, 13UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long PIXCfgParser::_tokenSet_8_data_[] = { 4026531856UL, 15UL, 0UL, 0UL };
// NEWLINE "host" IPV4 "any" "log" "log-input" "established" "fragments" 
// "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long PIXCfgParser::_tokenSet_9_data_[] = { 4026663184UL, 15UL, 0UL, 0UL };
// NEWLINE WORD INT_CONST "host" IPV4 "any" "log" "log-input" "established" 
// "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_9(_tokenSet_9_data_,4);



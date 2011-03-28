/* $ANTLR 2.7.7 (20090306): "pix.g" -> "PIXCfgParser.cpp"$ */
#line 42 "pix.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "PIXCfgParser.cpp"
#include "PIXCfgParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 48 "pix.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#line 20 "PIXCfgParser.cpp"
#line 1 "pix.g"
#line 22 "PIXCfgParser.cpp"
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
			case INTRFACE:
			{
				intrface();
				break;
			}
			case NAMEIF:
			{
				nameif_top_level();
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
			case SSH:
			{
				ssh_command();
				break;
			}
			case TELNET:
			{
				telnet_command();
				break;
			}
			case ICMP:
			{
				icmp_top_level_command();
				break;
			}
			case ACCESS_GROUP:
			{
				access_group();
				break;
			}
			case EXIT:
			{
				exit();
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
			case NAMES:
			{
				names_section();
				break;
			}
			case NAME:
			{
				name_entry();
				break;
			}
			case CRYPTO:
			{
				crypto();
				break;
			}
			case NO:
			{
				no_commands();
				break;
			}
			case TIMEOUT:
			{
				timeout_command();
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
				if ((LA(1) == IP) && (LA(2) == COMMUNITY_LIST)) {
					community_list_command();
				}
				else if ((LA(1) == IP) && (LA(2) == WORD)) {
					unknown_ip_command();
				}
				else if ((LA(1) == OBJECT) && (LA(2) == NETWORK)) {
					named_object_network();
				}
				else if ((LA(1) == OBJECT) && (LA(2) == SERVICE)) {
					named_object_service();
				}
				else if ((LA(1) == OBJECT_GROUP) && (LA(2) == NETWORK)) {
					object_group_network();
				}
				else if ((LA(1) == OBJECT_GROUP) && (LA(2) == SERVICE)) {
					object_group_service();
				}
				else if ((LA(1) == OBJECT_GROUP) && (LA(2) == PROTOCOL)) {
					object_group_protocol();
				}
				else if ((LA(1) == OBJECT_GROUP) && (LA(2) == ICMP_OBJECT)) {
					object_group_icmp_8_0();
				}
				else if ((LA(1) == OBJECT_GROUP) && (LA(2) == ICMP_TYPE)) {
					object_group_icmp_8_3();
				}
			else {
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
#line 822 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setDiscoveredVersion(LT(0)->getText());
			*dbg << "VERSION " << LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 272 "PIXCfgParser.cpp"
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
#line 832 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setHostName( LT(0)->getText() );
			*dbg << "HOSTNAME "
			<< "LT0=" << LT(0)->getText()
			<< std::endl;
			
#line 316 "PIXCfgParser.cpp"
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

void PIXCfgParser::community_list_command() {
	
	try {      // for error handling
		match(IP);
		match(COMMUNITY_LIST);
		if ( inputState->guessing==0 ) {
#line 185 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 339 "PIXCfgParser.cpp"
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

void PIXCfgParser::unknown_ip_command() {
	
	try {      // for error handling
		match(IP);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 791 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 362 "PIXCfgParser.cpp"
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
	
	try {      // for error handling
		match(INTRFACE);
		{
		if ((LA(1) == WORD) && (_tokenSet_2.member(LA(2)))) {
			interface_command_6();
		}
		else if ((LA(1) == WORD) && (LA(2) == NEWLINE)) {
			interface_command_7();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
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

void PIXCfgParser::nameif_top_level() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  p_intf = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  intf_label = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  sec_level = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(NAMEIF);
		p_intf = LT(1);
		match(WORD);
		intf_label = LT(1);
		match(WORD);
		sec_level = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 1362 "pix.g"
			
			std::string label = (intf_label) ? intf_label->getText() : "";
			std::string seclevel = (sec_level) ? sec_level->getText() : "";
			importer->setInterfaceParametes(p_intf->getText(), label, seclevel);
			*dbg << " NAMEIF: "
			<< p_intf->getText() << label << seclevel << std::endl;
			
#line 424 "PIXCfgParser.cpp"
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

void PIXCfgParser::controller() {
	
	try {      // for error handling
		match(CONTROLLER);
		if ( inputState->guessing==0 ) {
#line 1278 "pix.g"
			
			importer->clearCurrentInterface();
			consumeUntil(NEWLINE);
			
#line 447 "PIXCfgParser.cpp"
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
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_LIST);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 844 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newUnidirRuleSet(
			name->getText(), libfwbuilder::Policy::TYPENAME );
			*dbg << name->getLine() << ":"
			<< " ACL ext " << name->getText() << std::endl;
			
#line 477 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
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
			if ((LA(1) == EXTENDED || LA(1) == PERMIT) && (_tokenSet_3.member(LA(2)))) {
				permit_extended();
			}
			else if ((LA(1) == EXTENDED || LA(1) == DENY) && (_tokenSet_4.member(LA(2)))) {
				deny_extended();
			}
			else if ((LA(1) == STANDARD) && (LA(2) == PERMIT)) {
				permit_standard();
			}
			else if ((LA(1) == STANDARD) && (LA(2) == DENY)) {
				deny_standard();
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 867 "pix.g"
			
			*dbg << LT(0)->getLine() << ":"
			<< " ACL line end" << std::endl << std::endl;
			
#line 521 "PIXCfgParser.cpp"
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

void PIXCfgParser::ssh_command() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  intf_label = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SSH);
		{
		switch ( LA(1)) {
		case TIMEOUT:
		{
			{
			match(TIMEOUT);
			match(INT_CONST);
			}
			break;
		}
		case IPV4:
		case OBJECT:
		case HOST:
		case OBJECT_GROUP:
		case INTRFACE:
		case ANY:
		{
			{
			hostaddr_expr();
			intf_label = LT(1);
			match(WORD);
			}
			if ( inputState->guessing==0 ) {
#line 1609 "pix.g"
				
				importer->clear();
				std::string acl_name = "ssh_commands_" + intf_label->getText();
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->newUnidirRuleSet(acl_name, libfwbuilder::Policy::TYPENAME );
				importer->newPolicyRule();
				importer->action = "permit";
				importer->SaveTmpAddrToDst();
				importer->setDstSelf();
				importer->protocol = "tcp";
				importer->dst_port_op = "eq";
				importer->dst_port_spec = "ssh";
				importer->setInterfaceAndDirectionForRuleSet(
				acl_name, intf_label->getText(), "in" );
				importer->pushRule();
				
#line 579 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::telnet_command() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  intf_label = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(TELNET);
		{
		switch ( LA(1)) {
		case TIMEOUT:
		{
			{
			match(TIMEOUT);
			match(INT_CONST);
			}
			break;
		}
		case IPV4:
		case OBJECT:
		case HOST:
		case OBJECT_GROUP:
		case INTRFACE:
		case ANY:
		{
			{
			hostaddr_expr();
			intf_label = LT(1);
			match(WORD);
			}
			if ( inputState->guessing==0 ) {
#line 1630 "pix.g"
				
				importer->clear();
				std::string acl_name = "telnet_commands_" + intf_label->getText();
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->newUnidirRuleSet(acl_name, libfwbuilder::Policy::TYPENAME );
				importer->newPolicyRule();
				importer->action = "permit";
				importer->SaveTmpAddrToDst();
				importer->setDstSelf();
				importer->protocol = "tcp";
				importer->dst_port_op = "eq";
				importer->dst_port_spec = "telnet";
				importer->setInterfaceAndDirectionForRuleSet(
				acl_name, intf_label->getText(), "in" );
				importer->pushRule();
				
#line 645 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::icmp_top_level_command() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  permit = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  deny = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  intf_label = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ICMP);
		{
		switch ( LA(1)) {
		case UNREACHABLE:
		{
			{
			match(UNREACHABLE);
			if ( inputState->guessing==0 ) {
#line 1659 "pix.g"
				
				consumeUntil(NEWLINE);
				
#line 684 "PIXCfgParser.cpp"
			}
			}
			break;
		}
		case PERMIT:
		case DENY:
		{
			{
			{
			switch ( LA(1)) {
			case PERMIT:
			{
				permit = LT(1);
				match(PERMIT);
				break;
			}
			case DENY:
			{
				deny = LT(1);
				match(DENY);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 1666 "pix.g"
				
				importer->clear();
				
#line 718 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 1670 "pix.g"
				
				importer->SaveTmpAddrToSrc();
				
#line 726 "PIXCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case INT_CONST:
			case ECHO:
			case ECHO_REPLY:
			case TIME_EXCEEDED:
			case UNREACHABLE:
			{
				icmp_types_for_icmp_command();
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
			intf_label = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 1675 "pix.g"
				
				std::string acl_name = "icmp_commands_" + intf_label->getText();
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->newUnidirRuleSet(acl_name, libfwbuilder::Policy::TYPENAME );
				importer->newPolicyRule();
				if (permit) importer->action = "permit";
				if (deny) importer->action = "deny";
				importer->setDstSelf();
				importer->protocol = "icmp";
				importer->setInterfaceAndDirectionForRuleSet(
				acl_name, intf_label->getText(), "in" );
				importer->pushRule();
				
#line 766 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::access_group() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  aclname = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dir = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  intf_label = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_GROUP);
		aclname = LT(1);
		match(WORD);
		dir = LT(1);
		match(WORD);
		match(INTRFACE);
		intf_label = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 1733 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setInterfaceAndDirectionForRuleSet(
			aclname->getText(),
			intf_label->getText(),
			dir->getText() );
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE: ACL '" << aclname->getText() << "'"
			<< " " << intf_label->getText()
			<< " " << dir->getText() << std::endl;
			
#line 815 "PIXCfgParser.cpp"
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

void PIXCfgParser::certificate() {
	
	try {      // for error handling
		match(CERTIFICATE);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 814 "pix.g"
			
			consumeUntil(NEWLINE);
			consumeUntil(QUIT);
			
#line 854 "PIXCfgParser.cpp"
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
#line 178 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 876 "PIXCfgParser.cpp"
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

void PIXCfgParser::names_section() {
	
	try {      // for error handling
		match(NAMES);
		if ( inputState->guessing==0 ) {
#line 199 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addMessageToLog(
			"Parser warning: \"names\" section detected. "
			"Import of configuration that uses \"names\" "
			"is not supported at this time");
			
#line 902 "PIXCfgParser.cpp"
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

void PIXCfgParser::name_entry() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(NAME);
		{
		switch ( LA(1)) {
		case IPV4:
		{
			a = LT(1);
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
		n = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 209 "pix.g"
			
			if (a)
			{
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addMessageToLog(
			"Name " + a->getText() + " " + n->getText());
			*dbg << "Name " << a->getText() << " " << n->getText() << std::endl;
			}
			if (v6)
			{
			importer->addMessageToLog(
			"Parser warning: IPv6 import is not supported. ");
			consumeUntil(NEWLINE);
			}
			
#line 961 "PIXCfgParser.cpp"
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

void PIXCfgParser::named_object_network() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT);
		match(NETWORK);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 242 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newNamedObjectAddress(name->getText());
			*dbg << name->getLine() << ":"
			<< " Named Object " << name->getText() << std::endl;
			
#line 991 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == NEWLINE) && ((LA(2) >= NAT && LA(2) <= SUBNET))) {
				match(NEWLINE);
				named_object_network_parameters();
			}
			else {
				goto _loop14;
			}
			
		}
		_loop14:;
		} // ( ... )*
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

void PIXCfgParser::named_object_service() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT);
		match(SERVICE);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 352 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newNamedObjectService(name->getText());
			*dbg << name->getLine() << ":"
			<< " Named Object " << name->getText() << std::endl;
			
#line 1034 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == SERVICE)) {
				match(NEWLINE);
				named_object_service_parameters();
			}
			else {
				goto _loop30;
			}
			
		}
		_loop30:;
		} // ( ... )*
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

void PIXCfgParser::object_group_network() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(NETWORK);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 468 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupNetwork(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1077 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt49=0;
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == GROUP_OBJECT || LA(2) == NETWORK_OBJECT)) {
				object_group_network_parameters();
			}
			else {
				if ( _cnt49>=1 ) { goto _loop49; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt49++;
		}
		_loop49:;
		}  // ( ... )+
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

void PIXCfgParser::object_group_service() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  tcp = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  udp = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  tcpudp = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(SERVICE);
		name = LT(1);
		match(WORD);
		{
		switch ( LA(1)) {
		case TCP:
		{
			tcp = LT(1);
			match(TCP);
			break;
		}
		case UDP:
		{
			udp = LT(1);
			match(UDP);
			break;
		}
		case TCP_UDP:
		{
			tcpudp = LT(1);
			match(TCP_UDP);
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
#line 690 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupService(name->getText());
			if (tcp) importer->setObjectGroupServiceProtocol("tcp");
			if (udp) importer->setObjectGroupServiceProtocol("udp");
			if (tcpudp) importer->setObjectGroupServiceProtocol("tcp-udp");
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1157 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt81=0;
		for (;;) {
			if ((LA(1) == NEWLINE) && (_tokenSet_5.member(LA(2)))) {
				object_group_service_parameters();
			}
			else {
				if ( _cnt81>=1 ) { goto _loop81; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt81++;
		}
		_loop81:;
		}  // ( ... )+
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

void PIXCfgParser::object_group_protocol() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(PROTOCOL);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 566 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupProtocol(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1201 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt61=0;
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == GROUP_OBJECT || LA(2) == PROTOCOL_OBJECT)) {
				object_group_protocol_parameters();
			}
			else {
				if ( _cnt61>=1 ) { goto _loop61; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt61++;
		}
		_loop61:;
		}  // ( ... )+
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

void PIXCfgParser::object_group_icmp_8_0() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(ICMP_OBJECT);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 621 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupICMP(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1245 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == GROUP_OBJECT || LA(2) == ICMP_OBJECT)) {
				object_group_icmp_parameters();
			}
			else {
				goto _loop69;
			}
			
		}
		_loop69:;
		} // ( ... )*
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

void PIXCfgParser::object_group_icmp_8_3() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(ICMP_TYPE);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 634 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupICMP(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1287 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == GROUP_OBJECT || LA(2) == ICMP_OBJECT)) {
				object_group_icmp_parameters();
			}
			else {
				goto _loop72;
			}
			
		}
		_loop72:;
		} // ( ... )*
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

void PIXCfgParser::crypto() {
	
	try {      // for error handling
		match(CRYPTO);
		if ( inputState->guessing==0 ) {
#line 784 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1321 "PIXCfgParser.cpp"
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

void PIXCfgParser::no_commands() {
	
	try {      // for error handling
		match(NO);
		if ( inputState->guessing==0 ) {
#line 805 "pix.g"
			
			*dbg << " TOP LEVEL \"NO\" COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 1345 "PIXCfgParser.cpp"
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

void PIXCfgParser::timeout_command() {
	
	try {      // for error handling
		match(TIMEOUT);
		if ( inputState->guessing==0 ) {
#line 192 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1367 "PIXCfgParser.cpp"
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
#line 798 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1389 "PIXCfgParser.cpp"
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

void PIXCfgParser::ip_protocol_names() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
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
		case ESP:
		{
			match(ESP);
			break;
		}
		case GRE:
		{
			match(GRE);
			break;
		}
		case IGMP:
		{
			match(IGMP);
			break;
		}
		case IGRP:
		{
			match(IGRP);
			break;
		}
		case IP:
		{
			match(IP);
			break;
		}
		case IPINIP:
		{
			match(IPINIP);
			break;
		}
		case IPSEC:
		{
			match(IPSEC);
			break;
		}
		case NOS:
		{
			match(NOS);
			break;
		}
		case OSPF:
		{
			match(OSPF);
			break;
		}
		case PCP:
		{
			match(PCP);
			break;
		}
		case PIM:
		{
			match(PIM);
			break;
		}
		case PPTP:
		{
			match(PPTP);
			break;
		}
		case SNP:
		{
			match(SNP);
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
			recover(ex,_tokenSet_6);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::named_object_network_parameters() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case NAT:
		{
			named_object_nat();
			break;
		}
		case HOST:
		{
			host_addr();
			break;
		}
		case RANGE:
		{
			range_addr();
			break;
		}
		case SUBNET:
		{
			subnet_addr();
			break;
		}
		case DESCRIPTION:
		{
			named_object_description();
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

void PIXCfgParser::named_object_nat() {
	
	try {      // for error handling
		match(NAT);
		if ( inputState->guessing==0 ) {
#line 270 "pix.g"
			
			importer->addMessageToLog(
			"Parser warning: "
			"Import of named objects with \"nat\" command "
			"is not supported at this time");
			consumeUntil(NEWLINE);
			
#line 1559 "PIXCfgParser.cpp"
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

void PIXCfgParser::host_addr() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		match(HOST);
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
		}
		if ( inputState->guessing==0 ) {
#line 301 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			if (h)
			{
			importer->tmp_a = h->getText();
			importer->tmp_nm = "255.255.255.255";
			importer->commitNamedAddressObject();
			*dbg << h->getText() << "/255.255.255.255";
			}
			if (v6)
			{
			importer->addMessageToLog(
			"Parser warning: IPv6 import is not supported. ");
			consumeUntil(NEWLINE);
			}
			
#line 1618 "PIXCfgParser.cpp"
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

void PIXCfgParser::range_addr() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  r1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  r2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		match(RANGE);
		r1 = LT(1);
		match(IPV4);
		r2 = LT(1);
		match(IPV4);
		}
		if ( inputState->guessing==0 ) {
#line 320 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->tmp_range_1 = r1->getText();
			importer->tmp_range_2 = r2->getText();
			importer->commitNamedAddressRangeObject();
			*dbg << r1->getText() << "/" << r2->getText();
			
#line 1652 "PIXCfgParser.cpp"
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

void PIXCfgParser::subnet_addr() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		match(SUBNET);
		{
		switch ( LA(1)) {
		case IPV4:
		{
			{
			a = LT(1);
			match(IPV4);
			nm = LT(1);
			match(IPV4);
			}
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
		}
		if ( inputState->guessing==0 ) {
#line 330 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			if (a)
			{
			importer->tmp_a = a->getText();
			importer->tmp_nm = nm->getText();
			importer->commitNamedAddressObject();
			*dbg << a->getText() << "/" << nm->getText();
			}
			if (v6)
			{
			importer->addMessageToLog(
			"Parser warning: IPv6 import is not supported. ");
			consumeUntil(NEWLINE);
			}
			
#line 1716 "PIXCfgParser.cpp"
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

void PIXCfgParser::named_object_description() {
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 280 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			*dbg << LT(1)->getLine() << ":";
			std::string descr;
			while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
			{
			descr += LT(1)->getText() + " ";
			consume();
			}
			importer->setNamedObjectDescription(descr);
			*dbg << " DESCRIPTION " << descr << std::endl;
			
#line 1747 "PIXCfgParser.cpp"
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

void PIXCfgParser::named_object_service_parameters() {
	
	try {      // for error handling
		{
		if ((LA(1) == SERVICE) && (LA(2) == ICMP)) {
			service_icmp();
		}
		else if ((LA(1) == SERVICE) && (LA(2) == ICMP6)) {
			service_icmp6();
		}
		else if ((LA(1) == SERVICE) && (LA(2) == TCP || LA(2) == UDP)) {
			service_tcp_udp();
		}
		else if ((LA(1) == SERVICE) && (_tokenSet_7.member(LA(2)))) {
			service_other();
		}
		else if ((LA(1) == SERVICE) && (LA(2) == WORD)) {
			service_unknown();
		}
		else if ((LA(1) == DESCRIPTION)) {
			named_object_description();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
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

void PIXCfgParser::service_icmp() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SERVICE);
		match(ICMP);
		{
		switch ( LA(1)) {
		case INT_CONST:
		{
			icmp_type = LT(1);
			match(INT_CONST);
			if ( inputState->guessing==0 ) {
#line 384 "pix.g"
				
				importer->icmp_type = LT(0)->getText();
				
#line 1815 "PIXCfgParser.cpp"
			}
			break;
		}
		case ECHO:
		case ALTERNATE_ADDRESS:
		case CONVERSION_ERROR:
		case ECHO_REPLY:
		case INFORMATION_REPLY:
		case INFORMATION_REQUEST:
		case MASK_REPLY:
		case MASK_REQUEST:
		case MOBILE_REDIRECT:
		case PARAMETER_PROBLEM:
		case REDIRECT:
		case ROUTER_ADVERTISEMENT:
		case ROUTER_SOLICITATION:
		case SOURCE_QUENCH:
		case TIME_EXCEEDED:
		case TIMESTAMP_REPLY:
		case TIMESTAMP_REQUEST:
		case TRACEROUTE:
		case UNREACHABLE:
		{
			icmp_names();
			if ( inputState->guessing==0 ) {
#line 389 "pix.g"
				
				importer->icmp_spec = LT(0)->getText();
				
#line 1845 "PIXCfgParser.cpp"
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
#line 393 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->commitNamedICMPServiceObject();
			*dbg << "NAMED OBJECT SERVICE ICMP " << LT(0)->getText() << " ";
			
#line 1862 "PIXCfgParser.cpp"
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

void PIXCfgParser::service_icmp6() {
	
	try {      // for error handling
		match(SERVICE);
		match(ICMP6);
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
#line 401 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addMessageToLog("Parser warning: "
			"Import of IPv6 addresses and servcies "
			"is not supported at this time");
			*dbg << "NAMED OBJECT SERVICE ICMP6 " << LT(0)->getText() << " ";
			consumeUntil(NEWLINE);
			
#line 1908 "PIXCfgParser.cpp"
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

void PIXCfgParser::service_tcp_udp() {
	
	try {      // for error handling
		match(SERVICE);
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
#line 412 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
			
#line 1949 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case SOURCE:
		{
			src_port_spec();
			break;
		}
		case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
		case NEWLINE:
		case QUIT:
		case IP:
		case TIMEOUT:
		case NAMES:
		case NAME:
		case WORD:
		case OBJECT:
		case RANGE:
		case ICMP:
		case DESTINATION:
		case OBJECT_GROUP:
		case CRYPTO:
		case NO:
		case CERTIFICATE:
		case PIX_WORD:
		case ASA_WORD:
		case HOSTNAME:
		case ACCESS_LIST:
		case P_EQ:
		case P_GT:
		case P_LT:
		case P_NEQ:
		case SSH:
		case TELNET:
		case INTRFACE:
		case CONTROLLER:
		case LINE_COMMENT:
		case EXIT:
		case NAMEIF:
		case ACCESS_GROUP:
		case COLON_COMMENT:
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
		case RANGE:
		case DESTINATION:
		case P_EQ:
		case P_GT:
		case P_LT:
		case P_NEQ:
		{
			dst_port_spec();
			break;
		}
		case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
		case NEWLINE:
		case QUIT:
		case IP:
		case TIMEOUT:
		case NAMES:
		case NAME:
		case WORD:
		case OBJECT:
		case ICMP:
		case OBJECT_GROUP:
		case CRYPTO:
		case NO:
		case CERTIFICATE:
		case PIX_WORD:
		case ASA_WORD:
		case HOSTNAME:
		case ACCESS_LIST:
		case SSH:
		case TELNET:
		case INTRFACE:
		case CONTROLLER:
		case LINE_COMMENT:
		case EXIT:
		case NAMEIF:
		case ACCESS_GROUP:
		case COLON_COMMENT:
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
#line 418 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->commitNamedTCPUDPServiceObject();
			
#line 2054 "PIXCfgParser.cpp"
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

void PIXCfgParser::service_other() {
	
	try {      // for error handling
		match(SERVICE);
		{
		switch ( LA(1)) {
		case INT_CONST:
		{
			match(INT_CONST);
			break;
		}
		case IP:
		case AH:
		case EIGRP:
		case ESP:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
		case PIM:
		case PPTP:
		case SNP:
		{
			ip_protocol_names();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 437 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->protocol = LT(0)->getText();
			importer->commitNamedIPServiceObject();
			*dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
			
#line 2111 "PIXCfgParser.cpp"
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

void PIXCfgParser::service_unknown() {
	
	try {      // for error handling
		match(SERVICE);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 452 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->protocol = "ip";
			importer->commitNamedIPServiceObject();
			std::string err = "Parser warning: Unknown service name " +
			LT(0)->getText();
			importer->setNamedObjectDescription(err);
			importer->addMessageToLog(err);
			*dbg << "UNKNOWN SERVICE " << LT(0)->getText() << " ";
			
#line 2141 "PIXCfgParser.cpp"
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

void PIXCfgParser::icmp_names() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case ALTERNATE_ADDRESS:
		{
			match(ALTERNATE_ADDRESS);
			break;
		}
		case CONVERSION_ERROR:
		{
			match(CONVERSION_ERROR);
			break;
		}
		case ECHO:
		{
			match(ECHO);
			break;
		}
		case ECHO_REPLY:
		{
			match(ECHO_REPLY);
			break;
		}
		case INFORMATION_REPLY:
		{
			match(INFORMATION_REPLY);
			break;
		}
		case INFORMATION_REQUEST:
		{
			match(INFORMATION_REQUEST);
			break;
		}
		case MASK_REPLY:
		{
			match(MASK_REPLY);
			break;
		}
		case MASK_REQUEST:
		{
			match(MASK_REQUEST);
			break;
		}
		case MOBILE_REDIRECT:
		{
			match(MOBILE_REDIRECT);
			break;
		}
		case PARAMETER_PROBLEM:
		{
			match(PARAMETER_PROBLEM);
			break;
		}
		case REDIRECT:
		{
			match(REDIRECT);
			break;
		}
		case ROUTER_ADVERTISEMENT:
		{
			match(ROUTER_ADVERTISEMENT);
			break;
		}
		case ROUTER_SOLICITATION:
		{
			match(ROUTER_SOLICITATION);
			break;
		}
		case SOURCE_QUENCH:
		{
			match(SOURCE_QUENCH);
			break;
		}
		case TIME_EXCEEDED:
		{
			match(TIME_EXCEEDED);
			break;
		}
		case TIMESTAMP_REPLY:
		{
			match(TIMESTAMP_REPLY);
			break;
		}
		case TIMESTAMP_REQUEST:
		{
			match(TIMESTAMP_REQUEST);
			break;
		}
		case TRACEROUTE:
		{
			match(TRACEROUTE);
			break;
		}
		case UNREACHABLE:
		{
			match(UNREACHABLE);
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

void PIXCfgParser::src_port_spec() {
	
	try {      // for error handling
		match(SOURCE);
		xoperator();
		if ( inputState->guessing==0 ) {
#line 425 "pix.g"
			
			importer->SaveTmpPortToSrc();
			
#line 2281 "PIXCfgParser.cpp"
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

void PIXCfgParser::dst_port_spec() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case DESTINATION:
		{
			match(DESTINATION);
			break;
		}
		case RANGE:
		case P_EQ:
		case P_GT:
		case P_LT:
		case P_NEQ:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		xoperator();
		if ( inputState->guessing==0 ) {
#line 431 "pix.g"
			
			importer->SaveTmpPortToDst();
			
#line 2324 "PIXCfgParser.cpp"
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
		case RANGE:
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
			recover(ex,_tokenSet_10);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::object_group_network_parameters() {
	
	try {      // for error handling
		match(NEWLINE);
		{
		switch ( LA(1)) {
		case DESCRIPTION:
		{
			object_group_description();
			break;
		}
		case GROUP_OBJECT:
		{
			group_object();
			break;
		}
		case NETWORK_OBJECT:
		{
			network_object();
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

void PIXCfgParser::object_group_description() {
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 492 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			*dbg << LT(1)->getLine() << ":";
			std::string descr;
			while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
			{
			descr += LT(1)->getText() + " ";
			consume();
			}
			importer->setObjectGroupDescription(descr);
			*dbg << " DESCRIPTION " << descr << std::endl;
			
#line 2426 "PIXCfgParser.cpp"
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

void PIXCfgParser::group_object() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(GROUP_OBJECT);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 507 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addNamedObjectToObjectGroup(name->getText());
			*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
			
#line 2454 "PIXCfgParser.cpp"
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

void PIXCfgParser::network_object() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  hv6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(NETWORK_OBJECT);
		if ( inputState->guessing==0 ) {
#line 516 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 2483 "PIXCfgParser.cpp"
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
				{
				a = LT(1);
				match(IPV4);
				nm = LT(1);
				match(IPV4);
				}
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
			if ( inputState->guessing==0 ) {
#line 522 "pix.g"
				
				if (a)
				{
				importer->tmp_a = a->getText();
				importer->tmp_nm = nm->getText();
				importer->addNetworkToObjectGroup();
				*dbg << a->getText() << "/" << nm->getText();
				}
				if (v6)
				{
				importer->addMessageToLog(
				"Parser warning: IPv6 import is not supported. ");
				consumeUntil(NEWLINE);
				}
				
#line 2531 "PIXCfgParser.cpp"
			}
			break;
		}
		case HOST:
		{
			match(HOST);
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
				hv6 = LT(1);
				match(IPV6);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 539 "pix.g"
				
				if (h)
				{
				importer->tmp_a = h->getText();
				importer->tmp_nm = "255.255.255.255";
				importer->addNetworkToObjectGroup();
				*dbg << h->getText() << "/255.255.255.255";
				}
				if (hv6)
				{
				importer->addMessageToLog(
				"Parser warning: IPv6 import is not supported. ");
				consumeUntil(NEWLINE);
				}
				
#line 2575 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 556 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 2590 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::object_group_protocol_parameters() {
	
	try {      // for error handling
		match(NEWLINE);
		{
		switch ( LA(1)) {
		case DESCRIPTION:
		{
			object_group_description();
			break;
		}
		case GROUP_OBJECT:
		{
			group_object();
			break;
		}
		case PROTOCOL_OBJECT:
		{
			protocol_object();
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

void PIXCfgParser::protocol_object() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(PROTOCOL_OBJECT);
		if ( inputState->guessing==0 ) {
#line 590 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 2660 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case IP:
		case AH:
		case EIGRP:
		case ESP:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
		case PIM:
		case PPTP:
		case SNP:
		case ICMP:
		case INT_CONST:
		case TCP:
		case UDP:
		{
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			case ICMP:
			{
				match(ICMP);
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
			case IP:
			case AH:
			case EIGRP:
			case ESP:
			case GRE:
			case IGMP:
			case IGRP:
			case IPINIP:
			case IPSEC:
			case NOS:
			case OSPF:
			case PCP:
			case PIM:
			case PPTP:
			case SNP:
			{
				ip_protocol_names();
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 596 "pix.g"
				
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->protocol = LT(0)->getText();
				importer->addIPServiceToObjectGroup();
				*dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
				
#line 2739 "PIXCfgParser.cpp"
			}
			break;
		}
		case ICMP6:
		{
			match(ICMP6);
			if ( inputState->guessing==0 ) {
#line 604 "pix.g"
				
				importer->addMessageToLog(
				"Parser warning: IPv6 import is not supported. ");
				consumeUntil(NEWLINE);
				
#line 2753 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 611 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 2768 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::object_group_icmp_parameters() {
	
	try {      // for error handling
		match(NEWLINE);
		{
		switch ( LA(1)) {
		case DESCRIPTION:
		{
			object_group_description();
			break;
		}
		case GROUP_OBJECT:
		{
			group_object();
			break;
		}
		case ICMP_OBJECT:
		{
			icmp_object();
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

void PIXCfgParser::icmp_object() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ICMP_OBJECT);
		if ( inputState->guessing==0 ) {
#line 658 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 2839 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case INT_CONST:
		case ECHO:
		case ALTERNATE_ADDRESS:
		case CONVERSION_ERROR:
		case ECHO_REPLY:
		case INFORMATION_REPLY:
		case INFORMATION_REQUEST:
		case MASK_REPLY:
		case MASK_REQUEST:
		case MOBILE_REDIRECT:
		case PARAMETER_PROBLEM:
		case REDIRECT:
		case ROUTER_ADVERTISEMENT:
		case ROUTER_SOLICITATION:
		case SOURCE_QUENCH:
		case TIME_EXCEEDED:
		case TIMESTAMP_REPLY:
		case TIMESTAMP_REQUEST:
		case TRACEROUTE:
		case UNREACHABLE:
		{
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				icmp_type = LT(1);
				match(INT_CONST);
				if ( inputState->guessing==0 ) {
#line 665 "pix.g"
					
					importer->icmp_type = LT(0)->getText();
					
#line 2875 "PIXCfgParser.cpp"
				}
				break;
			}
			case ECHO:
			case ALTERNATE_ADDRESS:
			case CONVERSION_ERROR:
			case ECHO_REPLY:
			case INFORMATION_REPLY:
			case INFORMATION_REQUEST:
			case MASK_REPLY:
			case MASK_REQUEST:
			case MOBILE_REDIRECT:
			case PARAMETER_PROBLEM:
			case REDIRECT:
			case ROUTER_ADVERTISEMENT:
			case ROUTER_SOLICITATION:
			case SOURCE_QUENCH:
			case TIME_EXCEEDED:
			case TIMESTAMP_REPLY:
			case TIMESTAMP_REQUEST:
			case TRACEROUTE:
			case UNREACHABLE:
			{
				icmp_names();
				if ( inputState->guessing==0 ) {
#line 670 "pix.g"
					
					importer->icmp_spec = LT(0)->getText();
					
#line 2905 "PIXCfgParser.cpp"
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
#line 674 "pix.g"
				
				importer->addICMPServiceToObjectGroup();
				*dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
				
#line 2921 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 680 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 2936 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::object_group_service_parameters() {
	
	try {      // for error handling
		match(NEWLINE);
		{
		switch ( LA(1)) {
		case DESCRIPTION:
		{
			object_group_description();
			break;
		}
		case GROUP_OBJECT:
		{
			group_object();
			break;
		}
		case SERVICE_OBJECT:
		{
			service_object();
			break;
		}
		case PORT_OBJECT:
		{
			port_object();
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

void PIXCfgParser::service_object() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SERVICE_OBJECT);
		if ( inputState->guessing==0 ) {
#line 719 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 3012 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case IP:
		case AH:
		case EIGRP:
		case ESP:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
		case PIM:
		case PPTP:
		case SNP:
		case INT_CONST:
		{
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			case IP:
			case AH:
			case EIGRP:
			case ESP:
			case GRE:
			case IGMP:
			case IGRP:
			case IPINIP:
			case IPSEC:
			case NOS:
			case OSPF:
			case PCP:
			case PIM:
			case PPTP:
			case SNP:
			{
				ip_protocol_names();
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 725 "pix.g"
				
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->protocol = LT(0)->getText();
				importer->addIPServiceToObjectGroup();
				*dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
				
#line 3073 "PIXCfgParser.cpp"
			}
			break;
		}
		case TCP:
		case UDP:
		case TCP_UDP:
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
			case TCP_UDP:
			{
				match(TCP_UDP);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 733 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << " SERVICE TCP/UDP" << LT(0)->getText() << " ";
				
#line 3110 "PIXCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case SOURCE:
			{
				src_port_spec();
				break;
			}
			case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
			case NEWLINE:
			case QUIT:
			case IP:
			case TIMEOUT:
			case NAMES:
			case NAME:
			case WORD:
			case OBJECT:
			case RANGE:
			case ICMP:
			case DESTINATION:
			case OBJECT_GROUP:
			case CRYPTO:
			case NO:
			case CERTIFICATE:
			case PIX_WORD:
			case ASA_WORD:
			case HOSTNAME:
			case ACCESS_LIST:
			case P_EQ:
			case P_GT:
			case P_LT:
			case P_NEQ:
			case SSH:
			case TELNET:
			case INTRFACE:
			case CONTROLLER:
			case LINE_COMMENT:
			case EXIT:
			case NAMEIF:
			case ACCESS_GROUP:
			case COLON_COMMENT:
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
			case RANGE:
			case DESTINATION:
			case P_EQ:
			case P_GT:
			case P_LT:
			case P_NEQ:
			{
				dst_port_spec();
				break;
			}
			case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
			case NEWLINE:
			case QUIT:
			case IP:
			case TIMEOUT:
			case NAMES:
			case NAME:
			case WORD:
			case OBJECT:
			case ICMP:
			case OBJECT_GROUP:
			case CRYPTO:
			case NO:
			case CERTIFICATE:
			case PIX_WORD:
			case ASA_WORD:
			case HOSTNAME:
			case ACCESS_LIST:
			case SSH:
			case TELNET:
			case INTRFACE:
			case CONTROLLER:
			case LINE_COMMENT:
			case EXIT:
			case NAMEIF:
			case ACCESS_GROUP:
			case COLON_COMMENT:
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
#line 739 "pix.g"
				
				importer->addTCPUDPServiceToObjectGroup();
				
#line 3214 "PIXCfgParser.cpp"
			}
			break;
		}
		case ICMP:
		{
			match(ICMP);
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				icmp_type = LT(1);
				match(INT_CONST);
				if ( inputState->guessing==0 ) {
#line 746 "pix.g"
					
					importer->icmp_type = LT(0)->getText();
					
#line 3232 "PIXCfgParser.cpp"
				}
				break;
			}
			case ECHO:
			case ALTERNATE_ADDRESS:
			case CONVERSION_ERROR:
			case ECHO_REPLY:
			case INFORMATION_REPLY:
			case INFORMATION_REQUEST:
			case MASK_REPLY:
			case MASK_REQUEST:
			case MOBILE_REDIRECT:
			case PARAMETER_PROBLEM:
			case REDIRECT:
			case ROUTER_ADVERTISEMENT:
			case ROUTER_SOLICITATION:
			case SOURCE_QUENCH:
			case TIME_EXCEEDED:
			case TIMESTAMP_REPLY:
			case TIMESTAMP_REQUEST:
			case TRACEROUTE:
			case UNREACHABLE:
			{
				icmp_names();
				if ( inputState->guessing==0 ) {
#line 751 "pix.g"
					
					importer->icmp_spec = LT(0)->getText();
					
#line 3262 "PIXCfgParser.cpp"
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
#line 755 "pix.g"
				
				importer->addICMPServiceToObjectGroup();
				*dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
				
#line 3278 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 761 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 3293 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::port_object() {
#line 768 "pix.g"
	
	importer->tmp_port_spec = "";
	importer->tmp_port_spec_2 = "";
	
#line 3320 "PIXCfgParser.cpp"
	
	try {      // for error handling
		match(PORT_OBJECT);
		xoperator();
		if ( inputState->guessing==0 ) {
#line 773 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			*dbg << " PORT OBJECT TCP/UDP " << LT(0)->getText() << " " << std::endl;
			importer->SaveTmpPortToDst();
			importer->addTCPUDPServiceToObjectGroup();
			*dbg << std::endl;
			
#line 3334 "PIXCfgParser.cpp"
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

void PIXCfgParser::permit_extended() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXTENDED:
		{
			match(EXTENDED);
			break;
		}
		case PERMIT:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(PERMIT);
		if ( inputState->guessing==0 ) {
#line 875 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 3376 "PIXCfgParser.cpp"
		}
		rule_extended();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 882 "pix.g"
			
			importer->pushRule();
			
#line 3385 "PIXCfgParser.cpp"
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

void PIXCfgParser::deny_extended() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case EXTENDED:
		{
			match(EXTENDED);
			break;
		}
		case DENY:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(DENY);
		if ( inputState->guessing==0 ) {
#line 888 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 3427 "PIXCfgParser.cpp"
		}
		rule_extended();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 895 "pix.g"
			
			importer->pushRule();
			
#line 3436 "PIXCfgParser.cpp"
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

void PIXCfgParser::permit_standard() {
	
	try {      // for error handling
		match(STANDARD);
		match(PERMIT);
		if ( inputState->guessing==0 ) {
#line 901 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 3462 "PIXCfgParser.cpp"
		}
		rule_standard();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 908 "pix.g"
			
			importer->pushRule();
			
#line 3471 "PIXCfgParser.cpp"
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

void PIXCfgParser::deny_standard() {
	
	try {      // for error handling
		match(STANDARD);
		match(DENY);
		if ( inputState->guessing==0 ) {
#line 914 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 3497 "PIXCfgParser.cpp"
		}
		rule_standard();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 921 "pix.g"
			
			importer->pushRule();
			
#line 3506 "PIXCfgParser.cpp"
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
#line 1715 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
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
			
#line 3538 "PIXCfgParser.cpp"
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

void PIXCfgParser::rule_extended() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IP:
		case AH:
		case EIGRP:
		case ESP:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
		case PIM:
		case PPTP:
		case SNP:
		case OBJECT:
		case ICMP6:
		case OBJECT_GROUP:
		{
			ip_protocols();
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 946 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 3580 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 947 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 3586 "PIXCfgParser.cpp"
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
#line 953 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 3656 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 957 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 3662 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 958 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 3668 "PIXCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case INT_CONST:
			case ECHO:
			case ALTERNATE_ADDRESS:
			case CONVERSION_ERROR:
			case ECHO_REPLY:
			case INFORMATION_REPLY:
			case INFORMATION_REQUEST:
			case MASK_REPLY:
			case MASK_REQUEST:
			case MOBILE_REDIRECT:
			case PARAMETER_PROBLEM:
			case REDIRECT:
			case ROUTER_ADVERTISEMENT:
			case ROUTER_SOLICITATION:
			case SOURCE_QUENCH:
			case TIME_EXCEEDED:
			case TIMESTAMP_REPLY:
			case TIMESTAMP_REQUEST:
			case TRACEROUTE:
			case UNREACHABLE:
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
			tcp_udp_rule_extended();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 966 "pix.g"
			
			*dbg << std::endl;
			
#line 3786 "PIXCfgParser.cpp"
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

void PIXCfgParser::rule_standard() {
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 931 "pix.g"
			
			importer->tmp_a = "0.0.0.0";
			importer->tmp_nm = "0.0.0.0";
			importer->SaveTmpAddrToSrc();
			
#line 3809 "PIXCfgParser.cpp"
		}
		hostaddr_expr();
		if ( inputState->guessing==0 ) {
#line 937 "pix.g"
			
			importer->SaveTmpAddrToDst();
			*dbg << "(dst) " << std::endl;
			
#line 3818 "PIXCfgParser.cpp"
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

void PIXCfgParser::hostaddr_expr() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  intf_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTRFACE:
		{
			match(INTRFACE);
			intf_name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 1207 "pix.g"
				
				importer->tmp_a = intf_name->getText();
				importer->tmp_nm = "interface";
				*dbg << "object " << intf_name->getText() << " ";
				
#line 3852 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		case OBJECT_GROUP:
		{
			{
			{
			switch ( LA(1)) {
			case OBJECT:
			{
				match(OBJECT);
				break;
			}
			case OBJECT_GROUP:
			{
				match(OBJECT_GROUP);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			name = LT(1);
			match(WORD);
			}
			if ( inputState->guessing==0 ) {
#line 1214 "pix.g"
				
				importer->tmp_a = name->getText();
				importer->tmp_nm = "";
				*dbg << "object " << name->getText() << " ";
				
#line 3888 "PIXCfgParser.cpp"
			}
			break;
		}
		case HOST:
		{
			{
			match(HOST);
			h = LT(1);
			match(IPV4);
			}
			if ( inputState->guessing==0 ) {
#line 1221 "pix.g"
				
				importer->tmp_a = h->getText();
				importer->tmp_nm = "255.255.255.255";
				*dbg << h->getText() << "/255.255.255.255";
				
#line 3906 "PIXCfgParser.cpp"
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
#line 1228 "pix.g"
				
				importer->tmp_a = a->getText();
				importer->tmp_nm = m->getText();
				*dbg << a->getText() << "/" << m->getText();
				
#line 3925 "PIXCfgParser.cpp"
			}
			break;
		}
		case ANY:
		{
			match(ANY);
			if ( inputState->guessing==0 ) {
#line 1235 "pix.g"
				
				importer->tmp_a = "0.0.0.0";
				importer->tmp_nm = "0.0.0.0";
				*dbg << "0.0.0.0/0.0.0.0";
				
#line 3939 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_12);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::ip_protocols() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IP:
		case AH:
		case EIGRP:
		case ESP:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
		case PIM:
		case PPTP:
		case SNP:
		case ICMP6:
		{
			{
			switch ( LA(1)) {
			case IP:
			case AH:
			case EIGRP:
			case ESP:
			case GRE:
			case IGMP:
			case IGRP:
			case IPINIP:
			case IPSEC:
			case NOS:
			case OSPF:
			case PCP:
			case PIM:
			case PPTP:
			case SNP:
			{
				ip_protocol_names();
				break;
			}
			case ICMP6:
			{
				match(ICMP6);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 1154 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 4020 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		case OBJECT_GROUP:
		{
			{
			{
			switch ( LA(1)) {
			case OBJECT:
			{
				match(OBJECT);
				break;
			}
			case OBJECT_GROUP:
			{
				match(OBJECT_GROUP);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			name = LT(1);
			match(WORD);
			}
			if ( inputState->guessing==0 ) {
#line 1160 "pix.g"
				
				importer->protocol = name->getText();
				*dbg << "protocol " << name->getText() << " ";
				
#line 4055 "PIXCfgParser.cpp"
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

void PIXCfgParser::time_range() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  tr_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(TIME_RANGE);
		tr_name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 1268 "pix.g"
			
			importer->time_range_name = tr_name->getText();
			*dbg << "time_range " << tr_name->getText() << " ";
			
#line 4089 "PIXCfgParser.cpp"
		}
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

void PIXCfgParser::fragments() {
	
	try {      // for error handling
		match(FRAGMENTS);
		if ( inputState->guessing==0 ) {
#line 1261 "pix.g"
			
			importer->fragments = true;
			*dbg << "fragments ";
			
#line 4112 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_15);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::log() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  level_int = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  log_interval = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  level_word = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
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
		{
		switch ( LA(1)) {
		case NEWLINE:
		case INT_CONST:
		{
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				level_int = LT(1);
				match(INT_CONST);
				{
				switch ( LA(1)) {
				case INTERVAL:
				{
					match(INTERVAL);
					log_interval = LT(1);
					match(INT_CONST);
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
			break;
		}
		case WORD:
		{
			level_word = LT(1);
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
#line 1249 "pix.g"
			
			importer->logging = true;
			if (level_int) importer->log_level = level_int->getText();
			if (level_word) importer->log_level = level_word->getText();
			if (log_interval) importer->log_interval = log_interval->getText();
			
			*dbg << "logging " << importer->log_level
			<< " " << importer->log_interval << std::endl;
			
#line 4216 "PIXCfgParser.cpp"
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

void PIXCfgParser::icmp_spec() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_code = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
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
#line 1170 "pix.g"
				
				importer->icmp_type = icmp_type->getText();
				importer->icmp_code = icmp_code->getText();
				importer->icmp_spec = "";
				*dbg << icmp_type->getText() << " "
				<< icmp_code->getText() << " ";
				
#line 4253 "PIXCfgParser.cpp"
			}
			break;
		}
		case ECHO:
		case ALTERNATE_ADDRESS:
		case CONVERSION_ERROR:
		case ECHO_REPLY:
		case INFORMATION_REPLY:
		case INFORMATION_REQUEST:
		case MASK_REPLY:
		case MASK_REQUEST:
		case MOBILE_REDIRECT:
		case PARAMETER_PROBLEM:
		case REDIRECT:
		case ROUTER_ADVERTISEMENT:
		case ROUTER_SOLICITATION:
		case SOURCE_QUENCH:
		case TIME_EXCEEDED:
		case TIMESTAMP_REPLY:
		case TIMESTAMP_REQUEST:
		case TRACEROUTE:
		case UNREACHABLE:
		{
			icmp_names();
			if ( inputState->guessing==0 ) {
#line 1179 "pix.g"
				
				importer->icmp_spec = LT(0)->getText();
				*dbg << LT(0)->getText() << " ";
				
#line 4284 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_16);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::tcp_udp_rule_extended() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  src_grp_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dst_addr_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
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
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 973 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << "protocol " << LT(0)->getText() << " ";
			
#line 4334 "PIXCfgParser.cpp"
		}
		hostaddr_expr();
		if ( inputState->guessing==0 ) {
#line 977 "pix.g"
			importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 4340 "PIXCfgParser.cpp"
		}
		{
		bool synPredMatched123 = false;
		if (((_tokenSet_13.member(LA(1))) && (_tokenSet_17.member(LA(2))))) {
			int _m123 = mark();
			synPredMatched123 = true;
			inputState->guessing++;
			try {
				{
				match(OBJECT_GROUP);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched123 = false;
			}
			rewind(_m123);
			inputState->guessing--;
		}
		if ( synPredMatched123 ) {
			{
			if (((LA(1) == OBJECT_GROUP) && (LA(2) == WORD))&&( importer->isKnownServiceGroupName(LT(2)->getText()) )) {
				match(OBJECT_GROUP);
				src_grp_name = LT(1);
				match(WORD);
				if ( inputState->guessing==0 ) {
#line 987 "pix.g"
					
					importer->src_port_spec = src_grp_name->getText();
					*dbg << "src port spec: "
					<< src_grp_name->getText() << std::endl;
					
#line 4372 "PIXCfgParser.cpp"
				}
				hostaddr_expr_1();
				if ( inputState->guessing==0 ) {
#line 995 "pix.g"
					
					importer->SaveTmpAddrToDst();
					*dbg << "(dst) ";
					
#line 4381 "PIXCfgParser.cpp"
				}
				acl_tcp_udp_dst_port_spec();
			}
			else if ((_tokenSet_13.member(LA(1))) && (_tokenSet_17.member(LA(2)))) {
				hostaddr_expr_2();
				if ( inputState->guessing==0 ) {
#line 1004 "pix.g"
					
					importer->SaveTmpAddrToDst();
					*dbg << "(dst) ";
					
#line 4393 "PIXCfgParser.cpp"
				}
				acl_tcp_udp_dst_port_spec();
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else if ((LA(1) == OBJECT) && (LA(2) == WORD)) {
			match(OBJECT);
			dst_addr_name = LT(1);
			match(WORD);
			{
			if ((_tokenSet_18.member(LA(1))) && (_tokenSet_19.member(LA(2)))) {
				acl_xoperator_dst();
			}
			else if ((_tokenSet_20.member(LA(1))) && (_tokenSet_21.member(LA(2)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			{
			if ((LA(1) == ESTABLISHED) && (_tokenSet_20.member(LA(2)))) {
				established();
			}
			else if ((_tokenSet_20.member(LA(1))) && (_tokenSet_22.member(LA(2)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			if ( inputState->guessing==0 ) {
#line 1013 "pix.g"
				
				// looks like "object foo" at this point can only be dest addr.
				// (judging by cli prompts on 8.3)
				importer->tmp_a = dst_addr_name->getText();
				importer->tmp_nm = "";
				importer->SaveTmpAddrToDst();
				*dbg << "dst addr object " << dst_addr_name->getText() << " ";
				
#line 4439 "PIXCfgParser.cpp"
			}
			acl_tcp_udp_dst_port_spec();
		}
		else if ((_tokenSet_23.member(LA(1))) && (_tokenSet_24.member(LA(2)))) {
			{
			switch ( LA(1)) {
			case RANGE:
			case P_EQ:
			case P_GT:
			case P_LT:
			case P_NEQ:
			{
				xoperator();
				if ( inputState->guessing==0 ) {
#line 1027 "pix.g"
					
					importer->SaveTmpPortToSrc();
					
#line 4458 "PIXCfgParser.cpp"
				}
				break;
			}
			case IPV4:
			case OBJECT:
			case HOST:
			case OBJECT_GROUP:
			case INTRFACE:
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
			hostaddr_expr_3();
			if ( inputState->guessing==0 ) {
#line 1031 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 4481 "PIXCfgParser.cpp"
			}
			acl_tcp_udp_dst_port_spec();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
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

void PIXCfgParser::hostaddr_expr_1() {
	
	try {      // for error handling
		hostaddr_expr();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_20);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::acl_tcp_udp_dst_port_spec() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  dst_port_group_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dst_port_obj_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case OBJECT_GROUP:
		{
			{
			match(OBJECT_GROUP);
			dst_port_group_name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 1049 "pix.g"
				
				importer->dst_port_spec = dst_port_group_name->getText();
				*dbg << "dst port spec: "
				<< dst_port_group_name->getText() << std::endl;
				
#line 4593 "PIXCfgParser.cpp"
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
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			dst_port_obj_name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 1059 "pix.g"
				
				importer->dst_port_spec = dst_port_obj_name->getText();
				*dbg << "dst addr object " << dst_port_obj_name->getText()
				<< std::endl;
				
#line 4631 "PIXCfgParser.cpp"
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
			break;
		}
		case NEWLINE:
		case RANGE:
		case P_EQ:
		case P_GT:
		case P_LT:
		case P_NEQ:
		case ESTABLISHED:
		case LOG:
		case LOG_INPUT:
		case FRAGMENTS:
		case TIME_RANGE:
		{
			{
			switch ( LA(1)) {
			case RANGE:
			case P_EQ:
			case P_GT:
			case P_LT:
			case P_NEQ:
			{
				acl_xoperator_dst();
				break;
			}
			case NEWLINE:
			case ESTABLISHED:
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
			recover(ex,_tokenSet_16);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::hostaddr_expr_2() {
	
	try {      // for error handling
		hostaddr_expr();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_20);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::acl_xoperator_dst() {
	
	try {      // for error handling
		xoperator();
		if ( inputState->guessing==0 ) {
#line 1074 "pix.g"
			
			importer->SaveTmpPortToDst();
			
#line 4758 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_20);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::established() {
	
	try {      // for error handling
		match(ESTABLISHED);
		if ( inputState->guessing==0 ) {
#line 1143 "pix.g"
			
			importer->established = true;
			*dbg << "established ";
			
#line 4781 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_20);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::hostaddr_expr_3() {
	
	try {      // for error handling
		hostaddr_expr();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_20);
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
#line 1084 "pix.g"
			
			importer->tmp_port_op = LT(0)->getText();
			*dbg << LT(0)->getText() << " ";
			
#line 4846 "PIXCfgParser.cpp"
		}
		port_spec();
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

void PIXCfgParser::port_range() {
	
	try {      // for error handling
		match(RANGE);
		pair_of_ports_spec();
		if ( inputState->guessing==0 ) {
#line 1099 "pix.g"
			
			importer->tmp_port_op = "range";
			*dbg << "range " << importer->tmp_port_spec;
			
#line 4871 "PIXCfgParser.cpp"
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

void PIXCfgParser::port_spec() {
	
	try {      // for error handling
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1092 "pix.g"
			
			importer->tmp_port_spec = std::string(" ") + importer->tmp_port_spec_2;
			*dbg << LT(0)->getText() << " " << importer->tmp_port_spec;
			
#line 4894 "PIXCfgParser.cpp"
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

void PIXCfgParser::tcp_udp_port_spec() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case PPTP:
		case HOSTNAME:
		case ECHO:
		case RIP:
		case SSH:
		case TELNET:
		{
			tcp_udp_port_names();
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
		if ( inputState->guessing==0 ) {
#line 1123 "pix.g"
			
			importer->tmp_port_spec_2 = LT(0)->getText();
			
#line 4943 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_25);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::pair_of_ports_spec() {
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 1106 "pix.g"
			
			importer->tmp_port_spec_2 = "";
			
#line 4964 "PIXCfgParser.cpp"
		}
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1110 "pix.g"
			
			importer->tmp_port_spec += importer->tmp_port_spec_2;
			
#line 4972 "PIXCfgParser.cpp"
		}
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1114 "pix.g"
			
			importer->tmp_port_spec += " ";
			importer->tmp_port_spec += importer->tmp_port_spec_2;
			
#line 4981 "PIXCfgParser.cpp"
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

void PIXCfgParser::tcp_udp_port_names() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case ECHO:
		{
			match(ECHO);
			break;
		}
		case HOSTNAME:
		{
			match(HOSTNAME);
			break;
		}
		case PPTP:
		{
			match(PPTP);
			break;
		}
		case RIP:
		{
			match(RIP);
			break;
		}
		case SSH:
		{
			match(SSH);
			break;
		}
		case TELNET:
		{
			match(TELNET);
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
			recover(ex,_tokenSet_25);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::interface_command_6() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		in = LT(1);
		match(WORD);
		pix6_interface_hw_speed();
		if ( inputState->guessing==0 ) {
#line 1327 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newInterface( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE: " << in->getText() << std::endl;
			
#line 5061 "PIXCfgParser.cpp"
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

void PIXCfgParser::interface_command_7() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1335 "pix.g"
	bool have_interface_parameters = false;
#line 5078 "PIXCfgParser.cpp"
	
	try {      // for error handling
		in = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 1336 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newInterface( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE: " << in->getText() << std::endl;
			
#line 5092 "PIXCfgParser.cpp"
		}
		{
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_26.member(LA(1)))) {
				interface_parameters();
				if ( inputState->guessing==0 ) {
#line 1343 "pix.g"
					have_interface_parameters = true;
#line 5102 "PIXCfgParser.cpp"
				}
			}
			else {
				goto _loop186;
			}
			
		}
		_loop186:;
		} // ( ... )*
		{
		switch ( LA(1)) {
		case LINE_COMMENT:
		{
			match(LINE_COMMENT);
			break;
		}
		case EXIT:
		{
			match(EXIT);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 1345 "pix.g"
			
			if ( ! have_interface_parameters )
			{
			importer->ignoreCurrentInterface();
			*dbg<< LT(1)->getLine() << ":"
			<< " EMPTY INTERFACE " << std::endl;
			}
			
#line 5140 "PIXCfgParser.cpp"
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

void PIXCfgParser::pix6_interface_hw_speed() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case AUI:
		{
			match(AUI);
			break;
		}
		case AUTO:
		{
			match(AUTO);
			break;
		}
		case BNC:
		{
			match(BNC);
			break;
		}
		case INT_CONST:
		{
			{
			match(INT_CONST);
			{
			switch ( LA(1)) {
			case FULL:
			{
				match(FULL);
				break;
			}
			case BASET:
			{
				match(BASET);
				break;
			}
			case BASETX:
			{
				match(BASETX);
				break;
			}
			case AUTO:
			{
				match(AUTO);
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::interface_parameters() {
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 1373 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 5234 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case IP:
		{
			intf_address();
			break;
		}
		case VLAN:
		{
			vlan_interface();
			break;
		}
		case SEC_LEVEL:
		{
			sec_level();
			break;
		}
		case NAMEIF:
		{
			nameif();
			break;
		}
		case DESCRIPTION:
		{
			interface_description();
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
		case NO:
		{
			interface_no_commands();
			break;
		}
		case IGMP:
		case OSPF:
		case PIM:
		case RIP:
		case SPEED:
		case DUPLEX:
		case DDNS:
		case FORWARD:
		case DELAY:
		case HOLD_TIME:
		case IPV6_C:
		case MAC_ADDRESS:
		case MULTICAST:
		case PPPOE:
		{
			unsupported_interface_commands();
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
			recover(ex,_tokenSet_27);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::intf_address() {
	
	try {      // for error handling
		match(IP);
		match(ADDRESS);
		{
		switch ( LA(1)) {
		case WORD:
		{
			v6_ip_address();
			break;
		}
		case IPV4:
		case DHCP:
		{
			v7_ip_address();
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
			recover(ex,_tokenSet_11);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::vlan_interface() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  vlan_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(VLAN);
		vlan_id = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1399 "pix.g"
			
			importer->setInterfaceVlanId(vlan_id->getText());
			*dbg << " VLAN: " << vlan_id->getText() << std::endl;
			
#line 5362 "PIXCfgParser.cpp"
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

void PIXCfgParser::sec_level() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  sec_level = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SEC_LEVEL);
		sec_level = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1451 "pix.g"
			
			importer->setInterfaceSecurityLevel(sec_level->getText());
			*dbg << "SEC_LEVEL: " << sec_level->getText() << std::endl;
			
#line 5388 "PIXCfgParser.cpp"
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

void PIXCfgParser::nameif() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  p_intf = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  intf_label = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  sec_level = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(NAMEIF);
		p_intf = LT(1);
		match(WORD);
		{
		switch ( LA(1)) {
		case WORD:
		{
			intf_label = LT(1);
			match(WORD);
			sec_level = LT(1);
			match(WORD);
			break;
		}
		case NEWLINE:
		{
			{
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
#line 1474 "pix.g"
			
			std::string label = (intf_label) ? intf_label->getText() : "";
			std::string seclevel = (sec_level) ? sec_level->getText() : "";
			importer->setInterfaceParametes(p_intf->getText(), label, seclevel);
			*dbg << " NAMEIF: "
			<< p_intf->getText() << label << seclevel << std::endl;
			
#line 5441 "PIXCfgParser.cpp"
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

void PIXCfgParser::interface_description() {
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 1486 "pix.g"
			
			*dbg << LT(1)->getLine() << ":";
			std::string descr;
			while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
			{
			descr += LT(1)->getText() + " ";
			consume();
			}
			importer->setInterfaceComment( descr );
			*dbg << " DESCRIPTION " << descr << std::endl;
			//consumeUntil(NEWLINE);
			
#line 5472 "PIXCfgParser.cpp"
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

void PIXCfgParser::switchport() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  vlan_num = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SWITCHPORT);
		match(ACCESS);
		match(VLAN);
		vlan_num = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1598 "pix.g"
			
			importer->addMessageToLog("Switch port vlan " + vlan_num->getText());
			*dbg << "Switch port vlan " <<  vlan_num->getText() << std::endl;
			
#line 5500 "PIXCfgParser.cpp"
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

void PIXCfgParser::shutdown() {
	
	try {      // for error handling
		match(SHUTDOWN);
		if ( inputState->guessing==0 ) {
#line 1501 "pix.g"
			
			importer->ignoreCurrentInterface();
			*dbg<< LT(1)->getLine() << ":"
			<< " INTERFACE SHUTDOWN " << std::endl;
			
#line 5524 "PIXCfgParser.cpp"
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

void PIXCfgParser::interface_no_commands() {
	
	try {      // for error handling
		match(NO);
		{
		switch ( LA(1)) {
		case NAMEIF:
		{
			match(NAMEIF);
			break;
		}
		case IP:
		{
			match(IP);
			break;
		}
		case SEC_LEVEL:
		{
			match(SEC_LEVEL);
			break;
		}
		case SHUTDOWN:
		{
			match(SHUTDOWN);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 1443 "pix.g"
			
			*dbg << " INTERFACE \"NO\" COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 5576 "PIXCfgParser.cpp"
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

void PIXCfgParser::unsupported_interface_commands() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case SPEED:
		{
			match(SPEED);
			break;
		}
		case DUPLEX:
		{
			match(DUPLEX);
			break;
		}
		case DDNS:
		{
			match(DDNS);
			break;
		}
		case FORWARD:
		{
			match(FORWARD);
			break;
		}
		case DELAY:
		{
			match(DELAY);
			break;
		}
		case HOLD_TIME:
		{
			match(HOLD_TIME);
			break;
		}
		case IGMP:
		{
			match(IGMP);
			break;
		}
		case IPV6_C:
		{
			match(IPV6_C);
			break;
		}
		case MAC_ADDRESS:
		{
			match(MAC_ADDRESS);
			break;
		}
		case MULTICAST:
		{
			match(MULTICAST);
			break;
		}
		case OSPF:
		{
			match(OSPF);
			break;
		}
		case PIM:
		{
			match(PIM);
			break;
		}
		case PPPOE:
		{
			match(PPPOE);
			break;
		}
		case RIP:
		{
			match(RIP);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 1435 "pix.g"
			
			*dbg << " UNSUPPORTED INTERFACE COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 5677 "PIXCfgParser.cpp"
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

void PIXCfgParser::v6_ip_address() {
	
	try {      // for error handling
		if ((LA(1) == WORD) && (LA(2) == DHCP)) {
			v6_dhcp_address();
		}
		else if ((LA(1) == WORD) && (LA(2) == IPV4)) {
			v6_static_address();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
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

void PIXCfgParser::v7_ip_address() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case DHCP:
		{
			v7_dhcp_address();
			break;
		}
		case IPV4:
		{
			v7_static_address();
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
			recover(ex,_tokenSet_11);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::v6_dhcp_address() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  lbl = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dhcp = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		lbl = LT(1);
		match(WORD);
		dhcp = LT(1);
		match(DHCP);
		if ( inputState->guessing==0 ) {
#line 1536 "pix.g"
			
			std::string label = lbl->getText();
			std::string addr = dhcp->getText();
			importer->addInterfaceAddress(label, addr, "");
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << std::endl;
			// there can be some other parameters after "dhcp", such as "setroute", "retry" etc.
			// which we do not support
			consumeUntil(NEWLINE);
			
#line 5765 "PIXCfgParser.cpp"
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

void PIXCfgParser::v6_static_address() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  lbl = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		lbl = LT(1);
		match(WORD);
		a = LT(1);
		match(IPV4);
		m = LT(1);
		match(IPV4);
		if ( inputState->guessing==0 ) {
#line 1549 "pix.g"
			
			std::string label = lbl->getText();
			std::string addr = a->getText();
			std::string netm = m->getText();
			importer->addInterfaceAddress(label, addr, netm);
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << "/" << netm << std::endl;
			// in case there are some other parameters after address and netmask
			consumeUntil(NEWLINE);
			
#line 5802 "PIXCfgParser.cpp"
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

void PIXCfgParser::v7_dhcp_address() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  dhcp = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		dhcp = LT(1);
		match(DHCP);
		if ( inputState->guessing==0 ) {
#line 1566 "pix.g"
			
			std::string addr = dhcp->getText();
			importer->addInterfaceAddress(addr, "");
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << std::endl;
			consumeUntil(NEWLINE);
			
#line 5830 "PIXCfgParser.cpp"
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

void PIXCfgParser::v7_static_address() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		a = LT(1);
		match(IPV4);
		m = LT(1);
		match(IPV4);
		{
		switch ( LA(1)) {
		case STANDBY:
		{
			s = LT(1);
			match(STANDBY);
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
#line 1577 "pix.g"
			
			std::string addr = a->getText();
			std::string netm = m->getText();
			importer->addInterfaceAddress(addr, netm);
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << "/" << netm << std::endl;
			// there can be other parameters after address/netmask pair, such as "standby"
			// We do not parse them yet.
			if (s)
			{
			importer->addMessageToLog("Parser warning: failover IP detected. "
			"Failover is not supported by import "
			"at this time");
			}
			consumeUntil(NEWLINE);
			
#line 5889 "PIXCfgParser.cpp"
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

void PIXCfgParser::icmp_types_for_icmp_command() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case INT_CONST:
		{
			match(INT_CONST);
			if ( inputState->guessing==0 ) {
#line 1694 "pix.g"
				
				importer->icmp_type = LT(0)->getText();
				importer->icmp_code = "0";
				importer->icmp_spec = "";
				
#line 5916 "PIXCfgParser.cpp"
			}
			break;
		}
		case ECHO:
		case ECHO_REPLY:
		case TIME_EXCEEDED:
		case UNREACHABLE:
		{
			{
			switch ( LA(1)) {
			case ECHO:
			{
				match(ECHO);
				break;
			}
			case ECHO_REPLY:
			{
				match(ECHO_REPLY);
				break;
			}
			case TIME_EXCEEDED:
			{
				match(TIME_EXCEEDED);
				break;
			}
			case UNREACHABLE:
			{
				match(UNREACHABLE);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 1701 "pix.g"
				
				importer->icmp_type = "";
				importer->icmp_code = "0";
				importer->icmp_spec = LT(0)->getText();
				
#line 5960 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_28);
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
	"\"quit\"",
	"\"ip\"",
	"\"community-list\"",
	"\"timeout\"",
	"\"names\"",
	"\"name\"",
	"IPV4",
	"IPV6",
	"WORD",
	"\"ah\"",
	"\"eigrp\"",
	"\"esp\"",
	"\"gre\"",
	"\"igmp\"",
	"\"igrp\"",
	"\"ipinip\"",
	"\"ipsec\"",
	"\"nos\"",
	"\"ospf\"",
	"\"pcp\"",
	"\"pim\"",
	"\"pptp\"",
	"\"snp\"",
	"OBJECT",
	"\"network\"",
	"\"nat\"",
	"\"description\"",
	"\"host\"",
	"\"range\"",
	"\"subnet\"",
	"\"service\"",
	"\"icmp\"",
	"INT_CONST",
	"\"icmp6\"",
	"\"tcp\"",
	"\"udp\"",
	"\"source\"",
	"\"destination\"",
	"OBJECT_GROUP",
	"\"group-object\"",
	"\"network-object\"",
	"\"protocol\"",
	"\"protocol-object\"",
	"\"icmp-object\"",
	"\"icmp-type\"",
	"\"tcp-udp\"",
	"\"service-object\"",
	"\"port-object\"",
	"\"crypto\"",
	"\"no\"",
	"\"certificate\"",
	"\"PIX\"",
	"\"ASA\"",
	"\"Version\"",
	"NUMBER",
	"\"hostname\"",
	"STRING",
	"\"access-list\"",
	"\"extended\"",
	"\"permit\"",
	"\"deny\"",
	"\"standard\"",
	"\"eq\"",
	"\"gt\"",
	"\"lt\"",
	"\"neq\"",
	"\"echo\"",
	"\"rip\"",
	"\"ssh\"",
	"\"telnet\"",
	"\"established\"",
	"\"alternate-address\"",
	"\"conversion-error\"",
	"\"echo-reply\"",
	"\"information-reply\"",
	"\"information-request\"",
	"\"mask-reply\"",
	"\"mask-request\"",
	"\"mobile-redirect\"",
	"\"parameter-problem\"",
	"\"redirect\"",
	"\"router-advertisement\"",
	"\"router-solicitation\"",
	"\"source-quench\"",
	"\"time-exceeded\"",
	"\"timestamp-reply\"",
	"\"timestamp-request\"",
	"\"traceroute\"",
	"\"unreachable\"",
	"\"interface\"",
	"\"any\"",
	"\"log\"",
	"\"log-input\"",
	"\"interval\"",
	"\"fragments\"",
	"\"time-range\"",
	"\"controller\"",
	"LINE_COMMENT",
	"\"exit\"",
	"\"aui\"",
	"\"auto\"",
	"\"bnc\"",
	"\"full\"",
	"\"baseT\"",
	"\"baseTX\"",
	"\"nameif\"",
	"\"vlan\"",
	"\"speed\"",
	"\"duplex\"",
	"\"ddns\"",
	"\"forward\"",
	"\"delay\"",
	"\"hold-time\"",
	"\"ipv6\"",
	"\"mac-address\"",
	"\"multicast\"",
	"PPPOE",
	"\"security-level\"",
	"\"shutdown\"",
	"\"address\"",
	"\"dhcp\"",
	"\"standby\"",
	"\"switchport\"",
	"\"access\"",
	"\"remark\"",
	"\"access-group\"",
	"COLON_COMMENT",
	"\"secondary\"",
	"\"setroute\"",
	"Whitespace",
	"HEX_CONST",
	"NEG_INT_CONST",
	"DIGIT",
	"HEXDIGIT",
	"NUMBER_ADDRESS_OR_WORD",
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
	"EXLAMATION",
	0
};

const unsigned long PIXCfgParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_0(_tokenSet_0_data_,6);
const unsigned long PIXCfgParser::_tokenSet_1_data_[] = { 268445554UL, 1407191056UL, 1073743360UL, 16608UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "icmp" 
// OBJECT_GROUP "crypto" "no" "certificate" "PIX" "ASA" "hostname" "access-list" 
// "ssh" "telnet" "interface" "controller" LINE_COMMENT "exit" "nameif" 
// "access-group" COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_1(_tokenSet_1_data_,12);
const unsigned long PIXCfgParser::_tokenSet_2_data_[] = { 0UL, 32UL, 0UL, 1792UL, 0UL, 0UL, 0UL, 0UL };
// INT_CONST "aui" "auto" "bnc" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long PIXCfgParser::_tokenSet_3_data_[] = { 536854592UL, 2512UL, 1UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" "ospf" 
// "pcp" "pim" "pptp" "snp" OBJECT "icmp" "icmp6" "tcp" "udp" OBJECT_GROUP 
// "permit" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long PIXCfgParser::_tokenSet_4_data_[] = { 536854592UL, 2512UL, 2UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" "ospf" 
// "pcp" "pim" "pptp" "snp" OBJECT "icmp" "icmp6" "tcp" "udp" OBJECT_GROUP 
// "deny" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long PIXCfgParser::_tokenSet_5_data_[] = { 2147483648UL, 1576960UL, 0UL, 0UL, 0UL, 0UL };
// "description" "group-object" "service-object" "port-object" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_5(_tokenSet_5_data_,6);
const unsigned long PIXCfgParser::_tokenSet_6_data_[] = { 268447602UL, 1407191057UL, 3221227008UL, 16608UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" IPV4 WORD OBJECT "host" 
// "icmp" OBJECT_GROUP "crypto" "no" "certificate" "PIX" "ASA" "hostname" 
// "access-list" "ssh" "telnet" "interface" "any" "controller" LINE_COMMENT 
// "exit" "nameif" "access-group" COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_6(_tokenSet_6_data_,12);
const unsigned long PIXCfgParser::_tokenSet_7_data_[] = { 268419136UL, 32UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" "ospf" 
// "pcp" "pim" "pptp" "snp" INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_7(_tokenSet_7_data_,6);
const unsigned long PIXCfgParser::_tokenSet_8_data_[] = { 268445554UL, 1407191056UL, 1073743360UL, 16635UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "icmp" 
// OBJECT_GROUP "crypto" "no" "certificate" "PIX" "ASA" "hostname" "access-list" 
// "ssh" "telnet" "interface" "log" "log-input" "fragments" "time-range" 
// "controller" LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_8(_tokenSet_8_data_,12);
const unsigned long PIXCfgParser::_tokenSet_9_data_[] = { 268445554UL, 1407192082UL, 1073743480UL, 16608UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "range" 
// "icmp" "destination" OBJECT_GROUP "crypto" "no" "certificate" "PIX" 
// "ASA" "hostname" "access-list" "eq" "gt" "lt" "neq" "ssh" "telnet" "interface" 
// "controller" LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_9(_tokenSet_9_data_,12);
const unsigned long PIXCfgParser::_tokenSet_10_data_[] = { 268447602UL, 1407192083UL, 3221229176UL, 16635UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" IPV4 WORD OBJECT "host" 
// "range" "icmp" "destination" OBJECT_GROUP "crypto" "no" "certificate" 
// "PIX" "ASA" "hostname" "access-list" "eq" "gt" "lt" "neq" "ssh" "telnet" 
// "established" "interface" "any" "log" "log-input" "fragments" "time-range" 
// "controller" LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_10(_tokenSet_10_data_,12);
const unsigned long PIXCfgParser::_tokenSet_11_data_[] = { 16UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_11(_tokenSet_11_data_,6);
const unsigned long PIXCfgParser::_tokenSet_12_data_[] = { 268445712UL, 2083UL, 4294965496UL, 27UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD OBJECT "host" "range" INT_CONST OBJECT_GROUP "eq" 
// "gt" "lt" "neq" "echo" "established" "alternate-address" "conversion-error" 
// "echo-reply" "information-reply" "information-request" "mask-reply" 
// "mask-request" "mobile-redirect" "parameter-problem" "redirect" "router-advertisement" 
// "router-solicitation" "source-quench" "time-exceeded" "timestamp-reply" 
// "timestamp-request" "traceroute" "unreachable" "interface" "any" "log" 
// "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_12(_tokenSet_12_data_,8);
const unsigned long PIXCfgParser::_tokenSet_13_data_[] = { 268437504UL, 2049UL, 3221225472UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IPV4 OBJECT "host" OBJECT_GROUP "interface" "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_13(_tokenSet_13_data_,8);
const unsigned long PIXCfgParser::_tokenSet_14_data_[] = { 16UL, 0UL, 0UL, 11UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_14(_tokenSet_14_data_,8);
const unsigned long PIXCfgParser::_tokenSet_15_data_[] = { 16UL, 0UL, 0UL, 3UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_15(_tokenSet_15_data_,8);
const unsigned long PIXCfgParser::_tokenSet_16_data_[] = { 16UL, 0UL, 0UL, 27UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_16(_tokenSet_16_data_,8);
const unsigned long PIXCfgParser::_tokenSet_17_data_[] = { 268445712UL, 2050UL, 2168UL, 27UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD OBJECT "range" OBJECT_GROUP "eq" "gt" "lt" "neq" "established" 
// "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_17(_tokenSet_17_data_,8);
const unsigned long PIXCfgParser::_tokenSet_18_data_[] = { 0UL, 2UL, 120UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "range" "eq" "gt" "lt" "neq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_18(_tokenSet_18_data_,8);
const unsigned long PIXCfgParser::_tokenSet_19_data_[] = { 67117056UL, 268435488UL, 1920UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD "pptp" INT_CONST "hostname" "echo" "rip" "ssh" "telnet" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_19(_tokenSet_19_data_,8);
const unsigned long PIXCfgParser::_tokenSet_20_data_[] = { 268435472UL, 2050UL, 2168UL, 27UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE OBJECT "range" OBJECT_GROUP "eq" "gt" "lt" "neq" "established" 
// "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_20(_tokenSet_20_data_,8);
const unsigned long PIXCfgParser::_tokenSet_21_data_[] = { 335554418UL, 1407191090UL, 1073745912UL, 16635UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD "pptp" OBJECT 
// "range" "icmp" INT_CONST OBJECT_GROUP "crypto" "no" "certificate" "PIX" 
// "ASA" "hostname" "access-list" "eq" "gt" "lt" "neq" "echo" "rip" "ssh" 
// "telnet" "established" "interface" "log" "log-input" "fragments" "time-range" 
// "controller" LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_21(_tokenSet_21_data_,12);
const unsigned long PIXCfgParser::_tokenSet_22_data_[] = { 335554418UL, 1407191088UL, 1073743744UL, 16635UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD "pptp" OBJECT 
// "icmp" INT_CONST OBJECT_GROUP "crypto" "no" "certificate" "PIX" "ASA" 
// "hostname" "access-list" "echo" "rip" "ssh" "telnet" "interface" "log" 
// "log-input" "fragments" "time-range" "controller" LINE_COMMENT "exit" 
// "nameif" "access-group" COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_22(_tokenSet_22_data_,12);
const unsigned long PIXCfgParser::_tokenSet_23_data_[] = { 268437504UL, 2051UL, 3221225592UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IPV4 OBJECT "host" "range" OBJECT_GROUP "eq" "gt" "lt" "neq" "interface" 
// "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_23(_tokenSet_23_data_,8);
const unsigned long PIXCfgParser::_tokenSet_24_data_[] = { 335554576UL, 268437538UL, 4088UL, 27UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD "pptp" OBJECT "range" INT_CONST OBJECT_GROUP "hostname" 
// "eq" "gt" "lt" "neq" "echo" "rip" "ssh" "telnet" "established" "log" 
// "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_24(_tokenSet_24_data_,8);
const unsigned long PIXCfgParser::_tokenSet_25_data_[] = { 335556466UL, 1407192115UL, 3221229560UL, 16635UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" IPV4 WORD "pptp" OBJECT 
// "host" "range" "icmp" INT_CONST "destination" OBJECT_GROUP "crypto" 
// "no" "certificate" "PIX" "ASA" "hostname" "access-list" "eq" "gt" "lt" 
// "neq" "echo" "rip" "ssh" "telnet" "established" "interface" "any" "log" 
// "log-input" "fragments" "time-range" "controller" LINE_COMMENT "exit" 
// "nameif" "access-group" COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_25(_tokenSet_25_data_,12);
const unsigned long PIXCfgParser::_tokenSet_26_data_[] = { 2189688896UL, 4194304UL, 256UL, 2415902720UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "igmp" "ospf" "pim" "description" "no" "rip" "nameif" "vlan" "speed" 
// "duplex" "ddns" "forward" "delay" "hold-time" "ipv6" "mac-address" "multicast" 
// PPPOE "security-level" "shutdown" "switchport" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_26(_tokenSet_26_data_,8);
const unsigned long PIXCfgParser::_tokenSet_27_data_[] = { 2189688896UL, 4194304UL, 256UL, 2415902912UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "igmp" "ospf" "pim" "description" "no" "rip" LINE_COMMENT "exit" 
// "nameif" "vlan" "speed" "duplex" "ddns" "forward" "delay" "hold-time" 
// "ipv6" "mac-address" "multicast" PPPOE "security-level" "shutdown" "switchport" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_27(_tokenSet_27_data_,8);
const unsigned long PIXCfgParser::_tokenSet_28_data_[] = { 8192UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_28(_tokenSet_28_data_,6);



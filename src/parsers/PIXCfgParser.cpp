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
			case FWSM_WORD:
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
			case HTTP:
			{
				http_command();
				break;
			}
			case ICMP:
			{
				icmp_top_level_command();
				break;
			}
			case NAT:
			{
				nat_top_level_command();
				break;
			}
			case GLOBAL:
			{
				global_top_level_command();
				break;
			}
			case STATIC:
			{
				static_top_level_command();
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
			case DNS:
			{
				dns_command();
				break;
			}
			case SERVICE:
			{
				service_top_level_command();
				break;
			}
			case PIM:
			{
				pim_top_level_command();
				break;
			}
			case NETWORK:
			{
				network_top_level_command();
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
				else if ((LA(1) == IP) && (LA(2) == ADDRESS)) {
					intf_address();
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
#line 874 "pix.g"
	std::string platform;
#line 288 "PIXCfgParser.cpp"
	
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
		case FWSM_WORD:
		{
			match(FWSM_WORD);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 878 "pix.g"
			
			platform = LT(0)->getText();
			
#line 319 "PIXCfgParser.cpp"
		}
		match(VERSION_WORD_CAP);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 882 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setDiscoveredPlatform(platform);
			importer->setDiscoveredVersion(LT(0)->getText());
			*dbg << "VERSION " << LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 332 "PIXCfgParser.cpp"
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
#line 893 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			// we get host name in the import wizard
			//importer->setHostName( LT(0)->getText() );
			*dbg << "HOSTNAME " << "LT0=" << LT(0)->getText() << std::endl;
			
#line 375 "PIXCfgParser.cpp"
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
#line 203 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 398 "PIXCfgParser.cpp"
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
#line 814 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 421 "PIXCfgParser.cpp"
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
#line 1474 "pix.g"
	
	std::string intf_name, intf_label, sec_level;
	
#line 466 "PIXCfgParser.cpp"
	
	try {      // for error handling
		match(NAMEIF);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 1479 "pix.g"
			intf_name = LT(0)->getText();
#line 474 "PIXCfgParser.cpp"
		}
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1480 "pix.g"
			intf_label = LT(0)->getText();
#line 480 "PIXCfgParser.cpp"
		}
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 1481 "pix.g"
			sec_level = LT(0)->getText();
#line 486 "PIXCfgParser.cpp"
		}
		if ( inputState->guessing==0 ) {
#line 1482 "pix.g"
			
			importer->setInterfaceParametes(intf_name, intf_label, sec_level);
			*dbg << " NAMEIF: "
			<< intf_name << " "
			<< intf_label << " "
			<< sec_level << std::endl;
			
#line 497 "PIXCfgParser.cpp"
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

void PIXCfgParser::intf_address() {
	
	try {      // for error handling
		match(IP);
		match(ADDRESS);
		{
		switch ( LA(1)) {
		case WORD:
		case OUTSIDE:
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
#line 1386 "pix.g"
			
			importer->clearCurrentInterface();
			consumeUntil(NEWLINE);
			
#line 556 "PIXCfgParser.cpp"
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
#line 904 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newUnidirRuleSet(
			name->getText(), libfwbuilder::Policy::TYPENAME );
			*dbg << name->getLine() << ":"
			<< " ACL ext " << name->getText() << std::endl;
			
#line 586 "PIXCfgParser.cpp"
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
#line 927 "pix.g"
			
			*dbg << LT(0)->getLine() << ":"
			<< " ACL line end" << std::endl << std::endl;
			
#line 630 "PIXCfgParser.cpp"
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
	
	try {      // for error handling
		match(SSH);
		if ( inputState->guessing==0 ) {
#line 1732 "pix.g"
			
			importer->clear();
			
#line 652 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case TIMEOUT:
		{
			{
			match(TIMEOUT);
			match(INT_CONST);
			}
			if ( inputState->guessing==0 ) {
#line 1737 "pix.g"
				
				// set ssh timeout here
				
#line 667 "PIXCfgParser.cpp"
			}
			break;
		}
		case SCOPY:
		{
			match(SCOPY);
			break;
		}
		case VERSION_WORD_LOW:
		{
			match(VERSION_WORD_LOW);
			match(INT_CONST);
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
			if ( inputState->guessing==0 ) {
#line 1747 "pix.g"
				
				importer->SaveTmpAddrToSrc();
				
#line 696 "PIXCfgParser.cpp"
			}
			interface_label();
			}
			if ( inputState->guessing==0 ) {
#line 1752 "pix.g"
				
				std::string intf_label = LT(0)->getText();
				std::string acl_name = "ssh_commands_" + intf_label;
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->newUnidirRuleSet(acl_name, libfwbuilder::Policy::TYPENAME );
				importer->newPolicyRule();
				importer->action = "permit";
				importer->setDstSelf();
				importer->protocol = "tcp";
				importer->dst_port_op = "eq";
				importer->dst_port_spec = "ssh";
				importer->setInterfaceAndDirectionForRuleSet(
				acl_name, intf_label, "in" );
				importer->pushRule();
				*dbg << std::endl;
				
#line 718 "PIXCfgParser.cpp"
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
	
	try {      // for error handling
		match(TELNET);
		if ( inputState->guessing==0 ) {
#line 1772 "pix.g"
			
			importer->clear();
			
#line 748 "PIXCfgParser.cpp"
		}
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
			if ( inputState->guessing==0 ) {
#line 1779 "pix.g"
				
				importer->SaveTmpAddrToSrc();
				
#line 774 "PIXCfgParser.cpp"
			}
			interface_label();
			}
			if ( inputState->guessing==0 ) {
#line 1784 "pix.g"
				
				std::string intf_label = LT(0)->getText();
				std::string acl_name = "telnet_commands_" + intf_label;
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->newUnidirRuleSet(acl_name, libfwbuilder::Policy::TYPENAME );
				importer->newPolicyRule();
				importer->action = "permit";
				importer->setDstSelf();
				importer->protocol = "tcp";
				importer->dst_port_op = "eq";
				importer->dst_port_spec = "telnet";
				importer->setInterfaceAndDirectionForRuleSet(
				acl_name, intf_label, "in" );
				importer->pushRule();
				*dbg << std::endl;
				
#line 796 "PIXCfgParser.cpp"
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

void PIXCfgParser::http_command() {
	
	try {      // for error handling
		match(HTTP);
		if ( inputState->guessing==0 ) {
#line 1806 "pix.g"
			
			importer->clear();
			
#line 826 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case REDIRECT:
		case AUTHENTICATION_CERTIFICATE:
		case SERVER:
		{
			{
			switch ( LA(1)) {
			case AUTHENTICATION_CERTIFICATE:
			{
				match(AUTHENTICATION_CERTIFICATE);
				break;
			}
			case REDIRECT:
			{
				match(REDIRECT);
				break;
			}
			case SERVER:
			{
				match(SERVER);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 1811 "pix.g"
				
				consumeUntil(NEWLINE);
				
#line 862 "PIXCfgParser.cpp"
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
			if ( inputState->guessing==0 ) {
#line 1817 "pix.g"
				
				importer->SaveTmpAddrToSrc();
				
#line 880 "PIXCfgParser.cpp"
			}
			interface_label();
			}
			if ( inputState->guessing==0 ) {
#line 1822 "pix.g"
				
				std::string intf_label = LT(0)->getText();
				std::string acl_name = "http_commands_" + intf_label;
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->newUnidirRuleSet(acl_name,
				libfwbuilder::Policy::TYPENAME );
				importer->newPolicyRule();
				importer->action = "permit";
				importer->setDstSelf();
				importer->protocol = "tcp";
				importer->dst_port_op = "eq";
				importer->dst_port_spec = "www";
				importer->setInterfaceAndDirectionForRuleSet(
				acl_name, intf_label, "in" );
				importer->pushRule();
				*dbg << std::endl;
				
#line 903 "PIXCfgParser.cpp"
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
	
	try {      // for error handling
		match(ICMP);
		{
		switch ( LA(1)) {
		case UNREACHABLE:
		{
			{
			match(UNREACHABLE);
			if ( inputState->guessing==0 ) {
#line 1852 "pix.g"
				
				consumeUntil(NEWLINE);
				
#line 941 "PIXCfgParser.cpp"
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
#line 1859 "pix.g"
				
				importer->clear();
				
#line 975 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 1863 "pix.g"
				
				importer->SaveTmpAddrToSrc();
				
#line 983 "PIXCfgParser.cpp"
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
			case OUTSIDE:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			interface_label();
			if ( inputState->guessing==0 ) {
#line 1868 "pix.g"
				
				std::string intf_label = LT(0)->getText();
				std::string acl_name = "icmp_commands_" + intf_label;
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->newUnidirRuleSet(acl_name, libfwbuilder::Policy::TYPENAME );
				importer->newPolicyRule();
				if (permit) importer->action = "permit";
				if (deny) importer->action = "deny";
				importer->setDstSelf();
				importer->protocol = "icmp";
				importer->setInterfaceAndDirectionForRuleSet(
				acl_name, intf_label, "in" );
				importer->pushRule();
				
#line 1024 "PIXCfgParser.cpp"
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

void PIXCfgParser::nat_top_level_command() {
	
	try {      // for error handling
		match(NAT);
		match(OPENING_PAREN);
		if ( inputState->guessing==0 ) {
#line 1954 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 1057 "PIXCfgParser.cpp"
		}
		{
		if ((LA(1) == WORD || LA(1) == OUTSIDE) && (LA(2) == CLOSING_PAREN)) {
			nat_old_top_level_command();
		}
		else if ((LA(1) == WORD || LA(1) == OUTSIDE) && (LA(2) == COMMA)) {
			nat_new_top_level_command();
		}
		else {
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

void PIXCfgParser::global_top_level_command() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  num = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(GLOBAL);
		match(OPENING_PAREN);
		if ( inputState->guessing==0 ) {
#line 2075 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 1094 "PIXCfgParser.cpp"
		}
		interface_label();
		if ( inputState->guessing==0 ) {
#line 2080 "pix.g"
			
			importer->tmp_global_pool.pool_interface = LT(0)->getText();
			
#line 1102 "PIXCfgParser.cpp"
		}
		match(CLOSING_PAREN);
		num = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 2085 "pix.g"
			
			importer->tmp_global_pool.str_num = num->getText();
			importer->tmp_global_pool.netmask = "255.255.255.255";
			*dbg << " GLOBAL POOL "
			<< importer->tmp_global_pool.str_num
			<< " "
			<< importer->tmp_global_pool.pool_interface;
			
#line 1117 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case INTRFACE:
		{
			match(INTRFACE);
			if ( inputState->guessing==0 ) {
#line 2098 "pix.g"
				
				importer->tmp_global_pool.start = LT(0)->getText();
				importer->tmp_global_pool.end = LT(0)->getText();
				
#line 1130 "PIXCfgParser.cpp"
			}
			break;
		}
		case IPV4:
		case IPV6:
		{
			single_addr();
			if ( inputState->guessing==0 ) {
#line 2104 "pix.g"
				
				importer->tmp_global_pool.start = importer->tmp_a;
				importer->tmp_global_pool.end = importer->tmp_a;
				
#line 1144 "PIXCfgParser.cpp"
			}
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
			single_addr();
			if ( inputState->guessing==0 ) {
#line 2113 "pix.g"
				
				importer->tmp_global_pool.end = importer->tmp_a;
				
#line 1165 "PIXCfgParser.cpp"
			}
			break;
		}
		case NEWLINE:
		case NETMASK:
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
		case NETMASK:
		{
			match(NETMASK);
			match(IPV4);
			if ( inputState->guessing==0 ) {
#line 2122 "pix.g"
				
				importer->tmp_global_pool.netmask = LT(0)->getText();
				
#line 1191 "PIXCfgParser.cpp"
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
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 2128 "pix.g"
			
			importer->addGlobalPool();
			*dbg << " " << importer->tmp_global_pool.start
			<< " " << importer->tmp_global_pool.end
			<< " " << importer->tmp_global_pool.netmask
			<< std::endl;
			
#line 1215 "PIXCfgParser.cpp"
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

void PIXCfgParser::static_top_level_command() {
	
	try {      // for error handling
		match(STATIC);
		match(OPENING_PAREN);
		if ( inputState->guessing==0 ) {
#line 2140 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 1239 "PIXCfgParser.cpp"
		}
		interface_label();
		if ( inputState->guessing==0 ) {
#line 2144 "pix.g"
			importer->prenat_interface = LT(0)->getText();
#line 1245 "PIXCfgParser.cpp"
		}
		match(COMMA);
		interface_label();
		if ( inputState->guessing==0 ) {
#line 2146 "pix.g"
			importer->postnat_interface = LT(0)->getText();
#line 1252 "PIXCfgParser.cpp"
		}
		match(CLOSING_PAREN);
		if ( inputState->guessing==0 ) {
#line 2148 "pix.g"
			
			importer->newUnidirRuleSet("nat", libfwbuilder::NAT::TYPENAME );
			*dbg << " DNAT rule ";
			importer->rule_type = libfwbuilder::NATRule::DNAT;
			
#line 1262 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case IPV4:
		case IPV6:
		case INTRFACE:
		{
			static_starts_with_hostaddr();
			break;
		}
		case TCP:
		case UDP:
		{
			static_starts_with_tcp_udp();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 2163 "pix.g"
			
			importer->pushNATRule();
			*dbg << std::endl;
			
#line 1292 "PIXCfgParser.cpp"
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
	
	try {      // for error handling
		match(ACCESS_GROUP);
		aclname = LT(1);
		match(WORD);
		dir = LT(1);
		match(WORD);
		match(INTRFACE);
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1927 "pix.g"
			
			std::string intf_label = LT(0)->getText();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setInterfaceAndDirectionForRuleSet(
			aclname->getText(),
			intf_label,
			dir->getText() );
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE: ACL '" << aclname->getText() << "'"
			<< " " << intf_label
			<< " " << dir->getText() << std::endl;
			
#line 1331 "PIXCfgParser.cpp"
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
#line 867 "pix.g"
			
			consumeUntil(NEWLINE);
			consumeUntil(QUIT);
			
#line 1370 "PIXCfgParser.cpp"
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
#line 196 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1392 "PIXCfgParser.cpp"
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
	
	try {      // for error handling
		match(NAME);
		{
		switch ( LA(1)) {
		case IPV4:
		{
			name_entry_ipv4();
			break;
		}
		case IPV6:
		{
			name_entry_ipv6();
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

void PIXCfgParser::named_object_network() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT);
		match(NETWORK);
		name = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 258 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newNamedObjectAddress(name->getText());
			*dbg << name->getLine() << ":"
			<< " Named Object " << name->getText() << std::endl;
			
#line 1471 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			switch ( LA(1)) {
			case DESCRIPTION:
			{
				named_object_description();
				break;
			}
			case HOST:
			case RANGE:
			case SUBNET:
			{
				named_object_network_parameters();
				break;
			}
			default:
				if ((LA(1) == NAT) && (LA(2) == OPENING_PAREN)) {
					named_object_nat();
				}
			else {
				goto _loop18;
			}
			}
		}
		_loop18:;
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
	
	try {      // for error handling
		match(OBJECT);
		match(SERVICE);
		if ( inputState->guessing==0 ) {
#line 371 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 1521 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
			break;
		}
		case HTTP:
		{
			match(HTTP);
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
		if ( inputState->guessing==0 ) {
#line 376 "pix.g"
			
			importer->newNamedObjectService(LT(0)->getText());
			*dbg << " NAMED OBJECT " << LT(0)->getText() << std::endl;
			
#line 1557 "PIXCfgParser.cpp"
		}
		match(NEWLINE);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == DESCRIPTION)) {
				named_object_description();
			}
			else if ((LA(1) == SERVICE) && (_tokenSet_6.member(LA(2)))) {
				named_object_service_parameters();
			}
			else {
				goto _loop35;
			}
			
		}
		_loop35:;
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
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 491 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupNetwork(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1604 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == DESCRIPTION || LA(1) == GROUP_OBJECT || LA(1) == NETWORK_OBJECT)) {
				object_group_network_parameters();
			}
			else {
				goto _loop54;
			}
			
		}
		_loop54:;
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
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 713 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupService(name->getText());
			if (tcp) importer->setObjectGroupServiceProtocol("tcp");
			if (udp) importer->setObjectGroupServiceProtocol("udp");
			if (tcpudp) importer->setObjectGroupServiceProtocol("tcp-udp");
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1683 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_7.member(LA(1)))) {
				object_group_service_parameters();
			}
			else {
				goto _loop86;
			}
			
		}
		_loop86:;
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

void PIXCfgParser::object_group_protocol() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(PROTOCOL);
		name = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 589 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupProtocol(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1726 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt66=0;
		for (;;) {
			if ((LA(1) == DESCRIPTION || LA(1) == GROUP_OBJECT || LA(1) == PROTOCOL_OBJECT)) {
				object_group_protocol_parameters();
			}
			else {
				if ( _cnt66>=1 ) { goto _loop66; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt66++;
		}
		_loop66:;
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
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 644 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupICMP(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1771 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == DESCRIPTION || LA(1) == GROUP_OBJECT || LA(1) == ICMP_OBJECT)) {
				object_group_icmp_parameters();
			}
			else {
				goto _loop74;
			}
			
		}
		_loop74:;
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
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 657 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupICMP(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1814 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == DESCRIPTION || LA(1) == GROUP_OBJECT || LA(1) == ICMP_OBJECT)) {
				object_group_icmp_parameters();
			}
			else {
				goto _loop77;
			}
			
		}
		_loop77:;
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
#line 807 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1848 "PIXCfgParser.cpp"
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
#line 858 "pix.g"
			
			*dbg << " TOP LEVEL \"NO\" COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 1872 "PIXCfgParser.cpp"
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
#line 210 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1894 "PIXCfgParser.cpp"
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

void PIXCfgParser::dns_command() {
	
	try {      // for error handling
		match(DNS);
		if ( inputState->guessing==0 ) {
#line 828 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1916 "PIXCfgParser.cpp"
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

void PIXCfgParser::service_top_level_command() {
	
	try {      // for error handling
		match(SERVICE);
		{
		switch ( LA(1)) {
		case CALL_HOME:
		{
			match(CALL_HOME);
			break;
		}
		case INTERNAL:
		{
			match(INTERNAL);
			break;
		}
		case PASSWORD_RECOVERY:
		{
			match(PASSWORD_RECOVERY);
			break;
		}
		case RESETINBOUND:
		{
			match(RESETINBOUND);
			break;
		}
		case RESETOUTBOUND:
		{
			match(RESETOUTBOUND);
			break;
		}
		case RESETOUTSIDE:
		{
			match(RESETOUTSIDE);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 851 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1976 "PIXCfgParser.cpp"
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

void PIXCfgParser::pim_top_level_command() {
	
	try {      // for error handling
		match(PIM);
		if ( inputState->guessing==0 ) {
#line 218 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1998 "PIXCfgParser.cpp"
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

void PIXCfgParser::network_top_level_command() {
	
	try {      // for error handling
		match(NETWORK);
		if ( inputState->guessing==0 ) {
#line 226 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 2020 "PIXCfgParser.cpp"
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
#line 821 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 2042 "PIXCfgParser.cpp"
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

void PIXCfgParser::name_entry_ipv4() {
	
	try {      // for error handling
		match(IPV4);
		match(WORD);
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

void PIXCfgParser::name_entry_ipv6() {
	
	try {      // for error handling
		match(IPV6);
		match(WORD);
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
		case ESP_WORD:
		{
			match(ESP_WORD);
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
			recover(ex,_tokenSet_8);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::named_object_nat() {
	
	try {      // for error handling
		nat_top_level_command();
		if ( inputState->guessing==0 ) {
#line 286 "pix.g"
			
			*dbg << "Named object with singleton nat command" << std::endl;
			importer->addMessageToLog(
			QString("Warning: "
			"Import of named objects with \"nat\" command "
			"is not supported at this time"));
			consumeUntil(NEWLINE);
			
#line 2198 "PIXCfgParser.cpp"
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

void PIXCfgParser::named_object_description() {
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 297 "pix.g"
			
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
			
#line 2229 "PIXCfgParser.cpp"
		}
		match(NEWLINE);
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

void PIXCfgParser::named_object_network_parameters() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
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
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::host_addr() {
	
	try {      // for error handling
		match(HOST);
		single_addr();
		if ( inputState->guessing==0 ) {
#line 313 "pix.g"
			
			importer->commitNamedAddressObject();
			
#line 2291 "PIXCfgParser.cpp"
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
#line 337 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->tmp_range_1 = r1->getText();
			importer->tmp_range_2 = r2->getText();
			importer->commitNamedAddressRangeObject();
			*dbg << r1->getText() << "/" << r2->getText();
			
#line 2325 "PIXCfgParser.cpp"
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
#line 347 "pix.g"
			
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
			QString("Warning: IPv6 import is not supported. "));
			consumeUntil(NEWLINE);
			}
			
#line 2389 "PIXCfgParser.cpp"
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

void PIXCfgParser::single_addr() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
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
		if ( inputState->guessing==0 ) {
#line 319 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			if (h)
			{
			importer->tmp_a = h->getText();
			importer->tmp_nm = "255.255.255.255";
			*dbg << importer->tmp_a << " ";
			}
			if (v6)
			{
			importer->addMessageToLog(
			QString("Warning: IPv6 import is not supported. "));
			consumeUntil(NEWLINE);
			}
			
#line 2444 "PIXCfgParser.cpp"
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
		else if ((LA(1) == SERVICE) && (_tokenSet_11.member(LA(2)))) {
			service_other();
		}
		else if ((LA(1) == SERVICE) && (LA(2) == WORD)) {
			service_unknown();
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
			recover(ex,_tokenSet_12);
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
#line 406 "pix.g"
				
				importer->icmp_type = LT(0)->getText();
				
#line 2510 "PIXCfgParser.cpp"
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
#line 411 "pix.g"
				
				importer->icmp_spec = LT(0)->getText();
				
#line 2540 "PIXCfgParser.cpp"
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
#line 415 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->commitNamedICMPServiceObject();
			*dbg << "NAMED OBJECT SERVICE ICMP " << LT(0)->getText() << " ";
			
#line 2557 "PIXCfgParser.cpp"
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
#line 423 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addMessageToLog(
			QString("Warning: "
			"Import of IPv6 addresses and servcies "
			"is not supported at this time"));
			*dbg << "NAMED OBJECT SERVICE ICMP6 " << LT(0)->getText() << " ";
			consumeUntil(NEWLINE);
			
#line 2604 "PIXCfgParser.cpp"
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
#line 435 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
			
#line 2645 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case SOURCE:
		{
			src_port_spec();
			break;
		}
		case NEWLINE:
		case RANGE:
		case DESTINATION:
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
#line 441 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->commitNamedTCPUDPServiceObject();
			
#line 2698 "PIXCfgParser.cpp"
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
		case PIM:
		case AH:
		case EIGRP:
		case ESP_WORD:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
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
#line 460 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->protocol = LT(0)->getText();
			importer->commitNamedIPServiceObject();
			*dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
			
#line 2755 "PIXCfgParser.cpp"
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

void PIXCfgParser::service_unknown() {
	
	try {      // for error handling
		match(SERVICE);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 475 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->protocol = "ip";
			importer->commitNamedIPServiceObject();
			std::string err = "Warning: Unknown service name " +
			LT(0)->getText();
			importer->setNamedObjectDescription(err);
			importer->addMessageToLog(err);
			*dbg << "UNKNOWN SERVICE " << LT(0)->getText() << " ";
			
#line 2785 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_13);
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
#line 448 "pix.g"
			
			importer->SaveTmpPortToSrc();
			
#line 2925 "PIXCfgParser.cpp"
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
#line 454 "pix.g"
			
			importer->SaveTmpPortToDst();
			
#line 2968 "PIXCfgParser.cpp"
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

void PIXCfgParser::xoperator() {
#line 1162 "pix.g"
	importer->tmp_port_spec = "";
#line 2984 "PIXCfgParser.cpp"
	
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
			recover(ex,_tokenSet_15);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::object_group_network_parameters() {
	
	try {      // for error handling
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
		match(NEWLINE);
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

void PIXCfgParser::object_group_description() {
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 515 "pix.g"
			
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
			
#line 3073 "PIXCfgParser.cpp"
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

void PIXCfgParser::group_object() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(GROUP_OBJECT);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 530 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addNamedObjectToObjectGroup(name->getText());
			*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
			
#line 3101 "PIXCfgParser.cpp"
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
#line 539 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 3130 "PIXCfgParser.cpp"
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
#line 545 "pix.g"
				
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
				QString("Warning: IPv6 import is not supported. "));
				consumeUntil(NEWLINE);
				}
				
#line 3178 "PIXCfgParser.cpp"
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
#line 562 "pix.g"
				
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
				QString("Warning: IPv6 import is not supported. "));
				consumeUntil(NEWLINE);
				}
				
#line 3222 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 579 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 3237 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::object_group_protocol_parameters() {
	
	try {      // for error handling
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
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_17);
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
#line 613 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 3307 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case IP:
		case PIM:
		case AH:
		case EIGRP:
		case ESP_WORD:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
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
			case PIM:
			case AH:
			case EIGRP:
			case ESP_WORD:
			case GRE:
			case IGMP:
			case IGRP:
			case IPINIP:
			case IPSEC:
			case NOS:
			case OSPF:
			case PCP:
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
#line 619 "pix.g"
				
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->protocol = LT(0)->getText();
				importer->addIPServiceToObjectGroup();
				*dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
				
#line 3386 "PIXCfgParser.cpp"
			}
			break;
		}
		case ICMP6:
		{
			match(ICMP6);
			if ( inputState->guessing==0 ) {
#line 627 "pix.g"
				
				importer->addMessageToLog(
				QString("Warning: IPv6 import is not supported. "));
				consumeUntil(NEWLINE);
				
#line 3400 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 634 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 3415 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::object_group_icmp_parameters() {
	
	try {      // for error handling
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
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_18);
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
#line 681 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 3486 "PIXCfgParser.cpp"
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
#line 688 "pix.g"
					
					importer->icmp_type = LT(0)->getText();
					
#line 3522 "PIXCfgParser.cpp"
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
#line 693 "pix.g"
					
					importer->icmp_spec = LT(0)->getText();
					
#line 3552 "PIXCfgParser.cpp"
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
#line 697 "pix.g"
				
				importer->addICMPServiceToObjectGroup();
				*dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
				
#line 3568 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 703 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 3583 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::object_group_service_parameters() {
	
	try {      // for error handling
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
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_19);
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
#line 742 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 3659 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case IP:
		case PIM:
		case AH:
		case EIGRP:
		case ESP_WORD:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
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
			case PIM:
			case AH:
			case EIGRP:
			case ESP_WORD:
			case GRE:
			case IGMP:
			case IGRP:
			case IPINIP:
			case IPSEC:
			case NOS:
			case OSPF:
			case PCP:
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
#line 748 "pix.g"
				
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->protocol = LT(0)->getText();
				importer->addIPServiceToObjectGroup();
				*dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
				
#line 3720 "PIXCfgParser.cpp"
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
#line 756 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << " SERVICE TCP/UDP" << LT(0)->getText() << " ";
				
#line 3757 "PIXCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case SOURCE:
			{
				src_port_spec();
				break;
			}
			case NEWLINE:
			case RANGE:
			case DESTINATION:
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
#line 762 "pix.g"
				
				importer->addTCPUDPServiceToObjectGroup();
				
#line 3809 "PIXCfgParser.cpp"
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
#line 769 "pix.g"
					
					importer->icmp_type = LT(0)->getText();
					
#line 3827 "PIXCfgParser.cpp"
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
#line 774 "pix.g"
					
					importer->icmp_spec = LT(0)->getText();
					
#line 3857 "PIXCfgParser.cpp"
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
#line 778 "pix.g"
				
				importer->addICMPServiceToObjectGroup();
				*dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
				
#line 3873 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 784 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 3888 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::port_object() {
#line 791 "pix.g"
	
	importer->tmp_port_spec = "";
	importer->tmp_port_spec_2 = "";
	
#line 3915 "PIXCfgParser.cpp"
	
	try {      // for error handling
		match(PORT_OBJECT);
		xoperator();
		if ( inputState->guessing==0 ) {
#line 796 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			*dbg << " PORT OBJECT TCP/UDP " << LT(0)->getText() << " " << std::endl;
			importer->SaveTmpPortToDst();
			importer->addTCPUDPServiceToObjectGroup();
			*dbg << std::endl;
			
#line 3929 "PIXCfgParser.cpp"
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
#line 935 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 3971 "PIXCfgParser.cpp"
		}
		rule_extended();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 942 "pix.g"
			
			importer->pushRule();
			
#line 3980 "PIXCfgParser.cpp"
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
#line 948 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 4022 "PIXCfgParser.cpp"
		}
		rule_extended();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 955 "pix.g"
			
			importer->pushRule();
			
#line 4031 "PIXCfgParser.cpp"
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
#line 961 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 4057 "PIXCfgParser.cpp"
		}
		rule_standard();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 968 "pix.g"
			
			importer->pushRule();
			
#line 4066 "PIXCfgParser.cpp"
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
#line 974 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 4092 "PIXCfgParser.cpp"
		}
		rule_standard();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 981 "pix.g"
			
			importer->pushRule();
			
#line 4101 "PIXCfgParser.cpp"
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
#line 1909 "pix.g"
			
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
			
#line 4133 "PIXCfgParser.cpp"
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
	ANTLR_USE_NAMESPACE(antlr)RefToken  grp_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IP:
		case PIM:
		case AH:
		case EIGRP:
		case ESP_WORD:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
		case PPTP:
		case SNP:
		case OBJECT:
		case ICMP6:
		case OBJECT_GROUP:
		{
			ip_protocols();
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 1006 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 4176 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 1007 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 4182 "PIXCfgParser.cpp"
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
#line 1013 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 4252 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 1017 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 4258 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 1018 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 4264 "PIXCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case OBJECT_GROUP:
			{
				match(OBJECT_GROUP);
				grp_name = LT(1);
				match(WORD);
				if ( inputState->guessing==0 ) {
#line 1035 "pix.g"
					
					importer->icmp_spec = grp_name->getText();
					*dbg << "service gorup: "
					<< grp_name->getText() << std::endl;
					
#line 4280 "PIXCfgParser.cpp"
				}
				break;
			}
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
#line 1049 "pix.g"
			
			*dbg << std::endl;
			
#line 4398 "PIXCfgParser.cpp"
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

void PIXCfgParser::rule_standard() {
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 991 "pix.g"
			
			importer->tmp_a = "0.0.0.0";
			importer->tmp_nm = "0.0.0.0";
			importer->SaveTmpAddrToSrc();
			
#line 4421 "PIXCfgParser.cpp"
		}
		hostaddr_expr();
		if ( inputState->guessing==0 ) {
#line 997 "pix.g"
			
			importer->SaveTmpAddrToDst();
			*dbg << "(dst) " << std::endl;
			
#line 4430 "PIXCfgParser.cpp"
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

void PIXCfgParser::hostaddr_expr() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTRFACE:
		{
			match(INTRFACE);
			interface_label();
			if ( inputState->guessing==0 ) {
#line 1296 "pix.g"
				
				importer->tmp_a = LT(0)->getText();
				importer->tmp_nm = "interface";
				*dbg << "object " << LT(0)->getText() << " ";
				
#line 4462 "PIXCfgParser.cpp"
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
#line 1303 "pix.g"
				
				importer->tmp_a = name->getText();
				importer->tmp_nm = "";
				*dbg << "object " << name->getText() << " ";
				
#line 4498 "PIXCfgParser.cpp"
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
#line 1310 "pix.g"
				
				importer->tmp_a = h->getText();
				importer->tmp_nm = "255.255.255.255";
				*dbg << h->getText() << "/255.255.255.255";
				
#line 4516 "PIXCfgParser.cpp"
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
#line 1317 "pix.g"
				
				importer->tmp_a = a->getText();
				importer->tmp_nm = m->getText();
				*dbg << a->getText() << "/" << m->getText();
				
#line 4535 "PIXCfgParser.cpp"
			}
			break;
		}
		case ANY:
		{
			match(ANY);
			if ( inputState->guessing==0 ) {
#line 1324 "pix.g"
				
				importer->tmp_a = "0.0.0.0";
				importer->tmp_nm = "0.0.0.0";
				*dbg << "0.0.0.0/0.0.0.0";
				
#line 4549 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_20);
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
		case PIM:
		case AH:
		case EIGRP:
		case ESP_WORD:
		case GRE:
		case IGMP:
		case IGRP:
		case IPINIP:
		case IPSEC:
		case NOS:
		case OSPF:
		case PCP:
		case PPTP:
		case SNP:
		case ICMP6:
		{
			{
			switch ( LA(1)) {
			case IP:
			case PIM:
			case AH:
			case EIGRP:
			case ESP_WORD:
			case GRE:
			case IGMP:
			case IGRP:
			case IPINIP:
			case IPSEC:
			case NOS:
			case OSPF:
			case PCP:
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
#line 1243 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 4630 "PIXCfgParser.cpp"
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
#line 1249 "pix.g"
				
				importer->protocol = name->getText();
				*dbg << "protocol " << name->getText() << " ";
				
#line 4665 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_21);
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
#line 1376 "pix.g"
			
			importer->time_range_name = tr_name->getText();
			*dbg << "time_range " << tr_name->getText() << " ";
			
#line 4699 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_22);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::fragments() {
	
	try {      // for error handling
		match(FRAGMENTS);
		if ( inputState->guessing==0 ) {
#line 1369 "pix.g"
			
			importer->fragments = true;
			*dbg << "fragments ";
			
#line 4722 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_23);
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
#line 1335 "pix.g"
			
			importer->logging = true;
			
#line 4761 "PIXCfgParser.cpp"
		}
		{
		{
		switch ( LA(1)) {
		case INT_CONST:
		case LOG_LEVEL_ALERTS:
		case LOG_LEVEL_CRITICAL:
		case LOG_LEVEL_DEBUGGING:
		case LOG_LEVEL_EMERGENCIES:
		case LOG_LEVEL_ERRORS:
		case LOG_LEVEL_INFORMATIONAL:
		case LOG_LEVEL_NOTIFICATIONS:
		case LOG_LEVEL_WARNINGS:
		case LOG_LEVEL_DISABLE:
		case LOG_LEVEL_INACTIVE:
		{
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				match(INT_CONST);
				break;
			}
			case LOG_LEVEL_ALERTS:
			{
				match(LOG_LEVEL_ALERTS);
				break;
			}
			case LOG_LEVEL_CRITICAL:
			{
				match(LOG_LEVEL_CRITICAL);
				break;
			}
			case LOG_LEVEL_DEBUGGING:
			{
				match(LOG_LEVEL_DEBUGGING);
				break;
			}
			case LOG_LEVEL_EMERGENCIES:
			{
				match(LOG_LEVEL_EMERGENCIES);
				break;
			}
			case LOG_LEVEL_ERRORS:
			{
				match(LOG_LEVEL_ERRORS);
				break;
			}
			case LOG_LEVEL_INFORMATIONAL:
			{
				match(LOG_LEVEL_INFORMATIONAL);
				break;
			}
			case LOG_LEVEL_NOTIFICATIONS:
			{
				match(LOG_LEVEL_NOTIFICATIONS);
				break;
			}
			case LOG_LEVEL_WARNINGS:
			{
				match(LOG_LEVEL_WARNINGS);
				break;
			}
			case LOG_LEVEL_DISABLE:
			{
				match(LOG_LEVEL_DISABLE);
				break;
			}
			case LOG_LEVEL_INACTIVE:
			{
				match(LOG_LEVEL_INACTIVE);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState->guessing==0 ) {
#line 1352 "pix.g"
				importer->log_level = LT(0)->getText();
#line 4844 "PIXCfgParser.cpp"
			}
			break;
		}
		case NEWLINE:
		case INTERVAL:
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
		case INTERVAL:
		{
			{
			match(INTERVAL);
			match(INT_CONST);
			}
			if ( inputState->guessing==0 ) {
#line 1357 "pix.g"
				importer->log_interval = LT(0)->getText();
#line 4870 "PIXCfgParser.cpp"
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
		}
		if ( inputState->guessing==0 ) {
#line 1360 "pix.g"
			
			//        if (importer->log_level == "log") importer->log_level = "";
			//        if (importer->log_interval == "log") importer->log_interval = "";
			*dbg << "logging level '" << importer->log_level
			<< "' interval '" << importer->log_interval << "'";
			
#line 4893 "PIXCfgParser.cpp"
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
#line 1259 "pix.g"
				
				importer->icmp_type = icmp_type->getText();
				importer->icmp_code = icmp_code->getText();
				importer->icmp_spec = "";
				*dbg << icmp_type->getText() << " "
				<< icmp_code->getText() << " ";
				
#line 4930 "PIXCfgParser.cpp"
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
#line 1268 "pix.g"
				
				importer->icmp_spec = LT(0)->getText();
				*dbg << LT(0)->getText() << " ";
				
#line 4961 "PIXCfgParser.cpp"
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
#line 1056 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << "protocol " << LT(0)->getText() << " ";
			
#line 5011 "PIXCfgParser.cpp"
		}
		hostaddr_expr();
		if ( inputState->guessing==0 ) {
#line 1060 "pix.g"
			importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 5017 "PIXCfgParser.cpp"
		}
		{
		bool synPredMatched131 = false;
		if (((_tokenSet_21.member(LA(1))) && (_tokenSet_24.member(LA(2))))) {
			int _m131 = mark();
			synPredMatched131 = true;
			inputState->guessing++;
			try {
				{
				match(OBJECT_GROUP);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched131 = false;
			}
			rewind(_m131);
			inputState->guessing--;
		}
		if ( synPredMatched131 ) {
			{
			if (((LA(1) == OBJECT_GROUP) && (LA(2) == WORD))&&( importer->isKnownServiceGroupName(LT(2)->getText()) )) {
				match(OBJECT_GROUP);
				src_grp_name = LT(1);
				match(WORD);
				if ( inputState->guessing==0 ) {
#line 1070 "pix.g"
					
					importer->src_port_spec = src_grp_name->getText();
					*dbg << "src port spec: "
					<< src_grp_name->getText() << std::endl;
					
#line 5049 "PIXCfgParser.cpp"
				}
				hostaddr_expr_1();
				if ( inputState->guessing==0 ) {
#line 1078 "pix.g"
					
					importer->SaveTmpAddrToDst();
					*dbg << "(dst) ";
					
#line 5058 "PIXCfgParser.cpp"
				}
				acl_tcp_udp_dst_port_spec();
			}
			else if ((_tokenSet_21.member(LA(1))) && (_tokenSet_24.member(LA(2)))) {
				hostaddr_expr_2();
				if ( inputState->guessing==0 ) {
#line 1087 "pix.g"
					
					importer->SaveTmpAddrToDst();
					*dbg << "(dst) ";
					
#line 5070 "PIXCfgParser.cpp"
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
			if ((_tokenSet_25.member(LA(1))) && (_tokenSet_26.member(LA(2)))) {
				acl_xoperator_dst();
			}
			else if ((_tokenSet_27.member(LA(1))) && (_tokenSet_28.member(LA(2)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			{
			if ((LA(1) == ESTABLISHED) && (_tokenSet_27.member(LA(2)))) {
				established();
			}
			else if ((_tokenSet_27.member(LA(1))) && (_tokenSet_29.member(LA(2)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			if ( inputState->guessing==0 ) {
#line 1096 "pix.g"
				
				// looks like "object foo" at this point can only be dest addr.
				// (judging by cli prompts on 8.3)
				importer->tmp_a = dst_addr_name->getText();
				importer->tmp_nm = "";
				importer->SaveTmpAddrToDst();
				*dbg << "dst addr object " << dst_addr_name->getText() << " ";
				
#line 5116 "PIXCfgParser.cpp"
			}
			acl_tcp_udp_dst_port_spec();
		}
		else if ((_tokenSet_30.member(LA(1))) && (_tokenSet_31.member(LA(2)))) {
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
#line 1110 "pix.g"
					
					importer->SaveTmpPortToSrc();
					
#line 5135 "PIXCfgParser.cpp"
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
#line 1114 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 5158 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_9);
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
			recover(ex,_tokenSet_27);
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
#line 1132 "pix.g"
				
				importer->dst_port_spec = dst_port_group_name->getText();
				*dbg << "dst port spec: "
				<< dst_port_group_name->getText() << std::endl;
				
#line 5270 "PIXCfgParser.cpp"
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
#line 1142 "pix.g"
				
				importer->dst_port_spec = dst_port_obj_name->getText();
				*dbg << "dst addr object " << dst_port_obj_name->getText()
				<< std::endl;
				
#line 5308 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_13);
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
			recover(ex,_tokenSet_27);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::acl_xoperator_dst() {
	
	try {      // for error handling
		xoperator();
		if ( inputState->guessing==0 ) {
#line 1157 "pix.g"
			
			importer->SaveTmpPortToDst();
			
#line 5435 "PIXCfgParser.cpp"
		}
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

void PIXCfgParser::established() {
	
	try {      // for error handling
		match(ESTABLISHED);
		if ( inputState->guessing==0 ) {
#line 1232 "pix.g"
			
			importer->established = true;
			*dbg << "established ";
			
#line 5458 "PIXCfgParser.cpp"
		}
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

void PIXCfgParser::hostaddr_expr_3() {
	
	try {      // for error handling
		hostaddr_expr();
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
#line 1169 "pix.g"
			
			importer->tmp_port_op = LT(0)->getText();
			*dbg << LT(0)->getText() << " ";
			
#line 5523 "PIXCfgParser.cpp"
		}
		port_spec();
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

void PIXCfgParser::port_range() {
	
	try {      // for error handling
		match(RANGE);
		pair_of_ports_spec();
		if ( inputState->guessing==0 ) {
#line 1188 "pix.g"
			
			importer->tmp_port_op = "range";
			*dbg << "range " << importer->tmp_port_spec;
			
#line 5548 "PIXCfgParser.cpp"
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

void PIXCfgParser::port_spec() {
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 1177 "pix.g"
			
			importer->tmp_port_spec_2 = "";
			
#line 5569 "PIXCfgParser.cpp"
		}
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1181 "pix.g"
			
			importer->tmp_port_spec = std::string(" ") + importer->tmp_port_spec_2;
			*dbg << LT(0)->getText() << " " << importer->tmp_port_spec;
			
#line 5578 "PIXCfgParser.cpp"
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

void PIXCfgParser::tcp_udp_port_spec() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case PPTP:
		case HTTP:
		case SSH:
		case TELNET:
		case HOSTNAME:
		case ECHO:
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
#line 1212 "pix.g"
			
			importer->tmp_port_spec_2 = LT(0)->getText();
			
#line 5627 "PIXCfgParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_32);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::pair_of_ports_spec() {
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 1195 "pix.g"
			
			importer->tmp_port_spec_2 = "";
			
#line 5648 "PIXCfgParser.cpp"
		}
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1199 "pix.g"
			
			importer->tmp_port_spec += importer->tmp_port_spec_2;
			
#line 5656 "PIXCfgParser.cpp"
		}
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1203 "pix.g"
			
			importer->tmp_port_spec += " ";
			importer->tmp_port_spec += importer->tmp_port_spec_2;
			
#line 5665 "PIXCfgParser.cpp"
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
		case HTTP:
		{
			match(HTTP);
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
			recover(ex,_tokenSet_32);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::interface_label() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case WORD:
		{
			match(WORD);
			break;
		}
		case OUTSIDE:
		{
			match(OUTSIDE);
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
			recover(ex,_tokenSet_33);
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
#line 1440 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newInterface( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE: " << in->getText() << std::endl;
			
#line 5775 "PIXCfgParser.cpp"
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
#line 1448 "pix.g"
	bool have_interface_parameters = false;
#line 5792 "PIXCfgParser.cpp"
	
	try {      // for error handling
		in = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 1449 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newInterface( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE: " << in->getText() << std::endl;
			
#line 5806 "PIXCfgParser.cpp"
		}
		{
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_34.member(LA(1)))) {
				interface_parameters();
				if ( inputState->guessing==0 ) {
#line 1456 "pix.g"
					have_interface_parameters = true;
#line 5816 "PIXCfgParser.cpp"
				}
			}
			else {
				goto _loop197;
			}
			
		}
		_loop197:;
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
#line 1458 "pix.g"
			
			if ( ! have_interface_parameters )
			{
			importer->ignoreCurrentInterface();
			*dbg<< LT(1)->getLine() << ":"
			<< " EMPTY INTERFACE " << std::endl;
			}
			
#line 5854 "PIXCfgParser.cpp"
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
#line 1493 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 5948 "PIXCfgParser.cpp"
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
		case PIM:
		case IGMP:
		case OSPF:
		case SPEED:
		case DUPLEX:
		case DDNS:
		case FORWARD:
		case DELAY:
		case HOLD_TIME:
		case IPV6_C:
		case MANAGEMENT_ONLY:
		case MAC_ADDRESS:
		case MULTICAST:
		case PPPOE:
		case RIP:
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
			recover(ex,_tokenSet_35);
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
#line 1519 "pix.g"
			
			importer->setInterfaceVlanId(vlan_id->getText());
			*dbg << " VLAN: " << vlan_id->getText() << std::endl;
			
#line 6042 "PIXCfgParser.cpp"
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

void PIXCfgParser::sec_level() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  sec_level = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SEC_LEVEL);
		sec_level = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1573 "pix.g"
			
			importer->setInterfaceSecurityLevel(sec_level->getText());
			*dbg << "SEC_LEVEL: " << sec_level->getText() << std::endl;
			
#line 6068 "PIXCfgParser.cpp"
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

void PIXCfgParser::nameif() {
	
	try {      // for error handling
		match(NAMEIF);
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1592 "pix.g"
			
			importer->setInterfaceParametes(LT(0)->getText(), "", "");
			*dbg << " NAMEIF: " << LT(0)->getText() << std::endl;
			
#line 6092 "PIXCfgParser.cpp"
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

void PIXCfgParser::interface_description() {
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 1601 "pix.g"
			
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
			
#line 6123 "PIXCfgParser.cpp"
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

void PIXCfgParser::switchport() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  vlan_num = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SWITCHPORT);
		match(ACCESS);
		match(VLAN);
		vlan_num = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1722 "pix.g"
			
			importer->addMessageToLog("Switch port vlan " + vlan_num->getText());
			*dbg << "Switch port vlan " <<  vlan_num->getText() << std::endl;
			
#line 6151 "PIXCfgParser.cpp"
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

void PIXCfgParser::shutdown() {
	
	try {      // for error handling
		match(SHUTDOWN);
		if ( inputState->guessing==0 ) {
#line 1616 "pix.g"
			
			importer->ignoreCurrentInterface();
			*dbg<< LT(1)->getLine() << ":"
			<< " INTERFACE SHUTDOWN " << std::endl;
			
#line 6175 "PIXCfgParser.cpp"
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
#line 1565 "pix.g"
			
			*dbg << " INTERFACE \"NO\" COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 6227 "PIXCfgParser.cpp"
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
		case MANAGEMENT_ONLY:
		{
			match(MANAGEMENT_ONLY);
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
#line 1557 "pix.g"
			
			*dbg << " UNSUPPORTED INTERFACE COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 6333 "PIXCfgParser.cpp"
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

void PIXCfgParser::v6_ip_address() {
	
	try {      // for error handling
		if ((LA(1) == WORD || LA(1) == OUTSIDE) && (LA(2) == DHCP)) {
			v6_dhcp_address();
		}
		else if ((LA(1) == WORD || LA(1) == OUTSIDE) && (LA(2) == IPV4)) {
			v6_static_address();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
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
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::v6_dhcp_address() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  dhcp = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1650 "pix.g"
	std::string lbl;
#line 6404 "PIXCfgParser.cpp"
	
	try {      // for error handling
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1652 "pix.g"
			
			lbl = LT(0)->getText();
			
#line 6413 "PIXCfgParser.cpp"
		}
		dhcp = LT(1);
		match(DHCP);
		if ( inputState->guessing==0 ) {
#line 1656 "pix.g"
			
			std::string addr = dhcp->getText();
			importer->addInterfaceAddress(lbl, addr, "");
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << std::endl;
			// there can be some other parameters after "dhcp", such as
			// "setroute", "retry" etc.  which we do not support
			consumeUntil(NEWLINE);
			
#line 6428 "PIXCfgParser.cpp"
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

void PIXCfgParser::v6_static_address() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1667 "pix.g"
	std::string lbl;
#line 6446 "PIXCfgParser.cpp"
	
	try {      // for error handling
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1669 "pix.g"
			
			lbl = LT(0)->getText();
			
#line 6455 "PIXCfgParser.cpp"
		}
		a = LT(1);
		match(IPV4);
		m = LT(1);
		match(IPV4);
		if ( inputState->guessing==0 ) {
#line 1673 "pix.g"
			
			std::string addr = a->getText();
			std::string netm = m->getText();
			importer->addInterfaceAddress(lbl, addr, netm);
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << "/" << netm << std::endl;
			// in case there are some other parameters after address and netmask
			consumeUntil(NEWLINE);
			
#line 6472 "PIXCfgParser.cpp"
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

void PIXCfgParser::v7_dhcp_address() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  dhcp = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		dhcp = LT(1);
		match(DHCP);
		if ( inputState->guessing==0 ) {
#line 1689 "pix.g"
			
			std::string addr = dhcp->getText();
			importer->addInterfaceAddress(addr, "");
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << std::endl;
			consumeUntil(NEWLINE);
			
#line 6500 "PIXCfgParser.cpp"
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
		case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
		case NEWLINE:
		case QUIT:
		case IP:
		case TIMEOUT:
		case PIM:
		case NETWORK:
		case NAMES:
		case NAME:
		case WORD:
		case OBJECT:
		case SERVICE:
		case HTTP:
		case SSH:
		case TELNET:
		case ICMP:
		case OBJECT_GROUP:
		case CRYPTO:
		case DNS:
		case NO:
		case CERTIFICATE:
		case PIX_WORD:
		case ASA_WORD:
		case FWSM_WORD:
		case HOSTNAME:
		case ACCESS_LIST:
		case INTRFACE:
		case CONTROLLER:
		case LINE_COMMENT:
		case EXIT:
		case NAMEIF:
		case ACCESS_GROUP:
		case COLON_COMMENT:
		case NAT:
		case GLOBAL:
		case STATIC:
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
#line 1700 "pix.g"
			
			std::string addr = a->getText();
			std::string netm = m->getText();
			importer->addInterfaceAddress(addr, netm);
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << "/" << netm << std::endl;
			// there can be other parameters after address/netmask pair, such as "standby"
			// We do not parse them yet.
			if (s)
			{
			importer->addMessageToLog(
			QString("Warning: failover IP detected. "
			"Failover is not supported by import "
			"at this time"));
			}
			consumeUntil(NEWLINE);
			
#line 6595 "PIXCfgParser.cpp"
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

void PIXCfgParser::icmp_types_for_icmp_command() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case INT_CONST:
		{
			match(INT_CONST);
			if ( inputState->guessing==0 ) {
#line 1888 "pix.g"
				
				importer->icmp_type = LT(0)->getText();
				importer->icmp_code = "0";
				importer->icmp_spec = "";
				
#line 6622 "PIXCfgParser.cpp"
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
#line 1895 "pix.g"
				
				importer->icmp_type = "";
				importer->icmp_code = "0";
				importer->icmp_spec = LT(0)->getText();
				
#line 6666 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_36);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::nat_old_top_level_command() {
	
	try {      // for error handling
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1963 "pix.g"
			
			importer->prenat_interface = LT(0)->getText();
			
#line 6695 "PIXCfgParser.cpp"
		}
		match(CLOSING_PAREN);
		if ( inputState->guessing==0 ) {
#line 1967 "pix.g"
			
			importer->newUnidirRuleSet("nat", libfwbuilder::NAT::TYPENAME );
			*dbg << " SNAT rule ";
			importer->rule_type = libfwbuilder::NATRule::SNAT;
			
#line 6705 "PIXCfgParser.cpp"
		}
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1975 "pix.g"
			
			importer->nat_num = LT(0)->getText();
			
#line 6713 "PIXCfgParser.cpp"
		}
		nat_addr_match();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_37.member(LA(1)))) {
				nat_command_last_parameters();
			}
			else {
				goto _loop254;
			}
			
		}
		_loop254:;
		} // ( ... )*
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 1987 "pix.g"
			
			importer->pushNATRule();
			*dbg << std::endl;
			
#line 6735 "PIXCfgParser.cpp"
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

void PIXCfgParser::nat_new_top_level_command() {
	
	try {      // for error handling
		interface_label();
		match(COMMA);
		interface_label();
		match(CLOSING_PAREN);
		if ( inputState->guessing==0 ) {
#line 2064 "pix.g"
			
			importer->addMessageToLog(
			QString("Warning: Import of ASA 8.3 nat command "
			"is not supported at this time"));
			consumeUntil(NEWLINE);
			
#line 6763 "PIXCfgParser.cpp"
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

void PIXCfgParser::nat_addr_match() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  acl_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case IPV4:
		case IPV6:
		{
			single_addr();
			if ( inputState->guessing==0 ) {
#line 1995 "pix.g"
				
				importer->nat_a = importer->tmp_a;
				
#line 6790 "PIXCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case IPV4:
			case IPV6:
			{
				single_addr();
				if ( inputState->guessing==0 ) {
#line 2003 "pix.g"
					
					importer->nat_nm = importer->tmp_a;
					
#line 6803 "PIXCfgParser.cpp"
				}
				break;
			}
			case NEWLINE:
			case INT_CONST:
			case TCP:
			case UDP:
			case DNS:
			case OUTSIDE:
			case NORANDOMSEQ:
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
		case INT_CONST:
		{
			match(INT_CONST);
			if ( inputState->guessing==0 ) {
#line 2009 "pix.g"
				
				if (LT(0)->getText() != std::string("0"))
				{
				importer->reportError(
				std::string("Unknown string ")+LT(0)->getText()+
				std::string("; expected \"0\""));
				*dbg << " UNKNOWN STRING " << LT(0)->getText();
				}
				importer->nat_a = "0.0.0.0";
				
#line 6840 "PIXCfgParser.cpp"
			}
			{
			if ((LA(1) == INT_CONST) && (_tokenSet_38.member(LA(2)))) {
				match(INT_CONST);
				if ( inputState->guessing==0 ) {
#line 2021 "pix.g"
					
					if (LT(0)->getText() != std::string("0"))
					{
					importer->reportError(
					std::string("Unknown string ")+LT(0)->getText()+
					std::string("; expected \"0\""));
					*dbg << " UNKNOWN STRING " << LT(0)->getText();
					}
					importer->nat_nm = "0.0.0.0";
					
#line 6857 "PIXCfgParser.cpp"
				}
			}
			else if ((_tokenSet_38.member(LA(1))) && (_tokenSet_39.member(LA(2)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			break;
		}
		case ACCESS_LIST:
		{
			match(ACCESS_LIST);
			acl_name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 2034 "pix.g"
				
				importer->nat_acl = acl_name->getText();
				
#line 6879 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_38);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::nat_command_last_parameters() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case INT_CONST:
		case TCP:
		case UDP:
		case DNS:
		case NORANDOMSEQ:
		{
			nat_and_static_command_common_last_parameters();
			break;
		}
		case OUTSIDE:
		{
			match(OUTSIDE);
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
			recover(ex,_tokenSet_38);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::nat_and_static_command_common_last_parameters() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  max_conn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  max_emb_conn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case DNS:
		{
			match(DNS);
			if ( inputState->guessing==0 ) {
#line 2306 "pix.g"
				
				importer->addMessageToLog(
				QString("Warning: 'nat' and 'static' command option 'dns' is not supported"));
				
#line 6948 "PIXCfgParser.cpp"
			}
			break;
		}
		case NORANDOMSEQ:
		{
			match(NORANDOMSEQ);
			if ( inputState->guessing==0 ) {
#line 2312 "pix.g"
				
				importer->addMessageToLog(
				QString("Warning: 'nat' and 'static' command option 'norandomseq' is not supported"));
				
#line 6961 "PIXCfgParser.cpp"
			}
			break;
		}
		case INT_CONST:
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
			max_conn = LT(1);
			match(INT_CONST);
			{
			if ((LA(1) == INT_CONST) && (_tokenSet_40.member(LA(2)))) {
				max_emb_conn = LT(1);
				match(INT_CONST);
			}
			else if ((_tokenSet_40.member(LA(1))) && (_tokenSet_41.member(LA(2)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			if ( inputState->guessing==0 ) {
#line 2319 "pix.g"
				
				importer->static_max_conn = max_conn->getText();
				if (max_emb_conn)
				importer->static_max_emb_conn = max_emb_conn->getText();
				
#line 7012 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_40);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::static_starts_with_hostaddr() {
	
	try {      // for error handling
		static_mapped_addr_match();
		static_real_addr_match();
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_42.member(LA(1)))) {
				static_command_common_last_parameters();
			}
			else {
				goto _loop268;
			}
			
		}
		_loop268:;
		} // ( ... )*
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

void PIXCfgParser::static_starts_with_tcp_udp() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  acl_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
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
#line 2216 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << " SERVICE TCP/UDP " << LT(0)->getText() << " ";
			
#line 7088 "PIXCfgParser.cpp"
		}
		static_mapped_addr_match();
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 2230 "pix.g"
			
			importer->mapped_port_spec = importer->tmp_port_spec_2;
			*dbg << "mapped port " << importer->mapped_port_spec << " ";
			
#line 7098 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case IPV4:
		case IPV6:
		{
			single_addr();
			if ( inputState->guessing==0 ) {
#line 2240 "pix.g"
				
				importer->real_a = importer->tmp_a;
				importer->real_nm = importer->tmp_nm;
				*dbg << "real: " << importer->real_a;
				
#line 7113 "PIXCfgParser.cpp"
			}
			tcp_udp_port_spec();
			if ( inputState->guessing==0 ) {
#line 2251 "pix.g"
				
				importer->real_port_spec = importer->tmp_port_spec_2;
				*dbg << "real port " << importer->real_port_spec << " ";
				
#line 7122 "PIXCfgParser.cpp"
			}
			break;
		}
		case ACCESS_LIST:
		{
			match(ACCESS_LIST);
			acl_name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 2258 "pix.g"
				
				importer->real_addr_acl = acl_name->getText();
				*dbg << "real: " << importer->real_addr_acl;
				
#line 7137 "PIXCfgParser.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_42.member(LA(1)))) {
				static_command_common_last_parameters();
			}
			else {
				goto _loop277;
			}
			
		}
		_loop277:;
		} // ( ... )*
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

void PIXCfgParser::static_mapped_addr_match() {
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IPV4:
		case IPV6:
		{
			single_addr();
			if ( inputState->guessing==0 ) {
#line 2183 "pix.g"
				
				importer->mapped_a = importer->tmp_a;
				importer->mapped_nm = importer->tmp_nm;
				*dbg << "mapped: " << importer->mapped_a;
				
#line 7186 "PIXCfgParser.cpp"
			}
			break;
		}
		case INTRFACE:
		{
			match(INTRFACE);
			if ( inputState->guessing==0 ) {
#line 2190 "pix.g"
				
				importer->mapped_a = "interface";
				importer->mapped_nm = "";
				*dbg << "mapped: " << importer->mapped_a;
				
#line 7200 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_43);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::static_real_addr_match() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  acl_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case IPV4:
		case IPV6:
		{
			single_addr();
			if ( inputState->guessing==0 ) {
#line 2201 "pix.g"
				
				importer->real_a = importer->tmp_a;
				importer->real_nm = importer->tmp_nm;
				*dbg << "real: " << importer->real_a;
				
#line 7238 "PIXCfgParser.cpp"
			}
			break;
		}
		case ACCESS_LIST:
		{
			match(ACCESS_LIST);
			acl_name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 2208 "pix.g"
				
				importer->real_addr_acl = acl_name->getText();
				*dbg << "real: " << importer->real_addr_acl;
				
#line 7253 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_44);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::static_command_common_last_parameters() {
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case INT_CONST:
		case TCP:
		case UDP:
		case DNS:
		case NORANDOMSEQ:
		{
			nat_and_static_command_common_last_parameters();
			break;
		}
		case NETMASK:
		{
			match(NETMASK);
			nm = LT(1);
			match(IPV4);
			if ( inputState->guessing==0 ) {
#line 2290 "pix.g"
				
				importer->real_nm = nm->getText();
				*dbg << "real netmask: " << importer->real_nm;
				
#line 7299 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_44);
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
	"\"pim\"",
	"\"network\"",
	"\"names\"",
	"\"name\"",
	"IPV4",
	"WORD",
	"IPV6",
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
	"\"pptp\"",
	"\"snp\"",
	"\"object\"",
	"\"description\"",
	"\"host\"",
	"\"range\"",
	"\"subnet\"",
	"\"service\"",
	"\"http\"",
	"\"ssh\"",
	"\"telnet\"",
	"\"icmp\"",
	"INT_CONST",
	"\"icmp6\"",
	"\"tcp\"",
	"\"udp\"",
	"\"source\"",
	"\"destination\"",
	"\"object-group\"",
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
	"\"dns\"",
	"\"call-home\"",
	"\"internal\"",
	"\"password-recovery\"",
	"\"resetinbound\"",
	"\"resetoutbound\"",
	"\"resetoutside\"",
	"\"no\"",
	"\"certificate\"",
	"\"PIX\"",
	"\"ASA\"",
	"\"FWSM\"",
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
	"\"interval\"",
	"\"fragments\"",
	"\"time-range\"",
	"\"controller\"",
	"\"outside\"",
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
	"\"management-only\"",
	"\"mac-address\"",
	"\"multicast\"",
	"PPPOE",
	"\"rip\"",
	"\"security-level\"",
	"\"shutdown\"",
	"\"address\"",
	"\"dhcp\"",
	"\"standby\"",
	"\"switchport\"",
	"\"access\"",
	"\"scopy\"",
	"\"version\"",
	"\"authentication-certificate\"",
	"\"server\"",
	"\"remark\"",
	"\"access-group\"",
	"COLON_COMMENT",
	"\"nat\"",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"COMMA",
	"\"global\"",
	"MINUS",
	"\"netmask\"",
	"\"static\"",
	"\"norandomseq\"",
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
	"STAR",
	"PLUS",
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

const unsigned long PIXCfgParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_0(_tokenSet_0_data_,8);
const unsigned long PIXCfgParser::_tokenSet_1_data_[] = { 536895346UL, 2172657788UL, 335UL, 54525984UL, 469762049UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "object" "service" "http" "ssh" "telnet" "icmp" "object-group" "crypto" 
// "dns" "no" "certificate" "PIX" "ASA" "FWSM" "hostname" "access-list" 
// "interface" "controller" LINE_COMMENT "exit" "nameif" "access-group" 
// COLON_COMMENT "nat" "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_1(_tokenSet_1_data_,12);
const unsigned long PIXCfgParser::_tokenSet_2_data_[] = { 0UL, 128UL, 0UL, 469762048UL, 0UL, 0UL, 0UL, 0UL };
// INT_CONST "aui" "auto" "bnc" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long PIXCfgParser::_tokenSet_3_data_[] = { 1073676864UL, 10048UL, 1024UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "pim" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" 
// "ospf" "pcp" "pptp" "snp" "object" "icmp" "icmp6" "tcp" "udp" "object-group" 
// "permit" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long PIXCfgParser::_tokenSet_4_data_[] = { 1073676864UL, 10048UL, 2048UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "pim" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" 
// "ospf" "pcp" "pptp" "snp" "object" "icmp" "icmp6" "tcp" "udp" "object-group" 
// "deny" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long PIXCfgParser::_tokenSet_5_data_[] = { 3758120818UL, 2172657791UL, 335UL, 54525984UL, 469762049UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "object" "description" "host" "range" "subnet" "service" "http" "ssh" 
// "telnet" "icmp" "object-group" "crypto" "dns" "no" "certificate" "PIX" 
// "ASA" "FWSM" "hostname" "access-list" "interface" "controller" LINE_COMMENT 
// "exit" "nameif" "access-group" COLON_COMMENT "nat" "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_5(_tokenSet_5_data_,12);
const unsigned long PIXCfgParser::_tokenSet_6_data_[] = { 536822336UL, 1984UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "pim" WORD "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" 
// "nos" "ospf" "pcp" "pptp" "snp" "icmp" INT_CONST "icmp6" "tcp" "udp" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_6(_tokenSet_6_data_,8);
const unsigned long PIXCfgParser::_tokenSet_7_data_[] = { 1073741824UL, 6307840UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "description" "group-object" "service-object" "port-object" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_7(_tokenSet_7_data_,8);
const unsigned long PIXCfgParser::_tokenSet_8_data_[] = { 2684362768UL, 8192UL, 0UL, 96UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 "object" "host" "object-group" "interface" "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_8(_tokenSet_8_data_,8);
const unsigned long PIXCfgParser::_tokenSet_9_data_[] = { 16UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_9(_tokenSet_9_data_,8);
const unsigned long PIXCfgParser::_tokenSet_10_data_[] = { 134275088UL, 16778936UL, 131392UL, 8388608UL, 0UL, 22UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD IPV6 "pptp" "http" "ssh" "telnet" INT_CONST "tcp" 
// "udp" "dns" "hostname" "access-list" "echo" "outside" MINUS "netmask" 
// "norandomseq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_10(_tokenSet_10_data_,12);
const unsigned long PIXCfgParser::_tokenSet_11_data_[] = { 536805952UL, 128UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "pim" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" 
// "ospf" "pcp" "pptp" "snp" INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_11(_tokenSet_11_data_,8);
const unsigned long PIXCfgParser::_tokenSet_12_data_[] = { 1610637170UL, 2172657788UL, 335UL, 54525984UL, 469762049UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "object" "description" "service" "http" "ssh" "telnet" "icmp" "object-group" 
// "crypto" "dns" "no" "certificate" "PIX" "ASA" "FWSM" "hostname" "access-list" 
// "interface" "controller" LINE_COMMENT "exit" "nameif" "access-group" 
// COLON_COMMENT "nat" "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_12(_tokenSet_12_data_,12);
const unsigned long PIXCfgParser::_tokenSet_13_data_[] = { 16UL, 0UL, 0UL, 3146112UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_13(_tokenSet_13_data_,8);
const unsigned long PIXCfgParser::_tokenSet_14_data_[] = { 16UL, 4097UL, 122880UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "range" "destination" "eq" "gt" "lt" "neq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_14(_tokenSet_14_data_,8);
const unsigned long PIXCfgParser::_tokenSet_15_data_[] = { 2684362768UL, 12289UL, 385024UL, 3146208UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 "object" "host" "range" "destination" "object-group" "eq" 
// "gt" "lt" "neq" "established" "interface" "any" "log" "log-input" "fragments" 
// "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_15(_tokenSet_15_data_,8);
const unsigned long PIXCfgParser::_tokenSet_16_data_[] = { 1610637170UL, 2172706940UL, 335UL, 54525984UL, 469762049UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "object" "description" "service" "http" "ssh" "telnet" "icmp" "object-group" 
// "group-object" "network-object" "crypto" "dns" "no" "certificate" "PIX" 
// "ASA" "FWSM" "hostname" "access-list" "interface" "controller" LINE_COMMENT 
// "exit" "nameif" "access-group" COLON_COMMENT "nat" "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_16(_tokenSet_16_data_,12);
const unsigned long PIXCfgParser::_tokenSet_17_data_[] = { 1610637170UL, 2172805244UL, 335UL, 54525984UL, 469762049UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "object" "description" "service" "http" "ssh" "telnet" "icmp" "object-group" 
// "group-object" "protocol-object" "crypto" "dns" "no" "certificate" "PIX" 
// "ASA" "FWSM" "hostname" "access-list" "interface" "controller" LINE_COMMENT 
// "exit" "nameif" "access-group" COLON_COMMENT "nat" "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_17(_tokenSet_17_data_,12);
const unsigned long PIXCfgParser::_tokenSet_18_data_[] = { 1610637170UL, 2172936316UL, 335UL, 54525984UL, 469762049UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "object" "description" "service" "http" "ssh" "telnet" "icmp" "object-group" 
// "group-object" "icmp-object" "crypto" "dns" "no" "certificate" "PIX" 
// "ASA" "FWSM" "hostname" "access-list" "interface" "controller" LINE_COMMENT 
// "exit" "nameif" "access-group" COLON_COMMENT "nat" "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_18(_tokenSet_18_data_,12);
const unsigned long PIXCfgParser::_tokenSet_19_data_[] = { 1610637170UL, 2178965628UL, 335UL, 54525984UL, 469762049UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "object" "description" "service" "http" "ssh" "telnet" "icmp" "object-group" 
// "group-object" "service-object" "port-object" "crypto" "dns" "no" "certificate" 
// "PIX" "ASA" "FWSM" "hostname" "access-list" "interface" "controller" 
// LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT "nat" "global" 
// "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_19(_tokenSet_19_data_,12);
const unsigned long PIXCfgParser::_tokenSet_20_data_[] = { 2684379152UL, 8321UL, 4294959104UL, 11534847UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD "object" "host" "range" INT_CONST "object-group" "eq" 
// "gt" "lt" "neq" "echo" "established" "alternate-address" "conversion-error" 
// "echo-reply" "information-reply" "information-request" "mask-reply" 
// "mask-request" "mobile-redirect" "parameter-problem" "redirect" "router-advertisement" 
// "router-solicitation" "source-quench" "time-exceeded" "timestamp-reply" 
// "timestamp-request" "traceroute" "unreachable" "interface" "any" "log" 
// "log-input" "fragments" "time-range" "outside" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_20(_tokenSet_20_data_,8);
const unsigned long PIXCfgParser::_tokenSet_21_data_[] = { 2684362752UL, 8192UL, 0UL, 96UL, 0UL, 0UL, 0UL, 0UL };
// IPV4 "object" "host" "object-group" "interface" "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_21(_tokenSet_21_data_,8);
const unsigned long PIXCfgParser::_tokenSet_22_data_[] = { 16UL, 0UL, 0UL, 1048960UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_22(_tokenSet_22_data_,8);
const unsigned long PIXCfgParser::_tokenSet_23_data_[] = { 16UL, 0UL, 0UL, 384UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_23(_tokenSet_23_data_,8);
const unsigned long PIXCfgParser::_tokenSet_24_data_[] = { 536895504UL, 8193UL, 385024UL, 11534720UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD "object" "range" "object-group" "eq" "gt" "lt" "neq" 
// "established" "log" "log-input" "fragments" "time-range" "outside" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_24(_tokenSet_24_data_,8);
const unsigned long PIXCfgParser::_tokenSet_25_data_[] = { 0UL, 1UL, 122880UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "range" "eq" "gt" "lt" "neq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_25(_tokenSet_25_data_,8);
const unsigned long PIXCfgParser::_tokenSet_26_data_[] = { 134234112UL, 184UL, 131136UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD "pptp" "http" "ssh" "telnet" INT_CONST "hostname" "echo" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_26(_tokenSet_26_data_,8);
const unsigned long PIXCfgParser::_tokenSet_27_data_[] = { 536870928UL, 8193UL, 385024UL, 3146112UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "object" "range" "object-group" "eq" "gt" "lt" "neq" "established" 
// "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_27(_tokenSet_27_data_,8);
const unsigned long PIXCfgParser::_tokenSet_28_data_[] = { 671113074UL, 2172657917UL, 516431UL, 58720160UL, 469762049UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "pptp" "object" "range" "service" "http" "ssh" "telnet" "icmp" INT_CONST 
// "object-group" "crypto" "dns" "no" "certificate" "PIX" "ASA" "FWSM" 
// "hostname" "access-list" "eq" "gt" "lt" "neq" "echo" "established" "interface" 
// "log" "log-input" "alerts" "critical" "debugging" "emergencies" "errors" 
// "informational" "notifications" "warnings" "disable" "inactive" "interval" 
// "fragments" "time-range" "controller" LINE_COMMENT "exit" "nameif" "access-group" 
// COLON_COMMENT "nat" "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_28(_tokenSet_28_data_,12);
const unsigned long PIXCfgParser::_tokenSet_29_data_[] = { 671113074UL, 2172657916UL, 131407UL, 58720160UL, 469762049UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "pptp" "object" "service" "http" "ssh" "telnet" "icmp" INT_CONST "object-group" 
// "crypto" "dns" "no" "certificate" "PIX" "ASA" "FWSM" "hostname" "access-list" 
// "echo" "interface" "log" "log-input" "alerts" "critical" "debugging" 
// "emergencies" "errors" "informational" "notifications" "warnings" "disable" 
// "inactive" "interval" "fragments" "time-range" "controller" LINE_COMMENT 
// "exit" "nameif" "access-group" COLON_COMMENT "nat" "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_29(_tokenSet_29_data_,12);
const unsigned long PIXCfgParser::_tokenSet_30_data_[] = { 2684362752UL, 8193UL, 122880UL, 96UL, 0UL, 0UL, 0UL, 0UL };
// IPV4 "object" "host" "range" "object-group" "eq" "gt" "lt" "neq" "interface" 
// "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_30(_tokenSet_30_data_,8);
const unsigned long PIXCfgParser::_tokenSet_31_data_[] = { 671113232UL, 8377UL, 516160UL, 11534720UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD "pptp" "object" "range" "http" "ssh" "telnet" INT_CONST 
// "object-group" "hostname" "eq" "gt" "lt" "neq" "echo" "established" 
// "log" "log-input" "fragments" "time-range" "outside" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_31(_tokenSet_31_data_,8);
const unsigned long PIXCfgParser::_tokenSet_32_data_[] = { 2818629648UL, 16791225UL, 516416UL, 3146208UL, 0UL, 20UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD IPV6 "pptp" "object" "host" "range" "http" "ssh" "telnet" 
// INT_CONST "tcp" "udp" "destination" "object-group" "dns" "hostname" 
// "access-list" "eq" "gt" "lt" "neq" "echo" "established" "interface" 
// "any" "log" "log-input" "fragments" "time-range" "netmask" "norandomseq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_32(_tokenSet_32_data_,12);
const unsigned long PIXCfgParser::_tokenSet_33_data_[] = { 2684387186UL, 2172657917UL, 4294959439UL, 66060799UL, 3691118593UL, 9UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" IPV4 
// WORD "object" "host" "range" "service" "http" "ssh" "telnet" "icmp" 
// INT_CONST "object-group" "crypto" "dns" "no" "certificate" "PIX" "ASA" 
// "FWSM" "hostname" "access-list" "eq" "gt" "lt" "neq" "echo" "established" 
// "alternate-address" "conversion-error" "echo-reply" "information-reply" 
// "information-request" "mask-reply" "mask-request" "mobile-redirect" 
// "parameter-problem" "redirect" "router-advertisement" "router-solicitation" 
// "source-quench" "time-exceeded" "timestamp-reply" "timestamp-request" 
// "traceroute" "unreachable" "interface" "any" "log" "log-input" "fragments" 
// "time-range" "controller" "outside" LINE_COMMENT "exit" "nameif" "dhcp" 
// "access-group" COLON_COMMENT "nat" CLOSING_PAREN COMMA "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_33(_tokenSet_33_data_,12);
const unsigned long PIXCfgParser::_tokenSet_34_data_[] = { 1108345408UL, 2147483648UL, 0UL, 0UL, 589823UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "pim" "igmp" "ospf" "description" "no" "nameif" "vlan" "speed" 
// "duplex" "ddns" "forward" "delay" "hold-time" "ipv6" "management-only" 
// "mac-address" "multicast" PPPOE "rip" "security-level" "shutdown" "switchport" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_34(_tokenSet_34_data_,12);
const unsigned long PIXCfgParser::_tokenSet_35_data_[] = { 1108345408UL, 2147483648UL, 0UL, 50331648UL, 589823UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "pim" "igmp" "ospf" "description" "no" LINE_COMMENT "exit" "nameif" 
// "vlan" "speed" "duplex" "ddns" "forward" "delay" "hold-time" "ipv6" 
// "management-only" "mac-address" "multicast" PPPOE "rip" "security-level" 
// "shutdown" "switchport" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_35(_tokenSet_35_data_,12);
const unsigned long PIXCfgParser::_tokenSet_36_data_[] = { 16384UL, 0UL, 0UL, 8388608UL, 0UL, 0UL, 0UL, 0UL };
// WORD "outside" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_36(_tokenSet_36_data_,8);
const unsigned long PIXCfgParser::_tokenSet_37_data_[] = { 0UL, 16778880UL, 0UL, 8388608UL, 0UL, 16UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// INT_CONST "tcp" "udp" "dns" "outside" "norandomseq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_37(_tokenSet_37_data_,12);
const unsigned long PIXCfgParser::_tokenSet_38_data_[] = { 16UL, 16778880UL, 0UL, 8388608UL, 0UL, 16UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE INT_CONST "tcp" "udp" "dns" "outside" "norandomseq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_38(_tokenSet_38_data_,12);
const unsigned long PIXCfgParser::_tokenSet_39_data_[] = { 3758120818UL, 2172659455UL, 335UL, 62914592UL, 469762049UL, 25UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" WORD 
// "object" "description" "host" "range" "subnet" "service" "http" "ssh" 
// "telnet" "icmp" INT_CONST "tcp" "udp" "object-group" "crypto" "dns" 
// "no" "certificate" "PIX" "ASA" "FWSM" "hostname" "access-list" "interface" 
// "controller" "outside" LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT 
// "nat" "global" "static" "norandomseq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_39(_tokenSet_39_data_,12);
const unsigned long PIXCfgParser::_tokenSet_40_data_[] = { 16UL, 16778880UL, 0UL, 8388608UL, 0UL, 20UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE INT_CONST "tcp" "udp" "dns" "outside" "netmask" "norandomseq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_40(_tokenSet_40_data_,12);
const unsigned long PIXCfgParser::_tokenSet_41_data_[] = { 3758129010UL, 2172659455UL, 335UL, 62914592UL, 469762049UL, 29UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "pim" "network" "names" "name" IPV4 
// WORD "object" "description" "host" "range" "subnet" "service" "http" 
// "ssh" "telnet" "icmp" INT_CONST "tcp" "udp" "object-group" "crypto" 
// "dns" "no" "certificate" "PIX" "ASA" "FWSM" "hostname" "access-list" 
// "interface" "controller" "outside" LINE_COMMENT "exit" "nameif" "access-group" 
// COLON_COMMENT "nat" "global" "netmask" "static" "norandomseq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_41(_tokenSet_41_data_,12);
const unsigned long PIXCfgParser::_tokenSet_42_data_[] = { 0UL, 16778880UL, 0UL, 0UL, 0UL, 20UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// INT_CONST "tcp" "udp" "dns" "netmask" "norandomseq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_42(_tokenSet_42_data_,12);
const unsigned long PIXCfgParser::_tokenSet_43_data_[] = { 134275072UL, 184UL, 131392UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IPV4 WORD IPV6 "pptp" "http" "ssh" "telnet" INT_CONST "hostname" "access-list" 
// "echo" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_43(_tokenSet_43_data_,8);
const unsigned long PIXCfgParser::_tokenSet_44_data_[] = { 16UL, 16778880UL, 0UL, 0UL, 0UL, 20UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE INT_CONST "tcp" "udp" "dns" "netmask" "norandomseq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_44(_tokenSet_44_data_,12);



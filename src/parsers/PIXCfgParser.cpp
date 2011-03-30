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
	Tracer traceInOut(this, "cfgfile");
	
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
			case NAT:
			{
				nat_old_top_level_command();
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
	Tracer traceInOut(this, "comment");
	
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
	Tracer traceInOut(this, "version");
	
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
#line 824 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setDiscoveredVersion(LT(0)->getText());
			*dbg << "VERSION " << LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 290 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "hostname");
	
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
#line 834 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setHostName( LT(0)->getText() );
			*dbg << "HOSTNAME "
			<< "LT0=" << LT(0)->getText()
			<< std::endl;
			
#line 335 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "community_list_command");
	
	try {      // for error handling
		match(IP);
		match(COMMUNITY_LIST);
		if ( inputState->guessing==0 ) {
#line 193 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 359 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "unknown_ip_command");
	
	try {      // for error handling
		match(IP);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 793 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 383 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "intrface");
	
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
	Tracer traceInOut(this, "nameif_top_level");
#line 1387 "pix.g"
	
	std::string intf_name, intf_label, sec_level;
	
#line 430 "PIXCfgParser.cpp"
	
	try {      // for error handling
		match(NAMEIF);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 1392 "pix.g"
			intf_name = LT(0)->getText();
#line 438 "PIXCfgParser.cpp"
		}
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1393 "pix.g"
			intf_label = LT(0)->getText();
#line 444 "PIXCfgParser.cpp"
		}
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 1394 "pix.g"
			sec_level = LT(0)->getText();
#line 450 "PIXCfgParser.cpp"
		}
		if ( inputState->guessing==0 ) {
#line 1395 "pix.g"
			
			importer->setInterfaceParametes(intf_name, intf_label, sec_level);
			*dbg << " NAMEIF: "
			<< intf_name << " "
			<< intf_label << " "
			<< sec_level << std::endl;
			
#line 461 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "controller");
	
	try {      // for error handling
		match(CONTROLLER);
		if ( inputState->guessing==0 ) {
#line 1299 "pix.g"
			
			importer->clearCurrentInterface();
			consumeUntil(NEWLINE);
			
#line 485 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "access_list_commands");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ACCESS_LIST);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 846 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newUnidirRuleSet(
			name->getText(), libfwbuilder::Policy::TYPENAME );
			*dbg << name->getLine() << ":"
			<< " ACL ext " << name->getText() << std::endl;
			
#line 516 "PIXCfgParser.cpp"
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
#line 869 "pix.g"
			
			*dbg << LT(0)->getLine() << ":"
			<< " ACL line end" << std::endl << std::endl;
			
#line 560 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "ssh_command");
	
	try {      // for error handling
		match(SSH);
		if ( inputState->guessing==0 ) {
#line 1634 "pix.g"
			
			importer->clear();
			
#line 583 "PIXCfgParser.cpp"
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
#line 1641 "pix.g"
				
				importer->SaveTmpAddrToSrc();
				
#line 609 "PIXCfgParser.cpp"
			}
			interface_label();
			}
			if ( inputState->guessing==0 ) {
#line 1646 "pix.g"
				
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
				
#line 631 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "telnet_command");
	
	try {      // for error handling
		match(TELNET);
		if ( inputState->guessing==0 ) {
#line 1666 "pix.g"
			
			importer->clear();
			
#line 662 "PIXCfgParser.cpp"
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
#line 1673 "pix.g"
				
				importer->SaveTmpAddrToSrc();
				
#line 688 "PIXCfgParser.cpp"
			}
			interface_label();
			}
			if ( inputState->guessing==0 ) {
#line 1678 "pix.g"
				
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
				
#line 710 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "icmp_top_level_command");
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
#line 1707 "pix.g"
				
				consumeUntil(NEWLINE);
				
#line 749 "PIXCfgParser.cpp"
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
#line 1714 "pix.g"
				
				importer->clear();
				
#line 783 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 1718 "pix.g"
				
				importer->SaveTmpAddrToSrc();
				
#line 791 "PIXCfgParser.cpp"
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
#line 1723 "pix.g"
				
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
				
#line 832 "PIXCfgParser.cpp"
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

void PIXCfgParser::nat_old_top_level_command() {
	Tracer traceInOut(this, "nat_old_top_level_command");
	
	try {      // for error handling
		match(NAT);
		match(OPENING_PAREN);
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1809 "pix.g"
			importer->prenat_interface = LT(0)->getText();
#line 864 "PIXCfgParser.cpp"
		}
		match(CLOSING_PAREN);
		if ( inputState->guessing==0 ) {
#line 1811 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newUnidirRuleSet("nat", libfwbuilder::NAT::TYPENAME );
			*dbg << " SNAT rule " << std::endl;
			importer->rule_type = libfwbuilder::NATRule::SNAT;
			
			
#line 877 "PIXCfgParser.cpp"
		}
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1822 "pix.g"
			
			importer->nat_num = LT(0)->getText();
			
#line 885 "PIXCfgParser.cpp"
		}
		nat_addr_match();
		nat_command_last_parameters();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 1834 "pix.g"
			
			importer->pushNATRule();
			
#line 895 "PIXCfgParser.cpp"
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

void PIXCfgParser::global_top_level_command() {
	Tracer traceInOut(this, "global_top_level_command");
	ANTLR_USE_NAMESPACE(antlr)RefToken  num = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(GLOBAL);
		match(OPENING_PAREN);
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1880 "pix.g"
			importer->global_interface = LT(0)->getText();
#line 919 "PIXCfgParser.cpp"
		}
		match(CLOSING_PAREN);
		num = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1882 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->global_pool_num = num->getText();
			*dbg << " global address pool "
			<< importer->global_pool_num
			<< " "
			<< importer->global_interface
			<< std::endl;
			
#line 936 "PIXCfgParser.cpp"
		}
		if ( inputState->guessing==0 ) {
#line 1892 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 943 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "static_top_level_command");
	ANTLR_USE_NAMESPACE(antlr)RefToken  prenat_intf = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  postnat_intf = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(STATIC);
		match(OPENING_PAREN);
		prenat_intf = LT(1);
		match(WORD);
		match(COMMA);
		postnat_intf = LT(1);
		match(WORD);
		match(CLOSING_PAREN);
		if ( inputState->guessing==0 ) {
#line 1900 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newUnidirRuleSet("nat", libfwbuilder::NAT::TYPENAME );
			*dbg << " DNAT rule " << std::endl;
			importer->rule_type = libfwbuilder::NATRule::DNAT;
			importer->prenat_interface = prenat_intf->getText();
			importer->postnat_interface = postnat_intf->getText();
			
#line 981 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case HOST:
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
#line 1919 "pix.g"
			
			importer->pushNATRule();
			
#line 1009 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "access_group");
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
#line 1782 "pix.g"
			
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
			
#line 1049 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "exit");
	
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
	Tracer traceInOut(this, "certificate");
	
	try {      // for error handling
		match(CERTIFICATE);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 816 "pix.g"
			
			consumeUntil(NEWLINE);
			consumeUntil(QUIT);
			
#line 1090 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "quit");
	
	try {      // for error handling
		match(QUIT);
		if ( inputState->guessing==0 ) {
#line 186 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1113 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "names_section");
	
	try {      // for error handling
		match(NAMES);
		if ( inputState->guessing==0 ) {
#line 207 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addMessageToLog(
			"Parser warning: \"names\" section detected. "
			"Import of configuration that uses \"names\" "
			"is not supported at this time");
			
#line 1140 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "name_entry");
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
#line 217 "pix.g"
			
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
			
#line 1200 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "named_object_network");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT);
		match(NETWORK);
		name = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 250 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newNamedObjectAddress(name->getText());
			*dbg << name->getLine() << ":"
			<< " Named Object " << name->getText() << std::endl;
			
#line 1232 "PIXCfgParser.cpp"
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
				goto _loop14;
			}
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
	Tracer traceInOut(this, "named_object_service");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT);
		match(SERVICE);
		name = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 354 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newNamedObjectService(name->getText());
			*dbg << name->getLine() << ":"
			<< " Named Object " << name->getText() << std::endl;
			
#line 1290 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == DESCRIPTION || LA(1) == SERVICE)) {
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
	Tracer traceInOut(this, "object_group_network");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(NETWORK);
		name = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 470 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupNetwork(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1334 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == DESCRIPTION || LA(1) == GROUP_OBJECT || LA(1) == NETWORK_OBJECT)) {
				object_group_network_parameters();
			}
			else {
				goto _loop49;
			}
			
		}
		_loop49:;
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
	Tracer traceInOut(this, "object_group_service");
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
#line 692 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupService(name->getText());
			if (tcp) importer->setObjectGroupServiceProtocol("tcp");
			if (udp) importer->setObjectGroupServiceProtocol("udp");
			if (tcpudp) importer->setObjectGroupServiceProtocol("tcp-udp");
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1414 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
				object_group_service_parameters();
			}
			else {
				goto _loop81;
			}
			
		}
		_loop81:;
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
	Tracer traceInOut(this, "object_group_protocol");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(PROTOCOL);
		name = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 568 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupProtocol(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1458 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt61=0;
		for (;;) {
			if ((LA(1) == DESCRIPTION || LA(1) == GROUP_OBJECT || LA(1) == PROTOCOL_OBJECT)) {
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
	Tracer traceInOut(this, "object_group_icmp_8_0");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(ICMP_OBJECT);
		name = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 623 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupICMP(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1504 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == DESCRIPTION || LA(1) == GROUP_OBJECT || LA(1) == ICMP_OBJECT)) {
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
	Tracer traceInOut(this, "object_group_icmp_8_3");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(ICMP_TYPE);
		name = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 636 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupICMP(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1548 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == DESCRIPTION || LA(1) == GROUP_OBJECT || LA(1) == ICMP_OBJECT)) {
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
	Tracer traceInOut(this, "crypto");
	
	try {      // for error handling
		match(CRYPTO);
		if ( inputState->guessing==0 ) {
#line 786 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1583 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "no_commands");
	
	try {      // for error handling
		match(NO);
		if ( inputState->guessing==0 ) {
#line 807 "pix.g"
			
			*dbg << " TOP LEVEL \"NO\" COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 1608 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "timeout_command");
	
	try {      // for error handling
		match(TIMEOUT);
		if ( inputState->guessing==0 ) {
#line 200 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1631 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "unknown_command");
	
	try {      // for error handling
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 800 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1654 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "ip_protocol_names");
	
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

void PIXCfgParser::named_object_nat() {
	Tracer traceInOut(this, "named_object_nat");
	
	try {      // for error handling
		match(NAT);
		match(OPENING_PAREN);
		interface_label();
		if ( inputState->guessing==0 ) {
#line 278 "pix.g"
			
			importer->addMessageToLog(
			"Parser warning: "
			"Import of named objects with \"nat\" command "
			"is not supported at this time");
			consumeUntil(NEWLINE);
			
#line 1781 "PIXCfgParser.cpp"
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

void PIXCfgParser::named_object_description() {
	Tracer traceInOut(this, "named_object_description");
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 288 "pix.g"
			
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
			
#line 1813 "PIXCfgParser.cpp"
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

void PIXCfgParser::named_object_network_parameters() {
	Tracer traceInOut(this, "named_object_network_parameters");
	
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
			recover(ex,_tokenSet_7);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::host_addr() {
	Tracer traceInOut(this, "host_addr");
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
#line 303 "pix.g"
			
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
			
#line 1912 "PIXCfgParser.cpp"
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

void PIXCfgParser::range_addr() {
	Tracer traceInOut(this, "range_addr");
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
#line 322 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->tmp_range_1 = r1->getText();
			importer->tmp_range_2 = r2->getText();
			importer->commitNamedAddressRangeObject();
			*dbg << r1->getText() << "/" << r2->getText();
			
#line 1947 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "subnet_addr");
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
#line 332 "pix.g"
			
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
			
#line 2012 "PIXCfgParser.cpp"
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

void PIXCfgParser::interface_label() {
	Tracer traceInOut(this, "interface_label");
	
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
			recover(ex,_tokenSet_10);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::named_object_service_parameters() {
	Tracer traceInOut(this, "named_object_service_parameters");
	
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
		else if ((LA(1) == DESCRIPTION)) {
			named_object_description();
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
	Tracer traceInOut(this, "service_icmp");
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
#line 386 "pix.g"
				
				importer->icmp_type = LT(0)->getText();
				
#line 2114 "PIXCfgParser.cpp"
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
#line 391 "pix.g"
				
				importer->icmp_spec = LT(0)->getText();
				
#line 2144 "PIXCfgParser.cpp"
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
#line 395 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->commitNamedICMPServiceObject();
			*dbg << "NAMED OBJECT SERVICE ICMP " << LT(0)->getText() << " ";
			
#line 2161 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "service_icmp6");
	
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
#line 403 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addMessageToLog("Parser warning: "
			"Import of IPv6 addresses and servcies "
			"is not supported at this time");
			*dbg << "NAMED OBJECT SERVICE ICMP6 " << LT(0)->getText() << " ";
			consumeUntil(NEWLINE);
			
#line 2208 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "service_tcp_udp");
	
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
#line 414 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
			
#line 2250 "PIXCfgParser.cpp"
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
#line 420 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->commitNamedTCPUDPServiceObject();
			
#line 2303 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "service_other");
	
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
#line 439 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->protocol = LT(0)->getText();
			importer->commitNamedIPServiceObject();
			*dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
			
#line 2361 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "service_unknown");
	
	try {      // for error handling
		match(SERVICE);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 454 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->protocol = "ip";
			importer->commitNamedIPServiceObject();
			std::string err = "Parser warning: Unknown service name " +
			LT(0)->getText();
			importer->setNamedObjectDescription(err);
			importer->addMessageToLog(err);
			*dbg << "UNKNOWN SERVICE " << LT(0)->getText() << " ";
			
#line 2392 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "icmp_names");
	
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
	Tracer traceInOut(this, "src_port_spec");
	
	try {      // for error handling
		match(SOURCE);
		xoperator();
		if ( inputState->guessing==0 ) {
#line 427 "pix.g"
			
			importer->SaveTmpPortToSrc();
			
#line 2534 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "dst_port_spec");
	
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
#line 433 "pix.g"
			
			importer->SaveTmpPortToDst();
			
#line 2578 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "xoperator");
	
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
	Tracer traceInOut(this, "object_group_network_parameters");
	
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
	Tracer traceInOut(this, "object_group_description");
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 494 "pix.g"
			
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
			
#line 2683 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "group_object");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(GROUP_OBJECT);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 509 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addNamedObjectToObjectGroup(name->getText());
			*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
			
#line 2712 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "network_object");
	ANTLR_USE_NAMESPACE(antlr)RefToken  a = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  v6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  h = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  hv6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(NETWORK_OBJECT);
		if ( inputState->guessing==0 ) {
#line 518 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 2742 "PIXCfgParser.cpp"
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
#line 524 "pix.g"
				
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
				
#line 2790 "PIXCfgParser.cpp"
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
#line 541 "pix.g"
				
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
				
#line 2834 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 558 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 2849 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "object_group_protocol_parameters");
	
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
	Tracer traceInOut(this, "protocol_object");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(PROTOCOL_OBJECT);
		if ( inputState->guessing==0 ) {
#line 592 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 2921 "PIXCfgParser.cpp"
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
#line 598 "pix.g"
				
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->protocol = LT(0)->getText();
				importer->addIPServiceToObjectGroup();
				*dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
				
#line 3000 "PIXCfgParser.cpp"
			}
			break;
		}
		case ICMP6:
		{
			match(ICMP6);
			if ( inputState->guessing==0 ) {
#line 606 "pix.g"
				
				importer->addMessageToLog(
				"Parser warning: IPv6 import is not supported. ");
				consumeUntil(NEWLINE);
				
#line 3014 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 613 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 3029 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "object_group_icmp_parameters");
	
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
	Tracer traceInOut(this, "icmp_object");
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ICMP_OBJECT);
		if ( inputState->guessing==0 ) {
#line 660 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 3102 "PIXCfgParser.cpp"
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
#line 667 "pix.g"
					
					importer->icmp_type = LT(0)->getText();
					
#line 3138 "PIXCfgParser.cpp"
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
#line 672 "pix.g"
					
					importer->icmp_spec = LT(0)->getText();
					
#line 3168 "PIXCfgParser.cpp"
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
#line 676 "pix.g"
				
				importer->addICMPServiceToObjectGroup();
				*dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
				
#line 3184 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 682 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 3199 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "object_group_service_parameters");
	
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
	Tracer traceInOut(this, "service_object");
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SERVICE_OBJECT);
		if ( inputState->guessing==0 ) {
#line 721 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 3277 "PIXCfgParser.cpp"
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
#line 727 "pix.g"
				
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->protocol = LT(0)->getText();
				importer->addIPServiceToObjectGroup();
				*dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
				
#line 3338 "PIXCfgParser.cpp"
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
#line 735 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << " SERVICE TCP/UDP" << LT(0)->getText() << " ";
				
#line 3375 "PIXCfgParser.cpp"
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
#line 741 "pix.g"
				
				importer->addTCPUDPServiceToObjectGroup();
				
#line 3427 "PIXCfgParser.cpp"
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
#line 748 "pix.g"
					
					importer->icmp_type = LT(0)->getText();
					
#line 3445 "PIXCfgParser.cpp"
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
#line 753 "pix.g"
					
					importer->icmp_spec = LT(0)->getText();
					
#line 3475 "PIXCfgParser.cpp"
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
#line 757 "pix.g"
				
				importer->addICMPServiceToObjectGroup();
				*dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
				
#line 3491 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 763 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 3506 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "port_object");
#line 770 "pix.g"
	
	importer->tmp_port_spec = "";
	importer->tmp_port_spec_2 = "";
	
#line 3534 "PIXCfgParser.cpp"
	
	try {      // for error handling
		match(PORT_OBJECT);
		xoperator();
		if ( inputState->guessing==0 ) {
#line 775 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			*dbg << " PORT OBJECT TCP/UDP " << LT(0)->getText() << " " << std::endl;
			importer->SaveTmpPortToDst();
			importer->addTCPUDPServiceToObjectGroup();
			*dbg << std::endl;
			
#line 3548 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "permit_extended");
	
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
#line 877 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 3591 "PIXCfgParser.cpp"
		}
		rule_extended();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 884 "pix.g"
			
			importer->pushRule();
			
#line 3600 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "deny_extended");
	
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
#line 890 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 3643 "PIXCfgParser.cpp"
		}
		rule_extended();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 897 "pix.g"
			
			importer->pushRule();
			
#line 3652 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "permit_standard");
	
	try {      // for error handling
		match(STANDARD);
		match(PERMIT);
		if ( inputState->guessing==0 ) {
#line 903 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 3679 "PIXCfgParser.cpp"
		}
		rule_standard();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 910 "pix.g"
			
			importer->pushRule();
			
#line 3688 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "deny_standard");
	
	try {      // for error handling
		match(STANDARD);
		match(DENY);
		if ( inputState->guessing==0 ) {
#line 916 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 3715 "PIXCfgParser.cpp"
		}
		rule_standard();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 923 "pix.g"
			
			importer->pushRule();
			
#line 3724 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "remark");
	
	try {      // for error handling
		match(REMARK);
		if ( inputState->guessing==0 ) {
#line 1764 "pix.g"
			
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
			
#line 3757 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "rule_extended");
	
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
#line 948 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 3800 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 949 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 3806 "PIXCfgParser.cpp"
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
#line 955 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 3876 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 959 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 3882 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 960 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 3888 "PIXCfgParser.cpp"
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
#line 968 "pix.g"
			
			*dbg << std::endl;
			
#line 4006 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "rule_standard");
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 933 "pix.g"
			
			importer->tmp_a = "0.0.0.0";
			importer->tmp_nm = "0.0.0.0";
			importer->SaveTmpAddrToSrc();
			
#line 4030 "PIXCfgParser.cpp"
		}
		hostaddr_expr();
		if ( inputState->guessing==0 ) {
#line 939 "pix.g"
			
			importer->SaveTmpAddrToDst();
			*dbg << "(dst) " << std::endl;
			
#line 4039 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "hostaddr_expr");
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
#line 1209 "pix.g"
				
				importer->tmp_a = LT(0)->getText();
				importer->tmp_nm = "interface";
				*dbg << "object " << LT(0)->getText() << " ";
				
#line 4072 "PIXCfgParser.cpp"
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
#line 1216 "pix.g"
				
				importer->tmp_a = name->getText();
				importer->tmp_nm = "";
				*dbg << "object " << name->getText() << " ";
				
#line 4108 "PIXCfgParser.cpp"
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
#line 1223 "pix.g"
				
				importer->tmp_a = h->getText();
				importer->tmp_nm = "255.255.255.255";
				*dbg << h->getText() << "/255.255.255.255";
				
#line 4126 "PIXCfgParser.cpp"
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
#line 1230 "pix.g"
				
				importer->tmp_a = a->getText();
				importer->tmp_nm = m->getText();
				*dbg << a->getText() << "/" << m->getText();
				
#line 4145 "PIXCfgParser.cpp"
			}
			break;
		}
		case ANY:
		{
			match(ANY);
			if ( inputState->guessing==0 ) {
#line 1237 "pix.g"
				
				importer->tmp_a = "0.0.0.0";
				importer->tmp_nm = "0.0.0.0";
				*dbg << "0.0.0.0/0.0.0.0";
				
#line 4159 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "ip_protocols");
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
#line 1156 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 4241 "PIXCfgParser.cpp"
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
#line 1162 "pix.g"
				
				importer->protocol = name->getText();
				*dbg << "protocol " << name->getText() << " ";
				
#line 4276 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "time_range");
	ANTLR_USE_NAMESPACE(antlr)RefToken  tr_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(TIME_RANGE);
		tr_name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 1289 "pix.g"
			
			importer->time_range_name = tr_name->getText();
			*dbg << "time_range " << tr_name->getText() << " ";
			
#line 4311 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "fragments");
	
	try {      // for error handling
		match(FRAGMENTS);
		if ( inputState->guessing==0 ) {
#line 1282 "pix.g"
			
			importer->fragments = true;
			*dbg << "fragments ";
			
#line 4335 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "log");
	
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
#line 1248 "pix.g"
			
			importer->logging = true;
			
#line 4375 "PIXCfgParser.cpp"
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
#line 1265 "pix.g"
				importer->log_level = LT(0)->getText();
#line 4458 "PIXCfgParser.cpp"
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
#line 1270 "pix.g"
				importer->log_interval = LT(0)->getText();
#line 4484 "PIXCfgParser.cpp"
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
#line 1273 "pix.g"
			
			//        if (importer->log_level == "log") importer->log_level = "";
			//        if (importer->log_interval == "log") importer->log_interval = "";
			*dbg << "logging level '" << importer->log_level
			<< "' interval '" << importer->log_interval << "'";
			
#line 4507 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "icmp_spec");
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
#line 1172 "pix.g"
				
				importer->icmp_type = icmp_type->getText();
				importer->icmp_code = icmp_code->getText();
				importer->icmp_spec = "";
				*dbg << icmp_type->getText() << " "
				<< icmp_code->getText() << " ";
				
#line 4545 "PIXCfgParser.cpp"
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
#line 1181 "pix.g"
				
				importer->icmp_spec = LT(0)->getText();
				*dbg << LT(0)->getText() << " ";
				
#line 4576 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "tcp_udp_rule_extended");
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
#line 975 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << "protocol " << LT(0)->getText() << " ";
			
#line 4627 "PIXCfgParser.cpp"
		}
		hostaddr_expr();
		if ( inputState->guessing==0 ) {
#line 979 "pix.g"
			importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 4633 "PIXCfgParser.cpp"
		}
		{
		bool synPredMatched123 = false;
		if (((_tokenSet_21.member(LA(1))) && (_tokenSet_24.member(LA(2))))) {
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
#line 989 "pix.g"
					
					importer->src_port_spec = src_grp_name->getText();
					*dbg << "src port spec: "
					<< src_grp_name->getText() << std::endl;
					
#line 4665 "PIXCfgParser.cpp"
				}
				hostaddr_expr_1();
				if ( inputState->guessing==0 ) {
#line 997 "pix.g"
					
					importer->SaveTmpAddrToDst();
					*dbg << "(dst) ";
					
#line 4674 "PIXCfgParser.cpp"
				}
				acl_tcp_udp_dst_port_spec();
			}
			else if ((_tokenSet_21.member(LA(1))) && (_tokenSet_24.member(LA(2)))) {
				hostaddr_expr_2();
				if ( inputState->guessing==0 ) {
#line 1006 "pix.g"
					
					importer->SaveTmpAddrToDst();
					*dbg << "(dst) ";
					
#line 4686 "PIXCfgParser.cpp"
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
#line 1015 "pix.g"
				
				// looks like "object foo" at this point can only be dest addr.
				// (judging by cli prompts on 8.3)
				importer->tmp_a = dst_addr_name->getText();
				importer->tmp_nm = "";
				importer->SaveTmpAddrToDst();
				*dbg << "dst addr object " << dst_addr_name->getText() << " ";
				
#line 4732 "PIXCfgParser.cpp"
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
#line 1029 "pix.g"
					
					importer->SaveTmpPortToSrc();
					
#line 4751 "PIXCfgParser.cpp"
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
#line 1033 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 4774 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "hostaddr_expr_1");
	
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
	Tracer traceInOut(this, "acl_tcp_udp_dst_port_spec");
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
#line 1051 "pix.g"
				
				importer->dst_port_spec = dst_port_group_name->getText();
				*dbg << "dst port spec: "
				<< dst_port_group_name->getText() << std::endl;
				
#line 4888 "PIXCfgParser.cpp"
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
#line 1061 "pix.g"
				
				importer->dst_port_spec = dst_port_obj_name->getText();
				*dbg << "dst addr object " << dst_port_obj_name->getText()
				<< std::endl;
				
#line 4926 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "hostaddr_expr_2");
	
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
	Tracer traceInOut(this, "acl_xoperator_dst");
	
	try {      // for error handling
		xoperator();
		if ( inputState->guessing==0 ) {
#line 1076 "pix.g"
			
			importer->SaveTmpPortToDst();
			
#line 5055 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "established");
	
	try {      // for error handling
		match(ESTABLISHED);
		if ( inputState->guessing==0 ) {
#line 1145 "pix.g"
			
			importer->established = true;
			*dbg << "established ";
			
#line 5079 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "hostaddr_expr_3");
	
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
	Tracer traceInOut(this, "single_port_op");
	
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
#line 1086 "pix.g"
			
			importer->tmp_port_op = LT(0)->getText();
			*dbg << LT(0)->getText() << " ";
			
#line 5146 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "port_range");
	
	try {      // for error handling
		match(RANGE);
		pair_of_ports_spec();
		if ( inputState->guessing==0 ) {
#line 1101 "pix.g"
			
			importer->tmp_port_op = "range";
			*dbg << "range " << importer->tmp_port_spec;
			
#line 5172 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "port_spec");
	
	try {      // for error handling
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1094 "pix.g"
			
			importer->tmp_port_spec = std::string(" ") + importer->tmp_port_spec_2;
			*dbg << LT(0)->getText() << " " << importer->tmp_port_spec;
			
#line 5196 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "tcp_udp_port_spec");
	
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
#line 1125 "pix.g"
			
			importer->tmp_port_spec_2 = LT(0)->getText();
			
#line 5246 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "pair_of_ports_spec");
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 1108 "pix.g"
			
			importer->tmp_port_spec_2 = "";
			
#line 5268 "PIXCfgParser.cpp"
		}
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1112 "pix.g"
			
			importer->tmp_port_spec += importer->tmp_port_spec_2;
			
#line 5276 "PIXCfgParser.cpp"
		}
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1116 "pix.g"
			
			importer->tmp_port_spec += " ";
			importer->tmp_port_spec += importer->tmp_port_spec_2;
			
#line 5285 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "tcp_udp_port_names");
	
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
			recover(ex,_tokenSet_32);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::interface_command_6() {
	Tracer traceInOut(this, "interface_command_6");
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		in = LT(1);
		match(WORD);
		pix6_interface_hw_speed();
		if ( inputState->guessing==0 ) {
#line 1353 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newInterface( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE: " << in->getText() << std::endl;
			
#line 5367 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "interface_command_7");
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1361 "pix.g"
	bool have_interface_parameters = false;
#line 5385 "PIXCfgParser.cpp"
	
	try {      // for error handling
		in = LT(1);
		match(WORD);
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 1362 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newInterface( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE: " << in->getText() << std::endl;
			
#line 5399 "PIXCfgParser.cpp"
		}
		{
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_33.member(LA(1)))) {
				interface_parameters();
				if ( inputState->guessing==0 ) {
#line 1369 "pix.g"
					have_interface_parameters = true;
#line 5409 "PIXCfgParser.cpp"
				}
			}
			else {
				goto _loop189;
			}
			
		}
		_loop189:;
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
#line 1371 "pix.g"
			
			if ( ! have_interface_parameters )
			{
			importer->ignoreCurrentInterface();
			*dbg<< LT(1)->getLine() << ":"
			<< " EMPTY INTERFACE " << std::endl;
			}
			
#line 5447 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "pix6_interface_hw_speed");
	
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
	Tracer traceInOut(this, "interface_parameters");
	
	try {      // for error handling
		if ( inputState->guessing==0 ) {
#line 1406 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 5543 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_34);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::intf_address() {
	Tracer traceInOut(this, "intf_address");
	
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
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::vlan_interface() {
	Tracer traceInOut(this, "vlan_interface");
	ANTLR_USE_NAMESPACE(antlr)RefToken  vlan_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(VLAN);
		vlan_id = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1432 "pix.g"
			
			importer->setInterfaceVlanId(vlan_id->getText());
			*dbg << " VLAN: " << vlan_id->getText() << std::endl;
			
#line 5673 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "sec_level");
	ANTLR_USE_NAMESPACE(antlr)RefToken  sec_level = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SEC_LEVEL);
		sec_level = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1484 "pix.g"
			
			importer->setInterfaceSecurityLevel(sec_level->getText());
			*dbg << "SEC_LEVEL: " << sec_level->getText() << std::endl;
			
#line 5700 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "nameif");
	
	try {      // for error handling
		match(NAMEIF);
		interface_label();
		if ( inputState->guessing==0 ) {
#line 1503 "pix.g"
			
			importer->setInterfaceParametes(LT(0)->getText(), "", "");
			*dbg << " NAMEIF: " << LT(0)->getText() << std::endl;
			
#line 5725 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "interface_description");
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 1512 "pix.g"
			
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
			
#line 5757 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "switchport");
	ANTLR_USE_NAMESPACE(antlr)RefToken  vlan_num = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SWITCHPORT);
		match(ACCESS);
		match(VLAN);
		vlan_num = LT(1);
		match(INT_CONST);
		if ( inputState->guessing==0 ) {
#line 1624 "pix.g"
			
			importer->addMessageToLog("Switch port vlan " + vlan_num->getText());
			*dbg << "Switch port vlan " <<  vlan_num->getText() << std::endl;
			
#line 5786 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "shutdown");
	
	try {      // for error handling
		match(SHUTDOWN);
		if ( inputState->guessing==0 ) {
#line 1527 "pix.g"
			
			importer->ignoreCurrentInterface();
			*dbg<< LT(1)->getLine() << ":"
			<< " INTERFACE SHUTDOWN " << std::endl;
			
#line 5811 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "interface_no_commands");
	
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
#line 1476 "pix.g"
			
			*dbg << " INTERFACE \"NO\" COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 5864 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "unsupported_interface_commands");
	
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
#line 1468 "pix.g"
			
			*dbg << " UNSUPPORTED INTERFACE COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 5966 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "v6_ip_address");
	
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
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::v7_ip_address() {
	Tracer traceInOut(this, "v7_ip_address");
	
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
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::v6_dhcp_address() {
	Tracer traceInOut(this, "v6_dhcp_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  lbl = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dhcp = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		lbl = LT(1);
		match(WORD);
		dhcp = LT(1);
		match(DHCP);
		if ( inputState->guessing==0 ) {
#line 1562 "pix.g"
			
			std::string label = lbl->getText();
			std::string addr = dhcp->getText();
			importer->addInterfaceAddress(label, addr, "");
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << std::endl;
			// there can be some other parameters after "dhcp", such as "setroute", "retry" etc.
			// which we do not support
			consumeUntil(NEWLINE);
			
#line 6057 "PIXCfgParser.cpp"
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

void PIXCfgParser::v6_static_address() {
	Tracer traceInOut(this, "v6_static_address");
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
#line 1575 "pix.g"
			
			std::string label = lbl->getText();
			std::string addr = a->getText();
			std::string netm = m->getText();
			importer->addInterfaceAddress(label, addr, netm);
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << "/" << netm << std::endl;
			// in case there are some other parameters after address and netmask
			consumeUntil(NEWLINE);
			
#line 6095 "PIXCfgParser.cpp"
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

void PIXCfgParser::v7_dhcp_address() {
	Tracer traceInOut(this, "v7_dhcp_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  dhcp = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		dhcp = LT(1);
		match(DHCP);
		if ( inputState->guessing==0 ) {
#line 1592 "pix.g"
			
			std::string addr = dhcp->getText();
			importer->addInterfaceAddress(addr, "");
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << std::endl;
			consumeUntil(NEWLINE);
			
#line 6124 "PIXCfgParser.cpp"
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

void PIXCfgParser::v7_static_address() {
	Tracer traceInOut(this, "v7_static_address");
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
#line 1603 "pix.g"
			
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
			
#line 6184 "PIXCfgParser.cpp"
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

void PIXCfgParser::icmp_types_for_icmp_command() {
	Tracer traceInOut(this, "icmp_types_for_icmp_command");
	
	try {      // for error handling
		switch ( LA(1)) {
		case INT_CONST:
		{
			match(INT_CONST);
			if ( inputState->guessing==0 ) {
#line 1743 "pix.g"
				
				importer->icmp_type = LT(0)->getText();
				importer->icmp_code = "0";
				importer->icmp_spec = "";
				
#line 6212 "PIXCfgParser.cpp"
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
#line 1750 "pix.g"
				
				importer->icmp_type = "";
				importer->icmp_code = "0";
				importer->icmp_spec = LT(0)->getText();
				
#line 6256 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_35);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::nat_addr_match() {
	Tracer traceInOut(this, "nat_addr_match");
	ANTLR_USE_NAMESPACE(antlr)RefToken  acl_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case HOST:
		{
			host_addr();
			if ( inputState->guessing==0 ) {
#line 1842 "pix.g"
				
				importer->nat_a = importer->tmp_a;
				importer->nat_nm = importer->tmp_nm;
				
#line 6292 "PIXCfgParser.cpp"
			}
			break;
		}
		case ACCESS_LIST:
		{
			match(ACCESS_LIST);
			acl_name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 1848 "pix.g"
				
				importer->nat_acl = acl_name->getText();
				
#line 6306 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_36);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::nat_command_last_parameters() {
	Tracer traceInOut(this, "nat_command_last_parameters");
	ANTLR_USE_NAMESPACE(antlr)RefToken  max_conn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  max_emb_conn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case DNS:
		{
			match(DNS);
			break;
		}
		case INT_CONST:
		case TCP:
		case UDP:
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
		{
		switch ( LA(1)) {
		case OUTSIDE:
		{
			match(OUTSIDE);
			break;
		}
		case INT_CONST:
		case TCP:
		case UDP:
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
		switch ( LA(1)) {
		case INT_CONST:
		{
			max_emb_conn = LT(1);
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
		if ( inputState->guessing==0 ) {
#line 1865 "pix.g"
			
			importer->static_max_conn = max_conn->getText();
			if (max_emb_conn)
			importer->static_max_emb_conn = max_emb_conn->getText();
			
#line 6421 "PIXCfgParser.cpp"
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

void PIXCfgParser::nat_new_top_level_command() {
	Tracer traceInOut(this, "nat_new_top_level_command");
	
	try {      // for error handling
		match(NAT);
		if ( inputState->guessing==0 ) {
#line 1873 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 6444 "PIXCfgParser.cpp"
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

void PIXCfgParser::static_starts_with_hostaddr() {
	Tracer traceInOut(this, "static_starts_with_hostaddr");
	
	try {      // for error handling
		static_mapped_addr_match();
		static_real_addr_match();
		static_command_common_last_parameters();
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
	Tracer traceInOut(this, "static_starts_with_tcp_udp");
	
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
#line 1967 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << " SERVICE TCP/UDP" << LT(0)->getText() << " ";
			
#line 6503 "PIXCfgParser.cpp"
		}
		static_mapped_addr_match();
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1981 "pix.g"
			
			importer->mapped_port_spec = importer->tmp_port_spec_2;
			*dbg << "mapped port " << importer->mapped_port_spec;
			
#line 6513 "PIXCfgParser.cpp"
		}
		static_real_addr_match();
		tcp_udp_port_spec();
		if ( inputState->guessing==0 ) {
#line 1996 "pix.g"
			
			importer->real_port_spec = importer->tmp_port_spec_2;
			*dbg << "real port " << importer->real_port_spec;
			
#line 6523 "PIXCfgParser.cpp"
		}
		static_command_common_last_parameters();
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
	Tracer traceInOut(this, "static_mapped_addr_match");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case HOST:
		{
			host_addr();
			if ( inputState->guessing==0 ) {
#line 1938 "pix.g"
				
				importer->mapped_a = importer->tmp_a;
				importer->mapped_nm = importer->tmp_nm;
				
#line 6552 "PIXCfgParser.cpp"
			}
			break;
		}
		case INTRFACE:
		{
			match(INTRFACE);
			if ( inputState->guessing==0 ) {
#line 1944 "pix.g"
				
				importer->mapped_a = "interface";
				importer->mapped_nm = "";
				
#line 6565 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_37);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::static_real_addr_match() {
	Tracer traceInOut(this, "static_real_addr_match");
	ANTLR_USE_NAMESPACE(antlr)RefToken  acl_name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case HOST:
		{
			host_addr();
			if ( inputState->guessing==0 ) {
#line 1954 "pix.g"
				
				importer->real_a = importer->tmp_a;
				importer->real_nm = importer->tmp_nm;
				
#line 6602 "PIXCfgParser.cpp"
			}
			break;
		}
		case ACCESS_LIST:
		{
			match(ACCESS_LIST);
			acl_name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 1960 "pix.g"
				
				importer->real_addr_acl = acl_name->getText();
				
#line 6616 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_38);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::static_command_common_last_parameters() {
	Tracer traceInOut(this, "static_command_common_last_parameters");
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  max_conn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  max_emb_conn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case NETMASK:
		{
			match(NETMASK);
			nm = LT(1);
			match(IPv4);
			if ( inputState->guessing==0 ) {
#line 2012 "pix.g"
				
				importer->mapped_nm = nm->getText();
				
#line 6655 "PIXCfgParser.cpp"
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
#line 2017 "pix.g"
				
				// <0-65535>  The maximum number of simultaneous tcp connections
				
				
#line 6686 "PIXCfgParser.cpp"
			}
			break;
		}
		case INT_CONST:
		{
			max_conn = LT(1);
			match(INT_CONST);
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				max_emb_conn = LT(1);
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
			if ( inputState->guessing==0 ) {
#line 2023 "pix.g"
				
				importer->static_max_conn = max_conn->getText();
				if (max_emb_conn)
				importer->static_max_emb_conn = max_emb_conn->getText();
				
#line 6719 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_9);
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
	"OPENING_PAREN",
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
	"CLOSING_PAREN",
	"DNS",
	"\"global\"",
	"\"static\"",
	"COMMA",
	"NETMASK",
	"IPv4",
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
const unsigned long PIXCfgParser::_tokenSet_1_data_[] = { 1342187378UL, 2814382112UL, 2147486720UL, 67960832UL, 835584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "nat" "icmp" 
// OBJECT_GROUP "crypto" "no" "certificate" "PIX" "ASA" "hostname" "access-list" 
// "ssh" "telnet" "interface" "controller" LINE_COMMENT "exit" "nameif" 
// "access-group" COLON_COMMENT "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_1(_tokenSet_1_data_,12);
const unsigned long PIXCfgParser::_tokenSet_2_data_[] = { 0UL, 64UL, 0UL, 7340032UL, 0UL, 0UL, 0UL, 0UL };
// INT_CONST "aui" "auto" "bnc" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long PIXCfgParser::_tokenSet_3_data_[] = { 536854592UL, 5024UL, 2UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" "ospf" 
// "pcp" "pim" "pptp" "snp" OBJECT "icmp" "icmp6" "tcp" "udp" OBJECT_GROUP 
// "permit" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long PIXCfgParser::_tokenSet_4_data_[] = { 536854592UL, 5024UL, 4UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" "ospf" 
// "pcp" "pim" "pptp" "snp" OBJECT "icmp" "icmp6" "tcp" "udp" OBJECT_GROUP 
// "deny" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long PIXCfgParser::_tokenSet_5_data_[] = { 0UL, 3153921UL, 0UL, 0UL, 0UL, 0UL };
// "description" "group-object" "service-object" "port-object" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_5(_tokenSet_5_data_,6);
const unsigned long PIXCfgParser::_tokenSet_6_data_[] = { 268437520UL, 4098UL, 2147483648UL, 1UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 OBJECT "host" OBJECT_GROUP "interface" "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_6(_tokenSet_6_data_,8);
const unsigned long PIXCfgParser::_tokenSet_7_data_[] = { 1342187378UL, 2814382127UL, 2147486720UL, 67960832UL, 835584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "nat" "description" 
// "host" "range" "subnet" "icmp" OBJECT_GROUP "crypto" "no" "certificate" 
// "PIX" "ASA" "hostname" "access-list" "ssh" "telnet" "interface" "controller" 
// LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_7(_tokenSet_7_data_,12);
const unsigned long PIXCfgParser::_tokenSet_8_data_[] = { 67117072UL, 2684355394UL, 3840UL, 131072UL, 2228224UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE WORD "pptp" "host" INT_CONST "tcp" "udp" "hostname" "access-list" 
// "echo" "rip" "ssh" "telnet" "outside" DNS NETMASK 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_8(_tokenSet_8_data_,12);
const unsigned long PIXCfgParser::_tokenSet_9_data_[] = { 16UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_9(_tokenSet_9_data_,6);
const unsigned long PIXCfgParser::_tokenSet_10_data_[] = { 1342189426UL, 2814382191UL, 4294966768UL, 68141063UL, 901120UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" IPV4 WORD OBJECT "nat" 
// "description" "host" "range" "subnet" "icmp" INT_CONST OBJECT_GROUP 
// "crypto" "no" "certificate" "PIX" "ASA" "hostname" "access-list" "eq" 
// "gt" "lt" "neq" "echo" "ssh" "telnet" "established" "alternate-address" 
// "conversion-error" "echo-reply" "information-reply" "information-request" 
// "mask-reply" "mask-request" "mobile-redirect" "parameter-problem" "redirect" 
// "router-advertisement" "router-solicitation" "source-quench" "time-exceeded" 
// "timestamp-reply" "timestamp-request" "traceroute" "unreachable" "interface" 
// "any" "log" "log-input" "fragments" "time-range" "controller" "outside" 
// LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT CLOSING_PAREN 
// "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_10(_tokenSet_10_data_,12);
const unsigned long PIXCfgParser::_tokenSet_11_data_[] = { 268419136UL, 64UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" "ospf" 
// "pcp" "pim" "pptp" "snp" INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_11(_tokenSet_11_data_,6);
const unsigned long PIXCfgParser::_tokenSet_12_data_[] = { 1342187378UL, 2814382129UL, 2147486720UL, 67960832UL, 835584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "nat" "description" 
// "service" "icmp" OBJECT_GROUP "crypto" "no" "certificate" "PIX" "ASA" 
// "hostname" "access-list" "ssh" "telnet" "interface" "controller" LINE_COMMENT 
// "exit" "nameif" "access-group" COLON_COMMENT "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_12(_tokenSet_12_data_,12);
const unsigned long PIXCfgParser::_tokenSet_13_data_[] = { 16UL, 0UL, 0UL, 49158UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_13(_tokenSet_13_data_,8);
const unsigned long PIXCfgParser::_tokenSet_14_data_[] = { 16UL, 2052UL, 240UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "range" "destination" "eq" "gt" "lt" "neq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_14(_tokenSet_14_data_,8);
const unsigned long PIXCfgParser::_tokenSet_15_data_[] = { 268437520UL, 6150UL, 2147487984UL, 49159UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 OBJECT "host" "range" "destination" OBJECT_GROUP "eq" "gt" 
// "lt" "neq" "established" "interface" "any" "log" "log-input" "fragments" 
// "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_15(_tokenSet_15_data_,8);
const unsigned long PIXCfgParser::_tokenSet_16_data_[] = { 1342187378UL, 2814406689UL, 2147486720UL, 67960832UL, 835584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "nat" "description" 
// "icmp" OBJECT_GROUP "group-object" "network-object" "crypto" "no" "certificate" 
// "PIX" "ASA" "hostname" "access-list" "ssh" "telnet" "interface" "controller" 
// LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_16(_tokenSet_16_data_,12);
const unsigned long PIXCfgParser::_tokenSet_17_data_[] = { 1342187378UL, 2814455841UL, 2147486720UL, 67960832UL, 835584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "nat" "description" 
// "icmp" OBJECT_GROUP "group-object" "protocol-object" "crypto" "no" "certificate" 
// "PIX" "ASA" "hostname" "access-list" "ssh" "telnet" "interface" "controller" 
// LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_17(_tokenSet_17_data_,12);
const unsigned long PIXCfgParser::_tokenSet_18_data_[] = { 1342187378UL, 2814521377UL, 2147486720UL, 67960832UL, 835584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "nat" "description" 
// "icmp" OBJECT_GROUP "group-object" "icmp-object" "crypto" "no" "certificate" 
// "PIX" "ASA" "hostname" "access-list" "ssh" "telnet" "interface" "controller" 
// LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_18(_tokenSet_18_data_,12);
const unsigned long PIXCfgParser::_tokenSet_19_data_[] = { 1342187378UL, 2817536033UL, 2147486720UL, 67960832UL, 835584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD OBJECT "nat" "description" 
// "icmp" OBJECT_GROUP "group-object" "service-object" "port-object" "crypto" 
// "no" "certificate" "PIX" "ASA" "hostname" "access-list" "ssh" "telnet" 
// "interface" "controller" LINE_COMMENT "exit" "nameif" "access-group" 
// COLON_COMMENT "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_19(_tokenSet_19_data_,12);
const unsigned long PIXCfgParser::_tokenSet_20_data_[] = { 268445712UL, 4166UL, 4294963696UL, 180231UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD OBJECT "host" "range" INT_CONST OBJECT_GROUP "eq" 
// "gt" "lt" "neq" "echo" "established" "alternate-address" "conversion-error" 
// "echo-reply" "information-reply" "information-request" "mask-reply" 
// "mask-request" "mobile-redirect" "parameter-problem" "redirect" "router-advertisement" 
// "router-solicitation" "source-quench" "time-exceeded" "timestamp-reply" 
// "timestamp-request" "traceroute" "unreachable" "interface" "any" "log" 
// "log-input" "fragments" "time-range" "outside" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_20(_tokenSet_20_data_,8);
const unsigned long PIXCfgParser::_tokenSet_21_data_[] = { 268437504UL, 4098UL, 2147483648UL, 1UL, 0UL, 0UL, 0UL, 0UL };
// IPV4 OBJECT "host" OBJECT_GROUP "interface" "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_21(_tokenSet_21_data_,8);
const unsigned long PIXCfgParser::_tokenSet_22_data_[] = { 16UL, 0UL, 0UL, 16390UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_22(_tokenSet_22_data_,8);
const unsigned long PIXCfgParser::_tokenSet_23_data_[] = { 16UL, 0UL, 0UL, 6UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_23(_tokenSet_23_data_,8);
const unsigned long PIXCfgParser::_tokenSet_24_data_[] = { 268445712UL, 4100UL, 4336UL, 180230UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD OBJECT "range" OBJECT_GROUP "eq" "gt" "lt" "neq" "established" 
// "log" "log-input" "fragments" "time-range" "outside" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_24(_tokenSet_24_data_,8);
const unsigned long PIXCfgParser::_tokenSet_25_data_[] = { 0UL, 4UL, 240UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "range" "eq" "gt" "lt" "neq" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_25(_tokenSet_25_data_,8);
const unsigned long PIXCfgParser::_tokenSet_26_data_[] = { 67117056UL, 536870976UL, 3840UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD "pptp" INT_CONST "hostname" "echo" "rip" "ssh" "telnet" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_26(_tokenSet_26_data_,8);
const unsigned long PIXCfgParser::_tokenSet_27_data_[] = { 268435472UL, 4100UL, 4336UL, 49158UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE OBJECT "range" OBJECT_GROUP "eq" "gt" "lt" "neq" "established" 
// "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_27(_tokenSet_27_data_,8);
const unsigned long PIXCfgParser::_tokenSet_28_data_[] = { 1409296242UL, 2814382180UL, 2147491824UL, 68026366UL, 835584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD "pptp" OBJECT 
// "nat" "range" "icmp" INT_CONST OBJECT_GROUP "crypto" "no" "certificate" 
// "PIX" "ASA" "hostname" "access-list" "eq" "gt" "lt" "neq" "echo" "rip" 
// "ssh" "telnet" "established" "interface" "log" "log-input" "alerts" 
// "critical" "debugging" "emergencies" "errors" "informational" "notifications" 
// "warnings" "disable" "inactive" "interval" "fragments" "time-range" 
// "controller" LINE_COMMENT "exit" "nameif" "access-group" COLON_COMMENT 
// "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_28(_tokenSet_28_data_,12);
const unsigned long PIXCfgParser::_tokenSet_29_data_[] = { 1409296242UL, 2814382176UL, 2147487488UL, 68026366UL, 835584UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "timeout" "names" "name" WORD "pptp" OBJECT 
// "nat" "icmp" INT_CONST OBJECT_GROUP "crypto" "no" "certificate" "PIX" 
// "ASA" "hostname" "access-list" "echo" "rip" "ssh" "telnet" "interface" 
// "log" "log-input" "alerts" "critical" "debugging" "emergencies" "errors" 
// "informational" "notifications" "warnings" "disable" "inactive" "interval" 
// "fragments" "time-range" "controller" LINE_COMMENT "exit" "nameif" "access-group" 
// COLON_COMMENT "global" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_29(_tokenSet_29_data_,12);
const unsigned long PIXCfgParser::_tokenSet_30_data_[] = { 268437504UL, 4102UL, 2147483888UL, 1UL, 0UL, 0UL, 0UL, 0UL };
// IPV4 OBJECT "host" "range" OBJECT_GROUP "eq" "gt" "lt" "neq" "interface" 
// "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_30(_tokenSet_30_data_,8);
const unsigned long PIXCfgParser::_tokenSet_31_data_[] = { 335554576UL, 536875076UL, 8176UL, 180230UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD "pptp" OBJECT "range" INT_CONST OBJECT_GROUP "hostname" 
// "eq" "gt" "lt" "neq" "echo" "rip" "ssh" "telnet" "established" "log" 
// "log-input" "fragments" "time-range" "outside" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_31(_tokenSet_31_data_,8);
const unsigned long PIXCfgParser::_tokenSet_32_data_[] = { 335554576UL, 2684361542UL, 2147491824UL, 49159UL, 2097152UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD "pptp" OBJECT "host" "range" INT_CONST "tcp" "udp" 
// "destination" OBJECT_GROUP "hostname" "access-list" "eq" "gt" "lt" "neq" 
// "echo" "rip" "ssh" "telnet" "established" "interface" "any" "log" "log-input" 
// "fragments" "time-range" NETMASK 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_32(_tokenSet_32_data_,12);
const unsigned long PIXCfgParser::_tokenSet_33_data_[] = { 42205248UL, 8388609UL, 512UL, 4227858432UL, 2303UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "igmp" "ospf" "pim" "description" "no" "rip" "nameif" "vlan" "speed" 
// "duplex" "ddns" "forward" "delay" "hold-time" "ipv6" "mac-address" "multicast" 
// PPPOE "security-level" "shutdown" "switchport" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_33(_tokenSet_33_data_,12);
const unsigned long PIXCfgParser::_tokenSet_34_data_[] = { 42205248UL, 8388609UL, 512UL, 4228644864UL, 2303UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "igmp" "ospf" "pim" "description" "no" "rip" LINE_COMMENT "exit" 
// "nameif" "vlan" "speed" "duplex" "ddns" "forward" "delay" "hold-time" 
// "ipv6" "mac-address" "multicast" PPPOE "security-level" "shutdown" "switchport" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_34(_tokenSet_34_data_,12);
const unsigned long PIXCfgParser::_tokenSet_35_data_[] = { 8192UL, 0UL, 0UL, 131072UL, 0UL, 0UL, 0UL, 0UL };
// WORD "outside" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_35(_tokenSet_35_data_,8);
const unsigned long PIXCfgParser::_tokenSet_36_data_[] = { 0UL, 832UL, 0UL, 131072UL, 131072UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// INT_CONST "tcp" "udp" "outside" DNS 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_36(_tokenSet_36_data_,12);
const unsigned long PIXCfgParser::_tokenSet_37_data_[] = { 67117056UL, 2684354626UL, 3840UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD "pptp" "host" INT_CONST "hostname" "access-list" "echo" "rip" "ssh" 
// "telnet" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_37(_tokenSet_37_data_,8);
const unsigned long PIXCfgParser::_tokenSet_38_data_[] = { 67117056UL, 536871744UL, 3840UL, 0UL, 2097152UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// WORD "pptp" INT_CONST "tcp" "udp" "hostname" "echo" "rip" "ssh" "telnet" 
// NETMASK 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_38(_tokenSet_38_data_,12);



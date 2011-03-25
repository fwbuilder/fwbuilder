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
#line 796 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setDiscoveredVersion(LT(0)->getText());
			*dbg << "VERSION " << LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 255 "PIXCfgParser.cpp"
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
#line 806 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setHostName( LT(0)->getText() );
			*dbg << "HOSTNAME "
			<< "LT0=" << LT(0)->getText()
			<< std::endl;
			
#line 300 "PIXCfgParser.cpp"
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
#line 174 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 324 "PIXCfgParser.cpp"
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
#line 765 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 348 "PIXCfgParser.cpp"
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
		interface_command();
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
#line 1187 "pix.g"
			
			std::string label = (intf_label) ? intf_label->getText() : "";
			std::string seclevel = (sec_level) ? sec_level->getText() : "";
			importer->setInterfaceParametes(p_intf->getText(), label, seclevel);
			*dbg << " NAMEIF: "
			<< p_intf->getText() << label << seclevel << std::endl;
			
#line 401 "PIXCfgParser.cpp"
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
#line 1109 "pix.g"
			
			importer->clearCurrentInterface();
			consumeUntil(NEWLINE);
			
#line 425 "PIXCfgParser.cpp"
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
#line 818 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newUnidirRuleSet(
			name->getText(), libfwbuilder::Policy::TYPENAME );
			*dbg << name->getLine() << ":"
			<< " ACL ext " << name->getText() << std::endl;
			
#line 456 "PIXCfgParser.cpp"
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
			if ((LA(1) == EXTENDED || LA(1) == PERMIT) && (_tokenSet_2.member(LA(2)))) {
				permit_extended();
			}
			else if ((LA(1) == EXTENDED || LA(1) == DENY) && (_tokenSet_3.member(LA(2)))) {
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
#line 841 "pix.g"
			
			*dbg << LT(0)->getLine() << ":"
			<< " ACL line end" << std::endl << std::endl;
			
#line 500 "PIXCfgParser.cpp"
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
#line 1453 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->setInterfaceAndDirectionForRuleSet(
			aclname->getText(),
			intf_label->getText(),
			dir->getText() );
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE: ACL '" << aclname->getText() << "'"
			<< " " << intf_label->getText()
			<< " " << dir->getText() << std::endl;
			
#line 541 "PIXCfgParser.cpp"
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
#line 788 "pix.g"
			
			consumeUntil(NEWLINE);
			consumeUntil(QUIT);
			
#line 582 "PIXCfgParser.cpp"
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
#line 167 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 605 "PIXCfgParser.cpp"
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
#line 181 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addMessageToLog(
			"Parser warning: \"names\" section detected. "
			"Import of configuration that uses \"names\" "
			"is not supported at this time");
			
#line 632 "PIXCfgParser.cpp"
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
#line 191 "pix.g"
			
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
			
#line 692 "PIXCfgParser.cpp"
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
		if ( inputState->guessing==0 ) {
#line 224 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newNamedObjectAddress(name->getText());
			*dbg << name->getLine() << ":"
			<< " Named Object " << name->getText() << std::endl;
			
#line 723 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == NEWLINE) && ((LA(2) >= NAT && LA(2) <= SUBNET))) {
				match(NEWLINE);
				named_object_network_parameters();
			}
			else {
				goto _loop13;
			}
			
		}
		_loop13:;
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
		if ( inputState->guessing==0 ) {
#line 334 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newNamedObjectService(name->getText());
			*dbg << name->getLine() << ":"
			<< " Named Object " << name->getText() << std::endl;
			
#line 767 "PIXCfgParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == SERVICE)) {
				match(NEWLINE);
				named_object_service_parameters();
			}
			else {
				goto _loop29;
			}
			
		}
		_loop29:;
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
		if ( inputState->guessing==0 ) {
#line 449 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupNetwork(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 811 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt48=0;
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == GROUP_OBJECT || LA(2) == NETWORK_OBJECT)) {
				object_group_network_parameters();
			}
			else {
				if ( _cnt48>=1 ) { goto _loop48; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt48++;
		}
		_loop48:;
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
		if ( inputState->guessing==0 ) {
#line 670 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupService(name->getText());
			if (tcp) importer->setObjectGroupServiceProtocol("tcp");
			if (udp) importer->setObjectGroupServiceProtocol("udp");
			if (tcpudp) importer->setObjectGroupServiceProtocol("tcp-udp");
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 892 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt80=0;
		for (;;) {
			if ((LA(1) == NEWLINE) && (_tokenSet_4.member(LA(2)))) {
				object_group_service_parameters();
			}
			else {
				if ( _cnt80>=1 ) { goto _loop80; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt80++;
		}
		_loop80:;
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
	Tracer traceInOut(this, "object_group_protocol");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(PROTOCOL);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 547 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupProtocol(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 937 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt60=0;
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == GROUP_OBJECT || LA(2) == PROTOCOL_OBJECT)) {
				object_group_protocol_parameters();
			}
			else {
				if ( _cnt60>=1 ) { goto _loop60; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt60++;
		}
		_loop60:;
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
		if ( inputState->guessing==0 ) {
#line 602 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupICMP(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 982 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt68=0;
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == GROUP_OBJECT || LA(2) == ICMP_OBJECT)) {
				object_group_icmp_parameters();
			}
			else {
				if ( _cnt68>=1 ) { goto _loop68; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt68++;
		}
		_loop68:;
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

void PIXCfgParser::object_group_icmp_8_3() {
	Tracer traceInOut(this, "object_group_icmp_8_3");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(OBJECT_GROUP);
		match(ICMP_TYPE);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 615 "pix.g"
			
			importer->clear();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newObjectGroupICMP(name->getText());
			*dbg << name->getLine() << ":"
			<< " Object Group " << name->getText() << std::endl;
			
#line 1027 "PIXCfgParser.cpp"
		}
		{ // ( ... )+
		int _cnt71=0;
		for (;;) {
			if ((LA(1) == NEWLINE) && (LA(2) == DESCRIPTION || LA(2) == GROUP_OBJECT || LA(2) == ICMP_OBJECT)) {
				object_group_icmp_parameters();
			}
			else {
				if ( _cnt71>=1 ) { goto _loop71; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt71++;
		}
		_loop71:;
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

void PIXCfgParser::crypto() {
	Tracer traceInOut(this, "crypto");
	
	try {      // for error handling
		match(CRYPTO);
		if ( inputState->guessing==0 ) {
#line 758 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1064 "PIXCfgParser.cpp"
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
#line 779 "pix.g"
			
			*dbg << " TOP LEVEL \"NO\" COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 1089 "PIXCfgParser.cpp"
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
#line 772 "pix.g"
			
			consumeUntil(NEWLINE);
			
#line 1112 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_5);
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
	Tracer traceInOut(this, "named_object_nat");
	
	try {      // for error handling
		match(NAT);
		if ( inputState->guessing==0 ) {
#line 252 "pix.g"
			
			importer->addMessageToLog(
			"Parser warning: "
			"Import of named objects with \"nat\" command "
			"is not supported at this time");
			consumeUntil(NEWLINE);
			
#line 1285 "PIXCfgParser.cpp"
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
#line 283 "pix.g"
			
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
#line 302 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->tmp_range_1 = r1->getText();
			importer->tmp_range_2 = r2->getText();
			importer->commitNamedAddressRangeObject();
			*dbg << r1->getText() << "/" << r2->getText();
			
#line 1380 "PIXCfgParser.cpp"
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
#line 312 "pix.g"
			
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
			
#line 1445 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "named_object_description");
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 262 "pix.g"
			
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
			
#line 1477 "PIXCfgParser.cpp"
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
		else if ((LA(1) == SERVICE) && (_tokenSet_6.member(LA(2)))) {
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
	Tracer traceInOut(this, "service_icmp");
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_word = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
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
#line 366 "pix.g"
				
				importer->icmp_type = LT(0)->getText();
				
#line 1548 "PIXCfgParser.cpp"
			}
			break;
		}
		case WORD:
		{
			icmp_word = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 370 "pix.g"
				
				importer->icmp_spec = icmp_word->getText();
				
#line 1561 "PIXCfgParser.cpp"
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
#line 374 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->commitNamedICMPServiceObject();
			*dbg << "NAMED OBJECT SERVICE ICMP " << LT(0)->getText() << " ";
			
#line 1578 "PIXCfgParser.cpp"
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
#line 382 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addMessageToLog("Parser warning: "
			"Import of IPv6 addresses and servcies "
			"is not supported at this time");
			*dbg << "NAMED OBJECT SERVICE ICMP6 " << LT(0)->getText() << " ";
			consumeUntil(NEWLINE);
			
#line 1625 "PIXCfgParser.cpp"
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
#line 393 "pix.g"
			
			importer->protocol = LT(0)->getText();
			*dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
			
#line 1667 "PIXCfgParser.cpp"
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
		case NAMES:
		case NAME:
		case WORD:
		case OBJECT:
		case RANGE:
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
		case INTRFACE:
		case CONTROLLER:
		case NAMEIF:
		case ACCESS_GROUP:
		case EXIT:
		case LINE_COMMENT:
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
		case NAMES:
		case NAME:
		case WORD:
		case OBJECT:
		case OBJECT_GROUP:
		case CRYPTO:
		case NO:
		case CERTIFICATE:
		case PIX_WORD:
		case ASA_WORD:
		case HOSTNAME:
		case ACCESS_LIST:
		case INTRFACE:
		case CONTROLLER:
		case NAMEIF:
		case ACCESS_GROUP:
		case EXIT:
		case LINE_COMMENT:
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
#line 399 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->commitNamedTCPUDPServiceObject();
			
#line 1764 "PIXCfgParser.cpp"
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
#line 418 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->protocol = LT(0)->getText();
			importer->commitNamedIPServiceObject();
			*dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
			
#line 1822 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "service_unknown");
	
	try {      // for error handling
		match(SERVICE);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 433 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->protocol = "ip";
			importer->commitNamedIPServiceObject();
			std::string err = "Parser warning: Unknown service name " +
			LT(0)->getText();
			importer->setNamedObjectDescription(err);
			importer->addMessageToLog(err);
			*dbg << "UNKNOWN SERVICE " << LT(0)->getText() << " ";
			
#line 1853 "PIXCfgParser.cpp"
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

void PIXCfgParser::src_port_spec() {
	Tracer traceInOut(this, "src_port_spec");
	
	try {      // for error handling
		match(SOURCE);
		xoperator();
		if ( inputState->guessing==0 ) {
#line 406 "pix.g"
			
			importer->SaveTmpPortToSrc();
			
#line 1877 "PIXCfgParser.cpp"
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
#line 412 "pix.g"
			
			importer->SaveTmpPortToDst();
			
#line 1921 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_8);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::object_group_network_parameters() {
	Tracer traceInOut(this, "object_group_network_parameters");
	
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
	Tracer traceInOut(this, "object_group_description");
	
	try {      // for error handling
		match(DESCRIPTION);
		if ( inputState->guessing==0 ) {
#line 473 "pix.g"
			
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
			
#line 2026 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "group_object");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(GROUP_OBJECT);
		name = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 488 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->addNamedObjectToObjectGroup(name->getText());
			*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
			
#line 2055 "PIXCfgParser.cpp"
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
#line 497 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 2085 "PIXCfgParser.cpp"
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
#line 503 "pix.g"
				
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
				
#line 2133 "PIXCfgParser.cpp"
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
#line 520 "pix.g"
				
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
				
#line 2177 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 537 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 2192 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "object_group_protocol_parameters");
	
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
	Tracer traceInOut(this, "protocol_object");
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(PROTOCOL_OBJECT);
		if ( inputState->guessing==0 ) {
#line 571 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 2264 "PIXCfgParser.cpp"
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
#line 577 "pix.g"
				
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->protocol = LT(0)->getText();
				importer->addIPServiceToObjectGroup();
				*dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
				
#line 2343 "PIXCfgParser.cpp"
			}
			break;
		}
		case ICMP6:
		{
			match(ICMP6);
			if ( inputState->guessing==0 ) {
#line 585 "pix.g"
				
				importer->addMessageToLog(
				"Parser warning: IPv6 import is not supported. ");
				consumeUntil(NEWLINE);
				
#line 2357 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 592 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 2372 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "object_group_icmp_parameters");
	
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
	Tracer traceInOut(this, "icmp_object");
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_word = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(ICMP_OBJECT);
		if ( inputState->guessing==0 ) {
#line 639 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 2446 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case WORD:
		case INT_CONST:
		{
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				icmp_type = LT(1);
				match(INT_CONST);
				if ( inputState->guessing==0 ) {
#line 646 "pix.g"
					
					importer->icmp_type = LT(0)->getText();
					
#line 2464 "PIXCfgParser.cpp"
				}
				break;
			}
			case WORD:
			{
				icmp_word = LT(1);
				match(WORD);
				if ( inputState->guessing==0 ) {
#line 650 "pix.g"
					
					importer->icmp_spec = icmp_word->getText();
					
#line 2477 "PIXCfgParser.cpp"
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
#line 654 "pix.g"
				
				importer->addICMPServiceToObjectGroup();
				*dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
				
#line 2493 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 660 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 2508 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "object_group_service_parameters");
	
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
	Tracer traceInOut(this, "service_object");
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  icmp_word = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(SERVICE_OBJECT);
		if ( inputState->guessing==0 ) {
#line 699 "pix.g"
			
			importer->clearTempVars();
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 2587 "PIXCfgParser.cpp"
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
#line 705 "pix.g"
				
				importer->setCurrentLineNumber(LT(0)->getLine());
				importer->protocol = LT(0)->getText();
				importer->addIPServiceToObjectGroup();
				*dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
				
#line 2648 "PIXCfgParser.cpp"
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
#line 713 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << " SERVICE TCP/UDP" << LT(0)->getText() << " ";
				
#line 2685 "PIXCfgParser.cpp"
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
			case NAMES:
			case NAME:
			case WORD:
			case OBJECT:
			case RANGE:
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
			case INTRFACE:
			case CONTROLLER:
			case NAMEIF:
			case ACCESS_GROUP:
			case EXIT:
			case LINE_COMMENT:
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
			case NAMES:
			case NAME:
			case WORD:
			case OBJECT:
			case OBJECT_GROUP:
			case CRYPTO:
			case NO:
			case CERTIFICATE:
			case PIX_WORD:
			case ASA_WORD:
			case HOSTNAME:
			case ACCESS_LIST:
			case INTRFACE:
			case CONTROLLER:
			case NAMEIF:
			case ACCESS_GROUP:
			case EXIT:
			case LINE_COMMENT:
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
#line 719 "pix.g"
				
				importer->addTCPUDPServiceToObjectGroup();
				
#line 2781 "PIXCfgParser.cpp"
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
#line 726 "pix.g"
					
					importer->icmp_type = LT(0)->getText();
					
#line 2799 "PIXCfgParser.cpp"
				}
				break;
			}
			case WORD:
			{
				icmp_word = LT(1);
				match(WORD);
				if ( inputState->guessing==0 ) {
#line 730 "pix.g"
					
					importer->icmp_spec = icmp_word->getText();
					
#line 2812 "PIXCfgParser.cpp"
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
#line 734 "pix.g"
				
				importer->addICMPServiceToObjectGroup();
				*dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
				
#line 2828 "PIXCfgParser.cpp"
			}
			break;
		}
		case OBJECT:
		{
			match(OBJECT);
			name = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 740 "pix.g"
				
				importer->addNamedObjectToObjectGroup(name->getText());
				*dbg << " GROUP MEMBER " << name->getLine() << std::endl;
				
#line 2843 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "port_object");
	
	try {      // for error handling
		match(PORT_OBJECT);
		xoperator();
		if ( inputState->guessing==0 ) {
#line 748 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->SaveTmpPortToDst();
			importer->addTCPUDPServiceToObjectGroup();
			*dbg << " PORT OBJECT TCP/UDP" << LT(0)->getText() << " ";
			
#line 2878 "PIXCfgParser.cpp"
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
#line 849 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 2921 "PIXCfgParser.cpp"
		}
		rule_extended();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 856 "pix.g"
			
			importer->pushRule();
			
#line 2930 "PIXCfgParser.cpp"
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
#line 862 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 2973 "PIXCfgParser.cpp"
		}
		rule_extended();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 869 "pix.g"
			
			importer->pushRule();
			
#line 2982 "PIXCfgParser.cpp"
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
#line 875 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "permit";
			*dbg << LT(1)->getLine() << ":" << " permit ";
			
#line 3009 "PIXCfgParser.cpp"
		}
		rule_standard();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 882 "pix.g"
			
			importer->pushRule();
			
#line 3018 "PIXCfgParser.cpp"
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
#line 888 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newPolicyRule();
			importer->action = "deny";
			*dbg << LT(1)->getLine() << ":" << " deny   ";
			
#line 3045 "PIXCfgParser.cpp"
		}
		rule_standard();
		match(NEWLINE);
		if ( inputState->guessing==0 ) {
#line 895 "pix.g"
			
			importer->pushRule();
			
#line 3054 "PIXCfgParser.cpp"
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
#line 1435 "pix.g"
			
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
			
#line 3087 "PIXCfgParser.cpp"
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
#line 905 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 3130 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 906 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 3136 "PIXCfgParser.cpp"
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
#line 912 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 3206 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 916 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 3212 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 917 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 3218 "PIXCfgParser.cpp"
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
#line 924 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 3328 "PIXCfgParser.cpp"
			}
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 928 "pix.g"
				importer->SaveTmpAddrToSrc(); *dbg << "(src) ";
#line 3334 "PIXCfgParser.cpp"
			}
			{
			switch ( LA(1)) {
			case RANGE:
			case P_EQ:
			case P_GT:
			case P_LT:
			case P_NEQ:
			{
				acl_xoperator_src();
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
			hostaddr_expr();
			if ( inputState->guessing==0 ) {
#line 930 "pix.g"
				importer->SaveTmpAddrToDst(); *dbg << "(dst) ";
#line 3366 "PIXCfgParser.cpp"
			}
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
#line 937 "pix.g"
			
			*dbg << std::endl;
			
#line 3485 "PIXCfgParser.cpp"
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
#line 944 "pix.g"
			
			importer->tmp_a = "0.0.0.0";
			importer->tmp_nm = "0.0.0.0";
			importer->SaveTmpAddrToSrc();
			
#line 3509 "PIXCfgParser.cpp"
		}
		hostaddr_expr();
		if ( inputState->guessing==0 ) {
#line 950 "pix.g"
			
			importer->SaveTmpAddrToDst();
			*dbg << "(dst) " << std::endl;
			
#line 3518 "PIXCfgParser.cpp"
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
#line 960 "pix.g"
				
				importer->protocol = LT(0)->getText();
				*dbg << "protocol " << LT(0)->getText() << " ";
				
#line 3593 "PIXCfgParser.cpp"
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
#line 966 "pix.g"
				
				importer->protocol = name->getText();
				*dbg << "protocol " << name->getText() << " ";
				
#line 3628 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_10);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::hostaddr_expr() {
	Tracer traceInOut(this, "hostaddr_expr");
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
#line 1042 "pix.g"
				
				importer->tmp_a = intf_name->getText();
				importer->tmp_nm = "interface";
				*dbg << "object " << intf_name->getText() << " ";
				
#line 3671 "PIXCfgParser.cpp"
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
#line 1049 "pix.g"
				
				importer->tmp_a = name->getText();
				importer->tmp_nm = "";
				*dbg << "object " << name->getText() << " ";
				
#line 3707 "PIXCfgParser.cpp"
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
#line 1056 "pix.g"
				
				importer->tmp_a = h->getText();
				importer->tmp_nm = "255.255.255.255";
				*dbg << h->getText() << "/255.255.255.255";
				
#line 3725 "PIXCfgParser.cpp"
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
#line 1063 "pix.g"
				
				importer->tmp_a = a->getText();
				importer->tmp_nm = m->getText();
				*dbg << a->getText() << "/" << m->getText();
				
#line 3744 "PIXCfgParser.cpp"
			}
			break;
		}
		case ANY:
		{
			match(ANY);
			if ( inputState->guessing==0 ) {
#line 1070 "pix.g"
				
				importer->tmp_a = "0.0.0.0";
				importer->tmp_nm = "0.0.0.0";
				*dbg << "0.0.0.0/0.0.0.0";
				
#line 3758 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_11);
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
#line 1099 "pix.g"
			
			importer->time_range_name = tr_name->getText();
			*dbg << "time_range " << tr_name->getText() << " ";
			
#line 3792 "PIXCfgParser.cpp"
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

void PIXCfgParser::fragments() {
	Tracer traceInOut(this, "fragments");
	
	try {      // for error handling
		match(FRAGMENTS);
		if ( inputState->guessing==0 ) {
#line 1092 "pix.g"
			
			importer->fragments = true;
			*dbg << "fragments ";
			
#line 3816 "PIXCfgParser.cpp"
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
		{
		switch ( LA(1)) {
		case NEWLINE:
		case INT_CONST:
		{
			{
			switch ( LA(1)) {
			case INT_CONST:
			{
				match(INT_CONST);
				{
				switch ( LA(1)) {
				case INTERVAL:
				{
					match(INTERVAL);
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
#line 1078 "pix.g"
			
			importer->logging = true;
			*dbg << "logging ";
			
#line 3910 "PIXCfgParser.cpp"
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
#line 976 "pix.g"
				
				importer->icmp_type = icmp_type->getText();
				importer->icmp_code = icmp_code->getText();
				importer->icmp_spec = "";
				*dbg << icmp_type->getText() << " "
				<< icmp_code->getText() << " ";
				
#line 3949 "PIXCfgParser.cpp"
			}
			break;
		}
		case WORD:
		{
			icmp_word = LT(1);
			match(WORD);
			if ( inputState->guessing==0 ) {
#line 985 "pix.g"
				
				importer->icmp_spec = icmp_word->getText();
				*dbg << icmp_word->getText() << " ";
				
#line 3963 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_14);
		} else {
			throw;
		}
	}
}

void PIXCfgParser::acl_xoperator_src() {
	Tracer traceInOut(this, "acl_xoperator_src");
	
	try {      // for error handling
		xoperator();
		if ( inputState->guessing==0 ) {
#line 993 "pix.g"
			
			importer->SaveTmpPortToSrc();
			
#line 3994 "PIXCfgParser.cpp"
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

void PIXCfgParser::acl_xoperator_dst() {
	Tracer traceInOut(this, "acl_xoperator_dst");
	
	try {      // for error handling
		xoperator();
		if ( inputState->guessing==0 ) {
#line 999 "pix.g"
			
			importer->SaveTmpPortToDst();
			
#line 4017 "PIXCfgParser.cpp"
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

void PIXCfgParser::established() {
	Tracer traceInOut(this, "established");
	
	try {      // for error handling
		match(ESTABLISHED);
		if ( inputState->guessing==0 ) {
#line 1085 "pix.g"
			
			importer->established = true;
			*dbg << "established ";
			
#line 4041 "PIXCfgParser.cpp"
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
#line 1007 "pix.g"
			
			importer->tmp_port_op = LT(0)->getText();
			*dbg << LT(0)->getText() << " ";
			
#line 4092 "PIXCfgParser.cpp"
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
	Tracer traceInOut(this, "port_range");
	
	try {      // for error handling
		match(RANGE);
		pair_of_ports_spec();
		if ( inputState->guessing==0 ) {
#line 1022 "pix.g"
			
			importer->tmp_port_op = "range";
			*dbg << "range ";
			
#line 4118 "PIXCfgParser.cpp"
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

void PIXCfgParser::port_spec() {
	Tracer traceInOut(this, "port_spec");
	
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
#line 1015 "pix.g"
			
			importer->tmp_port_spec = (std::string(" ") + LT(0)->getText());
			*dbg << LT(0)->getText() << " " << importer->tmp_port_spec;
			
#line 4159 "PIXCfgParser.cpp"
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

void PIXCfgParser::pair_of_ports_spec() {
	Tracer traceInOut(this, "pair_of_ports_spec");
	ANTLR_USE_NAMESPACE(antlr)RefToken  s1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  s2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  e1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  e2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case WORD:
		{
			s1 = LT(1);
			match(WORD);
			break;
		}
		case INT_CONST:
		{
			s2 = LT(1);
			match(INT_CONST);
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
			e1 = LT(1);
			match(WORD);
			break;
		}
		case INT_CONST:
		{
			e2 = LT(1);
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
#line 1029 "pix.g"
			
			importer->tmp_port_spec = "";
			if (s1) importer->tmp_port_spec += s1->getText();
			if (s2) importer->tmp_port_spec += s2->getText();
			importer->tmp_port_spec += " ";
			if (e1) importer->tmp_port_spec += e1->getText();
			if (e2) importer->tmp_port_spec += e2->getText();
			*dbg << "pair of ports: " << importer->tmp_port_spec;
			
#line 4231 "PIXCfgParser.cpp"
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

void PIXCfgParser::interface_command() {
	Tracer traceInOut(this, "interface_command");
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		in = LT(1);
		match(WORD);
		if ( inputState->guessing==0 ) {
#line 1158 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			importer->newInterface( in->getText() );
			*dbg << in->getLine() << ":"
			<< " INTRFACE: " << in->getText() << std::endl;
			
#line 4259 "PIXCfgParser.cpp"
		}
		{
		switch ( LA(1)) {
		case WORD:
		case INT_CONST:
		{
			{ // ( ... )+
			int _cnt164=0;
			for (;;) {
				if ((LA(1) == INT_CONST)) {
					match(INT_CONST);
				}
				else if ((LA(1) == WORD) && (_tokenSet_16.member(LA(2)))) {
					match(WORD);
				}
				else {
					if ( _cnt164>=1 ) { goto _loop164; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt164++;
			}
			_loop164:;
			}  // ( ... )+
			break;
		}
		case NEWLINE:
		{
			{
			match(NEWLINE);
			{
			if ((_tokenSet_17.member(LA(1))) && (_tokenSet_18.member(LA(2)))) {
				{ // ( ... )+
				int _cnt168=0;
				for (;;) {
					if ((_tokenSet_17.member(LA(1))) && (_tokenSet_18.member(LA(2)))) {
						interface_parameters();
					}
					else {
						if ( _cnt168>=1 ) { goto _loop168; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
					}
					
					_cnt168++;
				}
				_loop168:;
				}  // ( ... )+
			}
			else if ((_tokenSet_1.member(LA(1))) && (_tokenSet_19.member(LA(2)))) {
				{
				}
				if ( inputState->guessing==0 ) {
#line 1175 "pix.g"
					
					importer->ignoreCurrentInterface();
					*dbg<< LT(1)->getLine() << ":"
					<< " EMPTY INTERFACE " << std::endl;
					
#line 4316 "PIXCfgParser.cpp"
				}
			}
			else {
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
#line 1198 "pix.g"
			
			importer->setCurrentLineNumber(LT(0)->getLine());
			
#line 4353 "PIXCfgParser.cpp"
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
			recover(ex,_tokenSet_20);
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
#line 1224 "pix.g"
			
			importer->setInterfaceVlanId(vlan_id->getText());
			*dbg << " VLAN: " << vlan_id->getText() << std::endl;
			
#line 4483 "PIXCfgParser.cpp"
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
#line 1276 "pix.g"
			
			importer->setInterfaceSecurityLevel(sec_level->getText());
			*dbg << "SEC_LEVEL: " << sec_level->getText() << std::endl;
			
#line 4510 "PIXCfgParser.cpp"
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
#line 1299 "pix.g"
			
			std::string label = (intf_label) ? intf_label->getText() : "";
			std::string seclevel = (sec_level) ? sec_level->getText() : "";
			importer->setInterfaceParametes(p_intf->getText(), label, seclevel);
			*dbg << " NAMEIF: "
			<< p_intf->getText() << label << seclevel << std::endl;
			
#line 4564 "PIXCfgParser.cpp"
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
#line 1311 "pix.g"
			
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
			
#line 4596 "PIXCfgParser.cpp"
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
#line 1423 "pix.g"
			
			importer->addMessageToLog("Switch port vlan " + vlan_num->getText());
			*dbg << "Switch port vlan " <<  vlan_num->getText() << std::endl;
			
#line 4625 "PIXCfgParser.cpp"
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
#line 1326 "pix.g"
			
			importer->ignoreCurrentInterface();
			*dbg<< LT(1)->getLine() << ":"
			<< " INTERFACE SHUTDOWN " << std::endl;
			
#line 4650 "PIXCfgParser.cpp"
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
#line 1268 "pix.g"
			
			*dbg << " INTERFACE \"NO\" COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 4708 "PIXCfgParser.cpp"
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
#line 1260 "pix.g"
			
			*dbg << " UNSUPPORTED INTERFACE COMMAND: "
			<< LT(0)->getText() << std::endl;
			consumeUntil(NEWLINE);
			
#line 4810 "PIXCfgParser.cpp"
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
#line 1361 "pix.g"
			
			std::string label = lbl->getText();
			std::string addr = dhcp->getText();
			importer->addInterfaceAddress(label, addr, "");
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << std::endl;
			// there can be some other parameters after "dhcp", such as "setroute", "retry" etc.
			// which we do not support
			consumeUntil(NEWLINE);
			
#line 4901 "PIXCfgParser.cpp"
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
#line 1374 "pix.g"
			
			std::string label = lbl->getText();
			std::string addr = a->getText();
			std::string netm = m->getText();
			importer->addInterfaceAddress(label, addr, netm);
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << "/" << netm << std::endl;
			// in case there are some other parameters after address and netmask
			consumeUntil(NEWLINE);
			
#line 4939 "PIXCfgParser.cpp"
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
#line 1391 "pix.g"
			
			std::string addr = dhcp->getText();
			importer->addInterfaceAddress(addr, "");
			*dbg << LT(1)->getLine() << ":"
			<< " INTRFACE ADDRESS: " << addr << std::endl;
			consumeUntil(NEWLINE);
			
#line 4968 "PIXCfgParser.cpp"
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
#line 1402 "pix.g"
			
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
			
#line 5028 "PIXCfgParser.cpp"
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
	"\"object\"",
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
	"\"interface\"",
	"\"any\"",
	"\"log\"",
	"\"log-input\"",
	"\"interval\"",
	"\"established\"",
	"\"fragments\"",
	"\"time-range\"",
	"\"controller\"",
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
	"\"rip\"",
	"\"security-level\"",
	"\"shutdown\"",
	"\"address\"",
	"\"dhcp\"",
	"\"standby\"",
	"\"switchport\"",
	"\"access\"",
	"\"remark\"",
	"\"access-group\"",
	"\"exit\"",
	"LINE_COMMENT",
	"COLON_COMMENT",
	"\"secondary\"",
	"\"setroute\"",
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
	"EXLAMATION",
	0
};

const unsigned long PIXCfgParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_0(_tokenSet_0_data_,6);
const unsigned long PIXCfgParser::_tokenSet_1_data_[] = { 134222706UL, 703595520UL, 49216UL, 240UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "names" "name" WORD "object" "object-group" 
// "crypto" "no" "certificate" "PIX" "ASA" "hostname" "access-list" "interface" 
// "controller" "nameif" "access-group" "exit" LINE_COMMENT COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_1(_tokenSet_1_data_,8);
const unsigned long PIXCfgParser::_tokenSet_2_data_[] = { 268427328UL, 2147484904UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" "ospf" 
// "pcp" "pim" "pptp" "snp" "object" "icmp" "icmp6" "tcp" "udp" "object-group" 
// "permit" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_2(_tokenSet_2_data_,6);
const unsigned long PIXCfgParser::_tokenSet_3_data_[] = { 268427328UL, 1256UL, 1UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" "ospf" 
// "pcp" "pim" "pptp" "snp" "object" "icmp" "icmp6" "tcp" "udp" "object-group" 
// "deny" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long PIXCfgParser::_tokenSet_4_data_[] = { 1073741824UL, 788480UL, 0UL, 0UL, 0UL, 0UL };
// "description" "group-object" "service-object" "port-object" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_4(_tokenSet_4_data_,6);
const unsigned long PIXCfgParser::_tokenSet_5_data_[] = { 2281707378UL, 703595520UL, 49344UL, 240UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "names" "name" IPV4 WORD "object" "host" "object-group" 
// "crypto" "no" "certificate" "PIX" "ASA" "hostname" "access-list" "interface" 
// "any" "controller" "nameif" "access-group" "exit" LINE_COMMENT COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_5(_tokenSet_5_data_,8);
const unsigned long PIXCfgParser::_tokenSet_6_data_[] = { 134209600UL, 16UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "ah" "eigrp" "esp" "gre" "igmp" "igrp" "ipinip" "ipsec" "nos" "ospf" 
// "pcp" "pim" "pptp" "snp" INT_CONST 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_6(_tokenSet_6_data_,6);
const unsigned long PIXCfgParser::_tokenSet_7_data_[] = { 134222706UL, 703596033UL, 49276UL, 240UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "names" "name" WORD "object" "range" "destination" 
// "object-group" "crypto" "no" "certificate" "PIX" "ASA" "hostname" "access-list" 
// "eq" "gt" "lt" "neq" "interface" "controller" "nameif" "access-group" 
// "exit" LINE_COMMENT COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_7(_tokenSet_7_data_,8);
const unsigned long PIXCfgParser::_tokenSet_8_data_[] = { 2281707378UL, 703596033UL, 64508UL, 240UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "names" "name" IPV4 WORD "object" "host" "range" 
// "destination" "object-group" "crypto" "no" "certificate" "PIX" "ASA" 
// "hostname" "access-list" "eq" "gt" "lt" "neq" "interface" "any" "log" 
// "log-input" "established" "fragments" "time-range" "controller" "nameif" 
// "access-group" "exit" LINE_COMMENT COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_8(_tokenSet_8_data_,8);
const unsigned long PIXCfgParser::_tokenSet_9_data_[] = { 16UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_9(_tokenSet_9_data_,6);
const unsigned long PIXCfgParser::_tokenSet_10_data_[] = { 2281702400UL, 1024UL, 192UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IPV4 "object" "host" "object-group" "interface" "any" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_10(_tokenSet_10_data_,8);
const unsigned long PIXCfgParser::_tokenSet_11_data_[] = { 2281706512UL, 1041UL, 15356UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE IPV4 WORD "object" "host" "range" INT_CONST "object-group" "eq" 
// "gt" "lt" "neq" "interface" "any" "log" "log-input" "established" "fragments" 
// "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_11(_tokenSet_11_data_,8);
const unsigned long PIXCfgParser::_tokenSet_12_data_[] = { 16UL, 0UL, 4864UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_12(_tokenSet_12_data_,8);
const unsigned long PIXCfgParser::_tokenSet_13_data_[] = { 16UL, 0UL, 768UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_13(_tokenSet_13_data_,8);
const unsigned long PIXCfgParser::_tokenSet_14_data_[] = { 16UL, 0UL, 13056UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_14(_tokenSet_14_data_,8);
const unsigned long PIXCfgParser::_tokenSet_15_data_[] = { 16UL, 0UL, 15104UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "log" "log-input" "established" "fragments" "time-range" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_15(_tokenSet_15_data_,8);
const unsigned long PIXCfgParser::_tokenSet_16_data_[] = { 134222706UL, 703595536UL, 49216UL, 240UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "names" "name" WORD "object" INT_CONST "object-group" 
// "crypto" "no" "certificate" "PIX" "ASA" "hostname" "access-list" "interface" 
// "controller" "nameif" "access-group" "exit" LINE_COMMENT COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_16(_tokenSet_16_data_,8);
const unsigned long PIXCfgParser::_tokenSet_17_data_[] = { 1094844480UL, 2097152UL, 1073709056UL, 2UL, 0UL, 0UL, 0UL, 0UL };
// "ip" "igmp" "ospf" "pim" "description" "no" "nameif" "vlan" "speed" 
// "duplex" "ddns" "forward" "delay" "hold-time" "ipv6" "mac-address" "multicast" 
// PPPOE "rip" "security-level" "shutdown" "switchport" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_17(_tokenSet_17_data_,8);
const unsigned long PIXCfgParser::_tokenSet_18_data_[] = { 4176UL, 16UL, 1879080960UL, 4UL, 0UL, 0UL, 0UL, 0UL };
// NEWLINE "ip" WORD INT_CONST "nameif" "security-level" "shutdown" "address" 
// "access" 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_18(_tokenSet_18_data_,8);
const unsigned long PIXCfgParser::_tokenSet_19_data_[] = { 402661362UL, 1005691908UL, 49216UL, 240UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "community-list" "names" "name" IPV4 IPV6 WORD 
// "object" "network" "service" "object-group" "protocol" "icmp-object" 
// "icmp-type" "crypto" "no" "certificate" "PIX" "ASA" "Version" "hostname" 
// STRING "access-list" "interface" "controller" "nameif" "access-group" 
// "exit" LINE_COMMENT COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_19(_tokenSet_19_data_,8);
const unsigned long PIXCfgParser::_tokenSet_20_data_[] = { 1229067122UL, 703595520UL, 1073725504UL, 242UL, 0UL, 0UL, 0UL, 0UL };
// EOF NEWLINE "quit" "ip" "names" "name" WORD "igmp" "ospf" "pim" "object" 
// "description" "object-group" "crypto" "no" "certificate" "PIX" "ASA" 
// "hostname" "access-list" "interface" "controller" "nameif" "vlan" "speed" 
// "duplex" "ddns" "forward" "delay" "hold-time" "ipv6" "mac-address" "multicast" 
// PPPOE "rip" "security-level" "shutdown" "switchport" "access-group" 
// "exit" LINE_COMMENT COLON_COMMENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgParser::_tokenSet_20(_tokenSet_20_data_,8);



/* $ANTLR 2.7.7 (20090306): "ifconfig.g" -> "IfconfigCfgParser.cpp"$ */
#line 43 "ifconfig.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "IfconfigCfgParser.cpp"
#include "IfconfigCfgParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 49 "ifconfig.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#line 20 "IfconfigCfgParser.cpp"
#line 1 "ifconfig.g"
#line 22 "IfconfigCfgParser.cpp"
IfconfigCfgParser::IfconfigCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

IfconfigCfgParser::IfconfigCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
}

IfconfigCfgParser::IfconfigCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

IfconfigCfgParser::IfconfigCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
}

IfconfigCfgParser::IfconfigCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
}

void IfconfigCfgParser::cfgfile() {
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
			case LLADDR:
			{
				bsd_hwaddr_line();
				break;
			}
			case HWADDR:
			{
				linux_hwaddr_line();
				break;
			}
			case GROUPS:
			{
				groups();
				break;
			}
			case UP:
			case UPPER_BROADCAST:
			case UPPER_POINTOPOINT:
			case UPPER_LOOPBACK:
			case UPPER_NOARP:
			case UPPER_RUNNING:
			case LOOPBACK:
			{
				linux_interface_flags();
				break;
			}
			case COLLISIONS:
			case RX:
			case TX:
			{
				linux_interface_statistics();
				break;
			}
			case NEWLINE:
			{
				match(NEWLINE);
				break;
			}
			case DOUBLE_NEWLINE:
			{
				match(DOUBLE_NEWLINE);
				break;
			}
			default:
				if ((LA(1) == WORD) && (LA(2) == COLON)) {
					bsd_interface_line();
				}
				else if ((LA(1) == WORD) && (LA(2) == LINK)) {
					linux_interface_line();
				}
				else if ((LA(1) == INET) && (LA(2) == IPV4)) {
					bsd_inet_address();
				}
				else if ((LA(1) == INET) && (LA(2) == ADDR)) {
					linux_inet_address();
				}
				else if ((LA(1) == INET6) && (LA(2) == IPV6)) {
					bsd_inet6_address();
				}
				else if ((LA(1) == INET6) && (LA(2) == ADDR)) {
					linux_inet6_address();
				}
				else if (((LA(1) >= PRIORITY && LA(1) <= WORD)) && (_tokenSet_0.member(LA(2)))) {
					unknown_line();
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

void IfconfigCfgParser::comment() {
	Tracer traceInOut(this, "comment");
	
	try {      // for error handling
		match(LINE_COMMENT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::bsd_interface_line() {
	Tracer traceInOut(this, "bsd_interface_line");
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 213 "ifconfig.g"
	InterfaceSpec is;
#line 156 "IfconfigCfgParser.cpp"
	
	try {      // for error handling
		in = LT(1);
		match(WORD);
		match(COLON);
		match(FLAGS);
		match(EQUAL);
		match(INT_CONST);
#line 216 "ifconfig.g"
		
		// interface name and status
		is.name = in->getText();
		importer->newInterface(is);
		consumeUntil(NEWLINE);
		
#line 172 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::linux_interface_line() {
	Tracer traceInOut(this, "linux_interface_line");
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 229 "ifconfig.g"
	InterfaceSpec is;
#line 185 "IfconfigCfgParser.cpp"
	
	try {      // for error handling
		in = LT(1);
		match(WORD);
		match(LINK);
		match(ENCAP);
		match(COLON);
		match(WORD);
#line 232 "ifconfig.g"
		
		// interface name and status
		is.name = in->getText();
		importer->newInterface(is);
		
#line 200 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::bsd_hwaddr_line() {
	Tracer traceInOut(this, "bsd_hwaddr_line");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(LLADDR);
		addr = LT(1);
		match(MAC_ADDRESS);
#line 244 "ifconfig.g"
		
		importer->HwAddressForCurrentInterface(addr->getText());
		
#line 220 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::linux_hwaddr_line() {
	Tracer traceInOut(this, "linux_hwaddr_line");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(HWADDR);
		addr = LT(1);
		match(MAC_ADDRESS);
#line 252 "ifconfig.g"
		
		importer->HwAddressForCurrentInterface(addr->getText());
		
#line 240 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::bsd_inet_address() {
	Tracer traceInOut(this, "bsd_inet_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  netm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  bcast = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 261 "ifconfig.g"
	AddressSpec as;
#line 255 "IfconfigCfgParser.cpp"
	
	try {      // for error handling
		match(INET);
		addr = LT(1);
		match(IPV4);
		match(NETMASK);
		netm = LT(1);
		match(HEX_CONST);
		match(BROADCAST);
		bcast = LT(1);
		match(IPV4);
#line 264 "ifconfig.g"
		
		as.at = AddressSpec::INTERFACE_CONFIGURATION;
		as.address = addr->getText();
		as.netmask = netm->getText();
		as.broadcast = bcast->getText();
		importer->inetConfigurationForCurrentInterface(as);
		
#line 275 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::linux_inet_address() {
	Tracer traceInOut(this, "linux_inet_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  bcast = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  netm = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 274 "ifconfig.g"
	AddressSpec as;
#line 290 "IfconfigCfgParser.cpp"
	
	try {      // for error handling
		match(INET);
		match(ADDR);
		match(COLON);
		addr = LT(1);
		match(IPV4);
#line 278 "ifconfig.g"
		
		as.at = AddressSpec::INTERFACE_CONFIGURATION;
		as.address = addr->getText();
		
#line 303 "IfconfigCfgParser.cpp"
		{
		switch ( LA(1)) {
		case BCAST:
		case MASK:
		{
			{
			{
			switch ( LA(1)) {
			case BCAST:
			{
				match(BCAST);
				match(COLON);
				bcast = LT(1);
				match(IPV4);
				break;
			}
			case MASK:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(MASK);
			match(COLON);
			netm = LT(1);
			match(IPV4);
			}
#line 284 "ifconfig.g"
			
			as.netmask = netm->getText();
			if (bcast) as.broadcast = bcast->getText();
			
#line 340 "IfconfigCfgParser.cpp"
			break;
		}
		case P_T_P:
		{
			{
			match(P_T_P);
			match(COLON);
			match(IPV4);
			match(MASK);
			match(COLON);
			match(IPV4);
			}
#line 290 "ifconfig.g"
			
			// we do not support p2p interfaces at this time
			
#line 357 "IfconfigCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 294 "ifconfig.g"
		
		importer->inetConfigurationForCurrentInterface(as);
		
#line 370 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::bsd_inet6_address() {
	Tracer traceInOut(this, "bsd_inet6_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  netm = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 302 "ifconfig.g"
	AddressSpec as;
#line 384 "IfconfigCfgParser.cpp"
	
	try {      // for error handling
		match(INET6);
		addr = LT(1);
		match(IPV6);
		match(PERCENT);
		match(WORD);
		match(PREFIXLEN);
		netm = LT(1);
		match(INT_CONST);
		match(SCOPEID);
		match(HEX_CONST);
#line 305 "ifconfig.g"
		
		as.at = AddressSpec::INTERFACE_CONFIGURATION;
		as.address = addr->getText();
		as.netmask = netm->getText();
		importer->inet6ConfigurationForCurrentInterface(as);
		
#line 404 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::linux_inet6_address() {
	Tracer traceInOut(this, "linux_inet6_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  netm = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 314 "ifconfig.g"
	AddressSpec as;
#line 418 "IfconfigCfgParser.cpp"
	
	try {      // for error handling
		match(INET6);
		match(ADDR);
		match(COLON);
		addr = LT(1);
		match(IPV6);
		match(SLASH);
		netm = LT(1);
		match(INT_CONST);
		match(SCOPE);
		match(COLON);
		{
		switch ( LA(1)) {
		case HOST:
		{
			match(HOST);
			break;
		}
		case LINK:
		{
			match(LINK);
			break;
		}
		case GLOBAL:
		{
			match(GLOBAL);
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
#line 317 "ifconfig.g"
		
		as.at = AddressSpec::INTERFACE_CONFIGURATION;
		as.address = addr->getText();
		as.netmask = netm->getText();
		importer->inet6ConfigurationForCurrentInterface(as);
		
#line 466 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::groups() {
	Tracer traceInOut(this, "groups");
	
	try {      // for error handling
		match(GROUPS);
		match(COLON);
		groups_list();
		match(NEWLINE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::linux_interface_flags() {
	Tracer traceInOut(this, "linux_interface_flags");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case UP:
		{
			match(UP);
			break;
		}
		case UPPER_BROADCAST:
		{
			match(UPPER_BROADCAST);
			break;
		}
		case UPPER_POINTOPOINT:
		{
			match(UPPER_POINTOPOINT);
			break;
		}
		case UPPER_LOOPBACK:
		{
			match(UPPER_LOOPBACK);
			break;
		}
		case UPPER_NOARP:
		{
			match(UPPER_NOARP);
			break;
		}
		case UPPER_RUNNING:
		{
			match(UPPER_RUNNING);
			break;
		}
		case LOOPBACK:
		{
			match(LOOPBACK);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 186 "ifconfig.g"
		
		consumeUntil(NEWLINE);
		
#line 540 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::linux_interface_statistics() {
	Tracer traceInOut(this, "linux_interface_statistics");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case COLLISIONS:
		{
			{
			match(COLLISIONS);
			match(COLON);
			}
			break;
		}
		case RX:
		{
			match(RX);
			break;
		}
		case TX:
		{
			match(TX);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 200 "ifconfig.g"
		
		consumeUntil(NEWLINE);
		
#line 582 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::unknown_line() {
	Tracer traceInOut(this, "unknown_line");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case PRIORITY:
		{
			match(PRIORITY);
			break;
		}
		case MEDIA:
		{
			match(MEDIA);
			break;
		}
		case STATUS:
		{
			match(STATUS);
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
#line 164 "ifconfig.g"
		
		consumeUntil(NEWLINE);
		
#line 626 "IfconfigCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigCfgParser::groups_list() {
	Tracer traceInOut(this, "groups_list");
	
	try {      // for error handling
		match(WORD);
#line 334 "ifconfig.g"
		importer->addGroupToCurrentInterface(LT(0)->getText());
#line 641 "IfconfigCfgParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == WORD)) {
				match(WORD);
#line 336 "ifconfig.g"
				importer->addGroupToCurrentInterface(LT(0)->getText());
#line 648 "IfconfigCfgParser.cpp"
			}
			else {
				goto _loop28;
			}
			
		}
		_loop28:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_2);
	}
}

void IfconfigCfgParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* IfconfigCfgParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"NEWLINE",
	"DOUBLE_NEWLINE",
	"LINE_COMMENT",
	"\"priority\"",
	"\"media\"",
	"\"status\"",
	"WORD",
	"\"UP\"",
	"\"BROADCAST\"",
	"\"POINTOPOINT\"",
	"\"LOOPBACK\"",
	"\"NOARP\"",
	"\"RUNNING\"",
	"\"Loopback\"",
	"\"collisions\"",
	"COLON",
	"\"RX\"",
	"\"TX\"",
	"\"flags\"",
	"EQUAL",
	"INT_CONST",
	"\"Link\"",
	"\"encap\"",
	"LLADDR",
	"MAC_ADDRESS",
	"\"HWaddr\"",
	"\"inet\"",
	"IPV4",
	"\"netmask\"",
	"HEX_CONST",
	"\"broadcast\"",
	"\"addr\"",
	"\"Bcast\"",
	"\"Mask\"",
	"\"P-t-P\"",
	"\"inet6\"",
	"IPV6",
	"PERCENT",
	"\"prefixlen\"",
	"\"scopeid\"",
	"SLASH",
	"\"Scope\"",
	"\"Host\"",
	"\"Global\"",
	"\"groups\"",
	"\"mtu\"",
	"\"lladdr\"",
	"Whitespace",
	"NUMBER",
	"NEG_INT_CONST",
	"HEX_DIGIT",
	"DIGIT",
	"NUM_3DIGIT",
	"NUM_HEX_4DIGIT",
	"NUMBER_ADDRESS_OR_WORD",
	"AMPERSAND",
	"STAR",
	"MINUS",
	"DOT",
	"QUESTION",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"OPENING_SQUARE",
	"CLOSING_SQUARE",
	"OPENING_BRACE",
	"CLOSING_BRACE",
	"LESS_THAN",
	"GREATER_THAN",
	0
};

const unsigned long IfconfigCfgParser::_tokenSet_0_data_[] = { 1748500466UL, 65664UL, 0UL, 0UL };
// EOF NEWLINE DOUBLE_NEWLINE LINE_COMMENT "priority" "media" "status" 
// WORD "UP" "BROADCAST" "POINTOPOINT" "LOOPBACK" "NOARP" "RUNNING" "Loopback" 
// "collisions" "RX" "TX" LLADDR "HWaddr" "inet" "inet6" "groups" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IfconfigCfgParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long IfconfigCfgParser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet IfconfigCfgParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long IfconfigCfgParser::_tokenSet_2_data_[] = { 16UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet IfconfigCfgParser::_tokenSet_2(_tokenSet_2_data_,4);



/* $ANTLR 2.7.7 (20100319): "ifconfig_linux.g" -> "IfconfigLinuxCfgParser.cpp"$ */
#line 43 "ifconfig_linux.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "IfconfigLinuxCfgParser.cpp"
#include "IfconfigLinuxCfgParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 49 "ifconfig_linux.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#line 20 "IfconfigLinuxCfgParser.cpp"
#line 1 "ifconfig_linux.g"
#line 22 "IfconfigLinuxCfgParser.cpp"
IfconfigLinuxCfgParser::IfconfigLinuxCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

IfconfigLinuxCfgParser::IfconfigLinuxCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
}

IfconfigLinuxCfgParser::IfconfigLinuxCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

IfconfigLinuxCfgParser::IfconfigLinuxCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
}

IfconfigLinuxCfgParser::IfconfigLinuxCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
}

void IfconfigLinuxCfgParser::cfgfile() {
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
			case HWADDR:
			{
				hwaddr_line();
				break;
			}
			case INET:
			{
				inet_address();
				break;
			}
			case INET6:
			{
				inet6_address();
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
				interface_flags();
				break;
			}
			case INTERRUPT:
			case COLLISIONS:
			case RX:
			case TX:
			{
				interface_statistics();
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
				if ((LA(1) == WORD) && (LA(2) == COLON || LA(2) == DOT || LA(2) == LINK)) {
					interface_line();
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

void IfconfigLinuxCfgParser::comment() {
	Tracer traceInOut(this, "comment");
	
	try {      // for error handling
		match(LINE_COMMENT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigLinuxCfgParser::interface_line() {
	Tracer traceInOut(this, "interface_line");
	ANTLR_USE_NAMESPACE(antlr)RefToken  in1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  in2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  lbl1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  lbl2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 206 "ifconfig_linux.g"
	InterfaceSpec is;
#line 150 "IfconfigLinuxCfgParser.cpp"
	
	try {      // for error handling
		in1 = LT(1);
		match(WORD);
		{
		switch ( LA(1)) {
		case DOT:
		{
			match(DOT);
			in2 = LT(1);
			match(INT_CONST);
			break;
		}
		case COLON:
		case LINK:
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
		case COLON:
		{
			match(COLON);
			{
			switch ( LA(1)) {
			case WORD:
			{
				lbl1 = LT(1);
				match(WORD);
				break;
			}
			case INT_CONST:
			{
				lbl2 = LT(1);
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
		case LINK:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(LINK);
		match(ENCAP);
		match(COLON);
		match(WORD);
#line 212 "ifconfig_linux.g"
		
		// interface name and status
		if (in2) is.name = in1->getText() + "." + in2->getText();
		else is.name = in1->getText();
		if (lbl1) is.label = lbl1->getText();
		if (lbl2) is.label = lbl2->getText();
		importer->newInterface(is);
		
#line 225 "IfconfigLinuxCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigLinuxCfgParser::hwaddr_line() {
	Tracer traceInOut(this, "hwaddr_line");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(HWADDR);
		addr = LT(1);
		match(MAC_ADDRESS);
#line 227 "ifconfig_linux.g"
		
		importer->HwAddressForCurrentInterface(addr->getText());
		
#line 245 "IfconfigLinuxCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigLinuxCfgParser::inet_address() {
	Tracer traceInOut(this, "inet_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  bcast = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  netm = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 236 "ifconfig_linux.g"
	AddressSpec as;
#line 260 "IfconfigLinuxCfgParser.cpp"
	
	try {      // for error handling
		match(INET);
		match(ADDR);
		match(COLON);
		addr = LT(1);
		match(IPV4);
#line 240 "ifconfig_linux.g"
		
		as.at = AddressSpec::INTERFACE_CONFIGURATION;
		as.address = addr->getText();
		
#line 273 "IfconfigLinuxCfgParser.cpp"
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
#line 246 "ifconfig_linux.g"
			
			as.netmask = netm->getText();
			if (bcast) as.broadcast = bcast->getText();
			
#line 310 "IfconfigLinuxCfgParser.cpp"
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
#line 252 "ifconfig_linux.g"
			
			// we do not support p2p interfaces at this time
			
#line 327 "IfconfigLinuxCfgParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 256 "ifconfig_linux.g"
		
		importer->inetConfigurationForCurrentInterface(as);
		
#line 340 "IfconfigLinuxCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigLinuxCfgParser::inet6_address() {
	Tracer traceInOut(this, "inet6_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  netm = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 264 "ifconfig_linux.g"
	AddressSpec as;
#line 354 "IfconfigLinuxCfgParser.cpp"
	
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
#line 267 "ifconfig_linux.g"
		
		as.at = AddressSpec::INTERFACE_CONFIGURATION;
		as.address = addr->getText();
		as.netmask = netm->getText();
		importer->inet6ConfigurationForCurrentInterface(as);
		
#line 402 "IfconfigLinuxCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigLinuxCfgParser::groups() {
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

void IfconfigLinuxCfgParser::interface_flags() {
	Tracer traceInOut(this, "interface_flags");
	
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
#line 178 "ifconfig_linux.g"
		
		consumeUntil(NEWLINE);
		
#line 476 "IfconfigLinuxCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigLinuxCfgParser::interface_statistics() {
	Tracer traceInOut(this, "interface_statistics");
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case INTERRUPT:
		case COLLISIONS:
		{
			{
			{
			switch ( LA(1)) {
			case INTERRUPT:
			{
				match(INTERRUPT);
				break;
			}
			case COLLISIONS:
			{
				match(COLLISIONS);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
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
#line 192 "ifconfig_linux.g"
		
		consumeUntil(NEWLINE);
		
#line 536 "IfconfigLinuxCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigLinuxCfgParser::unknown_line() {
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
#line 156 "ifconfig_linux.g"
		
		consumeUntil(NEWLINE);
		
#line 580 "IfconfigLinuxCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigLinuxCfgParser::groups_list() {
	Tracer traceInOut(this, "groups_list");
	
	try {      // for error handling
		match(WORD);
#line 284 "ifconfig_linux.g"
		importer->addGroupToCurrentInterface(LT(0)->getText());
#line 595 "IfconfigLinuxCfgParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == WORD)) {
				match(WORD);
#line 286 "ifconfig_linux.g"
				importer->addGroupToCurrentInterface(LT(0)->getText());
#line 602 "IfconfigLinuxCfgParser.cpp"
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

void IfconfigLinuxCfgParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* IfconfigLinuxCfgParser::tokenNames[] = {
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
	"\"Interrupt\"",
	"\"collisions\"",
	"COLON",
	"\"RX\"",
	"\"TX\"",
	"DOT",
	"INT_CONST",
	"\"Link\"",
	"\"encap\"",
	"\"HWaddr\"",
	"MAC_ADDRESS",
	"\"inet\"",
	"\"addr\"",
	"IPV4",
	"\"Bcast\"",
	"\"Mask\"",
	"\"P-t-P\"",
	"\"inet6\"",
	"IPV6",
	"SLASH",
	"\"Scope\"",
	"\"Host\"",
	"\"Global\"",
	"\"groups\"",
	"\"flags\"",
	"\"broadcast\"",
	"\"netmask\"",
	"\"prefixlen\"",
	"\"scopeid\"",
	"\"mtu\"",
	"\"lladdr\"",
	"Whitespace",
	"HEX_CONST",
	"NUMBER",
	"NEG_INT_CONST",
	"HEX_DIGIT",
	"DIGIT",
	"NUM_3DIGIT",
	"NUM_HEX_4DIGIT",
	"NUMBER_ADDRESS_OR_WORD",
	"PERCENT",
	"AMPERSAND",
	"STAR",
	"MINUS",
	"EQUAL",
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

const unsigned long IfconfigLinuxCfgParser::_tokenSet_0_data_[] = { 678428658UL, 520UL, 0UL, 0UL };
// EOF NEWLINE DOUBLE_NEWLINE LINE_COMMENT "priority" "media" "status" 
// WORD "UP" "BROADCAST" "POINTOPOINT" "LOOPBACK" "NOARP" "RUNNING" "Loopback" 
// "Interrupt" "collisions" "RX" "TX" "HWaddr" "inet" "inet6" "groups" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IfconfigLinuxCfgParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long IfconfigLinuxCfgParser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet IfconfigLinuxCfgParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long IfconfigLinuxCfgParser::_tokenSet_2_data_[] = { 16UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet IfconfigLinuxCfgParser::_tokenSet_2(_tokenSet_2_data_,4);



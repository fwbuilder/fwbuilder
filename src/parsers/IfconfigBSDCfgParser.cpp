/* $ANTLR 2.7.7 (20100319): "ifconfig_bsd.g" -> "IfconfigBSDCfgParser.cpp"$ */
#line 43 "ifconfig_bsd.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "IfconfigBSDCfgParser.cpp"
#include "IfconfigBSDCfgParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 49 "ifconfig_bsd.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#line 20 "IfconfigBSDCfgParser.cpp"
#line 1 "ifconfig_bsd.g"
#line 22 "IfconfigBSDCfgParser.cpp"
IfconfigBSDCfgParser::IfconfigBSDCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

IfconfigBSDCfgParser::IfconfigBSDCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
}

IfconfigBSDCfgParser::IfconfigBSDCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

IfconfigBSDCfgParser::IfconfigBSDCfgParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
}

IfconfigBSDCfgParser::IfconfigBSDCfgParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
}

void IfconfigBSDCfgParser::cfgfile() {
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

void IfconfigBSDCfgParser::comment() {
	Tracer traceInOut(this, "comment");
	
	try {      // for error handling
		match(LINE_COMMENT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigBSDCfgParser::interface_line() {
	Tracer traceInOut(this, "interface_line");
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 165 "ifconfig_bsd.g"
	InterfaceSpec is;
#line 128 "IfconfigBSDCfgParser.cpp"
	
	try {      // for error handling
		in = LT(1);
		match(WORD);
		match(COLON);
		match(FLAGS);
		match(EQUAL);
		match(INT_CONST);
#line 168 "ifconfig_bsd.g"
		
		// interface name and status
		is.name = in->getText();
		importer->newInterface(is);
		consumeUntil(NEWLINE);
		
#line 144 "IfconfigBSDCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigBSDCfgParser::hwaddr_line() {
	Tracer traceInOut(this, "hwaddr_line");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(LLADDR);
		addr = LT(1);
		match(MAC_ADDRESS);
#line 181 "ifconfig_bsd.g"
		
		importer->HwAddressForCurrentInterface(addr->getText());
		
#line 164 "IfconfigBSDCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigBSDCfgParser::inet_address() {
	Tracer traceInOut(this, "inet_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  netm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  bcast = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 190 "ifconfig_bsd.g"
	AddressSpec as;
#line 179 "IfconfigBSDCfgParser.cpp"
	
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
#line 193 "ifconfig_bsd.g"
		
		as.at = AddressSpec::INTERFACE_CONFIGURATION;
		as.address = addr->getText();
		as.netmask = netm->getText();
		as.broadcast = bcast->getText();
		importer->inetConfigurationForCurrentInterface(as);
		
#line 199 "IfconfigBSDCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigBSDCfgParser::inet6_address() {
	Tracer traceInOut(this, "inet6_address");
	ANTLR_USE_NAMESPACE(antlr)RefToken  addr = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  netm = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 205 "ifconfig_bsd.g"
	AddressSpec as;
#line 213 "IfconfigBSDCfgParser.cpp"
	
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
#line 208 "ifconfig_bsd.g"
		
		as.at = AddressSpec::INTERFACE_CONFIGURATION;
		as.address = addr->getText();
		as.netmask = netm->getText();
		importer->inet6ConfigurationForCurrentInterface(as);
		
#line 233 "IfconfigBSDCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigBSDCfgParser::groups() {
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

void IfconfigBSDCfgParser::unknown_line() {
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
#line 152 "ifconfig_bsd.g"
		
		consumeUntil(NEWLINE);
		
#line 292 "IfconfigBSDCfgParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
}

void IfconfigBSDCfgParser::groups_list() {
	Tracer traceInOut(this, "groups_list");
	
	try {      // for error handling
		match(WORD);
#line 224 "ifconfig_bsd.g"
		importer->addGroupToCurrentInterface(LT(0)->getText());
#line 307 "IfconfigBSDCfgParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == WORD)) {
				match(WORD);
#line 226 "ifconfig_bsd.g"
				importer->addGroupToCurrentInterface(LT(0)->getText());
#line 314 "IfconfigBSDCfgParser.cpp"
			}
			else {
				goto _loop14;
			}
			
		}
		_loop14:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_2);
	}
}

void IfconfigBSDCfgParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* IfconfigBSDCfgParser::tokenNames[] = {
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
	"COLON",
	"\"flags\"",
	"EQUAL",
	"INT_CONST",
	"LLADDR",
	"MAC_ADDRESS",
	"\"inet\"",
	"IPV4",
	"\"netmask\"",
	"HEX_CONST",
	"\"broadcast\"",
	"\"inet6\"",
	"IPV6",
	"PERCENT",
	"\"prefixlen\"",
	"\"scopeid\"",
	"\"groups\"",
	"\"Link\"",
	"\"Global\"",
	"\"Host\"",
	"\"addr\"",
	"\"Bcast\"",
	"\"P-t-P\"",
	"\"Mask\"",
	"\"Scope\"",
	"\"mtu\"",
	"\"encap\"",
	"\"Loopback\"",
	"\"UP\"",
	"\"BROADCAST\"",
	"\"POINTOPOINT\"",
	"\"LOOPBACK\"",
	"\"NOARP\"",
	"\"RUNNING\"",
	"\"RX\"",
	"\"TX\"",
	"\"collisions\"",
	"\"Interrupt\"",
	"\"HWaddr\"",
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
	"SLASH",
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

const unsigned long IfconfigBSDCfgParser::_tokenSet_0_data_[] = { 138577906UL, 0UL, 0UL, 0UL };
// EOF NEWLINE DOUBLE_NEWLINE LINE_COMMENT "priority" "media" "status" 
// WORD LLADDR "inet" "inet6" "groups" 
const ANTLR_USE_NAMESPACE(antlr)BitSet IfconfigBSDCfgParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long IfconfigBSDCfgParser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet IfconfigBSDCfgParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long IfconfigBSDCfgParser::_tokenSet_2_data_[] = { 16UL, 0UL, 0UL, 0UL };
// NEWLINE 
const ANTLR_USE_NAMESPACE(antlr)BitSet IfconfigBSDCfgParser::_tokenSet_2(_tokenSet_2_data_,4);



/* $ANTLR 2.7.7 (20090306): "pix.g" -> "PIXCfgLexer.cpp"$ */
#line 42 "pix.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "PIXCfgLexer.cpp"
#include "PIXCfgLexer.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>

#line 48 "pix.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#line 25 "PIXCfgLexer.cpp"
#line 1 "pix.g"
#line 27 "PIXCfgLexer.cpp"
PIXCfgLexer::PIXCfgLexer(ANTLR_USE_NAMESPACE(std)istream& in)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(new ANTLR_USE_NAMESPACE(antlr)CharBuffer(in),true)
{
	initLiterals();
}

PIXCfgLexer::PIXCfgLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(ib,true)
{
	initLiterals();
}

PIXCfgLexer::PIXCfgLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(state,true)
{
	initLiterals();
}

void PIXCfgLexer::initLiterals()
{
	literals["full"] = 119;
	literals["parameter-problem"] = 85;
	literals["port-object"] = 53;
	literals["notifications"] = 105;
	literals["duplex"] = 125;
	literals["no"] = 55;
	literals["static"] = 147;
	literals["esp"] = 16;
	literals["time-range"] = 111;
	literals["gre"] = 17;
	literals["timestamp-request"] = 92;
	literals["echo"] = 72;
	literals["speed"] = 124;
	literals["warnings"] = 106;
	literals["timeout"] = 8;
	literals["eigrp"] = 15;
	literals["icmp-type"] = 50;
	literals["permit"] = 65;
	literals["remark"] = 141;
	literals["network"] = 29;
	literals["igmp"] = 18;
	literals["range"] = 34;
	literals["destination"] = 43;
	literals["setroute"] = 152;
	literals["vlan"] = 123;
	literals["debugging"] = 101;
	literals["controller"] = 112;
	literals["interface"] = 95;
	literals["dhcp"] = 137;
	literals["aui"] = 116;
	literals["Version"] = 59;
	literals["auto"] = 117;
	literals["subnet"] = 35;
	literals["time-exceeded"] = 90;
	literals["outside"] = 113;
	literals["shutdown"] = 135;
	literals["group-object"] = 45;
	literals["eq"] = 68;
	literals["fragments"] = 110;
	literals["unreachable"] = 94;
	literals["delay"] = 128;
	literals["ip"] = 6;
	literals["security-level"] = 134;
	literals["mobile-redirect"] = 84;
	literals["ospf"] = 23;
	literals["name"] = 10;
	literals["errors"] = 103;
	literals["mask-request"] = 83;
	literals["PIX"] = 57;
	literals["any"] = 96;
	literals["ASA"] = 58;
	literals["pptp"] = 26;
	literals["redirect"] = 86;
	literals["forward"] = 127;
	literals["description"] = 32;
	literals["timestamp-reply"] = 91;
	literals["alerts"] = 99;
	literals["lt"] = 70;
	literals["bnc"] = 118;
	literals["global"] = 146;
	literals["nos"] = 22;
	literals["extended"] = 64;
	literals["certificate"] = 56;
	literals["service"] = 36;
	literals["telnet"] = 75;
	literals["udp"] = 41;
	literals["hold-time"] = 129;
	literals["baseT"] = 120;
	literals["ipinip"] = 20;
	literals["standby"] = 138;
	literals["crypto"] = 54;
	literals["pim"] = 25;
	literals["secondary"] = 151;
	literals["emergencies"] = 102;
	literals["disable"] = 107;
	literals["mask-reply"] = 82;
	literals["tcp"] = 40;
	literals["tcp-udp"] = 51;
	literals["source"] = 42;
	literals["names"] = 9;
	literals["icmp"] = 37;
	literals["log"] = 97;
	literals["snp"] = 27;
	literals["mac-address"] = 131;
	literals["established"] = 76;
	literals["deny"] = 66;
	literals["information-request"] = 81;
	literals["ssh"] = 74;
	literals["protocol-object"] = 48;
	literals["gt"] = 69;
	literals["ah"] = 14;
	literals["interval"] = 109;
	literals["ddns"] = 126;
	literals["ipv6"] = 130;
	literals["rip"] = 73;
	literals["baseTX"] = 121;
	literals["access-group"] = 142;
	literals["critical"] = 100;
	literals["standard"] = 67;
	literals["quit"] = 5;
	literals["community-list"] = 7;
	literals["network-object"] = 46;
	literals["hostname"] = 61;
	literals["information-reply"] = 80;
	literals["icmp6"] = 39;
	literals["switchport"] = 139;
	literals["ipsec"] = 21;
	literals["conversion-error"] = 78;
	literals["host"] = 33;
	literals["echo-reply"] = 79;
	literals["nameif"] = 122;
	literals["pcp"] = 24;
	literals["service-object"] = 52;
	literals["nat"] = 30;
	literals["access-list"] = 63;
	literals["informational"] = 104;
	literals["igrp"] = 19;
	literals["traceroute"] = 93;
	literals["address"] = 136;
	literals["log-input"] = 98;
	literals["router-advertisement"] = 87;
	literals["router-solicitation"] = 88;
	literals["access"] = 140;
	literals["icmp-object"] = 49;
	literals["source-quench"] = 89;
	literals["protocol"] = 47;
	literals["inactive"] = 108;
	literals["multicast"] = 132;
	literals["exit"] = 115;
	literals["neq"] = 71;
	literals["alternate-address"] = 77;
}

ANTLR_USE_NAMESPACE(antlr)RefToken PIXCfgLexer::nextToken()
{
	ANTLR_USE_NAMESPACE(antlr)RefToken theRetToken;
	for (;;) {
		ANTLR_USE_NAMESPACE(antlr)RefToken theRetToken;
		int _ttype = ANTLR_USE_NAMESPACE(antlr)Token::INVALID_TYPE;
		resetText();
		try {   // for lexical and char stream error handling
			switch ( LA(1)) {
			case 0xa /* '\n' */ :
			case 0xd /* '\r' */ :
			{
				mNEWLINE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x24 /* '$' */ :
			case 0x30 /* '0' */ :
			case 0x31 /* '1' */ :
			case 0x32 /* '2' */ :
			case 0x33 /* '3' */ :
			case 0x34 /* '4' */ :
			case 0x35 /* '5' */ :
			case 0x36 /* '6' */ :
			case 0x37 /* '7' */ :
			case 0x38 /* '8' */ :
			case 0x39 /* '9' */ :
			case 0x41 /* 'A' */ :
			case 0x42 /* 'B' */ :
			case 0x43 /* 'C' */ :
			case 0x44 /* 'D' */ :
			case 0x45 /* 'E' */ :
			case 0x46 /* 'F' */ :
			case 0x47 /* 'G' */ :
			case 0x48 /* 'H' */ :
			case 0x49 /* 'I' */ :
			case 0x4a /* 'J' */ :
			case 0x4b /* 'K' */ :
			case 0x4c /* 'L' */ :
			case 0x4d /* 'M' */ :
			case 0x4e /* 'N' */ :
			case 0x4f /* 'O' */ :
			case 0x50 /* 'P' */ :
			case 0x51 /* 'Q' */ :
			case 0x52 /* 'R' */ :
			case 0x53 /* 'S' */ :
			case 0x54 /* 'T' */ :
			case 0x55 /* 'U' */ :
			case 0x56 /* 'V' */ :
			case 0x57 /* 'W' */ :
			case 0x58 /* 'X' */ :
			case 0x59 /* 'Y' */ :
			case 0x5a /* 'Z' */ :
			case 0x61 /* 'a' */ :
			case 0x62 /* 'b' */ :
			case 0x63 /* 'c' */ :
			case 0x64 /* 'd' */ :
			case 0x65 /* 'e' */ :
			case 0x66 /* 'f' */ :
			case 0x67 /* 'g' */ :
			case 0x68 /* 'h' */ :
			case 0x69 /* 'i' */ :
			case 0x6a /* 'j' */ :
			case 0x6b /* 'k' */ :
			case 0x6c /* 'l' */ :
			case 0x6d /* 'm' */ :
			case 0x6e /* 'n' */ :
			case 0x6f /* 'o' */ :
			case 0x70 /* 'p' */ :
			case 0x71 /* 'q' */ :
			case 0x72 /* 'r' */ :
			case 0x73 /* 's' */ :
			case 0x74 /* 't' */ :
			case 0x75 /* 'u' */ :
			case 0x76 /* 'v' */ :
			case 0x77 /* 'w' */ :
			case 0x78 /* 'x' */ :
			case 0x79 /* 'y' */ :
			case 0x7a /* 'z' */ :
			{
				mNUMBER_ADDRESS_OR_WORD(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2e /* '.' */ :
			{
				mDOT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x22 /* '\"' */ :
			{
				mSTRING(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7c /* '|' */ :
			{
				mPIPE_CHAR(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x23 /* '#' */ :
			{
				mNUMBER_SIGN(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x25 /* '%' */ :
			{
				mPERCENT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x26 /* '&' */ :
			{
				mAMPERSAND(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x27 /* '\'' */ :
			{
				mAPOSTROPHE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x28 /* '(' */ :
			{
				mOPENING_PAREN(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x29 /* ')' */ :
			{
				mCLOSING_PAREN(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2a /* '*' */ :
			{
				mSTAR(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2b /* '+' */ :
			{
				mPLUS(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2c /* ',' */ :
			{
				mCOMMA(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2d /* '-' */ :
			{
				mMINUS(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2f /* '/' */ :
			{
				mSLASH(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3b /* ';' */ :
			{
				mSEMICOLON(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3c /* '<' */ :
			{
				mLESS_THAN(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3d /* '=' */ :
			{
				mEQUALS(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3e /* '>' */ :
			{
				mGREATER_THAN(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3f /* '?' */ :
			{
				mQUESTION(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x40 /* '@' */ :
			{
				mCOMMERCIAL_AT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5b /* '[' */ :
			{
				mOPENING_SQUARE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5d /* ']' */ :
			{
				mCLOSING_SQUARE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5e /* '^' */ :
			{
				mCARET(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5f /* '_' */ :
			{
				mUNDERLINE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7b /* '{' */ :
			{
				mOPENING_BRACE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7d /* '}' */ :
			{
				mCLOSING_BRACE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7e /* '~' */ :
			{
				mTILDE(true);
				theRetToken=_returnToken;
				break;
			}
			default:
				if ((LA(1) == 0x21 /* '!' */ ) && ((LA(2) >= 0x3 /* '\3' */  && LA(2) <= 0xff))) {
					mLINE_COMMENT(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x3a /* ':' */ ) && ((LA(2) >= 0x3 /* '\3' */  && LA(2) <= 0xff))) {
					mCOLON_COMMENT(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x3a /* ':' */ ) && (true)) {
					mCOLON(true);
					theRetToken=_returnToken;
				}
				else if ((_tokenSet_0.member(LA(1)))) {
					mWhitespace(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x21 /* '!' */ ) && (true)) {
					mEXLAMATION(true);
					theRetToken=_returnToken;
				}
			else {
				if (LA(1)==EOF_CHAR)
				{
					uponEOF();
					_returnToken = makeToken(ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE);
				}
				else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			}
			if ( !_returnToken )
				goto tryAgain; // found SKIP token

			_ttype = _returnToken->getType();
			_ttype = testLiteralsTable(_ttype);
			_returnToken->setType(_ttype);
			return _returnToken;
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& e) {
				throw ANTLR_USE_NAMESPACE(antlr)TokenStreamRecognitionException(e);
		}
		catch (ANTLR_USE_NAMESPACE(antlr)CharStreamIOException& csie) {
			throw ANTLR_USE_NAMESPACE(antlr)TokenStreamIOException(csie.io);
		}
		catch (ANTLR_USE_NAMESPACE(antlr)CharStreamException& cse) {
			throw ANTLR_USE_NAMESPACE(antlr)TokenStreamException(cse.getMessage());
		}
tryAgain:;
	}
}

void PIXCfgLexer::mLINE_COMMENT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = LINE_COMMENT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("!");
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			{
			match(_tokenSet_1);
			}
		}
		else {
			goto _loop263;
		}
		
	}
	_loop263:;
	} // ( ... )*
	mNEWLINE(false);
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mNEWLINE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NEWLINE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	{
	if ((LA(1) == 0xd /* '\r' */ ) && (LA(2) == 0xa /* '\n' */ )) {
		match("\r\n");
	}
	else if ((LA(1) == 0xd /* '\r' */ ) && (true)) {
		match('\r' /* charlit */ );
	}
	else if ((LA(1) == 0xa /* '\n' */ )) {
		match('\n' /* charlit */ );
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	
	}
	if ( inputState->guessing==0 ) {
#line 2228 "pix.g"
		newline();
#line 530 "PIXCfgLexer.cpp"
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mCOLON_COMMENT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = COLON_COMMENT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	mCOLON(false);
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			{
			match(_tokenSet_1);
			}
		}
		else {
			goto _loop267;
		}
		
	}
	_loop267:;
	} // ( ... )*
	mNEWLINE(false);
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mCOLON(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = COLON;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match(':' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mWhitespace(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Whitespace;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x3 /* '\3' */ :
	case 0x4 /* '\4' */ :
	case 0x5 /* '\5' */ :
	case 0x6 /* '\6' */ :
	case 0x7 /* '\7' */ :
	case 0x8 /* '\10' */ :
	{
		matchRange('\3','\10');
		break;
	}
	case 0x9 /* '\t' */ :
	{
		match('\t' /* charlit */ );
		break;
	}
	case 0xb /* '\13' */ :
	{
		match('\13' /* charlit */ );
		break;
	}
	case 0xc /* '\14' */ :
	{
		match('\14' /* charlit */ );
		break;
	}
	case 0xe /* '\16' */ :
	case 0xf /* '\17' */ :
	case 0x10 /* '\20' */ :
	case 0x11 /* '\21' */ :
	case 0x12 /* '\22' */ :
	case 0x13 /* '\23' */ :
	case 0x14 /* '\24' */ :
	case 0x15 /* '\25' */ :
	case 0x16 /* '\26' */ :
	case 0x17 /* '\27' */ :
	case 0x18 /* '\30' */ :
	case 0x19 /* '\31' */ :
	case 0x1a /* '\32' */ :
	case 0x1b /* '\33' */ :
	case 0x1c /* '\34' */ :
	case 0x1d /* '\35' */ :
	case 0x1e /* '\36' */ :
	case 0x1f /* '\37' */ :
	{
		matchRange('\16','\37');
		break;
	}
	case 0x20 /* ' ' */ :
	{
		match(' ' /* charlit */ );
		break;
	}
	default:
		if (((LA(1) >= 0x7f && LA(1) <= 0xff))) {
			matchRange('\177',static_cast<unsigned char>('\377'));
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	}
	if ( inputState->guessing==0 ) {
#line 2223 "pix.g"
		_ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP;
#line 654 "PIXCfgLexer.cpp"
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mINT_CONST(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = INT_CONST;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mHEX_CONST(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = HEX_CONST;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mNUMBER(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NUMBER;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mNEG_INT_CONST(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NEG_INT_CONST;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mDIGIT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = DIGIT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	matchRange('0','9');
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mHEXDIGIT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = HEXDIGIT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	matchRange('a','f');
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mOBJECT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = OBJECT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mOBJECT_GROUP(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = OBJECT_GROUP;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mNUMBER_ADDRESS_OR_WORD(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NUMBER_ADDRESS_OR_WORD;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	{
	bool synPredMatched328 = false;
	if (((LA(1) == 0x6f /* 'o' */ ) && (LA(2) == 0x62 /* 'b' */ ) && (LA(3) == 0x6a /* 'j' */ ))) {
		int _m328 = mark();
		synPredMatched328 = true;
		inputState->guessing++;
		try {
			{
			match("obj");
			match("ect");
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched328 = false;
		}
		rewind(_m328);
		inputState->guessing--;
	}
	if ( synPredMatched328 ) {
		{
		match("object");
		{
		if ((LA(1) == 0x2d /* '-' */ )) {
			{
			match("-gr");
			match("oup");
			}
			if ( inputState->guessing==0 ) {
#line 2280 "pix.g"
				_ttype = OBJECT_GROUP;
#line 805 "PIXCfgLexer.cpp"
			}
		}
		else {
			match("");
			if ( inputState->guessing==0 ) {
#line 2282 "pix.g"
				_ttype = OBJECT;
#line 813 "PIXCfgLexer.cpp"
			}
		}
		
		}
		}
	}
	else {
		bool synPredMatched318 = false;
		if (((_tokenSet_2.member(LA(1))) && (_tokenSet_3.member(LA(2))) && (true))) {
			int _m318 = mark();
			synPredMatched318 = true;
			inputState->guessing++;
			try {
				{
				{ // ( ... )+
				int _cnt317=0;
				for (;;) {
					switch ( LA(1)) {
					case 0x61 /* 'a' */ :
					case 0x62 /* 'b' */ :
					case 0x63 /* 'c' */ :
					case 0x64 /* 'd' */ :
					case 0x65 /* 'e' */ :
					case 0x66 /* 'f' */ :
					{
						matchRange('a','f');
						break;
					}
					case 0x30 /* '0' */ :
					case 0x31 /* '1' */ :
					case 0x32 /* '2' */ :
					case 0x33 /* '3' */ :
					case 0x34 /* '4' */ :
					case 0x35 /* '5' */ :
					case 0x36 /* '6' */ :
					case 0x37 /* '7' */ :
					case 0x38 /* '8' */ :
					case 0x39 /* '9' */ :
					{
						matchRange('0','9');
						break;
					}
					default:
					{
						if ( _cnt317>=1 ) { goto _loop317; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					}
					_cnt317++;
				}
				_loop317:;
				}  // ( ... )+
				mCOLON(false);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched318 = false;
			}
			rewind(_m318);
			inputState->guessing--;
		}
		if ( synPredMatched318 ) {
			{
			{
			{ // ( ... )+
			int _cnt322=0;
			for (;;) {
				switch ( LA(1)) {
				case 0x61 /* 'a' */ :
				case 0x62 /* 'b' */ :
				case 0x63 /* 'c' */ :
				case 0x64 /* 'd' */ :
				case 0x65 /* 'e' */ :
				case 0x66 /* 'f' */ :
				{
					matchRange('a','f');
					break;
				}
				case 0x30 /* '0' */ :
				case 0x31 /* '1' */ :
				case 0x32 /* '2' */ :
				case 0x33 /* '3' */ :
				case 0x34 /* '4' */ :
				case 0x35 /* '5' */ :
				case 0x36 /* '6' */ :
				case 0x37 /* '7' */ :
				case 0x38 /* '8' */ :
				case 0x39 /* '9' */ :
				{
					matchRange('0','9');
					break;
				}
				default:
				{
					if ( _cnt322>=1 ) { goto _loop322; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				}
				_cnt322++;
			}
			_loop322:;
			}  // ( ... )+
			{ // ( ... )+
			int _cnt326=0;
			for (;;) {
				if ((LA(1) == 0x3a /* ':' */ )) {
					mCOLON(false);
					{ // ( ... )*
					for (;;) {
						switch ( LA(1)) {
						case 0x61 /* 'a' */ :
						case 0x62 /* 'b' */ :
						case 0x63 /* 'c' */ :
						case 0x64 /* 'd' */ :
						case 0x65 /* 'e' */ :
						case 0x66 /* 'f' */ :
						{
							matchRange('a','f');
							break;
						}
						case 0x30 /* '0' */ :
						case 0x31 /* '1' */ :
						case 0x32 /* '2' */ :
						case 0x33 /* '3' */ :
						case 0x34 /* '4' */ :
						case 0x35 /* '5' */ :
						case 0x36 /* '6' */ :
						case 0x37 /* '7' */ :
						case 0x38 /* '8' */ :
						case 0x39 /* '9' */ :
						{
							matchRange('0','9');
							break;
						}
						default:
						{
							goto _loop325;
						}
						}
					}
					_loop325:;
					} // ( ... )*
				}
				else {
					if ( _cnt326>=1 ) { goto _loop326; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt326++;
			}
			_loop326:;
			}  // ( ... )+
			}
			if ( inputState->guessing==0 ) {
#line 2273 "pix.g"
				_ttype = IPV6;
#line 967 "PIXCfgLexer.cpp"
			}
			}
		}
		else {
			bool synPredMatched283 = false;
			if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (true) && (true))) {
				int _m283 = mark();
				synPredMatched283 = true;
				inputState->guessing++;
				try {
					{
					mDIGIT(false);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched283 = false;
				}
				rewind(_m283);
				inputState->guessing--;
			}
			if ( synPredMatched283 ) {
				{
				bool synPredMatched292 = false;
				if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_4.member(LA(2))) && (_tokenSet_4.member(LA(3))))) {
					int _m292 = mark();
					synPredMatched292 = true;
					inputState->guessing++;
					try {
						{
						{ // ( ... )+
						int _cnt287=0;
						for (;;) {
							if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
								mDIGIT(false);
							}
							else {
								if ( _cnt287>=1 ) { goto _loop287; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt287++;
						}
						_loop287:;
						}  // ( ... )+
						mDOT(false);
						{ // ( ... )+
						int _cnt289=0;
						for (;;) {
							if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
								mDIGIT(false);
							}
							else {
								if ( _cnt289>=1 ) { goto _loop289; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt289++;
						}
						_loop289:;
						}  // ( ... )+
						mDOT(false);
						{ // ( ... )+
						int _cnt291=0;
						for (;;) {
							if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
								mDIGIT(false);
							}
							else {
								if ( _cnt291>=1 ) { goto _loop291; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt291++;
						}
						_loop291:;
						}  // ( ... )+
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched292 = false;
					}
					rewind(_m292);
					inputState->guessing--;
				}
				if ( synPredMatched292 ) {
					{
					{ // ( ... )+
					int _cnt295=0;
					for (;;) {
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
							mDIGIT(false);
						}
						else {
							if ( _cnt295>=1 ) { goto _loop295; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt295++;
					}
					_loop295:;
					}  // ( ... )+
					mDOT(false);
					{ // ( ... )+
					int _cnt297=0;
					for (;;) {
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
							mDIGIT(false);
						}
						else {
							if ( _cnt297>=1 ) { goto _loop297; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt297++;
					}
					_loop297:;
					}  // ( ... )+
					mDOT(false);
					{ // ( ... )+
					int _cnt299=0;
					for (;;) {
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
							mDIGIT(false);
						}
						else {
							if ( _cnt299>=1 ) { goto _loop299; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt299++;
					}
					_loop299:;
					}  // ( ... )+
					mDOT(false);
					{ // ( ... )+
					int _cnt301=0;
					for (;;) {
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
							mDIGIT(false);
						}
						else {
							if ( _cnt301>=1 ) { goto _loop301; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt301++;
					}
					_loop301:;
					}  // ( ... )+
					}
					if ( inputState->guessing==0 ) {
#line 2261 "pix.g"
						_ttype = IPV4;
#line 1114 "PIXCfgLexer.cpp"
					}
				}
				else {
					bool synPredMatched307 = false;
					if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_4.member(LA(2))) && (_tokenSet_4.member(LA(3))))) {
						int _m307 = mark();
						synPredMatched307 = true;
						inputState->guessing++;
						try {
							{
							{ // ( ... )+
							int _cnt304=0;
							for (;;) {
								if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
									mDIGIT(false);
								}
								else {
									if ( _cnt304>=1 ) { goto _loop304; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt304++;
							}
							_loop304:;
							}  // ( ... )+
							mDOT(false);
							{ // ( ... )+
							int _cnt306=0;
							for (;;) {
								if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
									mDIGIT(false);
								}
								else {
									if ( _cnt306>=1 ) { goto _loop306; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt306++;
							}
							_loop306:;
							}  // ( ... )+
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched307 = false;
						}
						rewind(_m307);
						inputState->guessing--;
					}
					if ( synPredMatched307 ) {
						{
						{ // ( ... )+
						int _cnt310=0;
						for (;;) {
							if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
								mDIGIT(false);
							}
							else {
								if ( _cnt310>=1 ) { goto _loop310; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt310++;
						}
						_loop310:;
						}  // ( ... )+
						mDOT(false);
						{ // ( ... )+
						int _cnt312=0;
						for (;;) {
							if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
								mDIGIT(false);
							}
							else {
								if ( _cnt312>=1 ) { goto _loop312; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt312++;
						}
						_loop312:;
						}  // ( ... )+
						}
						if ( inputState->guessing==0 ) {
#line 2264 "pix.g"
							_ttype = NUMBER;
#line 1197 "PIXCfgLexer.cpp"
						}
					}
					else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (true) && (true)) {
						{ // ( ... )+
						int _cnt314=0;
						for (;;) {
							if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
								mDIGIT(false);
							}
							else {
								if ( _cnt314>=1 ) { goto _loop314; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt314++;
						}
						_loop314:;
						}  // ( ... )+
						if ( inputState->guessing==0 ) {
#line 2266 "pix.g"
							_ttype = INT_CONST;
#line 1218 "PIXCfgLexer.cpp"
						}
					}
				else {
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
				}
				}
				}
			}
			else if ((_tokenSet_5.member(LA(1))) && (true) && (true)) {
				{
				switch ( LA(1)) {
				case 0x61 /* 'a' */ :
				case 0x62 /* 'b' */ :
				case 0x63 /* 'c' */ :
				case 0x64 /* 'd' */ :
				case 0x65 /* 'e' */ :
				case 0x66 /* 'f' */ :
				case 0x67 /* 'g' */ :
				case 0x68 /* 'h' */ :
				case 0x69 /* 'i' */ :
				case 0x6a /* 'j' */ :
				case 0x6b /* 'k' */ :
				case 0x6c /* 'l' */ :
				case 0x6d /* 'm' */ :
				case 0x6e /* 'n' */ :
				case 0x6f /* 'o' */ :
				case 0x70 /* 'p' */ :
				case 0x71 /* 'q' */ :
				case 0x72 /* 'r' */ :
				case 0x73 /* 's' */ :
				case 0x74 /* 't' */ :
				case 0x75 /* 'u' */ :
				case 0x76 /* 'v' */ :
				case 0x77 /* 'w' */ :
				case 0x78 /* 'x' */ :
				case 0x79 /* 'y' */ :
				case 0x7a /* 'z' */ :
				{
					matchRange('a','z');
					break;
				}
				case 0x41 /* 'A' */ :
				case 0x42 /* 'B' */ :
				case 0x43 /* 'C' */ :
				case 0x44 /* 'D' */ :
				case 0x45 /* 'E' */ :
				case 0x46 /* 'F' */ :
				case 0x47 /* 'G' */ :
				case 0x48 /* 'H' */ :
				case 0x49 /* 'I' */ :
				case 0x4a /* 'J' */ :
				case 0x4b /* 'K' */ :
				case 0x4c /* 'L' */ :
				case 0x4d /* 'M' */ :
				case 0x4e /* 'N' */ :
				case 0x4f /* 'O' */ :
				case 0x50 /* 'P' */ :
				case 0x51 /* 'Q' */ :
				case 0x52 /* 'R' */ :
				case 0x53 /* 'S' */ :
				case 0x54 /* 'T' */ :
				case 0x55 /* 'U' */ :
				case 0x56 /* 'V' */ :
				case 0x57 /* 'W' */ :
				case 0x58 /* 'X' */ :
				case 0x59 /* 'Y' */ :
				case 0x5a /* 'Z' */ :
				{
					matchRange('A','Z');
					break;
				}
				case 0x24 /* '$' */ :
				{
					match('$' /* charlit */ );
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
				}
				}
				}
				{ // ( ... )*
				for (;;) {
					switch ( LA(1)) {
					case 0x21 /* '!' */ :
					case 0x22 /* '\"' */ :
					case 0x23 /* '#' */ :
					case 0x24 /* '$' */ :
					case 0x25 /* '%' */ :
					case 0x26 /* '&' */ :
					case 0x27 /* '\'' */ :
					{
						matchRange('!','\'');
						break;
					}
					case 0x2a /* '*' */ :
					case 0x2b /* '+' */ :
					case 0x2c /* ',' */ :
					case 0x2d /* '-' */ :
					case 0x2e /* '.' */ :
					case 0x2f /* '/' */ :
					{
						matchRange('*','/');
						break;
					}
					case 0x30 /* '0' */ :
					case 0x31 /* '1' */ :
					case 0x32 /* '2' */ :
					case 0x33 /* '3' */ :
					case 0x34 /* '4' */ :
					case 0x35 /* '5' */ :
					case 0x36 /* '6' */ :
					case 0x37 /* '7' */ :
					case 0x38 /* '8' */ :
					case 0x39 /* '9' */ :
					{
						matchRange('0','9');
						break;
					}
					case 0x3a /* ':' */ :
					{
						match(':' /* charlit */ );
						break;
					}
					case 0x3b /* ';' */ :
					{
						match(';' /* charlit */ );
						break;
					}
					case 0x3c /* '<' */ :
					{
						match('<' /* charlit */ );
						break;
					}
					case 0x3d /* '=' */ :
					{
						match('=' /* charlit */ );
						break;
					}
					case 0x3e /* '>' */ :
					{
						match('>' /* charlit */ );
						break;
					}
					case 0x3f /* '?' */ :
					{
						match('?' /* charlit */ );
						break;
					}
					case 0x40 /* '@' */ :
					{
						match('@' /* charlit */ );
						break;
					}
					case 0x41 /* 'A' */ :
					case 0x42 /* 'B' */ :
					case 0x43 /* 'C' */ :
					case 0x44 /* 'D' */ :
					case 0x45 /* 'E' */ :
					case 0x46 /* 'F' */ :
					case 0x47 /* 'G' */ :
					case 0x48 /* 'H' */ :
					case 0x49 /* 'I' */ :
					case 0x4a /* 'J' */ :
					case 0x4b /* 'K' */ :
					case 0x4c /* 'L' */ :
					case 0x4d /* 'M' */ :
					case 0x4e /* 'N' */ :
					case 0x4f /* 'O' */ :
					case 0x50 /* 'P' */ :
					case 0x51 /* 'Q' */ :
					case 0x52 /* 'R' */ :
					case 0x53 /* 'S' */ :
					case 0x54 /* 'T' */ :
					case 0x55 /* 'U' */ :
					case 0x56 /* 'V' */ :
					case 0x57 /* 'W' */ :
					case 0x58 /* 'X' */ :
					case 0x59 /* 'Y' */ :
					case 0x5a /* 'Z' */ :
					{
						matchRange('A','Z');
						break;
					}
					case 0x5c /* '\\' */ :
					{
						match('\\' /* charlit */ );
						break;
					}
					case 0x5e /* '^' */ :
					{
						match('^' /* charlit */ );
						break;
					}
					case 0x5f /* '_' */ :
					{
						match('_' /* charlit */ );
						break;
					}
					case 0x60 /* '`' */ :
					{
						match('`' /* charlit */ );
						break;
					}
					case 0x61 /* 'a' */ :
					case 0x62 /* 'b' */ :
					case 0x63 /* 'c' */ :
					case 0x64 /* 'd' */ :
					case 0x65 /* 'e' */ :
					case 0x66 /* 'f' */ :
					case 0x67 /* 'g' */ :
					case 0x68 /* 'h' */ :
					case 0x69 /* 'i' */ :
					case 0x6a /* 'j' */ :
					case 0x6b /* 'k' */ :
					case 0x6c /* 'l' */ :
					case 0x6d /* 'm' */ :
					case 0x6e /* 'n' */ :
					case 0x6f /* 'o' */ :
					case 0x70 /* 'p' */ :
					case 0x71 /* 'q' */ :
					case 0x72 /* 'r' */ :
					case 0x73 /* 's' */ :
					case 0x74 /* 't' */ :
					case 0x75 /* 'u' */ :
					case 0x76 /* 'v' */ :
					case 0x77 /* 'w' */ :
					case 0x78 /* 'x' */ :
					case 0x79 /* 'y' */ :
					case 0x7a /* 'z' */ :
					{
						matchRange('a','z');
						break;
					}
					default:
					{
						goto _loop334;
					}
					}
				}
				_loop334:;
				} // ( ... )*
				if ( inputState->guessing==0 ) {
#line 2289 "pix.g"
					_ttype = WORD;
#line 1465 "PIXCfgLexer.cpp"
				}
			}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mDOT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = DOT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('.' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mSTRING(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = STRING;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('\"' /* charlit */ );
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_6.member(LA(1)))) {
			matchNot('\"' /* charlit */ );
		}
		else {
			goto _loop337;
		}
		
	}
	_loop337:;
	} // ( ... )*
	match('\"' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mPIPE_CHAR(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = PIPE_CHAR;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('|' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mNUMBER_SIGN(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NUMBER_SIGN;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('#' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mPERCENT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = PERCENT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('%' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mAMPERSAND(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = AMPERSAND;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('&' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mAPOSTROPHE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = APOSTROPHE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('\'' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mOPENING_PAREN(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = OPENING_PAREN;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('(' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mCLOSING_PAREN(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = CLOSING_PAREN;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match(')' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mSTAR(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = STAR;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('*' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mPLUS(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = PLUS;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('+' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mCOMMA(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = COMMA;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match(',' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mMINUS(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = MINUS;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('-' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mSLASH(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = SLASH;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('/' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mSEMICOLON(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = SEMICOLON;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match(';' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mLESS_THAN(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = LESS_THAN;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('<' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mEQUALS(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = EQUALS;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('=' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mGREATER_THAN(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = GREATER_THAN;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('>' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mQUESTION(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = QUESTION;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('?' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mCOMMERCIAL_AT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = COMMERCIAL_AT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('@' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mOPENING_SQUARE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = OPENING_SQUARE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('[' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mCLOSING_SQUARE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = CLOSING_SQUARE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match(']' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mCARET(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = CARET;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('^' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mUNDERLINE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = UNDERLINE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('_' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mOPENING_BRACE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = OPENING_BRACE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('{' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mCLOSING_BRACE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = CLOSING_BRACE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('}' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mTILDE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TILDE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('~' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PIXCfgLexer::mEXLAMATION(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = EXLAMATION;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('!' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}


const unsigned long PIXCfgLexer::_tokenSet_0_data_[] = { 4294958072UL, 1UL, 0UL, 2147483648UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   0x7f 0x80 0x81 
// 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 
// 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 
// 0x9e 0x9f 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 
// 0xac 0xad 0xae 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgLexer::_tokenSet_0(_tokenSet_0_data_,16);
const unsigned long PIXCfgLexer::_tokenSet_1_data_[] = { 4294958072UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # $ % 
// & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G 
// H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g h 
// i j k l m n o p q r s t u v w x y z { | } ~ 0x7f 0x80 0x81 0x82 0x83 
// 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 0x90 0x91 
// 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 
// 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 
// 0xae 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgLexer::_tokenSet_1(_tokenSet_1_data_,16);
const unsigned long PIXCfgLexer::_tokenSet_2_data_[] = { 0UL, 67043328UL, 0UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 a b c d e f 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgLexer::_tokenSet_2(_tokenSet_2_data_,10);
const unsigned long PIXCfgLexer::_tokenSet_3_data_[] = { 0UL, 134152192UL, 0UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 : a b c d e f 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgLexer::_tokenSet_3(_tokenSet_3_data_,10);
const unsigned long PIXCfgLexer::_tokenSet_4_data_[] = { 0UL, 67059712UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgLexer::_tokenSet_4(_tokenSet_4_data_,10);
const unsigned long PIXCfgLexer::_tokenSet_5_data_[] = { 0UL, 16UL, 134217726UL, 134217726UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// $ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g 
// h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgLexer::_tokenSet_5(_tokenSet_5_data_,10);
const unsigned long PIXCfgLexer::_tokenSet_6_data_[] = { 4294967288UL, 4294967291UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! # $ 
// % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g 
// h i j k l m n o p q r s t u v w x y z { | } ~ 0x7f 0x80 0x81 0x82 0x83 
// 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 0x90 0x91 
// 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 
// 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 
// 0xae 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 
const ANTLR_USE_NAMESPACE(antlr)BitSet PIXCfgLexer::_tokenSet_6(_tokenSet_6_data_,16);


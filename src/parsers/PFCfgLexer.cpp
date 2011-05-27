/* $ANTLR 2.7.7 (20090306): "pf.g" -> "PFCfgLexer.cpp"$ */
#line 42 "pf.g"

    // gets inserted before the antlr generated includes in the cpp
    // file

#line 8 "PFCfgLexer.cpp"
#include "PFCfgLexer.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>

#line 48 "pf.g"

    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>

#line 25 "PFCfgLexer.cpp"
#line 1 "pf.g"
#line 27 "PFCfgLexer.cpp"
PFCfgLexer::PFCfgLexer(ANTLR_USE_NAMESPACE(std)istream& in)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(new ANTLR_USE_NAMESPACE(antlr)CharBuffer(in),true)
{
	initLiterals();
}

PFCfgLexer::PFCfgLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(ib,true)
{
	initLiterals();
}

PFCfgLexer::PFCfgLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(state,true)
{
	initLiterals();
}

void PFCfgLexer::initLiterals()
{
	literals["vrrp"] = 64;
	literals["critical"] = 103;
	literals["ospf"] = 62;
	literals["rdp"] = 56;
	literals["disable"] = 110;
	literals["scrub"] = 12;
	literals["ipsec"] = 92;
	literals["inet"] = 48;
	literals["pcp"] = 94;
	literals["emergencies"] = 105;
	literals["debugging"] = 104;
	literals["persist"] = 16;
	literals["snp"] = 98;
	literals["timeout"] = 35;
	literals["to"] = 45;
	literals["flags"] = 74;
	literals["isis"] = 66;
	literals["icmp6-type"] = 77;
	literals["const"] = 17;
	literals["pptp"] = 96;
	literals["pass"] = 36;
	literals["no"] = 80;
	literals["from"] = 67;
	literals["igrp"] = 91;
	literals["pim"] = 95;
	literals["tagged"] = 78;
	literals["rsvp"] = 57;
	literals["route-to"] = 72;
	literals["nos"] = 93;
	literals["quit"] = 88;
	literals["->"] = 112;
	literals["peer"] = 100;
	literals["icmp-type"] = 75;
	literals["exit"] = 87;
	literals["modulate"] = 82;
	literals["nat"] = 32;
	literals["range"] = 101;
	literals["urpf-failed"] = 68;
	literals["out"] = 39;
	literals["queue"] = 10;
	literals["gre"] = 58;
	literals["set"] = 11;
	literals["warnings"] = 109;
	literals["ah"] = 60;
	literals["host"] = 99;
	literals["interface"] = 89;
	literals["rip"] = 97;
	literals["icmp6"] = 90;
	literals["broadcast"] = 27;
	literals["notifications"] = 108;
	literals["file"] = 19;
	literals["network"] = 26;
	literals["synproxy"] = 83;
	literals["altq"] = 9;
	literals["any"] = 69;
	literals["esp"] = 59;
	literals["alerts"] = 102;
	literals["all"] = 43;
	literals["inet6"] = 49;
	literals["inactive"] = 111;
	literals["label"] = 85;
	literals["no-route"] = 70;
	literals["udp"] = 55;
	literals["reply-to"] = 73;
	literals["tag"] = 79;
	literals["port"] = 86;
	literals["code"] = 76;
	literals["ip"] = 51;
	literals["table"] = 13;
	literals["eigrp"] = 61;
	literals["errors"] = 106;
	literals["ipip"] = 63;
	literals["antispoof"] = 8;
	literals["binat"] = 33;
	literals["igmp"] = 53;
	literals["on"] = 47;
	literals["state"] = 84;
	literals["log"] = 40;
	literals["proto"] = 50;
	literals["rdr"] = 34;
	literals["informational"] = 107;
	literals["self"] = 28;
	literals["in"] = 38;
	literals["keep"] = 81;
	literals["block"] = 37;
	literals["l2tp"] = 65;
	literals["quick"] = 46;
	literals["user"] = 44;
	literals["icmp"] = 52;
	literals["tcp"] = 54;
}

ANTLR_USE_NAMESPACE(antlr)RefToken PFCfgLexer::nextToken()
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
			case 0x3a /* ':' */ :
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
			case 0x7c /* '|' */ :
			{
				mPIPE_CHAR(true);
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
			case 0x2e /* '.' */ :
			{
				mDOT(true);
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
			case 0x3d /* '=' */ :
			{
				mEQUAL(true);
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
			case 0x7e /* '~' */ :
			{
				mTILDE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x21 /* '!' */ :
			{
				mEXLAMATION(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3c /* '<' */ :
			{
				mLESS_THAN(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3e /* '>' */ :
			{
				mGREATER_THAN(true);
				theRetToken=_returnToken;
				break;
			}
			default:
				if ((LA(1) == 0x23 /* '#' */ ) && ((LA(2) >= 0x3 /* '\3' */  && LA(2) <= 0xff))) {
					mLINE_COMMENT(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x22 /* '\"' */ ) && ((LA(2) >= 0x3 /* '\3' */  && LA(2) <= 0xff))) {
					mSTRING(true);
					theRetToken=_returnToken;
				}
				else if ((_tokenSet_0.member(LA(1)))) {
					mWhitespace(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x23 /* '#' */ ) && (true)) {
					mNUMBER_SIGN(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x22 /* '\"' */ ) && (true)) {
					mDOUBLE_QUOTE(true);
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

void PFCfgLexer::mLINE_COMMENT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = LINE_COMMENT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match("#");
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			{
			match(_tokenSet_1);
			}
		}
		else {
			goto _loop156;
		}
		
	}
	_loop156:;
	} // ( ... )*
	mNEWLINE(false);
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mNEWLINE(bool _createToken) {
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
#line 1061 "pf.g"
		newline();
#line 483 "PFCfgLexer.cpp"
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mWhitespace(bool _createToken) {
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
#line 1056 "pf.g"
		_ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP;
#line 564 "PFCfgLexer.cpp"
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mINT_CONST(bool _createToken) {
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

void PFCfgLexer::mHEX_CONST(bool _createToken) {
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

void PFCfgLexer::mNUMBER(bool _createToken) {
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

void PFCfgLexer::mNEG_INT_CONST(bool _createToken) {
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

void PFCfgLexer::mCOLON(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = COLON;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mHEX_DIGIT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = HEX_DIGIT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	matchRange('0','9');
	matchRange('a','f');
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mDIGIT(bool _createToken) {
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

void PFCfgLexer::mNUM_3DIGIT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NUM_3DIGIT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	{
	matchRange('0','9');
	}
	{
	if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
		{
		matchRange('0','9');
		}
		{
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
			matchRange('0','9');
		}
		else {
		}
		
		}
	}
	else {
	}
	
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mNUM_HEX_4DIGIT(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NUM_HEX_4DIGIT;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	mHEX_DIGIT(false);
	{
	if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
		{
		mHEX_DIGIT(false);
		}
		{
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
			{
			mHEX_DIGIT(false);
			}
			{
			if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
				mHEX_DIGIT(false);
			}
			else {
			}
			
			}
		}
		else {
		}
		
		}
	}
	else {
	}
	
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mNUMBER_ADDRESS_OR_WORD(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = NUMBER_ADDRESS_OR_WORD;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	bool synPredMatched181 = false;
	if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_2.member(LA(2))) && (_tokenSet_2.member(LA(3))))) {
		int _m181 = mark();
		synPredMatched181 = true;
		inputState->guessing++;
		try {
			{
			mNUM_3DIGIT(false);
			match('.' /* charlit */ );
			mNUM_3DIGIT(false);
			match('.' /* charlit */ );
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched181 = false;
		}
		rewind(_m181);
		inputState->guessing--;
	}
	if ( synPredMatched181 ) {
		{
		mNUM_3DIGIT(false);
		match('.' /* charlit */ );
		mNUM_3DIGIT(false);
		match('.' /* charlit */ );
		mNUM_3DIGIT(false);
		match('.' /* charlit */ );
		mNUM_3DIGIT(false);
		}
		if ( inputState->guessing==0 ) {
#line 1098 "pf.g"
			_ttype = IPV4;
#line 781 "PFCfgLexer.cpp"
		}
	}
	else {
		bool synPredMatched188 = false;
		if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_2.member(LA(2))) && (_tokenSet_2.member(LA(3))))) {
			int _m188 = mark();
			synPredMatched188 = true;
			inputState->guessing++;
			try {
				{
				{ // ( ... )+
				int _cnt185=0;
				for (;;) {
					if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
						mDIGIT(false);
					}
					else {
						if ( _cnt185>=1 ) { goto _loop185; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt185++;
				}
				_loop185:;
				}  // ( ... )+
				match('.' /* charlit */ );
				{ // ( ... )+
				int _cnt187=0;
				for (;;) {
					if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
						mDIGIT(false);
					}
					else {
						if ( _cnt187>=1 ) { goto _loop187; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt187++;
				}
				_loop187:;
				}  // ( ... )+
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched188 = false;
			}
			rewind(_m188);
			inputState->guessing--;
		}
		if ( synPredMatched188 ) {
			{
			{ // ( ... )+
			int _cnt191=0;
			for (;;) {
				if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
					mDIGIT(false);
				}
				else {
					if ( _cnt191>=1 ) { goto _loop191; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt191++;
			}
			_loop191:;
			}  // ( ... )+
			match('.' /* charlit */ );
			{ // ( ... )+
			int _cnt193=0;
			for (;;) {
				if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
					mDIGIT(false);
				}
				else {
					if ( _cnt193>=1 ) { goto _loop193; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt193++;
			}
			_loop193:;
			}  // ( ... )+
			}
			if ( inputState->guessing==0 ) {
#line 1101 "pf.g"
				_ttype = NUMBER;
#line 864 "PFCfgLexer.cpp"
			}
		}
		else {
			bool synPredMatched212 = false;
			if (((LA(1) == 0x3a /* ':' */ ) && (LA(2) == 0x3a /* ':' */ ) && ((LA(3) >= 0x30 /* '0' */  && LA(3) <= 0x39 /* '9' */ )))) {
				int _m212 = mark();
				synPredMatched212 = true;
				inputState->guessing++;
				try {
					{
					match(':' /* charlit */ );
					match(':' /* charlit */ );
					mNUM_HEX_4DIGIT(false);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched212 = false;
				}
				rewind(_m212);
				inputState->guessing--;
			}
			if ( synPredMatched212 ) {
				match(':' /* charlit */ );
				match(':' /* charlit */ );
				mNUM_HEX_4DIGIT(false);
				{ // ( ... )*
				for (;;) {
					if ((LA(1) == 0x3a /* ':' */ )) {
						match(':' /* charlit */ );
						mNUM_HEX_4DIGIT(false);
					}
					else {
						goto _loop214;
					}
					
				}
				_loop214:;
				} // ( ... )*
				if ( inputState->guessing==0 ) {
#line 1124 "pf.g"
					_ttype = IPV6;
#line 906 "PFCfgLexer.cpp"
				}
			}
			else {
				bool synPredMatched197 = false;
				if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && ((LA(2) >= 0x61 /* 'a' */  && LA(2) <= 0x66 /* 'f' */ )))) {
					int _m197 = mark();
					synPredMatched197 = true;
					inputState->guessing++;
					try {
						{
						mNUM_HEX_4DIGIT(false);
						match(':' /* charlit */ );
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched197 = false;
					}
					rewind(_m197);
					inputState->guessing--;
				}
				if ( synPredMatched197 ) {
					{
					bool synPredMatched202 = false;
					if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && ((LA(2) >= 0x61 /* 'a' */  && LA(2) <= 0x66 /* 'f' */ )) && ((LA(3) >= 0x30 /* '0' */  && LA(3) <= 0x3a /* ':' */ )))) {
						int _m202 = mark();
						synPredMatched202 = true;
						inputState->guessing++;
						try {
							{
							{ // ( ... )+
							int _cnt201=0;
							for (;;) {
								if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
									mNUM_HEX_4DIGIT(false);
									match(':' /* charlit */ );
								}
								else {
									if ( _cnt201>=1 ) { goto _loop201; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt201++;
							}
							_loop201:;
							}  // ( ... )+
							match(':' /* charlit */ );
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched202 = false;
						}
						rewind(_m202);
						inputState->guessing--;
					}
					if ( synPredMatched202 ) {
						{
						{ // ( ... )+
						int _cnt205=0;
						for (;;) {
							if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
								mNUM_HEX_4DIGIT(false);
								match(':' /* charlit */ );
							}
							else {
								if ( _cnt205>=1 ) { goto _loop205; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt205++;
						}
						_loop205:;
						}  // ( ... )+
						match(':' /* charlit */ );
						{
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
							mNUM_HEX_4DIGIT(false);
							{ // ( ... )*
							for (;;) {
								if ((LA(1) == 0x3a /* ':' */ )) {
									match(':' /* charlit */ );
									mNUM_HEX_4DIGIT(false);
								}
								else {
									goto _loop208;
								}
								
							}
							_loop208:;
							} // ( ... )*
						}
						else {
						}
						
						}
						}
						if ( inputState->guessing==0 ) {
#line 1115 "pf.g"
							_ttype = IPV6;
#line 1003 "PFCfgLexer.cpp"
						}
					}
					else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && ((LA(2) >= 0x61 /* 'a' */  && LA(2) <= 0x66 /* 'f' */ )) && ((LA(3) >= 0x30 /* '0' */  && LA(3) <= 0x3a /* ':' */ ))) {
						mNUM_HEX_4DIGIT(false);
						{ // ( ... )+
						int _cnt210=0;
						for (;;) {
							if ((LA(1) == 0x3a /* ':' */ )) {
								match(':' /* charlit */ );
								mNUM_HEX_4DIGIT(false);
							}
							else {
								if ( _cnt210>=1 ) { goto _loop210; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt210++;
						}
						_loop210:;
						}  // ( ... )+
						if ( inputState->guessing==0 ) {
#line 1118 "pf.g"
							_ttype = IPV6;
#line 1026 "PFCfgLexer.cpp"
						}
					}
					else {
						throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
					}
					
					}
					if ( inputState->guessing==0 ) {
#line 1120 "pf.g"
						_ttype = IPV6;
#line 1037 "PFCfgLexer.cpp"
					}
				}
				else if ((LA(1) == 0x3a /* ':' */ ) && (LA(2) == 0x3a /* ':' */ ) && (true)) {
					match(':' /* charlit */ );
					match(':' /* charlit */ );
					if ( inputState->guessing==0 ) {
#line 1127 "pf.g"
						_ttype = IPV6;
#line 1046 "PFCfgLexer.cpp"
					}
				}
				else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (true) && (true)) {
					{ // ( ... )+
					int _cnt195=0;
					for (;;) {
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
							mDIGIT(false);
						}
						else {
							if ( _cnt195>=1 ) { goto _loop195; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt195++;
					}
					_loop195:;
					}  // ( ... )+
					if ( inputState->guessing==0 ) {
#line 1106 "pf.g"
						_ttype = INT_CONST;
#line 1067 "PFCfgLexer.cpp"
					}
				}
				else if ((LA(1) == 0x3a /* ':' */ ) && (true)) {
					match(':' /* charlit */ );
					if ( inputState->guessing==0 ) {
#line 1130 "pf.g"
						_ttype = COLON;
#line 1075 "PFCfgLexer.cpp"
					}
				}
				else if ((_tokenSet_3.member(LA(1)))) {
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
					default:
					{
						throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
					}
					}
					}
					{ // ( ... )*
					for (;;) {
						switch ( LA(1)) {
						case 0x22 /* '\"' */ :
						{
							match('\"' /* charlit */ );
							break;
						}
						case 0x24 /* '$' */ :
						{
							match('$' /* charlit */ );
							break;
						}
						case 0x25 /* '%' */ :
						{
							match('%' /* charlit */ );
							break;
						}
						case 0x26 /* '&' */ :
						{
							match('&' /* charlit */ );
							break;
						}
						case 0x2d /* '-' */ :
						{
							match('-' /* charlit */ );
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
						case 0x3b /* ';' */ :
						{
							match(';' /* charlit */ );
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
							goto _loop217;
						}
						}
					}
					_loop217:;
					} // ( ... )*
					if ( inputState->guessing==0 ) {
#line 1142 "pf.g"
						_ttype = WORD;
#line 1295 "PFCfgLexer.cpp"
					}
				}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}}}
	_ttype = testLiteralsTable(_ttype);
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mSTRING(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = STRING;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('\"' /* charlit */ );
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_4.member(LA(1)))) {
			matchNot('\"' /* charlit */ );
		}
		else {
			goto _loop220;
		}
		
	}
	_loop220:;
	} // ( ... )*
	match('\"' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mPIPE_CHAR(bool _createToken) {
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

void PFCfgLexer::mNUMBER_SIGN(bool _createToken) {
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

void PFCfgLexer::mPERCENT(bool _createToken) {
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

void PFCfgLexer::mAMPERSAND(bool _createToken) {
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

void PFCfgLexer::mAPOSTROPHE(bool _createToken) {
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

void PFCfgLexer::mSTAR(bool _createToken) {
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

void PFCfgLexer::mPLUS(bool _createToken) {
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

void PFCfgLexer::mCOMMA(bool _createToken) {
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

void PFCfgLexer::mMINUS(bool _createToken) {
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

void PFCfgLexer::mDOT(bool _createToken) {
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

void PFCfgLexer::mSLASH(bool _createToken) {
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

void PFCfgLexer::mSEMICOLON(bool _createToken) {
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

void PFCfgLexer::mEQUAL(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = EQUAL;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('=' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void PFCfgLexer::mQUESTION(bool _createToken) {
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

void PFCfgLexer::mCOMMERCIAL_AT(bool _createToken) {
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

void PFCfgLexer::mOPENING_PAREN(bool _createToken) {
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

void PFCfgLexer::mCLOSING_PAREN(bool _createToken) {
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

void PFCfgLexer::mOPENING_SQUARE(bool _createToken) {
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

void PFCfgLexer::mCLOSING_SQUARE(bool _createToken) {
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

void PFCfgLexer::mOPENING_BRACE(bool _createToken) {
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

void PFCfgLexer::mCLOSING_BRACE(bool _createToken) {
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

void PFCfgLexer::mCARET(bool _createToken) {
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

void PFCfgLexer::mUNDERLINE(bool _createToken) {
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

void PFCfgLexer::mTILDE(bool _createToken) {
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

void PFCfgLexer::mEXLAMATION(bool _createToken) {
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

void PFCfgLexer::mLESS_THAN(bool _createToken) {
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

void PFCfgLexer::mGREATER_THAN(bool _createToken) {
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

void PFCfgLexer::mDOUBLE_QUOTE(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = DOUBLE_QUOTE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	match('\"' /* charlit */ );
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}


const unsigned long PFCfgLexer::_tokenSet_0_data_[] = { 4294958072UL, 1UL, 0UL, 2147483648UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   0x7f 0x80 0x81 
// 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_0(_tokenSet_0_data_,16);
const unsigned long PFCfgLexer::_tokenSet_1_data_[] = { 4294958072UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # $ % 
// & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G 
// H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g h 
// i j k l m n o p q r s t u v w x y z { | } ~ 0x7f 0x80 0x81 0x82 0x83 
// 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_1(_tokenSet_1_data_,16);
const unsigned long PFCfgLexer::_tokenSet_2_data_[] = { 0UL, 67059712UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_2(_tokenSet_2_data_,10);
const unsigned long PFCfgLexer::_tokenSet_3_data_[] = { 0UL, 0UL, 134217726UL, 134217726UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h 
// i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_3(_tokenSet_3_data_,10);
const unsigned long PFCfgLexer::_tokenSet_4_data_[] = { 4294967288UL, 4294967291UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! # $ 
// % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g 
// h i j k l m n o p q r s t u v w x y z { | } ~ 0x7f 0x80 0x81 0x82 0x83 
// 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_4(_tokenSet_4_data_,16);


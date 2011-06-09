/* $ANTLR 2.7.7 (20100319): "pf.g" -> "PFCfgLexer.cpp"$ */
#line 43 "pf.g"

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

#line 49 "pf.g"

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
	literals["badhead"] = 198;
	literals["notifications"] = 232;
	literals["state-policy"] = 26;
	literals["floating"] = 28;
	literals["no"] = 84;
	literals["counters"] = 72;
	literals["esp"] = 126;
	literals["routersol"] = 157;
	literals["frags"] = 60;
	literals["reply-to"] = 139;
	literals["icmp.first"] = 49;
	literals["string-key"] = 99;
	literals["gre"] = 125;
	literals["pass"] = 86;
	literals["scrub"] = 65;
	literals["warnings"] = 233;
	literals["include"] = 6;
	literals["skip"] = 32;
	literals["timeout"] = 13;
	literals["eigrp"] = 128;
	literals["icmp-type"] = 148;
	literals["transit"] = 196;
	literals["inet"] = 115;
	literals["no-df"] = 143;
	literals["network"] = 77;
	literals["photuris"] = 173;
	literals["igmp"] = 120;
	literals["unreach"] = 151;
	literals["range"] = 225;
	literals["rsvp"] = 124;
	literals["debugging"] = 228;
	literals["host-tos"] = 186;
	literals["paramprob"] = 159;
	literals["user"] = 112;
	literals["interface"] = 214;
	literals["adaptive.end"] = 58;
	literals["limit"] = 21;
	literals["state-defaults"] = 29;
	literals["hex-key"] = 98;
	literals["net-unk"] = 180;
	literals["antispoof"] = 9;
	literals["udp.single"] = 47;
	literals["inforeq"] = 162;
	literals["ipv6-here"] = 170;
	literals["redir"] = 153;
	literals["static-port"] = 88;
	literals["common-adv"] = 195;
	literals["loginterface"] = 22;
	literals["ip"] = 118;
	literals["mobregreq"] = 171;
	literals["conservative"] = 17;
	literals["ospf"] = 129;
	literals["proto-unr"] = 176;
	literals["peer"] = 79;
	literals["inforep"] = 163;
	literals["errors"] = 230;
	literals["tables-entries"] = 64;
	literals["any"] = 136;
	literals["mobregrep"] = 172;
	literals["label"] = 211;
	literals["pptp"] = 221;
	literals["synproxy"] = 209;
	literals["debug"] = 37;
	literals["alerts"] = 226;
	literals["all"] = 111;
	literals["state"] = 210;
	literals["tag"] = 206;
	literals["in"] = 108;
	literals["tables"] = 63;
	literals["file"] = 73;
	literals["nos"] = 218;
	literals["src-nodes"] = 62;
	literals["ipv6-where"] = 169;
	literals["require-order"] = 30;
	literals["udp"] = 122;
	literals["states"] = 61;
	literals["sticky-address"] = 101;
	literals["return-icmp"] = 106;
	literals["redir-tos-net"] = 192;
	literals["pim"] = 220;
	literals["emergencies"] = 229;
	literals["squench"] = 152;
	literals["disable"] = 234;
	literals["flags"] = 147;
	literals["tcp"] = 121;
	literals["net-tos"] = 185;
	literals["reassemble"] = 38;
	literals["adaptive.start"] = 57;
	literals["frag"] = 54;
	literals["port"] = 92;
	literals["icmp"] = 119;
	literals["to"] = 113;
	literals["return-rst"] = 104;
	literals["normal-adv"] = 194;
	literals["optimization"] = 15;
	literals["log"] = 110;
	literals["fragment"] = 140;
	literals["snp"] = 223;
	literals["broadcast"] = 78;
	literals["icmp6-type"] = 204;
	literals["normal"] = 19;
	literals["code"] = 149;
	literals["if-bound"] = 27;
	literals["src.track"] = 56;
	literals["drop-ovl"] = 142;
	literals["routeradv"] = 156;
	literals["other.single"] = 52;
	literals["bitmask"] = 95;
	literals["maskreq"] = 164;
	literals["ipip"] = 130;
	literals["tcp.closed"] = 45;
	literals["block"] = 103;
	literals["high-latency"] = 18;
	literals["udp.first"] = 46;
	literals["badlen"] = 200;
	literals["tcp.first"] = 40;
	literals["host-unr"] = 175;
	literals["ah"] = 127;
	literals["random-id"] = 146;
	literals["modulate"] = 208;
	literals["interval"] = 55;
	literals["maskrep"] = 165;
	literals["ruleset-optimization"] = 14;
	literals["trace"] = 166;
	literals["rip"] = 222;
	literals["urpf-failed"] = 135;
	literals["set"] = 12;
	literals["source-hash"] = 97;
	literals["critical"] = 227;
	literals["quit"] = 213;
	literals["icmp.error"] = 50;
	literals["const"] = 71;
	literals["altq"] = 10;
	literals["tcp.closing"] = 43;
	literals["port-unr"] = 177;
	literals["table"] = 67;
	literals["redir-tos-host"] = 193;
	literals["fingerprints"] = 31;
	literals["return"] = 25;
	literals["optmiss"] = 199;
	literals["match"] = 66;
	literals["keep"] = 207;
	literals["net-prohib"] = 183;
	literals["inet6"] = 116;
	literals["from"] = 134;
	literals["tcp.finwait"] = 44;
	literals["hostid"] = 39;
	literals["proto"] = 117;
	literals["vrrp"] = 131;
	literals["drop"] = 24;
	literals["l2tp"] = 132;
	literals["max-mss"] = 145;
	literals["isolate"] = 182;
	literals["timereq"] = 160;
	literals["aggressive"] = 16;
	literals["icmp6"] = 215;
	literals["echoreq"] = 155;
	literals["tcp.established"] = 42;
	literals["decrypt-fail"] = 203;
	literals["mobredir"] = 168;
	literals["other.first"] = 51;
	literals["ipsec"] = 217;
	literals["no-route"] = 137;
	literals["random"] = 96;
	literals["binat"] = 102;
	literals["srcfail"] = 179;
	literals["self"] = 80;
	literals["timerep"] = 161;
	literals["crop"] = 141;
	literals["host-preced"] = 188;
	literals["host"] = 224;
	literals["echorep"] = 150;
	literals["other.multiple"] = 53;
	literals["althost"] = 154;
	literals["udp.multiple"] = 48;
	literals["cutoff-preced"] = 189;
	literals["redir-host"] = 191;
	literals["rdr"] = 89;
	literals["tagged"] = 205;
	literals["on"] = 33;
	literals["round-robin"] = 100;
	literals["pcp"] = 219;
	literals["block-policy"] = 23;
	literals["persist"] = 70;
	literals["unknown-ind"] = 201;
	literals["redir-net"] = 190;
	literals["filter-prohib"] = 187;
	literals["nat"] = 85;
	literals["satellite"] = 20;
	literals["informational"] = 231;
	literals["needfrag"] = 178;
	literals["tcp.opening"] = 41;
	literals["igrp"] = 216;
	literals["quick"] = 114;
	literals["timex"] = 158;
	literals["host-unk"] = 181;
	literals["route-to"] = 138;
	literals["dataconv"] = 167;
	literals["rdp"] = 123;
	literals["net-unr"] = 174;
	literals["queue"] = 11;
	literals["isis"] = 133;
	literals["reassemb"] = 197;
	literals["inactive"] = 235;
	literals["out"] = 109;
	literals["min-ttl"] = 144;
	literals["auth-fail"] = 202;
	literals["exit"] = 212;
	literals["host-prohib"] = 184;
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
			goto _loop256;
		}
		
	}
	_loop256:;
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
#line 1933 "pf.g"
		newline();
#line 602 "PFCfgLexer.cpp"
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
#line 1928 "pf.g"
		_ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP;
#line 683 "PFCfgLexer.cpp"
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
	
	{
	switch ( LA(1)) {
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
	case 0x41 /* 'A' */ :
	case 0x42 /* 'B' */ :
	case 0x43 /* 'C' */ :
	case 0x44 /* 'D' */ :
	case 0x45 /* 'E' */ :
	case 0x46 /* 'F' */ :
	{
		matchRange('A','F');
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	}
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
	if ((_tokenSet_2.member(LA(1)))) {
		{
		mHEX_DIGIT(false);
		}
		{
		if ((_tokenSet_2.member(LA(1)))) {
			{
			mHEX_DIGIT(false);
			}
			{
			if ((_tokenSet_2.member(LA(1)))) {
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
	
	bool synPredMatched309 = false;
	if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_3.member(LA(2))) && (_tokenSet_3.member(LA(3))))) {
		int _m309 = mark();
		synPredMatched309 = true;
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
			synPredMatched309 = false;
		}
		rewind(_m309);
		inputState->guessing--;
	}
	if ( synPredMatched309 ) {
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
#line 1988 "pf.g"
			_ttype = IPV4;
#line 940 "PFCfgLexer.cpp"
		}
	}
	else {
		bool synPredMatched316 = false;
		if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_3.member(LA(2))) && (_tokenSet_3.member(LA(3))))) {
			int _m316 = mark();
			synPredMatched316 = true;
			inputState->guessing++;
			try {
				{
				{ // ( ... )+
				int _cnt313=0;
				for (;;) {
					if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
						mDIGIT(false);
					}
					else {
						if ( _cnt313>=1 ) { goto _loop313; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt313++;
				}
				_loop313:;
				}  // ( ... )+
				match('.' /* charlit */ );
				{ // ( ... )+
				int _cnt315=0;
				for (;;) {
					if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
						mDIGIT(false);
					}
					else {
						if ( _cnt315>=1 ) { goto _loop315; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt315++;
				}
				_loop315:;
				}  // ( ... )+
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched316 = false;
			}
			rewind(_m316);
			inputState->guessing--;
		}
		if ( synPredMatched316 ) {
			{
			{ // ( ... )+
			int _cnt319=0;
			for (;;) {
				if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
					mDIGIT(false);
				}
				else {
					if ( _cnt319>=1 ) { goto _loop319; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt319++;
			}
			_loop319:;
			}  // ( ... )+
			match('.' /* charlit */ );
			{ // ( ... )+
			int _cnt321=0;
			for (;;) {
				if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
					mDIGIT(false);
				}
				else {
					if ( _cnt321>=1 ) { goto _loop321; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt321++;
			}
			_loop321:;
			}  // ( ... )+
			}
			if ( inputState->guessing==0 ) {
#line 1991 "pf.g"
				_ttype = NUMBER;
#line 1023 "PFCfgLexer.cpp"
			}
		}
		else {
			bool synPredMatched284 = false;
			if (((_tokenSet_2.member(LA(1))) && (_tokenSet_4.member(LA(2))) && (true))) {
				int _m284 = mark();
				synPredMatched284 = true;
				inputState->guessing++;
				try {
					{
					{ // ( ... )+
					int _cnt283=0;
					for (;;) {
						if ((_tokenSet_2.member(LA(1)))) {
							mHEX_DIGIT(false);
						}
						else {
							if ( _cnt283>=1 ) { goto _loop283; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt283++;
					}
					_loop283:;
					}  // ( ... )+
					match(':' /* charlit */ );
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched284 = false;
				}
				rewind(_m284);
				inputState->guessing--;
			}
			if ( synPredMatched284 ) {
				{
				{
				{ // ( ... )+
				int _cnt288=0;
				for (;;) {
					if ((_tokenSet_2.member(LA(1)))) {
						mHEX_DIGIT(false);
					}
					else {
						if ( _cnt288>=1 ) { goto _loop288; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt288++;
				}
				_loop288:;
				}  // ( ... )+
				{ // ( ... )+
				int _cnt292=0;
				for (;;) {
					if ((LA(1) == 0x3a /* ':' */ )) {
						match(':' /* charlit */ );
						{ // ( ... )*
						for (;;) {
							if ((_tokenSet_2.member(LA(1)))) {
								mHEX_DIGIT(false);
							}
							else {
								goto _loop291;
							}
							
						}
						_loop291:;
						} // ( ... )*
					}
					else {
						if ( _cnt292>=1 ) { goto _loop292; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt292++;
				}
				_loop292:;
				}  // ( ... )+
				}
				if ( inputState->guessing==0 ) {
#line 1973 "pf.g"
					_ttype = IPV6;
#line 1104 "PFCfgLexer.cpp"
				}
				}
			}
			else {
				bool synPredMatched294 = false;
				if (((LA(1) == 0x3a /* ':' */ ))) {
					int _m294 = mark();
					synPredMatched294 = true;
					inputState->guessing++;
					try {
						{
						match(':' /* charlit */ );
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched294 = false;
					}
					rewind(_m294);
					inputState->guessing--;
				}
				if ( synPredMatched294 ) {
					{
					bool synPredMatched299 = false;
					if (((LA(1) == 0x3a /* ':' */ ) && (LA(2) == 0x3a /* ':' */ ) && (_tokenSet_2.member(LA(3))))) {
						int _m299 = mark();
						synPredMatched299 = true;
						inputState->guessing++;
						try {
							{
							match(':' /* charlit */ );
							match(':' /* charlit */ );
							{ // ( ... )+
							int _cnt298=0;
							for (;;) {
								if ((_tokenSet_2.member(LA(1)))) {
									mHEX_DIGIT(false);
								}
								else {
									if ( _cnt298>=1 ) { goto _loop298; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt298++;
							}
							_loop298:;
							}  // ( ... )+
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched299 = false;
						}
						rewind(_m299);
						inputState->guessing--;
					}
					if ( synPredMatched299 ) {
						{
						match(':' /* charlit */ );
						match(':' /* charlit */ );
						{ // ( ... )+
						int _cnt302=0;
						for (;;) {
							if ((_tokenSet_2.member(LA(1)))) {
								mHEX_DIGIT(false);
							}
							else {
								if ( _cnt302>=1 ) { goto _loop302; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt302++;
						}
						_loop302:;
						}  // ( ... )+
						{ // ( ... )*
						for (;;) {
							if ((LA(1) == 0x3a /* ':' */ )) {
								match(':' /* charlit */ );
								{ // ( ... )+
								int _cnt305=0;
								for (;;) {
									if ((_tokenSet_2.member(LA(1)))) {
										mHEX_DIGIT(false);
									}
									else {
										if ( _cnt305>=1 ) { goto _loop305; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
									}
									
									_cnt305++;
								}
								_loop305:;
								}  // ( ... )+
							}
							else {
								goto _loop306;
							}
							
						}
						_loop306:;
						} // ( ... )*
						}
						if ( inputState->guessing==0 ) {
#line 1979 "pf.g"
							_ttype = IPV6;
#line 1206 "PFCfgLexer.cpp"
						}
					}
					else if ((LA(1) == 0x3a /* ':' */ ) && (LA(2) == 0x3a /* ':' */ ) && (true)) {
						{
						match(':' /* charlit */ );
						match(':' /* charlit */ );
						}
						if ( inputState->guessing==0 ) {
#line 1981 "pf.g"
							_ttype = IPV6;
#line 1217 "PFCfgLexer.cpp"
						}
					}
					else if ((LA(1) == 0x3a /* ':' */ ) && (true)) {
						match(':' /* charlit */ );
						if ( inputState->guessing==0 ) {
#line 1983 "pf.g"
							_ttype = COLON;
#line 1225 "PFCfgLexer.cpp"
						}
					}
					else {
						throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
					}
					
					}
				}
				else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (true) && (true)) {
					{ // ( ... )+
					int _cnt323=0;
					for (;;) {
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
							mDIGIT(false);
						}
						else {
							if ( _cnt323>=1 ) { goto _loop323; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt323++;
					}
					_loop323:;
					}  // ( ... )+
					if ( inputState->guessing==0 ) {
#line 1993 "pf.g"
						_ttype = INT_CONST;
#line 1252 "PFCfgLexer.cpp"
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
						case 0x2e /* '.' */ :
						{
							match('.' /* charlit */ );
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
							goto _loop326;
						}
						}
					}
					_loop326:;
					} // ( ... )*
					if ( inputState->guessing==0 ) {
#line 2004 "pf.g"
						_ttype = WORD;
#line 1477 "PFCfgLexer.cpp"
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
		if ((_tokenSet_6.member(LA(1)))) {
			matchNot('\"' /* charlit */ );
		}
		else {
			goto _loop329;
		}
		
	}
	_loop329:;
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
// 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 
// 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 
// 0x9e 0x9f 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 
// 0xac 0xad 0xae 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 
// 0xba 0xbb 0xbc 0xbd 0xbe 0xbf 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 
// 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 
// 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf 0xe0 0xe1 0xe2 0xe3 
// 0xe4 0xe5 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef 0xf0 0xf1 
// 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_0(_tokenSet_0_data_,16);
const unsigned long PFCfgLexer::_tokenSet_1_data_[] = { 4294958072UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # $ % 
// & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G 
// H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g h 
// i j k l m n o p q r s t u v w x y z { | } ~ 0x7f 0x80 0x81 0x82 0x83 
// 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 0x90 0x91 
// 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 
// 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 
// 0xae 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 
// 0xbc 0xbd 0xbe 0xbf 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 
// 0xca 0xcb 0xcc 0xcd 0xce 0xcf 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 
// 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 
// 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef 0xf0 0xf1 0xf2 0xf3 
// 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_1(_tokenSet_1_data_,16);
const unsigned long PFCfgLexer::_tokenSet_2_data_[] = { 0UL, 67043328UL, 126UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 A B C D E F a b c d e f 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_2(_tokenSet_2_data_,10);
const unsigned long PFCfgLexer::_tokenSet_3_data_[] = { 0UL, 67059712UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_3(_tokenSet_3_data_,10);
const unsigned long PFCfgLexer::_tokenSet_4_data_[] = { 0UL, 134152192UL, 126UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 : A B C D E F a b c d e f 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_4(_tokenSet_4_data_,10);
const unsigned long PFCfgLexer::_tokenSet_5_data_[] = { 0UL, 0UL, 134217726UL, 134217726UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h 
// i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_5(_tokenSet_5_data_,10);
const unsigned long PFCfgLexer::_tokenSet_6_data_[] = { 4294967288UL, 4294967291UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! # $ 
// % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g 
// h i j k l m n o p q r s t u v w x y z { | } ~ 0x7f 0x80 0x81 0x82 0x83 
// 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 0x90 0x91 
// 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 
// 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 
// 0xae 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 
// 0xbc 0xbd 0xbe 0xbf 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 
// 0xca 0xcb 0xcc 0xcd 0xce 0xcf 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 
// 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 
// 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef 0xf0 0xf1 0xf2 0xf3 
// 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_6(_tokenSet_6_data_,16);


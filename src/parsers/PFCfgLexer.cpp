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
	literals["badhead"] = 196;
	literals["notifications"] = 230;
	literals["state-policy"] = 25;
	literals["floating"] = 27;
	literals["no"] = 82;
	literals["esp"] = 124;
	literals["routersol"] = 155;
	literals["frags"] = 59;
	literals["reply-to"] = 137;
	literals["icmp.first"] = 45;
	literals["string-key"] = 97;
	literals["gre"] = 123;
	literals["pass"] = 84;
	literals["scrub"] = 64;
	literals["warnings"] = 231;
	literals["skip"] = 31;
	literals["timeout"] = 12;
	literals["eigrp"] = 126;
	literals["icmp-type"] = 146;
	literals["transit"] = 194;
	literals["inet"] = 113;
	literals["no-df"] = 141;
	literals["network"] = 76;
	literals["photuris"] = 171;
	literals["igmp"] = 118;
	literals["unreach"] = 149;
	literals["range"] = 223;
	literals["rsvp"] = 122;
	literals["debugging"] = 226;
	literals["host-tos"] = 184;
	literals["paramprob"] = 157;
	literals["user"] = 110;
	literals["interface"] = 212;
	literals["adaptive.end"] = 54;
	literals["limit"] = 20;
	literals["state-defaults"] = 28;
	literals["hex-key"] = 96;
	literals["net-unk"] = 178;
	literals["antispoof"] = 8;
	literals["udp.single"] = 43;
	literals["inforeq"] = 160;
	literals["ipv6-here"] = 168;
	literals["redir"] = 151;
	literals["static-port"] = 86;
	literals["common-adv"] = 193;
	literals["loginterface"] = 21;
	literals["ip"] = 116;
	literals["mobregreq"] = 169;
	literals["conservative"] = 16;
	literals["ospf"] = 127;
	literals["proto-unr"] = 174;
	literals["peer"] = 78;
	literals["inforep"] = 161;
	literals["errors"] = 228;
	literals["tables-entries"] = 63;
	literals["any"] = 134;
	literals["mobregrep"] = 170;
	literals["label"] = 209;
	literals["pptp"] = 219;
	literals["synproxy"] = 207;
	literals["debug"] = 33;
	literals["alerts"] = 224;
	literals["all"] = 109;
	literals["state"] = 208;
	literals["tag"] = 204;
	literals["in"] = 106;
	literals["tables"] = 62;
	literals["file"] = 72;
	literals["nos"] = 216;
	literals["src-nodes"] = 61;
	literals["ipv6-where"] = 167;
	literals["require-order"] = 29;
	literals["udp"] = 120;
	literals["states"] = 60;
	literals["sticky-address"] = 99;
	literals["return-icmp"] = 104;
	literals["redir-tos-net"] = 190;
	literals["pim"] = 218;
	literals["emergencies"] = 227;
	literals["squench"] = 150;
	literals["disable"] = 232;
	literals["flags"] = 145;
	literals["tcp"] = 119;
	literals["net-tos"] = 183;
	literals["reassemble"] = 34;
	literals["adaptive.start"] = 53;
	literals["frag"] = 50;
	literals["port"] = 90;
	literals["icmp"] = 117;
	literals["to"] = 111;
	literals["return-rst"] = 102;
	literals["normal-adv"] = 192;
	literals["optimization"] = 14;
	literals["log"] = 108;
	literals["fragment"] = 138;
	literals["snp"] = 221;
	literals["broadcast"] = 77;
	literals["icmp6-type"] = 202;
	literals["normal"] = 18;
	literals["code"] = 147;
	literals["if-bound"] = 26;
	literals["src.track"] = 52;
	literals["drop-ovl"] = 140;
	literals["routeradv"] = 154;
	literals["other.single"] = 48;
	literals["bitmask"] = 93;
	literals["maskreq"] = 162;
	literals["ipip"] = 128;
	literals["tcp.closed"] = 41;
	literals["block"] = 101;
	literals["high-latency"] = 17;
	literals["udp.first"] = 42;
	literals["badlen"] = 198;
	literals["tcp.first"] = 36;
	literals["host-unr"] = 173;
	literals["ah"] = 125;
	literals["random-id"] = 144;
	literals["modulate"] = 206;
	literals["interval"] = 51;
	literals["maskrep"] = 163;
	literals["ruleset-optimization"] = 13;
	literals["trace"] = 164;
	literals["rip"] = 220;
	literals["urpf-failed"] = 133;
	literals["set"] = 11;
	literals["source-hash"] = 95;
	literals["critical"] = 225;
	literals["quit"] = 211;
	literals["icmp.error"] = 46;
	literals["const"] = 70;
	literals["altq"] = 9;
	literals["tcp.closing"] = 39;
	literals["port-unr"] = 175;
	literals["table"] = 66;
	literals["redir-tos-host"] = 191;
	literals["fingerprints"] = 30;
	literals["return"] = 24;
	literals["optmiss"] = 197;
	literals["match"] = 65;
	literals["keep"] = 205;
	literals["net-prohib"] = 181;
	literals["inet6"] = 114;
	literals["from"] = 132;
	literals["tcp.finwait"] = 40;
	literals["hostid"] = 35;
	literals["proto"] = 115;
	literals["vrrp"] = 129;
	literals["drop"] = 23;
	literals["l2tp"] = 130;
	literals["max-mss"] = 143;
	literals["isolate"] = 180;
	literals["timereq"] = 158;
	literals["aggressive"] = 15;
	literals["icmp6"] = 213;
	literals["echoreq"] = 153;
	literals["tcp.established"] = 38;
	literals["decrypt-fail"] = 201;
	literals["mobredir"] = 166;
	literals["other.first"] = 47;
	literals["ipsec"] = 215;
	literals["no-route"] = 135;
	literals["random"] = 94;
	literals["binat"] = 100;
	literals["srcfail"] = 177;
	literals["self"] = 79;
	literals["timerep"] = 159;
	literals["crop"] = 139;
	literals["host-preced"] = 186;
	literals["host"] = 222;
	literals["echorep"] = 148;
	literals["other.multiple"] = 49;
	literals["althost"] = 152;
	literals["udp.multiple"] = 44;
	literals["cutoff-preced"] = 187;
	literals["redir-host"] = 189;
	literals["rdr"] = 87;
	literals["tagged"] = 203;
	literals["on"] = 32;
	literals["round-robin"] = 98;
	literals["pcp"] = 217;
	literals["block-policy"] = 22;
	literals["persist"] = 69;
	literals["unknown-ind"] = 199;
	literals["redir-net"] = 188;
	literals["filter-prohib"] = 185;
	literals["nat"] = 83;
	literals["satellite"] = 19;
	literals["informational"] = 229;
	literals["needfrag"] = 176;
	literals["tcp.opening"] = 37;
	literals["igrp"] = 214;
	literals["quick"] = 112;
	literals["timex"] = 156;
	literals["host-unk"] = 179;
	literals["route-to"] = 136;
	literals["dataconv"] = 165;
	literals["rdp"] = 121;
	literals["net-unr"] = 172;
	literals["queue"] = 10;
	literals["isis"] = 131;
	literals["reassemb"] = 195;
	literals["inactive"] = 233;
	literals["out"] = 107;
	literals["min-ttl"] = 142;
	literals["auth-fail"] = 200;
	literals["exit"] = 210;
	literals["host-prohib"] = 182;
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
			goto _loop248;
		}
		
	}
	_loop248:;
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
#line 1890 "pf.g"
		newline();
#line 600 "PFCfgLexer.cpp"
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
#line 1885 "pf.g"
		_ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP;
#line 681 "PFCfgLexer.cpp"
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
	
	bool synPredMatched286 = false;
	if (((_tokenSet_2.member(LA(1))) && (_tokenSet_3.member(LA(2))) && (_tokenSet_3.member(LA(3))))) {
		int _m286 = mark();
		synPredMatched286 = true;
		inputState->guessing++;
		try {
			{
			mNUM_HEX_4DIGIT(false);
			match(':' /* charlit */ );
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched286 = false;
		}
		rewind(_m286);
		inputState->guessing--;
	}
	if ( synPredMatched286 ) {
		{
		bool synPredMatched291 = false;
		if (((_tokenSet_2.member(LA(1))) && (_tokenSet_3.member(LA(2))) && (_tokenSet_3.member(LA(3))))) {
			int _m291 = mark();
			synPredMatched291 = true;
			inputState->guessing++;
			try {
				{
				{ // ( ... )+
				int _cnt290=0;
				for (;;) {
					if ((_tokenSet_2.member(LA(1)))) {
						mNUM_HEX_4DIGIT(false);
						match(':' /* charlit */ );
					}
					else {
						if ( _cnt290>=1 ) { goto _loop290; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt290++;
				}
				_loop290:;
				}  // ( ... )+
				match(':' /* charlit */ );
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched291 = false;
			}
			rewind(_m291);
			inputState->guessing--;
		}
		if ( synPredMatched291 ) {
			{
			{ // ( ... )+
			int _cnt294=0;
			for (;;) {
				if ((_tokenSet_2.member(LA(1)))) {
					mNUM_HEX_4DIGIT(false);
					match(':' /* charlit */ );
				}
				else {
					if ( _cnt294>=1 ) { goto _loop294; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt294++;
			}
			_loop294:;
			}  // ( ... )+
			match(':' /* charlit */ );
			{
			if ((_tokenSet_2.member(LA(1)))) {
				mNUM_HEX_4DIGIT(false);
				{ // ( ... )*
				for (;;) {
					if ((LA(1) == 0x3a /* ':' */ )) {
						match(':' /* charlit */ );
						mNUM_HEX_4DIGIT(false);
					}
					else {
						goto _loop297;
					}
					
				}
				_loop297:;
				} // ( ... )*
			}
			else {
			}
			
			}
			}
			if ( inputState->guessing==0 ) {
#line 1938 "pf.g"
				_ttype = IPV6;
#line 999 "PFCfgLexer.cpp"
			}
		}
		else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_3.member(LA(2))) && (_tokenSet_3.member(LA(3)))) {
			{
			mNUM_HEX_4DIGIT(false);
			{ // ( ... )+
			int _cnt300=0;
			for (;;) {
				if ((LA(1) == 0x3a /* ':' */ )) {
					match(':' /* charlit */ );
					mNUM_HEX_4DIGIT(false);
				}
				else {
					if ( _cnt300>=1 ) { goto _loop300; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt300++;
			}
			_loop300:;
			}  // ( ... )+
			}
			if ( inputState->guessing==0 ) {
#line 1940 "pf.g"
				_ttype = IPV6;
#line 1024 "PFCfgLexer.cpp"
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
	}
	else {
		bool synPredMatched302 = false;
		if (((LA(1) == 0x3a /* ':' */ ) && (LA(2) == 0x3a /* ':' */ ) && (_tokenSet_2.member(LA(3))))) {
			int _m302 = mark();
			synPredMatched302 = true;
			inputState->guessing++;
			try {
				{
				match(':' /* charlit */ );
				match(':' /* charlit */ );
				mNUM_HEX_4DIGIT(false);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched302 = false;
			}
			rewind(_m302);
			inputState->guessing--;
		}
		if ( synPredMatched302 ) {
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
					goto _loop304;
				}
				
			}
			_loop304:;
			} // ( ... )*
			if ( inputState->guessing==0 ) {
#line 1944 "pf.g"
				_ttype = IPV6;
#line 1072 "PFCfgLexer.cpp"
			}
		}
		else {
			bool synPredMatched306 = false;
			if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_4.member(LA(2))) && (_tokenSet_4.member(LA(3))))) {
				int _m306 = mark();
				synPredMatched306 = true;
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
					synPredMatched306 = false;
				}
				rewind(_m306);
				inputState->guessing--;
			}
			if ( synPredMatched306 ) {
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
#line 1962 "pf.g"
					_ttype = IPV4;
#line 1108 "PFCfgLexer.cpp"
				}
			}
			else {
				bool synPredMatched313 = false;
				if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_4.member(LA(2))) && (_tokenSet_4.member(LA(3))))) {
					int _m313 = mark();
					synPredMatched313 = true;
					inputState->guessing++;
					try {
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
						match('.' /* charlit */ );
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
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched313 = false;
					}
					rewind(_m313);
					inputState->guessing--;
				}
				if ( synPredMatched313 ) {
					{
					{ // ( ... )+
					int _cnt316=0;
					for (;;) {
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
							mDIGIT(false);
						}
						else {
							if ( _cnt316>=1 ) { goto _loop316; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt316++;
					}
					_loop316:;
					}  // ( ... )+
					match('.' /* charlit */ );
					{ // ( ... )+
					int _cnt318=0;
					for (;;) {
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
							mDIGIT(false);
						}
						else {
							if ( _cnt318>=1 ) { goto _loop318; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt318++;
					}
					_loop318:;
					}  // ( ... )+
					}
					if ( inputState->guessing==0 ) {
#line 1965 "pf.g"
						_ttype = NUMBER;
#line 1191 "PFCfgLexer.cpp"
					}
				}
				else {
					bool synPredMatched276 = false;
					if (((_tokenSet_2.member(LA(1))) && (_tokenSet_3.member(LA(2))) && (true))) {
						int _m276 = mark();
						synPredMatched276 = true;
						inputState->guessing++;
						try {
							{
							{ // ( ... )+
							int _cnt275=0;
							for (;;) {
								if ((_tokenSet_2.member(LA(1)))) {
									mHEX_DIGIT(false);
								}
								else {
									if ( _cnt275>=1 ) { goto _loop275; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt275++;
							}
							_loop275:;
							}  // ( ... )+
							match(':' /* charlit */ );
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched276 = false;
						}
						rewind(_m276);
						inputState->guessing--;
					}
					if ( synPredMatched276 ) {
						{
						{
						{ // ( ... )+
						int _cnt280=0;
						for (;;) {
							if ((_tokenSet_2.member(LA(1)))) {
								mHEX_DIGIT(false);
							}
							else {
								if ( _cnt280>=1 ) { goto _loop280; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt280++;
						}
						_loop280:;
						}  // ( ... )+
						{ // ( ... )+
						int _cnt284=0;
						for (;;) {
							if ((LA(1) == 0x3a /* ':' */ )) {
								match(':' /* charlit */ );
								{ // ( ... )*
								for (;;) {
									if ((_tokenSet_2.member(LA(1)))) {
										mHEX_DIGIT(false);
									}
									else {
										goto _loop283;
									}
									
								}
								_loop283:;
								} // ( ... )*
							}
							else {
								if ( _cnt284>=1 ) { goto _loop284; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt284++;
						}
						_loop284:;
						}  // ( ... )+
						}
						if ( inputState->guessing==0 ) {
#line 1929 "pf.g"
							_ttype = IPV6;
#line 1272 "PFCfgLexer.cpp"
						}
						}
					}
					else if ((LA(1) == 0x3a /* ':' */ ) && (LA(2) == 0x3a /* ':' */ ) && (true)) {
						match(':' /* charlit */ );
						match(':' /* charlit */ );
						if ( inputState->guessing==0 ) {
#line 1946 "pf.g"
							_ttype = IPV6;
#line 1282 "PFCfgLexer.cpp"
						}
					}
					else if ((LA(1) == 0x3a /* ':' */ ) && (true)) {
						match(':' /* charlit */ );
						if ( inputState->guessing==0 ) {
#line 1948 "pf.g"
							_ttype = COLON;
#line 1290 "PFCfgLexer.cpp"
						}
					}
					else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (true) && (true)) {
						{ // ( ... )+
						int _cnt320=0;
						for (;;) {
							if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
								mDIGIT(false);
							}
							else {
								if ( _cnt320>=1 ) { goto _loop320; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt320++;
						}
						_loop320:;
						}  // ( ... )+
						if ( inputState->guessing==0 ) {
#line 1967 "pf.g"
							_ttype = INT_CONST;
#line 1311 "PFCfgLexer.cpp"
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
								goto _loop323;
							}
							}
						}
						_loop323:;
						} // ( ... )*
						if ( inputState->guessing==0 ) {
#line 1980 "pf.g"
							_ttype = WORD;
#line 1536 "PFCfgLexer.cpp"
						}
					}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}}}}
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
			goto _loop326;
		}
		
	}
	_loop326:;
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
const unsigned long PFCfgLexer::_tokenSet_3_data_[] = { 0UL, 134152192UL, 126UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 : A B C D E F a b c d e f 
const ANTLR_USE_NAMESPACE(antlr)BitSet PFCfgLexer::_tokenSet_3(_tokenSet_3_data_,10);
const unsigned long PFCfgLexer::_tokenSet_4_data_[] = { 0UL, 67059712UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 
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


#ifndef INC_IOSCfgLexer_hpp_
#define INC_IOSCfgLexer_hpp_

#line 27 "iosacl.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "IOSImporter.h"

#line 11 "IOSCfgLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090306): "iosacl.g" -> "IOSCfgLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "IOSCfgParserTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
#line 33 "iosacl.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class IOSImporter;

#line 28 "IOSCfgLexer.hpp"
#line 57 "iosacl.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 34 "IOSCfgLexer.hpp"
class CUSTOM_API IOSCfgLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public IOSCfgParserTokenTypes
{
#line 1 "iosacl.g"
#line 38 "IOSCfgLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
	IOSCfgLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	IOSCfgLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	IOSCfgLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mLINE_COMMENT(bool _createToken);
	public: void mWhitespace(bool _createToken);
	public: void mNEWLINE(bool _createToken);
	protected: void mINT_CONST(bool _createToken);
	protected: void mHEX_CONST(bool _createToken);
	protected: void mNEG_INT_CONST(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	protected: void mHEXDIGIT(bool _createToken);
	public: void mNUMBER(bool _createToken);
	public: void mDOT(bool _createToken);
	public: void mWORD(bool _createToken);
	public: void mSTRING(bool _createToken);
	public: void mPIPE_CHAR(bool _createToken);
	public: void mNUMBER_SIGN(bool _createToken);
	public: void mPERCENT(bool _createToken);
	public: void mAMPERSAND(bool _createToken);
	public: void mAPOSTROPHE(bool _createToken);
	public: void mOPENING_PAREN(bool _createToken);
	public: void mCLOSING_PAREN(bool _createToken);
	public: void mSTAR(bool _createToken);
	public: void mPLUS(bool _createToken);
	public: void mCOMMA(bool _createToken);
	public: void mMINUS(bool _createToken);
	public: void mSLASH(bool _createToken);
	public: void mCOLON(bool _createToken);
	public: void mSEMICOLON(bool _createToken);
	public: void mLESS_THAN(bool _createToken);
	public: void mEQUALS(bool _createToken);
	public: void mGREATER_THAN(bool _createToken);
	public: void mQUESTION(bool _createToken);
	public: void mCOMMERCIAL_AT(bool _createToken);
	public: void mOPENING_SQUARE(bool _createToken);
	public: void mCLOSING_SQUARE(bool _createToken);
	public: void mCARET(bool _createToken);
	public: void mUNDERLINE(bool _createToken);
	public: void mOPENING_BRACE(bool _createToken);
	public: void mCLOSING_BRACE(bool _createToken);
	public: void mTILDE(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
};

#endif /*INC_IOSCfgLexer_hpp_*/

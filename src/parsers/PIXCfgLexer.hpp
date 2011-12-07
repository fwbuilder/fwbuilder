#ifndef INC_PIXCfgLexer_hpp_
#define INC_PIXCfgLexer_hpp_

#line 25 "pix.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "PIXImporter.h"

#line 11 "PIXCfgLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090306): "pix.g" -> "PIXCfgLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "PIXCfgParserTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
#line 32 "pix.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class PIXImporter;

#line 28 "PIXCfgLexer.hpp"
#line 56 "pix.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 34 "PIXCfgLexer.hpp"
class CUSTOM_API PIXCfgLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public PIXCfgParserTokenTypes
{
#line 1 "pix.g"
#line 38 "PIXCfgLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
	PIXCfgLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	PIXCfgLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	PIXCfgLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mLINE_COMMENT(bool _createToken);
	public: void mNEWLINE(bool _createToken);
	public: void mCOLON_COMMENT(bool _createToken);
	public: void mCOLON(bool _createToken);
	public: void mWhitespace(bool _createToken);
	protected: void mINT_CONST(bool _createToken);
	protected: void mHEX_CONST(bool _createToken);
	protected: void mNUMBER(bool _createToken);
	protected: void mNEG_INT_CONST(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	protected: void mHEXDIGIT(bool _createToken);
	public: void mNUMBER_ADDRESS_OR_WORD(bool _createToken);
	public: void mDOT(bool _createToken);
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
	public: void mEXLAMATION(bool _createToken);
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
	static const unsigned long _tokenSet_5_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_6;
};

#endif /*INC_PIXCfgLexer_hpp_*/

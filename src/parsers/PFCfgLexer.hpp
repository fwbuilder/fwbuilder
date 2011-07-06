#ifndef INC_PFCfgLexer_hpp_
#define INC_PFCfgLexer_hpp_

#line 25 "pf.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "PFImporter.h"

#line 11 "PFCfgLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20100319): "pf.g" -> "PFCfgLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "PFCfgParserTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
#line 32 "pf.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <iostream>
#include <sstream>

class PFImporter;

#line 29 "PFCfgLexer.hpp"
#line 57 "pf.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 35 "PFCfgLexer.hpp"
class CUSTOM_API PFCfgLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public PFCfgParserTokenTypes
{
#line 1 "pf.g"
#line 39 "PFCfgLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
	PFCfgLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	PFCfgLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	PFCfgLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mWhitespace(bool _createToken);
	public: void mCOMMENT_START(bool _createToken);
	public: void mNEWLINE(bool _createToken);
	protected: void mINT_CONST(bool _createToken);
	protected: void mHEX_CONST(bool _createToken);
	protected: void mNUMBER(bool _createToken);
	protected: void mNEG_INT_CONST(bool _createToken);
	protected: void mCOLON(bool _createToken);
	protected: void mHEX_DIGIT(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	protected: void mNUM_3DIGIT(bool _createToken);
	protected: void mNUM_HEX_4DIGIT(bool _createToken);
	public: void mNUMBER_ADDRESS_OR_WORD(bool _createToken);
	public: void mSTRING(bool _createToken);
	public: void mPIPE_CHAR(bool _createToken);
	public: void mPERCENT(bool _createToken);
	public: void mAMPERSAND(bool _createToken);
	public: void mAPOSTROPHE(bool _createToken);
	public: void mSTAR(bool _createToken);
	public: void mPLUS(bool _createToken);
	public: void mCOMMA(bool _createToken);
	public: void mMINUS(bool _createToken);
	public: void mDOT(bool _createToken);
	public: void mSLASH(bool _createToken);
	public: void mSEMICOLON(bool _createToken);
	public: void mEQUAL(bool _createToken);
	public: void mQUESTION(bool _createToken);
	public: void mCOMMERCIAL_AT(bool _createToken);
	public: void mOPENING_PAREN(bool _createToken);
	public: void mCLOSING_PAREN(bool _createToken);
	public: void mOPENING_SQUARE(bool _createToken);
	public: void mCLOSING_SQUARE(bool _createToken);
	public: void mOPENING_BRACE(bool _createToken);
	public: void mCLOSING_BRACE(bool _createToken);
	public: void mCARET(bool _createToken);
	public: void mUNDERLINE(bool _createToken);
	public: void mTILDE(bool _createToken);
	public: void mEXLAMATION(bool _createToken);
	public: void mLESS_THAN(bool _createToken);
	public: void mGREATER_THAN(bool _createToken);
	public: void mDOUBLE_QUOTE(bool _createToken);
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
};

#endif /*INC_PFCfgLexer_hpp_*/

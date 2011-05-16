#ifndef INC_IPTCfgLexer_hpp_
#define INC_IPTCfgLexer_hpp_

#line 38 "iptables.g"

    // gets inserted before antlr generated includes in the header
    // file
#include "IPTImporter.h"

#line 11 "IPTCfgLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090306): "iptables.g" -> "IPTCfgLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "IPTCfgParserTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
#line 45 "iptables.g"

    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class IPTImporter;

#line 28 "IPTCfgLexer.hpp"
#line 74 "iptables.g"

    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.

#line 34 "IPTCfgLexer.hpp"
class CUSTOM_API IPTCfgLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public IPTCfgParserTokenTypes
{
#line 1 "iptables.g"
#line 38 "IPTCfgLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
	IPTCfgLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	IPTCfgLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	IPTCfgLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mIPTABLES_SAVE_HEADER(bool _createToken);
	public: void mWhitespace(bool _createToken);
	public: void mNEWLINE(bool _createToken);
	protected: void mIPV4(bool _createToken);
	protected: void mTHREE_COMPONENT_VERSION(bool _createToken);
	protected: void mIPV6(bool _createToken);
	protected: void mMAC_ADDRESS(bool _createToken);
	protected: void mINT_CONST(bool _createToken);
	protected: void mHEX_CONST(bool _createToken);
	protected: void mNEG_INT_CONST(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	protected: void mHEXDIGIT(bool _createToken);
	protected: void mNUM_3DIGIT(bool _createToken);
	protected: void mNUM_HEX_4DIGIT(bool _createToken);
	public: void mNUMBER(bool _createToken);
	public: void mWORD(bool _createToken);
	public: void mSTRING(bool _createToken);
	protected: void mUNSUPPORTED_OPTION(bool _createToken);
	public: void mRSOURCE(bool _createToken);
	public: void mADD_RULE(bool _createToken);
	public: void mMATCH_STATE(bool _createToken);
	public: void mMATCH_SRC_MULTIPORT(bool _createToken);
	public: void mMATCH_DST_MULTIPORT(bool _createToken);
	public: void mMATCH_BOTH_MULTIPORT(bool _createToken);
	public: void mMATCH_SRC_PORT(bool _createToken);
	public: void mMATCH_DST_PORT(bool _createToken);
	public: void mMATCH_SRC_PORT_SHORT(bool _createToken);
	public: void mMATCH_DST_PORT_SHORT(bool _createToken);
	public: void mMATCH_SYN(bool _createToken);
	public: void mMATCH_TCP_FLAGS(bool _createToken);
	public: void mMATCH_TCP_OPTION(bool _createToken);
	public: void mMATCH_ICMP_TYPE(bool _createToken);
	public: void mMATCH_MARK(bool _createToken);
	public: void mMATCH_LENGTH(bool _createToken);
	public: void mMATCH_LIMIT(bool _createToken);
	public: void mMATCH_LIMIT_BURST(bool _createToken);
	public: void mMATCH_RECENT_NAME(bool _createToken);
	public: void mMATCH_RECENT_RCHECK(bool _createToken);
	public: void mMATCH_RECENT_UPDATE(bool _createToken);
	public: void mMATCH_RECENT_REMOVE(bool _createToken);
	public: void mMATCH_RECENT_SECONDS(bool _createToken);
	public: void mMATCH_RECENT_HITCOUNT(bool _createToken);
	public: void mMATCH_RECENT_RTTL(bool _createToken);
	public: void mMATCH_RECENT_RDEST(bool _createToken);
	public: void mMATCH_RECENT_SET(bool _createToken);
	public: void mMATCH_IPRANGE_SRC(bool _createToken);
	public: void mMATCH_IPRANGE_DST(bool _createToken);
	public: void mMATCH_COMMENT(bool _createToken);
	public: void mMATCH_PKT_TYPE(bool _createToken);
	public: void mREJECT_WITH(bool _createToken);
	public: void mSET_CLASS(bool _createToken);
	public: void mSET_MARK(bool _createToken);
	public: void mSAVE_MARK(bool _createToken);
	public: void mRESTORE_MARK(bool _createToken);
	public: void mSET_TOS(bool _createToken);
	public: void mCONTINUE(bool _createToken);
	public: void mROUTE_IIF(bool _createToken);
	public: void mROUTE_OIF(bool _createToken);
	public: void mROUTE_GW(bool _createToken);
	public: void mROUTE_TEE(bool _createToken);
	public: void mLOG_PREFIX(bool _createToken);
	public: void mLOG_LEVEL(bool _createToken);
	public: void mLOG_TCP_SEQ(bool _createToken);
	public: void mLOG_TCP_OPT(bool _createToken);
	public: void mLOG_IP_OPT(bool _createToken);
	public: void mULOG_PREFIX(bool _createToken);
	public: void mULOG_QTHR(bool _createToken);
	public: void mULOG_NLG(bool _createToken);
	public: void mULOG_CPR(bool _createToken);
	public: void mTO_SOURCE(bool _createToken);
	public: void mTO_DESTINATION(bool _createToken);
	public: void mTO_PORTS(bool _createToken);
	public: void mTO_NETMAP(bool _createToken);
	public: void mCLAMP_MSS(bool _createToken);
	public: void mOPT_MODULE(bool _createToken);
	public: void mOPT_SRC(bool _createToken);
	public: void mOPT_DST(bool _createToken);
	public: void mOPT_IN_INTF(bool _createToken);
	public: void mOPT_OUT_INTF(bool _createToken);
	public: void mOPT_PROTO(bool _createToken);
	public: void mOPT_TARGET(bool _createToken);
	public: void mOPT_FRAGM(bool _createToken);
	public: void mEXCLAMATION(bool _createToken);
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
	public: void mDOT(bool _createToken);
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
};

#endif /*INC_IPTCfgLexer_hpp_*/

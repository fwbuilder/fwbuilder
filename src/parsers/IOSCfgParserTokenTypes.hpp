#ifndef INC_IOSCfgParserTokenTypes_hpp_
#define INC_IOSCfgParserTokenTypes_hpp_

/* $ANTLR 2.7.4: "iosacl.g" -> "IOSCfgParserTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API IOSCfgParserTokenTypes {
#endif
	enum {
		EOF_ = 1,
		NEWLINE = 4,
		IP = 5,
		WORD = 6,
		IOSVERSION = 7,
		NUMBER = 8,
		HOSTNAME = 9,
		STRING = 10,
		ACCESS_LIST = 11,
		INT_CONST = 12,
		EXTENDED = 13,
		PERMIT = 14,
		DENY = 15,
		ICMP = 16,
		TCP = 17,
		UDP = 18,
		P_EQ = 19,
		P_GT = 20,
		P_LT = 21,
		P_NEQ = 22,
		P_RANGE = 23,
		HOST = 24,
		IPV4 = 25,
		ANY = 26,
		LOG = 27,
		LOG_INPUT = 28,
		ESTABLISHED = 29,
		FRAGMENTS = 30,
		TIME_RANGE = 31,
		VLAN = 32,
		INTRFACE = 33,
		DESCRIPTION = 34,
		REMARK = 35,
		SHUTDOWN = 36,
		ACCESS_GROUP = 37,
		ADDRESS = 38,
		SECONDARY = 39,
		EXIT = 40,
		LINE_COMMENT = 41,
		STANDARD = 42,
		Whitespace = 43,
		HEX_CONST = 44,
		NEG_INT_CONST = 45,
		DIGIT = 46,
		HEXDIGIT = 47,
		NUMBER_SIGN = 48,
		PERCENT = 49,
		AMPERSAND = 50,
		APOSTROPHE = 51,
		OPENING_PAREN = 52,
		CLOSING_PAREN = 53,
		STAR = 54,
		PLUS = 55,
		COMMA = 56,
		MINUS = 57,
		DOT = 58,
		SLASH = 59,
		COLON = 60,
		SEMICOLON = 61,
		LESS_THAN = 62,
		EQUALS = 63,
		GREATER_THAN = 64,
		QUESTION = 65,
		COMMERCIAL_AT = 66,
		OPENING_SQUARE = 67,
		CLOSING_SQUARE = 68,
		CARET = 69,
		UNDERLINE = 70,
		OPENING_BRACE = 71,
		CLOSING_BRACE = 72,
		TILDE = 73,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_IOSCfgParserTokenTypes_hpp_*/

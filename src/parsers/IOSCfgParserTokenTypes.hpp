#ifndef INC_IOSCfgParserTokenTypes_hpp_
#define INC_IOSCfgParserTokenTypes_hpp_

/* $ANTLR 2.7.7 (20090306): "iosacl.g" -> "IOSCfgParserTokenTypes.hpp"$ */

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
		QUIT = 6,
		COMMUNITY_LIST = 7,
		WORD = 8,
		CERTIFICATE = 9,
		IOSVERSION = 10,
		NUMBER = 11,
		HOSTNAME = 12,
		STRING = 13,
		ACCESS_LIST = 14,
		INT_CONST = 15,
		EXTENDED = 16,
		PERMIT = 17,
		DENY = 18,
		ICMP = 19,
		TCP = 20,
		UDP = 21,
		P_EQ = 22,
		P_GT = 23,
		P_LT = 24,
		P_NEQ = 25,
		P_RANGE = 26,
		HOST = 27,
		IPV4 = 28,
		ANY = 29,
		LOG = 30,
		LOG_INPUT = 31,
		ESTABLISHED = 32,
		FRAGMENTS = 33,
		TIME_RANGE = 34,
		VLAN = 35,
		CONTROLLER = 36,
		INTRFACE = 37,
		DESCRIPTION = 38,
		REMARK = 39,
		SHUTDOWN = 40,
		ACCESS_GROUP = 41,
		ADDRESS = 42,
		SECONDARY = 43,
		EXIT = 44,
		LINE_COMMENT = 45,
		STANDARD = 46,
		Whitespace = 47,
		HEX_CONST = 48,
		NEG_INT_CONST = 49,
		DIGIT = 50,
		HEXDIGIT = 51,
		PIPE_CHAR = 52,
		NUMBER_SIGN = 53,
		PERCENT = 54,
		AMPERSAND = 55,
		APOSTROPHE = 56,
		OPENING_PAREN = 57,
		CLOSING_PAREN = 58,
		STAR = 59,
		PLUS = 60,
		COMMA = 61,
		MINUS = 62,
		DOT = 63,
		SLASH = 64,
		COLON = 65,
		SEMICOLON = 66,
		LESS_THAN = 67,
		EQUALS = 68,
		GREATER_THAN = 69,
		QUESTION = 70,
		COMMERCIAL_AT = 71,
		OPENING_SQUARE = 72,
		CLOSING_SQUARE = 73,
		CARET = 74,
		UNDERLINE = 75,
		OPENING_BRACE = 76,
		CLOSING_BRACE = 77,
		TILDE = 78,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_IOSCfgParserTokenTypes_hpp_*/

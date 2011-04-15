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
		ICMP = 7,
		TCP = 8,
		HOST = 9,
		COMMUNITY_LIST = 10,
		WORD = 11,
		CERTIFICATE = 12,
		IOSVERSION = 13,
		NUMBER = 14,
		HOSTNAME = 15,
		STRING = 16,
		ACCESS_LIST = 17,
		INT_CONST = 18,
		EXTENDED = 19,
		PERMIT = 20,
		DENY = 21,
		UDP = 22,
		P_EQ = 23,
		P_GT = 24,
		P_LT = 25,
		P_NEQ = 26,
		P_RANGE = 27,
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
		POINT_TO_POINT = 38,
		DESCRIPTION = 39,
		REMARK = 40,
		SHUTDOWN = 41,
		ACCESS_GROUP = 42,
		ADDRESS = 43,
		SECONDARY = 44,
		EXIT = 45,
		LINE_COMMENT = 46,
		STANDARD = 47,
		Whitespace = 48,
		HEX_CONST = 49,
		NEG_INT_CONST = 50,
		DIGIT = 51,
		HEXDIGIT = 52,
		PIPE_CHAR = 53,
		NUMBER_SIGN = 54,
		PERCENT = 55,
		AMPERSAND = 56,
		APOSTROPHE = 57,
		OPENING_PAREN = 58,
		CLOSING_PAREN = 59,
		STAR = 60,
		PLUS = 61,
		COMMA = 62,
		MINUS = 63,
		DOT = 64,
		SLASH = 65,
		COLON = 66,
		SEMICOLON = 67,
		LESS_THAN = 68,
		EQUALS = 69,
		GREATER_THAN = 70,
		QUESTION = 71,
		COMMERCIAL_AT = 72,
		OPENING_SQUARE = 73,
		CLOSING_SQUARE = 74,
		CARET = 75,
		UNDERLINE = 76,
		OPENING_BRACE = 77,
		CLOSING_BRACE = 78,
		TILDE = 79,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_IOSCfgParserTokenTypes_hpp_*/

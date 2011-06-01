#ifndef INC_IfconfigCfgParserTokenTypes_hpp_
#define INC_IfconfigCfgParserTokenTypes_hpp_

/* $ANTLR 2.7.7 (20090306): "ifconfig.g" -> "IfconfigCfgParserTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API IfconfigCfgParserTokenTypes {
#endif
	enum {
		EOF_ = 1,
		NEWLINE = 4,
		DOUBLE_NEWLINE = 5,
		LINE_COMMENT = 6,
		PRIORITY = 7,
		MEDIA = 8,
		STATUS = 9,
		WORD = 10,
		UP = 11,
		UPPER_BROADCAST = 12,
		UPPER_POINTOPOINT = 13,
		UPPER_LOOPBACK = 14,
		UPPER_NOARP = 15,
		UPPER_RUNNING = 16,
		LOOPBACK = 17,
		COLLISIONS = 18,
		COLON = 19,
		RX = 20,
		TX = 21,
		FLAGS = 22,
		EQUAL = 23,
		INT_CONST = 24,
		LINK = 25,
		ENCAP = 26,
		LLADDR = 27,
		MAC_ADDRESS = 28,
		HWADDR = 29,
		INET = 30,
		IPV4 = 31,
		NETMASK = 32,
		HEX_CONST = 33,
		BROADCAST = 34,
		ADDR = 35,
		BCAST = 36,
		MASK = 37,
		P_T_P = 38,
		INET6 = 39,
		IPV6 = 40,
		PERCENT = 41,
		PREFIXLEN = 42,
		SCOPEID = 43,
		SLASH = 44,
		SCOPE = 45,
		HOST = 46,
		GLOBAL = 47,
		GROUPS = 48,
		MTU = 49,
		LLADR = 50,
		Whitespace = 51,
		NUMBER = 52,
		NEG_INT_CONST = 53,
		HEX_DIGIT = 54,
		DIGIT = 55,
		NUM_3DIGIT = 56,
		NUM_HEX_4DIGIT = 57,
		NUMBER_ADDRESS_OR_WORD = 58,
		AMPERSAND = 59,
		STAR = 60,
		MINUS = 61,
		DOT = 62,
		QUESTION = 63,
		OPENING_PAREN = 64,
		CLOSING_PAREN = 65,
		OPENING_SQUARE = 66,
		CLOSING_SQUARE = 67,
		OPENING_BRACE = 68,
		CLOSING_BRACE = 69,
		LESS_THAN = 70,
		GREATER_THAN = 71,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_IfconfigCfgParserTokenTypes_hpp_*/

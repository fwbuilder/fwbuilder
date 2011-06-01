#ifndef INC_IfconfigBSDCfgParserTokenTypes_hpp_
#define INC_IfconfigBSDCfgParserTokenTypes_hpp_

/* $ANTLR 2.7.7 (20100319): "ifconfig_bsd.g" -> "IfconfigBSDCfgParserTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API IfconfigBSDCfgParserTokenTypes {
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
		COLON = 11,
		FLAGS = 12,
		EQUAL = 13,
		INT_CONST = 14,
		LLADDR = 15,
		MAC_ADDRESS = 16,
		INET = 17,
		IPV4 = 18,
		NETMASK = 19,
		HEX_CONST = 20,
		BROADCAST = 21,
		INET6 = 22,
		IPV6 = 23,
		PERCENT = 24,
		PREFIXLEN = 25,
		SCOPEID = 26,
		GROUPS = 27,
		LINK = 28,
		GLOBAL = 29,
		HOST = 30,
		ADDR = 31,
		BCAST = 32,
		P_T_P = 33,
		MASK = 34,
		SCOPE = 35,
		MTU = 36,
		ENCAP = 37,
		LOOPBACK = 38,
		UP = 39,
		UPPER_BROADCAST = 40,
		UPPER_POINTOPOINT = 41,
		UPPER_LOOPBACK = 42,
		UPPER_NOARP = 43,
		UPPER_RUNNING = 44,
		RX = 45,
		TX = 46,
		COLLISIONS = 47,
		INTERRUPT = 48,
		HWADDR = 49,
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
		SLASH = 63,
		QUESTION = 64,
		OPENING_PAREN = 65,
		CLOSING_PAREN = 66,
		OPENING_SQUARE = 67,
		CLOSING_SQUARE = 68,
		OPENING_BRACE = 69,
		CLOSING_BRACE = 70,
		LESS_THAN = 71,
		GREATER_THAN = 72,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_IfconfigBSDCfgParserTokenTypes_hpp_*/

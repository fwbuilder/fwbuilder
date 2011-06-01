#ifndef INC_IfconfigLinuxCfgParserTokenTypes_hpp_
#define INC_IfconfigLinuxCfgParserTokenTypes_hpp_

/* $ANTLR 2.7.7 (20100319): "ifconfig_linux.g" -> "IfconfigLinuxCfgParserTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API IfconfigLinuxCfgParserTokenTypes {
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
		INTERRUPT = 18,
		COLLISIONS = 19,
		COLON = 20,
		RX = 21,
		TX = 22,
		DOT = 23,
		INT_CONST = 24,
		LINK = 25,
		ENCAP = 26,
		HWADDR = 27,
		MAC_ADDRESS = 28,
		INET = 29,
		ADDR = 30,
		IPV4 = 31,
		BCAST = 32,
		MASK = 33,
		P_T_P = 34,
		INET6 = 35,
		IPV6 = 36,
		SLASH = 37,
		SCOPE = 38,
		HOST = 39,
		GLOBAL = 40,
		GROUPS = 41,
		FLAGS = 42,
		BROADCAST = 43,
		NETMASK = 44,
		PREFIXLEN = 45,
		SCOPEID = 46,
		MTU = 47,
		LLADR = 48,
		Whitespace = 49,
		HEX_CONST = 50,
		NUMBER = 51,
		NEG_INT_CONST = 52,
		HEX_DIGIT = 53,
		DIGIT = 54,
		NUM_3DIGIT = 55,
		NUM_HEX_4DIGIT = 56,
		NUMBER_ADDRESS_OR_WORD = 57,
		PERCENT = 58,
		AMPERSAND = 59,
		STAR = 60,
		MINUS = 61,
		EQUAL = 62,
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
#endif /*INC_IfconfigLinuxCfgParserTokenTypes_hpp_*/

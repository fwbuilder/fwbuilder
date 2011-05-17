/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

header "pre_include_hpp"
{
    // gets inserted before antlr generated includes in the header
    // file
#include "PFImporter.h"
}

header "post_include_hpp"
{
    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class PFImporter;
}

header "pre_include_cpp"
{
    // gets inserted before the antlr generated includes in the cpp
    // file
}

header "post_include_cpp"
{
    // gets inserted after the antlr generated includes in the cpp
    // file
#include <antlr/Token.hpp>
#include <antlr/TokenBuffer.hpp>
}

header
{
    // gets inserted after generated namespace specifications in the
    // header file. But outside the generated class.
}

options
{
	language="Cpp";
}


class PFCfgParser extends Parser;
options
{
    k = 2;

// when default error handler is disabled, parser errors cause
// exception and terminate parsing process. We can catch the exception
// and make the error appear in importer log, but import process
// terminates which is not always optimal
//
//    defaultErrorHandler = false;

// see http://www.antlr2.org/doc/options.html
}
{
// additional methods and members

    public:

    std::ostream *dbg;
    PFImporter *importer;

    /// Parser error-reporting function can be overridden in subclass
    virtual void reportError(const ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex)
    {
        importer->addMessageToLog("Parser error: " + ex.toString());
        std::cerr << ex.toString() << std::endl;
    }

    /// Parser error-reporting function can be overridden in subclass
    virtual void reportError(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser error: " + s);
        std::cerr << s << std::endl;
    }

    /// Parser warning-reporting function can be overridden in subclass
    virtual void reportWarning(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser warning: " + s);
        std::cerr << s << std::endl;
    }

}

cfgfile :
        (
            comment
        |
            pass_command
        |
            drop_command
        |
            timeout_command
        |
            unknown_command
        |
            NEWLINE
        )+
    ;

//****************************************************************
comment : LINE_COMMENT ;


//****************************************************************
timeout_command : TIMEOUT
        {
            consumeUntil(NEWLINE);
        }
    ;


//****************************************************************
unknown_command : WORD
        {
            consumeUntil(NEWLINE);
        }
    ;


//****************************************************************

pass_command: PASS
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "pass";
            *dbg << LT(1)->getLine() << ":" << " pass ";
        }
        rule_extended NEWLINE
        {
            importer->pushRule();
        }
    ;

drop_command: DROP
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "drop";
            *dbg << LT(1)->getLine() << ":" << " drop   ";
        }
        rule_extended NEWLINE
        {
            importer->pushRule();
        }
    ;

rule_extended: 
    ;

single_addr : (h:IPV4 | v6:IPV6)
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            if (h)
            {
                importer->tmp_a = h->getText();
                importer->tmp_nm = "255.255.255.255";
                *dbg << importer->tmp_a << " ";
            }
            if (v6)
            {
                importer->addMessageToLog(
                    QString("Warning: IPv6 import is not supported. "));
                consumeUntil(NEWLINE);
            }
        }
    ;

//****************************************************************

class PFCfgLexer extends Lexer;
options
{
    k = 3;
    // ASCII only
    charVocabulary = '\3'..'\377';
}

tokens
{
    EXIT = "exit";
    QUIT = "quit";

    NO = "no";

    INTRFACE = "interface";

    PASS = "pass";
    DROP = "drop";

// protocols 

    IP = "ip";
    ICMP = "icmp";
    ICMP6 = "icmp6";
    TCP  = "tcp";
    UDP  = "udp";

    AH = "ah";
    EIGRP = "eigrp";
    ESP = "esp";
    GRE = "gre";
    IGMP = "igmp";
    IGRP = "igrp";
    IPINIP = "ipinip";
    IPSEC = "ipsec";
    NOS = "nos";
    OSPF = "ospf";
    PCP = "pcp";
    PIM = "pim";
    PPTP = "pptp";
    RIP = "rip";
    SNP = "snp";

    HOST = "host";
    ANY  = "any";

    RANGE = "range";

    LOG = "log";

    LOG_LEVEL_ALERTS = "alerts";
    LOG_LEVEL_CRITICAL = "critical";
    LOG_LEVEL_DEBUGGING = "debugging";
    LOG_LEVEL_EMERGENCIES = "emergencies";
    LOG_LEVEL_ERRORS = "errors";
    LOG_LEVEL_INFORMATIONAL = "informational";
    LOG_LEVEL_NOTIFICATIONS = "notifications";
    LOG_LEVEL_WARNINGS = "warnings";
    LOG_LEVEL_DISABLE = "disable";
    LOG_LEVEL_INACTIVE = "inactive";

    TIMEOUT = "timeout";
}

LINE_COMMENT : "#" (~('\r' | '\n'))* NEWLINE ;

Whitespace :  ( '\003'..'\010' | '\t' | '\013' | '\f' | '\016'.. '\037' | '\177'..'\377' | ' ' )
        { _ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP;  } ;


//COMMENT_START : '!' ;

NEWLINE : ( "\r\n" | '\r' | '\n' ) { newline();  } ;

protected
INT_CONST:;

protected
HEX_CONST:;

protected
NUMBER:;

protected
NEG_INT_CONST:;

protected
DIGIT : '0'..'9'  ;

protected
HEXDIGIT : 'a'..'f' ;




NUMBER_ADDRESS_OR_WORD :
		(
            ( DIGIT ) =>
                (
                    ( (DIGIT)+ DOT (DIGIT)+ DOT (DIGIT)+ ) =>
                        ( (DIGIT)+ DOT (DIGIT)+ DOT (DIGIT)+ DOT (DIGIT)+ )
                        { _ttype = IPV4; }
                |
                    ( (DIGIT)+ DOT (DIGIT)+ )=> ( (DIGIT)+ DOT (DIGIT)+ )
                    { _ttype = NUMBER; }
                |
                    ( DIGIT )+ { _ttype = INT_CONST; }
                )
        |
            ( ( 'a'..'f' | '0'..'9' )+ COLON ) =>
                (
                    ( ( 'a'..'f' | '0'..'9' )+
                    ( COLON ( 'a'..'f' | '0'..'9' )* )+ )
                    { _ttype = IPV6; }
                )
        |
// making sure ',' '(' ')' are not part of WORD
            ( 'a'..'z' | 'A'..'Z' | '$' )
            ( '!'..'\'' | '*' | '+' | '-' | '.' | '/' | '0'..'9' | ':' |
              ';' | '<' | '=' | '>' |
              '?' | '@' | 'A'..'Z' | '\\' | '^' | '_' | '`' | 'a'..'z' )*
            { _ttype = WORD; }
        )
    ;

STRING : '"' (~'"')* '"';

PIPE_CHAR : '|';
NUMBER_SIGN : '#' ;
// DOLLAR : '$' ;
PERCENT : '%' ;
AMPERSAND : '&' ;
APOSTROPHE : '\'' ;
OPENING_PAREN : '(' ;
CLOSING_PAREN : ')' ;
STAR : '*' ;
PLUS : '+' ;
COMMA : ',' ;
MINUS : '-' ;
DOT : '.' ;
SLASH : '/' ;

COLON : ':' ;
SEMICOLON : ';' ;
LESS_THAN : '<' ;
EQUALS : '=' ;
GREATER_THAN : '>' ;
QUESTION : '?' ;
COMMERCIAL_AT : '@' ;

OPENING_SQUARE : '[' ;
CLOSING_SQUARE : ']' ;
CARET : '^' ;
UNDERLINE : '_' ;

OPENING_BRACE : '{' ;
CLOSING_BRACE : '}' ;
TILDE : '~' ;

EXLAMATION : '!';

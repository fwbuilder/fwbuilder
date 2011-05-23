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
            macro_definition
        |
            altq_command
        |
            queue_command
        |
            set_command
        |
            scrub_command
        |
            nat_command
        |
            rdr_command
        |
            binat_command
        |
            pass_command
        |
            block_command
        |
            timeout_command
        |
            unknown_command
        |
            NEWLINE
        )*
    ;

//****************************************************************
comment : LINE_COMMENT ;

//****************************************************************
macro_definition : WORD EQUAL
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
altq_command : ALTQ
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: import of 'altq' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
queue_command : QUEUE
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: import of 'queue' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
set_command : SET
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: import of 'set' commands has not been implemented yet."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
scrub_command : SCRUB
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: import of 'scrub' commands has not been implemented yet."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
nat_command : NAT
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: import of 'nat' commands has not been implemented yet."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
binat_command : BINAT
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: import of 'binat' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
rdr_command : RDR
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: import of 'rdr' commands has not been implemented yet."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
timeout_command : TIMEOUT
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: import of 'timeout' commands has not been implemented yet."));
            consumeUntil(NEWLINE);
        }
    ;


//****************************************************************
unknown_command : WORD
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            consumeUntil(NEWLINE);
        }
    ;


//****************************************************************

pass_command: PASS
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "pass";
            *dbg << LT(1)->getLine() << ":" << " pass ";
        }
        rule_extended NEWLINE
        {
            importer->setInterfaceAndDirectionForRuleSet(
                "", importer->iface, importer->direction);
            importer->pushRule();
        }
    ;

block_command: BLOCK
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "block";
            *dbg << LT(1)->getLine() << ":" << " block   ";
        }
        rule_extended NEWLINE
        {
            importer->setInterfaceAndDirectionForRuleSet(
                "", importer->iface, importer->direction);
            importer->pushRule();
        }
    ;

rule_extended:
        direction
        (logging)?
        (quick)?
        (intrface)?
        (
            (address_family)?
            (protospec)?
            hosts
            filteropts
        )?
    ;

direction: (IN | OUT)
        {
            importer->direction = LT(0)->getText();
        }
    ;

logging: LOG logopts
        {
            importer->logging = true;
        }
    ;

logopts:
        logopt
        (
            COMMA
            logopt
        )*
    ;

logopt: ALL | USER | TO WORD
    ;

quick: QUICK
        {
            importer->quick = true;
        }
    ;

intrface: ON WORD
        {
            importer->iface = LT(0)->getText();
            importer->newInterface(importer->iface);
        }
    ;

address_family: INET | INET6
        {
            importer->address_family = LT(0)->getText();
        }
    ;

protospec: PROTO
        (
            proto_name
        |
            proto_number
        |
            proto_list
        )
    ;

proto_name: (IP | ICMP | IGMP | TCP | UDP | RDP | RSVP | GRE | ESP | AH |
             EIGRP | OSPF | IPIP | VRRP | L2TP | ISIS )
        {
            importer->proto_list.push_back(LT(0)->getText());
        }
    ;

proto_number: INT_CONST
        {
            importer->proto_list.push_back(LT(0)->getText());
        }
    ;

proto_list: 
        OPENING_BRACE
        protospec
        (
            COMMA
            protospec
        )*
        CLOSING_BRACE
    ;

hosts:
        ALL
    |
        (
            (
                FROM
                ( src_hosts_part )?
                ( src_port_part )?
            )?
            (
                TO
                ( dst_hosts_part )?
                ( dst_port_part )?
            )
        )
    ;

src_hosts_part:
        (
            ANY
            {
                importer->tmp_group.push_back(
                    std::pair<std::string, std::string>("0.0.0.0", "0.0.0.0"));
            }
        |
            SELF
            {   
                importer->tmp_group.push_back(
                    std::pair<std::string, std::string>("self", "255.255.255.255"));
            }
        |
            host
        |
            host_list
        )
        {
            importer->src_neg = importer->tmp_neg;
            importer->src_group.splice(importer->src_group.begin(),
                                       importer->tmp_group);
        }
    ;

dst_hosts_part:
        (
            ANY
            {
                importer->tmp_group.push_back(
                    std::pair<std::string, std::string>("0.0.0.0", "0.0.0.0"));
            }
        |
            SELF
            {   
                importer->tmp_group.push_back(
                    std::pair<std::string, std::string>("self", "255.255.255.255"));
            }
        |
            host
        |
            host_list
        )
        {
            importer->dst_neg = importer->tmp_neg;
            importer->dst_group.splice(importer->src_group.begin(),
                                       importer->tmp_group);
        }

    ;

host :
        (
            EXCLAMATION
            {
                importer->tmp_neg = true;
            }
        )?
        (
            (h:IPV4 | v6:IPV6) (SLASH (nm:IPV4 | nm6:INT_CONST))?
            {
                if (v6)
                {
                    importer->addMessageToLog(
                        QString("Warning: IPv6 import is not supported. "));
                    consumeUntil(NEWLINE);
                } else
                {
                    std::string addr = "0.0.0.0";
                    std::string netm = "255.255.255.255";
                    if (h) addr = h->getText();
                    if (nm) netm = nm->getText();
                    importer->tmp_group.push_back(
                        std::pair<std::string, std::string>(addr, netm));
                }
            }
        |
            WORD
            {
                // This should be an interface name
                importer->tmp_group.push_back(
                    std::pair<std::string, std::string>(
                        LT(0)->getText(), "255.255.255.255"));
            }
        // Add table matching here
        )
    ;

host_list :
        OPENING_BRACE
        host
        (
            COMMA
            host
        )*
        CLOSING_BRACE
    ;

filteropts:
        filteropt
        (
            COMMA
            filteropt
        )*
    ;

filteropt:
        (state)?
        (queue)?
    ;

state:
        (
            NO
        |
            KEEP
        |
            MODULATE
        |
            SYNPROXY
        )
        {
            importer->state_op = LT(0)->getText();
        }
        STATE
    ;

queue:
        QUEUE
        (
            WORD { importer->queue += LT(0)->getText(); }
        |
            OPENING_PAREN { importer->queue += "("; }
            WORD          { importer->queue += LT(0)->getText(); }
            (
                COMMA     { importer->queue += ","; }
                WORD      { importer->queue += LT(0)->getText(); }
            )*
            CLOSING_PAREN { importer->queue += ")"; }
        )
    ;

//****************************************************************

src_port_part :
        PORT ( unary_op | binary_op | op_list )
        {
            importer->src_port_group.splice(importer->src_port_group.begin(),
                                            importer->tmp_port_group);
        }
    ;

dst_port_part :
        PORT ( unary_op | binary_op | op_list )
        {
            importer->dst_port_group.splice(importer->dst_port_group.begin(),
                                            importer->tmp_port_group);
        }
    ;

unary_op :
        {
            std::string op = "=";
        }
        (
            (
                EQUAL
            |
                NOT_EQUAL
            |
                LESS_THAN
            |
                LESS_OR_EQUAL_THAN
            |
                GREATER_THAN
            |
                GREATER_OR_EQUAL_THAN
            )
            {
                op = LT(0)->getText();
            }
        )?
        port_def
        {
            std::vector<std::string> tuple;
            tuple.push_back(op);
            tuple.push_back(importer->tmp_port_def);
            importer->tmp_port_group.push_back(tuple);
        }
    ;

binary_op :
        {
            std::string op;
            std::string arg1;
            std::vector<std::string> tuple;
        }
        port_def
        {
            arg1 = importer->tmp_port_def;
        }
        (
            EXCEPT_RANGE
        |
            INSIDE_RANGE
        |
            COLON
        )
        {
            op = LT(0)->getText();
        }
        port_def
        {
            tuple.push_back(op);
            tuple.push_back(arg1);
            tuple.push_back(importer->tmp_port_def);
            importer->tmp_port_group.push_back(tuple);
        }
    ;

port_def :
        ( WORD | INT_CONST )
        {
            importer->tmp_port_def = LT(0)->getText();
        }
    ;

op_list :
        OPENING_BRACE
        ( unary_op | binary_op )
        (
            COMMA
            ( unary_op | binary_op )
        )*
        CLOSING_BRACE
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
    BLOCK = "block";

    QUICK = "quick";

    IN = "in";
    OUT = "out";

    ON = "on";
    PROTO = "proto";

    FROM = "from";
    TO = "to";

    INET = "inet";
    INET6 = "inet6";

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
    IPIP = "ipip";
    IPSEC = "ipsec";
    NOS = "nos";
    OSPF = "ospf";
    PCP = "pcp";
    PIM = "pim";
    PPTP = "pptp";
    RIP = "rip";
    SNP = "snp";
    RDP = "rdp";
    RSVP = "rsvp";
    VRRP = "vrrp";
    L2TP = "l2tp";
    ISIS = "isis";

    HOST = "host";
    ANY  = "any";
    PORT = "port";

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

    ALTQ = "altq";
    SET = "set";
    SCRUB = "scrub";
    NAT = "nat";
    RDR = "rdr";
    BINAT = "binat";

    QUEUE = "queue";

    NOT_EQUAL = "!=" ;
    LESS_OR_EQUAL_THAN = "<=" ;
    GREATER_OR_EQUAL_THAN = ">=" ;
    EXCEPT_RANGE = "<>";
    INSIDE_RANGE = "><";

    TRANSLATE_TO = "->";

    STATE = "state";
    KEEP = "keep";
    MODULATE = "modulate";
    SYNPROXY = "synproxy";
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
                    (
                        ( 'a'..'f' | '0'..'9' )+
                        ( COLON ( 'a'..'f' | '0'..'9' )* )+
                    )
                    { _ttype = IPV6; }
                )
        |
// making sure ',' '(' ')' '=' '<' '>' '-' '+' are not part of WORD
// do not start WORD with '$' since we expand macros in PFImporterRun using regex.
            ( 'a'..'z' | 'A'..'Z' )
            ( '$' | '%' | '&' | '0'..'9' | ';' |
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
STAR : '*' ;
PLUS : '+' ;
COMMA : ',' ;
MINUS : '-' ;
DOT : '.' ;
SLASH : '/' ;

COLON : ':' ;
SEMICOLON : ';' ;
EQUAL : '=' ;

QUESTION : '?' ;
COMMERCIAL_AT : '@' ;

OPENING_PAREN : '(' ;
CLOSING_PAREN : ')' ;

OPENING_SQUARE : '[' ;
CLOSING_SQUARE : ']' ;

OPENING_BRACE : '{' ;
CLOSING_BRACE : '}' ;

CARET : '^' ;
UNDERLINE : '_' ;

TILDE : '~' ;

EXLAMATION : '!';

LESS_THAN : '<' ;
GREATER_THAN : '>' ;

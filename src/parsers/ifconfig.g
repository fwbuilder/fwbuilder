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
#include "IfconfigImporter.h"
}

header "post_include_hpp"
{
    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <iostream>
#include <sstream>

class IfconfigImporter;
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


class IfconfigCfgParser extends Parser;
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
    IfconfigImporter *importer;

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

cfgfile
    :
        (
            comment
        |
            bsd_interface_line
        |
            linux_interface_line
        |
            bsd_hwaddr_line
        |
            linux_hwaddr_line
        |
            bsd_inet_address
        |
            linux_inet_address
        |
            bsd_inet6_address
        |
            linux_inet6_address
        |
            groups
        |
            linux_interface_flags
        |
            linux_interface_statistics
        |
            unknown_line
        |
            NEWLINE
        |
            DOUBLE_NEWLINE
        )*
    ;

//****************************************************************
comment
    : LINE_COMMENT ;

//****************************************************************
unknown_line
    :
        (
            PRIORITY
        |
            MEDIA
        |
            STATUS
        |
            WORD
        )
        {
            consumeUntil(NEWLINE);
        }
    ;

linux_interface_flags
    :
        (
            UP
        |
            UPPER_BROADCAST
        |
            UPPER_POINTOPOINT
        |
            UPPER_LOOPBACK
        |
            UPPER_NOARP
        |
            UPPER_RUNNING
        |
            LOOPBACK
        )
        {
            consumeUntil(NEWLINE);
        }
    ;

linux_interface_statistics
    :
        (
            ( COLLISIONS COLON )
        |
            RX
        |
            TX
        )
        {
            consumeUntil(NEWLINE);
        }
    ;


//****************************************************************
//

// lo0: flags=8049<UP,LOOPBACK,RUNNING,MULTICAST> mtu 33200
// vic0: flags=8843<UP,BROADCAST,RUNNING,SIMPLEX,MULTICAST> mtu 1500
// enc0: flags=0<> mtu 1536
//
bsd_interface_line  { InterfaceSpec is; }
    :
        in:WORD COLON FLAGS EQUAL INT_CONST
        {
            // interface name and status
            is.name = in->getText();
            importer->newInterface(is);
            consumeUntil(NEWLINE);
        }
    ;

// lo        Link encap:Local Loopback  
// eth0      Link encap:Ethernet  HWaddr 00:0c:29:5f:12:cb  
// he-ipv6   Link encap:IPv6-in-IPv4  
// sit0      Link encap:IPv6-in-IPv4  
//
linux_interface_line  { InterfaceSpec is; }
    :
        in:WORD LINK ENCAP COLON WORD
        {
            // interface name and status
            is.name = in->getText();
            importer->newInterface(is);
        }
    ;

//****************************************************************

bsd_hwaddr_line
    :
        LLADDR addr:MAC_ADDRESS
        {
            importer->HwAddressForCurrentInterface(addr->getText());
        }
    ;

linux_hwaddr_line
    :
        HWADDR addr:MAC_ADDRESS
        {
            importer->HwAddressForCurrentInterface(addr->getText());
        }
    ;

//****************************************************************
//

// BSD:        inet 10.3.14.81 netmask 0xffffff00 broadcast 10.3.14.255
bsd_inet_address  { AddressSpec as; }
    :
        INET addr:IPV4 NETMASK netm:HEX_CONST BROADCAST bcast:IPV4
        {
            as.at = AddressSpec::INTERFACE_CONFIGURATION;
            as.address = addr->getText();
            as.netmask = netm->getText();
            as.broadcast = bcast->getText();
            importer->inetConfigurationForCurrentInterface(as);
        }
    ;

// Linux:      inet addr:10.10.11.11  Bcast:10.10.11.255  Mask:255.255.255.0
linux_inet_address  { AddressSpec as; }
    :
        INET ADDR COLON
        addr:IPV4
        {
            as.at = AddressSpec::INTERFACE_CONFIGURATION;
            as.address = addr->getText();
        }
        (
            ( ( BCAST COLON bcast:IPV4 )? MASK COLON netm:IPV4 )
            {
                as.netmask = netm->getText();
                if (bcast) as.broadcast = bcast->getText();
            }
        |
            ( P_T_P COLON IPV4 MASK COLON IPV4 )
            {
                    // we do not support p2p interfaces at this time
            }
        )
        {
            importer->inetConfigurationForCurrentInterface(as);
        }
    ;

//****************************************************************

// BSD:        inet6 fe80::20c:29ff:fe3f:ac3c%vic0 prefixlen 64 scopeid 0x1
bsd_inet6_address  { AddressSpec as; }
    :
        INET6 addr:IPV6 PERCENT WORD PREFIXLEN netm:INT_CONST SCOPEID HEX_CONST 
        {
            as.at = AddressSpec::INTERFACE_CONFIGURATION;
            as.address = addr->getText();
            as.netmask = netm->getText();
            importer->inet6ConfigurationForCurrentInterface(as);
        }
    ;

// Linux:      inet6 addr: fe80::20c:29ff:fe5f:12cb/64 Scope:Link
linux_inet6_address  { AddressSpec as; }
    :
        INET6 ADDR COLON addr:IPV6 SLASH netm:INT_CONST SCOPE COLON (HOST | LINK | GLOBAL | INT_CONST)
        {
            as.at = AddressSpec::INTERFACE_CONFIGURATION;
            as.address = addr->getText();
            as.netmask = netm->getText();
            importer->inet6ConfigurationForCurrentInterface(as);
        }
    ;


//****************************************************************
groups
    :
        GROUPS COLON groups_list NEWLINE
    ;

groups_list
    :
        WORD      { importer->addGroupToCurrentInterface(LT(0)->getText()); }
        (
            WORD  { importer->addGroupToCurrentInterface(LT(0)->getText()); }
        )*
    ;

//****************************************************************

class IfconfigCfgLexer extends Lexer;
options
{
    k = 3;
    // ASCII only
    charVocabulary = '\3'..'\377';
}

tokens
{
    INET = "inet";
    INET6 = "inet6";

    FLAGS = "flags";
    LINK = "Link";
    GLOBAL = "Global";
    HOST = "Host";

    ADDR = "addr";
    BCAST = "Bcast";
    P_T_P = "P-t-P";

    MASK = "Mask";

    BROADCAST = "broadcast";
    NETMASK = "netmask";

    PREFIXLEN = "prefixlen";
    SCOPEID = "scopeid";

    SCOPE = "Scope";

    GROUPS = "groups";

    MTU = "mtu";

    ENCAP = "encap";

    LOOPBACK = "Loopback";

    UP = "UP";
    UPPER_BROADCAST = "BROADCAST";
    UPPER_POINTOPOINT = "POINTOPOINT";
    UPPER_LOOPBACK = "LOOPBACK";
    UPPER_NOARP = "NOARP";
    UPPER_RUNNING = "RUNNING";

    RX = "RX";
    TX =  "TX";
    COLLISIONS = "collisions";

    PRIORITY = "priority";
    MEDIA = "media";
    STATUS = "status";
    HWADDR = "HWaddr";
    LLADR = "lladdr";
}

LINE_COMMENT : "#" (~('\r' | '\n'))* NEWLINE ;

Whitespace :  ( '\003'..'\010' | '\t' | '\013' | '\f' | '\016'.. '\037' | '\177'..'\377' | ' ' )
        { $setType(ANTLR_USE_NAMESPACE(antlr)Token::SKIP);  } ;




protected
INT_CONST:;

protected
HEX_CONST:;

protected
NUMBER:;

protected
NEG_INT_CONST:;

protected
COLON : ;

protected
HEX_DIGIT : ('0'..'9'|'a'..'f'|'A'..'F') ;

protected
DIGIT : '0'..'9'  ;

protected
NUM_3DIGIT: ('0'..'9') (('0'..'9') ('0'..'9')?)? ;

protected
NUM_HEX_4DIGIT: HEX_DIGIT ((HEX_DIGIT) ((HEX_DIGIT) (HEX_DIGIT)?)?)? ;

protected
MAC_ADDRESS : ;

NUMBER_ADDRESS_OR_WORD 
options {
    testLiterals = true;
}
    :
        ( NUM_3DIGIT '.' NUM_3DIGIT '.' ) =>
        (NUM_3DIGIT '.' NUM_3DIGIT '.' NUM_3DIGIT '.' NUM_3DIGIT)
        { $setType(IPV4); }

    //
    // MAC adress rule - exactly 6 COLON separated ints
    //
    |   (NUM_HEX_4DIGIT ':' NUM_HEX_4DIGIT ':'
         NUM_HEX_4DIGIT ':' NUM_HEX_4DIGIT ':'
         NUM_HEX_4DIGIT ':' NUM_HEX_4DIGIT ~(':') )=>
        (
            NUM_HEX_4DIGIT ':' NUM_HEX_4DIGIT ':'
            NUM_HEX_4DIGIT ':' NUM_HEX_4DIGIT ':'
            NUM_HEX_4DIGIT ':' NUM_HEX_4DIGIT
        ) { $setType(MAC_ADDRESS); }

    // IPv6 RULE
    |   (NUM_HEX_4DIGIT ':')=>
        (
            ((NUM_HEX_4DIGIT ':')+ ':')=>
            (
                (NUM_HEX_4DIGIT ':')+ ':'
                (NUM_HEX_4DIGIT (':' NUM_HEX_4DIGIT)*)?
            )   { $setType(IPV6); }

            |   NUM_HEX_4DIGIT (':' NUM_HEX_4DIGIT)+
                { $setType(IPV6); }

        )   { $setType(IPV6); }

    |   (':' ':' NUM_HEX_4DIGIT)=>
        ':' ':' NUM_HEX_4DIGIT (':' NUM_HEX_4DIGIT)* { $setType(IPV6); }

    |   ':' ':' { $setType(IPV6); }

    |   ':' { $setType(COLON); }

    | ( (DIGIT)+ '.' (DIGIT)+ )=> ( (DIGIT)+ '.' (DIGIT)+ )
        { $setType(NUMBER); }

    | ( DIGIT )+ { $setType(INT_CONST); }
            
    | '0' ('x' | 'X') HEX_DIGIT ( HEX_DIGIT )* { $setType(HEX_CONST); }

// making sure ',' '(' ')' '=' '<' '>' '+' are not part of WORD
// double quote " should be included, without it STRING does not match

    | ( 'a'..'z' | 'A'..'Z' )
        ( '"' | '$' | '%' | '&' | '-' | '0'..'9' | ';' |
          '?' | '@' | 'A'..'Z' | '\\' | '^' | '_' | '`' | 'a'..'z' )*
        { $setType(WORD); }
    ;


NEWLINE : ( "\r\n" | '\r' | '\n' ) { newline();  } ;

// DOUBLE_NEWLINE : NEWLINE NEWLINE ;

// protected
// NEWLINE :;

// protected
// DOUBLE_NEWLINE : ;

// NEWLINES
//    : 
//        ( ( "\r\n" | '\r' | '\n' ) ~('\r' | '\n') ) =>
//            ( "\r\n" | '\r' | '\n' ) { $setType(NEWLINE); }
//    |
//        ( "\r\n\r\n" | "\r\r" | "\n\n" ) { $setType(DOUBLE_NEWLINE); }
//    ;

PERCENT : '%' ;
AMPERSAND : '&' ;
STAR : '*' ;
MINUS : '-' ;
DOT : '.' ;
SLASH : '/' ;

EQUAL : '=';

QUESTION : '?' ;

OPENING_PAREN : '(' ;
CLOSING_PAREN : ')' ;

OPENING_SQUARE : '[' ;
CLOSING_SQUARE : ']' ;

OPENING_BRACE : '{' ;
CLOSING_BRACE : '}' ;

LESS_THAN : '<' ;
GREATER_THAN : '>' ;


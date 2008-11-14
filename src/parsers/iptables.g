/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id$

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
}
header "post_include_hpp"
{
    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class IPTImporter;
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

#include "../gui/IPTImporter.h"
#include "fwbuilder/TCPService.h"

#include <algorithm>

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


class IPTCfgParser extends Parser;
options
{
    k = 2;
//    defaultErrorHandler=false;
}
{
// additional methods and members

    public:
    
    std::ostream *dbg;
    IPTImporter *importer;
}

cfgfile :
        (
            comment
        |
            start_table
        |
            create_chain
        |
            add_rule
        |
            commit
        |
            NEWLINE
        )+
    ;

//****************************************************************

comment : LINE_COMMENT ;

//****************************************************************

commit : COMMIT
        {
            // push last rule
            importer->pushRule();
            *dbg << " COMMIT" << std::endl;
            // clear current table
            importer->current_table = "";
        }
    ;

//****************************************************************

start_table : STAR WORD
        {
            importer->current_table = LT(0)->getText();
            *dbg << "TABLE " << LT(0)->getText() << std::endl;
        }
    ;

//****************************************************************
chain_def : (INPUT | FORWARD | OUTPUT | PREROUTING | POSTROUTING | WORD)
    ;

//****************************************************************
//
// :INPUT DROP [2:104]
// :FORWARD DROP [0:0]
// :OUTPUT DROP [1:1492]
// :Cid4089E41E.0 - [0:0]
// :Cid45349B7222600.0 - [0:0]

create_chain : COLON chain_def
        {
            importer->newUnidirRuleSet(LT(0)->getText());
            *dbg << "NEW CHAIN " << LT(0)->getText() << std::endl;
        }
        (WORD | MINUS)
        (OPENING_SQUARE INT_CONST COLON INT_CONST CLOSING_SQUARE)?
    ;

//****************************************************************

add_rule : ADD_RULE chain_def
        {
            // push previous rule
            *dbg << std::endl;
            importer->pushRule();
            // start new one
            if (importer->current_table=="nat")
                importer->newNATRule();
            else
                importer->newPolicyRule();
            importer->current_chain = LT(0)->getText();
            *dbg << "add_rule: line=" << LT(0)->getLine()
                << " chain=" << LT(0)->getText();
        }
        ( ipt_option )+ NEWLINE
    ;

//****************************************************************
ipt_option :
        (
            module
        |
            src
        |
            dst
        |
            i_intf
        |
            o_intf
        |
            proto
        |
            target
        |
            fragm
        | 
            icmp_type_spec
        |
            basic_tcp_udp_port_spec
        |
            multiport_tcp_udp_port_spec
        |
            tcp_options
        |
            match_mark
        |
            match_limit
        |
            match_limit_burst
        |
            unknown_option
        )
    ;

//****************************************************************
unknown_option :
        (
            MINUS WORD
            {
                importer->markCurrentRuleBad(
                    std::string("Unknown option: -")+LT(0)->getText());
                *dbg << " UNKNOWN OPTION=-" << LT(0)->getText();
            }
            (unknown_parameter)?
        )
    | 
        (
            ( MINUS MINUS WORD )
            {
                importer->markCurrentRuleBad(
                    std::string("Unknown option: --")+LT(0)->getText());
                *dbg << " UNKNOWN OPTION=--" << LT(0)->getText();
            }
            (unknown_parameter)?
        )
    |
        (
            UNSUPPORTED_OPTION
            {
                importer->markCurrentRuleBad(
                    std::string("Unknown option: ")+LT(0)->getText());
                *dbg << " UNKNOWN OPTION=" << LT(0)->getText();
            }
            (unknown_parameter)?
        )
    ;

unknown_parameter :
        { std::string s; }
        (
            (
                (
                    DIGIT     {s+=LT(0)->getText();}
                |
                    INT_CONST {s+=LT(0)->getText();}
                )
                SLASH         {s+=LT(0)->getText();}
                WORD          {s+=LT(0)->getText();}
            )
            {
                importer->markCurrentRuleBad(
                    std::string("Unknown parameter: ")+s);
                *dbg << " UNKNOWN PARMETER=" << s;
            }
        )
    |
        (
            (DIGIT | INT_CONST | WORD)
            {
                importer->markCurrentRuleBad(
                    std::string("Unknown parameter: ")+LT(0)->getText());
                *dbg << " UNKNOWN PARMETER=" << LT(0)->getText();
            }
        )
    ;

//****************************************************************

module   : OPT_MODULE ( m_state | m_mport | m_icmp | m_tcp | m_udp | m_mark | m_limit | m_unknown_module) 
    ;

//****************************************************************
src      : OPT_SRC
        {
            *dbg << " SRC=";
        }
        (
            EXCLAMATION
            {
                importer->src_neg = true;
            }
        )?
        ( (WORD | IPV4)
            {
                importer->src_a = LT(0)->getText();
                *dbg << LT(0)->getText();
            }
            (SLASH (IPV4 | INT_CONST)
            {
                importer->src_nm = LT(0)->getText();
                *dbg << "/" << LT(0)->getText();
            }  )?
        )
    ;
                             
//****************************************************************
dst      : OPT_DST
        {
            *dbg << " DST=";
        }
        (
            EXCLAMATION
            {
                importer->dst_neg = true;
            }
        )?
        ( (WORD | IPV4)
            {
                importer->dst_a = LT(0)->getText();
                *dbg << LT(0)->getText();
            }
            (SLASH (IPV4 | INT_CONST)
            {
                importer->dst_nm = LT(0)->getText();
                *dbg << "/" << LT(0)->getText();
            }  )?
        )
    ;
                             
//****************************************************************
i_intf  : OPT_IN_INTF
        (
            EXCLAMATION
            {
                importer->intf_neg = true;
            }
        )?
        i:WORD
        {
            importer->i_intf = LT(0)->getText();
            *dbg << " I_INTF=" << i->getText();
        }
    ;
                            
//****************************************************************
o_intf : OPT_OUT_INTF
        (
            EXCLAMATION
            {
                importer->intf_neg = true;
            }
        )?
        i:WORD
        {
            importer->o_intf = LT(0)->getText();
            *dbg << " O_INTF=" << i->getText();
        }
    ;
                            
//****************************************************************
protocol_word : (TCP | UDP | ICMP | WORD | INT_CONST )
    ;

proto    : OPT_PROTO
        (
            EXCLAMATION
            {
                importer->srv_neg = true;
            }
        )? 
        protocol_word
        {
            std::string tmp_s = LT(0)->getText();
            importer->protocol.resize(tmp_s.size());
            std::transform(tmp_s.begin(), 
                           tmp_s.end(),
                           importer->protocol.begin(), 
                           ::tolower);
            *dbg << " PROTO=" << importer->protocol;
        }
    ;
                            
//****************************************************************
target   : OPT_TARGET   t:WORD
        {
            importer->target = LT(0)->getText();
            *dbg << " TARGET=" << t->getText();
        }
        ( target_options )*
    ;

//****************************************************************
target_options : 
        (
            REJECT_WITH WORD
            {
                importer->action_params["reject_with"] = LT(0)->getText();
                *dbg << " REJECT WITH=" << LT(0)->getText();
            }
        |
            LOG_PREFIX (WORD | STRING)
            {
                importer->action_params["log_prefix"] = LT(0)->getText();
                *dbg << " LOG PREFIX=" << LT(0)->getText();
            }
        |
            LOG_TCP_SEQ
            {
                importer->action_params["log_tcp_seq"] = LT(0)->getText();
                *dbg << " LOG TCP SEQUENCE=";
            }
        |
            LOG_TCP_OPT
            {
                importer->action_params["log_tcp_options"] = LT(0)->getText();
                *dbg << " LOG TCP OPTIONS=";
            }
        |
            LOG_IP_OPT
            {
                importer->action_params["log_ip_options"] = LT(0)->getText();
                *dbg << " LOG IP OPTIONS=";
            }
        |
            ULOG_PREFIX (WORD | STRING)
            {
                importer->action_params["log_prefix"] = LT(0)->getText();
                *dbg << " ULOG PREFIX=" << LT(0)->getText();
            }
        |
            LOG_LEVEL WORD
            {
                importer->action_params["log_level"] = LT(0)->getText();
                *dbg << " LOG LEVEL=" << LT(0)->getText();
            }
        |
            SET_MARK (INT_CONST | HEX_CONST)
            {
                importer->action_params["set_mark"] = LT(0)->getText();
                *dbg << " SET MARK=" << LT(0)->getText();
            }
        |
            SET_TOS HEX_CONST
            {
                importer->action_params["set_tos"] = LT(0)->getText();
                *dbg << " SET TOS=" << LT(0)->getText();
            }
        |
            SET_TOS WORD
            {
                importer->action_params["set_tos"] = LT(0)->getText();
                *dbg << " SET TOS=" << LT(0)->getText();
            }
        |
            SAVE_MARK
            {
                importer->action_params["connmark_save_mark"] = "--save-mark";
                *dbg << " SAVE MARK";
            }
        |
            RESTORE_MARK
            {
                importer->action_params["connmark_restore_mark"] = "--restore-mark";
                *dbg << " RESTORE MARK";
            }
        |
            CONTINUE
            {
                importer->action_params["route_continue"] = "--continue";
                *dbg << " CONTINUE";
            }
        |
            ROUTE_IIF WORD
            {
                importer->action_params["route_iif"] = LT(0)->getText();
                *dbg << " ROUTE_IIF=" << LT(0)->getText();
            }
        |
            ROUTE_OIF WORD
            {
                importer->action_params["route_oif"] = LT(0)->getText();
                *dbg << " ROUTE_OIF=" << LT(0)->getText();
            }
        |
            ROUTE_GW IPV4
            {
                importer->action_params["route_gw"] = LT(0)->getText();
                *dbg << " ROUTE_GW=" << LT(0)->getText();
            }
        |
            ROUTE_TEE
            {
                importer->action_params["route_tee"] = "--tee";
                *dbg << " ROUTE_TEE";
            }
        |
            TO_SOURCE
            {
                *dbg << " TO-SOURCE";
            }
            nat_spec
        |
            TO_DESTINATION
            {
                *dbg << " TO-DESTINATION";
            }
            nat_spec
        |
            TO_PORTS redirect_spec
        |
            TO_NETMAP
            {
                *dbg << " TO-NETMAP";
            }
            (
                IPV4
                {
                    importer->nat_addr1 = LT(0)->getText();
                    importer->nat_addr2 = LT(0)->getText();
                    *dbg << LT(0)->getText();
                }
                SLASH (IPV4 | INT_CONST)
                {
                    importer->nat_nm = LT(0)->getText();
                    *dbg << "/" << LT(0)->getText();
                }
            )
        |
            CLAMP_MSS
            {
                importer->action_params["clamp-mss-to-pmtu"] = "--clamp-mss-to-pmtu";
                *dbg << " TO-NETMAP";
            }
        )
    ;

//****************************************************************
nat_spec : 
        nat_addr_range
        (COLON nat_port_def_with_range)?
        {
            *dbg << " "
                << importer->nat_addr1
                << "-"
                << importer->nat_addr2
                << ":"
                << importer->nat_port_range_start
                << "-"
                << importer->nat_port_range_end;
        }
    ;

//****************************************************************
nat_addr_range : 
        IPV4
        {
            importer->nat_port_range_start = "";
            importer->nat_port_range_end = "";
            importer->nat_addr1 = LT(0)->getText();
            importer->nat_addr2 = LT(0)->getText();
        }
        (
            MINUS IPV4  { importer->nat_addr2 = LT(0)->getText(); }
        )?
    ;

//****************************************************************
redirect_spec : nat_port_def_with_range
        {
            *dbg << " TO-PORTS "
                << importer->nat_addr1
                << "-"
                << importer->nat_addr2
                << ":"
                << importer->nat_port_range_start
                << importer->nat_port_range_end;
        }
    ;

//****************************************************************
fragm    : OPT_FRAGM
        {
            importer->fragments = true;
            *dbg << " FRAGM";
        }
    ;

//****************************************************************

m_unknown_module : WORD
        {
            *dbg << " UNKNOWN MODULE=" << LT(0)->getText();
            importer->markCurrentRuleBad(
                std::string("Unknown module: ")+LT(0)->getText());
        }
    ;

//****************************************************************

state_word : ( INVALID | NEW | ESTABLISHED | RELATED )
    ;

m_state : M_STATE MATCH_STATE 
        {
            importer->current_state = "";
        }
        state_word
        {
            importer->current_state += LT(0)->getText();
        }
        (
            COMMA state_word
            {
                importer->current_state += std::string(",") + LT(0)->getText();
            }
        )?
        {
            *dbg << " STATE MATCH=" << importer->current_state;
        }

    ;

//****************************************************************

m_mark : M_MARK
        {
            *dbg << " MARK";
        }
    ;

//****************************************************************

match_mark : MATCH_MARK INT_CONST
        {
            importer->match_mark = LT(0)->getText();
            *dbg << " MATCH MARK " << LT(0)->getText();
        }
    ;

//****************************************************************

m_limit : M_LIMIT
        {
            *dbg << " LIMIT";
        }
    ;

//****************************************************************

match_limit : MATCH_LIMIT limit_rate
    ;

limit_rate :
        INT_CONST { importer->limit_val = LT(0)->getText(); }
        SLASH
        WORD      { importer->limit_suffix = LT(0)->getText(); }
        {
            *dbg << " MATCH LIMIT "
                << importer->limit_val << "/"
                << importer->limit_suffix;
        }
    ;

match_limit_burst : MATCH_LIMIT_BURST INT_CONST
        {
            importer->limit_burst = LT(0)->getText();
            *dbg << " LIMIT BURST " << LT(0)->getText();
        }
    ;
        
//****************************************************************

m_mport : M_MPORT
        {
            *dbg << " MULTIPORT";
        }
    ;

//****************************************************************

multiport_tcp_udp_port_spec :
        (
            ( (MATCH_SRC_MULTIPORT | MATCH_SRC_MULTIPORT_SHORT)
                {
                    importer->startSrcMultiPort();
                    *dbg << " SRC MULTIPORT=";
                }
                port_def_no_range
                {
                    importer->pushTmpPortSpecToSrcPortList();
                }
                ( COMMA port_def_no_range
                    {
                        importer->pushTmpPortSpecToSrcPortList();
                    } )+
            )
        |
            ( (MATCH_DST_MULTIPORT | MATCH_DST_MULTIPORT_SHORT)
                {
                    importer->startDstMultiPort();
                    *dbg << " DST MULTIPORT=";
                }
                port_def_no_range
                {
                    importer->pushTmpPortSpecToDstPortList();
                }
                ( COMMA port_def_no_range
                    {
                        importer->pushTmpPortSpecToDstPortList();
                    } )+
            )
        )
    ;

//****************************************************************

m_icmp : ICMP
        {
            importer->protocol = "icmp";
            *dbg << " ICMP";
        }
    ;

//****************************************************************
icmp_type_spec : MATCH_ICMP_TYPE
        (
            WORD
            {
                importer->icmp_spec = LT(0)->getText();
                *dbg << " ICMP_SPEC=" << LT(0)->getText();
            }
        |
            (
                INT_CONST
                {
                    importer->icmp_type = LT(0)->getText();
                    importer->icmp_code = "-1";
                    *dbg << " ICMP_TYPE=" << LT(0)->getText();
                }
                (
                    SLASH INT_CONST
                    {
                        importer->icmp_code = LT(0)->getText();
                        *dbg << " ICMP_CODE=" << LT(0)->getText();
                    }
                )?
            )
        )
    ;

//****************************************************************
// port definition that does not allow for port range
port_def_no_range : (WORD|INT_CONST)
        {
            importer->tmp_port_range_start = LT(0)->getText();
            importer->tmp_port_range_end = LT(0)->getText();
            *dbg << " PORT=" << LT(0)->getText();
        }
    ;

//****************************************************************
// port definition that allows for port range
port_def_with_range : 
        (WORD|INT_CONST)
        {
            importer->tmp_port_range_start = LT(0)->getText();
            importer->tmp_port_range_end = LT(0)->getText();
            *dbg << " PORT=" << LT(0)->getText();
        }
        (
            COLON (WORD|INT_CONST)
            {
                importer->tmp_port_range_end = LT(0)->getText();
                *dbg << ":" << LT(0)->getText();
            }
        )?
    ;

//****************************************************************
// nat port definition that allows for port range
// (uses '-' instead of ':')
nat_port_def_with_range : 
        (WORD|INT_CONST)
        {
            importer->nat_port_range_start = LT(0)->getText();
            importer->nat_port_range_end = LT(0)->getText();
            *dbg << " PORT=" << LT(0)->getText();
        }
        (
            MINUS (WORD|INT_CONST)
            {
                importer->nat_port_range_end = LT(0)->getText();
                *dbg << ":" << LT(0)->getText();
            }
        )?
    ;

//****************************************************************
basic_tcp_udp_port_spec :
        (MATCH_SRC_PORT | MATCH_SRC_PORT_SHORT)
        (
            EXCLAMATION
            {
                importer->srv_neg = true;
            }
        )?
        port_def_with_range
        {
            importer->pushTmpPortSpecToSrcPortList();
        }
    |
        (MATCH_DST_PORT | MATCH_DST_PORT_SHORT)
        (
            EXCLAMATION
            {
                importer->srv_neg = true;
            }
        )?
        port_def_with_range
        {
            importer->pushTmpPortSpecToDstPortList();
        }
    ;


//****************************************************************

m_udp : UDP
        {
            importer->protocol = "udp";
            *dbg << " UDP";
        }
    ;

//****************************************************************

m_tcp : TCP
        {
            importer->protocol = "tcp";
            *dbg << " TCP";
        }
    ;

//****************************************************************
// tcp options can follow "-p tcp", the "-m tcp" seems to be optional,
// at least in the older versions of iptables

tcp_options : 
    ( EXCLAMATION { importer->srv_neg = true; } )?
    ( syn | tcp_flags | tcp_option)
    ;

syn :   MATCH_SYN
        {
            importer->tcp_flags_mask.clear();
            importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::SYN);
            importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::RST);
            importer->tcp_flags_mask.push_back(libfwbuilder::TCPService::ACK);

            importer->tcp_flags_comp.clear();
            importer->tcp_flags_comp.push_back(libfwbuilder::TCPService::SYN);
        }
    ;

tcp_flag_word : 
        (
            SYN { importer->tmp_tcp_flag_code = libfwbuilder::TCPService::SYN; }
        |
            ACK { importer->tmp_tcp_flag_code = libfwbuilder::TCPService::ACK; }
        |
            FIN { importer->tmp_tcp_flag_code = libfwbuilder::TCPService::FIN; }
        |
            RST { importer->tmp_tcp_flag_code = libfwbuilder::TCPService::RST; }
        |
            URG { importer->tmp_tcp_flag_code = libfwbuilder::TCPService::URG; }
        |
            PSH { importer->tmp_tcp_flag_code = libfwbuilder::TCPService::PSH; }
        |
            ALL { importer->tmp_tcp_flag_code = 99; }
        |
            NONE { importer->tmp_tcp_flag_code = 98; }
        )
    ;

tcp_flags_list :
        {
            importer->tmp_tcp_flags_list.clear();
            importer->tmp_tcp_flag_code = 0;
        }
        tcp_flag_word
        {
            importer->tmp_tcp_flags_list.push_back(importer->tmp_tcp_flag_code);
        }
        (
            COMMA tcp_flag_word
            {
                importer->tmp_tcp_flags_list.push_back(
                    importer->tmp_tcp_flag_code);
            }
        )*
    ;

tcp_flags : MATCH_TCP_FLAGS
        tcp_flags_list
        {
            importer->tcp_flags_mask = importer->tmp_tcp_flags_list;
            importer->tmp_tcp_flags_list.clear();
        }
        tcp_flags_list
        {
            importer->tcp_flags_comp = importer->tmp_tcp_flags_list;
            importer->tmp_tcp_flags_list.clear();
            *dbg << " TCP FLAGS=";
            std::list<int>::iterator i;
            for (i=importer->tcp_flags_mask.begin();
                 i!=importer->tcp_flags_mask.end(); ++i)
                *dbg << *i << "|";
            *dbg << " ";
            for (i=importer->tcp_flags_comp.begin();
                 i!=importer->tcp_flags_comp.end(); ++i)
                *dbg << *i << "|";
        }
    ;

// --tcp-option is not supported in fwbuilder at this time
tcp_option : MATCH_TCP_OPTION (NUMBER | EXCLAMATION NUMBER)
    ;

//****************************************************************


class IPTCfgLexer extends Lexer;
options {
    k = 20;
    // ASCII only
    charVocabulary = '\3'..'\377';
}


tokens
{
    INPUT = "INPUT";
    FORWARD = "FORWARD";
    OUTPUT = "OUTPUT";
    PREROUTING = "PREROUTING";
    POSTROUTING = "POSTROUTING";

    INVALID = "INVALID";
    NEW = "NEW";
    ESTABLISHED = "ESTABLISHED";
    RELATED = "RELATED";

    COMMIT = "COMMIT";

    M_STATE = "state";
    M_MPORT = "multiport";
    M_MARK = "mark";
    M_LIMIT = "limit" ;

    ICMP = "icmp";
    TCP = "tcp";
    UDP = "udp";

    SYN = "SYN";
    ACK = "ACK";
    FIN = "FIN";
    RST = "RST";
    URG = "URG";
    PSH = "PSH";
    ALL = "ALL";
    NONE = "NONE";

}

LINE_COMMENT : "#" (~('\r' | '\n'))* NEWLINE ;

Whitespace :  ( '\003'..'\010' | '\t' | '\013' | '\f' | '\016'.. '\037' | '\177'..'\377' | ' ' )
        { _ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP;  } ;

NEWLINE : ( "\r\n" | '\r' | '\n' ) { newline(); resetText(); } ;

protected
INT_CONST:;

protected
HEX_CONST:;

protected
NEG_INT_CONST:;

protected
DIGIT : '0'..'9'  ;

protected
HEXDIGIT : '0'..'9' | 'A'..'F' | 'a'..'f';

NUMBER : 
		(
            ( (DIGIT)+ DOT (DIGIT)+ DOT (DIGIT)+ )=> ( (DIGIT)+ DOT (DIGIT)+ DOT (DIGIT)+ DOT (DIGIT)+ )
            { _ttype = IPV4; }
		|
            ( (DIGIT)+ DOT (DIGIT)+ )=> ( (DIGIT)+ DOT (DIGIT)+ )
        |
            ( DIGIT )+ { _ttype = INT_CONST; }
		|
            ( '0' 'x' ( HEXDIGIT )+ )  { _ttype = HEX_CONST; }   
        )
    ;

WORD : ( 'a'..'z' | 'A'..'Z' | '$' ) ( '!'..'+' | '-' | '.' | '/' | '0'..'9' | ':' | ';' | '<' | '=' | '>' | '?' | '@' | 'A'..'Z' | '^' | '_' | '`' | 'a'..'z'  )*
    ;

STRING : '"' (~'"')* '"';

// ------------------------------------------------------------------------
// I have to add these options even though I do not support them
//
protected
UNSUPPORTED_OPTION:;

//"--seconds" confuses lexer because it interprets it as "-" "-s" "econds"
SECONDS : "--seconds" { _ttype = UNSUPPORTED_OPTION; };

//"--seconds" confuses lexer because it interprets it as "-" "-s" "econds"
SET : "--set" { _ttype = UNSUPPORTED_OPTION; };

// "--rsource" also confuses lexer which expects "--reject"
RSOURCE : "--rsource" { _ttype = UNSUPPORTED_OPTION; };
// ------------------------------------------------------------------------

ADD_RULE : "-A" ;
MATCH_STATE : "--state" ;

MATCH_SRC_MULTIPORT : "--source-ports" ;
MATCH_DST_MULTIPORT : "--destination-ports" ;

MATCH_SRC_MULTIPORT_SHORT : "--sports" ;
MATCH_DST_MULTIPORT_SHORT : "--dports" ;

MATCH_SRC_PORT : "--source-port" ;
MATCH_DST_PORT : "--destination-port" ;

MATCH_SYN : "--syn" ;
MATCH_TCP_FLAGS : "--tcp-flags" ;
MATCH_TCP_OPTION : "--tcp-option" ;

MATCH_SRC_PORT_SHORT : "--sport" ;
MATCH_DST_PORT_SHORT : "--dport" ;

MATCH_ICMP_TYPE : "--icmp-type" ;

MATCH_MARK : "--mark" ;
MATCH_LIMIT : "--limit" ;
MATCH_LIMIT_BURST : "--limit-burst" ;

// ----------------------------------------------------------------
//  target options
REJECT_WITH : "--reject-with" ;
SET_MARK : "--set-mark" ;
SAVE_MARK : "--save-mark" ;
RESTORE_MARK : "--restore-mark" ;
SET_TOS : "--set-tos" ;
CONTINUE : "--continue" ;
ROUTE_IIF : "--iif" ;
ROUTE_OIF : "--oif" ;
ROUTE_GW : "--gw" ;
ROUTE_TEE : "--tee" ;

LOG_PREFIX : "--log-prefix" ;
LOG_LEVEL : "--log-level" ;
LOG_TCP_SEQ : "--log-tcp-sequence";
LOG_TCP_OPT : "--log-tcp-options";
LOG_IP_OPT : "--log-ip-options";

ULOG_PREFIX : "--ulog-prefix" ;
ULOG_QTHR : "--ulog-qthreshold" { _ttype = UNSUPPORTED_OPTION; };
ULOG_NLG : "--ulog-nlgroup" { _ttype = UNSUPPORTED_OPTION; };
ULOG_CPR : "--ulog-cprange" { _ttype = UNSUPPORTED_OPTION; };

TO_SOURCE : "--to-source" ;
TO_DESTINATION : "--to-destination" ;
TO_PORTS : "--to-ports" ;
TO_NETMAP : "--to" ;

CLAMP_MSS : "--clamp-mss-to-pmtu" ;

// ----------------------------------------------------------------
// these are the basic iptables options, not too many really
OPT_MODULE : "-m" ;
OPT_SRC : "-s" ;
OPT_DST : "-d" ;
OPT_IN_INTF : "-i" ;
OPT_OUT_INTF : "-o" ;
OPT_PROTO : "-p" ;
OPT_TARGET : "-j" ;
OPT_FRAGM : "-f" ;

EXCLAMATION : '!' ;

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


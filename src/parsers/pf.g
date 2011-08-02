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

#include <iostream>
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
        importer->error_tracker->registerError("Parser error: " + ex.toString());
        std::cerr << ex.toString() << std::endl;
    }

    /// Parser error-reporting function can be overridden in subclass
    virtual void reportError(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser error: " + s);
        importer->error_tracker->registerError("Parser error: " + s);
        std::cerr << s << std::endl;
    }

    /// Parser warning-reporting function can be overridden in subclass
    virtual void reportWarning(const ANTLR_USE_NAMESPACE(std)string& s)
    {
        importer->addMessageToLog("Parser warning: " + s);
        importer->error_tracker->registerError("Parser warning: " + s);
        std::cerr << s << std::endl;
    }

}

cfgfile :
        (
            comment
        |
            include_command
        |
            macro_definition
        |
            altq_rule
        |
            antispoof_rule
        |
            queue_rule
        |
            set_rule
        |
            scrub_rule
        |
            match_rule
        |
            table_rule
        |
            no_nat_rule 
        |
            nat_rule
        |
            rdr_rule
        |
            binat_rule
        |
            pass_rule
        |
            block_rule
        |
//            unknown_rule
//        |
            NEWLINE
        )*
    ;

//****************************************************************
comment :
        COMMENT_START
        {
            QStringList str;
            while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE &&
                   LA(1) != NEWLINE)
            {
                str << QString::fromUtf8(LT(1)->getText().c_str());
                consume();
            }
            importer->last_comment << str.join(" ");
        }
    ;

//****************************************************************
include_command : INCLUDE_COMMAND
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Error: import of 'include' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
macro_definition : WORD EQUAL
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
antispoof_rule : ANTISPOOF
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: import of 'antispoof' commands has not been implemented yet."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
altq_rule : ALTQ
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->error_tracker->registerError(
                QString("import of 'altq' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
queue_rule : QUEUE
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->error_tracker->registerError(
                QString("import of 'queue' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
set_rule : SET
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
        (
            set_timeout
        |
            set_ruleset_optimization
        |
            set_optimization
        |
            set_limit
        |
            set_loginterface
        |
            set_block_policy
        |
            set_state_policy
        |
            set_state_defaults
        |
            set_require_order
        |
            set_fingerprints
        |
            set_skip
        |
            set_debug
        |
            set_reassemble
        |
            set_hostid
        )
    ;

set_timeout
    :
        TIMEOUT ( timeout_def | timeout_def_list )
    ;

set_ruleset_optimization
    :
        "ruleset-optimization"
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Error: import of 'set ruleset-optimization' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

set_optimization
    :
        "optimization"
        (
            "aggressive"
        |
            "conservative"
        |
            "high-latency"
        |
            "normal"
        |
            "satellite"
        ) { importer->set_optimization = LT(0)->getText(); }
    ;

set_limit
    :
        "limit" ( limit_def | limit_def_list )
    ;

set_loginterface
    :
        "loginterface"
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Error: import of 'set loginterface' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

set_block_policy
    :
        "block-policy"
        (DROP | RETURN) { importer->set_block_policy = LT(0)->getText(); }
    ;

set_state_policy
    :
        "state-policy"
        ("if-bound" | "floating") { importer->set_state_policy = LT(0)->getText(); }
    ;

set_state_defaults
    :
        "state-defaults"
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Error: import of 'set state-defaults' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

set_require_order
    :
        "require-order"
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Error: import of 'set require-order' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

set_fingerprints
    :
        "fingerprints"
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Error: import of 'set fingerprints' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

set_skip
    :
        "skip" ON skip_def
    ;

skip_def
    :
        WORD { importer->set_skip_on.push_back(LT(0)->getText()); }

    |
        skip_list
    ;

skip_list
    :
        OPENING_BRACE
        WORD { importer->set_skip_on.push_back(LT(0)->getText()); }
        (
            ( COMMA )*
            WORD { importer->set_skip_on.push_back(LT(0)->getText()); }
        )*
        CLOSING_BRACE
    ;

set_debug
    :
        "debug" WORD
        {
            importer->set_debug = LT(0)->getText();
        }
    ;

set_reassemble
    :
        "reassemble"
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Error: import of 'set reassemble' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

set_hostid
    :
        "hostid"
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Error: import of 'set hostid' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;

/*
timeout = ( "tcp.first" | "tcp.opening" | "tcp.established" |
            "tcp.closing" | "tcp.finwait" | "tcp.closed" |
            "udp.first" | "udp.single" | "udp.multiple" |
            "icmp.first" | "icmp.error" |
            "other.first" | "other.single" | "other.multiple" |
            "frag" | "interval" | "src.track" |
            "adaptive.start" | "adaptive.end" ) number
*/
timeout_def { std::string timeout_name, timeout_value; }
    :
        (
            "tcp.first"
        |
            "tcp.opening"
        |
            "tcp.established"
        |
            "tcp.closing"
        |
            "tcp.finwait"
        |
            "tcp.closed"
        |
            "udp.first"
        |
            "udp.single"
        |
            "udp.multiple"
        |
            "icmp.first"
        |
            "icmp.error"
        |
            "other.first"
        |
            "other.single"
        |
            "other.multiple"
        |
            "frag"
        |
            "interval"
        |
            "src.track"
        |
            "adaptive.start"
        |
            "adaptive.end"
        )
        {
            timeout_name = LT(0)->getText();
        }
        INT_CONST
        {
            timeout_value = LT(0)->getText();
            importer->timeouts.push_back(
                std::pair<std::string, std::string>(timeout_name, timeout_value));
        }
    ;

timeout_def_list
    : 
        OPENING_BRACE
        timeout_def
        (
            ( COMMA )?
            timeout_def
        )*
        CLOSING_BRACE
    ;

limit_def { std::string limit_name, limit_value; }
    :
        (
            "frags"
        |
            "states"
        |
            "src-nodes"
        |
            "tables"
        |
            "tables-entries"
        )
        {
            limit_name = LT(0)->getText();
        }
        INT_CONST
        {
            limit_value = LT(0)->getText();
            importer->limits.push_back(
                std::pair<std::string, std::string>(limit_name, limit_value));
        }
    ;

limit_def_list
    : 
        OPENING_BRACE
        limit_def
        (
            ( COMMA )?
            limit_def
        )*
        CLOSING_BRACE
    ;


//****************************************************************
scrub_rule : SCRUB
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "scrub";
            *dbg << LT(1)->getLine() << ":" << " scrub ";
        }
        rule_extended 
        // do not call pushRule() for scrub rules because we configure
        // scrub parameters as firewall options
        NEWLINE
    ;

//****************************************************************
match_rule : MATCH
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "match";
            *dbg << LT(1)->getLine() << ":" << " match ";
        }
        rule_extended 
        {
            if ( ! importer->scrub_rule) importer->pushRule();
        }
        NEWLINE
    ;

//****************************************************************
table_rule :
        TABLE
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
        LESS_THAN
        name:WORD
        GREATER_THAN 
        (
            PERSIST
        ) ?
        (
            CONST_WORD
            {
                importer->addMessageToLog(
                    QString("Warning: attribute \"const\" will be dropped from table configuration since this attribute is not supported at this time"));
            }
        ) ?
        (
            COUNTERS
            {
                importer->addMessageToLog(
                    QString("Warning: attribute \"counters\" will be dropped from table configuration since this attribute is not supported at this time"));
            }
        )?
        (
            FILE file:STRING
            {
                importer->newAddressTableObject(
                    name->getText(), file->getText());
            }
        |
            OPENING_BRACE
            tableaddr_spec
            (
                ( COMMA )?
                tableaddr_spec
            )*
            CLOSING_BRACE
            {
                importer->newAddressTableObject(
                    name->getText(), importer->tmp_group);
            }
        |
            NEWLINE
            {
                // Special case: table definition without file name or list of addresses.
                // Create run-time AddressTable object with name but no file spec.
                importer->newAddressTableObject(name->getText(), "");
            }
        )
    ;

tableaddr_spec { AddressSpec as; } :
        ( EXLAMATION { as.neg = true; } )?
        (
            WORD
            {
                // interface name or domain/host name
                as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
                as.address = LT(0)->getText();
            }
            (
                COLON
                (
                    NETWORK
                    {
                        as.at = AddressSpec::INTERFACE_NETWORK;
                    }
                |
                    BROADCAST
                    {
                        as.at = AddressSpec::INTERFACE_BROADCAST;
                    }
                |
                    PEER
                    {
                        importer->error_tracker->registerError(
                            QString("import of 'interface:peer' is not supported."));
                    }
                |
                    INT_CONST
                    {
                        importer->error_tracker->registerError(
                            QString("import of 'interface:0' is not supported."));
                    }
                )
            )?
        |
            SELF
            {
                as.at = AddressSpec::SPECIAL_ADDRESS;
                as.address = "self";
            }
        |
            // match NUMBER and INT_CONST to account for 10/8, 172.16/12 cases.
            ( IPV4 | NUMBER | INT_CONST )
            {
                as.at = AddressSpec::HOST_ADDRESS;
                as.address = LT(0)->getText();
            }
            (
                SLASH 
                {
                    as.at = AddressSpec::NETWORK_ADDRESS;
                }
                ( IPV4 | INT_CONST )
                {
                    as.netmask = LT(0)->getText(); 
                }
            )?
        )
        {
            importer->tmp_group.push_back(as);
        }
    ;

//****************************************************************
no_nat_rule :
        NO
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newNATRule();
            importer->action = "nonat";
            *dbg << LT(1)->getLine() << ":" << " nonat ";
        }
        (
            nat_rule
        |
            rdr_rule
        )
    ;

//****************************************************************
nat_rule :
        NAT
        {
            if ( importer->action != "nonat" )
            {
                importer->clear();
                importer->setCurrentLineNumber(LT(0)->getLine());
                importer->newNATRule();
                importer->action = "nat";
                *dbg << LT(1)->getLine() << ":" << " nat ";
            }
        }
        (
            PASS 
            {
                importer->error_tracker->registerError(
                    QString("import of 'nat pass' commands is not supported."));
            }
            ( logging )?
        )?
        ( intrface )?
        ( address_family )?
        ( protospec )?
        hosts
        ( tagged )?
        (
            tag_clause
            {
                importer->error_tracker->registerError(
                    QString("import of 'nat ... tag' commands is not supported."));
            }
        )?
        (
            MINUS GREATER_THAN
            ( redirhost | redirhost_list )
            {
                importer->nat_group = importer->tmp_group;
            }
            (
                portspec
                {
                    importer->nat_port_group = importer->tmp_port_group;
                }
            )?
            ( pooltype )?
            (
                STATIC_PORT { importer->nat_rule_opt_2 = "static-port"; }
            )?
        )?
        {
            importer->pushRule();
        }
        NEWLINE
    ;

//****************************************************************
rdr_rule :
        RDR
        {
            if ( importer->action != "nonat" )
            {
                importer->clear();
                importer->setCurrentLineNumber(LT(0)->getLine());
                importer->newNATRule();
                importer->action = "rdr";
                *dbg << LT(1)->getLine() << ":" << " rdr ";
            }
        }
        (
            PASS 
            {
                importer->error_tracker->registerError(
                    QString("import of 'nat pass' commands is not supported."));
            }
            ( logging )?
        )?
        ( intrface )?
        ( address_family )?
        ( protospec )?
        hosts
        ( tagged )?
        (
            tag_clause
            {
                importer->error_tracker->registerError(
                    QString("import of 'nat ... tag' commands is not supported."));
            }
        )?
        (
            MINUS GREATER_THAN
            ( redirhost | redirhost_list )
            {
                importer->nat_group = importer->tmp_group;
            }
            (
                portspec
                {
                    importer->nat_port_group = importer->tmp_port_group;
                }
            )?
            ( pooltype )?
        )?
        {
            importer->pushRule();
        }
        NEWLINE
    ;

// redirhost = address [ "/" mask-bits ]
// address  = ( interface-name | interface-group |
//           "(" ( interface-name | interface-group ) ")" |
//            hostname | ipv4-dotted-quad | ipv6-coloned-hex )
//
redirhost  { AddressSpec as; } :
        (
            IPV4
            {
                as.at = AddressSpec::HOST_ADDRESS;
                as.address = LT(0)->getText();
            }
            (
                SLASH
                {
                    as.at = AddressSpec::NETWORK_ADDRESS;
                }
                ( IPV4 | INT_CONST )
                {
                    as.netmask = LT(0)->getText(); 
                }
            )?
        |
            OPENING_PAREN
            WORD
            {
                // interface name or domain/host name
                as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
                as.address = LT(0)->getText();
            }
            CLOSING_PAREN
        |
            WORD
            {
                // interface name or domain/host name
                as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
                as.address = LT(0)->getText();
            }
        )
        {
            importer->tmp_group.push_back(as);
        }
    ;

redirhost_list : 
        OPENING_BRACE
        redirhost
        (
            ( COMMA )?
            redirhost
        )*
        CLOSING_BRACE
    ;

//  portspec = "port" ( number | name ) [ ":" ( "*" | number | name ) ]
//
//
//  rdr   The packet is redirected to another destination and possibly a dif-
//        ferent port.  rdr rules can optionally specify port ranges instead
//        of single ports.  rdr ... port 2000:2999 -> ... port 4000 redirects
//        ports 2000 to 2999 (inclusive) to port 4000.  rdr ... port
//        2000:2999 -> ... port 4000:* redirects port 2000 to 4000, 2001 to
//        4001, ..., 2999 to 4999.
//
portspec { PortSpec ps; } :
        PORT
        (
            port_def 
            {
                ps.port1 = importer->tmp_port_def;
                ps.port2 = ps.port1;
                ps.port_op = "=";
            }
        |
//  lexer matches port range (1000:1010) as IPv6, see rule
// NUMBER_ADDRESS_OR_WORD. Combination "1000:*" comes as IPV6 STAR
            IPV6
            {
                ps.setFromPortRange(LT(0)->getText());
            }
            (
                STAR     { ps.port2 = "65535"; }
            )?
        )
        {
            importer->tmp_port_group.push_back(ps);
        }
    ;

// pooltype = ( "bitmask" | "random" |
//              "source-hash" [ ( hex-key | string-key ) ] |
//              "round-robin" ) [ sticky-address ]
//
// Note that as of v4.2 we can not generate optinal parameters for the
// "source-hash" pooltype. "sticky-address" is not supported either.
//
pooltype :
        (
            BITMASK       { importer->pooltype_opt = "bitmask"; }
        |
            RANDOM        { importer->pooltype_opt = "random"; }
        |
            SOURCE_HASH   { importer->pooltype_opt = "source-hash"; }
            (
                HEX_KEY
                {
                    importer->error_tracker->registerError(
                        QString("import of commands with pool type 'source-hash hex-key' "
                                "option is not supported"));
                }
            |
                STRING_KEY
                {
                    importer->error_tracker->registerError(
                        QString("import of commands with pool type 'source-hash string-key' "
                                "option is not supported"));
                }
            )?
        |
            ROUND_ROBIN   { importer->pooltype_opt = "round-robin"; }
        )
        ( STICKY_ADDRESS )?
    ;

//****************************************************************
binat_rule : BINAT
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->error_tracker->registerError(
                QString("import of 'binat' commands is not supported."));
            consumeUntil(NEWLINE);
        }
    ;


//****************************************************************
//unknown_rule : WORD
//        {
//            importer->clear();
//            importer->setCurrentLineNumber(LT(0)->getLine());
//            consumeUntil(NEWLINE);
//        }
//    ;


//****************************************************************

pass_rule : PASS
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "pass";
            *dbg << LT(1)->getLine() << ":" << " pass ";
        }
        rule_extended 
        {
            importer->pushRule();
        }
        NEWLINE
    ;

block_rule : BLOCK
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "block";
            *dbg << LT(1)->getLine() << ":" << " block   ";
        }
        ( block_return )?
        rule_extended 
        {
            importer->pushRule();
        }
        NEWLINE
    ;

block_return :
        (
            DROP         { importer->block_action_params.push_back("drop"); }
        |
            RETURN       { importer->block_action_params.push_back("return"); }
        |
            RETURN_RST   { importer->block_action_params.push_back("return-rst"); }
            (
                TTL INT_CONST
                {
                    importer->error_tracker->registerError(
                        QString("Import of \"block return-rst ttl number\" is not supported. "));
                }
            )?
        |
            RETURN_ICMP  { importer->block_action_params.push_back("return-icmp"); }
            (
                OPENING_PAREN
                ( icmp_code_by_name | INT_CONST )
                { importer->block_action_params.push_back(LT(0)->getText()); }
                (
                    COMMA
                    ( icmp_code_by_name | INT_CONST )
                    {
                        importer->error_tracker->registerError(
                            QString("Import of \"block return-icmp (icmp_code, icmp6_code)\" is not supported"));
                    }
                )?
                CLOSING_PAREN
            )?
        |
            RETURN_ICMP6 
            {
                importer->error_tracker->registerError(
                    QString("Import of \"block return-icmp6\" is not supported"));
                importer->block_action_params.push_back("return-icmp");
            }
        )
    ;

rule_extended :
        ( direction )?
        ( quick_or_log )?
        ( intrface )?
        ( route )?
        ( address_family )?
        ( protospec )?
        ( hosts )?
        ( filteropts )?
        ( route )?
    ;

direction : ( IN_WORD | OUT_WORD )
        {
            importer->direction = LT(0)->getText();
        }
    ;

// looks like both "block log quick" and "block quick log" are legitimate
quick_or_log : 
        (
            LOG (logopts)? { importer->logging = true; }
            ( QUICK { importer->quick = true; } )?
        |
            QUICK { importer->quick = true; }
            ( LOG (logopts)? { importer->logging = true; } )?
        )
    ;

logging :
        LOG (logopts)? { importer->logging = true; }
    ;

logopts :
        OPENING_PAREN
        logopt
        (
            COMMA    { importer->logopts += ","; }
            logopt
        )*
        CLOSING_PAREN
    ;

logopt : ALL | USER | TO WORD
        {
            importer->logopts += LT(0)->getText();
        }
    ;

intrface : ON ( ifspec | interface_list )
    ;

ifspec { InterfaceSpec is; } :
        ( EXLAMATION { is.neg = true; } )?
        WORD
        {
            is.name = LT(0)->getText();
            importer->iface_group.push_back(is);
            importer->newInterface(is.name);
        }
    ;

interface_list :
        OPENING_BRACE
        ifspec
        (
            ( COMMA )?
            ifspec
        )*
        CLOSING_BRACE
    ;


address_family : INET | INET6
        {
            importer->address_family = LT(0)->getText();
        }
    ;

protospec : PROTO proto_def
    ;

proto_def :
        (
            proto_name
        |
            proto_number
        |
            proto_list
        )
    ;

proto_name : (IP | ICMP | IGMP | TCP | UDP | RDP | RSVP | GRE | ESP_WORD | AH |
             EIGRP | OSPF | IPIP | VRRP | L2TP | ISIS )
        {
            importer->proto_list.push_back(LT(0)->getText());
        }
    ;

proto_number : INT_CONST
        {
            importer->proto_list.push_back(LT(0)->getText());
        }
    ;

proto_list : 
        OPENING_BRACE
        proto_def
        (
            ( COMMA )?
            proto_def
        )*
        CLOSING_BRACE
    ;

hosts :
        ALL
        {
            importer->src_group.push_back(
                AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
            importer->dst_group.push_back(
                AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
        }
    |
        ( hosts_from )?  ( hosts_to )?
    ;

hosts_from :
        FROM src_hosts_part ( src_port_part )?
    ;

hosts_to :
        TO dst_hosts_part ( dst_port_part )?
    ;

src_hosts_part :
        (
            common_hosts_part
        |
            URPF_FAILED
            {
                importer->tmp_group.push_back(
                    AddressSpec(AddressSpec::SPECIAL_ADDRESS, false,
                                "urpf-failed", ""));
            }
        )
        {
            importer->src_neg = importer->tmp_neg;
            importer->src_group.splice(importer->src_group.begin(),
                                       importer->tmp_group);
        }
    ;

dst_hosts_part :
        common_hosts_part
        {
            importer->dst_neg = importer->tmp_neg;
            importer->dst_group.splice(importer->dst_group.begin(),
                                       importer->tmp_group);
        }
    ;

common_hosts_part :
        ANY
        {
            importer->tmp_group.push_back(
                AddressSpec(AddressSpec::ANY, false, "0.0.0.0", "0.0.0.0"));
        }
    |
        NO_ROUTE
        {
            importer->tmp_group.push_back(
                AddressSpec(AddressSpec::SPECIAL_ADDRESS, false, "no-route", ""));
        }
    |
        host
    |
        host_list
    ;

host { AddressSpec as; } :
        ( EXLAMATION { as.neg = true; } )?
        (
            ( WORD | MACRO )
            {
                // interface name or domain/host name
                as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
                as.address = LT(0)->getText();
            }
            (
                COLON
                (
                    NETWORK
                    {
                        as.at = AddressSpec::INTERFACE_NETWORK;
                    }
                |
                    BROADCAST
                    {
                        as.at = AddressSpec::INTERFACE_BROADCAST;
                    }
                |
                    PEER
                    {
                        importer->error_tracker->registerError(
                            QString("import of 'interface:peer' is not supported."));
                    }
                |
                    INT_CONST
                    {
                        importer->error_tracker->registerError(
                            QString("import of 'interface:0' is not supported."));
                    }
                )
            )?
        |
            SELF
            {   
                as.at = AddressSpec::SPECIAL_ADDRESS;
                as.address = "self";
            }
        |
            IPV6
            {
                importer->error_tracker->registerError(
                    QString("IPv6 import is not supported. "));
                consumeUntil(NEWLINE);
            }
        |
            IPV4
            {
                as.at = AddressSpec::HOST_ADDRESS;
                as.address = LT(0)->getText();
            }
            (
                SLASH 
                {
                    as.at = AddressSpec::NETWORK_ADDRESS;
                }
                ( IPV4 | INT_CONST )
                {
                    as.netmask = LT(0)->getText(); 
                }
            )?
        |
            LESS_THAN tn:WORD GREATER_THAN
            {
                as.at = AddressSpec::TABLE;
                as.address = tn->getText();
            }
        |
            OPENING_PAREN in:WORD CLOSING_PAREN
            {
                // interface name or domain/host name
                as.at = AddressSpec::INTERFACE_OR_HOST_NAME;
                as.address = in->getText();
            }
        )
        {
            importer->tmp_group.push_back(as);
        }
    ;

host_list :
        OPENING_BRACE
        host
        (
            ( COMMA )?
            host
        )*
        CLOSING_BRACE
    ;

// ************************************************************************
route :
        route_to | reply_to | dup_to
    ;

route_to :
        ROUTE_TO ( routehost | routehost_list ) ( pooltype )?
        {
            importer->route_type = PFImporter::ROUTE_TO;
        }
    ;

reply_to :
        REPLY_TO ( routehost | routehost_list ) ( pooltype )?
        {
            importer->route_type = PFImporter::REPLY_TO;
        }
    ;

dup_to :
        DUP_TO ( routehost | routehost_list ) ( pooltype )?
        {
            importer->route_type = PFImporter::DUP_TO;
        }
    ;

routehost { RouteSpec rs; } :
        OPENING_PAREN
        WORD { rs.iface = LT(0)->getText(); }
        (h:IPV4 | v6:IPV6) (SLASH (nm:IPV4 | nm6:INT_CONST))?
        {
            if (v6)
            {
                importer->error_tracker->registerError(
                    QString("IPv6 import is not supported. "));
                consumeUntil(NEWLINE);
            } else
            {
                if (h) rs.address = h->getText();
                if (nm) rs.netmask = nm->getText();
                importer->route_group.push_back(rs);
            }
        }
        CLOSING_PAREN
    ;

routehost_list :
        OPENING_BRACE
        routehost
        (
            ( COMMA )?
            routehost
        )*
        CLOSING_BRACE
    ;

// ************************************************************************
filteropts :
        filteropt
        (
            ( COMMA )?
            filteropt
        )*
    ;

filteropt :
        user_match
    |
        group_match
    |
        tcp_flags
    |
        icmp_type
    |
        icmp6_type
    |
        tagged
    |
        tag_clause
    |
        state
    |
        queue
    |
        label
    |
        match_rule_scrub_options
    |
        scrub_options
    ;

//************************************************************************
user_match
    :
        USER ( user_group_op | user_group_op_list )
        {
            importer->addMessageToLog(
                QString("Error: import of 'user' match is not supported."));
        }
    ;

group_match
    :
        GROUP ( user_group_op | user_group_op_list )
        {
            importer->addMessageToLog(
                QString("Error: import of 'group' match is not supported."));
        }
    ;

user_group_op
    :
        (
            unary_op ( WORD | INT_CONST )
        |
            ( WORD | INT_CONST )
            (
                binary_op
                ( WORD | INT_CONST )
            )?
        )
    ;

user_group_op_list :
        OPENING_BRACE
        user_group_op
        (
            ( COMMA )?
            user_group_op
        )*
        CLOSING_BRACE
    ;

//************************************************************************
match_rule_scrub_options
    :
        SCRUB scrub_options
    ;

scrub_options
    :
        ( scrub_option | scrub_option_list )
    ;

scrub_option_list
    :
        OPENING_PAREN
        scrub_option
        (
            ( COMMA )?
            scrub_option
        )*
        CLOSING_PAREN
    ;

scrub_option
    :
    (
        "fragment"
        (
            "reassemble"
        |
            "crop"
        |
            "drop-ovl"
        )
        {
            importer->scrub_options.push_back(
                str_tuple("fragment", LT(0)->getText()));
            importer->scrub_rule = true;
        }
    |
        "reassemble" TCP
        {
            importer->scrub_options.push_back(
                str_tuple("reassemble", "tcp"));
            importer->scrub_rule = true;
        }
    |
        "no-df"
        {
            importer->scrub_options.push_back(
                str_tuple(LT(0)->getText(), ""));
            importer->scrub_rule = true;
        }
    |
        "min-ttl" INT_CONST
        {
            importer->scrub_options.push_back(
                str_tuple("min-ttl", LT(0)->getText()));
            importer->scrub_rule = true;
        }
    |
        "max-mss" INT_CONST
        {
            importer->scrub_options.push_back(
                str_tuple("max-mss", LT(0)->getText()));
            importer->scrub_rule = true;
        }
    |
        "random-id"
        {
            importer->scrub_options.push_back(
                str_tuple(LT(0)->getText(), ""));
            importer->scrub_rule = true;
        }
    )
    ;

tcp_flags
    :
    FLAGS
    (
        ANY
        {
            importer->flags_check = "none";
            importer->flags_mask = "none";
        }
    |
        ( check:WORD )? SLASH ( mask:WORD )?
        {
            if (check)
                importer->flags_check = check->getText();
            else
                importer->flags_check = "any";
            if (mask)
                importer->flags_mask = mask->getText();
            else
                importer->flags_mask = "all";
        }
    )
    ;

icmp_type :
        ICMP_TYPE
        (
            icmp_type_code
        |
            icmp_list
        )
    ;

icmp_type_code { IcmpSpec is; } :
        (
            icmp_type_by_name { is.icmp_type_name = LT(0)->getText(); }
        |   
            INT_CONST     { is.icmp_type_int = LT(0)->getText();  }
        )

        (
            ICMP_CODE
            (
                icmp_code_by_name { is.icmp_code_name = LT(0)->getText(); }
            |
                INT_CONST { is.icmp_code_int = LT(0)->getText();  }
            )
        )?
        {
            importer->icmp_type_code_group.push_back(is);
        }
    ;

icmp_type_by_name
    :
        ( 
            "echorep"
        |
            "unreach"
        |
            "squench"
        |
            "redir"
        |
            "althost"
        |
            "echoreq"
        |
            "routeradv"
        |
            "routersol"
        |
            "timex"
        |
            "paramprob"
        |
            "timereq"
        |
            "timerep"
        |
            "inforeq"
        |
            "inforep"
        |
            "maskreq"
        |
            "maskrep"
        |
            "trace"
        |
            "dataconv"
        |
            "mobredir"
        |
            "ipv6-where"
        |
            "ipv6-here"
        |
            "mobregreq"
        |
            "mobregrep"
        |
            "skip"
        |
            "photuris"
        )
    ;

icmp_code_by_name
    :
        (
            "net-unr"
        |
            "host-unr"
        |
            "proto-unr"
        |
            "port-unr"
        |
            "needfrag"
        |
            "srcfail"
        |
            "net-unk"
        |
            "host-unk"
        |
            "isolate"
        |
            "net-prohib"
        |
            "host-prohib"
        |
            "net-tos"
        |
            "host-tos"
        |
            "filter-prohib"
        |
            "host-preced"
        |
            "cutoff-preced"
        |
            "redir-net"
        |
            "redir-host"
        |
            "redir-tos-net"
        |
            "redir-tos-host"
        |
            "normal-adv"
        |
            "common-adv"
        |
            "transit"
        |
            "reassemb"
        |
            "badhead"
        |
            "optmiss"
        |
            "badlen"
        |
            "unknown-ind"
        |
            "auth-fail"
        |
            "decrypt-fail"
        )
    ;

icmp_list :
    OPENING_BRACE
    icmp_type_code
    (
        ( COMMA )?
        icmp_type_code
    )*
    CLOSING_BRACE
    ;


icmp6_type :
        ICMP6_TYPE
        {
            importer->error_tracker->registerError(
                QString("ICMP6 import is not supported. "));
            consumeUntil(NEWLINE);
        }
    ;

tagged :
        ( EXLAMATION { importer->tagged_neg = true; } )?
        TAGGED WORD
        {
            importer->tagged = LT(0)->getText();
        }
    ;

tag_clause :
        TAG WORD
        {
            importer->tag = LT(0)->getText();
        }
    ;

state :
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

queue :
        QUEUE
        (
            WORD { importer->queue += LT(0)->getText(); }
        |
            OPENING_PAREN
            WORD          { importer->queue += LT(0)->getText(); }
            (
                COMMA     { importer->queue += ","; }
                WORD      { importer->queue += LT(0)->getText(); }
            )*
            CLOSING_PAREN
        )
    ;

label :
        LABEL STRING
    ;

//****************************************************************

src_port_part :
        PORT
        (
            port_op
        |
            port_op_list
        )
        {
            importer->src_port_group.splice(importer->src_port_group.begin(),
                                            importer->tmp_port_group);
        }
    ;

dst_port_part :
        PORT
        (
            port_op
        |
            port_op_list
        )
        {
            importer->dst_port_group.splice(importer->dst_port_group.begin(),
                                            importer->tmp_port_group);
        }
    ;

unary_op :
        (
            EQUAL              { importer->tmp_port_op = "="; }
        |
            EXLAMATION EQUAL   { importer->tmp_port_op = "!="; }
        |
            LESS_THAN          { importer->tmp_port_op = "<"; }
        |
            LESS_THAN EQUAL    { importer->tmp_port_op = "<="; }
        |
            GREATER_THAN       { importer->tmp_port_op = ">"; }
        |
            GREATER_THAN EQUAL { importer->tmp_port_op = ">="; }
        )
    ;

binary_op :
        (
            LESS_THAN GREATER_THAN { importer->tmp_port_op = "<>"; }
        |
            GREATER_THAN LESS_THAN { importer->tmp_port_op = "><"; }
        |
            COLON                  { importer->tmp_port_op = ":"; }
        )
    ;

//  lexer matches port range (1000:1010) as IPv6, see rule
// NUMBER_ADDRESS_OR_WORD
port_op { PortSpec ps; } :
        (
            unary_op { ps.port_op = importer->tmp_port_op; }
            port_def
            {
                ps.port1 = importer->tmp_port_def;
                ps.port2 = importer->tmp_port_def;
            }
        |
            port_def
            {
                ps.port1 = importer->tmp_port_def;
                ps.port2 = ps.port1;
                ps.port_op = "=";
            }
            (
                binary_op { ps.port_op = importer->tmp_port_op; }
                port_def       { ps.port2 = LT(0)->getText(); }
            )?
        |
            IPV6
            {
                ps.setFromPortRange(LT(0)->getText());
            }
        )
        {
            importer->tmp_port_group.push_back(ps);
        }
    ;

port_def :
        ( WORD | INT_CONST )
        {
            importer->tmp_port_def = LT(0)->getText();
        }
    ;

port_op_list :
        OPENING_BRACE
        port_op
        (
            ( COMMA )?
            port_op
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
    INCLUDE_COMMAND = "include";

    EXIT = "exit";
    QUIT = "quit";

    NO = "no";

    INTRFACE = "interface";

    PASS = "pass";
    BLOCK = "block";
    MATCH = "match";

    QUICK = "quick";

    IN_WORD = "in";
    OUT_WORD = "out";

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
    ESP_WORD = "esp";
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
    ANY = "any";
    ALL = "all";
    USER = "user";
    GROUP = "group";
    NETWORK = "network";
    BROADCAST = "broadcast";
    PEER = "peer";

    PORT = "port";

    RANGE = "range";

    LOG = "log";

    NO_ROUTE = "no-route";
    SELF = "self";
    URPF_FAILED = "urpf-failed";

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
    ANTISPOOF = "antispoof";

    SET = "set";
    SCRUB = "scrub";
    NAT = "nat";
    RDR = "rdr";
    BINAT = "binat";
    TABLE = "table";
    CONST_WORD = "const";
    PERSIST = "persist";
    COUNTERS = "counters";
    FILE = "file";

    QUEUE = "queue";

    LABEL = "label";

    ROUTE_TO = "route-to";
    REPLY_TO = "reply-to";
    DUP_TO = "dup-to";

    DROP = "drop";
    RETURN = "return";
    RETURN_RST = "return-rst";
    RETURN_ICMP = "return-icmp";

    TAG = "tag";
    TAGGED = "tagged";

    STATE = "state";
    KEEP = "keep";
    MODULATE = "modulate";
    SYNPROXY = "synproxy";

    FLAGS = "flags";
    ICMP_TYPE = "icmp-type";
    ICMP6_TYPE = "icmp6-type";
    ICMP_CODE = "code";

    BITMASK = "bitmask";
    RANDOM = "random";
    SOURCE_HASH = "source-hash";
    HEX_KEY = "hex-key";
    STRING_KEY = "string-key";
    ROUND_ROBIN = "round-robin";
    STICKY_ADDRESS = "sticky-address";
    STATIC_PORT = "static-port";
}

// LINE_COMMENT : "#" (~('\r' | '\n'))* NEWLINE ;

Whitespace :  ( '\003'..'\010' | '\t' | '\013' | '\f' | '\016'.. '\037' | '\177'..'\377' | ' ' )
        { $setType(ANTLR_USE_NAMESPACE(antlr)Token::SKIP);  } ;


COMMENT_START : '#' ;

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
COLON : ;

protected
HEX_DIGIT : ( '0'..'9' | 'a'..'f' | 'A'..'F') ;

protected
DIGIT : '0'..'9'  ;

protected
NUM_3DIGIT: ('0'..'9') (('0'..'9') ('0'..'9')?)? ;

protected
NUM_HEX_4DIGIT: HEX_DIGIT ((HEX_DIGIT) ((HEX_DIGIT) (HEX_DIGIT)?)?)? ;

// Unfortunately IPV6 rule below matches "1000:1010" or "1024:65535"
// as IPV6. This is not a valid IPv6 address and it creates problems
// with port ranges
//
NUMBER_ADDRESS_OR_WORD 
options {
    testLiterals = true;
}
    :
    ( ( HEX_DIGIT )+ ':' ) =>
            (
                ( ( HEX_DIGIT )+ ( ':' ( HEX_DIGIT )* )+ ) { $setType(IPV6); }
            )

    | ( ':' ) =>
        (
            (':' ':' ( HEX_DIGIT )+ ) =>
                (':' ':' ( HEX_DIGIT )+ (':' ( HEX_DIGIT )+)*) {$setType(IPV6);}
        |
            (':' ':' ) { $setType(IPV6); }

        |   ':' { $setType(COLON); }
        )

    | ( NUM_3DIGIT '.' NUM_3DIGIT '.' ) =>
            (NUM_3DIGIT '.' NUM_3DIGIT '.' NUM_3DIGIT '.' NUM_3DIGIT)
            { $setType(IPV4); }

    | ( (DIGIT)+ '.' (DIGIT)+ )=> ( (DIGIT)+ '.' (DIGIT)+ )
        { $setType(NUMBER); }

    | ( DIGIT )+ { $setType(INT_CONST); }


// Making sure ',' '(' ')' '=' '<' '>' '+' are not part of WORD.
// Double quote " should be included, without it STRING does not match

    | ( 'a'..'z' | 'A'..'Z' )
      ( '"' | '$' | '%' | '&' | '-' | '.' | '0'..'9' | ';' |
        '?' | '@' | 'A'..'Z' | '\\' | '^' | '_' | '`' | 'a'..'z' )*
      { $setType(WORD); }

    | ( '$' ) =>
        (
            '$' ( 'a'..'z' | 'A'..'Z' ) ( 'a'..'z' | 'A'..'Z' | '0'..'9' | '_' )*
            { $setType(MACRO); }
        |
            '$'
        )
    ;

STRING : '"' (~'"')* '"';

PIPE_CHAR : '|';
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

// COLON : ':' ;
SEMICOLON : ';' ;

EQUAL : '=';

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

DOUBLE_QUOTE : '"';

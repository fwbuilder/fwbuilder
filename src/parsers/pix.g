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
#include "PIXImporter.h"
}

header "post_include_hpp"
{
    // gets inserted after antlr generated includes in the header file
    // outside any generated namespace specifications

#include <sstream>

class PIXImporter;
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


class PIXCfgParser extends Parser;
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
    PIXImporter *importer;

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
            version
        |
            hostname
        |
            community_list_command
        |
            unknown_ip_command
        |
            intrface
        |
            nameif_top_level
        |
            intf_address
        |
            controller
        |
            access_list_commands
        |
            ssh_command
        |
            telnet_command
        |
            http_command
        |
            icmp_top_level_command
        |
            nat_top_level_command
        |
            global_top_level_command
        |
            static_top_level_command
        |
            access_group
        |
            exit
        |
            certificate
        |
            quit
        |
            names_section
        |
            name_entry
        |
            named_object_network
        |
            named_object_service
        |
            object_group_network
        |
            object_group_service
        |
            object_group_protocol
        |
            object_group_icmp_8_0
        |
            object_group_icmp_8_3
        |
            crypto
        |
            no_commands
        |
            timeout_command
        |
            dns_command
        |
            service_top_level_command
        |
            pim_top_level_command
        |
            network_top_level_command
        |
            unknown_command
        |
            NEWLINE
        )+
    ;

//****************************************************************
quit : QUIT
        {
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
community_list_command : IP COMMUNITY_LIST
        {
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
timeout_command : TIMEOUT
        {
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
// need this because we have token PIM used in a different context
pim_top_level_command : PIM
        {
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
// need this because we have token NETWORK used in a different context
network_top_level_command : NETWORK
        {
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
// just skip this line since we pre-process names in PIXImporterRun.cpp
names_section : NAMES
    ;

name_entry : NAME ( name_entry_ipv4 | name_entry_ipv6 )
    ;

name_entry_ipv4 : IPV4 WORD
    ;

name_entry_ipv6 : IPV6 WORD
    ;
//****************************************************************

//
// these are used in access-list and named object definitions
//
ip_protocol_names : (
            AH | EIGRP | ESP_WORD | GRE |
            IGMP |  IGRP |  IP |  IPINIP |  IPSEC |
            NOS |  OSPF |  PCP |  PIM |  PPTP |  SNP )
    ;

//****************************************************************

named_object_network : OBJECT NETWORK name:WORD NEWLINE
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newNamedObjectAddress(name->getText());
            *dbg << name->getLine() << ":"
                << " Named Object " << name->getText() << std::endl;
        }
        (
            named_object_nat
        |
            named_object_description
        |
            named_object_network_parameters
        )*
    ;

named_object_network_parameters :
        (
            host_addr
        |
            range_addr
        |
            subnet_addr
        )
        NEWLINE
    ;

named_object_nat : nat_top_level_command
        {
            *dbg << "Named object with singleton nat command" << std::endl;
            importer->addMessageToLog(
                QString("Warning: "
                        "Import of named objects with \"nat\" command "
                        "is not supported at this time"));
            consumeUntil(NEWLINE);
        }
    ;

named_object_description : DESCRIPTION 
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            *dbg << LT(1)->getLine() << ":";
            std::string descr;
            while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
            {
                descr += LT(1)->getText() + " ";
                consume();
            }
            importer->setNamedObjectDescription(descr);
            *dbg << " DESCRIPTION " << descr << std::endl;
        }
        NEWLINE
    ;

host_addr : HOST single_addr
        {
            importer->commitNamedAddressObject();
        }
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

range_addr : (RANGE r1:IPV4 r2:IPV4)
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->tmp_range_1 = r1->getText();
            importer->tmp_range_2 = r2->getText();
            importer->commitNamedAddressRangeObject();
            *dbg << r1->getText() << "/" << r2->getText();
        }
    ;

subnet_addr : (SUBNET ((a:IPV4 nm:IPV4) | v6:IPV6))
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            if (a)
            {
                importer->tmp_a = a->getText();
                importer->tmp_nm = nm->getText();
                importer->commitNamedAddressObject();
                *dbg << a->getText() << "/" << nm->getText();
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

// Unfortunately any keyword can be used as named object name
//
named_object_service : OBJECT SERVICE 
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
        (WORD | HTTP | SSH | TELNET)
        {
            importer->newNamedObjectService(LT(0)->getText());
            *dbg << " NAMED OBJECT " << LT(0)->getText() << std::endl;
        }
        NEWLINE
        (
            named_object_description
        |
            named_object_service_parameters
        )*
    ;

named_object_service_parameters :
        (
            service_icmp
        |
            service_icmp6
        |
            service_tcp_udp
        |
            service_other
        |
            service_unknown
        )
        NEWLINE
        ;

service_icmp : SERVICE ICMP
        (
            icmp_type:INT_CONST
            {
                importer->icmp_type = LT(0)->getText();
            }
        |
            icmp_names
            {
                importer->icmp_spec = LT(0)->getText();
            }
        )
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->commitNamedICMPServiceObject();
            *dbg << "NAMED OBJECT SERVICE ICMP " << LT(0)->getText() << " ";
        }
    ;

service_icmp6 : SERVICE ICMP6 (INT_CONST | WORD)
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                QString("Warning: "
                        "Import of IPv6 addresses and servcies "
                        "is not supported at this time"));
            *dbg << "NAMED OBJECT SERVICE ICMP6 " << LT(0)->getText() << " ";
            consumeUntil(NEWLINE);
        }
    ;

service_tcp_udp : SERVICE (TCP|UDP)
        {
            importer->protocol = LT(0)->getText();
            *dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
        }
        ( src_port_spec )?
        ( dst_port_spec )?
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->commitNamedTCPUDPServiceObject();
        }
    ;

src_port_spec : SOURCE xoperator
        {
            importer->SaveTmpPortToSrc();
        }
    ;

dst_port_spec : ( DESTINATION )? xoperator
        {
            importer->SaveTmpPortToDst();
        }
    ;

service_other : SERVICE ( INT_CONST | ip_protocol_names)
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->protocol = LT(0)->getText();
            importer->commitNamedIPServiceObject();
            *dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
        }
    ;

// we should create a placeholder object even when its protocol is
// unknown because this object may be used in some object groups or
// acls later on. Add a note to the object comment to clarify there
// has been a parser error. Note that this is done because of the
// overall liberal policy of the importer that tries to import as much
// as possible even when some constructs are not recognized.
service_unknown : SERVICE WORD
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->protocol = "ip";
            importer->commitNamedIPServiceObject();
            std::string err = "Warning: Unknown service name " +
                LT(0)->getText();
            importer->setNamedObjectDescription(err);
            importer->addMessageToLog(err);
            *dbg << "UNKNOWN SERVICE " << LT(0)->getText() << " ";
        }
    ;


//****************************************************************

object_group_network : OBJECT_GROUP NETWORK name:WORD NEWLINE
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newObjectGroupNetwork(name->getText());
            *dbg << name->getLine() << ":"
                 << " Object Group " << name->getText() << std::endl;
        }
        (
            object_group_network_parameters
        )*
    ;

object_group_network_parameters :
        (
            object_group_description
        |
            group_object
        |
            network_object
        )
        NEWLINE
    ;

object_group_description : DESCRIPTION
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            *dbg << LT(1)->getLine() << ":";
            std::string descr;
            while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
            {
                descr += LT(1)->getText() + " ";
                consume();
            }
            importer->setObjectGroupDescription(descr);
            *dbg << " DESCRIPTION " << descr << std::endl;
        }
    ;

group_object : GROUP_OBJECT name:WORD
        {
            importer->clearTempVars();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addNamedObjectToObjectGroup(name->getText());
            *dbg << " GROUP MEMBER " << name->getLine() << std::endl;
        }
    ;

network_object : NETWORK_OBJECT
        {
            importer->clearTempVars();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
        (
        ( (a:IPV4 nm:IPV4) | v6:IPV6 )
        {
            if (a)
            {
                importer->tmp_a = a->getText();
                importer->tmp_nm = nm->getText();
                importer->addNetworkToObjectGroup();
                *dbg << a->getText() << "/" << nm->getText();
            }
            if (v6)
            {
                importer->addMessageToLog(
                    QString("Warning: IPv6 import is not supported. "));
                consumeUntil(NEWLINE);
            }
        }
    |
        HOST ( h:IPV4 | hv6:IPV6)
        {
            if (h)
            {
                importer->tmp_a = h->getText();
                importer->tmp_nm = "255.255.255.255";
                importer->addNetworkToObjectGroup();
                *dbg << h->getText() << "/255.255.255.255";
            }
            if (hv6)
            {
                importer->addMessageToLog(
                    QString("Warning: IPv6 import is not supported. "));
                consumeUntil(NEWLINE);
            }
        }
    |
        OBJECT name:WORD
        {
            importer->addNamedObjectToObjectGroup(name->getText());
            *dbg << " GROUP MEMBER " << name->getLine() << std::endl;
        }
    )
    ;

//****************************************************************

object_group_protocol : OBJECT_GROUP PROTOCOL name:WORD NEWLINE
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newObjectGroupProtocol(name->getText());
            *dbg << name->getLine() << ":"
                 << " Object Group " << name->getText() << std::endl;
        }
        (
            object_group_protocol_parameters
        )+
    ;

object_group_protocol_parameters :
        (
            object_group_description
        |
            group_object
        |
            protocol_object
        )
        NEWLINE
    ;

protocol_object : PROTOCOL_OBJECT
        {
            importer->clearTempVars();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
    (
        ( INT_CONST | ICMP | TCP | UDP | ip_protocol_names)
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->protocol = LT(0)->getText();
            importer->addIPServiceToObjectGroup();
            *dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
        }
    |
        ICMP6
        {
            importer->addMessageToLog(
                QString("Warning: IPv6 import is not supported. "));
            consumeUntil(NEWLINE);
        }
    |
        OBJECT name:WORD
        {
            importer->addNamedObjectToObjectGroup(name->getText());
            *dbg << " GROUP MEMBER " << name->getLine() << std::endl;
        }
    )
    ;

//****************************************************************

object_group_icmp_8_0 : OBJECT_GROUP ICMP_OBJECT name:WORD NEWLINE
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newObjectGroupICMP(name->getText());
            *dbg << name->getLine() << ":"
                 << " Object Group " << name->getText() << std::endl;
        }
        (
            object_group_icmp_parameters
        )*
    ;

object_group_icmp_8_3 : OBJECT_GROUP ICMP_TYPE name:WORD NEWLINE
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newObjectGroupICMP(name->getText());
            *dbg << name->getLine() << ":"
                 << " Object Group " << name->getText() << std::endl;
        }
        (
            object_group_icmp_parameters
        )*
    ;

object_group_icmp_parameters :
        (
            object_group_description
        |
            group_object
        |
            icmp_object
        )
        NEWLINE
    ;

icmp_object : ICMP_OBJECT
        {
            importer->clearTempVars();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
    (
        (
            icmp_type:INT_CONST
            {
                importer->icmp_type = LT(0)->getText();
            }
        | 
            icmp_names
            {
                importer->icmp_spec = LT(0)->getText();
            }
        )
        {
            importer->addICMPServiceToObjectGroup();
            *dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
        }
    |
        OBJECT name:WORD
        {
            importer->addNamedObjectToObjectGroup(name->getText());
            *dbg << " GROUP MEMBER " << name->getLine() << std::endl;
        }
    )
    ;

//****************************************************************

object_group_service : OBJECT_GROUP SERVICE name:WORD ( tcp:TCP | udp:UDP | tcpudp:TCP_UDP )? NEWLINE
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newObjectGroupService(name->getText());
            if (tcp) importer->setObjectGroupServiceProtocol("tcp");
            if (udp) importer->setObjectGroupServiceProtocol("udp");
            if (tcpudp) importer->setObjectGroupServiceProtocol("tcp-udp");
            *dbg << name->getLine() << ":"
                 << " Object Group " << name->getText() << std::endl;
        }
        (
            object_group_service_parameters
        )*
    ;

object_group_service_parameters :
        (
            object_group_description
        |
            group_object
        |
            service_object
        |
            port_object
        )
        NEWLINE
    ;

service_object : SERVICE_OBJECT
        {
            importer->clearTempVars();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
    (
        ( INT_CONST | ip_protocol_names)
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->protocol = LT(0)->getText();
            importer->addIPServiceToObjectGroup();
            *dbg << " GROUP MEMBER " << LT(0)->getText() << " ";
        }
    |
        ( TCP | UDP | TCP_UDP )
        {
            importer->protocol = LT(0)->getText();
            *dbg << " SERVICE TCP/UDP" << LT(0)->getText() << " ";
        }
        ( src_port_spec )?
        ( dst_port_spec )?
        {
            importer->addTCPUDPServiceToObjectGroup();
        }
    |
        ICMP
        (
            icmp_type:INT_CONST
            {
                importer->icmp_type = LT(0)->getText();
            }
        | 
            icmp_names
            {
                importer->icmp_spec = LT(0)->getText();
            }
        )
        {
            importer->addICMPServiceToObjectGroup();
            *dbg << " SERVICE ICMP " << LT(0)->getText() << " ";
        }
    |
        OBJECT name:WORD
        {
            importer->addNamedObjectToObjectGroup(name->getText());
            *dbg << " GROUP MEMBER " << name->getLine() << std::endl;
        }
    )
    ;

port_object
        {
            importer->tmp_port_spec = "";
            importer->tmp_port_spec_2 = "";
        } : PORT_OBJECT xoperator
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            *dbg << " PORT OBJECT TCP/UDP " << LT(0)->getText() << " " << std::endl;
            importer->SaveTmpPortToDst();
            importer->addTCPUDPServiceToObjectGroup();
            *dbg << std::endl;
        }
    ;

//****************************************************************
crypto : CRYPTO
        {
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
unknown_ip_command : IP WORD
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
dns_command : DNS
        {
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
//
//asa5505(config)# service  ?
//
//  call-home          Enable or disable Smart Call-Home
//  internal           Advanced settings (use only under Cisco supervision)
//  password-recovery  Password recovery configuration
//  resetinbound       Send reset to a denied inbound TCP packet
//  resetoutbound      Send reset to a denied outbound TCP packet
//  resetoutside       Send reset to a denied TCP packet to outside interface

service_top_level_command : SERVICE
        ( CALL_HOME |
          INTERNAL |
          PASSWORD_RECOVERY |
          RESETINBOUND |
          RESETOUTBOUND |
          RESETOUTSIDE )
        {
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
no_commands : NO
        {
            *dbg << " TOP LEVEL \"NO\" COMMAND: "
                 << LT(0)->getText() << std::endl;
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
certificate : CERTIFICATE WORD
        {
            consumeUntil(NEWLINE);
            consumeUntil(QUIT);
        }
    ;

//****************************************************************
version { std::string platform; } :
        (
            PIX_WORD | ASA_WORD | FWSM_WORD
        )
        {
            platform = LT(0)->getText();
        }
        VERSION_WORD_CAP NUMBER
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->setDiscoveredPlatform(platform);
            importer->setDiscoveredVersion(LT(0)->getText());
            *dbg << "VERSION " << LT(0)->getText() << std::endl;
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
hostname : HOSTNAME ( STRING | WORD )
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            // we get host name in the import wizard
            //importer->setHostName( LT(0)->getText() );
            *dbg << "HOSTNAME " << "LT0=" << LT(0)->getText() << std::endl;
        }
    ;

//****************************************************************

access_list_commands : ACCESS_LIST name:WORD
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newUnidirRuleSet(
                name->getText(), libfwbuilder::Policy::TYPENAME );
            *dbg << name->getLine() << ":"
                << " ACL ext " << name->getText() << std::endl;
        }
        (
            permit_extended
        |
            deny_extended
        |
            permit_standard
        |
            deny_standard
        |
            comment
        |
            remark
        |
            NEWLINE
        )
        {
            *dbg << LT(0)->getLine() << ":"
                << " ACL line end" << std::endl << std::endl;
        }
    ;

//****************************************************************
permit_extended: ( EXTENDED )? PERMIT
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "permit";
            *dbg << LT(1)->getLine() << ":" << " permit ";
        }
        rule_extended NEWLINE
        {
            importer->pushRule();
        }
    ;

deny_extended: ( EXTENDED )? DENY
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "deny";
            *dbg << LT(1)->getLine() << ":" << " deny   ";
        }
        rule_extended NEWLINE
        {
            importer->pushRule();
        }
    ;

permit_standard: STANDARD PERMIT
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "permit";
            *dbg << LT(1)->getLine() << ":" << " permit ";
        }
        rule_standard NEWLINE
        {
            importer->pushRule();
        }
    ;

deny_standard: STANDARD DENY
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "deny";
            *dbg << LT(1)->getLine() << ":" << " deny   ";
        }
        rule_standard NEWLINE
        {
            importer->pushRule();
        }
    ;

//****************************************************************
// the difference between standard and extended acls should be in these rules

// standard acl only matches destination address
rule_standard :
        {
            importer->tmp_a = "0.0.0.0";
            importer->tmp_nm = "0.0.0.0";
            importer->SaveTmpAddrToSrc();
        }
        hostaddr_expr
        {
            importer->SaveTmpAddrToDst();
            *dbg << "(dst) " << std::endl;
        }
    ;

rule_extended :
        (
            ip_protocols
            hostaddr_expr { importer->SaveTmpAddrToSrc(); *dbg << "(src) "; }
            hostaddr_expr { importer->SaveTmpAddrToDst(); *dbg << "(dst) "; }
            (time_range)?
            (fragments)?
            (log)?
        |
            ICMP
            {
                importer->protocol = LT(0)->getText();
                *dbg << "protocol " << LT(0)->getText() << " ";
            }
            hostaddr_expr { importer->SaveTmpAddrToSrc(); *dbg << "(src) "; }
            hostaddr_expr { importer->SaveTmpAddrToDst(); *dbg << "(dst) "; }

            // at this point:
         
            // configure mode commands/options:
            //   <0-255>               Enter ICMP type number (0 - 255)
            //   alternate-address     
            //   conversion-error      
            //   echo                  
            //   echo-reply            
            //   inactive              Keyword for disabling an ACL element
            //       . . . . more icmp service names
            //  object-group          ICMP object-group for destination port
            //       . . . . more icmp service names
            //  <cr>
            (
                OBJECT_GROUP grp_name:WORD
                {
                    importer->icmp_spec = grp_name->getText();
                    *dbg << "service gorup: "
                     << grp_name->getText() << std::endl;
                }
            |
                icmp_spec
            )?
            (time_range)?
            (fragments)?
            (log)?
        |
            tcp_udp_rule_extended
        )
        {
            *dbg << std::endl;
        }
    ;

tcp_udp_rule_extended :
        ( TCP | UDP )
        {
            importer->protocol = LT(0)->getText();
            *dbg << "protocol " << LT(0)->getText() << " ";
        }
        hostaddr_expr { importer->SaveTmpAddrToSrc(); *dbg << "(src) "; }
        (
            (OBJECT_GROUP) => (
                // This object-group can be either
                // source port or destination address
                //
                // Using disambiguating predicate; it must be the first element
                // in the production (i.e. nothing should precede {}?)
                { importer->isKnownServiceGroupName(LT(2)->getText()) }?
                OBJECT_GROUP src_grp_name:WORD
                {
                    importer->src_port_spec = src_grp_name->getText();
                    *dbg << "src port spec: "
                     << src_grp_name->getText() << std::endl;
                }
                // destination address spec follows; hostaddr_expr matches
                // OBJECT | OBJECT_GROUP among pure addresses
                hostaddr_expr_1
                {
                    importer->SaveTmpAddrToDst();
                    *dbg << "(dst) ";
                }
                acl_tcp_udp_dst_port_spec
            |
                // still object-group after src address but this group is not
                // a known service group - must be dest. address group
                hostaddr_expr_2
                {
                    importer->SaveTmpAddrToDst();
                    *dbg << "(dst) ";
                }
                acl_tcp_udp_dst_port_spec
            )
        |
            // not "object-group" keyword after src address spec.
            OBJECT dst_addr_name:WORD (acl_xoperator_dst)? (established)?
            {
                // looks like "object foo" at this point can only be dest addr.
                // (judging by cli prompts on 8.3)
                importer->tmp_a = dst_addr_name->getText();
                importer->tmp_nm = "";
                importer->SaveTmpAddrToDst();
                *dbg << "dst addr object " << dst_addr_name->getText() << " ";
            }
            acl_tcp_udp_dst_port_spec
        |
            // if not object-group and object, then it can optionally
            // be regular inline port spec, followed by dest address spec
            (
                xoperator
                {
                    importer->SaveTmpPortToSrc();
                }
            )?
            hostaddr_expr_3 { importer->SaveTmpAddrToDst(); *dbg << "(dst) "; }
            acl_tcp_udp_dst_port_spec
        )
        (time_range)?
        (fragments)?
        (log)?
    ;

//****************************************************************

acl_tcp_udp_dst_port_spec :
            (
                // destination port spec. Can be blank, a named
                // object, object-group or inline

                (OBJECT_GROUP) => (
                    // This object-group can be only destination port
                    OBJECT_GROUP dst_port_group_name:WORD
                    {
                        importer->dst_port_spec = dst_port_group_name->getText();
                        *dbg << "dst port spec: "
                         << dst_port_group_name->getText() << std::endl;
                    }
                    (established)?
                )
            |
                // not "object-group"
                OBJECT dst_port_obj_name:WORD
                {
                    importer->dst_port_spec = dst_port_obj_name->getText();
                    *dbg << "dst addr object " << dst_port_obj_name->getText()
                         << std::endl;
                }
                (established)?
            |
                // if not object-group and object, then it can optionally
                // be regular inline port spec
                (acl_xoperator_dst)?
                (established)?
            )
;

acl_xoperator_dst : xoperator
        {
            importer->SaveTmpPortToDst();
        }
    ;

xoperator { importer->tmp_port_spec = ""; } :
        single_port_op | port_range 
    ;

//****************************************************************

single_port_op : (P_EQ | P_GT | P_LT | P_NEQ )
        {
            importer->tmp_port_op = LT(0)->getText();
            *dbg << LT(0)->getText() << " ";
        }
        port_spec
    ;

port_spec : 
        {
            importer->tmp_port_spec_2 = "";
        }
        tcp_udp_port_spec
        {
            importer->tmp_port_spec = std::string(" ") + importer->tmp_port_spec_2;
            *dbg << LT(0)->getText() << " " << importer->tmp_port_spec;
        }
    ;

port_range : RANGE pair_of_ports_spec
        {
            importer->tmp_port_op = "range";
            *dbg << "range " << importer->tmp_port_spec;
        }
    ;

pair_of_ports_spec : 
        {
            importer->tmp_port_spec_2 = "";
        }
        tcp_udp_port_spec
        {
            importer->tmp_port_spec += importer->tmp_port_spec_2;
        }
        tcp_udp_port_spec
        {
            importer->tmp_port_spec += " ";
            importer->tmp_port_spec += importer->tmp_port_spec_2;
        }
    ;

// note that some words coincide as names of protocols or ports and
// can be used in other parts of configuration
tcp_udp_port_spec : (tcp_udp_port_names | WORD | INT_CONST)
        {
            importer->tmp_port_spec_2 = LT(0)->getText();
        }
    ;

// tokens that can be tcp/udp port names (but can also be used for
// something else). If I ever decide to make tokens for every known
// port name, they should be added here
tcp_udp_port_names : 
    (
        ECHO |
        HOSTNAME |
        PPTP |
        SSH |
        TELNET |
        HTTP
    )
    ;

established : ESTABLISHED
        {
            importer->established = true;
            *dbg << "established ";
        }
    ;

//****************************************************************

ip_protocols :
        (
            ( ip_protocol_names | ICMP6 )
            {
                importer->protocol = LT(0)->getText();
                *dbg << "protocol " << LT(0)->getText() << " ";
            }
        |
            ( ( OBJECT | OBJECT_GROUP ) name:WORD )
            {
                importer->protocol = name->getText();
                *dbg << "protocol " << name->getText() << " ";
            }
        )
    ;

icmp_spec :
        (
            (INT_CONST) => (icmp_type:INT_CONST icmp_code:INT_CONST)
            {
                importer->icmp_type = icmp_type->getText();
                importer->icmp_code = icmp_code->getText();
                importer->icmp_spec = "";
                *dbg << icmp_type->getText() << " "
                    << icmp_code->getText() << " ";
            }
        |
            icmp_names
            {
                importer->icmp_spec = LT(0)->getText();
                *dbg << LT(0)->getText() << " ";
            }
        )
    ;

icmp_names :
            (
                ALTERNATE_ADDRESS | CONVERSION_ERROR | ECHO |
                ECHO_REPLY | INFORMATION_REPLY | INFORMATION_REQUEST |
                MASK_REPLY | MASK_REQUEST | MOBILE_REDIRECT |
                PARAMETER_PROBLEM | REDIRECT | ROUTER_ADVERTISEMENT |
                ROUTER_SOLICITATION | SOURCE_QUENCH | TIME_EXCEEDED |
                TIMESTAMP_REPLY | TIMESTAMP_REQUEST | TRACEROUTE |
                UNREACHABLE
            )
    ;

//****************************************************************

// using these to help with debugging
hostaddr_expr_1 : hostaddr_expr ;
hostaddr_expr_2 : hostaddr_expr ;
hostaddr_expr_3 : hostaddr_expr ;

hostaddr_expr :
        INTRFACE interface_label
        {
            importer->tmp_a = LT(0)->getText();
            importer->tmp_nm = "interface";
            *dbg << "object " << LT(0)->getText() << " ";
        }
    |
        ( ( OBJECT | OBJECT_GROUP ) name:WORD )
        {
            importer->tmp_a = name->getText();
            importer->tmp_nm = "";
            *dbg << "object " << name->getText() << " ";
        }
    |
        (HOST h:IPV4)
        {
            importer->tmp_a = h->getText();
            importer->tmp_nm = "255.255.255.255";
            *dbg << h->getText() << "/255.255.255.255";
        }
    |
        (a:IPV4 m:IPV4)
        {
            importer->tmp_a = a->getText();
            importer->tmp_nm = m->getText();
            *dbg << a->getText() << "/" << m->getText();
        }
    |
        ANY
        {
            importer->tmp_a = "0.0.0.0";
            importer->tmp_nm = "0.0.0.0";
            *dbg << "0.0.0.0/0.0.0.0";
        }
    ;

//****************************************************************


log : (LOG | LOG_INPUT)
    {
        importer->logging = true;
    }
    (
          (
            (
                INT_CONST | 
                LOG_LEVEL_ALERTS |
                LOG_LEVEL_CRITICAL |
                LOG_LEVEL_DEBUGGING |
                LOG_LEVEL_EMERGENCIES |
                LOG_LEVEL_ERRORS |
                LOG_LEVEL_INFORMATIONAL |
                LOG_LEVEL_NOTIFICATIONS |
                LOG_LEVEL_WARNINGS |
                LOG_LEVEL_DISABLE |
                LOG_LEVEL_INACTIVE
            ) { importer->log_level = LT(0)->getText(); }
          )?
          (
            (
                INTERVAL INT_CONST
            ) { importer->log_interval = LT(0)->getText(); }
          )?
    )
    {
//        if (importer->log_level == "log") importer->log_level = "";
//        if (importer->log_interval == "log") importer->log_interval = "";
        *dbg << "logging level '" << importer->log_level
             << "' interval '" << importer->log_interval << "'";
    }
    ;

fragments : FRAGMENTS
        {
            importer->fragments = true;
            *dbg << "fragments ";
        }
    ;

time_range : TIME_RANGE tr_name:WORD
        {
            importer->time_range_name = tr_name->getText();
            *dbg << "time_range " << tr_name->getText() << " ";
        }
    ;


//****************************************************************

controller : CONTROLLER
        {
            importer->clearCurrentInterface();
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
//
// **************** PIX 6 "interface" command:
//
//	interface <hardware_id> [<hw_speed> [shutdown]]
//	[no] interface <hardware_id> <vlan_id> [logical|physical] [shutdown]
//	interface <hardware_id> change-vlan <old_vlan_id> <new_vlan_id>
//	show interface
//
// Example:
//
// interface ethernet0 auto
// interface ethernet1 auto
// nameif ethernet0 outside security0
// nameif ethernet1 inside security100
//
// **************** PIX 7 "interface" command
//
//	interface <type> <port>
//	interface <type> <port>.<subif_number>
//	no interface <type> <port>.<subif_number>
//
// Examples:
//
// interface Ethernet0
//  no nameif
//  no security-level
//  no ip address
// !
// interface Ethernet0.101
//  vlan 101
//  nameif outside
//  security-level 0
//  ip address 192.0.2.253 255.255.255.0
// !


// vlans in pix6 config format are not parsed

intrface  : INTRFACE ( interface_command_6 | interface_command_7 )
    ;

// unfortunately word "outside" is used as a keyword in nat commands
// and is also common interface label
interface_label : WORD | OUTSIDE
    ;

interface_command_6 : in:WORD pix6_interface_hw_speed    // pix 6
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newInterface( in->getText() );
            *dbg << in->getLine() << ":"
                << " INTRFACE: " << in->getText() << std::endl;
        }
    ;

interface_command_7 {bool have_interface_parameters = false;} : in:WORD NEWLINE
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newInterface( in->getText() );
            *dbg << in->getLine() << ":"
                << " INTRFACE: " << in->getText() << std::endl;
        }
        (
            ( interface_parameters {have_interface_parameters = true;}  )*
            ( LINE_COMMENT | EXIT )
            {
                if ( ! have_interface_parameters )
                {
                    importer->ignoreCurrentInterface();
                    *dbg<< LT(1)->getLine() << ":"
                        << " EMPTY INTERFACE " << std::endl;
                }
            }
        )
    ;

pix6_interface_hw_speed : (
        AUI | AUTO | BNC | ( INT_CONST ( FULL | BASET | BASETX | AUTO ) )
    )
        ;

nameif_top_level
        {
            std::string intf_name, intf_label, sec_level;
        } :
        NAMEIF
        WORD { intf_name = LT(0)->getText(); }
        interface_label { intf_label = LT(0)->getText(); }
        WORD { sec_level = LT(0)->getText(); }
        {
            importer->setInterfaceParametes(intf_name, intf_label, sec_level);
            *dbg << " NAMEIF: "
                 << intf_name << " "
                 << intf_label << " "
                 << sec_level << std::endl;
        }
    ;


interface_parameters :
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
        (
            intf_address
        |
            vlan_interface
        |
            sec_level
        |
            nameif
        |
            interface_description
        |
            switchport
        |
            shutdown
        |
            interface_no_commands
        |
            unsupported_interface_commands
        )
        NEWLINE
    ;

vlan_interface : VLAN vlan_id:INT_CONST
        {
            importer->setInterfaceVlanId(vlan_id->getText());
            *dbg << " VLAN: " << vlan_id->getText() << std::endl;
        }
    ;

unsupported_interface_commands :
        (
            SPEED
        |
            DUPLEX
        |
            DDNS
        |
            FORWARD
        |
            DELAY
        |
            HOLD_TIME
        |
            IGMP
        |
            IPV6_C
        |
            MANAGEMENT_ONLY
        |
            MAC_ADDRESS
        |
            MULTICAST
        |
            OSPF
        |
            PIM
        |
            PPPOE
        |
            RIP
        )
        {
            *dbg << " UNSUPPORTED INTERFACE COMMAND: "
                 << LT(0)->getText() << std::endl;
            consumeUntil(NEWLINE);
        }
    ;

interface_no_commands : NO (NAMEIF | IP | SEC_LEVEL | SHUTDOWN)
        {
            *dbg << " INTERFACE \"NO\" COMMAND: "
                 << LT(0)->getText() << std::endl;
            consumeUntil(NEWLINE);
        }
    ;

sec_level : SEC_LEVEL sec_level:INT_CONST
        {
            importer->setInterfaceSecurityLevel(sec_level->getText());
            *dbg << "SEC_LEVEL: " << sec_level->getText() << std::endl;
        }
    ;

//
// If there is a word after label, then there must be sec_level
// also. Otherwise there must be nothing.
//
// In case of pix6 configs, "nameif" is not really inside interface
// context but is rather located at the top level, the same level
// where "interface" line is found. Also, pix6 places all definitions
// of physical interfaces ("interface") first, then all nameif lines
// under them. Even though match for nameif is in the interface
// context in the grammar, function setInterfaceParametes() can locate
// right interface using its first parameter.
//
nameif  : NAMEIF interface_label
        {
            importer->setInterfaceParametes(LT(0)->getText(), "", "");
            *dbg << " NAMEIF: " << LT(0)->getText() << std::endl;
        }
    ;

// interface description
// Use it for comment
interface_description : DESCRIPTION
        {
            *dbg << LT(1)->getLine() << ":";
            std::string descr;
            while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
            {
                descr += LT(1)->getText() + " ";
                consume();
            }
            importer->setInterfaceComment( descr );
            *dbg << " DESCRIPTION " << descr << std::endl;
            //consumeUntil(NEWLINE);
        }
    ;

shutdown : SHUTDOWN
        {
            importer->ignoreCurrentInterface();
            *dbg<< LT(1)->getLine() << ":"
                << " INTERFACE SHUTDOWN " << std::endl;
        }
    ;


// Interface IP address.
//
// **************** PIX 6
//
// ip address outside dhcp setroute retry 10
// ip address inside 10.3.14.202 255.255.255.0
//
// **************** PIX 7
//
// interface Ethernet0.101
//  vlan 101
//  nameif outside
//  security-level 0
//  ip address 192.0.2.253 255.255.255.0
// !
//
// interface Vlan1
//  nameif inside
//  security-level 100
//  ip address dhcp setroute
// !

intf_address : IP ADDRESS (v6_ip_address | v7_ip_address) ;

v6_ip_address : v6_dhcp_address | v6_static_address;

v6_dhcp_address { std::string lbl; } : 
        interface_label 
        {
            lbl = LT(0)->getText();
        }
        dhcp:DHCP
        {
            std::string addr = dhcp->getText();
            importer->addInterfaceAddress(lbl, addr, "");
            *dbg << LT(1)->getLine() << ":"
                 << " INTRFACE ADDRESS: " << addr << std::endl;
// there can be some other parameters after "dhcp", such as
// "setroute", "retry" etc.  which we do not support
            consumeUntil(NEWLINE);
        }
    ;

v6_static_address  { std::string lbl; } : 
        interface_label 
        {
            lbl = LT(0)->getText();
        }
        a:IPV4 m:IPV4
        {
            std::string addr = a->getText();
            std::string netm = m->getText();
            importer->addInterfaceAddress(lbl, addr, netm);
            *dbg << LT(1)->getLine() << ":"
                 << " INTRFACE ADDRESS: " << addr << "/" << netm << std::endl;
// in case there are some other parameters after address and netmask
            consumeUntil(NEWLINE);
        }
    ;



v7_ip_address : v7_dhcp_address | v7_static_address;

v7_dhcp_address : dhcp:DHCP
        {
            std::string addr = dhcp->getText();
            importer->addInterfaceAddress(addr, "");
            *dbg << LT(1)->getLine() << ":"
                << " INTRFACE ADDRESS: " << addr << std::endl;
            consumeUntil(NEWLINE);
        }
//        NEWLINE
    ;

v7_static_address : a:IPV4 m:IPV4 (s:STANDBY)?
        {
            std::string addr = a->getText();
            std::string netm = m->getText();
            importer->addInterfaceAddress(addr, netm);
            *dbg << LT(1)->getLine() << ":"
                << " INTRFACE ADDRESS: " << addr << "/" << netm << std::endl;
// there can be other parameters after address/netmask pair, such as "standby"
// We do not parse them yet.
            if (s)
            {
                importer->addMessageToLog(
                    QString("Warning: failover IP detected. "
                            "Failover is not supported by import "
                            "at this time"));
            }
            consumeUntil(NEWLINE);
        }
//        NEWLINE
    ;


switchport : SWITCHPORT ACCESS VLAN vlan_num:INT_CONST
        {
            importer->addMessageToLog("Switch port vlan " + vlan_num->getText());
            *dbg << "Switch port vlan " <<  vlan_num->getText() << std::endl;
        }
    ;

//****************************************************************
// pretend ssh commands are rules in access lists with names
// "ssh_commands_" + interface_label
ssh_command : SSH
        {
            importer->clear();
        }
        (
            ( TIMEOUT INT_CONST ) 
            {
                // set ssh timeout here
            }
        |
            SCOPY
        |
            VERSION_WORD_LOW INT_CONST
        |
            (
                hostaddr_expr
                {
                    importer->SaveTmpAddrToSrc();
                }
                interface_label
            )
            {
                std::string intf_label = LT(0)->getText();
                std::string acl_name = "ssh_commands_" + intf_label;
                importer->setCurrentLineNumber(LT(0)->getLine());
                importer->newUnidirRuleSet(acl_name, libfwbuilder::Policy::TYPENAME );
                importer->newPolicyRule();
                importer->action = "permit";
                importer->setDstSelf();
                importer->protocol = "tcp";
                importer->dst_port_op = "eq";
                importer->dst_port_spec = "ssh";
                importer->setInterfaceAndDirectionForRuleSet(
                    acl_name, intf_label, "in" );
                importer->pushRule();
                *dbg << std::endl;
            }
        )
    ;

telnet_command : TELNET
        {
            importer->clear();
        }
        (
            ( TIMEOUT INT_CONST ) |
            (
                hostaddr_expr
                {
                    importer->SaveTmpAddrToSrc();
                }
                interface_label
            )
            {
                std::string intf_label = LT(0)->getText();
                std::string acl_name = "telnet_commands_" + intf_label;
                importer->setCurrentLineNumber(LT(0)->getLine());
                importer->newUnidirRuleSet(acl_name, libfwbuilder::Policy::TYPENAME );
                importer->newPolicyRule();
                importer->action = "permit";
                importer->setDstSelf();
                importer->protocol = "tcp";
                importer->dst_port_op = "eq";
                importer->dst_port_spec = "telnet";
                importer->setInterfaceAndDirectionForRuleSet(
                    acl_name, intf_label, "in" );
                importer->pushRule();
                *dbg << std::endl;
            }
        )
    ;

// pretend ssh commands are rules in access lists with names
// "htto_commands_" + interface_label
http_command : HTTP
        {
            importer->clear();
        }
        (
            ( AUTHENTICATION_CERTIFICATE | REDIRECT | SERVER ) 
            {
                consumeUntil(NEWLINE);
            }
        |
            (
                hostaddr_expr
                {
                    importer->SaveTmpAddrToSrc();
                }
                interface_label
            )
            {
                std::string intf_label = LT(0)->getText();
                std::string acl_name = "http_commands_" + intf_label;
                importer->setCurrentLineNumber(LT(0)->getLine());
                importer->newUnidirRuleSet(acl_name,
                                           libfwbuilder::Policy::TYPENAME );
                importer->newPolicyRule();
                importer->action = "permit";
                importer->setDstSelf();
                importer->protocol = "tcp";
                importer->dst_port_op = "eq";
                importer->dst_port_spec = "www";
                importer->setInterfaceAndDirectionForRuleSet(
                    acl_name, intf_label, "in" );
                importer->pushRule();
                *dbg << std::endl;
            }
        )
    ;


// icmp command is non-determenistic syntactically because WORD can be
// used as a name of icmp type or as interface label.  I am going to
// define all icmp types as tokens in icmp_types_for_icmp_command
// Looks like "icmp" command accepts limited set of icmp type names
// and can accept numeric code.
//
icmp_top_level_command : ICMP 
    (
        ( UNREACHABLE
            {
                consumeUntil(NEWLINE);
            }
        )
    |
        (
            (permit:PERMIT | deny:DENY)
            {
                importer->clear();
            }
            hostaddr_expr
            {
                importer->SaveTmpAddrToSrc();
            }
            ( icmp_types_for_icmp_command )?
            interface_label
            {
                std::string intf_label = LT(0)->getText();
                std::string acl_name = "icmp_commands_" + intf_label;
                importer->setCurrentLineNumber(LT(0)->getLine());
                importer->newUnidirRuleSet(acl_name, libfwbuilder::Policy::TYPENAME );
                importer->newPolicyRule();
                if (permit) importer->action = "permit";
                if (deny) importer->action = "deny";
                importer->setDstSelf();
                importer->protocol = "icmp";
                importer->setInterfaceAndDirectionForRuleSet(
                    acl_name, intf_label, "in" );
                importer->pushRule();
            }
         )
    )
    ;

icmp_types_for_icmp_command : 
        INT_CONST
        {
            importer->icmp_type = LT(0)->getText();
            importer->icmp_code = "0";
            importer->icmp_spec = "";
        }
    | 
        (ECHO | ECHO_REPLY | TIME_EXCEEDED | UNREACHABLE)
        {
            importer->icmp_type = "";
            importer->icmp_code = "0";
            importer->icmp_spec = LT(0)->getText();
        }

    ;

//****************************************************************

// remark. According to the Cisco docs, can only be used
// within access list
// Use it for the current rule comment
remark : REMARK
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            *dbg << LT(1)->getLine() << ":";
            std::string rem;
            while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
            {
                rem += LT(1)->getText() + " ";
                consume();
            }
            importer->addRuleComment( rem );
            *dbg << " REMARK " << rem << std::endl;
            //consumeUntil(NEWLINE);
        }
    ;

//****************************************************************

access_group : ACCESS_GROUP aclname:WORD dir:WORD INTRFACE interface_label
        {
            std::string intf_label = LT(0)->getText();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->setInterfaceAndDirectionForRuleSet(
                aclname->getText(),
                intf_label,
                dir->getText() );
            *dbg << LT(1)->getLine() << ":"
                << " INTRFACE: ACL '" << aclname->getText() << "'"
                << " " << intf_label
                << " " << dir->getText() << std::endl;
        }
    ;

//****************************************************************

exit: EXIT
    ;

comment : (LINE_COMMENT | COLON_COMMENT) ;


//****************************************************************
// NAT commands

nat_top_level_command : 
        NAT OPENING_PAREN
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
        ( nat_old_top_level_command | nat_new_top_level_command )
    ;

nat_old_top_level_command : 
        interface_label
        {
            importer->prenat_interface = LT(0)->getText();
        }
        CLOSING_PAREN
        {
            importer->newUnidirRuleSet("nat", libfwbuilder::NAT::TYPENAME );
            *dbg << " SNAT rule ";
            importer->rule_type = libfwbuilder::NATRule::SNAT;
        }

        //  <0-2147483647>  The <nat_id> of this group of hosts/networks.
        INT_CONST
        {
            importer->nat_num = LT(0)->getText();
        }

        //  Hostname or A.B.C.D  The hosts/networks in this <nat_id> group
        //  access-list          Specify access-list name after this keyword

        nat_addr_match

        ( nat_command_last_parameters )*

        NEWLINE
        {
            importer->pushNATRule();
            *dbg << std::endl;
        }
    ;

nat_addr_match :
        single_addr   // real
        {
            importer->nat_a = importer->tmp_a;
        }

        // A.B.C.D  IP netmask to apply to the local IP address
        // <cr>
        (
            single_addr  
            {
                importer->nat_nm = importer->tmp_a;
            }
        )?
    |
        INT_CONST
        {
            if (LT(0)->getText() != std::string("0"))
            {
                importer->reportError(
                    std::string("Unknown string ")+LT(0)->getText()+
                    std::string("; expected \"0\""));
                *dbg << " UNKNOWN STRING " << LT(0)->getText();
            }
            importer->nat_a = "0.0.0.0";
        }
        (
            INT_CONST
            {
                if (LT(0)->getText() != std::string("0"))
                {
                    importer->reportError(
                        std::string("Unknown string ")+LT(0)->getText()+
                        std::string("; expected \"0\""));
                    *dbg << " UNKNOWN STRING " << LT(0)->getText();
                }
                importer->nat_nm = "0.0.0.0";
            }
        )?
    |
        ACCESS_LIST acl_name:WORD
        {
            importer->nat_acl = acl_name->getText();
        }
    ;

nat_command_last_parameters :
        // nat (real_ifc) nat_id real_ip [mask [dns] [outside] [[tcp] tcp_max_conns [emb_limit]] [udp udp_max_conns] [norandomseq]]
        //
        // here we deal with parameters starting with "dns". Note that compiler
        // does not know anything about max_conns and emb_limit options anyway
        //
        //
        //  <0-65535>    The maximum number of simultaneous TCP connections
        //  dns          Rewrite DNS address record
        //  norandomseq  Disable TCP sequence number randomization
        //  outside      Enable Outside NAT
        //  tcp          Configure TCP specific parameters
        //  udp          Configure UDP specific parameters
        // <cr>

        nat_and_static_command_common_last_parameters
    |
        OUTSIDE
    ;

nat_new_top_level_command :
        interface_label 
        COMMA
        interface_label 
        CLOSING_PAREN
        {
            importer->addMessageToLog(
                QString("Warning: Import of ASA 8.3 nat command "
                        "is not supported at this time"));
            consumeUntil(NEWLINE);
        }
    ;

global_top_level_command :
        GLOBAL
        OPENING_PAREN 
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
        interface_label
        {
            importer->tmp_global_pool.pool_interface = LT(0)->getText();
        }
        CLOSING_PAREN
        num:INT_CONST 
        {
            importer->tmp_global_pool.str_num = num->getText();
            importer->tmp_global_pool.netmask = "255.255.255.255";
            *dbg << " GLOBAL POOL "
                 << importer->tmp_global_pool.str_num
                 << " "
                 << importer->tmp_global_pool.pool_interface;
        }

        // WORD Enter IP address or a range of IP addresses <start_ip>[-<end_ip>]
        // interface  Specifies PAT using the IP address at the interface
        (
            INTRFACE
            {
                importer->tmp_global_pool.start = LT(0)->getText();
                importer->tmp_global_pool.end = LT(0)->getText();
            }
        |
            single_addr
            {
                importer->tmp_global_pool.start = importer->tmp_a;
                importer->tmp_global_pool.end = importer->tmp_a;
            }
        )

        (
            MINUS
            single_addr
            {
                importer->tmp_global_pool.end = importer->tmp_a;
            }
        )?

        // netmask  Specify netmask for the IP address(es) after this keyword
        // <cr>
        (
            NETMASK IPV4
            {
                importer->tmp_global_pool.netmask = LT(0)->getText();
            }
        )?

        NEWLINE
        {
            importer->addGlobalPool();
            *dbg << " " << importer->tmp_global_pool.start
                 << " " << importer->tmp_global_pool.end
                 << " " << importer->tmp_global_pool.netmask
                 << std::endl;
        }
    ;

static_top_level_command :
        STATIC
        OPENING_PAREN 
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
        }
        interface_label { importer->prenat_interface = LT(0)->getText(); }
        COMMA
        interface_label { importer->postnat_interface = LT(0)->getText(); }
        CLOSING_PAREN
        {
            importer->newUnidirRuleSet("nat", libfwbuilder::NAT::TYPENAME );
            *dbg << " DNAT rule ";
            importer->rule_type = libfwbuilder::NATRule::DNAT;
        }
        //  Hostname or A.B.C.D  Global or mapped address
        //  interface            Global address overload from interface
        //  tcp                  TCP to be used as transport protocol
        //  udp                  UDP to be used as transport protocol
        (
            static_starts_with_hostaddr
        |
            static_starts_with_tcp_udp
        )
        NEWLINE
        {
            importer->pushNATRule();
            *dbg << std::endl;
        }
    ;

static_starts_with_hostaddr : 
        static_mapped_addr_match

        //  Hostname or A.B.C.D  Real IP address of the host or hosts
        //  access-list          Configure access-list name after this keyw

        static_real_addr_match

        ( static_command_common_last_parameters )*
    ;

static_mapped_addr_match :
        (
            single_addr
            {
                importer->mapped_a = importer->tmp_a;
                importer->mapped_nm = importer->tmp_nm;
                *dbg << "mapped: " << importer->mapped_a;
            }
        |
            INTRFACE
            {
                importer->mapped_a = "interface";
                importer->mapped_nm = "";
                *dbg << "mapped: " << importer->mapped_a;
            }
        )
    ;

static_real_addr_match :
        (   
            single_addr   // real
            {
                importer->real_a = importer->tmp_a;
                importer->real_nm = importer->tmp_nm;
                *dbg << "real: " << importer->real_a;
            }
        |
            ACCESS_LIST acl_name:WORD
            {
                importer->real_addr_acl = acl_name->getText();
                *dbg << "real: " << importer->real_addr_acl;
            }
        )
    ;

static_starts_with_tcp_udp : ( TCP | UDP )
        {
            importer->protocol = LT(0)->getText();
            *dbg << " SERVICE TCP/UDP " << LT(0)->getText() << " ";
        }
        //  Hostname or A.B.C.D  Global or mapped address
        //  interface            Global address overload from interface

        static_mapped_addr_match

        // <0-65535>        Enter port number (0 - 65535)
        // aol              
        // bgp              
        // chargen          
        tcp_udp_port_spec
        {
            importer->mapped_port_spec = importer->tmp_port_spec_2;
            *dbg << "mapped port " << importer->mapped_port_spec << " ";
        }

        // Hostname or A.B.C.D  Real IP address of the host or hosts
        // access-list          Configure access-list name after this keyword

        (   
            single_addr   // real
            {
                importer->real_a = importer->tmp_a;
                importer->real_nm = importer->tmp_nm;
                *dbg << "real: " << importer->real_a;
            }

            // <0-65535>        Enter port number (0 - 65535)
            // aol              
            // bgp              
            // chargen          
            tcp_udp_port_spec
            {
                importer->real_port_spec = importer->tmp_port_spec_2;
                *dbg << "real port " << importer->real_port_spec << " ";
            }

        |
            ACCESS_LIST acl_name:WORD
            {
                importer->real_addr_acl = acl_name->getText();
                *dbg << "real: " << importer->real_addr_acl;
            }
        )

        // <0-65535>    The maximum number of simultaneous tcp connections the loc
        //               hosts are to allow, default is 0 which means unlimited
        //               connections. Idle connections are closed after the time
        //               specified by the timeout conn command
        //  dns          Use the created xlate to rewrite DNS address record
        //  netmask      Configure Netmask to apply to IP addresses
        //  norandomseq  Disable TCP sequence number randomization
        //  tcp          Configure TCP specific parameters
        //  udp          Configure UDP specific parameters

        ( static_command_common_last_parameters )*

    ;

static_command_common_last_parameters :
        // <0-65535>    The maximum number of simultaneous tcp connections
        //  dns          Use the created xlate to rewrite DNS address record
        //  netmask      Configure Netmask to apply to IP addresses
        //  norandomseq  Disable TCP sequence number randomization
        //  tcp          Configure TCP specific parameters
        //  udp          Configure UDP specific parameters
        //  <cr>

        nat_and_static_command_common_last_parameters
    |
        NETMASK nm:IPV4
        {
            importer->real_nm = nm->getText();
            *dbg << "real netmask: " << importer->real_nm;
        }
    ;

nat_and_static_command_common_last_parameters :
        // <0-65535>    The maximum number of simultaneous tcp connections
        //  dns          Use the created xlate to rewrite DNS address record
        //  netmask      Configure Netmask to apply to IP addresses
        //  norandomseq  Disable TCP sequence number randomization
        //  tcp          Configure TCP specific parameters
        //  udp          Configure UDP specific parameters
        //  <cr>

        DNS
        {
            importer->addMessageToLog(
                QString("Warning: 'nat' and 'static' command option 'dns' is not supported"));
        }
    |
        NORANDOMSEQ
        {
            importer->addMessageToLog(
                QString("Warning: 'nat' and 'static' command option 'norandomseq' is not supported"));
        }
    |
        (TCP | UDP)?
        max_conn:INT_CONST (max_emb_conn:INT_CONST)?
        {
            importer->static_max_conn = max_conn->getText();
            if (max_emb_conn)
                importer->static_max_emb_conn = max_emb_conn->getText();
        }
    ;

//****************************************************************

class PIXCfgLexer extends Lexer;
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

    HOSTNAME = "hostname";
    CERTIFICATE = "certificate";

    INTRFACE = "interface";
    CONTROLLER = "controller";
    DESCRIPTION = "description";
    REMARK = "remark";
    SHUTDOWN = "shutdown";
    SPEED = "speed";
    DUPLEX = "duplex";
    DELAY = "delay";
    DNS = "dns";
    DDNS = "ddns";
    FORWARD = "forward";
    HOLD_TIME = "hold-time";
    IPV6_C = "ipv6";
    MAC_ADDRESS = "mac-address";
    MANAGEMENT_ONLY = "management-only";

    MULTICAST = "multicast";
    NETMASK = "netmask";

    INTERVAL = "interval";

    OUTSIDE = "outside";

    VLAN = "vlan";
    SWITCHPORT = "switchport";
    ACCESS = "access";
    SEC_LEVEL = "security-level";

    ACCESS_LIST = "access-list";
    ACCESS_GROUP = "access-group";

    ADDRESS = "address";
    SECONDARY = "secondary";
    STANDBY = "standby";

    COMMUNITY_LIST = "community-list";

    PERMIT = "permit";
    DENY = "deny";

    DHCP = "dhcp";
    SETROUTE = "setroute";

// protocols for 'permit' and 'deny' commands

    IP = "ip";
    ICMP = "icmp";
    ICMP6 = "icmp6";
    TCP  = "tcp";
    UDP  = "udp";
    TCP_UDP = "tcp-udp";

    DESTINATION = "destination";
    SOURCE = "source";

    AH = "ah";
    EIGRP = "eigrp";
    ESP_WORD = "esp";
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

    P_EQ = "eq";
    P_GT = "gt";
    P_LT = "lt";
    P_NEQ = "neq";

    RANGE = "range";


    LOG = "log";
    LOG_INPUT = "log-input";

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

    ESTABLISHED = "established";
    FRAGMENTS = "fragments";
    TIME_RANGE = "time-range";

    EXTENDED = "extended" ;
    STANDARD = "standard" ;

    PIX_WORD = "PIX" ;
    ASA_WORD = "ASA" ;
    FWSM_WORD = "FWSM" ;

    VERSION_WORD_CAP = "Version" ;
    VERSION_WORD_LOW = "version" ;

    CRYPTO = "crypto";

//    OBJECT = "object";
//    OBJECT_GROUP = "object-group";

    GROUP_OBJECT = "group-object";
    NETWORK_OBJECT = "network-object";
    SERVICE_OBJECT = "service-object";
    PORT_OBJECT = "port-object";
    PROTOCOL_OBJECT = "protocol-object";
    ICMP_OBJECT = "icmp-object";
    ICMP_TYPE = "icmp-type";

    NETWORK = "network";
    SERVICE = "service";
    PROTOCOL = "protocol";

    SUBNET = "subnet";

    NAT = "nat";
    GLOBAL = "global";
    STATIC = "static";

    SSH = "ssh";
    TELNET = "telnet";

    AUI = "aui";
    AUTO = "auto";
    BNC = "bnc";
    BASET = "baseT";
    FULL = "full";
    BASETX = "baseTX";

  TIMEOUT = "timeout";

  ALTERNATE_ADDRESS = "alternate-address";
  CONVERSION_ERROR = "conversion-error";
  ECHO = "echo";
  ECHO_REPLY = "echo-reply";
  INFORMATION_REPLY = "information-reply";
  INFORMATION_REQUEST = "information-request";
  MASK_REPLY = "mask-reply";
  MASK_REQUEST = "mask-request";
  MOBILE_REDIRECT = "mobile-redirect";
  PARAMETER_PROBLEM = "parameter-problem";
  REDIRECT = "redirect";
  ROUTER_ADVERTISEMENT = "router-advertisement";
  ROUTER_SOLICITATION = "router-solicitation";
  SOURCE_QUENCH = "source-quench";
  TIME_EXCEEDED = "time-exceeded";
  TIMESTAMP_REPLY = "timestamp-reply";
  TIMESTAMP_REQUEST = "timestamp-request";
  TRACEROUTE = "traceroute";
  UNREACHABLE = "unreachable";

    NORANDOMSEQ = "norandomseq";

    SCOPY = "scopy";

    CALL_HOME = "call-home";
    INTERNAL = "internal";
    PASSWORD_RECOVERY = "password-recovery";
    RESETINBOUND = "resetinbound";
    RESETOUTBOUND = "resetoutbound";
    RESETOUTSIDE = "resetoutside";

    HTTP = "http";
    AUTHENTICATION_CERTIFICATE = "authentication-certificate";
    SERVER = "server";

    NAME = "name";
    NAMES = "names";
    NAMEIF = "nameif";

    OBJECT = "object";
    OBJECT_GROUP = "object-group";
}

LINE_COMMENT : "!" (~('\r' | '\n'))* NEWLINE ;

// This is for lines like these that appear at the top of "show run"
// : Saved
// :

COLON_COMMENT : COLON (~('\r' | '\n'))* NEWLINE ;

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

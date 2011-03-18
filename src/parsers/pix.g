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

#include "PIXImporter.h"
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
//    defaultErrorHandler=false;
}
{
// additional methods and members

    public:
    
    std::ostream *dbg;
    PIXImporter *importer;
}

cfgfile :
        (
            comment
        |
            version
        |
            hostname
        |
            ip_access_list_ext
        |
            community_list_command
        |
            unknown_ip_command 
        |
            intrface
        |
            controller
        |
            access_list_commands
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
            crypto
        |
            unknown_command
        |
            NEWLINE
        )+
    ;

//****************************************************************

// ip_commands : IP ( ip_access_list_ext | community_list_command | unknown_ip_command )
//    ;

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
names_section : NAMES
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                "Parser warning: \"names\" section detected. "
                "Import of configuration that uses \"names\" "
                "is not supported at this time");
        }
    ;

name_entry : NAME a:IPV4 n:WORD
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->addMessageToLog(
                "Name " + a->getText() + " " + n->getText());
            *dbg << "Name " << a->getText() << " " << n->getText() << std::endl;
        }
    ;

//****************************************************************

//
// these are used in access-list and named object definitions
//
ip_protocol_names : (
            AH | EIGRP | ESP | GRE |
            IGMP |  IGRP |  IP |  IPINIP |  IPSEC |
            NOS |  OSPF |  PCP |  PIM |  PPTP |  SNP )
    ;

//****************************************************************

//****************************************************************

named_object_network : OBJECT NETWORK name:WORD
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newNamedObjectAddress(name->getText());
            *dbg << name->getLine() << ":"
                << " Named Object " << name->getText() << std::endl;
        }
        (
            NEWLINE
            named_object_network_parameters
        )*
    ;

named_object_network_parameters : 
        (
            named_object_nat
        |
            host_addr
        |
            range_addr
        |
            subnet_addr
        | 
            named_object_description
        )
    ;

named_object_nat : NAT
        {
            importer->addMessageToLog(
                "Parser warning: "
                "Import of named objects with \"nat\" command "
                "is not supported at this time");
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
    ;

host_addr : (HOST h:IPV4)
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->tmp_a = h->getText();
            importer->tmp_nm = "255.255.255.255";
            importer->commitNamedAddressObject();
            *dbg << h->getText() << "/255.255.255.255";
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

subnet_addr : (SUBNET a:IPV4 nm:IPV4)
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->tmp_a = a->getText();
            importer->tmp_nm = nm->getText();
            importer->commitNamedAddressObject();
            *dbg << a->getText() << "/" << nm->getText();
        }
    ;
//****************************************************************

named_object_service : OBJECT SERVICE name:WORD
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newNamedObjectService(name->getText());
            *dbg << name->getLine() << ":"
                << " Named Object " << name->getText() << std::endl;
        }
        (
            NEWLINE
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
            named_object_description
        )
    ;

service_icmp : SERVICE ICMP
        (
            icmp_type:INT_CONST
            {
                importer->icmp_type = LT(0)->getText();
            }
        | icmp_word:WORD
            {
                importer->icmp_spec = icmp_word->getText();
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
            importer->addMessageToLog("Parser warning: "
                                      "Import of IPv6 addresses and servcies "
                                      "is not supported at this time");
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

dst_port_spec : DESTINATION xoperator
        {
            importer->SaveTmpPortToDst();
        }
    ;

service_other : SERVICE ( INT_CONST | ip_protocol_names )
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->protocol = LT(0)->getText();
            importer->commitNamedIPServiceObject();
            *dbg << "NAMED OBJECT SERVICE " << LT(0)->getText() << " ";
        }
    ;


//****************************************************************

object_group_network : OBJECT_GROUP NETWORK name:WORD
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newObjectGroupNetwork(name->getText());
            *dbg << name->getLine() << ":"
                 << " Object Group " << name->getText() << std::endl;
        }
        (
            object_group_network_parameters
        )+
    ;

object_group_network_parameters : 
        NEWLINE
        (
            object_group_description
        |
            group_object
        |
            network_object
        )
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
        a:IPV4 nm:IPV4
        {
            importer->tmp_a = a->getText();
            importer->tmp_nm = nm->getText();
            importer->addNetworkToObjectGroup();
            *dbg << a->getText() << "/" << nm->getText();
        }
    |
        HOST h:IPV4
        {
            importer->tmp_a = h->getText();
            importer->tmp_nm = "255.255.255.255";
            importer->addNetworkToObjectGroup();
            *dbg << h->getText() << "/255.255.255.255";
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

object_group_service : OBJECT_GROUP SERVICE name:WORD
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newObjectGroupService(name->getText());
            *dbg << name->getLine() << ":"
                 << " Object Group " << name->getText() << std::endl;
        }
        (
            object_group_service_parameters
        )+
    ;

object_group_service_parameters : 
        NEWLINE
        (
            object_group_description
        |
            group_object
        |
            service_object
        )
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
        ( TCP | UDP )
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
        | icmp_word:WORD
            {
                importer->icmp_spec = icmp_word->getText();
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
certificate : CERTIFICATE WORD 
        {
            consumeUntil(NEWLINE);
            consumeUntil(QUIT);
        }
    ;

//****************************************************************
version : (PIX_WORD | ASA_WORD) VERSION_WORD NUMBER
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->setDiscoveredVersion(LT(0)->getText());
            *dbg << "VERSION " << LT(0)->getText() << std::endl;
            consumeUntil(NEWLINE);
        }
    ;

//****************************************************************
hostname : HOSTNAME ( STRING | WORD )
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->setHostName( LT(0)->getText() );
            *dbg << "HOSTNAME "
                << "LT0=" << LT(0)->getText()
                << std::endl;
        }
    ;

//****************************************************************
// note that permit_ext and deny_ext eat NEWLINE. This is necessary
// because the same parser rules are used for ip access-list commands,
// where they should work the same way as LINE_COMMENT which eats
// NEWLINE
//
access_list_commands : ACCESS_LIST acl_num:INT_CONST
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newUnidirRuleSet( std::string("acl_") + acl_num->getText(),
                                        libfwbuilder::Policy::TYPENAME );
            *dbg << acl_num->getLine() << ":"
                << " ACL #" << acl_num->getText() << " ";
        }
        (
            permit_ext
        | 
            deny_ext
        |
            remark
        )
    ;

//****************************************************************

ip_access_list_ext : IP ACCESS_LIST name:WORD
        {
            importer->clear();
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newUnidirRuleSet(
                name->getText(), libfwbuilder::Policy::TYPENAME );
            *dbg << name->getLine() << ":"
                << " ACL ext " << name->getText() << std::endl;
        }
        (
            permit_ext
        |
            deny_ext
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
permit_ext: PERMIT 
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "permit";
            *dbg << LT(1)->getLine() << ":" << " permit ";
        }
        rule_ext NEWLINE
        {
            importer->pushRule();
        }
    ;

deny_ext: DENY
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newPolicyRule();
            importer->action = "deny";
            *dbg << LT(1)->getLine() << ":" << " deny   ";
        }
        rule_ext NEWLINE
        {
            importer->pushRule();
        }
    ;

//****************************************************************
// the difference between standard and extended acls should be in these rules
rule_ext : 
        (
            ip_protocols
            hostaddr_ext { importer->SaveTmpAddrToSrc(); *dbg << "(src) "; }
            hostaddr_ext { importer->SaveTmpAddrToDst(); *dbg << "(dst) "; }
            (time_range)?
            (fragments)?
            (log)?
        |
            ICMP
            {
                importer->protocol = LT(0)->getText();
                *dbg << "protocol " << LT(0)->getText() << " ";
            }
            hostaddr_ext { importer->SaveTmpAddrToSrc(); *dbg << "(src) "; }
            hostaddr_ext { importer->SaveTmpAddrToDst(); *dbg << "(dst) "; }
            (icmp_spec)?
            (time_range)?
            (fragments)?
            (log)?
        |
            (TCP|UDP)
            {
                importer->protocol = LT(0)->getText();
                *dbg << "protocol " << LT(0)->getText() << " ";
            }
            hostaddr_ext { importer->SaveTmpAddrToSrc(); *dbg << "(src) "; }
            (xoperator { importer->SaveTmpPortToSrc();  } )?
            hostaddr_ext { importer->SaveTmpAddrToDst(); *dbg << "(dst) "; }
            (xoperator { importer->SaveTmpPortToDst();  } )?
            (established)? 
            (time_range)?
            (fragments)?
            (log)?
        )
        {
            *dbg << std::endl;
        }
    ;

//****************************************************************
ip_protocols : ip_protocol_names
        {
            importer->protocol = LT(0)->getText();
            *dbg << "protocol " << LT(0)->getText() << " ";
        };

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
            icmp_word:WORD
            {
                importer->icmp_spec = icmp_word->getText();
                *dbg << icmp_word->getText() << " ";
            }
        )
    ;


xoperator : single_port_op | port_range  ;

single_port_op : (P_EQ | P_GT | P_LT | P_NEQ )
        {
            importer->tmp_port_op = LT(0)->getText();
            *dbg << LT(0)->getText() << " ";
        }
        port_spec
    ;

port_spec : (WORD|INT_CONST)
        {
            importer->tmp_port_spec = (std::string(" ") + LT(0)->getText());
            *dbg << LT(0)->getText() << " " << importer->tmp_port_spec;
        }
    ;

port_range : RANGE pair_of_ports_spec
        {
            importer->tmp_port_op = "range";
            *dbg << "range ";
        }
    ;

pair_of_ports_spec : (s1:WORD|s2:INT_CONST) (e1:WORD|e2:INT_CONST)
        {
            importer->tmp_port_spec = "";
            if (s1) importer->tmp_port_spec += s1->getText();
            if (s2) importer->tmp_port_spec += s2->getText();
            importer->tmp_port_spec += " ";
            if (e1) importer->tmp_port_spec += e1->getText();
            if (e2) importer->tmp_port_spec += e2->getText();
            *dbg << "pair of ports: " << importer->tmp_port_spec;
        }
    ;

hostaddr_ext :
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

hostaddr_std :
        (h:IPV4)
        {
            importer->tmp_a = h->getText();
            importer->tmp_nm = "0.0.0.0";
            *dbg << h->getText() << "/0.0.0.0";
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

log : (LOG | LOG_INPUT)
        {
            importer->logging = true;
            *dbg << "logging ";
        }
    ;

established : ESTABLISHED
        {
            importer->established = true;
            *dbg << "established ";
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

intrface  : INTRFACE in:WORD
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->newInterface( in->getText() );
            *dbg << in->getLine() << ":"
                << " INTRFACE: " << in->getText() << std::endl;
            consumeUntil(NEWLINE);
        }
        (
            interface_description
        )?
        (
            interface_parameters
        )+
        NEWLINE LINE_COMMENT
    ;
   
interface_parameters :
        NEWLINE
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
            IPV6
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

interface_no_commands : NO WORD
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
nameif  : NAMEIF p_intf:WORD 
        (
            ( WORD ) => intf_label:WORD sec_level:WORD |
            ( )
        )
        {
            std::string label = (intf_label) ? intf_label->getText() : "";
            std::string seclevel = (sec_level) ? sec_level->getText() : "";
            importer->setInterfaceParametes(p_intf->getText(), label, seclevel);
            *dbg << " NAMEIF: "
                 << p_intf->getText() << label << seclevel << std::endl;
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

v6_dhcp_address : lbl:WORD dhcp:DHCP
        {
            std::string label = lbl->getText();
            std::string addr = dhcp->getText();
            importer->addInterfaceAddress(label, addr, "");
            *dbg << LT(1)->getLine() << ":"
                 << " INTRFACE ADDRESS: " << addr << std::endl;
// there can be some other parameters after "dhcp", such as "setroute", "retry" etc.
// which we do not support
            consumeUntil(NEWLINE);
        }
    ;

v6_static_address : lbl:WORD a:IPV4 m:IPV4
        {
            std::string label = lbl->getText();
            std::string addr = a->getText();
            std::string netm = m->getText();
            importer->addInterfaceAddress(label, addr, netm);
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
                importer->addMessageToLog("Parser warning: failover IP detected. "
                                          "Failover is not supported by import "
                                          "at this time");
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

access_group_by_name : ACCESS_GROUP acln:WORD dir:WORD INTRFACE intf_label:WORD
        {
            importer->setCurrentLineNumber(LT(0)->getLine());
            importer->setInterfaceAndDirectionForRuleSet(
                acln->getText(),
                intf_label->getText(),
                dir->getText() );
            *dbg << LT(1)->getLine() << ":"
                << " INTRFACE: ACL '" << acln->getText() << "'"
                << " " << intf_label->getText()
                << " " << dir->getText() << std::endl;
        }
    ;

//****************************************************************

exit: EXIT
    ;

comment : (LINE_COMMENT | COLON_COMMENT) ;

// comment: COMMENT_START
//         {
//             *dbg << LT(1)->getLine() << ":";
//             std::string comm;
//             while (LA(1) != ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE && LA(1) != NEWLINE)
//             {
//                 comm += LT(1)->getText() + " ";
//                 consume();
//             }
//             importer->addInterfaceComment( comm );
//             *dbg << " COMMENT " << comm << std::endl;
//         }
//     ;


//****************************************************************

class PIXCfgLexer extends Lexer;
options
{
    k = 10;
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
    DDNS = "ddns";
    FORWARD = "forward";
    HOLD_TIME = "hold-time";
    IPV6 = "ipv6";
    MAC_ADDRESS = "mac-address";
    MULTICAST = "multicast";

    VLAN = "vlan";
    SWITCHPORT = "switchport";
    ACCESS = "access";
    NAMEIF = "nameif";
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
    TCP  = "tcp";
    UDP  = "udp";

    DESTINATION = "destination";
    SOURCE = "source";

    AHP = "ahp";
    EIGRP = "eigrp";
    ESP = "esp";
    GRE = "gre";
    IGMP = "igmp";
    IGRP = "igrp";
    IPINIP = "ipinip";
    NOS = "nos";
    OSPF = "ospf";
    PCP = "pcp";
    PIM = "pim";
    RIP = "rip";

    HOST = "host";
    ANY  = "any";

    P_EQ = "eq";
    P_GT = "gt";
    P_LT = "lt";
    P_NEQ = "neq";

    RANGE = "range";

    LOG = "log";
    LOG_INPUT = "log-input";

    ESTABLISHED = "established";
    FRAGMENTS = "fragments";
    TIME_RANGE = "time-range";

    EXTENDED = "extended" ;
    STANDARD = "standard" ;

    PIX_WORD = "PIX" ;
    ASA_WORD = "ASA" ;
    VERSION_WORD = "Version" ;

    CRYPTO = "crypto";

    NAMES = "names";
    NAME = "name";

    OBJECT = "object";
    OBJECT_GROUP = "object-group";
    GROUP_OBJECT = "group-object";
    NETWORK_OBJECT = "network-object";
    SERVICE_OBJECT = "service-object";

    NETWORK = "network";
    SERVICE = "service";

    SUBNET = "subnet";

    NAT = "nat";
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
NEG_INT_CONST:;

protected
DIGIT : '0'..'9'  ;

protected
HEXDIGIT : '0'..'9' | 'A'..'F' ;

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

WORD : ( 'a'..'z' | 'A'..'Z' | '$' ) ( '!'..'/' | '0'..'9' | ':' | ';' | '<' | '=' | '>' | '?' | '@' | 'A'..'Z' | '\\' | '^' | '_' | '`' | 'a'..'z' )*
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

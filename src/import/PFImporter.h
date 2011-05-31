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


#ifndef _FWB_POLICY_IMPORTER_PF_H_
#define _FWB_POLICY_IMPORTER_PF_H_

#include <map>
#include <list>
#include <string>
#include <functional>
#include <sstream>

#include "IOSImporter.h"

#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/Logger.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/NAT.h"

#include <QString>


class InterfaceSpec
{
public:

    bool neg;
    std::string name;

    InterfaceSpec()
    { neg = false; name = ""; }

    InterfaceSpec(const InterfaceSpec &other)
    {
        neg = other.neg;
        name = other.name;
    }
    
    InterfaceSpec(bool _neg, const std::string _name)
    { neg = _neg; name = _name; }
};



class AddressSpec
{
public:
    
    typedef enum {
        UNKNOWN,
        ANY,
        HOST_NAME,
        HOST_ADDRESS,
        NETWORK_ADDRESS,
        SPECIAL_ADDRESS,
        INTERFACE_NAME,
        INTERFACE_NETWORK,
        INTERFACE_BROADCAST,
        TABLE } address_type;

    address_type at;
    bool neg;
    std::string address;
    std::string netmask;

    AddressSpec()
    { at = UNKNOWN; neg = false;  address = ""; netmask = ""; }

    AddressSpec(const AddressSpec &other)
    {
        at = other.at;
        neg = other.neg;
        address = other.address;
        netmask = other.netmask;
    }
    
    AddressSpec(address_type _at, bool _neg, const std::string _addr, const std::string _nm)
    { at = _at; neg= _neg; address = _addr; netmask = _nm; }
};


class PortSpec
{
public:
    std::string port1;
    std::string port2;
    std::string port_op;

    PortSpec()
    { port1 = ""; port2 = ""; port_op = ""; }

    PortSpec(const PortSpec &other)
    {
        port1 = other.port1;
        port2 = other.port2;
        port_op = other.port_op;
    }
    
    PortSpec(const std::string s1, const std::string s2, const std::string s3)
    { port1 = s1; port2 = s2; port_op = s3; }

    std::string toString()
    { return std::string("PortSpec: ") + port_op + " " + port1 + " " + port2; }
};

class IcmpSpec
{
public:
    std::string icmp_type_name;
    std::string icmp_type_int;
    std::string icmp_code_name;
    std::string icmp_code_int;

    IcmpSpec()
    {
        icmp_type_name = "";
        icmp_type_int = "";
        icmp_code_name = "";
        icmp_code_int = "";
    }

    IcmpSpec(const IcmpSpec &other)
    {
        icmp_type_name = other.icmp_type_name;
        icmp_type_int = other.icmp_type_int;
        icmp_code_name = other.icmp_code_name;
        icmp_code_int = other.icmp_code_int;
    }
    
    IcmpSpec(const std::string s1, const std::string s2,
             const std::string s3, const std::string s4)
    {
        icmp_type_name = s1;
        icmp_type_int = s2;
        icmp_code_name = s3;
        icmp_code_int = s4;
    }
};


class RouteSpec
{
public:
    
    std::string iface;
    std::string address;
    std::string netmask;

    RouteSpec()
    { iface = ""; address = ""; netmask = ""; }

    RouteSpec(const RouteSpec &other)
    {
        iface = other.iface;
        address = other.address;
        netmask = other.netmask;
    }
    
    RouteSpec(const std::string _iface,
              const std::string _addr, const std::string _nm)
    { iface = _iface; address = _addr; netmask = _nm; }
};




class PFImporter : public Importer
{

    void addServiceObjectsToRE(libfwbuilder::RuleElement *re);
    void addTCPUDPServiceObjectsToRE(
        libfwbuilder::RuleElement *re,
        const std::string &protocol,
        std::list< PortSpec > &port_spec_list,
        bool source,
        bool for_nat_rhs);
    
public:

    typedef enum {
        UNKNOWN,
        ROUTE_TO,
        REPLY_TO,
        DUP_TO} route_op_type;
    
    QMap<QString,libfwbuilder::FWObject*> address_table_registry;

    std::string direction;
    std::string address_family;
    bool quick;
    bool src_neg;
    bool dst_neg;
    bool tmp_neg;

    std::map<std::string, std::string> icmp_codes_to_reject_parameters;
    std::list<std::string> block_action_params;
    
    std::list<InterfaceSpec> iface_group;
    
    std::list<std::string> proto_list;
    std::list< AddressSpec > src_group;
    std::list< AddressSpec > dst_group;
    std::list< AddressSpec > nat_group;
    std::list< AddressSpec > tmp_group;

    std::string tmp_port_op;
    std::string tmp_port_def;
    std::list< PortSpec > src_port_group;
    std::list< PortSpec > dst_port_group;
    std::list< PortSpec > nat_port_group;
    std::list< PortSpec > tmp_port_group;

    std::list< IcmpSpec >  icmp_type_code_group;

    route_op_type route_type;
    std::list<RouteSpec> route_group;
    
    std::string queue;
    std::string state_op;
    std::string logopts;
    std::string flags_check;
    std::string flags_mask;
    
    std::string tag;

    bool tagged_neg;
    std::string tagged;

    std::string nat_pool_type;
    
    libfwbuilder::NATRule::NATRuleTypes rule_type;
    
    PFImporter(libfwbuilder::FWObject *lib,
                std::istringstream &input,
                libfwbuilder::Logger *log,
                const std::string &fwname);
    ~PFImporter();

    virtual void clear();

    void clearTempVars();
    
    virtual void run();

    void pushPolicyRule();
    void pushNATRule();
    void buildDNATRule();
    void buildSNATRule();
    virtual void pushRule();
    
    // this method actually adds interfaces to the firewall object
    // and does final clean up.
    virtual libfwbuilder::Firewall* finalize();

    virtual libfwbuilder::FWObject* makeAddressObj(AddressSpec &as);
    
    virtual void addSrc();
    virtual void addDst();
    virtual void addSrv();

    virtual void addOSrc();
    virtual void addODst();
    virtual void addOSrv();

    virtual void addTSrc();
    virtual void addTDst();
    virtual void addTSrv();

    virtual void addLogging();

    libfwbuilder::Interface* getInterfaceByName(const std::string &name);

    void newAddressTableObject(const std::string &name, const std::string &file);
    void newAddressTableObject(const std::string &name,
                               std::list<AddressSpec> &addresses);


    bool buildTCPUDPObjectSingature(ObjectSignature *sig,
                                    const QString &port_op,
                                    const QString &port_spec,
                                    bool source,
                                    const QString &protocol,
                                    const QString &flags_check,
                                    const QString &flags_mask);
    
    void convertTcpFlags(QList<int> &flags_list, const QString &flags_str);
};

#endif

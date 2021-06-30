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
#include "AddressSpec.h"
#include "InterfaceSpec.h"
#include "PortSpec.h"
#include "IcmpSpec.h"
#include "RouteSpec.h"

#include "fwbuilder/Logger.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/NAT.h"

#include <QString>







class PFImporter : public Importer
{

    void addServiceObjectsToRE(libfwbuilder::RuleElement *re);
    void addTCPUDPServiceObjectsToRE(
        libfwbuilder::RuleElement *re,
        const std::string &protocol,
        const std::list< PortSpec > &src_port_spec_list,
        const std::list< PortSpec > &dst_port_spec_list,
        bool for_nat_rhs);

    void substituteMacros(const QMap<QString,QString> &macros,
                          QString &buffer);

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

    std::string pooltype_opt;
    std::string nat_rule_opt_2;
    
    libfwbuilder::NATRule::NATRuleTypes rule_type;

    std::list<str_tuple> timeouts;
    std::list<str_tuple> limits;
    std::string set_optimization;
    std::string set_block_policy;
    std::string set_state_policy;
    std::string set_debug;

    std::list<std::string> set_skip_on;

    std::list<str_tuple> scrub_options;
    bool scrub_rule;

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

    void addTSrvSNAT();
    void addTSrvDNAT();

    virtual void addLogging();

    libfwbuilder::Interface* getInterfaceByName(const std::string &name);

    void newAddressTableObject(const std::string &name, const std::string &file);
    void newAddressTableObject(const std::string &name,
                               std::list<AddressSpec> &addresses);


    bool buildTCPUDPObjectSingature(ObjectSignature *sig,
                                    const PortSpec &src_port,
                                    const PortSpec &dst_port,
                                    const QString &protocol,
                                    const QString &flags_check,
                                    const QString &flags_mask);
    
    void convertTcpFlags(QList<int> &flags_list, const QString &flags_str);
};

#endif

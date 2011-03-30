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


#ifndef _FWB_POLICY_IMPORTER_PIX_H_
#define _FWB_POLICY_IMPORTER_PIX_H_

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


class PIXImporter : public IOSImporter
{
    public:

    QString named_object_name;
    QString named_object_comment;
    libfwbuilder::FWObject *current_named_object;

    QString object_group_name;
    QString object_group_comment;
    QString object_group_service_protocol;
    libfwbuilder::FWObject *current_object_group;

    QMap<QString,libfwbuilder::FWObject*> named_objects_registry;

    // variables for the nat rules
    libfwbuilder::NATRule::NATRuleTypes rule_type;
    std::string prenat_interface;
    std::string postnat_interface;

    std::string real_a;
    std::string real_nm;
    std::string mapped_a;
    std::string mapped_nm;
    std::string real_addr_acl;
    std::string mapped_port_spec;
    std::string real_port_spec;
    std::string static_max_conn;
    std::string static_max_emb_conn;

    std::string nat_num;
    std::string nat_a;
    std::string nat_nm;
    std::string nat_acl;

    std::string global_pool_num;
    std::string global_interface;
    
    PIXImporter(libfwbuilder::FWObject *lib,
                std::istringstream &input,
                libfwbuilder::Logger *log,
                const std::string &fwname);
    ~PIXImporter();

    virtual void clear();

    void clearTempVars();
    
    virtual void run();

    void pushPolicyRule();
    void pushNATRule();
    virtual void pushRule();
    
    // this method actually adds interfaces to the firewall object
    // and does final clean up.
    virtual libfwbuilder::Firewall* finalize();

    virtual libfwbuilder::FWObject* makeSrcObj();
    virtual libfwbuilder::FWObject* makeDstObj();
    virtual libfwbuilder::FWObject* makeSrvObj();

    virtual void addLogging();
    
    /*
     * the difference is that in PIX, we get interface label instead
     * of its name in "access-group" command
     */
    virtual void setInterfaceAndDirectionForRuleSet(
        const std::string &ruleset_name,
        const std::string &interface_name,
        const std::string &dir);
    
    virtual void newNamedObjectAddress(const std::string &name);
    virtual void newNamedObjectService(const std::string &name);

    /*
     * it looks like "description" line is always the last in the
     * named object block output of "show run" command on ASA, however
     * "description" is optional and we create the object when we see
     * "subnet", "host" or "service" line. This function adds
     * description to existing named object.
     */
    virtual void setNamedObjectDescription(const std::string &txt);
    
    virtual void commitNamedAddressObject();
    virtual void commitNamedAddressRangeObject();
    virtual void commitNamedIPServiceObject();
    virtual void commitNamedICMPServiceObject();
    virtual void commitNamedTCPUDPServiceObject();

    virtual libfwbuilder::FWObject* commitObject(libfwbuilder::FWObject *obj);
    virtual libfwbuilder::FWObject* setNameOfNamedObject(
        libfwbuilder::FWObject *obj);

    void newObjectGroupNetwork(const std::string &group_name);
    void newObjectGroupService(const std::string &group_name);
    void newObjectGroupProtocol(const std::string &group_name);
    void newObjectGroupICMP(const std::string &group_name);

    bool isKnownServiceGroupName(const std::string &object_group_name);
    
    void setObjectGroupServiceProtocol(const std::string &proto)
    { object_group_service_protocol = proto.c_str(); }
    
    void setObjectGroupDescription(const std::string &descr);
    
    void addNamedObjectToObjectGroup(const std::string &object_name);

    void addNetworkToObjectGroup();
    void addIPServiceToObjectGroup();
    void addTCPUDPServiceToObjectGroup();
    void addICMPServiceToObjectGroup();
    
        
    void rearrangeVlanInterfaces();
};

#endif

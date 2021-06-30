/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org
           Vadim Kurland vadim@fwbuilder.org


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <cstring>
#include <signal.h>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWIntervalReference.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/FWServiceReference.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"

#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/DynamicGroup.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/physAddress.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace libfwbuilder;


static std::map<std::string, create_function_ptr> create_methods;


#define CREATE_OBJ_METHOD(classname) \
FWObject* libfwbuilder::create_##classname(int id) \
{ \
    classname *nobj = new classname(); \
    if (id > -1) nobj->setId(id); \
    return nobj; \
} \
\
classname * FWObjectDatabase::create##classname(int id) \
{ \
    classname * nobj = classname::cast(create_##classname(id)); \
    addToIndex(nobj); \
    nobj->init(this); \
    return nobj; \
}



void FWObjectDatabase::registerObjectType(const std::string &type_name,
                                          create_function_ptr create_function)
{
    create_methods[type_name] = create_function;
}
        

void FWObjectDatabase::init_create_methods_table()
{
    if (create_methods.size()==0)
    {
        registerObjectType("AddressRange",
                           &create_AddressRange);

        registerObjectType("AddressTable",
                           &create_AddressTable);
        registerObjectType("AttachedNetworks",
                           &create_AttachedNetworks);
        registerObjectType("Cluster",
                           &create_Cluster);
        registerObjectType("StateSyncClusterGroup",
                           &create_StateSyncClusterGroup);
        registerObjectType("FailoverClusterGroup",
                           &create_FailoverClusterGroup);
        registerObjectType("ClusterGroupOptions",
                           &create_ClusterGroupOptions);
        registerObjectType("CustomService",
                           &create_CustomService);
        registerObjectType("DNSName",
                           &create_DNSName);
        registerObjectType("DynamicGroup",
                           &create_DynamicGroup);
        registerObjectType("FWBDManagement",
                           &create_FWBDManagement);
        registerObjectType("IntervalRef",
                           &create_FWIntervalReference);
        registerObjectType("ObjectRef",
                           &create_FWObjectReference);
        registerObjectType("ServiceRef",
                           &create_FWServiceReference);
        registerObjectType("Firewall",
                           &create_Firewall);
        registerObjectType("FirewallOptions",
                           &create_FirewallOptions);
        registerObjectType("Host",
                           &create_Host);
        registerObjectType("HostOptions",
                           &create_HostOptions);
        registerObjectType("ICMP6Service",
                           &create_ICMP6Service);
        registerObjectType("ICMPService",
                           &create_ICMPService);
        registerObjectType("InterfaceOptions",
                           &create_InterfaceOptions);
        registerObjectType("IPService",
                           &create_IPService);
        registerObjectType("IPv4",
                           &create_IPv4);
        registerObjectType("IPv6",
                           &create_IPv6);
        registerObjectType("Interface",
                           &create_Interface);
        registerObjectType("Interval",
                           &create_Interval);
        registerObjectType("IntervalGroup",
                           &create_IntervalGroup);
        registerObjectType("Library",
                           &create_Library);
        registerObjectType("Management",
                           &create_Management);
        registerObjectType("NAT",
                           &create_NAT);
        registerObjectType("NATRule",
                           &create_NATRule);
        registerObjectType("NATRuleOptions",
                           &create_NATRuleOptions);
        registerObjectType("Network",
                           &create_Network);
        registerObjectType("NetworkIPv6",
                           &create_NetworkIPv6);
        registerObjectType("ObjectGroup",
                           &create_ObjectGroup);
        registerObjectType("Policy",
                           &create_Policy);
        registerObjectType("PolicyInstallScript",
                           &create_PolicyInstallScript);
        registerObjectType("PolicyRule",
                           &create_PolicyRule);
        registerObjectType("PolicyRuleOptions",
                           &create_PolicyRuleOptions);
        registerObjectType("Routing",
                           &create_Routing);
        registerObjectType("RoutingRule",
                           &create_RoutingRule);
        registerObjectType("RoutingRuleOptions",
                           &create_RoutingRuleOptions);
        registerObjectType("RuleSetOptions",
                           &create_RuleSetOptions);

        registerObjectType("Dst",
                           &create_RuleElementDst);
        registerObjectType("When",
                           &create_RuleElementInterval);
        registerObjectType("Itf",
                           &create_RuleElementItf);
        registerObjectType("ItfInb",
                           &create_RuleElementItfInb);
        registerObjectType("ItfOutb",
                           &create_RuleElementItfOutb);
        registerObjectType("ODst",
                           &create_RuleElementODst);
        registerObjectType("OSrc",
                           &create_RuleElementOSrc);
        registerObjectType("OSrv",
                           &create_RuleElementOSrv);
        registerObjectType("RDst",
                           &create_RuleElementRDst);
        registerObjectType("RGtw",
                           &create_RuleElementRGtw);
        registerObjectType("RItf",
                           &create_RuleElementRItf);
        registerObjectType("Src",
                           &create_RuleElementSrc);
        registerObjectType("Srv",
                           &create_RuleElementSrv);
        registerObjectType("TDst",
                           &create_RuleElementTDst);
        registerObjectType("TSrc",
                           &create_RuleElementTSrc);
        registerObjectType("TSrv",
                           &create_RuleElementTSrv);

        registerObjectType("SNMPManagement",
                           &create_SNMPManagement);
        registerObjectType("ServiceGroup",
                           &create_ServiceGroup);
        registerObjectType("TCPService",
                           &create_TCPService);
        registerObjectType("TagService",
                           &create_TagService);
        registerObjectType("UDPService",
                           &create_UDPService);
        registerObjectType("UserService",
                           &create_UserService);
        registerObjectType("physAddress",
                           &create_physAddress);

        registerObjectType("Group",
                           &create_Group);
    }
}

FWObject *FWObjectDatabase::create(const string &type_name, int id, bool init)
{
    create_function_ptr fn = create_methods[type_name];
    if (fn == nullptr)
    {
        const char *type_name_cptr = type_name.c_str();
        FWObject   *nobj;

        if (strcmp("comment", type_name_cptr)==SAME) return nullptr;
        if (strcmp("SelectionCriteria", type_name_cptr) == 0) return 0;

        if (strcmp("AnyNetwork", type_name_cptr)==SAME)
        {
            nobj = new Network();
            if (id > -1) nobj->setId(id);
            nobj->setXMLName("AnyNetwork");
            addToIndex(nobj);
            return nobj;
        }

        if (strcmp("AnyIPService", type_name_cptr)==SAME)
        {
            nobj = new IPService();
            if (id > -1) nobj->setId(id);
            nobj->setXMLName("AnyIPService");
            addToIndex(nobj);
            return nobj;
        }

        if (strcmp("AnyInterval", type_name_cptr)==SAME)
        {
            nobj = new Interval();
            if (id > -1) nobj->setId(id);
            nobj->setXMLName("AnyInterval");
            addToIndex(nobj);
            return nobj;
        }

        if (strcmp("DummyNetwork", type_name_cptr)==SAME)
        {
            nobj = new Network();
            if (id > -1) nobj->setId(id);
            nobj->setXMLName("DummyNetwork");
            addToIndex(nobj);
            return nobj;
        }

        if (strcmp("DummyIPService", type_name_cptr)==SAME)
        {
            nobj = new IPService();
            if (id > -1) nobj->setId(id);
            nobj->setXMLName("DummyIPService");
            addToIndex(nobj);
            return nobj;
        }

        if (strcmp("DummyInterface", type_name_cptr)==SAME)
        {
            nobj = new Interface();
            if (id > -1) nobj->setId(id);
            nobj->setXMLName("DummyInterface");
            addToIndex(nobj);
            return nobj;
        }

        cerr << "Do not have method to create object of type "
             << type_name << endl;
        return nullptr;
    }

    FWObject *nobj = (*fn)(id);

    addToIndex(nobj);
    if (init) nobj->init(this);
    return nobj;
}

FWObject *FWObjectDatabase::createFromXML(xmlNodePtr data)
{
    const char *n;
    string typen;
    int id = -1;

    n = XMLTools::FromXmlCast(data->name);
    if (!n) return nullptr;
    typen = n;

    n = XMLTools::FromXmlCast(xmlGetProp(data, XMLTools::ToXmlCast("id")));
    if (n)
    {
        id = registerStringId(n);
        XMLTools::FreeXmlBuff(n);
    }

// create new object but do not prepopulate objects that
// automatically create children in constructor
    return create(typen, id, false);
}

CREATE_OBJ_METHOD(AddressRange);
CREATE_OBJ_METHOD(AddressTable);
CREATE_OBJ_METHOD(AttachedNetworks);
CREATE_OBJ_METHOD(Cluster);
CREATE_OBJ_METHOD(StateSyncClusterGroup);
CREATE_OBJ_METHOD(FailoverClusterGroup);
CREATE_OBJ_METHOD(ClusterGroupOptions);
CREATE_OBJ_METHOD(CustomService);
CREATE_OBJ_METHOD(DNSName);
CREATE_OBJ_METHOD(DynamicGroup);
CREATE_OBJ_METHOD(FWBDManagement);
CREATE_OBJ_METHOD(FWIntervalReference);
CREATE_OBJ_METHOD(FWObjectReference);
CREATE_OBJ_METHOD(FWServiceReference);
CREATE_OBJ_METHOD(Firewall);
CREATE_OBJ_METHOD(FirewallOptions);
CREATE_OBJ_METHOD(InterfaceOptions);
CREATE_OBJ_METHOD(Host);
CREATE_OBJ_METHOD(HostOptions);
CREATE_OBJ_METHOD(ICMP6Service);
CREATE_OBJ_METHOD(ICMPService);
CREATE_OBJ_METHOD(IPService);
CREATE_OBJ_METHOD(IPv4);
CREATE_OBJ_METHOD(IPv6);
CREATE_OBJ_METHOD(Interface);
CREATE_OBJ_METHOD(Interval);
CREATE_OBJ_METHOD(IntervalGroup);
CREATE_OBJ_METHOD(Library);
CREATE_OBJ_METHOD(Management);
CREATE_OBJ_METHOD(NAT);
CREATE_OBJ_METHOD(NATRule);
CREATE_OBJ_METHOD(NATRuleOptions);
CREATE_OBJ_METHOD(Network);
CREATE_OBJ_METHOD(NetworkIPv6);
CREATE_OBJ_METHOD(ObjectGroup);
CREATE_OBJ_METHOD(Policy);
CREATE_OBJ_METHOD(PolicyInstallScript);
CREATE_OBJ_METHOD(PolicyRule);
CREATE_OBJ_METHOD(PolicyRuleOptions);
CREATE_OBJ_METHOD(Routing);
CREATE_OBJ_METHOD(RoutingRule);
CREATE_OBJ_METHOD(RoutingRuleOptions);
CREATE_OBJ_METHOD(RuleElementDst);
CREATE_OBJ_METHOD(RuleElementInterval);
CREATE_OBJ_METHOD(RuleElementItf);
CREATE_OBJ_METHOD(RuleElementItfInb);
CREATE_OBJ_METHOD(RuleElementItfOutb);
CREATE_OBJ_METHOD(RuleElementODst);
CREATE_OBJ_METHOD(RuleElementOSrc);
CREATE_OBJ_METHOD(RuleElementOSrv);
CREATE_OBJ_METHOD(RuleElementRDst);
CREATE_OBJ_METHOD(RuleElementRGtw);
CREATE_OBJ_METHOD(RuleElementRItf);
CREATE_OBJ_METHOD(RuleElementSrc);
CREATE_OBJ_METHOD(RuleElementSrv);
CREATE_OBJ_METHOD(RuleElementTDst);
CREATE_OBJ_METHOD(RuleElementTSrc);
CREATE_OBJ_METHOD(RuleElementTSrv);
CREATE_OBJ_METHOD(RuleSetOptions);
CREATE_OBJ_METHOD(SNMPManagement);
CREATE_OBJ_METHOD(ServiceGroup);
CREATE_OBJ_METHOD(TCPService);
CREATE_OBJ_METHOD(TagService);
CREATE_OBJ_METHOD(UDPService);
CREATE_OBJ_METHOD(UserService);
CREATE_OBJ_METHOD(physAddress);
CREATE_OBJ_METHOD(Group);

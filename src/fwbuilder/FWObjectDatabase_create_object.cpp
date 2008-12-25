/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org
           Vadim Kurland vadim@fwbuilder.org

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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/FWObject.h>
#include <fwbuilder/FWObjectDatabase.h>

#include <fwbuilder/Library.h>
#include <fwbuilder/Interval.h>
#include <fwbuilder/ICMPService.h>
#include <fwbuilder/ICMP6Service.h>
#include <fwbuilder/IPService.h>
#include <fwbuilder/TCPService.h>
#include <fwbuilder/UDPService.h>
#include <fwbuilder/CustomService.h>
#include <fwbuilder/FWReference.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWServiceReference.h>
#include <fwbuilder/FWIntervalReference.h>
#include <fwbuilder/Host.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/IPv4.h>
#include <fwbuilder/IPv6.h>
#include <fwbuilder/physAddress.h>
#include <fwbuilder/DNSName.h>
#include <fwbuilder/AddressTable.h>
#include <fwbuilder/Group.h>
#include <fwbuilder/Rule.h>
#include <fwbuilder/RuleElement.h>
#include <fwbuilder/RuleSet.h>
#include <fwbuilder/FWOptions.h>
#include <fwbuilder/Firewall.h>
#include <fwbuilder/NAT.h>
#include <fwbuilder/Policy.h>
#include <fwbuilder/Routing.h>
#include <fwbuilder/ObjectGroup.h>
#include <fwbuilder/ServiceGroup.h>
#include <fwbuilder/IntervalGroup.h>
#include <fwbuilder/Network.h>
#include <fwbuilder/NetworkIPv6.h>
#include <fwbuilder/AddressRange.h>
#include <fwbuilder/Management.h>
#include <fwbuilder/XMLTools.h>
#include <fwbuilder/TagService.h>
#include <fwbuilder/UserService.h>

#include <iostream>
#include <sstream>

using namespace std;
using namespace libfwbuilder;


static std::map<std::string, create_function_ptr> create_methods;


#define CREATE_OBJ_METHOD(classname)     \
classname * FWObjectDatabase::create##classname(int id, bool prepopulate) \
{ \
  classname *nobj = new classname(this, prepopulate); \
  if (id > -1) nobj->setId(id); \
  addToIndex(nobj); \
  return nobj; \
} \
\
FWObject* FWObjectDatabase::createFWObject##classname(int id, bool prepopulate)\
{ return create##classname(id, prepopulate); }




void FWObjectDatabase::init_create_methods_table()
{
    if (create_methods.size()==0)
    {
        create_methods["AddressRange"] =
            &FWObjectDatabase::createFWObjectAddressRange;
        create_methods["AddressTable"] =
            &FWObjectDatabase::createFWObjectAddressTable;
        create_methods["CustomService"] =
            &FWObjectDatabase::createFWObjectCustomService;
        create_methods["DNSName"] =
            &FWObjectDatabase::createFWObjectDNSName;
        create_methods["FWBDManagement"] =
            &FWObjectDatabase::createFWObjectFWBDManagement;
        create_methods["IntervalRef"] =
            &FWObjectDatabase::createFWObjectFWIntervalReference;
        create_methods["ObjectRef"] =
            &FWObjectDatabase::createFWObjectFWObjectReference;
        create_methods["ServiceRef"] =
            &FWObjectDatabase::createFWObjectFWServiceReference;
        create_methods["Firewall"] =
            &FWObjectDatabase::createFWObjectFirewall;
        create_methods["FirewallOptions"] =
            &FWObjectDatabase::createFWObjectFirewallOptions;
        create_methods["Host"] =
            &FWObjectDatabase::createFWObjectHost;
        create_methods["HostOptions"] =
            &FWObjectDatabase::createFWObjectHostOptions;
        create_methods["ICMP6Service"] =
            &FWObjectDatabase::createFWObjectICMP6Service;
        create_methods["ICMPService"] =
            &FWObjectDatabase::createFWObjectICMPService;
        create_methods["IPService"] =
            &FWObjectDatabase::createFWObjectIPService;
        create_methods["IPv4"] =
            &FWObjectDatabase::createFWObjectIPv4;
        create_methods["IPv6"] =
            &FWObjectDatabase::createFWObjectIPv6;
        create_methods["Interface"] =
            &FWObjectDatabase::createFWObjectInterface;
        create_methods["Interval"] =
            &FWObjectDatabase::createFWObjectInterval;
        create_methods["IntervalGroup"] =
            &FWObjectDatabase::createFWObjectIntervalGroup;
        create_methods["Library"] =
            &FWObjectDatabase::createFWObjectLibrary;
        create_methods["Management"] =
            &FWObjectDatabase::createFWObjectManagement;
        create_methods["NAT"] =
            &FWObjectDatabase::createFWObjectNAT;
        create_methods["NATRule"] =
            &FWObjectDatabase::createFWObjectNATRule;
        create_methods["NATRuleOptions"] =
            &FWObjectDatabase::createFWObjectNATRuleOptions;
        create_methods["Network"] =
            &FWObjectDatabase::createFWObjectNetwork;
        create_methods["NetworkIPv6"] =
            &FWObjectDatabase::createFWObjectNetworkIPv6;
        create_methods["ObjectGroup"] =
            &FWObjectDatabase::createFWObjectObjectGroup;
        create_methods["Policy"] =
            &FWObjectDatabase::createFWObjectPolicy;
        create_methods["PolicyInstallScript"] =
            &FWObjectDatabase::createFWObjectPolicyInstallScript;
        create_methods["PolicyRule"] =
            &FWObjectDatabase::createFWObjectPolicyRule;
        create_methods["PolicyRuleOptions"] =
            &FWObjectDatabase::createFWObjectPolicyRuleOptions;
        create_methods["Routing"] =
            &FWObjectDatabase::createFWObjectRouting;
        create_methods["RoutingRule"] =
            &FWObjectDatabase::createFWObjectRoutingRule;
        create_methods["RoutingRuleOptions"] =
            &FWObjectDatabase::createFWObjectRoutingRuleOptions;

        create_methods["Dst"] =
            &FWObjectDatabase::createFWObjectRuleElementDst;
        create_methods["When"] =
            &FWObjectDatabase::createFWObjectRuleElementInterval;
        create_methods["Itf"] =
            &FWObjectDatabase::createFWObjectRuleElementItf;
        create_methods["ODst"] =
            &FWObjectDatabase::createFWObjectRuleElementODst;
        create_methods["OSrc"] =
            &FWObjectDatabase::createFWObjectRuleElementOSrc;
        create_methods["OSrv"] =
            &FWObjectDatabase::createFWObjectRuleElementOSrv;
        create_methods["RDst"] =
            &FWObjectDatabase::createFWObjectRuleElementRDst;
        create_methods["RGtw"] =
            &FWObjectDatabase::createFWObjectRuleElementRGtw;
        create_methods["RItf"] =
            &FWObjectDatabase::createFWObjectRuleElementRItf;
        create_methods["Src"] =
            &FWObjectDatabase::createFWObjectRuleElementSrc;
        create_methods["Srv"] =
            &FWObjectDatabase::createFWObjectRuleElementSrv;
        create_methods["TDst"] =
            &FWObjectDatabase::createFWObjectRuleElementTDst;
        create_methods["TSrc"] =
            &FWObjectDatabase::createFWObjectRuleElementTSrc;
        create_methods["TSrv"] =
            &FWObjectDatabase::createFWObjectRuleElementTSrv;

        create_methods["SNMPManagement"] =
            &FWObjectDatabase::createFWObjectSNMPManagement;
        create_methods["ServiceGroup"] =
            &FWObjectDatabase::createFWObjectServiceGroup;
        create_methods["TCPService"] =
            &FWObjectDatabase::createFWObjectTCPService;
        create_methods["TagService"] =
            &FWObjectDatabase::createFWObjectTagService;
        create_methods["UDPService"] =
            &FWObjectDatabase::createFWObjectUDPService;
        create_methods["UserService"] =
            &FWObjectDatabase::createFWObjectUserService;
        create_methods["physAddress"] =
            &FWObjectDatabase::createFWObjectphysAddress;
    }
}

FWObject *FWObjectDatabase::create(const string &type_name,
                                   int id,
                                   bool prepopulate)
{
    create_function_ptr fn = create_methods[type_name];
    if (fn == NULL)
    {
        const char *type_name_cptr = type_name.c_str();
        FWObject   *nobj;

        if (strcmp("comment", type_name_cptr)==SAME) return NULL;

        if (strcmp("AnyNetwork", type_name_cptr)==SAME)
        {
            nobj = new Network(this, prepopulate);
            if (id > -1) nobj->setId(id);
            nobj->setXMLName("AnyNetwork");
            addToIndex(nobj);
            return nobj;
        }

        if (strcmp("AnyIPService", type_name_cptr)==SAME)
        {
            nobj = new IPService(this,prepopulate);
            if (id > -1) nobj->setId(id);
            nobj->setXMLName("AnyIPService");
            addToIndex(nobj);
            return nobj;
        }

        if (strcmp("AnyInterval", type_name_cptr)==SAME)
        {
            nobj = new Interval(this,prepopulate);
            if (id > -1) nobj->setId(id);
            nobj->setXMLName("AnyInterval");
            addToIndex(nobj);
            return nobj;
        }

        cerr << "Do not have method to create object of type "
             << type_name << endl;
        return NULL;
    }

    return (this->*fn)(id, prepopulate);
}

FWObject *FWObjectDatabase::createFromXML(xmlNodePtr data)
{
    const char *n;
    string typen;
    int id = -1;

    n=FROMXMLCAST(data->name);
    if(!n)
        return NULL;
    typen = n;

    n=FROMXMLCAST(xmlGetProp(data,TOXMLCAST("id")));
    if(n)
    {
        id = registerStringId(n);
        FREEXMLBUFF(n);
    }

// create new object but do not prepopulate objects that
// automatically create children in constructor
    return create(typen, id, false);
}

CREATE_OBJ_METHOD(AddressRange);
CREATE_OBJ_METHOD(AddressTable);
CREATE_OBJ_METHOD(CustomService);
CREATE_OBJ_METHOD(DNSName);
CREATE_OBJ_METHOD(FWBDManagement);
CREATE_OBJ_METHOD(FWIntervalReference);
CREATE_OBJ_METHOD(FWObjectReference);
CREATE_OBJ_METHOD(FWServiceReference);
CREATE_OBJ_METHOD(Firewall);
CREATE_OBJ_METHOD(FirewallOptions);
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
CREATE_OBJ_METHOD(SNMPManagement);
CREATE_OBJ_METHOD(ServiceGroup);
CREATE_OBJ_METHOD(TCPService);
CREATE_OBJ_METHOD(TagService);
CREATE_OBJ_METHOD(UDPService);
CREATE_OBJ_METHOD(UserService);
CREATE_OBJ_METHOD(physAddress);


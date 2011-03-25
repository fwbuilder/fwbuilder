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

#include "../../config.h"

#include "PIXImporter.h"

#include <ios>
#include <iostream>
#include <algorithm>
#include <memory>

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Library.h"

#include <QString>
#include <QtDebug>

extern int fwbdebug;

// TODO: this should move to some common library, together with
// getVersionsForPlatform() it uses. Currently these functions are
// defined in libgui/platforms.cpp

extern QString findBestVersionMatch(const QString &platform,
                                    const QString &discovered_version);

using namespace std;
using namespace libfwbuilder;



PIXImporter::PIXImporter(FWObject *lib,
                         std::istringstream &input,
                         Logger *log,
                         const std::string &fwname) :
    IOSImporter(lib, input, log, fwname)
{
    setPlatform("pix");
    address_maker->setInvertedNetmasks(false); // since parent class sets to true
}



PIXImporter::~PIXImporter()
{
}

/*
 * do not clear named_objects_registry because this function is called
 * when we start processing each named object, object-group and some other
 * lines in the config
 */
void PIXImporter::clear()
{
    Importer::clear();

    current_named_object = NULL;
    named_object_name = "";
    named_object_comment = "";

    current_object_group = NULL;
    object_group_name = "";
    object_group_comment = "";
    object_group_service_protocol = "";
}

/*
 * this clears temporary variables inside Importer but does not touch
 * current_named_object and current_object_group
 */
void PIXImporter::clearTempVars()
{
    Importer::clear();
}

FWObject* PIXImporter::makeSrcObj()
{
    if (src_nm == "interface")
    {
        map<const string,Interface*>::iterator it;
        for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
        {
            Interface *intf = it->second;
            if (intf->getLabel() == src_a)
            {
                return intf;
            }
        }
        throw ImporterException(
            QString("Cannot find interface with label '%1'").arg(src_a.c_str()));
    }

    if (named_objects_registry.count(src_a.c_str()) > 0)
    {
        return named_objects_registry[src_a.c_str()];
    }
    return Importer::makeSrcObj();
}

FWObject* PIXImporter::makeDstObj()
{
    if (dst_nm == "interface")
    {
        map<const string,Interface*>::iterator it;
        for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
        {
            Interface *intf = it->second;
            if (intf->getLabel() == dst_a)
            {
                return intf;
            }
        }
        throw ImporterException(
            QString("Cannot find interface with label '%1'").arg(dst_a.c_str()));
    }

    if (named_objects_registry.count(dst_a.c_str()) > 0)
    {
        return named_objects_registry[dst_a.c_str()];
    }
    return Importer::makeDstObj();
}

FWObject* PIXImporter::makeSrvObj()
{
    if (protocol=="tcp" || protocol=="udp")
    {
        if (named_objects_registry.count(src_port_spec.c_str()) > 0)
            return named_objects_registry[src_port_spec.c_str()];
        if (named_objects_registry.count(dst_port_spec.c_str()) > 0)
            return named_objects_registry[dst_port_spec.c_str()];
    } else
    {
        if (named_objects_registry.count(protocol.c_str()) > 0)
            return named_objects_registry[protocol.c_str()];
    }
    return Importer::makeSrvObj();
}

void PIXImporter::setInterfaceAndDirectionForRuleSet(
    const string &ruleset_name, const string &interface_label, const string &dir)
{
    map<const string,Interface*>::iterator it;
    for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
    {
        Interface *intf = it->second;
        if (intf->getLabel() == interface_label)
        {
            Importer::setInterfaceAndDirectionForRuleSet(intf, ruleset_name, dir);
            return;
        }
    }
    QString err("Can not associate rule set %1 with any interface\n");
    *logger << err.arg(QString::fromUtf8(ruleset_name.c_str())).toStdString();
}

/*
 * Rearrange vlan interfaces. Importer creates all interfaces as
 * children of the firewall. Vlan interfaces should become
 * subinterfaces of the corresponding physical interfaces.
 */
void PIXImporter::rearrangeVlanInterfaces()
{
    std::auto_ptr<interfaceProperties> int_prop(
        interfacePropertiesObjectFactory::getInterfacePropertiesObject(
            getFirewallObject()));

    list<FWObject*> all_interface_objects =
        getFirewallObject()->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*> vlans;
    list<FWObject*>::iterator it;
    for (it=all_interface_objects.begin(); it!=all_interface_objects.end(); ++it)
    {
        Interface *intf = Interface::cast(*it);
        FWOptions *ifopt = intf->getOptionsObject();
        
        if (int_prop->looksLikeVlanInterface(intf->getName().c_str()) &&
            ifopt->getStr("type")=="8021q")
        {
            qDebug() << "Found vlan interface" << intf->getName().c_str();
            vlans.push_back(intf);
        }
    }

    for (it=vlans.begin(); it!=vlans.end(); ++it)
    {
        Interface *vlan_intf = Interface::cast(*it);

        qDebug() << "VLAN " << vlan_intf->getName().c_str();

        QString base_name;
        int vlan_id;
        int_prop->parseVlan(vlan_intf->getName().c_str(), &base_name, &vlan_id);

        qDebug() << "base name" << base_name;

        if ( ! base_name.isEmpty())
        {
            getFirewallObject()->remove(vlan_intf, false); // do not delete

            list<FWObject*>::iterator it2;
            for (it2=all_interface_objects.begin(); it2!=all_interface_objects.end(); ++it2)
            {
                if (base_name == (*it2)->getName().c_str())
                {
                    (*it2)->add(vlan_intf, false);
                    break;
                }
            }
        }
    }

}

Firewall* PIXImporter::finalize()
{
    // scan all UnidirectionalRuleSet objects, set interface and
    // direction in all rules of corresponding RuleSet and merge all
    // UnidirectionalRuleSet into one RuleSet object. Attach this
    // object to the firewall.

    if (fwbdebug) qDebug("PIXImporter::finalize()");

    if (haveFirewallObject())
    {
        Firewall *fw = Firewall::cast(getFirewallObject());
        fw->setStr("host_OS", "pix_os");
        Resources::setDefaultTargetOptions("pix_os" , fw);

        string version = findBestVersionMatch(
            "pix", discovered_version.c_str()).toStdString();
        if ( ! version.empty())
            fw->setStr("version", version);

        rearrangeVlanInterfaces();

        FWObject *policy = getFirewallObject()->getFirstByType(Policy::TYPENAME);
        assert( policy!=NULL );

        if (all_rulesets.size()!=0)
        {
            if (fwbdebug)
            {
                qDebug() << "Setting interface and direction for all rules";
                qDebug() << "all_rulesets.size()=" << all_rulesets.size();
            }

            std::map<const std::string,UnidirectionalRuleSet*>::iterator i;
            for (i=all_rulesets.begin(); i!=all_rulesets.end(); ++i)
            {
                UnidirectionalRuleSet *irs = (*i).second;

                if (fwbdebug)
                {
                    qDebug() << "  irs->name=" << irs->name.c_str();
                    qDebug() << "  irs->intf_dir.size()=" << irs->intf_dir.size();
                    qDebug() << "  irs->ruleset->size()=" << irs->ruleset->size();
                }

                // optimization: If we have several interfaces for
                // the ruleset, create a group
                // But first group interfaces by direction so
                // that later we can merge rules into the policy
                // with proper combination of interface group and
                // direction. Remember that the same access list
                // can be used with multiple interfaces with different
                // directions each time. The same list can be applied
                // to the same interface both in and out (although in
                // this case we have already switched direction to "both")
                //
                if (irs->intf_dir.size()>1)
                {
                    std::list<std::string> all_in;
                    std::list<std::string> all_out;
                    std::list<std::string> all_both;

                    std::map<std::string,std::string>::iterator i;
                    for (i = irs->intf_dir.begin();
                         i != irs->intf_dir.end(); ++i)
                    {
                        if ( (*i).second=="in")
                            all_in.push_back( (*i).first );
                        if ( (*i).second=="out")
                            all_out.push_back( (*i).first );
                        if ( (*i).second=="both")
                            all_both.push_back( (*i).first );
                    }

                    FWObject *og;
                    if (all_in.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_in);

                        MergeRules mr(irs->name, og, PolicyRule::Inbound, policy);
                        while (irs->ruleset->size() > 0)
                            mr.move(irs->ruleset->front());
                    }

                    if (all_out.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_out);

                        MergeRules mr(irs->name, og, PolicyRule::Outbound, policy);
                        while (irs->ruleset->size() > 0)
                            mr.move(irs->ruleset->front());
                    }

                    if (all_both.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_both);

                        MergeRules mr(irs->name, og, PolicyRule::Both, policy);
                        while (irs->ruleset->size() > 0)
                            mr.move(irs->ruleset->front());
                    }

                }
                else
                {
                    std::map<std::string,std::string>::iterator j;
                    for (j=irs->intf_dir.begin(); j!=irs->intf_dir.end(); ++j)
                    {
                        string intf_name = (*j).first;

                        Interface *intf = all_interfaces[intf_name];
                        std::string _dir = (*j).second;
                        PolicyRule::Direction direction = PolicyRule::Both;
                        if (_dir=="in")  direction = PolicyRule::Inbound;
                        if (_dir=="out") direction = PolicyRule::Outbound;

                        if (fwbdebug)
                            qDebug() << "Interface: " << intf
                                     << "dir: " << _dir.c_str();

                        // not all access lists are associated with interfaces
                        if (intf != NULL)
                        {
                            if (fwbdebug)
                                qDebug() << "    interface: " 
                                         << intf->getName().c_str();

                            MergeRules mr(irs->name, intf, direction, policy);
                            while (irs->ruleset->size() > 0)
                                mr.move(irs->ruleset->front());
                        }
                    }
                }

                if (fwbdebug) qDebug("ruleset done");

                // call clearChidren() not recursive because children objects
                // of all rules should not be deleted
                irs->ruleset->clearChildren(false);
                getFirewallObject()->remove(irs->ruleset, false);
                delete irs->ruleset;
            }
        }

        list<FWObject*> l2 = fw->getByType(Policy::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
            RuleSet *rs = RuleSet::cast(*i);
            rs->renumberRules();
        }

        return getFirewallObject();
    }
    else
    {
        return NULL;
    }
}

/*
 * Named objects
 *
 * At least in the case of Cisco configurations, I can only create an
 * object after I saw the line "host ... ", "subnet ..." or "range
 * ..." so I know its type. This means things like the name and
 * comment are known before the type. I use methods
 * commitNamed*Object() to create objects once all information is available.
 *
 * I other platforms information about named objects may not be
 * arranged in this way, for example in PF configs named objects are
 * represented by macros which do not have explicit type and have all
 * information on one line. Still, in that case the same commit*()
 * method will work if called by the grammar after all variables have
 * been parsed and values assigned to temporary member variables
 * inside the Importer object.
 */

void PIXImporter::newNamedObjectAddress(const string &name)
{
    named_object_name = QString::fromUtf8(name.c_str());
    named_object_comment = "";
    *logger << "Named object (address) " + name;
}

void PIXImporter::newNamedObjectService(const string &name)
{
    named_object_name = QString::fromUtf8(name.c_str());
    named_object_comment = "";
    *logger << "Named object (service) " + name;
}

void PIXImporter::commitNamedAddressObject()
{
    ObjectSignature sig;
    sig.object_name = named_object_name;
    sig.type_name = Address::TYPENAME;
    sig.address = tmp_a.c_str();
    sig.netmask = tmp_nm.c_str();
    current_named_object = commitObject(address_maker->createObject(sig));
    named_objects_registry[named_object_name] = current_named_object;
}

void PIXImporter::commitNamedAddressRangeObject()
{
    ObjectSignature sig;
    sig.object_name = named_object_name;
    sig.type_name = AddressRange::TYPENAME;
    sig.setAddressRangeStart(tmp_range_1.c_str());
    sig.setAddressRangeEnd(tmp_range_2.c_str());
    current_named_object = commitObject(address_maker->createObject(sig));
    named_objects_registry[named_object_name] = current_named_object;
}

void PIXImporter::commitNamedIPServiceObject()
{
    ObjectSignature sig;
    sig.object_name = named_object_name;
    sig.type_name = IPService::TYPENAME;
    sig.setProtocol(protocol.c_str());
    sig.fragments = fragments;
    current_named_object = commitObject(service_maker->createObject(sig));
    named_objects_registry[named_object_name] = current_named_object;
}

void PIXImporter::commitNamedICMPServiceObject()
{
    ObjectSignature sig;
    sig.object_name = named_object_name;
    sig.type_name = ICMPService::TYPENAME;

    if ( ! icmp_spec.empty())
    {
        sig.setIcmpFromName(icmp_spec.c_str());
    } else
    {
        sig.setIcmpType(icmp_type.c_str());
        sig.setIcmpCode(icmp_code.c_str());
    }

    current_named_object = commitObject(service_maker->createObject(sig));
    named_objects_registry[named_object_name] = current_named_object;
}

void PIXImporter::commitNamedTCPUDPServiceObject()
{
    FWObject *obj;
    // have to use createTCPService because it processes "neq" port
    // operators and may create a group with two services nstead of
    // just tcp service. Same for udp.
    if (protocol == "tcp") obj = createTCPService(named_object_name);
    if (protocol == "udp") obj = createUDPService(named_object_name);

    current_named_object = commitObject(obj);

    named_objects_registry[named_object_name] = current_named_object;
}

FWObject* PIXImporter::commitObject(FWObject *obj)
{
    return Importer::commitObject(obj);
}

FWObject* PIXImporter::setNameOfNamedObject(FWObject *obj)
{
    if (obj->isReadOnly()) return obj;

    if ( ! named_object_name.isEmpty())
        obj->setName(named_object_name.toUtf8().constData());

    if ( ! object_group_name.isEmpty())
        obj->setName(object_group_name.toUtf8().constData());
    
    return obj;
}

/*
 * it looks like "description" line is always the last in the named
 * object block output of "show run" command on ASA, however
 * "description" is optional and we create the object when we see
 * "subnet", "host" or "service" line. This function adds description
 * to existing named object if it exists or just sets the variable
 * named_object_comment. I dont want to lose the ability to parse the
 * description if it happens to appear first in the named object
 * block.
 */
void PIXImporter::setNamedObjectDescription(const std::string &txt)
{
    named_object_comment = QString::fromUtf8(txt.c_str());

    if (current_named_object != NULL && ! named_object_name.isEmpty())
    {
        current_named_object->setBool(".import-commited", false);
        current_named_object->setComment("");
        addStandardImportComment(current_named_object, named_object_comment);
    }
}

/************************************************************************/

void PIXImporter::newObjectGroupNetwork(const string &name)
{
    object_group_name = QString::fromUtf8(name.c_str());
    object_group_comment = "";

    ObjectMaker maker(Library::cast(library));
    current_object_group = 
        setNameOfNamedObject(
            commitObject(
                maker.createObject(ObjectGroup::TYPENAME, name)));
    named_objects_registry[object_group_name] = current_object_group;

    *logger << "Object Group (network) " + name;
}

void PIXImporter::newObjectGroupService(const string &name)
{
    object_group_name = QString::fromUtf8(name.c_str());
    object_group_comment = "";

    ObjectMaker maker(Library::cast(library));
    current_object_group = 
        setNameOfNamedObject(
            commitObject(
                maker.createObject(ServiceGroup::TYPENAME, name)));
    named_objects_registry[object_group_name] = current_object_group;

    object_group_service_protocol = "";

    *logger << "Object Group (service) " + name;
}

bool PIXImporter::isKnownServiceGroupName(const string &object_group_name)
{
    QString qs_object_group_name = QString::fromUtf8(object_group_name.c_str());
    qDebug() << "isKnownServiceGroupName:" << qs_object_group_name;

    if (named_objects_registry.count(qs_object_group_name) > 0)
    {
        FWObject *grp = named_objects_registry[qs_object_group_name];
        return ServiceGroup::isA(grp);
    }
    return false;
}

void PIXImporter::newObjectGroupProtocol(const string &name)
{
    object_group_name = QString::fromUtf8(name.c_str());
    object_group_comment = "";

    ObjectMaker maker(Library::cast(library));
    current_object_group = 
        setNameOfNamedObject(
            commitObject(
                maker.createObject(ServiceGroup::TYPENAME, name)));
    named_objects_registry[object_group_name] = current_object_group;

    *logger << "Object Group (protocol) " + name;
}

void PIXImporter::newObjectGroupICMP(const string &name)
{
    object_group_name = QString::fromUtf8(name.c_str());
    object_group_comment = "";

    ObjectMaker maker(Library::cast(library));
    current_object_group = 
        setNameOfNamedObject(
            commitObject(
                maker.createObject(ServiceGroup::TYPENAME, name)));
    named_objects_registry[object_group_name] = current_object_group;

    *logger << "Object Group (icmp) " + name;
}

void PIXImporter::setObjectGroupDescription(const std::string &descr)
{
    object_group_comment = QString::fromUtf8(descr.c_str());
    if (current_object_group != NULL && ! object_group_name.isEmpty())
    {
        current_object_group->setBool(".import-commited", false);
        current_object_group->setComment("");
        addStandardImportComment(current_object_group, object_group_comment);
    }
}

void PIXImporter::addNetworkToObjectGroup()
{
    ObjectSignature sig;
    sig.type_name = Address::TYPENAME;
    sig.setAddress(tmp_a.c_str());
    sig.setNetmask(tmp_nm.c_str());
    current_object_group->addRef(
        commitObject(address_maker->createObject(sig)));
}

void PIXImporter::addNamedObjectToObjectGroup(const std::string &object_name)
{
    QString no_name = QString::fromUtf8(object_name.c_str());
    if (named_objects_registry.count(no_name) > 0)
    {
        current_object_group->addRef(named_objects_registry[no_name]);
    } else
        throw ImporterException(
            QString("Attempt to add yet undefined named object '%1' "
                    "to object group '%2'").arg(no_name).arg(object_group_name));
}

void PIXImporter::addIPServiceToObjectGroup()
{
    ObjectSignature sig;
    sig.type_name = IPService::TYPENAME;
    sig.setProtocol(protocol.c_str());
    sig.fragments = fragments;
    FWObject *s = service_maker->createObject(sig);
    current_object_group->addRef(commitObject(s));
}

void PIXImporter::addTCPUDPServiceToObjectGroup()
{
    FWObject *new_obj = NULL;
    if (protocol.empty() && ! object_group_service_protocol.isEmpty())
        protocol = object_group_service_protocol.toStdString();

    if (protocol == "tcp") new_obj = createTCPService();
    if (protocol == "udp") new_obj = createUDPService();
    if (protocol == "tcp-udp") new_obj = createTCPUDPServicePair();

    if (new_obj)
        current_object_group->addRef(commitObject(new_obj));
}

void PIXImporter::addICMPServiceToObjectGroup()
{
    ObjectSignature sig;
    sig.type_name = ICMPService::TYPENAME;

    if ( ! icmp_spec.empty())
    {
        sig.setIcmpFromName(icmp_spec.c_str());
    } else
    {
        sig.setIcmpType(icmp_type.c_str());
        sig.setIcmpCode(icmp_code.c_str());
    }

    FWObject *s = service_maker->createObject(sig);
    current_object_group->addRef(commitObject(s));
}


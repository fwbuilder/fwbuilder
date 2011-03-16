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
}



PIXImporter::~PIXImporter()
{
}

FWObject* PIXImporter::createAddress(const string &a, const string &nm)
{
    return Importer::createAddress(a, nm);
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

    list<FWObject*> all_interfaces =
        getFirewallObject()->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*> vlans;
    list<FWObject*>::iterator it;
    for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
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
            for (it2=all_interfaces.begin(); it2!=all_interfaces.end(); ++it2)
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
                        std::for_each(irs->ruleset->begin(),
                                      irs->ruleset->end(),
                                      merge_rule(irs->name,
                                                 og,
                                                 PolicyRule::Inbound,
                                                 policy)
                        );
                    }

                    if (all_out.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_out);
                        std::for_each(irs->ruleset->begin(),
                                      irs->ruleset->end(),
                                      merge_rule(irs->name,
                                                 og,
                                                 PolicyRule::Outbound,
                                                 policy)
                        );
                    }

                    if (all_both.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_both);
                        std::for_each(irs->ruleset->begin(),
                                      irs->ruleset->end(),
                                      merge_rule(irs->name,
                                                 og,
                                                 PolicyRule::Both,
                                                 policy)
                        );
                    }

                }
                else
                {
                    std::map<std::string,std::string>::iterator j;
                    for (j=irs->intf_dir.begin(); j!=irs->intf_dir.end(); ++j)
                    {
                        Interface *intf = all_interfaces[ (*j).first ];
                        std::string _dir = (*j).second;
                        PolicyRule::Direction direction = PolicyRule::Both;
                        if (_dir=="in")  direction = PolicyRule::Inbound;
                        if (_dir=="out") direction = PolicyRule::Outbound;

                        // not all access lists are associated with interfaces
                        if (intf!=NULL)
                        {
                            if (fwbdebug)
                                qDebug() << "    interface=" 
                                         << intf->getName().c_str();

                            std::for_each(
                                irs->ruleset->begin(),
                                irs->ruleset->end(),
                                merge_rule(irs->name,
                                           intf,
                                           direction,
                                           policy)
                            );
                        }
                    }
                }
                qDebug("ruleset done");

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

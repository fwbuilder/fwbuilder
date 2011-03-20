/*

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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

#include "../../config.h"

// #include "global.h"
// #include "utils_no_qt.h"
// #include "platforms.h"


#include "IOSImporter.h"
#include "getProtoByName.h"
#include "getServByName.h"

#include <ios>
#include <iostream>
#include <algorithm>

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

using namespace std;
using namespace libfwbuilder;

IOSImporter::IOSImporter(FWObject *lib,
                         std::istringstream &input,
                         Logger *log,
                         const std::string &fwname) : Importer(lib, "iosacl", input, log, fwname)
{
    address_maker->setInvertedNetmasks(true);
}

IOSImporter::~IOSImporter()
{
    all_rulesets.clear();
    all_interfaces.clear();
}

void IOSImporter::setInterfaceAndDirectionForRuleSet(
    const std::string &ruleset_name,
    const std::string &_intf_name,
    const std::string &_dir)
{
    Importer::setInterfaceAndDirectionForRuleSet(
        ruleset_name, _intf_name, _dir);

}

ObjectSignature IOSImporter::packObjectSignatureTCPService()
{
    // use src_port_op, src_port_spec, dst_port_op, dst_port_spec
    // port_op can be: lt (less than), gt (greater than), eq (equal),
    // neq (not equal), and range (inclusive range).

    ObjectSignature sig;
    sig.type_name = TCPService::TYPENAME;

    sig.setSrcPortRangeFromPortOp(
        src_port_op.c_str(), src_port_spec.c_str(), protocol.c_str());
    sig.setDstPortRangeFromPortOp(
        dst_port_op.c_str(), dst_port_spec.c_str(), protocol.c_str());

    sig.established = established;
    sig.flags_mask = tcp_flags_mask;
    sig.flags_comp = tcp_flags_comp;

    return sig;
}

ObjectSignature IOSImporter::packObjectSignatureUDPService()
{
    // use src_port_op, src_port_spec, dst_port_op, dst_port_spec
    // port_op can be: lt (less than), gt (greater than), eq (equal),
    // neq (not equal), and range (inclusive range).

    ObjectSignature sig;
    sig.type_name = UDPService::TYPENAME;

    sig.setSrcPortRangeFromPortOp(
        src_port_op.c_str(), src_port_spec.c_str(), protocol.c_str());
    sig.setDstPortRangeFromPortOp(
        dst_port_op.c_str(), dst_port_spec.c_str(), protocol.c_str());

    return sig;
}

FWObject* IOSImporter::createTCPService()
{
    ObjectSignature sig = packObjectSignatureTCPService();
    return service_maker->createObject(sig);
}

FWObject* IOSImporter::createUDPService()
{
    ObjectSignature sig =  packObjectSignatureUDPService();
    return service_maker->createObject(sig);
}

void IOSImporter::ignoreCurrentInterface()
{
    if (current_interface)
    {
        QString err("Warning: interface %1 was not imported because it "
                    "is in \"shutdown\" mode\n");
        *Importer::logger <<
            err.arg(current_interface->getName().c_str()).toStdString();
        Importer::ignoreCurrentInterface(); // this clears current_interface
    }
}



void IOSImporter::merge_rule::operator()(FWObject* r)
{
    PolicyRule *rule = PolicyRule::cast(r);
    // Note that Policy object can have children that are objects of
    // classes PolicyRule and RuleSetOptions. If r does not cast to
    // PolicyRule, then it must be RuleSetOptions and we should just
    // skip it.
    if (rule==NULL) return;

    target_ruleset->reparent(rule);
//    target_ruleset->renumberRules();

    RuleElementItf* re =rule->getItf();
    re->addRef(intf);
    rule->setDirection(dir);

    std::string prev_comment = rule->getComment();
    rule->setComment(
        std::string("Imported from ") + ruleset_name + "\n" + prev_comment);
}

Firewall* IOSImporter::finalize()
{
    // scan all UnidirectionalRuleSet objects, set interface and
    // direction in all rules of corresponding RuleSet and merge all
    // UnidirectionalRuleSet into one RuleSet object. Attach this
    // object to the firewall.

    if (fwbdebug) qDebug("IOSImporter::finalize()");

    if (haveFirewallObject())
    {
        Firewall *fw = Firewall::cast(getFirewallObject());
        fw->setStr("host_OS", "ios");
        Resources::setDefaultTargetOptions("ios" , fw);
        fw->setStr("version", "");  // default version "any"
        fw->getManagementObject(); // creates management obj

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
                        merge_rule mr(irs->name,
                                      og,
                                      PolicyRule::Inbound,
                                      policy);
                        while (irs->ruleset->size() > 0)
                        {
                            Rule *rule = Rule::cast(irs->ruleset->front());
                            if (rule) mr(rule);
                            else irs->ruleset->pop_front();
                        }
                    }

                    if (all_out.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_out);
                        merge_rule mr(irs->name,
                                      og,
                                      PolicyRule::Outbound,
                                      policy);
                        while (irs->ruleset->size() > 0)
                        {
                            Rule *rule = Rule::cast(irs->ruleset->front());
                            if (rule) mr(rule);
                            else irs->ruleset->pop_front();
                        }
                    }

                    if (all_both.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_both);
                        merge_rule mr(irs->name,
                                      og,
                                      PolicyRule::Both,
                                      policy);
                        while (irs->ruleset->size() > 0)
                        {
                            Rule *rule = Rule::cast(irs->ruleset->front());
                            if (rule) mr(rule);
                            else irs->ruleset->pop_front();
                        }
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
                            merge_rule mr(irs->name,
                                          intf,
                                          direction,
                                          policy);
                            while (irs->ruleset->size() > 0)
                            {
                                Rule *rule = Rule::cast(irs->ruleset->front());
                                if (rule) mr(rule);
                                else irs->ruleset->pop_front();
                            }
                        }
                    }
                }
                // qDebug("ruleset done");

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

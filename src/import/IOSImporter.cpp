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
#include "fwbuilder/Library.h"

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

    ObjectSignature sig(error_tracker);
    sig.type_name = TCPService::TYPENAME;
    sig.port_range_inclusive = false;

    sig.setSrcPortRangeFromPortOpForCisco(
        src_port_op.c_str(), src_port_spec.c_str(), protocol.c_str());
    sig.setDstPortRangeFromPortOpForCisco(
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

    ObjectSignature sig(error_tracker);
    sig.type_name = UDPService::TYPENAME;
    sig.port_range_inclusive = false;

    sig.setSrcPortRangeFromPortOpForCisco(
        src_port_op.c_str(), src_port_spec.c_str(), protocol.c_str());
    sig.setDstPortRangeFromPortOpForCisco(
        dst_port_op.c_str(), dst_port_spec.c_str(), protocol.c_str());

    return sig;
}

FWObject* IOSImporter::createTCPService(const QString &name)
{
    if (src_port_op == "neq" || dst_port_op == "neq")
        return createTCPUDPNeqObject("tcp", name);

    ObjectSignature sig = packObjectSignatureTCPService();
    if( ! name.isEmpty()) sig.object_name = name;
    return commitObject(service_maker->createObject(sig));
}

FWObject* IOSImporter::createUDPService(const QString &name)
{
    if (src_port_op == "neq" || dst_port_op == "neq")
        return createTCPUDPNeqObject("udp", name);

    ObjectSignature sig =  packObjectSignatureUDPService();
    if ( ! name.isEmpty()) sig.object_name = name;
    return commitObject(service_maker->createObject(sig));
}

FWObject* IOSImporter::createTCPUDPServicePair(const QString &name)
{
    FWObject *srv1 = createTCPService((name.isEmpty()) ? "" : name + "-tcp");
    FWObject *srv2 = createUDPService((name.isEmpty()) ? "" : name + "-udp");

    QString group_name = name;
    if (name.isEmpty())
    {
        group_name = QString(srv1->getName().c_str()).replace("tcp ","tcp-udp ");
    }

    ObjectMaker maker(Library::cast(library), error_tracker);
    FWObject *grp = 
        commitObject(
            maker.createObject(ServiceGroup::TYPENAME, group_name.toStdString()));

    
    grp->addRef(srv1);
    grp->addRef(srv2);

    return grp;
}

/*
 * create two tcp service objects to cover port ranges before
 * and after src_port_spec, put them into service group and
 * return pointer to the group.  We ignore tcp ports and
 * "established" flag in combination with "neq"
 *
 */
FWObject* IOSImporter::createTCPUDPNeqObject(const QString &proto,
                                             const QString &name)
{
    ObjectSignature sig(error_tracker);
    sig.port_range_inclusive = false;

    if (proto == "tcp") sig.type_name = TCPService::TYPENAME;
    if (proto == "udp") sig.type_name = UDPService::TYPENAME;

    if ( ! name.isEmpty()) sig.object_name = name;

    QString group_name;
    FWObject *srv1 = nullptr;
    FWObject *srv2 = nullptr;

    if (src_port_op == "neq")
    {
        if ( ! dst_port_spec.empty())
            group_name = QString("%1 src neq %2 / dst %3")
                .arg(proto).arg(src_port_spec.c_str()).arg(dst_port_spec.c_str());
        else
            group_name = QString("%1 src neq %2").arg(proto).arg(src_port_spec.c_str());

        sig.setDstPortRangeFromPortOpForCisco(
            dst_port_op.c_str(), dst_port_spec.c_str(), proto);

        sig.setSrcPortRangeFromPortOpForCisco("lt", src_port_spec.c_str(), proto);
        srv1 = service_maker->createObject(sig);

        sig.setSrcPortRangeFromPortOpForCisco("gt", src_port_spec.c_str(), proto);
        srv2 = service_maker->createObject(sig);
    }

    if (dst_port_op == "neq")
    {
        if ( ! src_port_spec.empty())
            group_name = QString("%1 src %2 / dst neq %3")
                .arg(proto).arg(src_port_spec.c_str()).arg(dst_port_spec.c_str());
        else
            group_name = QString("%1 dst neq %2").arg(proto).arg(dst_port_spec.c_str());

        sig.setSrcPortRangeFromPortOpForCisco(
            src_port_op.c_str(), src_port_spec.c_str(), proto);

        sig.setDstPortRangeFromPortOpForCisco("lt", dst_port_spec.c_str(), proto);
        srv1 = service_maker->createObject(sig);

        sig.setDstPortRangeFromPortOpForCisco("gt", dst_port_spec.c_str(), proto);
        srv2 = service_maker->createObject(sig);
    }

    assert(srv1 != nullptr && srv2 != nullptr);

    ObjectMaker maker(Library::cast(library), error_tracker);
    FWObject *grp = 
        commitObject(
            maker.createObject(ServiceGroup::TYPENAME, group_name.toStdString()));

    grp->addRef(commitObject(srv1));
    grp->addRef(commitObject(srv2));

    return grp;
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



void IOSImporter::pushRule()
{
    assert(current_ruleset!=nullptr);
    assert(current_rule!=nullptr);
    // populate all elements of the rule

    addMessageToLog(
        QString("access list rule: access list %1, action %2")
        .arg(QString::fromUtf8(current_ruleset->ruleset->getName().c_str()))
        .arg(action.c_str()));
    
    Importer::pushRule();
}


void IOSImporter::MergeRules::move(FWObject* r)
{
    PolicyRule *rule = PolicyRule::cast(r);
    // Note that Policy object can have children that are objects of
    // classes PolicyRule and RuleSetOptions. If r does not cast to
    // PolicyRule, then it must be RuleSetOptions and we should just
    // skip it.
    if (rule==nullptr)
    {
        r->getParent()->remove(r);
        return;
    }

    target_ruleset->reparent(rule);

    if (intf)
    {
        RuleElementItf* re =rule->getItf();
        assert(re);
        re->addRef(intf);
    }

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
        assert( policy!=nullptr );

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
                        Interface *intf = all_interfaces[ (*j).first ];
                        std::string _dir = (*j).second;
                        PolicyRule::Direction direction = PolicyRule::Both;
                        if (_dir=="in")  direction = PolicyRule::Inbound;
                        if (_dir=="out") direction = PolicyRule::Outbound;

                        // not all access lists are associated with interfaces
                        if (intf!=nullptr)
                        {
                            if (fwbdebug)
                                qDebug() << "    interface=" 
                                         << intf->getName().c_str();

                            MergeRules mr(irs->name, intf, direction, policy);
                            while (irs->ruleset->size() > 0)
                                mr.move(irs->ruleset->front());
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
        return nullptr;
    }
}

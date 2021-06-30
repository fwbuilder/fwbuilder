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


#include "PIXImporter.h"

#include <ios>
#include <iostream>
#include <algorithm>
#include <memory>

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Library.h"

#include "../libgui/platforms.h"

#include <QString>
#include <QtDebug>

extern int fwbdebug;

using namespace libfwbuilder;
using namespace std;


QString GlobalPool::toString()
{
    QString l("number %1, interface %2, address range %3-%4, netmask %5 ");
    return l.arg(num).arg(pool_interface.c_str())
        .arg(start.c_str()).arg(end.c_str()).arg(netmask.c_str());
}

string GlobalPool::toStdString()
{
    return toString().toStdString();
}

GlobalPool& GlobalPool::operator=(const GlobalPool &other)
{
    num = other.num;
    pool_interface = other.pool_interface;
    start = other.start;
    end = other.end;
    netmask = other.netmask;
    return *this;
}


void PIXImporter::addGlobalPool()
{
    bool ok = false;
    int n;
    n = QString(tmp_global_pool.str_num.c_str()).toInt(&ok);
    if (ok)
    {
        tmp_global_pool.num = n;
        global_pools[tmp_global_pool.num].push_back(tmp_global_pool);
        addMessageToLog("Global address pool: " + tmp_global_pool.toString());
    }
}

void PIXImporter::pushNATRule()
{
    assert(current_ruleset!=nullptr);

    switch (rule_type)
    {
    case NATRule::DNAT:
        buildDNATRule();
        break;

    case NATRule::SNAT:
        buildSNATRule();
        break;

    default:
        assert(rule_type!=NATRule::DNAT && rule_type!=NATRule::SNAT);
    }
}

/*
 * DNAT rule.
 *
 * Using real_a, real_nm, mapped_a, mapped_nm, real_addr_acl,
 * real_port_spec, mapped_port_spec, prenat_interface,
 * postnat_interface
 */
void PIXImporter::buildDNATRule()
{
    addMessageToLog(QString("Destination translation rule (\"static\" command)"));

    newNATRule();

    NATRule *rule = NATRule::cast(current_rule);

    Interface *pre_intf = getInterfaceByLabel(prenat_interface);
    Interface *post_intf = getInterfaceByLabel(postnat_interface);

    rule->setAction(NATRule::Translate);

    if (real_nm.empty()) real_nm = InetAddr::getAllOnes().toString();
    if (mapped_nm.empty()) mapped_nm = InetAddr::getAllOnes().toString();

    if ( ! mapped_a.empty())
    {
        if (mapped_a == "interface")
        {
            RuleElementODst* odst = rule->getODst();
            assert(odst!=nullptr);
            odst->addRef(post_intf);
        } else
        {
            dst_a = mapped_a;
            dst_nm = mapped_nm;
            addODst();
        }
    }

    if ( ! real_a.empty())
    {
        dst_a = real_a;
        dst_nm = real_nm;

        RuleElement* tdst = rule->getTDst();
        assert(tdst!=nullptr);
        FWObject *s = makeDstObj();
        if (s) tdst->addRef( s );
    }

    if ( ! mapped_port_spec.empty())
    {
        src_port_spec = "";
        dst_port_op = "eq";
        dst_port_spec = mapped_port_spec;

        RuleElement* osrv = rule->getOSrv();
        assert(osrv!=nullptr);
        FWObject *s = Importer::makeSrvObj();

        if (s) osrv->addRef( s );
    }

    if ( ! real_port_spec.empty())
    {
        src_port_spec = "";
        dst_port_op = "eq";
        dst_port_spec = real_port_spec;

        RuleElement* tsrv = rule->getTSrv();
        assert(tsrv!=nullptr);
        FWObject *s = Importer::makeSrvObj();

        if (s) tsrv->addRef( s );
    }

    RuleElement *itf_i_re = rule->getItfInb();
    assert(itf_i_re!=nullptr);
    itf_i_re->addRef(post_intf);

    RuleElement *itf_o_re = rule->getItfOutb();
    assert(itf_o_re!=nullptr);
    itf_o_re->addRef(pre_intf);

    if ( ! real_addr_acl.empty())
    {
        UnidirectionalRuleSet *rs = all_rulesets[real_addr_acl];
        if (rs)
        {
            for(FWObject::iterator rs_it=rs->ruleset->begin();
                rs_it!=rs->ruleset->end(); ++rs_it)
            {
                PolicyRule *policy_rule = PolicyRule::cast(*rs_it);
            
                if (policy_rule)
                {
                    FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
                    NATRule *nat_rule = NATRule::cast(
                        dbroot->create(NATRule::TYPENAME));
                    nat_rule->duplicate(rule);

                    RuleElement* osrc = nat_rule->getOSrc();
                    RuleElement* osrv = nat_rule->getOSrv();
                    RuleElement* tdst = nat_rule->getTDst();
                    RuleElement* tsrv = nat_rule->getTSrv();

                    /* copy objects from a policy rule into
                     * rule elements of a nat rule
                     *
                     * Src --> TDst
                     * Dst --> OSrc
                     *
                     * If Srv matches destination ports, it should be mirrored and
                     * placed in OSrv
                     *
                     * If it matches source ports, it goes to TSrv, mirrored
                     *
                     */
                    RuleElement *re = policy_rule->getSrc();
                    FWObject::iterator it;
                    for (it=re->begin(); it!=re->end(); ++it)
                        tdst->addRef(FWReference::getObject(*it));

                    re = policy_rule->getDst();
                    for (it=re->begin(); it!=re->end(); ++it)
                        osrc->addRef(FWReference::getObject(*it));

                    re = policy_rule->getSrv();
                    for (it=re->begin(); it!=re->end(); ++it)
                    {
                        FWObject *old_obj = FWReference::getObject(*it);
                        TCPUDPService *tcpudp = TCPUDPService::cast(
                            mirrorServiceObjectRecursively(old_obj));
                        if (tcpudp == nullptr) tsrv->addRef(old_obj);
                        else
                        {
                            if (tcpudp->getSrcRangeEnd() > 0)
                                osrv->addRef(tcpudp);
                            if (tcpudp->getDstRangeEnd() > 0)
                                tsrv->addRef(tcpudp);
                        }
                    }

                    current_ruleset->ruleset->add(nat_rule);
                    addStandardImportComment(
                        nat_rule, QString::fromUtf8(rule_comment.c_str()));
                }
            }

            rs->to_be_deleted = true;
        }
    } else
    {
        // add it to the current ruleset
        current_ruleset->ruleset->add(rule);
        addStandardImportComment(rule, QString::fromUtf8(rule_comment.c_str()));
    }

}

/*
 * SNAT rule. Using rule_type, global_pools, prenat_interface,
 * nat_num, nat_a, nat_nm, nat_acl, max_conn, max_emb_conn
 *
 * Note that there can be multiple global pools with the same number
 * and same or different interfaces.  In that case we should create
 * multiple SNAT rules.
 */
void PIXImporter::buildSNATRule()
{
    addMessageToLog(QString("Source translation rule (\"nat\" command)"));

    bool ok = false;
    int pool_num = QString(nat_num.c_str()).toInt(&ok);
    // Parser matches INT_CONST so it can't be anything but integer...
    assert (ok);

    if (pool_num == 0) buildNoNATRule();
    else
    {
        foreach(GlobalPool pool, global_pools[pool_num])
        {
            if (fwbdebug)
            {
                qDebug() << "NAT command num=" << pool_num;
                qDebug() << "nat_a=" << nat_a.c_str()
                         << "nat_nm=" << nat_nm.c_str();
                qDebug() << "Using pool " << pool.toString();
            }

            Interface *post_intf = getInterfaceByLabel(pool.pool_interface);

            newNATRule();

            NATRule *rule = NATRule::cast(current_rule);

            Interface *pre_intf = getInterfaceByLabel(prenat_interface);

            rule->setAction(NATRule::Translate);

            if ( ! nat_a.empty())
            {
                // makeSrcObj() uses these variables
                src_a = nat_a;
                src_nm = nat_nm;

                RuleElement* osrc = rule->getOSrc();
                assert(osrc!=nullptr);
                FWObject *s = makeSrcObj();
                if (s) osrc->addRef( s );
            }

            ObjectSignature sig(error_tracker);
            FWObject *addr = nullptr;

            if (pool.start == "interface")
            {
                addr = post_intf;
            } else
            {
                if (pool.start == pool.end)
                {
                    sig.type_name = Address::TYPENAME;
                    sig.address = pool.start.c_str();
                    sig.netmask = pool.netmask.c_str();
                } else
                {
                    sig.type_name = AddressRange::TYPENAME;
                    sig.setAddressRangeStart(pool.start.c_str());
                    sig.setAddressRangeEnd(pool.end.c_str());
                }
                addr = commitObject(address_maker->createObject(sig));
            }

            RuleElement* tsrc = rule->getTSrc();
            assert(tsrc!=nullptr);
            if (addr) tsrc->addRef( addr );

            if (pre_intf)
            {
                RuleElement *itf_i_re = rule->getItfInb();
                assert(itf_i_re!=nullptr);
                itf_i_re->addRef(pre_intf);
            }

            if (post_intf)
            {
                RuleElement *itf_o_re = rule->getItfOutb();
                assert(itf_o_re!=nullptr);
                itf_o_re->addRef(post_intf);
            }

            if ( ! nat_acl.empty())
            {
                natRuleWithACL(rule);
            } else
            {
                // add it to the current ruleset
                current_ruleset->ruleset->add(rule);
                addStandardImportComment(rule,
                                         QString::fromUtf8(rule_comment.c_str()));
            }
        }
    }
}


void PIXImporter::buildNoNATRule()
{
    addMessageToLog(QString("NAT exemption rule (\"nat (interface) 0\" command)"));

    if (fwbdebug)
    {
        qDebug() << "NAT command num=0";
        qDebug() << "nat_a=" << nat_a.c_str() << "nat_nm=" << nat_nm.c_str();
    }

    newNATRule();

    NATRule *rule = NATRule::cast(current_rule);
    rule->setRuleType(libfwbuilder::NATRule::NONAT);

    Interface *pre_intf = getInterfaceByLabel(prenat_interface);

    rule->setAction(NATRule::Translate);

    if ( ! nat_a.empty())
    {
        // makeSrcObj() uses these variables
        src_a = nat_a;
        src_nm = nat_nm;

        RuleElement* osrc = rule->getOSrc();
        assert(osrc!=nullptr);
        FWObject *s = makeSrcObj();
        if (s) osrc->addRef( s );
    }

    RuleElement *itf_i_re = rule->getItfInb();
    assert(itf_i_re!=nullptr);
    itf_i_re->addRef(pre_intf);

    if ( ! nat_acl.empty())
    {
        natRuleWithACL(rule);
    } else
    {
        // add it to the current ruleset
        current_ruleset->ruleset->add(rule);
        addStandardImportComment(rule,
                                 QString::fromUtf8(rule_comment.c_str()));
    }
}

void PIXImporter::natRuleWithACL(NATRule *rule)
{

    UnidirectionalRuleSet *rs = all_rulesets[nat_acl];
    if (rs)
    {
        for(FWObject::iterator rs_it=rs->ruleset->begin();
            rs_it!=rs->ruleset->end(); ++rs_it)
        {
            PolicyRule *policy_rule = PolicyRule::cast(*rs_it);
            
            if (policy_rule)
            {
                FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
                NATRule *nat_rule = NATRule::cast(
                    dbroot->create(NATRule::TYPENAME));
                nat_rule->duplicate(rule);

                RuleElement* osrc = nat_rule->getOSrc();
                RuleElement* odst = nat_rule->getODst();
                RuleElement* osrv = nat_rule->getOSrv();

                /* copy objects from a policy rule into "original"
                 * rule elements of a nat rule
                 *
                 * Src --> OSrc
                 * Dst --> ODst
                 * Srv --> OSrv
                 */
                RuleElement *re = policy_rule->getSrc();
                FWObject::iterator it;
                for (it=re->begin(); it!=re->end(); ++it)
                    osrc->addRef(FWReference::getObject(*it));

                re = policy_rule->getDst();
                for (it=re->begin(); it!=re->end(); ++it)
                    odst->addRef(FWReference::getObject(*it));

                re = policy_rule->getSrv();
                for (it=re->begin(); it!=re->end(); ++it)
                    osrv->addRef(FWReference::getObject(*it));

                current_ruleset->ruleset->add(nat_rule);
                addStandardImportComment(
                    nat_rule, QString::fromUtf8(rule_comment.c_str()));
            }
        }

        rs->to_be_deleted = true;
    }
}


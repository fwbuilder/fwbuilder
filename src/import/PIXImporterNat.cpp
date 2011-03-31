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

/*

  Variables used to build nat rules

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
*/

void PIXImporter::pushNATRule()
{
    assert(current_ruleset!=NULL);
    assert(current_rule!=NULL);

    NATRule *rule = NATRule::cast(current_rule);

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

    // then add it to the current ruleset
    current_ruleset->ruleset->add(current_rule);

    addStandardImportComment(current_rule, QString::fromUtf8(rule_comment.c_str()));

    current_rule = NULL;
    rule_comment = "";

    clear();
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
            assert(odst!=NULL);
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
        assert(tdst!=NULL);
        FWObject *s = makeDstObj();
        if (s) tdst->addRef( s );
    }

    if ( ! real_addr_acl.empty())
    {
        UnidirectionalRuleSet *rs = all_rulesets[real_addr_acl];
        if (rs)
        {
            RuleElement* tdst = rule->getTDst();
            assert(tdst!=NULL);

            PolicyRule *policy_rule = PolicyRule::cast(
                rs->ruleset->getFirstByType(PolicyRule::TYPENAME));
            
            if (policy_rule)
            {
                RuleElement *src = policy_rule->getSrc();
                for (FWObject::iterator it=src->begin(); it!=src->end(); ++it)
                {
                    FWObject *o = FWReference::getObject(*it);
                    tdst->addRef(o);
                }
            }
        }
    }

    if ( ! mapped_port_spec.empty())
    {
        src_port_spec = "";
        dst_port_op = "eq";
        dst_port_spec = mapped_port_spec;

        RuleElement* osrv = rule->getOSrv();
        assert(osrv!=NULL);
        FWObject *s = Importer::makeSrvObj();

        if (s) osrv->addRef( s );
    }

    if ( ! real_port_spec.empty())
    {
        src_port_spec = "";
        dst_port_op = "eq";
        dst_port_spec = real_port_spec;

        RuleElement* tsrv = rule->getTSrv();
        assert(tsrv!=NULL);
        FWObject *s = Importer::makeSrvObj();

        if (s) tsrv->addRef( s );
    }

    RuleElement *itf_i_re = rule->getItfInb();
    assert(itf_i_re!=NULL);
    itf_i_re->addRef(post_intf);

    RuleElement *itf_o_re = rule->getItfOutb();
    assert(itf_o_re!=NULL);
    itf_o_re->addRef(pre_intf);
}

void PIXImporter::buildSNATRule()
{

}


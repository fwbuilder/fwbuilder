/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: PolicyCompiler_pix_v6_acls.cpp,v 1.1 2008/03/06 06:49:00 vkurland Exp $

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


  Rule processors in this module build ACLs for PIX v6

  they employ number of assumptions that are only valid for PIX <7.0
*/

#include "config.h"

#include "PolicyCompiler_pix.h"
#include "NATCompiler_pix.h"
#include "PIXObjectGroup.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/AddressTable.h"

#include <iostream>
#if __GNUC__ > 3 || \
    (__GNUC__ == 3 && (__GNUC_MINOR__ > 2 || (__GNUC_MINOR__ == 2 ) ) ) || \
     _MSC_VER
#  include <streambuf>
#else
#  include <streambuf.h>
#endif
#include <iomanip>
#include <algorithm>
#include <functional>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

bool PolicyCompiler_pix::InterfaceAndDirection_v6::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    bool icmp_cmd = rule->getBool("icmp_cmd");
    bool ssh_telnet_cmd = rule->getBool("ssh_telnet_cmd");

    int interface_id = rule->getInterfaceId();

    if (rule->getDirection()==PolicyRule::Undefined) 
        rule->setDirection( PolicyRule::Both );

    if (interface_id==-1 && rule->getDirection()==PolicyRule::Both)
        return true;

    if (interface_id==-1 && !icmp_cmd && !ssh_telnet_cmd && (
            rule->getDirection()==PolicyRule::Inbound ||
            rule->getDirection()==PolicyRule::Outbound)
    ) compiler->abort(string("Direction set without interface in rule ")+rule->getLabel());

    return true;
}

/*
 *  rules with direction 'both' associated with an interface are split
 *  and copies are assigned directions Inbound and Outbound
 *
 *  rules with direction 'both' not associated with any interface are
 *  simply converted to "Inbound". This is because we only generate
 *  outbound ACLs for rules that explicitly were defined by the user
 *  with direction "Outbound"; everything else is implemented using
 *  inbound ACLs
 *
 *  04/21/06 --vk
 */
bool PolicyCompiler_pix::SplitDirection_v6::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    Interface  *rule_iface = compiler->getCachedFwInterface(rule->getInterfaceId());

    if (rule->getDirection()==PolicyRule::Both)
    {
        if (rule_iface!=NULL)
        {
            PolicyRule *r= PolicyRule::cast(
                compiler->dbcopy->create(PolicyRule::TYPENAME) );
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setDirection(PolicyRule::Inbound);
            tmp_queue.push_back(r);

            r= PolicyRule::cast(
                compiler->dbcopy->create(PolicyRule::TYPENAME) );
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setDirection(PolicyRule::Outbound);
            tmp_queue.push_back(r);
        } else
        {
            rule->setDirection(PolicyRule::Inbound);
            tmp_queue.push_back(rule);
        }
    } else 
        tmp_queue.push_back(rule);

    return true;
}

/**
 * this processor emulates outbound ACL by splitting a rule onto
 * several rules and assigning them to interfaces. This processor
 * works only with rules that have direction "Outbound" and have
 * interface specified.
 *
 * Prerequisities:
 * 
 * Rule should have been split before this processor is called if
 * objects in src and/or dst belong to different network zones
 *
 *  
 *                  Internet ("any")
 *                      ^
 *                      |     
 *                      | i4
 *                 +----------+
 *               i1|          |i3
 *  ---------------+   PIX    +----------------
 *   host1         |          |         host3
 *                 +----------+
 *                      |i2 
 *                      |     
 *                      |host2
 *
 *   src      dst         interface
 *
 *   h1       h2            i2           change interface to i1
 *   any      h2            i2           split, use all interfaces but i2
 *   h1       any           i2           change interface to i1
 *   any      any           i2           split, use all interfaces but i2
 *
 *
 *   FWSM v2.3 and beyond, as well as PIX 7.0, support outbound ACLs
 *   (via "access-group out ..." command) We do not need to do this
 *   for these platforms.
 */
bool PolicyCompiler_pix::EmulateOutboundACL_v6::processNext()
{
    Helper helper(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    Interface  *rule_iface = compiler->getCachedFwInterface(rule->getInterfaceId());

    if (rule->getDirection()==PolicyRule::Outbound && rule_iface!=NULL)
    {
        if ( compiler->fw->getOptionsObject()->getBool("pix_emulate_out_acl") )
        {
            RuleElementSrc *src=rule->getSrc();    assert(src);
            RuleElementDst *dst=rule->getDst();    assert(dst);

            try
            {
                if (!src->isAny())
                {
                    int iface1_id = helper.findInterfaceByNetzone(
                        compiler->getFirstSrc(rule) );

/* special case: interface detected via comparison of src and the
 * network zone is the same as the one this rule is assigned to, but
 * direction is Outbound - drop this rule 
 */
                    if (iface1_id==rule->getInterfaceId())
                    {
                        return true;
                    }

                    rule->setInterfaceId(iface1_id);
                    rule->setDirection(PolicyRule::Inbound);
                    tmp_queue.push_back(rule);
                } else
                {
                    int iface2_id;
                    iface2_id = helper.findInterfaceByNetzone(
                        compiler->getFirstDst(rule) );

                    list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
                    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
                    {
                        if ( (*i)->getId()==iface2_id ) continue;

                        PolicyRule *r= PolicyRule::cast(
                            compiler->dbcopy->create(PolicyRule::TYPENAME) );
                        compiler->temp_ruleset->add(r);

                        r->duplicate(rule);
                        r->setInterfaceId((*i)->getId());
                        rule->setDirection(PolicyRule::Inbound);

                        tmp_queue.push_back(r);
                    }
                }
            } catch (string addr)
            {
                ostringstream str;
                str << "Can not find interface to assign rule "
                    << rule->getLabel()
                    << ": " << endl
                    << "Address " << addr
                    << " does not match address or network zone of any interface" 
                    << endl;
                compiler->abort(str.str());
            }
        } else
            compiler->abort("Outbound ACLs are not supported and emulation is not activated: Rule "+rule->getLabel());
    } else 
        tmp_queue.push_back(rule);

    return true;
}

/**
 *  this processor assigns rules to interfaces (since PIX only
 *  supports ACLs on interfaces and direction can only be "inbound").
 *
 *  
 *                  Internet ("any")
 *                      ^
 *                      |     
 *                      | i4
 *                 +----------+
 *               i1|          |i3
 *  ---------------+   PIX    +----------------
 *   host1         |          |         host3
 *                 +----------+
 *                      |i2 
 *                      |     
 *                      |host2
 *
 *   src      dst                assign to interface
 *
 *   any      i1                 i1
 *   any      i2                 i2
 *   any      i3                 i3
 *   any      i4                 i4
 *   any      host2              all
 *   host1    host2              i1
 *   host1    any                i1
 *   any      any                all
 */
bool PolicyCompiler_pix::assignRuleToInterface_v6::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    Helper helper(compiler);

    RuleElementSrc *src=rule->getSrc();    assert(src);
    RuleElementDst *dst=rule->getDst();    assert(dst);

    if (rule->getInterfaceId()==-1) 
    {
        try
        {
            if (! src->isAny() )
            {
                Address *a=compiler->getFirstSrc(rule);
                int iface1_id = helper.findInterfaceByNetzone(a);
                rule->setInterfaceId(iface1_id);
                tmp_queue.push_back(rule);
            } else {

                Address *a=compiler->getFirstDst(rule);
                if ( ! dst->isAny() && compiler->complexMatch(a,compiler->fw))
                {
                    int iface2_id = helper.findInterfaceByNetzone( a );
                    rule->setInterfaceId(iface2_id);
                    rule->setStr("direction","Inbound");
                    tmp_queue.push_back(rule);
                    return true;
                }

                list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
                for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
                {
                    PolicyRule *r= PolicyRule::cast(
                        compiler->dbcopy->create(PolicyRule::TYPENAME) );
                    compiler->temp_ruleset->add(r);

                    r->duplicate(rule);
                    r->setInterfaceId((*i)->getId());
                    r->setStr("direction","Inbound");

                    tmp_queue.push_back(r);
                }
            }	
        } catch (string addr)
        {
            ostringstream str;
            str << "Can not find interface to assign rule "
                << rule->getLabel()
                << ": " << endl
                << "Address " << addr
                << " does not match address or network zone of any interface" 
                << endl;
            compiler->abort(str.str());
        }

    } else {
        tmp_queue.push_back(rule);
    }
    return true;
}

/*
 * This processor is called after emulateOutboundACL
 */
bool PolicyCompiler_pix::pickACL_v6::processNext()
{
    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    Interface  *rule_iface = compiler->getCachedFwInterface(rule->getInterfaceId());
    if(rule_iface==NULL)
    {
        compiler->abort("Missing interface assignment for rule "+rule->getLabel());
    }

    string acl_name= rule_iface->getLabel() + "_acl_in";
    rule->setStr("acl",acl_name);

    ciscoACL *acl = new ciscoACL(acl_name,rule_iface, "in");
    pix_comp->acls[acl_name] = acl;

    acl->setWorkName(acl_name);

    tmp_queue.push_back(rule);
    return true;
}



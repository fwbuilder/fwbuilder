/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#include "PolicyCompiler_ipt.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Firewall.h"

#include "combinedAddress.h"

#include <limits.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

/*
 * Optimizer 1:
 *
 * splits rule, making sure we make only one parameter check at a time
 * That is, we only check source, or destination or service and then
 * pass control to a user-defined chain to check for the next
 * parameter. This helps avoid multiple checks for the same parameter.
 *
 * Assumtions:
 *
 * Can use this process with multiple objects in src,dst,srv
 * Run splitRuleIfSrvAnyActionReject before this processor to make sure
 * Srv contains only TCP objects if action is "Reject" and TCP RST is required
 */
void PolicyCompiler_ipt::optimize1::optimizeForRuleElement(
    PolicyRule  *rule, const std::string  &re_type)
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule     *r;

    string this_chain = rule->getStr("ipt_chain");
    string new_chain  = ipt_comp->getNewTmpChainName(rule);

    r= compiler->dbcopy->createPolicyRule();
    compiler->temp_ruleset->add(r);
    r->duplicate(rule);

    for (FWObject::iterator i=r->begin(); i!=r->end(); ++i)
    {
        if (RuleElement::cast(*i)!=nullptr)
        {
            if ((*i)->getTypeName()!=re_type && (*i)->size()!=1)
            {
               RuleElement *nre=RuleElement::cast(*i);
               nre->clearChildren();  
               nre->setAnyElement();
            } else
            {
               RuleElement *re = RuleElement::cast(
                   rule->getFirstByType((*i)->getTypeName()));
/* 
 * put "any tcp" service back in srv field if it was originally some
 * tcp service. This is needed because we may need to produce
 * --reject-with tcp-reset if the action is reject and we need to
 * reject with TCP RST.
 */

               if (RuleElementSrv::isA(re) &&
                   r->getAction()==PolicyRule::Reject &&
                   ipt_comp->isActionOnRejectTCPRST(r))
               {
                   Service  *srv= compiler->getFirstSrv(r);
                   if (TCPService::isA(srv))
                   {
                       re->clearChildren();
                       re->addRef(
                           compiler->dbcopy->findInIndex(
                               FWObjectDatabase::getIntId(ANY_TCP_OBJ_ID)));
/* also leave a flag indicating that further optimization by service
 * is not needed */
                       rule->setBool("do_not_optimize_by_srv",true);
                       r->setBool("do_not_optimize_by_srv",true);
                   }
                   else
                   {
                       re->reset();
                   }
               } else
               {
                   re->reset();  
               }
            }      
        }
    }
    r->setStr("ipt_target",new_chain);

    r->setClassification(false);
    r->setRouting(false);
    r->setTagging(false);

    tmp_queue.push_back(r);

    FWOptions *ruleopt=rule->getOptionsObject();
    ruleopt->setBool("stateless",true);
    ruleopt->setInt("limit_value",-1);
    ruleopt->setInt("connlimit_value",-1);
    ruleopt->setInt("hashlimit_value",-1);
    rule->setStr("ipt_chain",new_chain);
    rule->setBool("force_state_check",false);
    rule->setStr("upstream_rule_chain", this_chain);
    ipt_comp->registerChain(new_chain);
    ipt_comp->insertUpstreamChain(this_chain, new_chain);

//    if (rule->getInterfaceStr()=="")
//        rule->setInterfaceStr("nil");

    rule->setDirection( PolicyRule::Both ); 
   
    tmp_queue.push_back(rule);
}

bool PolicyCompiler_ipt::optimize1::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    RuleElementSrc      *srcrel;
    RuleElementDst      *dstrel;
    RuleElementSrv      *srvrel;
    RuleElementInterval *intrel;

    FWObject::iterator i1 = rule->begin();
    srcrel = RuleElementSrc::cast(*i1);
    i1++;
    dstrel = RuleElementDst::cast(*i1);
    i1++;
    srvrel = RuleElementSrv::cast(*i1);
    i1++;
    i1++;
    intrel = RuleElementInterval::cast(*i1);

    bool srcany = srcrel->isAny();
    bool dstany = dstrel->isAny();
    bool srvany = srvrel->isAny();
    bool intany = (intrel!=nullptr && intrel->isAny());

    int  srcn = srcrel->size();
    int  dstn = dstrel->size();
    int  srvn = srvrel->size();
    int  intn = 1;
    if (intrel!=nullptr) intn = intrel->size();

    bool all_tcp_or_udp = true;
    for (FWObject::iterator i=srvrel->begin(); i!=srvrel->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	    
        Service *service_object = Service::cast( o );
        if (service_object==nullptr)
        {
            compiler->abort(rule,
                                                      "Broken Service rule element");
        }

// tcp and udp will be collapsed because we can use multiport module
        if ( !TCPService::isA(service_object) && !UDPService::isA(service_object))
        {
            all_tcp_or_udp = false;
            break;
        }
    }
    
    if (all_tcp_or_udp) srvn = 1;

// Golden rule - try to introduce minimum forward rules ....
// we can't optimize 1 src, 1 dstn, 1 service and 1 time interval
// we can't optimize if we've got three 'anys' ..
    if ((srcn <= 1 && dstn <= 1 && srvn <= 1 && intn <= 1) ||
        (srcany && dstany && srvany) ||
        (srcany && dstany && intany) ||
        (srcany && srvany && intany) ||
        (dstany && srvany && intany) )
    {
        tmp_queue.push_back(rule);
        return true;
    }

// Assume any means LOTS of rules - i.e. not good candidate for optimization
    if (srcany) srcn=INT_MAX;
    if (dstany) dstn=INT_MAX;
    if (srvany) srvn=INT_MAX;
    if (intany) intn=INT_MAX;


// Now work out which is best optimization to do.
// this rule is called twice so we only need to do one op on each

    if ( !srvany && (srvn <= dstn) && (srvn <= srcn) && (srvn <= intn) &&
         ! rule->getBool("do_not_optimize_by_srv") )
    {
        optimizeForRuleElement(rule,RuleElementSrv::TYPENAME);
        return true;
    }

    if ( !srcany && (srcn <= dstn) && (srcn <= srvn) && (srcn <= intn))
    {
        optimizeForRuleElement(rule,RuleElementSrc::TYPENAME);
        return true;
    }

    if ( !dstany && (dstn <= srcn) && (dstn <= srvn) && (dstn <= intn))
    {
        optimizeForRuleElement(rule,RuleElementDst::TYPENAME);
        return true;
    }

    if ( !intany && (intn <= srcn) && (intn <= dstn) && (intn <= srvn))
    {
        optimizeForRuleElement(rule,RuleElementInterval::TYPENAME);
        return true;
    }


    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::optimize2::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *srvrel=rule->getSrv();

    if (rule->getBool("final"))
    {
        if ( rule->getAction()==PolicyRule::Reject && ipt_comp->isActionOnRejectTCPRST(rule))
        {
// preserve service
            ;
        } else 
        {
            srvrel->clearChildren();
            srvrel->setAnyElement();
        }
    }

    tmp_queue.push_back(rule);

    return true;
}

/*
 *  this processor eliminates duplicate rules _generated for the same
 *  high level rule_ This is different from processor
 *  PolicyCompiler_ipf::eliminateDuplicateRules, which finds and
 *  eliminates duplicate rules throughout the whole generated script.
 */
bool PolicyCompiler_ipt::optimize3::processNext()
{
    PolicyRule *rule;
    rule = getNext(); if (rule==nullptr) return false;

    if (rule->isFallback() || rule->isHidden())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    if (printRule==nullptr)
    {
        printRule = new PrintRule("");
        printRule->setContext(compiler);
        printRule->initialize();
    }
    string thisRule = rule->getLabel() + " " + printRule->PolicyRuleToString(rule);

    if (rules_seen_so_far.count(thisRule)!=0) return true;

    tmp_queue.push_back(rule);
    rules_seen_so_far[thisRule]=true;

    return true;
}

bool PolicyCompiler_ipt::optimizeForMinusIOPlus::processNext()
{
    PolicyRule *rule;
    rule=getNext(); if (rule==nullptr) return false;

    RuleElementItf *itf_re = rule->getItf(); assert(itf_re!=nullptr);
    FWObject *rule_iface = FWObjectReference::getObject(itf_re->front());
    string iface_name = rule_iface->getName();  // rule->getInterfaceStr();

    if (iface_name.empty() || iface_name=="nil" )
    {
        tmp_queue.push_back(rule);
        return true;
    }

    string chain = rule->getStr("ipt_chain");

    if (iface_name == "*" && (chain == "INPUT" || chain == "OUTPUT"))
        itf_re->reset();
//        rule->setInterfaceStr("");

    tmp_queue.push_back(rule);
    return true;
}


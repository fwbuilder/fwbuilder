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

#include "PolicyCompiler_ipf.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Policy.h"

#include <limits.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


static map<string,bool> skip_targets;


void PolicyCompiler_ipf::optimize1::optimizeForRuleElement(PolicyRule *rule, 
                                                  const std::string  &re_type)
{
    RuleElement    *re=RuleElement::cast(rule->getFirstByType(re_type));

    PolicyRule     *r;

    r= compiler->dbcopy->createPolicyRule();
    compiler->temp_ruleset->add(r);
    r->duplicate(rule);

/* duplicate copies everything, including attribute
 * "skip_label". That's why I set skip_label after I create a copy of the rule
 */

    string skip_target = FWObjectDatabase::getStringId(rule->getId());
    while (skip_targets[skip_target]) skip_target+=".A";
    skip_targets[skip_target]=true;

    // just need a unique label, and ID is unique
    rule->setStr("skip_label", skip_target);

    for (FWObject::iterator i=r->begin(); i!=r->end(); ++i)
    {
        if (RuleElement::cast(*i)!=nullptr && (*i)->getTypeName()!=re_type)
        {
            RuleElement *nre=RuleElement::cast(*i);
            nre->clearChildren();  
            nre->setAnyElement();
        }
    }
    r->setAction(PolicyRule::Skip);
    r->setBool("quick",false);
    r->setStr("skip_to",skip_target);
    tmp_queue.push_back(r);

    r= compiler->dbcopy->createPolicyRule();
    compiler->temp_ruleset->add(r);
    r->duplicate(rule);

    RuleElement *re1;
    re1=r->getSrc();    re1->clearChildren();    re1->setAnyElement();
    re1=r->getDst();    re1->clearChildren();    re1->setAnyElement();
    re1=r->getSrv();    re1->clearChildren();    re1->setAnyElement();
    r->setAction(PolicyRule::Continue);
    r->setStr("skip_label","");
    tmp_queue.push_back(r);

    re->clearChildren();  
    re->setAnyElement();

/* rules that we have inserted above 'rule' will skip over it. We should
 * not drop them when we eliminate duplicates */
    rule->setBool("skip_check_for_duplicates",true);
    tmp_queue.push_back(rule);
}

bool PolicyCompiler_ipf::optimize1::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementDst *dstrel=rule->getDst();
    RuleElementSrv *srvrel=rule->getSrv();

    int srcn=srcrel->size();
    int dstn=dstrel->size();
    int srvn=srvrel->size();

/* if all rule elements have exactly one object, there is nothing to optimize */
    if ( (srcn==1 && dstn==1) ||
         (dstn==1 && srvn==1) ||
         (srvn==1 && srcn==1) )
    {
        tmp_queue.push_back(rule);
        return true;
    }

    if (srcn==1) srcn=INT_MAX;
    if (dstn==1) dstn=INT_MAX;
    if (srvn==1) srvn=INT_MAX;

    string re=RuleElementSrc::TYPENAME;

    if (srcn>2 && srcn<=dstn && dstn<=srvn)
    {
        optimizeForRuleElement(rule,RuleElementSrc::TYPENAME);
        return true;
    }

    if (dstn>2 && dstn<=srvn && srvn<=srcn)
    {
        optimizeForRuleElement(rule,RuleElementDst::TYPENAME);
        return true;
    }

    if (srvn>2 && srvn<=srcn && srcn<=dstn)
    {
        optimizeForRuleElement(rule,RuleElementSrv::TYPENAME);
        return true;
    }

    tmp_queue.push_back(rule);

    return true;
}



bool PolicyCompiler_ipf::optimizeSrc::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementDst *dstrel=rule->getDst();
    RuleElementSrv *srvrel=rule->getSrv();

    int srcn=srcrel->size();
    int dstn=dstrel->size();
    int srvn=srvrel->size();

/* without optimization we generate N^3 rules (n1*n2*n3), with it we
 * generate 3*N (n1+n2+n3) rules. If n1+n2+n3 is greater than
 * n1*n2*n3, then we should not optimize
 */
    if (srcrel->isAny() || (srcn+dstn+srvn>=srcn*dstn*srvn))
    {
        tmp_queue.push_back(rule);
        return true;
    }

    optimizeForRuleElement(rule,RuleElementSrc::TYPENAME);

    return true;
}

bool PolicyCompiler_ipf::optimizeDst::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementDst *dstrel=rule->getDst();
    RuleElementSrv *srvrel=rule->getSrv();

    int srcn=srcrel->size();
    int dstn=dstrel->size();
    int srvn=srvrel->size();

/* without optimization we generate N^3 rules (n1*n2*n3), with it we
 * generate 3*N (n1+n2+n3) rules. If n1+n2+n3 is greater than
 * n1*n2*n3, then we should not optimize
 */
    if (dstrel->isAny() || (srcn+dstn+srvn>=srcn*dstn*srvn))
    {
        tmp_queue.push_back(rule);
        return true;
    }

    optimizeForRuleElement(rule,RuleElementDst::TYPENAME);

    return true;
}

bool PolicyCompiler_ipf::optimizeSrv::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementDst *dstrel=rule->getDst();
    RuleElementSrv *srvrel=rule->getSrv();

    int srcn=srcrel->size();
    int dstn=dstrel->size();
    int srvn=srvrel->size();

/* without optimization we generate N^3 rules (n1*n2*n3), with it we
 * generate 3*N (n1+n2+n3) rules. If n1+n2+n3 is greater than
 * n1*n2*n3, then we should not optimize
 */
    if (srvrel->isAny() || (srcn+dstn+srvn>=srcn*dstn*srvn))
    {
        tmp_queue.push_back(rule);
        return true;
    }

    optimizeForRuleElement(rule,RuleElementSrv::TYPENAME);

    return true;
}


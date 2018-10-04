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

#include "NATCompiler_pf.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Firewall.h"


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;




/*
 *   negation in OSrc :
 *
 *   !A    B    C    RULE_TYPE  
 *-----------------------------------------------
 *
 *    A    B    C    Continue ("no nat")
 *   any   B    C    SNAT/DNAT 
 */
bool NATCompiler_pf::doOSrcNegation::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrc *osrcrel=rule->getOSrc();

    if (osrcrel->getNeg()) {
	NATRule     *r;

	osrcrel->setNeg(false);

	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setRuleType(NATRule::Continue);
	tmp_queue.push_back(r);

	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	RuleElementOSrc *nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
	tmp_queue.push_back(r);
    } else
	tmp_queue.push_back(rule);

    return true;
}

/*
 *   negation in Odst :
 *
 *   A    !B   C    RULE_TYPE  TARGET
 *-------------------------------------
 *
 *   A    B    C    Continue ("no nat")
 *   A   any   C    SNAT/DNAT 
 */
bool NATCompiler_pf::doODstNegation::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementODst *odstrel=rule->getODst();

    if (odstrel->getNeg()) {
	NATRule     *r;

	odstrel->setNeg(false);

	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setRuleType(NATRule::Continue);
	tmp_queue.push_back(r);

	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	RuleElementODst *ndst=r->getODst();  ndst->clearChildren();  ndst->setAnyElement();
	tmp_queue.push_back(r);
    } else
	tmp_queue.push_back(rule);

    return true;
}


/*
 *   negation in OSrv :
 *
 *   A    B   !C    RULE_TYPE  TARGET
 *-------------------------------------
 * 
 *   A    B    C    Continue ("no nat")
 *   A    B   any   SNAT/DNAT 
 */
bool NATCompiler_pf::doOSrvNegation::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrv *osrvrel=rule->getOSrv();

/*   A  B ! C    */

    if (osrvrel->getNeg()) {
	NATRule     *r;

	osrvrel->setNeg(false);

	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setRuleType(NATRule::Continue);
	tmp_queue.push_back(r);

	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	RuleElementOSrv *nsrv=r->getOSrv();  nsrv->clearChildren();  nsrv->setAnyElement();
	tmp_queue.push_back(r);
    } else
	tmp_queue.push_back(rule);

    return true;
}


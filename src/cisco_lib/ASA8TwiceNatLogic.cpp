/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

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

#include "ASA8TwiceNatLogic.h"

#include "fwbuilder/RuleElement.h"
#include "fwbuilder/FWOptions.h"

#include <QtDebug>


using namespace libfwbuilder;
using namespace std;


ASA8TwiceNatStaticLogic::ASA8TwiceNatStaticLogic(NATRule *_rule)
{
    rule = _rule;
}

ASA8TwiceNatStaticLogic::TwiceNatRuleType ASA8TwiceNatStaticLogic::getAutomaticType()
{
    RuleElementOSrc *osrc_re = rule->getOSrc();
    assert(osrc_re!=NULL);
    Address  *osrc = Address::cast(FWReference::getObject(osrc_re->front()));

    RuleElementTSrc *tsrc_re = rule->getTSrc();
    assert(tsrc_re!=NULL);
    Address  *tsrc = Address::cast(FWReference::getObject(tsrc_re->front()));

    if (tsrc_re->isAny()) return STATIC;
    else
    {
        /*
         * Default behavior: if the number of ip addresses in OSrc is
         * equal to that in TSrc, then use "static". Otherwise use
         * "dynamic". Note that TSrc may be a group, in which case we
         * assume it has different number of addresses and we fall
         * back to dynamic
         */
        if (tsrc == NULL) return DYNAMIC;
        if (osrc->dimension() == tsrc->dimension()) return STATIC;
        else return DYNAMIC;
    }
    return DYNAMIC;
}

ASA8TwiceNatStaticLogic::TwiceNatRuleType ASA8TwiceNatStaticLogic::getType()
{
    TwiceNatRuleType res = getAutomaticType();
    FWOptions *ropt = rule->getOptionsObject();

    if (ropt->getBool("asa8_nat_dynamic")) res = DYNAMIC;
    if (ropt->getBool("asa8_nat_static")) res = STATIC;
    
    return res;
}


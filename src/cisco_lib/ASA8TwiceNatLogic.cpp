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
#include "fwbuilder/Network.h"

#include <QtDebug>


using namespace libfwbuilder;
using namespace std;


ASA8TwiceNatStaticLogic::ASA8TwiceNatStaticLogic(NATRule *_rule)
{
    rule = _rule;
}

int ASA8TwiceNatStaticLogic::countAddresses(FWObject *re)
{
    int res = 0;
    for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o = FWReference::getObject(*i1);
        if (Group::cast(o) != nullptr) res += countAddresses(o);
        else
        {
            Address *addr = Address::cast(o);
            assert(addr);
            res += addr->dimension();
        }
    }
    return res;
}

ASA8TwiceNatStaticLogic::TwiceNatRuleType ASA8TwiceNatStaticLogic::getAutomaticType()
{
    RuleElementOSrc *osrc_re = rule->getOSrc();
    assert(osrc_re!=nullptr);
    //Address  *osrc = Address::cast(FWReference::getObject(osrc_re->front()));

    RuleElementTSrc *tsrc_re = rule->getTSrc();
    assert(tsrc_re!=nullptr);
    //Address  *tsrc = Address::cast(FWReference::getObject(tsrc_re->front()));

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
        if (tsrc_re->size() > 1) return DYNAMIC;
        //if (tsrc == nullptr) return DYNAMIC;

        Address *tsrc = Address::cast(FWReference::getObject(tsrc_re->front()));
        // ASA sez: "ERROR: Subnet can not be used as mapped source in
        // dynamic NAT policy."
        if (Network::isA(tsrc)) return STATIC;
        // If we tranlate one-to-one, then use static as well
        if (countAddresses(osrc_re) == countAddresses(tsrc_re)) return STATIC;
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


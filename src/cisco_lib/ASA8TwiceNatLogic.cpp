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


using namespace libfwbuilder;
using namespace std;


ASA8TwiceNatStaticLogic::ASA8TwiceNatStaticLogic(NATRule *_rule)
{
    rule = _rule;
}

bool ASA8TwiceNatStaticLogic::isStatic()
{
    RuleElementOSrc *osrc_re = rule->getOSrc();
    assert(osrc_re!=NULL);
    Address  *osrc = Address::cast(FWReference::getObject(osrc_re->front()));

    RuleElementOSrc *tsrc_re = rule->getOSrc();
    assert(tsrc_re!=NULL);
    Address  *tsrc = Address::cast(FWReference::getObject(tsrc_re->front()));

    FWOptions *ropt = rule->getOptionsObject();

    if (tsrc->isAny()) return true;
    else
    {
        /*
         * Default behavior: if the number of ip addresses in OSrc is equal to
         * that in TSrc, then use "static". Otherwise use "dynamic". However if
         * rule option "asa8_nat_static" is true, use "static".
         */
        if (osrc->dimension() == tsrc->dimension()) return true;
        else
        {
            if (ropt->getBool("asa8_nat_static")) return true;
            else  return false;
        }
    }
    return false;
}

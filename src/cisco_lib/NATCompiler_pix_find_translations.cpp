/* 

                          Firewall Builder

                 Copyright (C) 2002-2011 NetCitadel, LLC

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

#include "NATCompiler_pix.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/RuleSet.h"

#include <sstream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


class triplet {
    public:
            libfwbuilder::Address *src;
            libfwbuilder::Address *dst;
            libfwbuilder::Service *srv;
            triplet() {src=nullptr; dst=nullptr; srv=nullptr;}
            triplet(libfwbuilder::Address *s,
                    libfwbuilder::Address *d,
                    libfwbuilder::Service *v) {src=s; dst=d; srv=v;}

            std::string hash();
};



std::string triplet::hash()
{
    ostringstream ostr;
    string dst_str;
    string src_str;

    Interface *intf = Interface::cast(src);
    if (intf && intf->isDyn()) src_str = intf->getId();
    else src_str = src->getAddressPtr()->toString();

    intf = Interface::cast(dst);
    if (intf && intf->isDyn()) dst_str = intf->getId();
    else dst_str = dst->getAddressPtr()->toString();

    ostr << src_str << "." << dst_str <<"." << srv->getId();
    return ostr.str();
}


list<NATRule*> NATCompiler_pix::findMatchingDNATRules(
    Address *src, Address *dst, Service *srv,
    const string &nat_re_type_to_match_dst)
{
    list<NATRule*> res;
    map<string,NATRule*> res_dict;

    FWObject *final_ruleset = persistent_objects->getRoot()->findInIndex(final_ruleset_id);

    for (FWObject::iterator i=final_ruleset->begin(); i!=final_ruleset->end(); ++i)
    {
        NATRule *rule = NATRule::cast(*i);
        if (rule == nullptr) continue; // skip RuleSetOptions object

        switch (rule->getRuleType())
        {
        case NATRule::DNAT:
        {
            FWObject *re_to_compare =
                rule->getFirstByType(nat_re_type_to_match_dst);
            Address *dst_to_compare = Address::cast(
                FWReference::getObject(re_to_compare->front()));

            Address  *osrc = getFirstOSrc(rule);  assert(osrc);
            Address  *odst = getFirstODst(rule);  assert(odst);
            Service  *osrv = getFirstOSrv(rule);  assert(osrv);

#ifndef NDEBUG
            Address  *tsrc = getFirstTSrc(rule);  assert(tsrc);
#endif
            // Address  *tdst = getFirstTDst(rule);  assert(tdst);
            Service  *tsrv = getFirstTSrv(rule);  assert(tsrv);

            const InetAddr *dst_to_compare_addr = dst_to_compare->getAddressPtr();

            // dst_to_compare_addr can be nullptr if object in rule
            // element is a dynamic interface or a group. We should
            // have expanded groups by now, but dynamic interface can
            // still be there.

            if (*(src->getAddressPtr()) == *(osrc->getAddressPtr()) &&
                (osrv->isAny() || srv->getId()==tsrv->getId()) &&
                (dst_to_compare_addr == nullptr || 
                 *(dst->getAddressPtr()) == *(dst_to_compare_addr)))
            {
                if (osrv->isAny())
                {
                    triplet tr(src, odst, srv);
                    res_dict[tr.hash()] = rule;
                }
                else
                {
                    triplet tr(src, odst, osrv);
                    res_dict[tr.hash()] = rule;
                }
            }
        }
        break;
        default: ;    // TODO: should actually be always_assert
        }
    }
    for (map<string,NATRule*>::iterator i=res_dict.begin(); i!=res_dict.end(); ++i)
    {
        res.push_back(i->second);
    }
    return res;
}
	


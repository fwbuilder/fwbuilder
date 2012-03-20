/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include <assert.h>

#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"


#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Firewall.h"

#include <iostream>

using namespace std;
using namespace libfwbuilder;

const char *FWOptions::TYPENAME={"FWOptions"};

FWOptions::FWOptions() 
{
    remStr("comment");
    remStr("name"   );
    remStr("id"     );
}

void FWOptions::fromXML(xmlNodePtr root) throw(FWException)
{
    const char *n;
    const char *cont;

    for (xmlNodePtr cur = root->xmlChildrenNode; cur; cur = cur->next)   
    {
        if (cur && !xmlIsBlankNode(cur))    
        {
            n = FROMXMLCAST(xmlGetProp(cur,TOXMLCAST("name")));
            assert(n!=NULL);
            cont = FROMXMLCAST( xmlNodeGetContent(cur) );
            if (cont)
            {
                setStr(n, cont );
                FREEXMLBUFF(cont);
            }
            FREEXMLBUFF(n);
        }
    }
}

xmlNodePtr FWOptions::toXML(xmlNodePtr root) throw(FWException)
{
    xmlNodePtr opt;

    xmlNodePtr me = xmlNewChild(
        root, NULL, 
        xml_name.empty() ? STRTOXMLCAST(getTypeName()) : STRTOXMLCAST(xml_name),
        NULL);

    for(map<string, string>::const_iterator i=data.begin(); i!=data.end(); ++i)
    {
        const string &name  = (*i).first;
        const string &value = (*i).second;

        if (name[0]=='.') continue;
        
        xmlChar *valbuf = xmlEncodeSpecialChars(root->doc,
                                                STRTOXMLCAST(value) );
//        xmlChar *valbuf = xmlEncodeEntitiesReentrant(root->doc,
//                                                     STRTOXMLCAST(value) );
        opt = xmlNewChild(me, NULL, TOXMLCAST("Option"), valbuf);
        FREEXMLBUFF(valbuf);

        xmlNewProp(opt, TOXMLCAST("name") , STRTOXMLCAST(name));
    }

    return me;
}

const char *HostOptions::TYPENAME  ={"HostOptions"};
HostOptions::HostOptions() : FWOptions() {}

const char *FirewallOptions::TYPENAME  ={"FirewallOptions"};
FirewallOptions::FirewallOptions() : FWOptions() {}

const char *InterfaceOptions::TYPENAME  ={"InterfaceOptions"};
InterfaceOptions::InterfaceOptions() : FWOptions() {}

const char *RuleSetOptions::TYPENAME={"RuleSetOptions"};
RuleSetOptions::RuleSetOptions() : FWOptions() {}

const char *PolicyRuleOptions::TYPENAME={"PolicyRuleOptions"};
PolicyRuleOptions::PolicyRuleOptions() : FWOptions() {}

const char *NATRuleOptions::TYPENAME={"NATRuleOptions"};
NATRuleOptions::NATRuleOptions() : FWOptions() {}

const char *RoutingRuleOptions::TYPENAME={"RoutingRuleOptions"};
RoutingRuleOptions::RoutingRuleOptions() : FWOptions() {}

const char *ClusterGroupOptions::TYPENAME={"ClusterGroupOptions"};
ClusterGroupOptions::ClusterGroupOptions() : FWOptions() {}



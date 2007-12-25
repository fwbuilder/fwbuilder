/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: FWOptions.cpp 975 2006-09-10 22:40:37Z vkurland $


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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/FWOptions.h>
#include <fwbuilder/Firewall.h>

using namespace std;
using namespace libfwbuilder;

const char *FWOptions::TYPENAME={"FWOptions"};

FWOptions::FWOptions() 
{
    remStr("comment");
    remStr("name"   );
    remStr("id"     );
}

FWOptions::FWOptions(const FWObject *root,bool prepopulate) :
    FWObject(root,prepopulate)
{
    remStr("comment");
    remStr("name"   );
    remStr("id"     );
}

void FWOptions::fromXML(xmlNodePtr root) throw(FWException)
{
    const char *n;
    const char *cont;

    for(xmlNodePtr cur=root->xmlChildrenNode; cur; cur=cur->next)   
    {
        if(cur && !xmlIsBlankNode(cur))    
        {

            n=FROMXMLCAST(xmlGetProp(cur,TOXMLCAST("name")));
            assert(n!=NULL);
            cont=FROMXMLCAST( xmlNodeGetContent(cur) );
            if (cont)
            {
                setStr(n, cont );
                FREEXMLBUFF(n);
                FREEXMLBUFF(cont);
            }
        }
    }
}

xmlNodePtr FWOptions::toXML(xmlNodePtr root) throw(FWException)
{
    xmlNodePtr opt;

    xmlNodePtr me = xmlNewChild(root, NULL, 
		xml_name.empty()?STRTOXMLCAST(getTypeName()):STRTOXMLCAST(xml_name), 
				NULL);


    map<string, string>::iterator i;
    for(i=data.begin(); i!=data.end(); ++i)  
    {
        string name  = (*i).first;
        string value = (*i).second;

        if (name[0]=='.') continue;
        
        xmlChar *valbuf = xmlEncodeSpecialChars(NULL, STRTOXMLCAST(value) );
        opt=xmlNewChild(me,NULL,TOXMLCAST("Option"),valbuf);
        FREEXMLBUFF(valbuf);
        xmlNewProp(opt, TOXMLCAST("name") , STRTOXMLCAST(name));
    }
    return me;
}

const char *HostOptions::TYPENAME  ={"HostOptions"};
HostOptions::HostOptions() : FWOptions() {}
HostOptions::HostOptions(const FWObject *root,bool prepopulate) : FWOptions(root,prepopulate) {}

const char *FirewallOptions::TYPENAME  ={"FirewallOptions"};
FirewallOptions::FirewallOptions() : FWOptions() {}
FirewallOptions::FirewallOptions(const FWObject *root,bool prepopulate) : FWOptions(root,prepopulate) {}

const char *PolicyRuleOptions::TYPENAME={"PolicyRuleOptions"};
PolicyRuleOptions::PolicyRuleOptions() : FWOptions() {}
PolicyRuleOptions::PolicyRuleOptions(const FWObject *root,bool prepopulate) : FWOptions(root,prepopulate) {}

const char *NATRuleOptions::TYPENAME={"NATRuleOptions"};

NATRuleOptions::NATRuleOptions() : FWOptions() {}
NATRuleOptions::NATRuleOptions(const FWObject *root,bool prepopulate) : FWOptions(root,prepopulate) {}

const char *RoutingRuleOptions::TYPENAME={"RoutingRuleOptions"};
RoutingRuleOptions::RoutingRuleOptions() : FWOptions() {}
RoutingRuleOptions::RoutingRuleOptions(const FWObject *root,bool prepopulate) : FWOptions(root,prepopulate) {}


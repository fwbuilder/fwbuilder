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

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/XMLTools.h"

using namespace std;
using namespace libfwbuilder;

const char *CustomService::TYPENAME={"CustomService"};

CustomService::CustomService() { address_family = AF_INET;}
CustomService::~CustomService() {}

string CustomService::getProtocolName() const
{
    if (protocol.empty()) return "any";
    return protocol;
}
int    CustomService::getProtocolNumber() const { return 65000; }


FWObject& CustomService::shallowDuplicate(const FWObject *x,
                                          bool preserve_id) throw(FWException)
{
    const CustomService *cs = dynamic_cast<const CustomService *>(x);
    codes = cs->codes;
    protocol = cs->protocol;
    address_family = cs->address_family;

    return FWObject::shallowDuplicate(x, preserve_id);
}

void CustomService::fromXML(xmlNodePtr root) throw(FWException)
{
    const char *n;
    const char *cont;

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("name")));
    if (n)
    {
        setName(n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("id")));
    if (n)
    {
        setId(FWObjectDatabase::registerStringId(n));
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("comment")));
    if (n)
    {
        setComment(XMLTools::unquote_linefeeds(n));
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("protocol")));
    if (n)
    {
        setProtocol(XMLTools::unquote_linefeeds(n));
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("address_family")));
    if (n)
    {
        string af(XMLTools::unquote_linefeeds(n));
        if (af=="ipv6") setAddressFamily(AF_INET6);
        else setAddressFamily(AF_INET);
        FREEXMLBUFF(n);
    }

    for (xmlNodePtr cur=root->xmlChildrenNode; cur; cur=cur->next)
    {
        if (cur && !xmlIsBlankNode(cur))
        {
    	    n = FROMXMLCAST(xmlGetProp(cur,TOXMLCAST("platform")));
            assert(n!=NULL);
            cont = FROMXMLCAST( xmlNodeGetContent(cur) );
            if (cont)
            {
                setCodeForPlatform(n, cont );
                FREEXMLBUFF(cont);
            }
            FREEXMLBUFF(n);
        }
    }
}

xmlNodePtr CustomService::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr opt;

    xmlNodePtr me = FWObject::toXML(parent);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

    xmlNewProp(me, TOXMLCAST("protocol"), STRTOXMLCAST(getProtocol()));
    string af;
    if (getAddressFamily() == AF_INET6) af ="ipv6";
    else af = "ipv4";
    xmlNewProp(me, TOXMLCAST("address_family"), STRTOXMLCAST(af));

    map<string, string>::const_iterator i;
    for(i=codes.begin(); i!=codes.end(); ++i)  
    {
        const string &platform  = (*i).first;
        const string &code      = (*i).second;
        xmlChar *codebuf = xmlEncodeSpecialChars(NULL, STRTOXMLCAST(code) );
        opt=xmlNewChild(me,NULL,TOXMLCAST("CustomServiceCommand"), codebuf);
        FREEXMLBUFF(codebuf);

        xmlNewProp(opt, TOXMLCAST("platform") , STRTOXMLCAST(platform));
    }
    return me;
}

bool CustomService::cmp(const FWObject *obj, bool recursive) throw(FWException)
{
    if (CustomService::constcast(obj)==NULL) return false;
    if (!FWObject::cmp(obj, recursive)) return false;
    
    const CustomService *o2 = CustomService::constcast(obj);

    if (protocol!=o2->protocol || address_family!=o2->address_family)
        return false;

    map<string, string>::const_iterator i;
    for (i=codes.begin(); i!=codes.end(); ++i)  
    {
        const string &platform  = (*i).first;
        const string &code      = (*i).second;

        if (o2->codes.count(platform)==0 ) return false;
        map<string, string>::const_iterator j=o2->codes.find(platform);
        if ( (*j).second!=code)     return false;
    }
    return true;
}

void  CustomService::setCodeForPlatform(const string& platform,
                                        const string& code)
{
    codes[platform]=code;
}

const string CustomService::getCodeForPlatform(const string& platform) const
{
    std::map<std::string,std::string>::const_iterator it = codes.find(platform);
    if (it == codes.end()) return "";
    return it->second;
}

list<string> CustomService::getAllKnownPlatforms()
{
    list<string> res;
    map<string, string>::const_iterator i;
    for (i=codes.begin(); i!=codes.end(); ++i)  
    {
        res.push_back( (*i).first);
    }
    return res;
}

void  CustomService::setProtocol(const string& proto)
{
    protocol = proto;
}

const string& CustomService::getProtocol()
{
    if (protocol.empty()) protocol = "any";
    return protocol;
}

void  CustomService::setAddressFamily(int af)
{
    address_family = af;
}

int CustomService::getAddressFamily()
{
    if (address_family==-1) return AF_INET;
    return address_family;
}

bool CustomService::isV4Only()
{
    return (getAddressFamily() == AF_INET);
}

bool CustomService::isV6Only()
{
    return (getAddressFamily() == AF_INET6);
}

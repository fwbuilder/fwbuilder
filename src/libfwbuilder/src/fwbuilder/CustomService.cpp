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
                                          bool preserve_id)
{
    const CustomService *cs = dynamic_cast<const CustomService *>(x);
    codes = cs->codes;
    protocol = cs->protocol;
    address_family = cs->address_family;

    return FWObject::shallowDuplicate(x, preserve_id);
}

void CustomService::fromXML(xmlNodePtr root)
{
    const char *n;
    const char *cont;

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("name")));
    if (n)
    {
        setName(n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("id")));
    if (n)
    {
        setId(FWObjectDatabase::registerStringId(n));
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("comment")));
    if (n)
    {
        setComment(XMLTools::unquote_linefeeds(n));
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("protocol")));
    if (n)
    {
        setProtocol(XMLTools::unquote_linefeeds(n));
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("address_family")));
    if (n)
    {
        string af(XMLTools::unquote_linefeeds(n));
        if (af=="ipv6") setAddressFamily(AF_INET6);
        else setAddressFamily(AF_INET);
        XMLTools::FreeXmlBuff(n);
    }

    for (xmlNodePtr cur=root->xmlChildrenNode; cur; cur=cur->next)
    {
        if (cur && !xmlIsBlankNode(cur))
        {
            n = XMLTools::FromXmlCast(xmlGetProp(cur,XMLTools::ToXmlCast("platform")));
            assert(n!=nullptr);
            cont = XMLTools::FromXmlCast( xmlNodeGetContent(cur) );
            if (cont)
            {
                setCodeForPlatform(n, cont );
                XMLTools::FreeXmlBuff(cont);
            }
            XMLTools::FreeXmlBuff(n);
        }
    }
}

xmlNodePtr CustomService::toXML(xmlNodePtr parent)
{
    xmlNodePtr opt;

    xmlNodePtr me = FWObject::toXML(parent);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    xmlNewProp(me, XMLTools::ToXmlCast("protocol"), XMLTools::StrToXmlCast(getProtocol()));
    string af;
    if (getAddressFamily() == AF_INET6) af ="ipv6";
    else af = "ipv4";
    xmlNewProp(me, XMLTools::ToXmlCast("address_family"), XMLTools::StrToXmlCast(af));

    map<string, string>::const_iterator i;
    for(i=codes.begin(); i!=codes.end(); ++i)  
    {
        const string &platform  = (*i).first;
        const string &code      = (*i).second;
        xmlChar *codebuf = xmlEncodeSpecialChars(nullptr, XMLTools::StrToXmlCast(code) );
        opt=xmlNewChild(me,nullptr,XMLTools::ToXmlCast("CustomServiceCommand"), codebuf);
        XMLTools::FreeXmlBuff(codebuf);

        xmlNewProp(opt, XMLTools::ToXmlCast("platform") , XMLTools::StrToXmlCast(platform));
    }
    return me;
}

bool CustomService::cmp(const FWObject *obj, bool recursive)
{
    if (CustomService::constcast(obj)==nullptr) return false;
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

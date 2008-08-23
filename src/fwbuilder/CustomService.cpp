/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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


#include <assert.h>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/CustomService.h>
#include <fwbuilder/XMLTools.h>

using namespace std;
using namespace libfwbuilder;

const char *CustomService::TYPENAME={"CustomService"};

CustomService::CustomService() {}
CustomService::CustomService(const FWObject *root,bool prepopulate) :
    Service(root,prepopulate) {}
CustomService::~CustomService() {}

string CustomService::getProtocolName()     {    return "custom_service";}
int    CustomService::getProtocolNumber()   {    return 65000; }


FWObject& CustomService::shallowDuplicate(const FWObject *x, bool preserve_id) throw(FWException)
{
    const CustomService *cs = dynamic_cast<const CustomService *>(x);
    codes = cs->codes;
    return FWObject::shallowDuplicate(x, preserve_id);
}

void CustomService::fromXML(xmlNodePtr root) throw(FWException)
{
    const char *n;
    const char *cont;

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("name")));
    if(n)
    {
        setName(n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("id")));
    if(n)
    {
        setId(FWObjectDatabase::registerStringId(n));
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("comment")));
    if(n)
    {
        setComment(XMLTools::unquote_linefeeds(n));
        FREEXMLBUFF(n);
    }

    for(xmlNodePtr cur=root->xmlChildrenNode; cur; cur=cur->next)
    {
        if(cur && !xmlIsBlankNode(cur))
        {
    	    n=FROMXMLCAST(xmlGetProp(cur,TOXMLCAST("platform")));
	        assert(n!=NULL);
	        cont=FROMXMLCAST( xmlNodeGetContent(cur) );
	        if (cont)
            {
		        setCodeForPlatform(n, cont );
                FREEXMLBUFF(n);
                FREEXMLBUFF(cont);
            }
        }
    }
}

xmlNodePtr CustomService::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr opt;

    xmlNodePtr me = FWObject::toXML(parent);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));

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

bool CustomService::cmp(const FWObject *obj) throw(FWException)
{
    if (CustomService::constcast(obj)==NULL) return false;
    if (!FWObject::cmp(obj)) return false;
    
    const CustomService *o2=CustomService::constcast(obj);

    map<string, string>::const_iterator i;
    for(i=codes.begin(); i!=codes.end(); ++i)  
    {
        const string &platform  = (*i).first;
        const string &code      = (*i).second;

        if (o2->codes.count(platform)==0 ) return false;
        map<string, string>::const_iterator j=o2->codes.find(platform);
        if ( (*j).second!=code)     return false;
    }
    return true;
}

void  CustomService::setCodeForPlatform(const string& platform, const string& code)
{
    codes[platform]=code;
}

const string& CustomService::getCodeForPlatform(const string& platform)
{
    return codes[platform];
}










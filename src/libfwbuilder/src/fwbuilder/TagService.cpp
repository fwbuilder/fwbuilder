
/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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


#include "fwbuilder/TagService.h"
#include "fwbuilder/XMLTools.h"

using namespace libfwbuilder;
using namespace std;

const char *TagService::TYPENAME={"TagService"};

string TagService::getProtocolName() const     {    return "tag_service";}
int    TagService::getProtocolNumber() const   {    return 65001; }

TagService::TagService() 
{
    setStr("tagcode", "");
}

TagService::~TagService() {}

void TagService::setCode(const string &p)
{
    setStr("tagcode", p);
}

string TagService::getCode() const
{
    return getStr("tagcode");
}

void TagService::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("tagcode")));
    assert(n!=NULL);
    setStr("tagcode", n);
    FREEXMLBUFF(n);
    

}

xmlNodePtr TagService::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

    return me;
}









/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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



#include "fwbuilder/UserService.h"
#include "fwbuilder/XMLTools.h"

using namespace std;
using namespace libfwbuilder;

const char *UserService::TYPENAME={"UserService"};

UserService::UserService() {}
UserService::~UserService() {}

string UserService::getProtocolName() const     {    return "user_service";}
int    UserService::getProtocolNumber() const   {    return 65002; }

FWObject& UserService::shallowDuplicate(const FWObject *x,
                                        bool preserve_id)
{
    const UserService *cs = dynamic_cast<const UserService *>(x);
    userid = cs->userid;
    return FWObject::shallowDuplicate(x, preserve_id);
}

void UserService::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    const char *n = XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("userid")));
    if(n)
    {
        userid = string(n);
        XMLTools::FreeXmlBuff(n);
    }
}

xmlNodePtr UserService::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    xmlNewProp(me, XMLTools::ToXmlCast("userid"), XMLTools::StrToXmlCast(userid));
    return me;
}

bool UserService::cmp(const FWObject *obj, bool recursive)
{
    if (UserService::constcast(obj)==nullptr) return false;
    if (!FWObject::cmp(obj, recursive)) return false;
    
    const UserService *user_serv = UserService::constcast(obj);
    return (userid == user_serv->userid);
}


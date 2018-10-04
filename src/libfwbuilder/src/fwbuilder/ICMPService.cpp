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



#include "fwbuilder/ICMPService.h"
#include "fwbuilder/XMLTools.h"

using namespace libfwbuilder;
using namespace std;

const char *ICMPService::TYPENAME={"ICMPService"};

ICMPService::ICMPService() 
{
    setStr("type", "-1");
    setStr("code", "-1");
}

ICMPService::~ICMPService() {}

string ICMPService::getProtocolName() const    {    return "icmp";}
int    ICMPService::getProtocolNumber() const  {    return 1;     }

void ICMPService::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    const char *n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("type")));
    assert(n!=nullptr);
    setStr("type", n);
    XMLTools::FreeXmlBuff(n);

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("code")));
    if(n!=nullptr)
    {
        setStr("code", n);
        XMLTools::FreeXmlBuff(n);
    }
}

xmlNodePtr ICMPService::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    return me;
}


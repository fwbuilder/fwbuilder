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

void ICMPService::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("type")));
    assert(n!=NULL);
    setStr("type", n);
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("code")));
    if(n!=NULL)
    {
        setStr("code", n);
        FREEXMLBUFF(n);
    }
}

xmlNodePtr ICMPService::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

    return me;
}


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



#include "fwbuilder/FWObject.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/XMLTools.h"

using namespace libfwbuilder;
using namespace std;

const char *Library::TYPENAME={"Library"};

Library::Library()  {}

Library::~Library() 
{
}

bool  Library::validateChild(FWObject*)
{ 
    return true;   // anything goes
}

void Library::fromXML(xmlNodePtr root)
{
    const char *n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("color")));
    if(n!=nullptr)  // color is not a mandatory attribute
    {
        setStr("color", n);
        XMLTools::FreeXmlBuff(n);
    }
    FWObject::fromXML(root);
}

xmlNodePtr Library::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j)
        (*j)->toXML(me);

    return me;
}





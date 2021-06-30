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



#include "fwbuilder/FWReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace std;

const char *FWReference::TYPENAME={"Ref"};


/*
FWReference::FWReference(FWObject *p)
{
    setPointer(p);
}
*/


FWReference::FWReference()
{
    setPointer(nullptr);
}

FWReference::~FWReference()  {}


void FWReference::fromXML(xmlNodePtr root)
{
    assert(root!=nullptr);
    FWObject::fromXML(root);

    const char *n = XMLTools::FromXmlCast(xmlGetProp(root, XMLTools::ToXmlCast("ref")));
    assert(n!=nullptr);
    str_ref = n;
    //setInt("ref", n);
    // if object with id str_ref has not been loaded yet, 
    // FWObjectDatabase::getIntId returns -1.
    int_ref = FWObjectDatabase::getIntId(str_ref);
    XMLTools::FreeXmlBuff(n);
}

// Note that XML elements represented by FWReference have only one
// attribute "ref" and no value
xmlNodePtr FWReference::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = xmlNewChild(
        parent,
        nullptr,
        xml_name.empty() ? XMLTools::StrToXmlCast(getTypeName()) : XMLTools::StrToXmlCast(xml_name),
        nullptr);

    if (int_ref == -1 && !str_ref.empty())
        int_ref = FWObjectDatabase::getIntId(str_ref);

    str_ref = FWObjectDatabase::getStringId(int_ref);

    xmlNewProp(me, XMLTools::ToXmlCast("ref"), XMLTools::StrToXmlCast(str_ref));
    //xmlAddRef(nullptr, parent->doc, XMLTools::StrToXmlCast(str_ref), pr);

    return me;
}

FWObject& FWReference::shallowDuplicate(const FWObject *_other,
                                        bool)
{
    const FWReference *other = FWReference::constcast(_other);
    int_ref = other->int_ref;
    str_ref = other->str_ref;
    return *this;
}

bool FWReference::cmp(const FWObject *obj, bool /* UNUSED recursive */)
{
    const FWReference *rx = FWReference::constcast(obj);
    if (rx == nullptr) return false;
    if (int_ref != rx->int_ref || str_ref != rx->str_ref) return false;
    return true;
}

void FWReference::add(FWObject*,bool validate)
{
    (void) validate; // Unused
    throw std::string("Can't add to a reference !");
}

void FWReference::setPointer(FWObject *p)
{
    if(p) setPointerId(p->getId());
    else
    {
        int_ref = -1;
        str_ref = "";
    }
}

void FWReference::setPointerId(int ref_id)
{
    //setInt("ref" , ref_id );
    int_ref = ref_id;
    // if object with id ref_id has not been loaded into database
    // yet, FWObjectDatabase::getStringId returns empty string.
    // This works as postponed initialization.
    // We really need string id only in toXML.
    str_ref = FWObjectDatabase::getStringId(int_ref);
}

FWObject *FWReference::getPointer()
{
    return getRoot()->findInIndex( getPointerId() );
}

int FWReference::getPointerId()
{
    // postponed initialization happens now
    if (int_ref==-1 && !str_ref.empty())
        int_ref = FWObjectDatabase::getIntId(str_ref);
        
    return int_ref;
    //return getInt("ref");
}


void FWReference::dump(std::ostream &f, bool recursive, bool brief, int offset) const
{
    FWObject::dump(f, recursive, brief, offset);

    FWObject* ptr = getRoot()->findInIndex(int_ref);

    f << string(offset,' ') << "PointerId: " << int_ref << endl;
    f << string(offset,' ') << "Pointer: " << ptr << endl;
    if (ptr)
    {
	f << string(offset,' ') << "Ptr.name: " << ptr->getName() <<endl;
	f << string(offset,' ') << "Ptr.id: "   << ptr->getId() <<endl;
    }
}

FWObject* FWReference::getObject(FWObject* o)
{
    if (o==nullptr) return nullptr;
    if (FWReference::cast(o)!=nullptr) return FWReference::cast(o)->getPointer();
    return o;
}

/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: FWReference.cpp 975 2006-09-10 22:40:37Z vkurland $


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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/FWReference.h>
#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/XMLTools.h>

#include <assert.h>

using namespace libfwbuilder;
using namespace std;

const char *FWReference::TYPENAME={"Ref"};


/*
FWReference::FWReference(FWObject *p)
{
    setPointer(p);

    remStr("name");
    remStr("comment");
    remStr("id");
}
*/


FWReference::FWReference()
{
    remStr("name");
    remStr("comment");
    remStr("id");
    setPointer(NULL);
}

FWReference::FWReference(const FWObject *root,bool prepopulate) : FWObject(root,prepopulate)
{
    remStr("name");
    remStr("comment");
    remStr("id");
    setPointer(NULL);
}

FWReference::~FWReference()  {}


void FWReference::fromXML(xmlNodePtr root)  throw(FWException)
{
    assert(root!=NULL);
    FWObject::fromXML(root);

    const char *n = FROMXMLCAST(xmlGetProp(root,TOXMLCAST("ref")));
    assert(n!=NULL);
    setStr("ref", n);
    FREEXMLBUFF(n);
}

void FWReference::add(FWObject *obj)
{
    throw std::string("Can't add to a reference !");
}

void FWReference::setPointer(FWObject *p)
{
    if(p) setPointerId(p->getId());
}

void FWReference::setPointerId(const string &ref_id)
{
    setStr("ref" , ref_id );
}

FWObject *FWReference::getPointer()
{
    return getRoot()->findInIndex( getPointerId() );
}

const string& FWReference::getPointerId()
{
    return getStr("ref");
}


#ifdef _REF_DUMP_
void   FWReference::dump(int offset)
{
    FWObject::dump(offset);

    FWObject* ptr=getPointer();

    cout << string(offset,' ') << "Pointer: " << ptr << endl;
    if (ptr) {
	cout<< string(offset,' ') << "Ptr.name: " << ptr->getName() <<endl;
	cout<< string(offset,' ') << "Ptr.id: "   << ptr->getId() <<endl;
    }
}
#endif

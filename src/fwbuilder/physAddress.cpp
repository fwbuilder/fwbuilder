/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@vk.crocodile.org>

  $Id: physAddress.cpp 975 2006-09-10 22:40:37Z vkurland $

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

#include <fwbuilder/physAddress.h>
#include <fwbuilder/FWException.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWObjectDatabase.h>

#include <iostream>
//#include <ostream>

using namespace libfwbuilder;
using namespace std;

const char *physAddress::TYPENAME={"physAddress"};

physAddress::physAddress(const FWObject *root,bool prepopulate) :
    Address(root,prepopulate) 
{
    setPhysAddress("00:00:00:00:00:00");
}

void physAddress::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("name")));
    assert(n!=NULL);
    setName(n);
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("address")));
    assert(n!=NULL);
    setStr("address", n);
    FREEXMLBUFF(n);
}


IPAddress physAddress::getAddress() const
{
//    std::cerr << "physAddress::getAddress() method called" << endl;
    return IPAddress();
}

void physAddress::setAddress(const std::string &s)
{
    std::cerr << "physAddress::setAddress() method called" << endl;
}


std::string physAddress::getPhysAddress() const
{
    return getStr("address");
}

void physAddress::setPhysAddress(const std::string &s)
{
    setStr("address",s);
}

FWReference* physAddress::createRef()
{
//    FWObjectReference *ref=new FWObjectReference();
    FWObjectReference *ref=FWObjectReference::cast(getRoot()->create(FWObjectReference::TYPENAME));
    ref->setPointer(this);
    return ref;
}

bool physAddress::isAny() const
{
    return getPhysAddress()=="";
}


/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>

  $Id: MultiAddress.cpp 975 2006-09-10 22:40:37Z vkurland $

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

#include <fwbuilder/MultiAddress.h>
#include <fwbuilder/FWException.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/Network.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace libfwbuilder;
using namespace std;

const char *MultiAddress::TYPENAME={"MultiAddress"};

MultiAddress::MultiAddress() : FWObject() 
{
    source_attribute_name = "";
    setSourceName("");
    setRunTime(false);
}

MultiAddress::MultiAddress(const FWObject *root,bool prepopulate) :
    FWObject(root,prepopulate) 
{
    source_attribute_name = "";
    setSourceName("");
    setRunTime(false);
}

void MultiAddress::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);
    const char *n;
    
    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST(source_attribute_name.c_str())));
    assert(n!=NULL);
    setStr(source_attribute_name, n);
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("run_time")));
    assert(n!=NULL);
    setStr("run_time", n);
    FREEXMLBUFF(n);
}

xmlNodePtr MultiAddress::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(xml_parent_node,false);
    
    return me;
}

void MultiAddress::registerSourceAttributeName(const std::string &an)
{
    source_attribute_name = an;
}

std::string MultiAddress::getSourceName() const
{
   return getStr(source_attribute_name);
}

void MultiAddress::setSourceName(const std::string &s)
{
    if (!source_attribute_name.empty()) setStr(source_attribute_name,s);
}

bool MultiAddress::isCompileTime() const
{
    return !getBool("run_time");
}

bool MultiAddress::isRunTime() const
{
    return getBool("run_time");
}

void MultiAddress::setCompileTime(const bool b)
{
    setBool("run_time",!b);
}

void MultiAddress::setRunTime(const bool b)
{
    setBool("run_time",b);
}

bool MultiAddress::validateChild(FWObject *o)
{ 
    return ObjectGroup::validateChild(o);
}

void MultiAddress::loadFromSource() throw(FWException)
{
    cerr << "virtual function MultiAddress::loadFromSource is not implemented"
         << endl;
}

// ========================================================================

const char *MultiAddressRunTime::TYPENAME={"MultiAddressRunTime"};

MultiAddressRunTime::MultiAddressRunTime()
{
    setName("UnknownMultiAddressRunTime");
    source_name = "source";
    run_time = false;
    subst_type_name = "Unknown";
}

MultiAddressRunTime::MultiAddressRunTime(MultiAddress *maddr)
{
    setName(maddr->getName());
    source_name = maddr->getSourceName();
    run_time = maddr->isRunTime();
    subst_type_name = maddr->getTypeName();
}


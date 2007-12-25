/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: IPv4.cpp 975 2006-09-10 22:40:37Z vkurland $


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

#include <fwbuilder/IPv4.h>
#include <fwbuilder/InterfacePolicy.h>
#include <fwbuilder/XMLTools.h>

using namespace std;
using namespace libfwbuilder;

const char *IPv4::TYPENAME={"IPv4"};

IPv4::IPv4(const IPv4 &i):Address()
{
    FWObject::operator=(i);
}

IPv4::IPv4():Address()
{
    setName("address");
    setAddress("0.0.0.0");
    setNetmask("0.0.0.0");
}

IPv4::IPv4(const FWObject *root,bool prepopulate) : Address(root,prepopulate)
{
    setName("address");
    setAddress("0.0.0.0");
    setNetmask("0.0.0.0");
}

IPv4::IPv4(const string& a,const string& nm):Address()
{
    setAddress(a);
    setNetmask(nm);
} 

IPv4::~IPv4() {}

void IPv4::fromXML(xmlNodePtr root) throw(FWException)
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

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("netmask")));
    assert(n!=NULL);
    setStr("netmask", n);
    FREEXMLBUFF(n);

}

IPNetwork IPv4::getIPNetwork() const throw(FWException)
{
    return IPNetwork(IPAddress(getStr("address")), Netmask(getStr("netmask")) );
}

void      IPv4::setAddress(const std::string &a) { setStr("address",a);}
void      IPv4::setAddress(const IPAddress &a)   { setStr("address" , a.toString()  );}
void      IPv4::setNetmask(const std::string &nm){ setStr("netmask" , nm  );}
void      IPv4::setNetmask(const Netmask &nm)    { setStr("netmask" , nm.toString()  );}
IPAddress IPv4::getAddress() const{    return IPAddress( getStr("address") );}
Netmask   IPv4::getNetmask() const{    return Netmask( getStr("netmask") );}
guint32   IPv4::dimension()  const{    return 1;}


/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: Network.cpp 975 2006-09-10 22:40:37Z vkurland $


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

#include <fwbuilder/Network.h>
#include <fwbuilder/XMLTools.h>

#include <string>


using namespace libfwbuilder;
using namespace std;


const char *Network::TYPENAME={"Network"};

Network::Network() : Address() , 
		     address("0.0.0.0") , 
		     netmask("0.0.0.0")
{
}

Network::Network(const FWObject *root,bool prepopulate) : Address(root,prepopulate),
                                                          address("0.0.0.0") , 
                                                          netmask("0.0.0.0")
{
}

Network::Network(Network &o) : Address() , 
			       address(o.getAddress()) , 
			       netmask(o.getNetmask())
{
    FWObject::operator=(o);
}

Network::Network (const string &s) : Address()
{
    *this=s;
}
                                     

FWObject& Network::shallowDuplicate(const FWObject *o, bool preserve_id) throw(FWException)
{
    const Network *n=dynamic_cast<const Network *>(o);
    address = n->getAddress();
    netmask = n->getNetmask();
    return FWObject::shallowDuplicate(o, preserve_id);
}

void Network::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);
    
    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("address")));
    assert(n!=NULL);
    address = n;
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("netmask")));
    assert(n!=NULL);
    netmask = n;
    FREEXMLBUFF(n);
}

xmlNodePtr Network::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    
    xmlNewProp(me, 
               TOXMLCAST("address"),
               STRTOXMLCAST(address.toString()));
    
    xmlNewProp(me, 
               TOXMLCAST("netmask"),
               STRTOXMLCAST(netmask.toString()));
    
    return me;
}

guint32   Network::dimension()  const
{
/* 
 * TODO: this code not portable 'cause it implies specific to IPv4
 * maximum length of netmask  
 */
    int masklength=netmask.getLength();

    if (masklength==0) return 0;

    guint32  u=1;
    for (int i=0; i<32-masklength; ++i) u<<=1;

    return u;
}

/* check if host address bits are cleared */
bool Network::isValidRoutingNet() const
{
    for(unsigned i=0;i<4;i++) 
        if ( ( address[i] & netmask[i] ) != address[i] )
            return false;

    return true;
}

Network& Network::operator=(const string &s) throw(FWException)
{
    if(s.find_first_not_of(".1234567890/")!=string::npos)
    {
        throw FWException(string("Invalid IP address: '")+s+"'");
    }
    
    size_type pos=s.find("/");
    
    if (pos==string::npos)
    {
        setAddress(s);
        setNetmask("255.255.255.255");
    }
    else
    {
        setAddress(s.substr(0,pos));
        string netm=s.substr(pos+1);
        
        if (netm.find(".")==string::npos)
        {
            int d=atoi(netm.c_str());
            netmask=Netmask(d);
        }
        else
        {
            setNetmask(netm);
        }
    }
   return *this; 
}

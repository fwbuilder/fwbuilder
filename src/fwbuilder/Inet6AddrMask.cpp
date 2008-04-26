/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Inet6AddrMask.h>

#include <stdio.h>
#include <iostream>
#include <string>

#ifndef _WIN32
#  include <sys/types.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

using namespace std;
using namespace libfwbuilder;

Inet6AddrMask::Inet6AddrMask() : InetAddrMask(true)
{
    address = new Inet6Addr();
    netmask = new Inet6Addr();
    broadcast_address = new Inet6Addr();
    network_address = new Inet6Addr();
}

Inet6AddrMask::Inet6AddrMask(const Inet6Addr &a, const Inet6Addr &n) :
    InetAddrMask(true)
{    
    address = new Inet6Addr(a & n);
    netmask = new Inet6Addr(n);
    broadcast_address = new Inet6Addr();
    network_address = new Inet6Addr();
    setNetworkAndBroadcastAddress();
}

Inet6AddrMask::Inet6AddrMask(const Inet6AddrMask& other) : InetAddrMask(true)
{
    Inet6Addr *i6_addr = dynamic_cast<Inet6Addr*>(other.address);
    assert(i6_addr);
    Inet6Addr *i6_nm = dynamic_cast<Inet6Addr*>(other.netmask);
    assert(i6_nm);

    address = new Inet6Addr(*i6_addr);
    netmask = new Inet6Addr(*i6_nm);
    broadcast_address = new Inet6Addr();
    network_address = new Inet6Addr();
    setNetworkAndBroadcastAddress();
}

Inet6AddrMask::Inet6AddrMask(const string &s) throw(FWException) : InetAddrMask(true)
{
    address = new Inet6Addr();
    netmask = new Inet6Addr();
    broadcast_address = new Inet6Addr();
    network_address = new Inet6Addr();

    if(s.find_first_not_of(":1234567890/")!=string::npos)
    {
        throw FWException(string("Invalid IP address: '")+s+"'");
    }
    
    string::size_type pos=s.find("/");
    
    if (pos==string::npos)
    {
        setAddress(Inet6Addr(s));
        setNetmask(Inet6Addr(Inet6Addr::getAllOnes()));
    }
    else
    {
        setAddress(Inet6Addr(s.substr(0,pos)));
        string netm = s.substr(pos+1);
        
        if (netm.find(":")==string::npos)
        {
            // netmask is represented as /NN (length in bits)
            int d = atoi(netm.c_str());
            *netmask = Inet6Addr(d);
        }
        else
        {
            setNetmask(Inet6Addr(netm));
        }
    }
    setNetworkAndBroadcastAddress();
}

Inet6AddrMask::~Inet6AddrMask()
{
    // destructor of InetAddrMask deletes address, netmask
    // and other member variables
}


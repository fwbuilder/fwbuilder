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



#include "fwbuilder/InetAddrMask.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <climits>
#include <assert.h>

#ifndef _WIN32
#  include <sys/types.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

using namespace std;
using namespace libfwbuilder;

void InetAddrMask::setNetworkAndBroadcastAddress()
{
    // see #2670. Per RFC3021 network with netmask /31 has no network
    // and direct broadcast addresses.
    if (netmask->isV4() && netmask->getLength() >= 31)
    {
        *network_address = *address;
        *broadcast_address = InetAddr(32);
        *last_host = *address;
        if (netmask->getLength() == 31) *last_host = *last_host + 1;
    } else
    {
        *network_address = *address & *netmask;
        *broadcast_address = *address | (~(*netmask));
        *last_host = *broadcast_address;
    }
}

InetAddrMask::InetAddrMask(bool)
{
    // this constructor does not create address, netmask and other
    // variables. This constructor should only be used by classes that
    // inherit InetAddrMask and create address, netmask themselves,
    // such as Inet6AddrMask
    address = nullptr;
    netmask = nullptr;
    broadcast_address = nullptr;
    network_address = nullptr;
    last_host = nullptr;
}

InetAddrMask::InetAddrMask()
{
    address = new InetAddr();
    netmask = new InetAddr();
    broadcast_address = new InetAddr();
    network_address = new InetAddr();
    last_host = new InetAddr();
}

InetAddrMask::InetAddrMask(const InetAddr &a, const InetAddr &n)
{    
    address = new InetAddr(a & n);
    netmask = new InetAddr(n);
    broadcast_address = new InetAddr();
    network_address = new InetAddr();
    last_host = new InetAddr();
    setNetworkAndBroadcastAddress();
}

InetAddrMask::InetAddrMask(const InetAddrMask& other)
{
    address = new InetAddr(*(other.address));
    netmask = new InetAddr(*(other.netmask));
    broadcast_address = new InetAddr();
    network_address = new InetAddr();
    last_host = new InetAddr();
    setNetworkAndBroadcastAddress();
}

InetAddrMask::InetAddrMask(const string &s)
{
    address = new InetAddr();
    netmask = new InetAddr();
    broadcast_address = new InetAddr();
    network_address = new InetAddr();
    last_host = new InetAddr();

    if(s.find_first_not_of(".1234567890/")!=string::npos)
    {
        throw FWException(string("Invalid IP address: '")+s+"'");
    }
    
    string::size_type pos=s.find("/");
    
    if (pos==string::npos)
    {
        setAddress(InetAddr(s));
        setNetmask(InetAddr(InetAddr::getAllOnes()));
    }
    else
    {
        setAddress(InetAddr(s.substr(0,pos)));
        string netm = s.substr(pos+1);
        
        if (netm.find(".")==string::npos)
        {
            // netmask is represented as /NN (length in bits)
            int d = atoi(netm.c_str());
            *netmask = InetAddr(d);
        }
        else
        {
            setNetmask(InetAddr(netm));
        }
    }
    setNetworkAndBroadcastAddress();
}

InetAddrMask::~InetAddrMask()
{
    if (address!=nullptr) delete address;
    if (netmask!=nullptr) delete netmask;
    if (network_address!=nullptr) delete network_address;
    if (broadcast_address!=nullptr) delete broadcast_address;
    if (last_host!=nullptr) delete last_host;
}

bool InetAddrMask::isAny()
{
    return (address->isAny() && netmask->isAny());
}

void InetAddrMask::setAddress(const InetAddr &a)
{
    *address = a;
    setNetworkAndBroadcastAddress();
}

void InetAddrMask::setNetmask(const InetAddr &nm)
{
    *netmask = nm;
    setNetworkAndBroadcastAddress();
}


// check if address 'o' belongs to the network
bool InetAddrMask::belongs(const InetAddr &o) const
{
    return ((o & *netmask) == *network_address);
}

unsigned int InetAddrMask::dimension()  const
{
/* 
 * TODO: this code not portable 'cause it implies specific to IPv4
 * maximum length of netmask  
 */
    int masklength = netmask->getLength();
    int host_part = netmask->addressLengthBits() - masklength;
    if (host_part>=32) return INT_MAX;  // can be >32 if ipv6

    unsigned int u = 1;
    for (int i=0; i<host_part; ++i) u<<=1;

    return u;
}

InetAddrMask& InetAddrMask::operator=(const InetAddrMask &o)
{
    *address = *(o.address);
    *netmask = *(o.netmask);
    setNetworkAndBroadcastAddress();
    return *this;
}

bool InetAddrMask::operator<(const InetAddrMask &b)
{
    if (this->getAddressPtr()->addressFamily() !=
        b.getAddressPtr()->addressFamily()) return false;
    return *(this->getAddressPtr()) < *(b.getAddressPtr());
}

bool libfwbuilder::operator==(const InetAddrMask &a, const InetAddrMask &b)
{
    if (a.getAddressPtr()->addressFamily() !=
        b.getAddressPtr()->addressFamily()) return false;
    return ( *(a.getNetmaskPtr()) == *(b.getNetmaskPtr()) &&
             *(a.getAddressPtr()) == *(b.getAddressPtr()));
}

bool libfwbuilder::operator<(const InetAddrMask &a, const InetAddrMask &b)
{
    if (a.getAddressPtr()->addressFamily() !=
        b.getAddressPtr()->addressFamily()) return false;
    return *(a.getAddressPtr()) < *(b.getAddressPtr());
}

/* this is just a better interface to _convert_range_to_networks */
vector<InetAddrMask> libfwbuilder::convertAddressRange(const InetAddr &start,
                                                       const InetAddr &end)
{
    vector<InetAddrMask> res;
    _convert_range_to_networks(start,end,res);
    return res;
}



bool libfwbuilder::_convert_range_to_networks(const InetAddr &start,
                                              const InetAddr &end,
                                              vector<InetAddrMask> &res)
{
    if (end < start) return false;
    if (start == end)
    {
	res.push_back(InetAddrMask(
                          start,
                          InetAddr(InetAddr::getAllOnes(start.addressFamily()))));
	return false;
    }

    if (start.isAny() && end.isBroadcast())
    {
	res.push_back( InetAddrMask() );
	return false;
    }

    unsigned int  size = start.distance(end);

    if (size==2)
    {
	res.push_back(InetAddrMask(
                          start,
                          InetAddr(InetAddr::getAllOnes(start.addressFamily()))));
	res.push_back(InetAddrMask(
                          end,
                          InetAddr(InetAddr::getAllOnes(end.addressFamily()))));
	return false;
    }

/* determine closest power of 2 which is less or equal to size */
    unsigned int l = size;
    int   mask_bits = 0;
    while ( l!=0 ) { l>>=1; mask_bits++; }
    mask_bits--;
    mask_bits = start.addressLengthBits() - mask_bits;

/* mask_bits  represents number of '1'in the netmask for the new subnet */

/* test start address to see if it is a good network address for netmask */
    InetAddr   nm1(mask_bits);  // new netmask
    InetAddrMask tn1(start, nm1);

    InetAddr nstart;
    InetAddr nend;
    InetAddr   nnm;

    nstart = start;

    if (start != *(tn1.getAddressPtr()))
    {
/* we can not use start address for the network because it shifts
 * beginning of the range back after netmask is applied to it. Need to
 * make netmask longer and then find the first address higher than
 * start, which matches the netmask and can be used as a network
 * address
 */
        do
        {
            mask_bits++;
            nnm = InetAddr(mask_bits);
            tn1 = InetAddrMask(nstart, nnm);
        } while (start != *(tn1.getAddressPtr()) && mask_bits>0);
        nend = nstart;
        nend = nend | (~nnm);
    } else 
    {
/* find shortest netmask that yields subnet with end address within
 * required range. Start with very short netmask and keep making it longer
 * while the end of the subnet it defines is still above required end of
 * the range. Once the end moves inside the range, stop.
 */
        mask_bits--;
        do 
        {
            mask_bits++;
            nnm = InetAddr(mask_bits);  // new netmask
            nend = start;
            nend = nend  | (~nnm);
        } while (nend > end);
    }
/* new range starts from nstart and ends at nend */
    res.push_back( InetAddrMask(nstart, nnm) );

    if (!(nstart == start))
    {
/* there are some addresses between start and nstart */
        while (libfwbuilder::_convert_range_to_networks(start,nstart-1,res)) ;
    }

    if (!(nend == end))
    {
/* the remainder of the original range is nend+1 - end */
        while (libfwbuilder::_convert_range_to_networks(nend+1,end,res)) ;
    }

    return false;
}


vector<InetAddrMask> libfwbuilder::getOverlap(const InetAddrMask &n1,
                                              const InetAddrMask &n2)
{
    const InetAddr& s1 = *(n1.getAddressPtr());
    const InetAddr& s2 = *(n2.getAddressPtr());

    const InetAddr& m1 = *(n1.getNetmaskPtr());
    const InetAddr& m2 = *(n2.getNetmaskPtr());

    InetAddr e1 = s1 | (~m1);
    InetAddr e2 = s2 | (~m2);

/* 
 * now both networks are represented by their first and last addresses
 * 
 * we assume network 0.0.0.0/0.0.0.0 in fact represents the whole
 * possible range of ip addresses (for ip v4 it is 0.0.0.0 -
 * 255.255.255.255). Check for this condition and replace e1 or e2
 * accordingly if needed.
 *
 */
    if (s1.isAny() && m1.isAny())
        e1 = InetAddr(InetAddr::getAllOnes(s1.addressFamily()));
    if (s2.isAny() && m2.isAny())
        e2 = InetAddr(InetAddr::getAllOnes(s2.addressFamily()));

    vector<InetAddrMask>  res;

    if (e2 < s1) return res;
    if (e1 < s2) return res;

    if (s1 == s2 && e1 == e2)
    {
        // subnets are identical
        res.push_back(n1);
        return res;
    }

    if (e2 == s1)
    {
        // end of n2 == start of n1, overlap is just 1 address
        res.push_back(InetAddrMask(s1, InetAddr::getAllOnes(s1.addressFamily())));
        return res;
    }

    if (e1 == s2)
    {
        // end of n1 == start of n2, overlap is just 1 address
        res.push_back(InetAddrMask(s2, InetAddr::getAllOnes(s2.addressFamily())));
        return res;
    }

    if (s1 == s2 && e1 < e2)
    {
        // both subnets have the same start address, but n1 is smaller
        res.push_back(n1);
        return res;
    }

    if (s1 == s2 && e2 < e1)
    {
        // both subnets have the same start address, but n2 is smaller
        res.push_back(n2);
        return res;
    }

    if (s2 < s1 && e1 == e2)
    {
        // both subnets have the same end address, but n1 is smaller
        res.push_back(n1);
        return res;
    }

    if (s1 < s2 && e1 == e2)
    {
        // both subnets have the same end address, but n2 is smaller
        res.push_back(n2);
        return res;
    }
    
    if (s1 > s2 && e1 < e2)
    {
        // n1 fits inside of n2
        res.push_back(n1);
        return res;
    }

    if (s2 > s1 && e2 < e1)
    {
        // n2 fits inside of n1
        res.push_back(n2);
        return res;
    }


    InetAddr rs, re;

    // if (s1 == s2) { rs = s1; }
    if (s1 < s2)  { rs = s2; }
    if (s2 < s1)  { rs = s1; }

    // if (e1 == e2) { re = e1; }
    if (e1 < e2)  { re = e1; }
    if (e2 < e1)  { re = e2; }

/* rb and re represent resulting address range boundaries */

    libfwbuilder::_convert_range_to_networks(rs, re, res);

    return res;
}

vector<InetAddrMask> libfwbuilder::substract(const InetAddrMask &n1,
                                             const InetAddrMask &n2)
{
    const InetAddr n1s = *(n1.getAddressPtr());
    const InetAddr n2s = *(n2.getAddressPtr());

    const InetAddr n1m = *(n1.getNetmaskPtr());
    const InetAddr n2m = *(n2.getNetmaskPtr());

    InetAddr n1e = n1s; n1e = n1e | (~n1m);
    InetAddr n2e = n2s; n2e = n2e | (~n2m);

/* 
 * now both networks are represented by their first and last addresses
 * 
 * we assume network 0.0.0.0/0.0.0.0 in fact represents the whole
 * possible range of ip addresses (for ip v4 it is 0.0.0.0 -
 * 255.255.255.255). Check for this condition and replace n1e or n2e
 * accordingly if needed.
 */
    if (n1s.isAny() && n1e.isAny())
        n1e=InetAddr(InetAddr::getAllOnes());
    if (n2s.isAny() && n2e.isAny())
        n2e=InetAddr(InetAddr::getAllOnes());

    vector<InetAddrMask>  res;

    if (/* n2s<n1s && */ n2e<n1s)
    {
	res.push_back(n1);
    }

    if (   n2s<n1s &&    n2e>n1s)
    {
	InetAddr  rs=n2e + 1;
	InetAddr  re=n1e;
	libfwbuilder::_convert_range_to_networks(rs,re,res);
    }

    if (   n2s>n1s &&    n2e<n1e)
    {
	InetAddr  rs1=n1s;
	InetAddr  re1=n2s - 1;
	libfwbuilder::_convert_range_to_networks(rs1,re1,res);

	InetAddr  rs2=n2e + 1;
	InetAddr  re2=n1e;
	libfwbuilder::_convert_range_to_networks(rs2,re2,res);
    }

    if (   n2s>n1s &&    n2e>n1e)
    {
	InetAddr  rs=n1s;
	InetAddr  re=n2s - 1;
	libfwbuilder::_convert_range_to_networks(rs,re,res);
    }

    if (n2s>n1e /* && n2e>n1e */)
    {
	res.push_back(n1);
    }

    if (n2s<n1s && n2e>n1e)
    {
/* 
 * Do nothing since in this case network n2 is bigger than n1 and n1
 * is totally eclipsed by n2. Result is empty list.
 */
	;
    }

    return res;
}



    

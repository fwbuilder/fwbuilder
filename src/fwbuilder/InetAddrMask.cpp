/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: InetAddr.cpp 1034 2007-08-02 05:19:28Z vkurland $


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

#include <fwbuilder/InetAddr.h>
#include <fwbuilder/Interface.h>

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

void InetAddrMask::setNetworkAndBroadcastAddress()
{
    *network_address = *address & *netmask;
    *broadcast_address = *address | (~(*netmask));
}

InetAddrMask::InetAddrMask()
{
    address = new InetAddr();
    netmask = new InetNetmask();
    broadcast_address = new InetAddr();
    network_address = new InetAddr();
}

InetAddrMask::InetAddrMask(const InetAddr &a, const InetNetmask &n)
{    
    address = new InetAddr(a & n);
    netmask = new InetNetmask(n);
    broadcast_address = new InetAddr();
    network_address = new InetAddr();
    setNetworkAndBroadcastAddress();
}

InetAddrMask::InetAddrMask(const InetAddrMask& other)
{
    address = new InetAddr(*(other.address));
    netmask = new InetNetmask(*(other.netmask));
    broadcast_address = new InetAddr();
    network_address = new InetAddr();
    setNetworkAndBroadcastAddress();
}

InetAddrMask::InetAddrMask(const string &s) throw(FWException)
{
    address = new InetAddr();
    netmask = new InetNetmask();
    broadcast_address = new InetAddr();
    network_address = new InetAddr();

    if(s.find_first_not_of(".1234567890/")!=string::npos)
    {
        throw FWException(string("Invalid IP address: '")+s+"'");
    }
    
    string::size_type pos=s.find("/");
    
    if (pos==string::npos)
    {
        setAddress(InetAddr(s));
        setNetmask(InetNetmask(InetAddr::getAllOnes()));
    }
    else
    {
        setAddress(InetAddr(s.substr(0,pos)));
        string netm = s.substr(pos+1);
        
        if (netm.find(".")==string::npos)
        {
            // netmask is represented as /NN (length in bits)
            int d = atoi(netm.c_str());
            *netmask = InetNetmask(d);
        }
        else
        {
            setNetmask(InetNetmask(netm));
        }
    }
    setNetworkAndBroadcastAddress();
}

InetAddrMask::~InetAddrMask()
{
    delete address;
    delete netmask;
    delete network_address;
    delete broadcast_address;
}

void InetAddrMask::setAddress(const InetAddr &a)
{
    *address = a;
    setNetworkAndBroadcastAddress();
}

void InetAddrMask::setNetmask(const InetNetmask &nm)
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

    if (masklength==0) return 0;

    unsigned int u = 1;
    for (int i=0; i<32-masklength; ++i) u<<=1;

    return u;
}

InetAddrMask& InetAddrMask::operator=(const InetAddrMask &o)
{
    *address = *(o.address);
    *netmask = *(o.netmask);
    setNetworkAndBroadcastAddress();
    return *this;
}

bool libfwbuilder::operator==(const InetAddrMask &a, const InetAddrMask &b)
{
    return a.getNetmask() == b.getNetmask() && a.getAddress() == b.getAddress();
}

bool libfwbuilder::operator<(const InetAddrMask &a, const InetAddrMask &b)
{
    return a.getAddress() < b.getAddress();
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
	res.push_back(InetAddrMask(start, InetNetmask(InetAddr::getAllOnes())));
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
	res.push_back(InetAddrMask(start, InetNetmask(InetAddr::getAllOnes())));
	res.push_back(InetAddrMask(end, InetNetmask(InetAddr::getAllOnes())));
	return false;
    }

/* determine closest power of 2 which is less or equal to size */
    unsigned int l = size;
    int   mask_bits = 0;
    while ( l!=0 ) { l>>=1; mask_bits++; }
    mask_bits--;
    mask_bits = 32 - mask_bits;

/* mask_bits  represents number of '1'in the netmask for the new subnet */

/* test start address to see if it is a good network address for netmask */
    InetNetmask   nm1(mask_bits);  // new netmask
    InetAddrMask tn1(start, nm1);

    InetAddr nstart;
    InetAddr nend;
    InetNetmask   nnm;

    nstart = start;

    if (start!=tn1.getAddress())
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
            nnm = InetNetmask(mask_bits);
            tn1 = InetAddrMask(nstart, nnm);
        } while (start!=tn1.getAddress() and mask_bits>0);
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
            nnm = InetNetmask(mask_bits);  // new netmask
            nend = start;
            nend = nend  | (~nnm);
        } while (nend > end);
    }
/* new range starts from nstart and ends at nend */
    res.push_back( InetAddrMask(nstart, nnm) );

    if (!(nstart == start))
    {
/* there are some addresses between start and nstart */
        while ( libfwbuilder::_convert_range_to_networks(start,nstart-1,res) );
    }

    if (!(nend == end))
    {
/* the remainder of the original range is nend+1 - end */
        while ( libfwbuilder::_convert_range_to_networks(nend+1,end,res) );
    }

    return false;
}


vector<InetAddrMask> libfwbuilder::getOverlap(const InetAddrMask &n1,
                                              const InetAddrMask &n2)
{
    const InetAddr& s1 = n1.getAddress();
    const InetAddr& s2 = n2.getAddress();

    const InetNetmask&   m1 = n1.getNetmask();
    const InetNetmask&   m2 = n2.getNetmask();

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
 * TODO: comparison with InetAddr() looks ugly. May be need to
 * overload operator==(int) so we could compare with '0'
 *
 */
    if (s1==InetAddr() && m1==InetAddr()) e1=InetAddr(InetAddr::getAllOnes());
    if (s2==InetAddr() && m2==InetAddr()) e2=InetAddr(InetAddr::getAllOnes());

    vector<InetAddrMask>  res;

    InetAddr rs,re;

    if (e2<s1) return res;
    if (s2<s1 && e2>s1 && e2<e1) { rs=s1; re=e2; }
    if (s2>s1 && e2<e1)          { rs=s2; re=e2; }
    if (s2>s1 && s2<e1 && e2>e1) { rs=s2; re=e1; }
    if (s2>e1) return res;
    if (s2<s1 && e2>e1)          { rs=s1; re=e1; }
    if (s1==s2 && e1==e2)        { rs=s1; re=e1; }

/* rb and re represent resulting address range boundaries */

    libfwbuilder::_convert_range_to_networks(rs,re,res);

    return res;
}

vector<InetAddrMask> libfwbuilder::substract(const InetAddrMask &n1,
                                          const InetAddrMask &n2)
{
    InetAddr n1s = n1.getAddress();
    InetAddr n2s = n2.getAddress();

    InetNetmask   n1m = n1.getNetmask();
    InetNetmask   n2m = n2.getNetmask();

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



    

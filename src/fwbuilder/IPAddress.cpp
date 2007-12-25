/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: IPAddress.cpp 1034 2007-08-02 05:19:28Z vkurland $


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

#include <fwbuilder/IPAddress.h>
#include <fwbuilder/Interface.h>

#include <stdio.h>
#include <iostream>

#ifndef _WIN32
#  include <sys/types.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

using namespace std;


namespace libfwbuilder
{

bool operator<(const IPAddress &a, const IPAddress &b)
{
    guint32 a1=ntohl( a.to32BitInt() );
    guint32 a2=ntohl( b.to32BitInt() );
    return (a1<a2);
#if 0
    for(int i=0;i<4;i++)
        if(a.octets[i]!=b.octets[i])
            return a.octets[i]<b.octets[i];
    return false;
#endif
}

bool operator==(const IPAddress &a, const IPAddress &b)
{
    for(int i=0;i<4;i++)
        if(a.octets[i]!=b.octets[i])
            return false;
    return true;
}

bool operator==(const Netmask &a, const Netmask &b)
{
    for(int i=0;i<4;i++)
        if(a.octets[i]!=b.octets[i])
            return false;
    return true;
}

}



using namespace libfwbuilder;

IPAddress::IPAddress(const unsigned char *data, size_t len) throw(FWException)
{
    if(len!=4)
        throw FWException("Invalid IP address length. Must be 4.");
    if(!data)
        throw FWException("NULL IP address data..");
    for(int i=0;i<4;i++)
        octets[i]=data[i];
}

IPAddress::IPAddress(const struct in_addr *na) throw(FWException)
{
    /*
      struct in_addr {
      unsigned long int s_addr;
      }
      
      Note that on the i80x86 the host byte order is Least  Sig­
      nificant  Byte  first,  whereas the network byte order, as
      used on the Internet, is Most Significant Byte first.
    */

    guint32 x = ntohl(na->s_addr);
    for(int i=3;i>=0;i--)
    {
        octets[i]=x&0377;
        x>>=8;
    }
}

IPAddress::IPAddress()
{
    octets[0]=octets[1]=octets[2]=octets[3]=0;
}

IPAddress::~IPAddress()
{
}

guint32 IPAddress::to32BitInt() const
{
    guint32 x=0l;

#if defined(WORDS_LITTLEENDIAN)
    for(int i=3;i>=0;i--)
#elif defined(WORDS_BIGENDIAN)
    for(int i=0; i<4; i++)
#else
  #error "Only LITTLE_ENDIAN and BIG_ENDIAN platforms are supported"
#endif

    {
        x<<=8;
        x|=octets[i]&0377;
    }
    return x;
}

IPAddress::IPAddress(const string &s) throw(FWException, FWNotSupportedException)
{
    *this=s;
}

IPAddress& IPAddress::operator=(const string &s) throw(FWException, FWNotSupportedException)
{
    //TODO: do actual parsing according to RFC1884
    if(s.find(":")!=string::npos && s.find_first_not_of(":0123456789ABCDEFabcdef")==string::npos)
        throw FWNotSupportedException("IPv6 addresses are not supported");
    
    if(sscanf(s.c_str(), "%3u.%3u.%3u.%3u", &octets[0], &octets[1], &octets[2], &octets[3])!=4)
        throw FWException(string("Invalid IP address: '")+s+"'");
    validate();
    return *this;
}

namespace libfwbuilder
{

/* TODO: use operator ulong() when it is fixed */
IPAddress operator+(const IPAddress &a,const IPAddress &b)
{
    guint32 x=0l;
	int     i;
    for(i=0;i<4;i++) { x<<=8; x|=a.octets[i]&0377; }
    guint32 a1=htonl(x);

    x=0l;
    for(i=0;i<4;i++) { x<<=8; x|=b.octets[i]&0377; }
    guint32 a2=htonl(x);

    struct in_addr na;
    na.s_addr=a1+a2;
    return IPAddress(&na);
}

/* TODO: use operator ulong() when it is fixed */
IPAddress operator-(const IPAddress &a,const IPAddress &b)
{
    guint32 x=0l;
	int     i;
    for(i=0;i<4;i++) { x<<=8; x|=a.octets[i]&0377; }
    guint32 a1=htonl(x);

    x=0l;
    for(i=0;i<4;i++) { x<<=8; x|=b.octets[i]&0377; }
    guint32 a2=htonl(x);

    struct in_addr na;
    na.s_addr=a1-a2;
    return IPAddress(&na);
}

IPAddress operator+(const IPAddress &addr,int increment)
{
    guint32 a= ntohl( addr.to32BitInt() );
    a++;

    struct in_addr na;
    na.s_addr=htonl(a);
    return IPAddress(&na);
}

IPAddress operator-(const IPAddress &addr,int decrement)
{
    guint32 a=ntohl( addr.to32BitInt() );
    a--;

    struct in_addr na;
    na.s_addr=htonl(a);
    return IPAddress(&na);
}

}

IPAddress& IPAddress::addMask(const Netmask &nm)
{
    for (int i=0; i<4; i++) 
	octets[i] |= nm.octets[i];

    return *this;
}

void IPAddress::validate() throw(FWException)
{
    if(!octets[0] && !octets[1] && !octets[2] && !octets[3])
        return; //0.0.0.0 magic IP address.

    if((octets[0]>255) ||
       (octets[1]>255) ||
       (octets[2]>255) ||
       (octets[3]>255)) 
    {
        throw FWException(string("Invalid IP address: '")+string(*this)+"'");
    }
}

IPAddress::IPAddress(const IPAddress &o)
{
    *this=o;
}

IPAddress& IPAddress::operator=(const IPAddress &o)
{
    octets[0]=o.octets[0];
    octets[1]=o.octets[1];
    octets[2]=o.octets[2];
    octets[3]=o.octets[3];
    return *this;
}

string IPAddress::toString() const
{
    char buf[32];
    sprintf(buf, "%u.%u.%u.%u", octets[0], octets[1], octets[2], octets[3]);
    return buf;
}

bool IPAddress::isBroadcast() const
{
    return (octets[0]==octets[1] && 
            octets[1]==octets[2] && 
            octets[2]==octets[3] && 
            (octets[0]==255 || octets[0]==0)
    );
}

bool IPAddress::isMulticast() const
{
    return (octets[0]>=224 && octets[0]<=239 );
}


void Netmask::validate() throw(FWException)
{
    guint32 nm = octets[3] | (octets[2]<<8) | (octets[1]<<16) | (octets[0]<<24);
    
    if(nm) 
    {
        nm = (~nm)+1;
        
        // at this point nm must consist 
        // of exactly one '1' in binary form 
        
        while(!(nm&1))
            nm>>=1;

        if(nm!=1)
            throw FWException(string("Invalid netmask: '")+string(*this)+string("'"));
    }
}

Netmask& Netmask::operator=(const string &s) throw(FWException)
{
    if(sscanf(s.c_str(), "%3u.%3u.%3u.%3u", &octets[0], &octets[1], &octets[2], &octets[3])!=4)
        throw FWException(string("Invalid netmask: '")+s+string("'"));
    validate();
    return *this;
}

namespace libfwbuilder
{

Netmask operator~(const Netmask &nm)
{
    Netmask res;
    for (int i=0; i<4; i++)
	res.octets[i]= (~ nm.octets[i] ) & 0xff;

    return res;
}

}

Netmask::Netmask(const IPAddress &a)
{
    octets[0]=255;
    octets[1]=octets[2]=octets[3]=0;
    
    if(a[0]>127) octets[1]=255;
    if(a[0]>191) octets[2]=255;
}

Netmask::Netmask(const unsigned char *data, size_t len) throw(FWException):IPAddress(data, len) 
{
}

Netmask::Netmask()
{
    octets[0]=octets[1]=octets[2]=octets[3]=255;
}

Netmask::Netmask(const string &s) throw(FWException)
{
    *this=s;
}

Netmask::Netmask(int n)  throw(FWException)
{
    if (n<0 || n>32) throw FWException(string("Invalid netmask length"));

    octets[0]=octets[1]=octets[2]=octets[3]=0;

    guint32 i=0;
    while (n>=8) { octets[i] = 255; i++; n-=8; }
    guint32 m=128;
    while (n!=0) { octets[i] |= m; m>>=1; n--; }
}

Netmask::~Netmask()
{
}

int Netmask::getLength() const
{
    if (toString()=="255.255.255.255") return 32;

    guint32 n= ntohl( to32BitInt() );
    int   i=0;
    while (n) {
	n=n<<1;
	i++;
    }

    return i;
}

IPNetwork::IPNetwork(const IPAddress &a, const Netmask &n, int _bcast_bits)
{    
    bcast_bits = _bcast_bits;
    netmask = n;
    for(unsigned i=0;i<4;i++)
        address.octets[i]=a[i]&netmask[i];
}

const IPAddress& IPNetwork::getAddress() const
{
    return address;
}

const Netmask&   IPNetwork::getNetmask() const
{
    return netmask;
}

IPAddress IPNetwork::getBroadcastAddress () const
{
    guint32 a;
    if(bcast_bits)
        a=address.to32BitInt() | ~( netmask.to32BitInt() );     
    else 
        a=address.to32BitInt() & netmask.to32BitInt();
    struct in_addr na;
    na.s_addr=a;
    return IPAddress(&na);
/*
    struct in_addr na;
    na.s_addr=htonl(a);
    return IPAddress(&na);
*/
}

bool IPNetwork::belongs(const IPAddress &o) const
{
    for(unsigned i=0;i<4;i++) 
        if ( ( o[i] & netmask[i] ) != address[i] )
            return false;

    return true;
}

bool IPNetwork::isBroadcast() const
{
    return address.isBroadcast();
}

bool IPNetwork::isMulticast() const
{
    return address.isMulticast();
}

IPNetwork& IPNetwork::operator=(const IPNetwork &o)
{
    address=o.address;
    netmask=o.netmask;
    return *this;
}

namespace libfwbuilder
{

bool operator==(const IPNetwork &a, const IPNetwork &b)
{
    return a.getNetmask()==b.getNetmask() && a.getAddress()==b.getAddress();
}

bool operator<(const IPNetwork &a, const IPNetwork &b)
{
    return a.getAddress()<b.getAddress();
}

/* this is just a better interface to _convert_range_to_networks */
vector<IPNetwork> convertAddressRange(const IPAddress &start,
                                                    const IPAddress &end)
{
    vector<IPNetwork> res;
    IPNetwork::_convert_range_to_networks(start,end,res);
    return res;
}

}

bool IPNetwork::_convert_range_to_networks(const IPAddress &start,
					   const IPAddress &end,
					   vector<IPNetwork> &res)
{
    if (end < start) return false;
    if (start == end)
    {
	res.push_back( IPNetwork(start, Netmask()) );
	return false;
    }

    if (htonl(start.to32BitInt())==0l && htonl(end.to32BitInt())==0xffffffff)
    {
	res.push_back( IPNetwork(IPAddress("0.0.0.0"),Netmask("0.0.0.0")) );
	return false;
    }

    guint32 size = htonl(end.to32BitInt()) - htonl(start.to32BitInt()) + 1;

    if (size==2)
    {
	res.push_back( IPNetwork(start, Netmask()) );
	res.push_back( IPNetwork(end,   Netmask()) );
	return false;
    }

/* determine closest power of 2 which is less or equal to size */
    guint32 l=size;
    int   m=0;
    while ( l!=0 ) { l>>=1; m++; }
    m--;
/* m  represents number of bits in a netmask for the new subnet */

/* test start address to see if it is a good network address for netmask */
    Netmask   nm1(32-m);  // new netmask
    IPNetwork tn1(start,nm1);

    IPAddress nstart;
    IPAddress nend;
    Netmask   nnm;

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
            nstart=start;
            nnm=Netmask(32-m);  // new netmask
            nstart.addMask( ~nnm );
            nstart=nstart+1;
            nend  =nstart;
            nend.addMask( ~nnm );
            m--;
        } while (m>0 && end < nend);
    } else 
    {
        nstart=start;
        nnm=Netmask(32-m);  // new netmask
        nend  =nstart;
        nend.addMask( ~nnm );
    }
/* new range starts from nstart and ends at nend */
    res.push_back( IPNetwork(nstart,nnm) );

    if (!(nstart == start))
    {
/* there are some addresses between start and nstart */
        while ( _convert_range_to_networks(start,nstart-1,res) );
    }

    if (!(nend == end))
    {
/* the remainder of the original range is nend+1 - end */
        while ( _convert_range_to_networks(nend+1,end,res) );
    }

    return false;
}

namespace libfwbuilder
{

vector<IPNetwork> getOverlap(const IPNetwork &n1,const IPNetwork &n2)
{
    IPAddress s1 = n1.getAddress();
    IPAddress s2 = n2.getAddress();

    Netmask   m1 = n1.getNetmask();
    Netmask   m2 = n2.getNetmask();

    IPAddress e1 = s1; e1.addMask( ~m1 );
    IPAddress e2 = s2; e2.addMask( ~m2 );

/* 
 * now both networks are represented by their first and last addresses
 * 
 * we assume network 0.0.0.0/0.0.0.0 in fact represents the whole
 * possible range of ip addresses (for ip v4 it is 0.0.0.0 -
 * 255.255.255.255). Check for this condition and replace e1 or e2
 * accordingly if needed.
 *
 * TODO: comparison with IPAddress() looks ugly. May be need to
 * overload operator==(int) so we could compare with '0'
 *
 */
    if (s1==IPAddress() && m1==IPAddress()) e1=IPAddress("255.255.255.255");
    if (s2==IPAddress() && m2==IPAddress()) e2=IPAddress("255.255.255.255");

    vector<IPNetwork>  res;

    IPAddress rs,re;

    if (e2<s1) return res;
    if (s2<s1 && e2>s1 && e2<e1) { rs=s1; re=e2; }
    if (s2>s1 && e2<e1)          { rs=s2; re=e2; }
    if (s2>s1 && s2<e1 && e2>e1) { rs=s2; re=e1; }
    if (s2>e1) return res;
    if (s2<s1 && e2>e1)          { rs=s1; re=e1; }
    if (s1==s2 && e1==e2)        { rs=s1; re=e1; }

/* rb and re represent resulting address range boundaries */

    IPNetwork::_convert_range_to_networks(rs,re,res);

    return res;
}

vector<IPNetwork> substract(const IPNetwork &n1,const IPNetwork &n2)
{
    IPAddress n1s = n1.getAddress();
    IPAddress n2s = n2.getAddress();

    Netmask   n1m = n1.getNetmask();
    Netmask   n2m = n2.getNetmask();

    IPAddress n1e = n1s; n1e.addMask( ~n1m );
    IPAddress n2e = n2s; n2e.addMask( ~n2m );

/* 
 * now both networks are represented by their first and last addresses
 * 
 * we assume network 0.0.0.0/0.0.0.0 in fact represents the whole
 * possible range of ip addresses (for ip v4 it is 0.0.0.0 -
 * 255.255.255.255). Check for this condition and replace n1e or n2e
 * accordingly if needed.
 */
    if (n1s.to32BitInt()==0 && n1e.to32BitInt()==0) n1e=IPAddress("255.255.255.255");
    if (n2s.to32BitInt()==0 && n2e.to32BitInt()==0) n2e=IPAddress("255.255.255.255");

    vector<IPNetwork>  res;

    if (/* n2s<n1s && */ n2e<n1s) {
	res.push_back(n1);
    }

    if (   n2s<n1s &&    n2e>n1s) {
	IPAddress  rs=n2e + 1;
	IPAddress  re=n1e;
	IPNetwork::_convert_range_to_networks(rs,re,res);
    }

    if (   n2s>n1s &&    n2e<n1e) {
	IPAddress  rs1=n1s;
	IPAddress  re1=n2s - 1;
	IPNetwork::_convert_range_to_networks(rs1,re1,res);

	IPAddress  rs2=n2e + 1;
	IPAddress  re2=n1e;
	IPNetwork::_convert_range_to_networks(rs2,re2,res);
    }

    if (   n2s>n1s &&    n2e>n1e) {
	IPAddress  rs=n1s;
	IPAddress  re=n2s - 1;
	IPNetwork::_convert_range_to_networks(rs,re,res);
    }

    if (n2s>n1e /* && n2e>n1e */) {
	res.push_back(n1);
    }

    if (n2s<n1s && n2e>n1e) {
/* 
 * Do nothing since in this case network n2 is bigger than n1 and n1
 * is totally eclipsed by n2. Result is empty list.
 */
	;
    }

    return res;
}

}

IPRoute::IPRoute(const IPRoute &o)
{
    nm     = o.nm     ;
    dst    = o.dst    ;
    gw     = o.gw     ;
    intf   = o.intf?new Interface(*o.intf):NULL ;
    direct = o.direct ;
}

IPRoute::~IPRoute()
{
    delete intf;
}

IPRoute::IPRoute(const IPAddress &_dst, const Netmask &_nm, const IPAddress &_gw, const Interface *_intf,  bool _direct)
{
    nm     = _nm     ;
    dst    = _dst    ;
    gw     = _gw     ;
    intf   = _intf?new Interface(*_intf):NULL ;
    direct = _direct ;
}


    

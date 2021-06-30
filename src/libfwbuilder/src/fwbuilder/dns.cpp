/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

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



#ifndef _WIN32
#  include <netdb.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <errno.h>
#  include <resolv.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <arpa/nameser.h>
#  include <unistd.h>
#else
#  include <windows.h>
#endif

#include "fwbuilder/ThreadTools.h"
#include <memory>
#include <sstream>
#include <cstdlib>
#include <cstring>

#include "fwbuilder/dns.h"

using namespace std;
using namespace libfwbuilder;

#undef DEBUG_DNS

Mutex DNS::gethostbyaddr_mutex;
Mutex DNS::gethostbyname_mutex;

/*
 * gethostbyaddr and gethostbyname return pointers to a static structure.
 * Since this is not thread safe, need to use mutex to protect calls to
 * these functions.
 */
HostEnt DNS::getHostByAddr(const InetAddr &addr, int type)
{
    struct hostent *hp;
    LockGuard lock(gethostbyaddr_mutex);

    if (type==AF_INET)
    {
        hp = gethostbyaddr((const char *)addr.getV4(),
                           sizeof(struct in_addr),
                           type);
    } else
    {
        hp = gethostbyaddr((const char *)addr.getV6(),
                           sizeof(struct in_addr),
                           type);
    }

    if(hp==nullptr)
    {
        throw FWException(string("Hostname of address: '") + 
                          addr.toString() + "' not found");
    } 
    HostEnt v;
    v.name = hp->h_name;
    if (hp->h_aliases)
        for(char **p = hp->h_aliases; *p; p++) v.aliases.insert(string(*p));

    return v;
}

list<InetAddr> DNS::getHostByName(const string &name, int type)
{
    list<InetAddr> v;
    
    struct addrinfo *aiList = nullptr;
    int retVal;

#ifdef DEBUG_DNS
    cerr << "DNS::getHostByName " << name << "  type=" << type << endl;
#endif

    LockGuard lock(gethostbyname_mutex);

    if ((retVal = getaddrinfo(name.c_str(), nullptr, nullptr, &aiList)) != 0)
    {
        std::ostringstream strerr;
        strerr << "Host or network '" + name + "' not found; last error: ";
#ifdef _WIN32
        strerr << WSAGetLastError();
#else
        strerr << gai_strerror(errno);
#endif
        throw FWException(strerr.str());
    }

    struct addrinfo *ai;
    try
    {
        for (ai=aiList; ai!=nullptr; ai=ai->ai_next)
        {
#ifdef DEBUG_DNS
            cerr << "DNS::getHostByName " << name
                 << "  returned address type=" << ai->ai_family
                 << endl;
#endif

            if (ai->ai_family!=type) continue;

            switch (ai->ai_family)
            {
            case AF_INET:
            {
                struct sockaddr_in *sa = (struct sockaddr_in *) ai->ai_addr;
                InetAddr addr((struct in_addr *)(&(sa->sin_addr)));
                v.push_back(addr);
#ifdef DEBUG_DNS
                cerr << "DNS::getHostByName " << name
                     << "  ipv4 address=" << addr.toString()
                     << endl;
#endif
            }
            break;
                    
            case AF_INET6:
            {
                struct sockaddr_in6 *sa = (struct sockaddr_in6 *) ai->ai_addr;
                InetAddr addr((struct in6_addr *)(&(sa->sin6_addr)));
                v.push_back(addr);
#ifdef DEBUG_DNS
                cerr << "DNS::getHostByName " << name
                     << "  ipv6 address=" << addr.toString()
                     << endl;
#endif
            }
            break;
            }
        }
    } catch(const FWException &e)
    {
        freeaddrinfo(aiList);
        throw e;
    }

    freeaddrinfo(aiList);

    v.sort();
    v.unique();

    return v;
}



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

#include <fwbuilder/libfwbuilder-config.h>

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
#  include <winsock2.h>
#  pragma warning(disable:4101)
#endif

#include <pthread.h>

#include <memory>
#include <sstream>

#include <fwbuilder/dns.h>
#include <fwbuilder/ThreadTools.h>

using namespace std;
using namespace libfwbuilder;

#undef DNS_DEBUG

Mutex *DNS::gethostbyname_mutex = NULL;
Mutex *DNS::gethostbyaddr_mutex = NULL;

// use this function for delayed initialization
void DNS::init()
{
    if (gethostbyname_mutex==NULL) gethostbyname_mutex = new Mutex();
    if (gethostbyaddr_mutex==NULL) gethostbyaddr_mutex = new Mutex();
}

/*
 * gethostbyaddr and gethostbyname return pointers to a static structure.
 * Since this is not thread safe, need to use mutex to protect calls to
 * these functions.
 */
HostEnt DNS::getHostByAddr(const InetAddr &addr, int type) throw(FWException)
{
    DNS::init();

    struct hostent *hp;
    size_t hstbuflen = 1024; 
    char *tmphstbuf = (char *)malloc(hstbuflen);

    gethostbyaddr_mutex->lock();
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

    if(hp==NULL)
    {
        gethostbyaddr_mutex->unlock();
        free(tmphstbuf);
        throw FWException(string("Hostname of address: '") + 
                          addr.toString() + "' not found");
    } 
    HostEnt v;
    v.name = hp->h_name;
    if (hp->h_aliases)
        for(char **p = hp->h_aliases; *p; p++) v.aliases.insert(string(*p));

    free(tmphstbuf);

    gethostbyaddr_mutex->unlock();
    return v;
}

list<InetAddr> DNS::getHostByName(const string &name, int type)
    throw(FWException)
{
    DNS::init();

    list<InetAddr> v;

#ifdef _WIN32
    
    struct addrinfo aiHints;
    struct addrinfo *aiList = NULL;
    int retVal;

    memset(&aiHints, 0, sizeof(aiHints));
    aiHints.ai_family = type;
    aiHints.ai_socktype = SOCK_DGRAM;
    aiHints.ai_protocol = 0;

    if ((retVal = getaddrinfo(name.c_str(), "", &aiHints, &aiList)) != 0)
    {
        std::ostringstream strerr;
        strerr << "Host or network '" + name + "' not found; last error: ";
        strerr << WSAGetLastError();
        throw FWException(strerr.str());
    }

    struct addrinfo *ai_ptr;
    try
    {
        for (ai_ptr=aiList; ai_ptr!=NULL; ai_ptr=ai_ptr->next)
        {
            v.push_back(
                InetAddr((struct in_addr *)(&(ai_ptr->ai_addr->sa_data))));
        }
    } catch(const FWException &e)
    {
        freeaddrinfo(&aiList);
        throw;
    }

    freeaddrinfo(&aiList);

#else
    struct hostent *hp=NULL;
    char  *tmphstbuf=NULL;
    

    gethostbyname_mutex->lock();
    hp = gethostbyname2(name.c_str(), type);
    if(!hp)
    {
        gethostbyname_mutex->unlock();

        std::ostringstream strerr;
        strerr << "Host or network '"+name+"' not found; last error: ";
        strerr << strerror(errno);
        throw FWException(strerr.str());
    }

    try
    {
        for(char **p = hp->h_addr_list; *p != 0; p++) 
            v.push_back(InetAddr((struct in_addr *)(*p)));
    } catch(const FWException &e)
    {
        if(tmphstbuf) free(tmphstbuf);
        gethostbyname_mutex->unlock();
        throw;
    }

    if(tmphstbuf) free(tmphstbuf);
    gethostbyname_mutex->unlock();

    v.sort();
    return v;

#endif
}



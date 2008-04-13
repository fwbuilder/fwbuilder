/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

  $Id: dns.cpp 1034 2007-08-02 05:19:28Z vkurland $


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
#ifdef HAVE_LWRES
#  include <lwres/lwres.h>
#  include <lwres/netdb.h>
#else
#  include <netdb.h>
#endif
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

const size_t DNS::RSP_BUF_SIZE=64*1024;

Mutex *DNS::gethostbyname_mutex;
Mutex *DNS::gethostbyaddr_mutex;


namespace libfwbuilder
{

void init_dns() throw(FWException)
{
    DNS::init();

#ifdef HAVE_LWRES_GETIPNODE
    struct hostent *hp;
    int    herr;
    int    ret;
    char   addrbuf[16];

    ret = inet_pton(AF_INET, "127.0.0.1", addrbuf);

    hp = lwres_getipnodebyaddr(addrbuf, 4, AF_INET, &herr);
    if (hp==NULL && herr==NO_RECOVERY)
        throw FWException(
"Lightweight resolver library test failed; perhaps lwresd is not running.\nDNS queries won't work.");

#endif
}

void* DNS_bulkBackResolve_Thread(void *args)
{
    std::ostringstream str;
    void **void_pair=(void**)args;
    DNS_bulkBackResolve_query *p      = static_cast<DNS_bulkBackResolve_query*>(void_pair[0]);
    Logger                    *logger = static_cast<Logger *>(void_pair[1]);
    SyncFlag            *stop_program = static_cast<SyncFlag*>(void_pair[2]);
    try 
    {
        while( true )
        {
            stop_program->lock();
            if ( stop_program->peek() )
            {
                stop_program->unlock();
                return NULL;
            }
            stop_program->unlock();

            p->queue_mutex.lock();
            if(p->ips.empty())
            {
                p->queue_mutex.unlock();
                break;
            }
            InetAddr j=p->ips.front();
            p->ips.pop();
            p->queue_mutex.unlock();
        
	    str << "Trying " << j.toString() 
                << ";   " << int(p->ips.size()) << " more in the queue\n";
            *logger << str;

            try
            {

                HostEnt he=DNS::getHostByAddr(j, p->retries, p->timeout);
                std::ostringstream str;
                str << "Resolved  " << j.toString() << ": " << he.name << "\n";
                *logger << str;
                p->result_mutex.lock();
                p->result[j]=he;
                p->result_mutex.unlock();
            } catch(FWException &ex)
            {
                std::ostringstream str;
                str << "Could not resolve address " << j.toString() 
		    << " to a host name, will use generic name\n";
                *logger << str;
                p->failed_mutex.lock();
                p->failed.insert(j);
                p->failed_mutex.unlock();
            }
        }
    } 
    catch (FWException &ex)
    {
        // do nothing, just break the "while" loop
    }

//#ifdef DNS_DEBUG
    str << " Worker thread exiting. " << p->running_count << " threads left.\n";
    *logger << str;
//#endif

    p->running_mutex.lock   ();
    p->running_count--;
    p->running_cond.signal  ();
    p->running_mutex.unlock ();

//#ifdef DNS_DEBUG
    str << " Worker thread terminated.\n";
    *logger << str;
//#endif

    return NULL;
}

}

void DNS::init()
{
    gethostbyname_mutex = new Mutex();
    gethostbyaddr_mutex = new Mutex();
}

DNS::DNS()
{
}

string DNS::getErrorMessage(int rcode)
{
#if !defined(HAVE_GOODLIBRESOLV)
    throw FWException("This feature is not implemented on your OS.");
#else
    map<int, string> error_messages;
    
    // Define some common error codes
    error_messages[ns_r_formerr]  = "Format error"        ;
    error_messages[ns_r_servfail] = "Server failed"       ;
    error_messages[ns_r_nxdomain] = "No such domain name" ;
    error_messages[ns_r_notimpl]  = "Not implemented"     ;
    error_messages[ns_r_refused]  = "Refused"             ;
    error_messages[ns_r_yxdomain] = "Domain name exists"  ;
    error_messages[ns_r_yxrrset]  = "Rrset exists"        ;
    error_messages[ns_r_nxrrset]  = "Rrset doesn't exist" ;
    error_messages[ns_r_notauth]  = "Not authoritative"   ;
    error_messages[ns_r_notzone]  = "Not in zone"         ;
    error_messages[ns_r_badsig]   = "Bad signature"       ;
    error_messages[ns_r_badkey]   = "Bad key"             ;
    error_messages[ns_r_badtime]  = "Bad time"            ;

    if(error_messages.count(rcode))
        return error_messages[rcode];
    else
    {
        char buf[80];
        sprintf(buf,"DNS Error '%d'", rcode);
        return buf;
    }
#endif
}

HostEnt DNS::getHostByAddr(const InetAddr &addr, int retries_, int timeout_) throw(FWException)
{
#if !defined(HAVE_GOODLIBRESOLV)
#ifndef _WIN32
# warning "getHostByAddr will use default system timeout because bind library was not found on the system "
#endif
    return getHostByAddr(addr);
#else
    struct __res_state res;

    if(res_ninit(&res)==-1)
        throw FWException("Error initializing resolver library");

    res.retrans = timeout_;
    res.retry   = retries_;

    char host[NS_MAXDNAME];
    sprintf(host, "%u.%u.%u.%u.IN-ADDR.ARPA.", addr[3], addr[2], addr[1], addr[0]);

    u_char buf[PACKETSZ];
    int n = res_nmkquery(&res, ns_o_query, host, ns_c_in, ns_t_ptr, NULL, 0, NULL,
                         buf, sizeof buf);
    if (n < 0) 
        throw FWException(string("Resoving ")+host+" failed in res_nmkquery");
    
    auto_ptr<unsigned char> answer(new unsigned char[RSP_BUF_SIZE]);
    int len = res_nsend(&res, buf, n, answer.get(), RSP_BUF_SIZE);
    if (len < 0) 
        throw FWException(string("Resoving ")+host+" failed in res_nsend");
    
    // Let's decode answer

    if(len<0)
        throw FWException("Error returned while quering domain NS records");

    // Rsp. buffer
    HostEnt v;

    ns_msg handle;
    
    if(ns_initparse(answer.get(), len, &handle) < 0) 
        throw FWException("Zone parse error in initparse");
    
    if(ns_msg_getflag(handle, ns_f_rcode) != ns_r_noerror)
        throw FWException(getErrorMessage(ns_msg_getflag(handle, ns_f_rcode)));
    
    if(ns_msg_count(handle, ns_s_an) == 0)
        throw FWException("Answer contains to records");
    
    while(true) 
    {        
        ns_rr rr;
        if(ns_parserr(&handle, ns_s_an, -1, &rr))
        {
            if(errno != ENODEV) 
                throw FWException("NS query response parse error in parserr");
            else
                break;
        }
        
        if(ns_rr_type(rr)==ns_t_ptr && ns_rr_class(rr)==ns_c_in)
        {
            char dn[NS_MAXDNAME];
            if(dn_expand(answer.get(), answer.get() + len, ns_rr_rdata(rr), dn, sizeof(dn))<0)
                throw FWException("A record parse error in parserr");
            if(v.name.empty())
                v.name=dn;
            else
                v.aliases.insert(dn);
        } 
    }
    return v;
#endif
}


HostEnt DNS::getHostByAddr(const InetAddr &addr) throw(FWException)
{
    struct hostent hostbuf;
    struct hostent *hp;
    int herr;
    
    size_t hstbuflen = 1024; 
    char *tmphstbuf = (char *)malloc(hstbuflen);

    struct in_addr naddr;
    inet_aton(addr.toString().c_str(), &naddr);


#ifdef HAVE_LWRES_GETIPNODE
    struct hostent *res;
    char   addrbuf[16];
    int    ret=inet_pton(AF_INET, addr.toString().c_str(), addrbuf);
    hp = res = lwres_getipnodebyaddr(addrbuf, 4, AF_INET, &herr);
    if (res!=NULL) herr=0;

#elif HAVE_GETHOSTBYADDR_R_8
    int res;
# ifdef GETHOSTBYADDR_FIRST_ARG_CHARPTR
    while((res = gethostbyaddr_r((const char *)&naddr, sizeof(naddr),
# else
    while((res = gethostbyaddr_r(&naddr, sizeof(naddr),
# endif
                                 AF_INET,
                                 &hostbuf,
                                 tmphstbuf, hstbuflen,
                                 &hp,
                                 &herr)
          ) && (herr == ERANGE))
    {
        hstbuflen *= 2;
        tmphstbuf = (char *)realloc(tmphstbuf, hstbuflen);
    }
#elif HAVE_GETHOSTBYADDR_R_7
    struct hostent *res;
    while((hp = res = gethostbyaddr_r((const char *)&naddr, sizeof(naddr),
                                 AF_INET,
                                 &hostbuf,
                                 tmphstbuf, hstbuflen,
                                 &herr)
          ) && (herr == ERANGE))
    {
        hstbuflen *= 2;
        tmphstbuf = (char *)realloc(tmphstbuf, hstbuflen);
    }
#else
#ifndef _WIN32
# warning "No gethostbyaddr_r(); all DNS calls will be serialized (slow!)"
#endif
    struct hostent *res;

    gethostbyaddr_mutex->lock();
    hp = res = gethostbyaddr((const char *)&naddr, 
                             sizeof(naddr), 
                             AF_INET
    );
    herr = 0;

#endif

    if(!res || herr)
    {
#if !defined(HAVE_LWRES_GETIPNODE) && !defined(HAVE_GETHOSTBYADDR_R_8) && !defined(HAVE_GETHOSTBYADDR_R_7)
        gethostbyaddr_mutex->unlock();
#endif
        free(tmphstbuf);

        throw FWException(string("Hostname of address: '")+InetAddr(&naddr).toString()+"' not found");
    } 
    
    HostEnt v;
    v.name=hp->h_name;
    if(hp->h_aliases)
        for(char **p = hp->h_aliases; *p; p++) 
            v.aliases.insert(string(*p));

    free(tmphstbuf);
#if !defined(HAVE_LWRES_GETIPNODE) && !defined(HAVE_GETHOSTBYADDR_R_8) && !defined(HAVE_GETHOSTBYADDR_R_7)
        gethostbyaddr_mutex->unlock();
#endif
    return v;
}

list<InetAddr> DNS::getHostByName(const string &name) throw(FWException)
{
    struct hostent hostbuf;
    struct hostent *hp=NULL;
    char  *tmphstbuf=NULL;
    size_t hstbuflen = 1024;
    int    herr;

#ifdef HAVE_LWRES_GETIPNODE

    hp=lwres_getipnodebyname(name.c_str(), AF_INET, AI_V4MAPPED + AF_INET6, &herr);
    if(!hp)
    {
        throw FWException("Host or network '"+name+"' not found.\nResolver returned error \""+
                          lwres_hstrerror(herr)+"\"");
    }

#elif HAVE_FUNC_GETHOSTBYNAME_R_6

    tmphstbuf = (char *)malloc(hstbuflen);
    int res;
    while((res = gethostbyname_r(name.c_str(), &hostbuf,tmphstbuf,hstbuflen,&hp,&herr))
          && (herr == ERANGE))
    {
        hstbuflen *= 2;
        tmphstbuf = (char *)realloc(tmphstbuf, hstbuflen);
    }
    if(res || !hp)
    {
        free(tmphstbuf);
        throw FWException("Host or network '"+name+"' not found");
    } 
#elif HAVE_FUNC_GETHOSTBYNAME_R_5

    tmphstbuf = (char *)malloc(hstbuflen);
    while(!(hp = gethostbyname_r(name.c_str(), &hostbuf, tmphstbuf, hstbuflen, &herr))
          && (herr == ERANGE))
    {
        hstbuflen *= 2;
        tmphstbuf = (char *)realloc(tmphstbuf, hstbuflen);
    }

    if(!hp)
    {
        free(tmphstbuf);
        throw FWException("Host or network '"+name+"' not found");
    } 
#else
#ifndef _WIN32
# warning "No gethostbyname_r() all DNS calls will be serialized (slow!)"
#endif
    
    tmphstbuf = NULL;
    
    gethostbyname_mutex->lock();
    hp = gethostbyname(name.c_str());
    if(!hp)
    {
        gethostbyname_mutex->unlock();

        std::ostringstream strerr;
        strerr << "Host or network '"+name+"' not found; last error: ";
#ifndef _WIN32
        strerr << strerror(errno);
#else
        strerr << WSAGetLastError();
#endif
        throw FWException(strerr.str());
    }

#endif
    
    list<InetAddr> v;
    try
    {
        for(char **p = hp->h_addr_list; *p != 0; p++) 
            v.push_back(InetAddr((struct in_addr *)(*p)));
    } catch(const FWException &e)
    {
        if(tmphstbuf)
            free(tmphstbuf);

#ifdef HAVE_LWRES_GETIPNODE
        lwres_freehostent(hp);
#endif
#if !defined( HAVE_LWRES_GETIPNODE ) && !defined( HAVE_FUNC_GETHOSTBYNAME_R_6) && !defined(HAVE_FUNC_GETHOSTBYNAME_R_5)
        gethostbyname_mutex->unlock();
#endif
        throw;
    }
    if(tmphstbuf)
       free(tmphstbuf);

#ifdef HAVE_LWRES_GETIPNODE
        lwres_freehostent(hp);
#endif
#if !defined( HAVE_LWRES_GETIPNODE ) && !defined( HAVE_FUNC_GETHOSTBYNAME_R_6) && !defined(HAVE_FUNC_GETHOSTBYNAME_R_5)
    gethostbyname_mutex->unlock();
#endif

    v.sort();

    return v;
}

multimap<string, InetAddr> DNS::getNS(const string &domain, Logger *logger,SyncFlag *stop_program, int retries_, int timeout_) throw(FWException)
{
    std::ostringstream str;

#if !defined(HAVE_GOODLIBRESOLV)
    throw FWException("This feature is not implemented on your OS.");
#else
    struct __res_state res;
    
    if(res_ninit(&res)==-1)
        throw FWException("Error initializing resolver library");

    res.retrans = timeout_;
    res.retry   = retries_;

    CHECK_STOP_AND_THROW_EXCEPTION

    auto_ptr<unsigned char> answer(new unsigned char[RSP_BUF_SIZE]);
    
    str << "Requesting list of name servers for domain '" << domain << "'"  << '\n';
    *logger << str;

    int  len = res_nquery(&res, 
                          domain.c_str(), 
                          ns_c_in,
                          ns_t_ns, 
                          answer.get(), 
                          RSP_BUF_SIZE);

    CHECK_STOP_AND_THROW_EXCEPTION
    
    if(len<0)
        throw FWException("Error returned while quering domain NS records");

    // Rsp. buffer
    multimap<string, InetAddr> v;

    ns_msg handle;
    
    if(ns_initparse(answer.get(), len, &handle) < 0) 
        throw FWException("Zone parse error in initparse");
    
    CHECK_STOP_AND_THROW_EXCEPTION

    if(ns_msg_getflag(handle, ns_f_rcode) != ns_r_noerror)
        throw FWException(getErrorMessage(ns_msg_getflag(handle, ns_f_rcode)));
    
    CHECK_STOP_AND_THROW_EXCEPTION

    if(ns_msg_count(handle, ns_s_an) == 0)
        throw FWException("Answer contains to records");
    
    while(true) 
    {        
        CHECK_STOP_AND_THROW_EXCEPTION
        ns_rr rr;
        if(ns_parserr(&handle, ns_s_an, -1, &rr))
        {
            if(errno != ENODEV) 
                throw FWException("NS query response parse error in parserr");
            else
                break;
        }
        
        CHECK_STOP_AND_THROW_EXCEPTION
        if(ns_rr_type(rr)==ns_t_ns && ns_rr_class(rr)==ns_c_in)
        {
            char dn[NS_MAXDNAME];
            if(dn_expand(answer.get(), answer.get() + len, ns_rr_rdata(rr), dn, sizeof(dn))<0)
                throw FWException("A record parse error in parserr");
            CHECK_STOP_AND_THROW_EXCEPTION
            list<InetAddr> a=DNS::getHostByName(dn);
            CHECK_STOP_AND_THROW_EXCEPTION
            for(list<InetAddr>::iterator i=a.begin();i!=a.end();++i)
                v.insert(pair<string, InetAddr>(string(dn), (*i)));
        } 
    }
    
    str << "Succesfuly found " << (int)v.size() << " name servers."  << '\n';
    *logger << str;

    return v;
#endif
}

/**
 * 'Retries' applicable only to UPD part of the query (if any).
 * TCP connection to transfer zone established and attempted 
 * only once.
 */
map<string, set<InetAddr> > DNS::findA(const string &domain, Logger *logger,SyncFlag *stop_program, int retries_, int timeout_) throw(FWException)
{
    std::ostringstream str;
#if !defined(HAVE_GOODLIBRESOLV)
    throw FWException("This feature is not implemented on your OS.");
#else
    TimeoutCounter timeout(timeout_, "Getting A records");

    *logger << "Looking for authoritative servers" << '\n';
    
    multimap<string, InetAddr> ns=DNS::getNS(domain, logger,stop_program);
    if(!ns.size())
        throw FWException("No NS records found");

    CHECK_STOP_AND_THROW_EXCEPTION
    timeout.check();

    FWException *last_err=NULL; // to avoid compiler warning
    for(multimap<string, InetAddr>::iterator nsi=ns.begin(); nsi!=ns.end(); ++nsi)
    {
        try
        {
            return findA(domain, (*nsi).second, logger,stop_program, retries_, timeout.timeLeft());
            timeout.check();
        } catch(FWException &ex)
        {
            str << "Quering NS " << (*nsi).second.toString() 
                << " ( " << (*nsi).first << " ) " 
                << " failed. with error: '" << ex.toString() << "'" << '\n' 
                << " Cycling to next one" << '\n';
            *logger << str;
            last_err = new FWException(ex) ;
        }
    }
    
    *logger << "No more servers to ask. Query failed." << '\n';
    
    // We get here if we cycled through all name servers
    // with no luck.
    // Throw last exception.
    throw *last_err;
#endif
}

/**
 * 'Retries' applicable only to UDP part of the query (if any).
 * TCP connection to transfer zone established and attempted 
 * only once.
 */
map<string, set<InetAddr> > DNS::findA(const string &domain, const InetAddr &ns, Logger *logger,SyncFlag *stop_program, int retries_, int timeout_) throw(FWException)
{
    std::ostringstream str;
#if !defined(HAVE_GOODLIBRESOLV)
    throw FWException("This feature is not implemented on your OS.");
#else
    TimeoutCounter timeout(timeout_, "Getting A records");

    str << "Querying server: " << ns.toString() << '\n';
    *logger << str;

    struct __res_state res;
    
    if(res_ninit(&res)==-1)
        throw FWException("Error initializing resolver library");

    res.retrans = timeout_;
    res.retry   = retries_;
    
    struct in_addr nsaddress;
    inet_aton(ns.toString().c_str(), &nsaddress);
    res.nsaddr.sin_addr = nsaddress;
    
    res.nscount = 1;
    res.nsaddr_list[0].sin_family = AF_INET;
    res.nsaddr_list[0].sin_port   = htons(NAMESERVER_PORT);
    res.nsaddr_list[0].sin_addr   = nsaddress;

    CHECK_STOP_AND_THROW_EXCEPTION
    timeout.check();

    // Create a query packet for the requested zone name.

    u_char buf[PACKETSZ];
    int msglen = res_nmkquery(&res, ns_o_query, domain.c_str(),
                              ns_c_in, ns_t_axfr, NULL,
                              0, 0, buf, sizeof buf);
    
    CHECK_STOP_AND_THROW_EXCEPTION
    timeout.check();

    if(msglen<0)
        throw FWException("Error returned while creating DNS query");
    
    const struct sockaddr_in *sin=&res.nsaddr_list[0];
    int sockFD;
    
    if((sockFD = socket(sin->sin_family, SOCK_STREAM, 0)) < 0) 
        throw FWException("Error creating DNS socket");

    if (connect(sockFD, (struct sockaddr *)sin, sizeof *sin) < 0) 
        throw FWException("Error connecting to DNS server");
    
    CHECK_STOP_AND_THROW_EXCEPTION
    timeout.check();

    // Send length & message for zone transfer
    
    u_char tmp[NS_INT16SZ];
    ns_put16(msglen, tmp);
    
    map<string, set<InetAddr> > v;
            
    try
    {
        CHECK_STOP_AND_THROW_EXCEPTION
	timeout.check();

        if(write(sockFD, tmp, NS_INT16SZ) != NS_INT16SZ)
            throw FWException("Error 01 sending AXFR query to DNS server");
        CHECK_STOP_AND_THROW_EXCEPTION
	timeout.check();

        *logger << "Sending Query" << '\n';
        if(write(sockFD, buf, msglen) != msglen) 
            throw FWException("Error 02 sending AXFR query to DNS server");
    
        bool firstsoa = true  ;
        bool done     = false ;
        string soa;

        *logger << "Reading response." << '\n';
        while(!done)
        {
            CHECK_STOP_AND_THROW_EXCEPTION
	    timeout.check();

            // Read the length of the response.
            u_char *cp = tmp;
            size_t amtToRead = INT16SZ;
            while(amtToRead > 0)
            {
                CHECK_STOP_AND_THROW_EXCEPTION
		timeout.check();

		int numRead=timeout.read(sockFD, cp, amtToRead);

                if(numRead <= 0) 
                    throw FWException("Error reading AXFR response length");
                cp        += numRead;
                amtToRead -= numRead;
            }
        
            int len = amtToRead = ns_get16(tmp);
            if(len == 0)
                break;
            else if(len<0)
                throw FWException("Ivalid packet size returned");

            CHECK_STOP_AND_THROW_EXCEPTION
	    timeout.check();

            try
            {
                cp=new unsigned char[len];
            } catch(bad_alloc)
            {
                throw FWException("Response packet too big");
            }
            auto_ptr<unsigned char> answer(cp);
    
            // Read the response.
            cp = answer.get();
            while(amtToRead > 0)
            {
                CHECK_STOP_AND_THROW_EXCEPTION
		timeout.check();

		int numRead=timeout.read(sockFD, cp, amtToRead);

                if(numRead <= 0)
                    throw FWException("Error reading AXFR response");
                cp        += numRead;
                amtToRead -= numRead;
            }

            // Read next answer.
            // Let's parse it

            ns_msg handle;
                    
            CHECK_STOP_AND_THROW_EXCEPTION
	    timeout.check();

            if(ns_initparse(answer.get(), len, &handle) < 0) 
                throw FWException("Zone parse error in initparse");
                    
            if(ns_msg_getflag(handle, ns_f_rcode) != ns_r_noerror)
                throw FWException(getErrorMessage(ns_msg_getflag(handle, ns_f_rcode)));
                    
            if(ns_msg_count(handle, ns_s_an) == 0)
                throw FWException("Answer contains to records");
                    
            while(true) 
            {
                CHECK_STOP_AND_THROW_EXCEPTION
		timeout.check();

                ns_rr rr;
                if(ns_parserr(&handle, ns_s_an, -1, &rr))
                {
                    if(errno != ENODEV) 
                        throw FWException("Zone parse error in parserr");
                    else
                        break;
                }
                        
                CHECK_STOP_AND_THROW_EXCEPTION
		timeout.check();

                if(ns_rr_type(rr)==ns_t_a && ns_rr_class(rr)==ns_c_in)
                {
                    if(ns_rr_rdlen(rr) != NS_INADDRSZ)
                        throw FWException("Invalid address length in A record");
                    
                    if(v.find(ns_rr_name(rr))==v.end())
                        v[ns_rr_name(rr)]=set<InetAddr>();
                    
                    v[ns_rr_name(rr)].insert(InetAddr((const struct in_addr *)ns_rr_rdata(rr)));
                    
                } else if(ns_rr_type(rr)==ns_t_soa)
                {
                    if(firstsoa)
                    {
                        firstsoa = false;
                        soa=ns_rr_name(rr);
                    } else
                    {
                        if(ns_samename(soa.c_str(), ns_rr_name(rr)) == 1)
                        {
                            done=true;
                            break;
                        }
                    }
                }
            }
        }
        (void) close(sockFD);
        
    } catch(FWException &ex)
    {
        (void) close(sockFD);
        throw;
    }
    str << "Succesfuly found " << (int)v.size() << " hosts."  << '\n';
    *logger << str;
    return v;
#endif
}

DNS_getNS_query::DNS_getNS_query(const string &domain_, int retries_, int timeout_)
{
    domain  = domain_  ;
    retries = retries_ ;
    timeout = timeout_ ;
}

void DNS_getNS_query::run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException)
{
    try
    {
        result = DNS::getNS(domain, logger,stop_program, retries, timeout);
    } catch (FWException &ex)
    {
    }
}

DNS_findA_query::DNS_findA_query()
{
}

DNS_findA_query::DNS_findA_query(const string &domain_, const InetAddr &ns_, int retries_, int timeout_)
{
        domain  = domain_  ;
        retries = retries_ ;
        timeout = timeout_ ;
        ns      = ns_      ;
}

void DNS_findA_query::init(const string &domain_, const InetAddr &ns_, int retries_, int timeout_)
{
        domain  = domain_  ;
        retries = retries_ ;
        timeout = timeout_ ;
        ns      = ns_      ;
}

void DNS_findA_query::run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException)
{
    try
    {
        result = findA(domain, ns, logger,stop_program, retries, timeout);
    } catch (FWException &ex)
    {
    }
}

DNS_bulkBackResolve_query::DNS_bulkBackResolve_query(set<InetAddr> _ips, 
                                                     unsigned int _nthreads,
						     int _retries,
                                                     int _timeout)
{
    for(set<InetAddr>::iterator j = _ips.begin(); j!=_ips.end(); ++j)
        ips.push(*j);

    retries  = _retries  ;
    timeout  = _timeout  ;
    nthreads = _nthreads ;

    pthread_attr_init(&tattr);
}

DNS_bulkBackResolve_query::~DNS_bulkBackResolve_query()
{
    pthread_attr_destroy(&tattr);
}

void DNS_bulkBackResolve_query::run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException)
{
    queue_mutex.lock();

    running_count = 0;
    for(unsigned int i=0;i<nthreads;i++)
    {
        void **void_pair = new void*[3];
        void_pair[0]     = this;
        void_pair[1]     = logger;
        void_pair[2]     = stop_program;

#ifdef DNS_DEBUG
	*logger << "  Creating worker thread\n";
#endif
        pthread_t tid;
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
        running_count++;
        pthread_create(&tid, NULL, DNS_bulkBackResolve_Thread, void_pair);
    }

#ifdef DNS_DEBUG
    std::ostringstream str;
    str << "  Created " << running_count << " worker threads" << "\n";
    *logger << str;
#endif

    queue_mutex.unlock();

/*
 *  thread itself decreases running_count just before it calls signal
 *  on running_cond and thus wakes up main process in this loop. This
 *  is done this way because of the situation when we have created
 *  more threads than we have IPs to be resolved. In this case some
 *  threads would have finished and exited _before_ this loop even
 *  started. Threads should decrease the counter themselves so that
 *  loop would wait only for those threads that worked when the loop
 *  started.
 *
 *  Note that mutex is unlocked before falling asleep in wait and then
 *  locked again when thread is woken up. This ensures that we always
 *  check running_count when mutex is locked. Threads use the same
 *  mutex when modify running_count.
 *
 *  Note 01/20/06:
 *
 *   The comment above is correct, but the code did not follow described
 *   model.
 *   method Cond::wait locks the mutex and calls pthread_cond_wait
 *   This is because pthread_cond_wait atomically unlocks the mutex
 *   and puts the thread into sleep mode waiting for for the condition
 *   variable to be signaled. Also once signal is caught, before
 *   returning to the calling thread, pthread_cond_wait locks the  mutex
 *   again. Since we do nothing that would require this mutex here (we only
 *   use this mechanism to make sure we get control back when all
 *   worker threads terminate), we just unlock it and proceed with the loop.
 */
    while(1)
    {
        CHECK_STOP_AND_RETURN

        running_mutex.lock();
        if (running_count<=0)
        {
            running_mutex.unlock();
            break;
        }
        running_mutex.unlock();

        running_cond.wait(running_mutex);
        running_mutex.unlock();
    }

    CHECK_STOP_AND_RETURN

    if(!failed.empty())
        throw FWException("Some of IPs did not resolve");
}


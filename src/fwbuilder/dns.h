/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

  $Id: dns.h 953 2006-05-17 00:20:37Z vkurland $


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


#ifndef __DNS_HH_FLAG__
#define __DNS_HH_FLAG__

#include <fwbuilder/libfwbuilder-config.h>

#ifndef _WIN32

/* do not change the order of these headers! resolv.h should be included
 * before netdb.h, otherwise there seems to be a problem with macro __P
 * which is defined in cdfes.h as '#define __P(args) args __THROW' and
 * gcc 3.3 does not like this */

#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/nameser.h>
#  include <resolv.h>
#  include <netdb.h>

#else

#  ifndef RES_TIMEOUT
#     define RES_TIMEOUT 1
#  endif

#endif // _WIN32

#include <fwbuilder/IPAddress.h>
#include <fwbuilder/FWException.h>
#include <fwbuilder/BackgroundOp.h>
#include <fwbuilder/ThreadTools.h>

#include <vector>
#include <map>
#include <queue>

#ifndef RES_DFLRETRY 
#  define RES_DFLRETRY 1
#endif

namespace libfwbuilder
{

    void init_dns() throw(FWException);


class HostEnt
{
    public:
    
    std::string           name    ;
    std::set<std::string> aliases ;
};

/**
 * This is abstract class
 */
class DNS : public BackgroundOp
{
    public:

    /** 
     * Should be called exactly once before this class
     * ever used.
     */
    static void init();
    
    DNS();

    /**
     * Finds IP adddresses of the host with given host name.
     * This operation does not run in backgound.
     * Returned list is sorted.
     */
    static std::list<IPAddress> getHostByName(const std::string &name) throw(FWException);

    /**
     * Find all host names of host with given IP.
     * This operation does not run in backgound.
     */
    static HostEnt getHostByAddr(const IPAddress &addr) throw(FWException);

    /**
     * Return name of host with given IP.
     */
    static HostEnt getHostByAddr(const IPAddress &addr, int retries_, int timeout_) throw(FWException);

    /**
     * Finds NS records for given domain
     */
    std::multimap<std::string, IPAddress> getNS(const std::string &domain, Logger *logger,SyncFlag *stop_program, int retries_=RES_DFLRETRY, int timeout_=RES_TIMEOUT) throw(FWException);

    /**
     * Attempts to fetch zone for given domain for
     * it's name server. If succeeded, finds all 'A'
     * records in it and returs hostname/IPAddress pairs.
     */
    std::map<std::string, std::set<IPAddress> > findA(const std::string &domain, Logger *logger,SyncFlag *stop_program,  int retries_=RES_DFLRETRY, int timeout_=RES_TIMEOUT) throw(FWException);

    /**
     * Attempts to fetch zone for given domain for
     * specific server. If succeeded, finds all 'A'
     * records in it and returs hostname/IPAddress pairs.
     */
    std::map<std::string, std::set<IPAddress> > findA(const std::string &domain, const IPAddress &ns, Logger *logger,SyncFlag *stop_program,  int retries_=RES_DFLRETRY, int timeout_=RES_TIMEOUT) throw(FWException);

    protected:

    static std::string getErrorMessage(int rcode);
    private:

    static const size_t RSP_BUF_SIZE;

    static Mutex *gethostbyname_mutex;
    static Mutex *gethostbyaddr_mutex;
    
};

class DNS_findA_query : public DNS
{
    public:

    explicit DNS_findA_query();

    DNS_findA_query(const std::string &domain_, const IPAddress &ns_, int retries_=RES_DFLRETRY, int timeout_=RES_TIMEOUT);

    void init(const std::string &domain_, const IPAddress &ns_, int retries_=RES_DFLRETRY, int timeout_=RES_TIMEOUT);
    
    virtual void run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException);
    
    std::map<std::string, std::set<IPAddress> > getResult() { return result; }

    private:

    std::map<std::string, std::set<IPAddress> > result;    
    std::string domain;
    int retries;
    int timeout;
    IPAddress ns;
};

class DNS_getNS_query : public DNS
{
    public:

    DNS_getNS_query(const std::string &domain_, int retries_=RES_DFLRETRY, int timeout_=RES_TIMEOUT);
    
    virtual void run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException);
    
    std::multimap<std::string, IPAddress> getResult() { return result; }
    
    private:
    
    std::multimap<std::string, IPAddress> result;    
    std::string domain;
    int retries;
    int timeout;
};

class DNS_bulkBackResolve_query : public DNS
{
    friend void *DNS_bulkBackResolve_Thread(void *);

    public:
    
    DNS_bulkBackResolve_query(std::set<IPAddress>, unsigned int nthreads,  
			      int retries_=RES_DFLRETRY,
			      int timeout_=RES_TIMEOUT);
    virtual ~DNS_bulkBackResolve_query();
    
    /**
     * Resolves all hosts in list. If resolving at least of one
     * of them failed - exception is thrown.
     * Even when exception is thrown, successfully resolved hosts
     * are returned.
     */
    virtual void run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException);
    
    std::map<IPAddress, HostEnt> getResult() { return result; }
    std::set<IPAddress> getFailed() { return failed; }
    pthread_attr_t tattr;
    
    protected:
    
    std::map<IPAddress, HostEnt> result;
    Mutex failed_mutex;
    
    std::set<IPAddress> failed;
    Mutex result_mutex;
    
    std::queue<IPAddress> ips;
    Mutex queue_mutex;

    int retries;
    int timeout;
    
    private:

    Mutex running_mutex         ;
    Cond  running_cond          ;
    unsigned int running_count ;
    
    unsigned int nthreads;
};

}

#endif




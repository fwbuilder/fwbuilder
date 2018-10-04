/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

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


#ifndef __SNMP_HH_FLAG__
#define __SNMP_HH_FLAG__


#ifdef HAVE_LIBSNMP

#include <vector>

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/BackgroundOp.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/InterfaceData.h"
#include "fwbuilder/IPRoute.h"
#include "fwbuilder/dns.h"

#ifdef UCD_SNMP
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/snmp_api.h>
#endif

#ifdef NET_SNMP
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#endif

#include <atomic>

namespace libfwbuilder
{

class SNMPConnection;

class SNMPVariable
{
    friend class SNMPConnection;

    public:
    
    virtual ~SNMPVariable() {}
    enum 
    {
        snmp_int       = ASN_INTEGER,
        snmp_string    = ASN_OCTET_STR,
        snmp_bits      = ASN_BIT_STR,
        snmp_counter64 = ASN_APP_COUNTER64,
        snmp_oid       = ASN_OBJECT_ID,
        snmp_ipaddr    = ASN_IPADDRESS
    } type;

    /**
     * Returns std::string representation of this variable
     */
    virtual std::string toString() = 0;

    static std::string varList2String(std::vector<SNMPVariable*> &v);
    static long varList2Int(std::vector<SNMPVariable*> &v);
    static long var2Int(SNMPVariable *var);
    static void freeVarList(std::vector<SNMPVariable*> &v);

    protected:   

    static SNMPVariable *create(struct variable_list *v);
};

class SNMPVariable_Int : public SNMPVariable
{
    friend class SNMPVariable;

    public:
    
    virtual std::string toString();
    long getIntValue() { return value; }
    
    protected:
    
    SNMPVariable_Int(long v) { type=snmp_int; value=v; }
    long value;
};

class SNMPVariable_Bits : public SNMPVariable
{
    friend class SNMPVariable;

    public:
    
    virtual ~SNMPVariable_Bits()
    {
        delete value;
    }

    virtual std::string toString();
    
    protected:
    
    SNMPVariable_Bits(u_char *v, size_t l) 
    { 
        type = snmp_bits; 
        if (v)
        {
            len=l; 
            value = (uint32_t*)(new unsigned char[len]);
            memcpy((void*)value, v, len);
        } else
        {
            value = nullptr ;
            len   = 0    ;
        }
    }

    const uint32_t *value;
    size_t len;
};

/**
 * Unfortunately SNMP does not distinguish Netmask and InetAddr
 * types. On our framework they are different, and thus
 * we have to do late type conversion.
 * (lord)
 */
class SNMPVariable_IPaddr  : public SNMPVariable
{
    friend class SNMPVariable;
    
    public:
    
    virtual std::string toString();

    virtual InetAddr getInetAddrValue();
    virtual InetAddr getNetmaskValue();

    protected:
    
    SNMPVariable_IPaddr(u_char *v, size_t l)
    { 
        type  = snmp_ipaddr; 
        if(v)
        {
            len=l; 
            value=new unsigned char[len];
            memcpy((void*)value, v, len);
        } else
        {
            throw FWException("Invalid data for snmp_ipaddress variable.");
        }
    }

    const unsigned char *value;
    size_t len;
};

class SNMPVariable_String : public SNMPVariable
{
    friend class SNMPVariable;

    public:
    
    virtual std::string toString();
    virtual const std::string toHexString();
    
    protected:
    
    SNMPVariable_String(u_char *v, size_t l) 
    { 
        type=snmp_string; 

        if(v)
        {
            for(u_char *p=v; l; p++,l--)
            {
                value+=*p;
            }
        }
        else
            value="";
    }
    std::string value;
};

class SNMPVariable_Counter64 : public SNMPVariable
{
    friend class SNMPVariable;

    public:
    
    virtual std::string toString();
    
    protected:
    
    SNMPVariable_Counter64(struct counter64 *v) 
    { 
        type=snmp_counter64; 
        if(v)
        {
            high=v->high; 
            low=v->low; 
        } else
        {
            high=low=0l;
        }
    }
    unsigned long high;
    unsigned long low;
};

class SNMPVariable_OID : public SNMPVariable
{
    friend class SNMPVariable;

    public:
    
    virtual std::string toString();
    
    protected:
    
    SNMPVariable_OID(oid v) { type=snmp_oid; value=v; }
    oid value;
};

class SNMPConnection
{
    public:

    SNMPConnection(const std::string &peer, const std::string &community);
    virtual ~SNMPConnection();
    
    /**
     * Optional parameter timeout is in milliseconds.
     */
    void connect(int retries=SNMP_DEFAULT_RETRIES, long timeout=SNMP_DEFAULT_TIMEOUT);
    void disconnect();
    std::vector<SNMPVariable*> get(const std::string &variable);
    std::multimap<std::string, SNMPVariable*> walk(const std::string &variable);

    private:

    std::string peer;
    std::string community;
    bool connected;
    struct snmp_session *session, *session_data;
    static bool lib_initialized;
};

class SNMPQuery : public BackgroundOp
{
    private:
    
    static const char *SNMP_INTERFACE_ASTATUS;
    static const char *SNMP_INTERFACE_OSTATUS;
    static const char *SNMP_INTERFACE_INDEX  ;
    static const char *SNMP_INTERFACES_DESCR ;
    static const char *SNMP_INTERFACES_PHYSA ;
    static const char *SNMP_INTERFACES_TYPE  ;
    static const char *SNMP_ADDR_INDEX_TABLE ;
    static const char *SNMP_NMASK_TABLE      ;
    static const char *SNMP_ADDR_TABLE       ;
    static const char *SNMP_BCAST_TABLE      ;

    static const char *SNMP_AT_TABLE_NET     ;
    static const char *SNMP_AT_TABLE_PHYS    ;

    static const char *SNMP_ROUTE_DST_TABLE  ;
    static const char *SNMP_ROUTE_NM_TABLE   ;
    static const char *SNMP_ROUTE_TYPE_TABLE ;
    static const char *SNMP_ROUTE_IF_TABLE   ;
    static const char *SNMP_ROUTE_GW_TABLE   ;
    static const int   SNMP_DIRECT_ROUTE     ;
    
    static const char *SNMP_SYSNAME  ;
    static const char *SNMP_SYSDESCR ;
    static const char *SNMP_CONTACT  ;
    static const char *SNMP_LOCATION ;

    static const char *IP_MIB_PREFIX ;
    static const char *IP_MIB_ENTIFINDEX_PREFIX ;

    static const char *RFC1213_MIB_PREFIX ;
    static const char *RFC1213_MIB_PREFIX_IPROUTEDEST_PREFIX ;

    static const char* SNMP_IP_MIB_RFC4293_V4_INDEX;
    static const char* SNMP_IP_MIB_RFC4293_V6_INDEX;
    static const char* SNMP_IP_MIB_RFC4293_V6_PREFIX;
    
    std::string  hostname, community;
    std::string  descr, contact, location, sysname;
    std::map<int, InterfaceData> interfaces;
    std::map<InetAddr, std::string> arptable;
    std::vector<IPRoute> routes;
    int  retries;
    long timeout;

    void walkInterfaceIndexTable(Logger *logger,
                                 SNMPConnection *c,
                                 const char* OID,
                                 std::map<long, std::list<std::string> > &addr);
    void getAddressAndNetmask(Logger *logger,
                              SNMPConnection *c,
                              std::string adentry,
                              InetAddr **addr,
                              InetAddr **netmask);

public:

    SNMPQuery() {}

    SNMPQuery(std::string hostname, 
	      std::string community, 
	      int retries_=SNMP_DEFAULT_RETRIES, 
	      long timeout_=SNMP_DEFAULT_TIMEOUT);

    virtual ~SNMPQuery();

    void init(std::string hostname, 
	      std::string community, 
	      int retries_=SNMP_DEFAULT_RETRIES, 
	      long timeout_=SNMP_DEFAULT_TIMEOUT);

    void fetchArpTable(Logger *,std::atomic<bool> *stop_program,
                       SNMPConnection *connection=nullptr);
    void fetchInterfaces(Logger *,std::atomic<bool> *stop_program,
                         SNMPConnection *connection=nullptr);
    void fetchSysInfo(Logger *,std::atomic<bool> *stop_program,
                      SNMPConnection *connection=nullptr);
    void fetchAll(Logger *,std::atomic<bool> *stop_program);
    void fetchRoutingTable(Logger *,std::atomic<bool> *stop_program,
                           SNMPConnection *connection=nullptr);
    
    std::map<int, InterfaceData>* getInterfaces();
    std::map<InetAddr, std::string>* getArpTable();
    std::vector<IPRoute>* getRoutes();

    const std::string& getSysname  ();
    const std::string& getDescr    ();
    const std::string& getContact  ();
    const std::string& getLocation ();

    protected:

    bool isDefault(const IPRoute &) const;

};

class SNMP_interface_query : public SNMPQuery
{
    public:

    SNMP_interface_query() : SNMPQuery() {}

    SNMP_interface_query(std::string hostname, 
			 std::string community, 
			 int retries_=SNMP_DEFAULT_RETRIES, 
			 long timeout_=SNMP_DEFAULT_TIMEOUT) :
	SNMPQuery(hostname, community, retries_, timeout_) {}

    void init(std::string hostname, 
	      std::string community, 
	      int retries_=SNMP_DEFAULT_RETRIES, 
	      long timeout_=SNMP_DEFAULT_TIMEOUT)  {
	SNMPQuery::init(hostname, community, retries_, timeout_);
    }
    
    virtual void run_impl(Logger *logger,std::atomic<bool> *stop_program);

};

class SNMP_sysdesc_query : public SNMPQuery
{
    public:
    
    SNMP_sysdesc_query() : SNMPQuery() {}

    SNMP_sysdesc_query(std::string hostname, 
		       std::string community, 
		       int retries_=SNMP_DEFAULT_RETRIES, 
		       long timeout_=SNMP_DEFAULT_TIMEOUT) :
	SNMPQuery(hostname, community, retries_, timeout_) {}

    void init(std::string hostname, 
	      std::string community, 
	      int retries_=SNMP_DEFAULT_RETRIES, 
	      long timeout_=SNMP_DEFAULT_TIMEOUT) {
	SNMPQuery::init(hostname, community, retries_, timeout_);
    }

    virtual void run_impl(Logger *logger,std::atomic<bool> *stop_program);
};

class SNMP_discover_query : public SNMPQuery
{
    private:

    bool fetch_inerfaces;
    
    public:
    
    SNMP_discover_query() : SNMPQuery() {}
    SNMP_discover_query(std::string hostname, std::string community, int retries_=SNMP_DEFAULT_RETRIES, long timeout_=SNMP_DEFAULT_TIMEOUT, bool _f=true):SNMPQuery(hostname, community, retries_, timeout_) { fetch_inerfaces=_f; }
    
    virtual void run_impl(Logger *logger,std::atomic<bool> *stop_program);
};

class CrawlerFind: public HostEnt
{
    public:
    
    CrawlerFind();
    virtual ~CrawlerFind();

    bool have_snmpd ;
    std::string  descr, contact, location, sysname;
    std::map<int, InterfaceData> interfaces;

    /**
     * What physical address host had in ARP table when it was found.
     * (optional)
     */
    std::string found_phys_addr; 

    //set when DNS resolution succeeded.
    bool dns_ok       ;
};

class SNMPCrawler : public BackgroundOp
{
    private:

    const std::vector<InetAddrMask> *limit_to_networks ;
    static const InetAddrMask LOOPBACK_NET ;
    static const InetAddrMask IPV6_LOOPBACK_NET ;
    static const InetAddr PTP_NETMASK  ;

    std::map<InetAddr, std::string> queue;
    std::map<InetAddr, CrawlerFind> found;
    std::set<InetAddrMask> networks;
    std::string community;
    int                          snmp_retries ;
    long                         snmp_timeout ;
    bool                         recursive    ;
    bool                         do_dns       ;
    bool                         follow_ptp   ;
    unsigned int                 dns_threads  ;
    int                          dns_retries  ;
    int                          dns_timeout  ;

    static const int  PTP_INTERFACE_TYPES[];

    protected:

    bool included    (const InetAddr &) const ;
    bool alreadyseen (const InetAddr &) const ;
    bool special     (const InetAddrMask &) const ;
    bool special     (const InetAddr &) const ;
    bool point2point (const InetAddrMask &, const InterfaceData&) const ;
    bool point2point (const InterfaceData&) const ;

    std::list<InterfaceData> guessInterface(
        const IPRoute &r, const std::map<int, InterfaceData> &intf) const;

    public:

    SNMPCrawler();
    SNMPCrawler(const InetAddr &seed, 
                const std::string &_community,
                bool _recursive=true,
                bool _do_dns=true,
                bool _follow_ptp=false,
                unsigned int _dns_threads=10,
                int  _retries=SNMP_DEFAULT_RETRIES, 
                long _timeout=SNMP_DEFAULT_TIMEOUT,
		int  _dns_retries=RES_DFLRETRY,
		int  _dns_timeout=RES_TIMEOUT,
                const std::vector<InetAddrMask> *include=nullptr);
    virtual ~SNMPCrawler();

    void init(const InetAddr &seed, 
	      const std::string &_community,
	      bool _recursive=true,
              bool _do_dns=true,
              bool _follow_ptp=false,
              unsigned int _dns_threads=10,
	      int  _snmp_retries=SNMP_DEFAULT_RETRIES, 
	      long _snmp_timeout=SNMP_DEFAULT_TIMEOUT,
	      int  _dns_retries=RES_DFLRETRY,
	      int  _dns_timeout=RES_TIMEOUT,
	      const std::vector<InetAddrMask> *include=nullptr);

    std::map<InetAddr, CrawlerFind>  getAllIPs();
    std::set<InetAddrMask> getNetworks();
        
    virtual void run_impl(Logger *logger,std::atomic<bool> *stop_program);
    void bacresolve_results(Logger *logger,std::atomic<bool> *stop_program);

};

}

#endif

#endif

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



#ifdef HAVE_LIBSNMP

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

#include "fwbuilder/physAddress.h"
#include "fwbuilder/InetAddrMask.h"
#include "fwbuilder/Inet6AddrMask.h"
#include "fwbuilder/IPv4.h"

/*
 * we do not really use getops here, but net-snmp does. On FreeBSD, if
 * port devel/libgnugetopt is installed, it installs file
 * /usr/local/include/getopt.h. Definition of getopt in this file
 * conflicts with that in unistd.h, so this module does not
 * compile. To solve this problem, I put -I/usr/include first in
 * INCLUDEPATH in Makefiles and #define _GETOPT_H here, so that when
 * it is included from /usr/local, its internal wrapper macro
 * _GETOPT_H will have been defined and include file won't be
 * processed
 */
#define _GETOPT_H

#include "fwbuilder/snmp.h"

#ifdef UCD_SNMP
#include <ucd-snmp/parse.h>
#endif

#include <typeinfo>
#include <algorithm>
#include <memory>
#include <sstream>
#include <iterator>

/*
#ifdef _WIN32
#define snprintf sprintf_s
#endif
*/

/**
 * Define this if you need extra debug output.
 */
#undef SNMP_CRAWLER_DEBUG

using namespace std;
using namespace libfwbuilder;

/* Compiled OIDs */
/* We use #define so the compiler can do string concatenation for snprintf */
#define SNMP_INTERFACE_ASTATUS ".1.3.6.1.2.1.2.2.1.7"
#define SNMP_INTERFACE_OSTATUS ".1.3.6.1.2.1.2.2.1.8"
#define SNMP_INTERFACE_INDEX   ".1.3.6.1.2.1.2.2.1.1"
#define SNMP_INTERFACES_DESCR  ".1.3.6.1.2.1.2.2.1.2"
#define SNMP_INTERFACES_PHYSA  ".1.3.6.1.2.1.2.2.1.6"
#define SNMP_INTERFACES_TYPE   ".1.3.6.1.2.1.2.2.1.3"
#define SNMP_ADDR_INDEX_TABLE  ".1.3.6.1.2.1.4.20.1.2"
#define SNMP_NMASK_TABLE       ".1.3.6.1.2.1.4.20.1.3"
#define SNMP_ADDR_TABLE        ".1.3.6.1.2.1.4.20.1.1"
#define SNMP_BCAST_TABLE       ".1.3.6.1.2.1.4.20.1.4"
#define SNMP_AT_TABLE_NET      ".1.3.6.1.2.1.3.1.1.3"
#define SNMP_AT_TABLE_PHYS     ".1.3.6.1.2.1.3.1.1.2"

const char *SNMPQuery::SNMP_SYSNAME          = ".1.3.6.1.2.1.1.5.0";
const char *SNMPQuery::SNMP_SYSDESCR         = ".1.3.6.1.2.1.1.1.0";
const char *SNMPQuery::SNMP_CONTACT          = ".1.3.6.1.2.1.1.4.0";
const char *SNMPQuery::SNMP_LOCATION         = ".1.3.6.1.2.1.1.6.0";

const char *SNMPQuery::SNMP_ROUTE_DST_TABLE  = ".1.3.6.1.2.1.4.21.1.1";
const char *SNMPQuery::SNMP_ROUTE_NM_TABLE   = ".1.3.6.1.2.1.4.21.1.11";
const char *SNMPQuery::SNMP_ROUTE_TYPE_TABLE = ".1.3.6.1.2.1.4.21.1.8";
const char *SNMPQuery::SNMP_ROUTE_IF_TABLE   = ".1.3.6.1.2.1.4.21.1.2";
const char *SNMPQuery::SNMP_ROUTE_GW_TABLE   = ".1.3.6.1.2.1.4.21.1.7";
const int   SNMPQuery::SNMP_DIRECT_ROUTE     = 3;

const char* SNMPQuery::SNMP_IP_MIB_RFC4293_V6_INDEX =
    ".1.3.6.1.2.1.4.34.1.3.2.16";
const char* SNMPQuery::SNMP_IP_MIB_RFC4293_V6_PREFIX =
    ".1.3.6.1.2.1.4.34.1.5.2.16";


/**
 * Following interface types correspond to point-to-point interfaces.
 * (in ascending order!)
 */
const int SNMPCrawler::PTP_INTERFACE_TYPES[]={
    17, // sdlc
    18, // ds1
    19, // e1
    20, // basicISDN
    21, // primaryISDN
    22, // propPointToPointSerial
    23, // ppp
    28, // slip
    30, // ds3
    32 // frameRelay. According to 'vk' it could be also broadcast
    // capable in some weird setup, but we assume it is p-t-p only.
   
};

/*
 * temporary database object, needs this to be able to create
 * interfaces and then add ip addresses to them
 */
static FWObjectDatabase *snmp_tmp_db = nullptr;


#include <iostream>

SNMPQuery::SNMPQuery(string h,string c, int retries_, long timeout_)
{
    hostname  = h;
    community = c;
    retries   = retries_;
    timeout   = timeout_;

    descr     = "";
    contact   = "";
    location  = "";
}

void SNMPQuery::init(string h, string c, int retries_, long timeout_)
{
    hostname  = h;
    community = c;
    retries   = retries_;
    timeout   = timeout_;

    descr     = "";
    contact   = "";
    location  = "";

    char *snmp_out_opt = (char*)("n");
    snmp_out_toggle_options(snmp_out_opt);

    if (snmp_tmp_db==nullptr)
        snmp_tmp_db = new FWObjectDatabase();
}

SNMPQuery::~SNMPQuery()
{
}

void SNMPQuery::fetchAll(Logger *logger,std::atomic<bool> *stop_program)
{
    if(community.empty())
        throw FWException("No SNMP community specified");

    if(hostname.empty())
        throw FWException("No SNMP hostname specified");

    SNMPConnection c(hostname, community);
    c.connect(retries, timeout);

    CHECK_STOP_AND_THROW_EXCEPTION;
    
    fetchSysInfo   (logger,stop_program, &c);
    CHECK_STOP_AND_THROW_EXCEPTION;
    
    fetchInterfaces(logger,stop_program, &c);
    CHECK_STOP_AND_THROW_EXCEPTION;
    
    fetchArpTable  (logger,stop_program, &c);
    CHECK_STOP_AND_THROW_EXCEPTION;

    fetchRoutingTable  (logger,stop_program, &c);
    CHECK_STOP_AND_THROW_EXCEPTION;
}

void SNMPQuery::fetchArpTable(Logger *logger,std::atomic<bool> *stop_program, SNMPConnection *connection)
{
    std::ostringstream str;
    *logger << "ARP table\n"; 
    arptable.clear();
    
    SNMPConnection *c;
    if(connection)
    {
        c=connection;
    } else
    {
        if(community.empty())
            throw FWException("No SNMP community specified");
       
        if(hostname.empty())
            throw FWException("No SNMP hostname specified");
       
        c=new SNMPConnection(hostname, community);
        try
        {
            c->connect(retries, timeout);
        } catch(...)
        {
            // If program was stopped, show this, rather
            // than error.
            throw;
        }
    }

    vector<SNMPVariable*> v;
    try
    {
        *logger << "Walking atNetAddress table...\n";
        multimap<string, SNMPVariable* > nw=c->walk(SNMP_AT_TABLE_NET  );
        CHECK_STOP_AND_THROW_EXCEPTION;
        *logger << "Walking atPhysAddress table...\n";
        multimap<string, SNMPVariable* > pw=c->walk(SNMP_AT_TABLE_PHYS );
        CHECK_STOP_AND_THROW_EXCEPTION;

        try
        {
//            int preflen = strlen(SNMP_AT_TABLE_NET);
            for(multimap<string, SNMPVariable* >::iterator j = nw.begin(); j!=nw.end(); ++j)
            {
                CHECK_STOP_AND_THROW_EXCEPTION;
                if((*j).second->type!=SNMPVariable::snmp_ipaddr)
                {
                    *logger << "unexpected result type in '" << 
                    SNMP_AT_TABLE_NET << "' table. Skipping it.\n";
                    continue;
                }
                InetAddr ip = dynamic_cast<SNMPVariable_IPaddr*>((*j).second)->getInetAddrValue();

                str << "Learned: " << ip.toString();

                for(multimap<string, SNMPVariable* >::iterator k = pw.begin(); k!=pw.end(); ++k)
                {
                    string pa_oid= k->first;
                    if ( pa_oid.find( ip.toString() )!=string::npos )
                    {
                        string hwaddr=dynamic_cast<SNMPVariable_String*>(k->second)->toHexString();
                        arptable.insert(make_pair(ip,hwaddr));
                        str << " at " << hwaddr;
                        break;
                    }
                }
                *logger << str;
                
//                *logger << "\n";
            }
        } catch(...)
        {
            for(multimap<string, SNMPVariable* >::iterator j = nw.begin(); j!=nw.end(); ++j)
                delete (*j).second;
            for(multimap<string, SNMPVariable* >::iterator j = pw.begin(); j!=pw.end(); ++j)
                delete (*j).second;
            throw;
        }
        for(multimap<string, SNMPVariable* >::iterator j = nw.begin(); j!=nw.end(); ++j)
            delete (*j).second;
        for(multimap<string, SNMPVariable* >::iterator j = pw.begin(); j!=pw.end(); ++j)
            delete (*j).second;
    } catch (FWException &ex)
    {
        SNMPVariable::freeVarList(v);
        if(!connection)
            delete c;
        // If program was stopped, show this, rather
        // than error.
        throw;
    }
    if(!connection)
        delete c;
    *logger << "Done fetching ARP table\n";
}

/**
 * Fetches routing table. If interfaces have been already found
 * it will contain also reference to interface object associated
 * with the route.
 */
void SNMPQuery::fetchRoutingTable(Logger *logger,std::atomic<bool> *stop_program, SNMPConnection *connection)
{
    std::ostringstream str;
    *logger << "Routing table\n"; 
    routes.clear();
    
    SNMPConnection *c;
    if(connection)
    {
        c=connection;
    } else
    {
        if(community.empty())
            throw FWException("No SNMP community specified");
        
        if(hostname.empty())
            throw FWException("No SNMP hostname specified");
        
        c=new SNMPConnection(hostname, community);
        try
        {
            c->connect(retries, timeout);
        } catch(...)
        {
            // If program was stopped, show this, rather
            // than error.
            throw;
        }
    }

    CHECK_STOP_AND_THROW_EXCEPTION;
    
    vector<SNMPVariable*> v;
    try
    {
        // Get Indices and destinations
        
        multimap<string, SNMPVariable* > w=c->walk(SNMP_ROUTE_DST_TABLE);
        CHECK_STOP_AND_THROW_EXCEPTION;
        try
        {
            for(multimap<string, SNMPVariable* >::iterator j = w.begin(); j!=w.end(); ++j)
            {
                CHECK_STOP_AND_THROW_EXCEPTION;
                if((*j).second->type!=SNMPVariable::snmp_ipaddr)
                {
                    str << "unexpected result type in '"
                        << SNMP_ROUTE_DST_TABLE << "' table. Skipping it.\n";
                    *logger << str;
                    
                    continue;
                }
                InetAddr dst = dynamic_cast<SNMPVariable_IPaddr*>((*j).second)->getInetAddrValue();
                string rname=(*j).first.substr(strlen(SNMP_ROUTE_DST_TABLE)+1);

                v=c->get(string(SNMP_ROUTE_NM_TABLE)+"."+rname);
                if(v[0]->type!=SNMPVariable::snmp_ipaddr)
                {
                    str << "unexpected result type in '"
                        << SNMP_ROUTE_NM_TABLE << "'table. Skipping it.\n";
                    *logger << str;
                    
                    SNMPVariable::freeVarList(v);
                    continue;
                }
                InetAddr nm = dynamic_cast<SNMPVariable_IPaddr*>(v[0])->getNetmaskValue();
                SNMPVariable::freeVarList(v);

                v=c->get(string(SNMP_ROUTE_TYPE_TABLE)+"."+rname);
                long type = SNMPVariable::varList2Int(v);
                SNMPVariable::freeVarList(v);

                v=c->get(string(SNMP_ROUTE_GW_TABLE)+"."+rname);
                if(v[0]->type!=SNMPVariable::snmp_ipaddr)
                {
                    str << "unexpected result type in '"
                        << SNMP_ROUTE_GW_TABLE << "'table. Skipping it.\n";
                    *logger << str;
                    
                    SNMPVariable::freeVarList(v);
                    continue;
                }
                InetAddr gw = dynamic_cast<SNMPVariable_IPaddr*>(v[0])->getInetAddrValue();
                SNMPVariable::freeVarList(v);

                v=c->get(string(SNMP_ROUTE_IF_TABLE)+"."+rname);
                int ifindex = SNMPVariable::varList2Int(v);
                SNMPVariable::freeVarList(v);
 
                map<int,InterfaceData>::iterator ici = interfaces.find(ifindex);
                
                InterfaceData route_intf;
                if(ici==interfaces.end())
                {
                    str << "Route references non-existing interface '"
                        << ifindex << "' in '"
                        << string(SNMP_ROUTE_IF_TABLE)+"."+rname
                        << "' variable. Ignoring interface reference.\n";
                    *logger << str;
                    route_intf.name = "(interface not found)";
                } else
                {
#ifdef SNMP_CRAWLER_DEBUG
                    str << "Found interface # " << ifindex
                        << " associated with the route: "
                        << string(SNMP_ROUTE_IF_TABLE)+"."+rname << "\n";
                    *logger << str;
                    
#endif
                    route_intf = ici->second;
                }
                bool isdef= type!=SNMP_DIRECT_ROUTE &&
                    nm.getLength() == 0 && dst == InetAddr("0.0.0.0");

		if (isdef) route_intf.ext = true;

                IPRoute rt(dst, nm, gw, route_intf, type==SNMP_DIRECT_ROUTE);
                routes.push_back(rt);

                str << "route: " 
                    << rt.getDestination().toString() 
                    << "/" << rt.getNetmask().getLength() 
                    << "  gw " 
                    << rt.getGateway().toString()
                    << "  "
                    << rt.getInterface().name
                    << string( (rt.getInterface().ext)?"(Ext)":"")
                    << "\n";
                *logger << str;
                
            }
        } catch(...)
        {
            for(multimap<string, SNMPVariable* >::iterator j = w.begin(); j!=w.end(); ++j)
                delete (*j).second;
            throw;
        }
        for(multimap<string, SNMPVariable* >::iterator j = w.begin(); j!=w.end(); ++j)
            delete (*j).second;
        w.clear();

    } catch (FWException &ex)
    {
        SNMPVariable::freeVarList(v);
        if(!connection)
            delete c;
        // If program was stopped, show this, rather
        // than error.
        throw;
    }
    if(!connection)
        delete c;
    *logger << "Done fetching routing table\n";
}

bool SNMPQuery::isDefault(const IPRoute &r) const
{
    return !r.isDirect() &&
    r.getNetmask().getLength() == 0 &&
    r.getDestination() == InetAddr("0.0.0.0");
}

void SNMPQuery::walkInterfaceIndexTable(Logger *logger, 
                                        SNMPConnection *c,
                                        const char* oid,
                                        map<long, list<string> > &addr)
{
    std::ostringstream str;
    multimap<string, SNMPVariable* > w;
    w = c->walk(oid);
    CHECK_STOP_AND_THROW_EXCEPTION;

    for (multimap<string, SNMPVariable* >::iterator j = w.begin();
         j!=w.end(); ++j)
    {
        CHECK_STOP_AND_THROW_EXCEPTION;
        if ((*j).second->type != SNMPVariable::snmp_int)
        {
            str << "unexpected result type in '"
                << oid << "' table. Skipping it.\n";
            *logger << str;
                
            continue;
        }
        long ind = SNMPVariable::var2Int((*j).second);

        string iname = (*j).first.substr(strlen(oid)+1);

/*
 * Example (two interfaces, first has two addresses):
 *
 * ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.10.3.14.201 = 1
 * ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.192.168.100.100 = 1
 * ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.222.22.22.222 = 2
 *
 * at this point iname is watever text we get after
 * "ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex." , ind is an index
 */
        addr[ind].push_back(iname);
        str << "interface #" << ind << ": " << iname << "\n";
        *logger << str;
    }

    for (multimap<string, SNMPVariable* >::iterator j = w.begin();
         j!=w.end(); ++j)
    {
        delete (*j).second;
    }
    w.clear();
}

void SNMPQuery::getAddressAndNetmask(Logger * /* UNUSED logger */,
                                     SNMPConnection *c,
                                     std::string adentry,
                                     InetAddr **addr,
                                     InetAddr **netmask)
{
    std::ostringstream str;
    vector<SNMPVariable*> v;
    list<string> components;
    istringstream iss(adentry);

    char item[4];
    while (iss.getline(item, 4, '.'))
    {
        components.push_back(item);
    }

    if (components.size()==4)
    {
        // IPv4 address

        // get netmasks
        v = c->get(string(SNMP_NMASK_TABLE)+"."+adentry);
        if (v.size()!=1)
            throw FWException("Can't get netmask");
        if (v[0]->type!=SNMPVariable::snmp_ipaddr)
            throw FWException("Wrong return type for netmask");
        *netmask = new InetAddr(dynamic_cast<SNMPVariable_IPaddr*>(
                                    v[0])->getNetmaskValue().toString());
        SNMPVariable::freeVarList(v);
            
        // get addresss
        v = c->get(string(SNMP_ADDR_TABLE)+"."+adentry);
        if(v.size()!=1)
            throw FWException("Can't get IP address");
        if(v[0]->type!=SNMPVariable::snmp_ipaddr)
            throw FWException("Wrong return type for IP address");
        *addr = new InetAddr(dynamic_cast<SNMPVariable_IPaddr*>(
                                 v[0])->getInetAddrValue().toString());
        SNMPVariable::freeVarList(v);
    }
    else
    {
        // Assuming everything that is not IPv4 is IPv6

        // get prefix
        string oid = string(SNMP_IP_MIB_RFC4293_V6_PREFIX) + "." + adentry;

        v = c->get(oid);
        if (v.size()!=1) throw FWException("Can't get prefix data");

        string pref = v[0]->toString();
        SNMPVariable::freeVarList(v);

        // response to this query has type OBJECT_ID. Corresponding
        // SNMPVariable class returns dot-separated string representation
        // (SNMPVariable_Bits)
        // We need to find the last octet, which is the prefix we need.
        
        pref = pref.substr(pref.rfind(".") + 1);

        *netmask = new InetAddr(AF_INET6, atoi(pref.c_str()));

        // to build proper ipv6 address need to convert adentry to
        // a ':' separated hex representation
        ostringstream str_addr;
        bool first = true;
        for (list<string>::iterator i=components.begin();
             i!=components.end(); ++i)
        {
            if (!first) str_addr << ":";
            str_addr << hex << atoi(i->c_str());
            ++i;
            str_addr << hex << atoi(i->c_str());
            first = false;
        }
        *addr = new InetAddr(AF_INET6, str_addr.str());
    }
}

void SNMPQuery::fetchInterfaces(Logger *logger, std::atomic<bool> *stop_program,
                                SNMPConnection *connection)
{
    std::ostringstream str;

    interfaces.clear();
    
    SNMPConnection *c;
    if(connection)
    {
        c=connection;
    } else
    {
        if(community.empty())
            throw FWException("No SNMP community specified");
        
        if(hostname.empty())
            throw FWException("No SNMP hostname specified");
        
        c=new SNMPConnection(hostname, community);
        try
        {
            c->connect(retries, timeout);
        } catch(...)
        {
            // If program was stopped, show this, rather
            // than error.
            throw;
        }
    }
    
    CHECK_STOP_AND_THROW_EXCEPTION;
            
    multimap<string, SNMPVariable* > w;
    vector<SNMPVariable*>            v;
    try
    {

        map<long, list<string> > addr;

        *logger << "Getting IPv4 addresses.";
        walkInterfaceIndexTable(logger, c, SNMP_ADDR_INDEX_TABLE, addr);

        *logger << "Getting IPv6 addresses using IP-MIB RFC4293.\n";
        *logger << "This MIB is only supported by latest versions of net-snmp\n";
        walkInterfaceIndexTable(logger, c, SNMP_IP_MIB_RFC4293_V6_INDEX, addr);

        // ****************************************************************
        *logger << "Collecting full interfaces info\n";

        w = c->walk(SNMP_INTERFACE_INDEX);
        CHECK_STOP_AND_THROW_EXCEPTION;
        
        str << w.size() << " interfaces found\n";
        *logger << str;
            
        for (multimap<string, SNMPVariable* >::iterator j = w.begin();
             j!=w.end(); ++j)
        {
            CHECK_STOP_AND_THROW_EXCEPTION;
                
            // Get index
            long ifindex = SNMPVariable::var2Int((*j).second);
            char oid[1024];

            // Get admin status
            snprintf(oid, sizeof(oid), SNMP_INTERFACE_ASTATUS ".%ld", ifindex);
            v=c->get(oid);
            if(v.size()!=1)
                throw FWException(
                    string("Unexpected response length for OID: ") + oid);

            int astatus = SNMPVariable::varList2Int(v);
            SNMPVariable::freeVarList(v);

            if (!astatus)
            {
                str << "Interface #" << ifindex
                    << " disabled by admin - skipping.\n";
                *logger << str;
                
                continue;
            }
            
            // Get operational status
            snprintf(oid, sizeof(oid), SNMP_INTERFACE_OSTATUS ".%ld", ifindex);
            v=c->get(oid);
            if(v.size()!=1)
                throw FWException(
                    string("Unexpected response length for OID: ") + oid);

            int ostatus = SNMPVariable::varList2Int(v);
            SNMPVariable::freeVarList(v);

/* gather all information for interface ifindex and create Interface object */

            // Get desriptions
            snprintf(oid, sizeof(oid), SNMP_INTERFACES_DESCR ".%ld", ifindex);
            v=c->get(oid);
            string descr = SNMPVariable::varList2String(v);
            SNMPVariable::freeVarList(v);

            list<string> &addlist = addr[ifindex];

            // Get physical address
            snprintf(oid, sizeof(oid), SNMP_INTERFACES_PHYSA ".%ld", ifindex);
            v=c->get(oid);
            if(v.size()!=1)
                throw FWException(string("Unexpected response length for OID: ")+oid);
            if(v[0]->type!=SNMPVariable::snmp_string)
                throw FWException(string("Unexpected response type for: ")+oid);
            string physa = dynamic_cast<SNMPVariable_String*>(v[0])->toHexString();
            SNMPVariable::freeVarList(v);

            // Get type
            snprintf(oid, sizeof(oid), SNMP_INTERFACES_TYPE ".%ld", ifindex);
            v=c->get(oid);
            if(v.size()!=1)
                throw FWException(string("Unexpected response length for OID: ")+oid);                
            int itype = SNMPVariable::varList2Int(v);
            SNMPVariable::freeVarList(v);

            InterfaceData idata;
            idata.name = descr;
            idata.snmp_type = itype;
            idata.ext = false;
            idata.ostatus = ostatus;

            //snmp_tmp_db->add( &interfaces[ifindex] );

            if (physa!="") idata.mac_addr = physa;

            str << "Adding interface #" << ifindex 
                << " " << idata.name
                << " MAC: " << idata.mac_addr << endl;
            *logger << str;

             if (addlist.empty())
             {
                 str << "    no IP addresses." << endl;
                 *logger << str;
             }

            for (list<string>::iterator ali = addlist.begin();
                 ali!=addlist.end(); ali++)
            {
                CHECK_STOP_AND_THROW_EXCEPTION;

                InetAddr *ad = nullptr;
                InetAddr *nm = nullptr;
                getAddressAndNetmask(logger, c, *ali, &ad, &nm);

                InetAddrMask *iam = nullptr;

                if (ad->isV6()) iam = new Inet6AddrMask();
                else iam = new InetAddrMask();
                iam->setAddress(*ad);
                iam->setNetmask(*nm);
                idata.addr_mask.push_back(iam);

                str << "    " << descr
                    << " IP address"
                    << " " << iam->toString()
                    << "\n";
                *logger << str;
                
                if (ad) delete ad;
                if (nm) delete nm;
            }

            interfaces[ifindex] = idata;

        } // index walk.

        for (multimap<string, SNMPVariable* >::iterator j = w.begin();
             j!=w.end(); ++j)
        {
            delete (*j).second;
        }
        w.clear();

    } catch (FWException &ex)
    {

        SNMPVariable::freeVarList(v);

        for (multimap<string, SNMPVariable* >::iterator j = w.begin();
             j!=w.end(); ++j)
        {
            delete (*j).second;
        }
        w.clear();

        if (!connection) delete c;

        // If program was stopped, show this, rather
        // than error.
        throw;
    }
    if (!connection) delete c;
    *logger << "Done fetching interfaces\n";
}

void SNMPQuery::fetchSysInfo(Logger *logger,
                             std::atomic<bool> *stop_program,
                             SNMPConnection *connection)
{
    std::ostringstream str;
    descr     = "";
    contact   = "";
    location  = "";
    sysname   = "";

    SNMPConnection *c;
    if(connection)
    {
        c=connection;
    } else
    {
        if(community.empty())
        {
            throw FWException("No SNMP community specified");
        }
        
        if(hostname.empty())
        {
            throw FWException("No SNMP hostname specified");
        }
        
        c=new SNMPConnection(hostname, community);
        try
        {
            c->connect(retries, timeout);
        } catch(...)
        {
            // If program was stopped, show this, rather
            // than error.
            throw;
        }
    }
    
    try
    {
        vector<SNMPVariable*> v;
        
        *logger << "Getting System name\n";
        v=c->get(SNMP_SYSNAME);
        sysname = SNMPVariable::varList2String(v);
        SNMPVariable::freeVarList(v);
        CHECK_STOP_AND_THROW_EXCEPTION;

        *logger << "Getting Description\n";
        v=c->get(SNMP_SYSDESCR);
        descr = SNMPVariable::varList2String(v);
        SNMPVariable::freeVarList(v);
        CHECK_STOP_AND_THROW_EXCEPTION;
        
        *logger << "Getting Location\n";
        v=c->get(SNMP_LOCATION);
        location = SNMPVariable::varList2String(v);
        SNMPVariable::freeVarList(v);
        CHECK_STOP_AND_THROW_EXCEPTION;
        
        *logger << "Getting Contact Info\n";
        v=c->get(SNMP_CONTACT);
        contact  = SNMPVariable::varList2String(v);
        SNMPVariable::freeVarList(v);
        CHECK_STOP_AND_THROW_EXCEPTION;
    } catch(...)
    {
        if(!connection)
            delete c;
        // If program was stopped, show this, rather
        // than error.
        throw;
    }
    if(!connection)
        delete c;
    *logger << "Done fetching sysinfo\n";
}

vector<IPRoute>* SNMPQuery::getRoutes()
{
    return &routes;
}

map<InetAddr, string>* SNMPQuery::getArpTable()
{
    return &arptable;
}

map<int, InterfaceData>* SNMPQuery::getInterfaces()
{
    return &interfaces;
}

const string& SNMPQuery::getSysname()
{
    return sysname;
}

const string& SNMPQuery::getDescr()
{
    return descr;
}

const string& SNMPQuery::getContact()
{
    return contact;
}

const string& SNMPQuery::getLocation()
{
    return location;
}

// -----------------------------------------------

bool SNMPConnection::lib_initialized = false;

SNMPConnection::SNMPConnection(const string &p, const string &c)
{
    connected    = false;
    session_data = nullptr;
    peer         = p;
    community    = c;
    if(!lib_initialized)
    {
        init_snmp("fwbuilder");
        lib_initialized = true;
    }
}

SNMPConnection::~SNMPConnection()
{
    if(connected)
        disconnect();
}

void SNMPConnection::connect(int retries, long timeout)
{
    if(connected)
        throw FWException("SNMPSession: already connected");
    
    session_data = new struct snmp_session;
    snmp_sess_init( session_data );
    session_data->version       = SNMP_VERSION_1;
    session_data->peername      = cxx_strdup(peer.c_str());
    session_data->community     = (unsigned char *)cxx_strdup(community.c_str());
    session_data->community_len = community.length();
    session_data->retries       = retries;
    session_data->timeout       = timeout;
    
    session = snmp_open(session_data);
    if(!session)
        throw FWException("SNMPSession: error while establishing connection.");
        
    connected=true;
}

void SNMPConnection::disconnect()
{
    if(!connected)
        throw FWException("SNMPSession: already disconnected");

    snmp_close(session);

    delete session_data->peername;
    delete session_data->community;
    delete session_data;
    session_data = nullptr;
    connected    = false;
}

multimap<string, SNMPVariable* > SNMPConnection::walk(const string &variable)
{
    multimap<string, SNMPVariable*> res;

    oid    root[MAX_OID_LEN];
    size_t rootlen;
    
    rootlen = MAX_OID_LEN;
//    get_node(variable.c_str(), root, &rootlen); //TODO: error check

    read_objid(variable.c_str(), root, &rootlen); //TODO: error check

    oid    name[MAX_OID_LEN];
    size_t name_length;

    /* get first object to start walk */
    memmove(name, root, rootlen * sizeof(oid));
    name_length = rootlen;

    bool running=true;
    while(running)
    {
        /* create PDU for GETNEXT request and add object name to request */
        struct snmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        snmp_add_null_var(pdu, name, name_length);
        
        /* do the request */
        struct snmp_pdu *response = nullptr;
        int status = snmp_synch_response(session, pdu, &response);
        if(status == STAT_SUCCESS)
        {
            if(response->errstat == SNMP_ERR_NOERROR)
            {
                for(struct variable_list *vars = response->variables; 
                    vars; 
                    vars = vars->next_variable
                )
                {
                    if ((vars->name_length < rootlen) ||
                        (memcmp(root, vars->name, rootlen * sizeof(oid))!=0))
                    {
                        /* not part of this subtree */
                        running = false;
                        continue;
                    }
                    char n[MAX_NAME_LEN];
                    snprint_objid (n, sizeof(n), vars->name, vars->name_length);
                    res.insert(make_pair(string(n),SNMPVariable::create(vars)));
                    if ((vars->type != SNMP_ENDOFMIBVIEW) &&
                        (vars->type != SNMP_NOSUCHOBJECT) &&
                        (vars->type != SNMP_NOSUCHINSTANCE)){
                        /* not an exception value */
                        memmove((char *)name, (char *)vars->name,
                                vars->name_length * sizeof(oid));
                        name_length = vars->name_length;
                    } else
                    {
                        /* an exception value, so stop */
                        running = false;
                    }

                }
            } else 
            {
                /* error in response, print it */
                if(response->errstat == SNMP_ERR_NOSUCHNAME)
                {
                    throw FWException("SNMPSession: error: End of MIB");
                } else 
                {
                    throw FWException(string("SNMPSession: SNMP error: ")+
                                      snmp_errstring(response->errstat));
                }
            }
//        } else if (status == STAT_TIMEOUT)
//        {
//            throw FWException("SNMPSession: SNMP timeout");
        } else
        {    // status == STAT_ERROR
            if (response != nullptr)
            {
                throw FWException(string("SNMPSession: SNMP error: '") +
                                  string(snmp_errstring(response->errstat)) + "'");
                snmp_free_pdu(response);
            } else 
            {
                std::ostringstream str;
                str << "SNMPSession: SNMP error, status ";
                str << status;
                int liberr,syserr;
                char *errstr = (char*)("");
                snmp_error(session, &liberr, &syserr, &errstr);
                str << " " << errstr;
                throw FWException(str.str());
            }
        }
    }
    
    return res;
}

vector<SNMPVariable*> SNMPConnection::get(const string &variable)
{
    if(!connected)
        throw FWException("SNMPSession: not connected");
    
    struct snmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_GET);
    oid anOID[MAX_OID_LEN];
    size_t anOID_len = MAX_OID_LEN;
//    get_node(variable.c_str(), anOID, &anOID_len); //TODO: error check
    read_objid(variable.c_str(), anOID, &anOID_len); //TODO: error check
    snmp_add_null_var(pdu, anOID, anOID_len);
    struct snmp_pdu *response;
    int status = snmp_synch_response(session, pdu, &response);
    if(status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) 
    {
        vector<SNMPVariable*> res;
        for(struct variable_list *vars = response->variables; 
            vars; 
            vars = vars->next_variable
        )
        {
            try
            {
                res.push_back(SNMPVariable::create(vars));
            } catch(const FWException &ex)
            {
                SNMPVariable::freeVarList(res);
                throw;
            }
        }
        
        if (response)
            snmp_free_pdu(response);
        
        return res;
    } else
    {
        if (response)
            snmp_free_pdu(response);
        throw FWException("SNMPSession: Error getting variable "+ variable);
    } 
}

SNMPVariable *SNMPVariable::create(struct variable_list *vars)
{
    switch(vars->type)
    {
    case ASN_INTEGER:
        return new SNMPVariable_Int(*vars->val.integer);
    case ASN_OCTET_STR:
        return new SNMPVariable_String(vars->val.string, vars->val_len);
    case ASN_BIT_STR:
        return new SNMPVariable_Bits(vars->val.bitstring, vars->val_len);
    case ASN_APP_COUNTER64:
        return new SNMPVariable_Counter64(vars->val.counter64);
    case ASN_OBJECT_ID:
        return new SNMPVariable_Bits(vars->val.bitstring, vars->val_len);
//        return new SNMPVariable_OID(*vars->val.objid);
    case ASN_IPADDRESS:
        return new SNMPVariable_IPaddr(vars->val.string, vars->val_len);
    default: 
        char x[32];
        snprintf(x, sizeof(x), "%d", (int)vars->type);
        throw FWException(string("Unknown SNMP variable type: ") + x);
    }
}

string SNMPVariable_Int::toString()
{
    char x[32];
    snprintf(x, sizeof(x), "%ld", value);
    return x;
}

string SNMPVariable_Bits::toString()
{
    string res;
    for (unsigned int i=0; i<len / 4; i++)
    {
        res += ".";

        char x[12];
        //TODO: now we print it in hex
        // we should print it in binary.
        snprintf(x, sizeof(x), "%d", (uint32_t)value[i]);
        //i += 4;
        res += x;
    }
    return res;
}

string SNMPVariable_IPaddr::toString()
{
    string res="SNMP InetAddr/Netmask[";
    for(size_t i=0;i<len;i++)
    {
        if(i) 
            res+=".";
        char x[12];
        snprintf(x, sizeof(x), "%d", (unsigned int)value[i]);
        res+=x;
    }
    res+="]";
    return res;
}

InetAddr SNMPVariable_IPaddr::getInetAddrValue()
{
    // value comes from ASN1-encoded snmp reply. I am not sure 100% but
    // I'll assume it consists of 4 bytes that represent ip address in
    // network order.
    union {
        struct in_addr ipaddr;
        char addr_bytes[4];
    } addr_conversion;
    memcpy((void*)(&addr_conversion), value, len);
    return InetAddr(&addr_conversion.ipaddr);
}

InetAddr SNMPVariable_IPaddr::getNetmaskValue()
{
    union {
        struct in_addr ipaddr;
        char addr_bytes[4];
    } addr_conversion;
    memcpy((void*)(&addr_conversion), value, len);
    return InetAddr(&addr_conversion.ipaddr);
}

string SNMPVariable_String::toString()
{
    return value;
}

const string SNMPVariable_String::toHexString()
{
    string res;
    for(unsigned i=0;i<value.length();i++)
    {
        if(i)
            res+=':';
        u_char c=value[i];
        char buf[16];
        snprintf(buf, sizeof(buf), "%02X", (unsigned int)c);
        res+=buf;
    }
    return res;
}

string SNMPVariable_Counter64::toString()
{
    char x[70];
    snprintf(x, sizeof(x), "[%ld:%ld]", (long)low, (long)high);
    return x;
}

string SNMPVariable_OID::toString()
{
    char x[32];
    snprintf(x, sizeof(x), "%ld", (long)value);
    return x;
}

void SNMPVariable::freeVarList(vector<SNMPVariable*> &v)
{
    vector<SNMPVariable*>::iterator j;
    for(j=v.begin(); j!=v.end(); ++j) 
        delete (*j);
    v.clear();
}

string SNMPVariable::varList2String(vector<SNMPVariable*> &v)
{
    string res;

    vector<SNMPVariable*>::iterator j;
    for(j=v.begin(); j!=v.end(); ++j) 
        res+=(*j)->toString();
    
    return res;
}

long SNMPVariable::var2Int(SNMPVariable *var)
{
    if(var->type != SNMPVariable::snmp_int)
        throw FWException("Could not extract integer from non-int SNMP variable.");
    return dynamic_cast<SNMPVariable_Int *>(var)->getIntValue();
}

long SNMPVariable::varList2Int(vector<SNMPVariable*> &v)
{
    if(v.size()!=1)
        throw FWException("Empty SNMP variable list returned. Could not extract integer");
    return SNMPVariable::var2Int(v[0]);
}

SNMPCrawler::SNMPCrawler()
{}

SNMPCrawler::SNMPCrawler(const InetAddr &_seed, 
                         const string &_community,
                         bool _recursive,
                         bool _do_dns,
                         bool _follow_ptp,
                         unsigned int _dns_threads,
                         int  _snmp_retries, 
                         long _snmp_timeout,
			 int  _dns_retries,
			 int  _dns_timeout,
                         const vector<InetAddrMask> *_limit_to_networks)
{
    init(_seed, _community, _recursive, 
         _do_dns, _follow_ptp, _dns_threads, 
	 _snmp_retries,  _snmp_timeout, _dns_retries, _dns_timeout, 
	 _limit_to_networks);
}

SNMPCrawler::~SNMPCrawler()
{
}

void SNMPCrawler::init(const InetAddr &_seed, 
		       const string &_community,
		       bool _recursive,
                       bool _do_dns,
                       bool _follow_ptp,
                       unsigned int _dns_threads,
		       int _snmp_retries, 
		       long _snmp_timeout,
		       int  _dns_retries,
		       int  _dns_timeout,
		       const vector<InetAddrMask> *_limit_to_networks)
{
    limit_to_networks = _limit_to_networks;
    community    = _community;
    snmp_retries = _snmp_retries;
    snmp_timeout = _snmp_timeout;
    recursive    = _recursive;
    do_dns       = _do_dns;
    follow_ptp   = _follow_ptp;
    dns_threads  = _dns_threads;
    dns_retries  = _dns_retries;
    dns_timeout  = _dns_timeout;

    queue.clear();
    found.clear();

    networks.clear();

    queue[_seed]="";
}

/**
 * This method attempts to guess what interface might be associated
 * with given route. It could be more than one.
 */
list<InterfaceData> SNMPCrawler::guessInterface(
    const IPRoute &r, const map<int, InterfaceData> &intf) const
{
    list<InterfaceData> res;
    map<int, InterfaceData>::const_iterator i;
    for(i=intf.begin(); i!=intf.end(); ++i)
    {
        for (list<InetAddrMask*>::const_iterator j=i->second.addr_mask.begin();
             j!=i->second.addr_mask.end(); ++j)
        {
            if ((*j)->belongs(r.getGateway())) 
            {
                res.push_back((*i).second);
                break;
            }
        }
    }
    return res;
}

bool SNMPCrawler::included(const InetAddr &a) const
{
    if (!limit_to_networks)
        return true; // no include list provided. All hosts are OK.
    
    // currently we allow the user to specify only ipv4 in the inlcude list
    if (a.isV6()) return true;

    for (vector<InetAddrMask>::const_iterator i=limit_to_networks->begin();
         i!=limit_to_networks->end(); ++i)
    {
        if ((*i).belongs(a)) return true;
    }
    return false;
}

bool SNMPCrawler::alreadyseen(const InetAddr &a) const
{
    return found.find(a) != found.end();
}

/**
 * loopback  :  All addresses on the net  127.0.0.0/255.0.0.0
 */
const InetAddrMask SNMPCrawler::LOOPBACK_NET(
    InetAddr::getLoopbackAddr(), InetAddr("255.0.0.0"));
const InetAddrMask SNMPCrawler::IPV6_LOOPBACK_NET(
    InetAddr::getLoopbackAddr(AF_INET6), InetAddr(AF_INET6, 128));

const InetAddr SNMPCrawler::PTP_NETMASK(InetAddr::getAllOnes());

bool SNMPCrawler::point2point(const InetAddrMask &n,
                              const InterfaceData& intf) const
{
    return *(n.getNetmaskPtr())==PTP_NETMASK ||
        point2point(intf);
}

bool SNMPCrawler::point2point(const InterfaceData& intf) const
{
    int itype = intf.snmp_type;
    for(unsigned int i=0;i<(sizeof(PTP_INTERFACE_TYPES)/sizeof(int));i++)
    {
        if(PTP_INTERFACE_TYPES[i]==itype)
            return true;
        else
            if(PTP_INTERFACE_TYPES[i]>itype)
                return false;
    }
    return false;
}

/**
 * Check if address one which we are ignoring.
 * This include hosts on loopback, special addresses
 * as "0.0.0.0".
 */
bool SNMPCrawler::special(const InetAddr &a) const 
{
    if (a.isAny()) return true;
    if (a.isV4()) return LOOPBACK_NET.belongs(a);
    if (a.isV6()) return IPV6_LOOPBACK_NET.belongs(a);
    return false;
}

bool SNMPCrawler::special(const InetAddrMask &n) const
{
    return n==LOOPBACK_NET || n==IPV6_LOOPBACK_NET ||
        n.getAddressPtr()->isBroadcast() ||
        n.getAddressPtr()->isMulticast() ||
        n.getAddressPtr()->isAny();
}

//TODO: multiple threads (via pool).
void SNMPCrawler::run_impl(Logger *logger,
                           std::atomic<bool> *stop_program)
{
    if (snmp_tmp_db==nullptr)
        snmp_tmp_db = new FWObjectDatabase();

    std::ostringstream str;
    time_t now=time(nullptr);
    str << "SNMPCrawler started at " << asctime(localtime(&now))
        << ". Seed host: " << (*(queue.begin())).first.toString() << "\n";
    *logger << str;

    snmp_tmp_db->destroyChildren();

    SNMP_discover_query q;
    do {
        CHECK_STOP_AND_RETURN;

        map<InetAddr,string>::iterator i=queue.begin();
        if(i==queue.end())
        {
            break;
        }
        InetAddr task = (*i).first;
        string task_phys_address = (*i).second;
        queue.erase(i);
        
        str << "\nProcessing " << task.toString() << "\n";

        *logger << str;
        

        // Now in task we have element to probe
        
        q.init(task.toString(), // fake host - IP in dotted notation
               community,
               snmp_retries,  
               snmp_timeout
        );
        
        CrawlerFind *res = new CrawlerFind();
        found[task] = *res; delete res;
        found[task].found_phys_addr = task_phys_address;
        try
        {
            q.fetchArpTable(logger,stop_program);
            found[task].have_snmpd = true;
        } catch(const FWException &ex)
        {
            // fetch failed
            str << ex.toString() << "\n";
            str << "Failed to fetch ARP table from " << task.toString();
            if (!ex.toString().empty()) str << " : " << ex.toString();
            str << "\n";
            *logger << str;
            
            continue;
        } catch (std::string s)
        {
            str << s << "\n";
            str << "Failed to fetch ARP table from " << task.toString();
            str << " : " << s;
            str << "\n";
            *logger << str;
            
            continue;
        }

        map<InetAddr, string>* at = q.getArpTable();    
        str << "Got " << long(at->size()) << " entries\n";
        *logger << str;
        
        int qplus=0, rplus=0, dplus=0;
        for(map<InetAddr, string>::iterator j=at->begin();j!=at->end();++j)
        {
            CHECK_STOP_AND_RETURN;

            InetAddr  c = (*j).first;
            string    pa = (*j).second;
            if (included(c) && !alreadyseen(c) && !special(c))
            {
                if(recursive)
                {
                    qplus++;
                    queue[c]=pa;
                } else
                {
                    rplus++;
                    found[c]=CrawlerFind();
                    found[c].found_phys_addr=pa;
                }
            } else
                dplus++;
        }
        if (qplus)
        {
            str << "Adding "   << qplus << " hosts to queue\n";
            *logger << str;
        }
        if (rplus)
        {
            str << "Adding "   << rplus << " hosts to results\n";
            *logger << str;
        }
        if (dplus)
        {
            str << "Skipping " << dplus
                << " hosts as duplicate, excluded or virtual\n";
            *logger << str;
        }

        set<InetAddr> interface_broadcasts;

        try
        {
            q.fetchInterfaces(logger,stop_program);
            found[task].have_snmpd = true;

            //cerr << "Copying list of interfaces" << endl;
            //map<int, InterfaceData> intf = q.getInterfaces();
            //cerr << "Done" << endl;

            map<int, InterfaceData>* intf = q.getInterfaces();
            map<int, InterfaceData>::iterator j;
            for (j=intf->begin(); j!=intf->end(); ++j)
            {
                // If interface is down or does not have ip address, it
                // will be ignored.
                if (!j->second.ostatus) continue;
                if (j->second.addr_mask.size()==0) continue;

                list<InetAddrMask*>::iterator n;
                for (n=j->second.addr_mask.begin();
                     n!=j->second.addr_mask.end(); ++n)
                {
                    InetAddrMask *net = *n;
                    const InetAddr *addr = net->getAddressPtr();
                    const InetAddr *netm = net->getNetmaskPtr();
                    (void)netm; //UNUSED
                    if (addr==nullptr) continue;

                    //InetAddrMask net(*addr, *netm);
                    interface_broadcasts.insert(
                        *(net->getBroadcastAddressPtr()));

                    if (!special(*net) &&
                        included(*(net->getAddressPtr())) &&
                        !point2point(*net, j->second))
                    {
                        str << "Network " << net->toString() << "\n";
                        *logger << str;

                        // NOTE: net is a pointer to InetAddrMask object
                        // created in fetchInterfaces when we filled
                        // map interfaces with InterfaceData objects.
                        // This object is destroyed when all InterfaceData
                        // objects are destroyed. Create a copy.

                        networks.insert(*net);
                    }
                }
            }
        } catch(FWException &ex)
        {
            // fetch failed
            str << ex.toString() << "\n";
            str << "Failed to fetch list of interfaces from "
                << task.toString() << "\n";
            *logger << str;
        }

        try
        {
            q.fetchSysInfo(logger,stop_program);
            found[task].have_snmpd = true;

            found[task].sysname  = q.getSysname  ();
            found[task].descr    = q.getDescr    ();
            found[task].contact  = q.getContact  ();
            found[task].location = q.getLocation ();
        } catch (const FWException &ex)
        {
            // fetch failed
            str << ex.toString() << "\n";
            str << "Failed to fetch sysinfo from " << task.toString() << "\n";
            *logger << str;
            
        }

        try
        {
            q.fetchRoutingTable(logger,stop_program);
            found[task].have_snmpd = true;
            
            vector<IPRoute>* routes = q.getRoutes();
            
            qplus=0; rplus=0; dplus=0; 
            int nplus=0;

            for (vector<IPRoute>::iterator j=routes->begin();
                 j!=routes->end(); ++j)
            {
                InterfaceData intf;
                bool have_intf;
                
                const InterfaceData& real_i = j->getInterface();
                intf      = real_i;
                have_intf = !intf.name.empty();
                
                InetAddrMask net(j->getDestination(),
                                 j->getNetmask());
                
                if (!have_intf)
                {
                    // No interface reported for this route by SNMP 
                    // we can try to guess it by route information.
                    // Since interface is used.
                    list<InterfaceData> gi =
                        guessInterface(*j, *(q.getInterfaces()));
                    
                    // From all resulting interfaces we select one
                    // using following rules:
                    //
                    // 1. If all interfaces are down we select any of them
                    // since this route will be ignored because of it.
                    //
                    // 2. From interfaces which are UP, we select first one
                    // with Point-to-Point attribute enabled.
                    //
                    list<InterfaceData>::const_iterator k;
                    for(k=gi.begin(); k!=gi.end(); ++k)
                    {
                        if(!have_intf)
                        {
                            // first interface ever found
                            intf=*k;
                            have_intf = true;
                        } else 
                        {
                            if ((*k).ostatus)
                            {
                                // Candidate is up
                                // Store it.
                                intf = *k;
                                have_intf = true;
                            } else
                            {
                                // Candidate is down.
                                // We take it into account only
                                // if everything found so far is down.
                                if (!intf.ostatus)
                                    intf = *k;
                            }
                        }
                        if(have_intf && intf.ostatus && point2point(intf))
                            break;
                    }
                    if (have_intf)
                        str << "Guessed that network " << net.toString()
                            << " is using interface " << intf.name << "\n";
                    *logger << str;
                    
                }

                // If route is associated with an interface which is down,
                // ignore it.
                if (have_intf && !intf.ostatus)
                {
                    str << "Skipping route for network " << net.toString()
                        << " which is associated with interface which is"
                        << " currently down.";
                    *logger << str;
                    continue; 
                }

                if (!special(net) && included(*(net.getAddressPtr())) )
                {
                    if (point2point(net, intf))
                    {
                        const InetAddr *c = net.getAddressPtr();

                        // For all addresses found in the routing table
                        // we must check if they are broadcast addresses
                        // for some of our interfaces, and if yes, ignore them.
                        // (see task #36520).

                        if (included(*c) && !alreadyseen(*c) && !special(*c) && 
                            !interface_broadcasts.count(*c))
                        {
                            if(recursive && follow_ptp)
                            {
                                qplus++;
                                queue[*c]="";
                            } else
                            {
                                rplus++;
                                found[*c] = CrawlerFind();
                            }
                        } else
                        {
                            dplus++;
                        }
                    } else
                    {
                        str << "Network " << net.toString()
                            << " found (via "
                            << string(((*j).isDirect())?"direct":"indirect") 
                            << " route).\n";
                        *logger << str;

                        networks.insert(
                            InetAddrMask(j->getDestination(), j->getNetmask()));
                        
                        nplus++;
                    }
                }

                InetAddr gw((*j).getGateway());
                if (included(gw) && !alreadyseen(gw) && 
                    !special(gw) && !interface_broadcasts.count(gw))
                {
                    bool isptp=point2point(net, intf);
                    if(recursive && (!isptp || (isptp && follow_ptp)))
                    {
                        qplus++;
                        queue[gw]="";
                    } else
                    {
                        rplus++;
                        found[gw]=CrawlerFind();
                    }
                }
            }

            if (qplus)
            {
                str << "Adding "   << qplus << " hosts to queue\n";
                *logger << str;
            }
            if (nplus)
            {
                str << "Adding "   << nplus << " networks to results\n";
                *logger << str;
            }
            if (rplus)
            {
                str << "Adding "   << rplus << " hosts to results\n";
                *logger << str;
            }
            if (dplus)
            {
                str << "Skipping " << dplus
                    << " hosts as duplicate, excluded or virtual\n";
                *logger << str;
            }

        } catch(const FWException &ex)
        {
            // fetch failed
            str << ex.toString() << "\n";
            str << "Failed to fetch routing table from "
                << task.toString() << "\n";
            *logger << str;
        }
        catch (...)
        {
            *logger << "Unsupported exception\n";
        }

        // We add interfaces _after_ fetching routing table,
        // since it's updates 'ext' attribute based on rounting
        // table.
        found[task].interfaces = *(q.getInterfaces());
        
    } while(recursive);

    if(do_dns)
        bacresolve_results(logger,stop_program);
    
    snmp_tmp_db->clear(); // at this point all children have been destroyed anyway

    now=time(nullptr);
    str << "SNMPCrawler - done at " << asctime(localtime(&now)) << "\n";
    *logger << str;
}

void SNMPCrawler::bacresolve_results(Logger *logger,
                                     std::atomic<bool> *)
{
    *logger << "Resolving names\n";
        
    set<InetAddr> resolv_set;
    for (map<InetAddr, CrawlerFind>::iterator j=found.begin();
         j!=found.end(); ++j)
    {
        HostEnt he = DNS::getHostByAddr((*j).first);

        found[(*j).first].dns_ok  = true;
        found[(*j).first].name    = he.name;
        found[(*j).first].aliases = he.aliases;
        *logger << ((*j).first).toString() << " : " << he.name << "\n";
    }

    *logger << "Finished.\n";
}


set<InetAddrMask> SNMPCrawler::getNetworks()
{
    return networks;
}

map<InetAddr, CrawlerFind> SNMPCrawler::getAllIPs()
{
    return found;
}

CrawlerFind::CrawlerFind()
{
    have_snmpd = false;
    dns_ok     = false;
}

CrawlerFind::~CrawlerFind()
{
}

void SNMP_interface_query::run_impl(Logger *logger, std::atomic<bool> *stop_program)
{
    fetchSysInfo(logger, stop_program);
    CHECK_STOP_AND_THROW_EXCEPTION;
    
    fetchInterfaces(logger, stop_program);
    
#if 0
    // See #2084 this takes forever on decides with large routing
    // tables.  Also it is unclear if routing table data is really
    // used to determine external interface. Besides, we only have
    // concept of external/internale on platforms that support
    // security levels (PIX) and there we guess levels by matching
    // addresses against RFC1918 and let the user user set levels
    // manually anyway.

    // We try to fetch routing table, to find
    // which interface is "external".
    // We do not fail if this query does not succeed.
    try
    {
        fetchRoutingTable(logger, stop_program);
    } catch(FWException &ex)
    {
        *logger << "Error fetching routing table, external interface will not be detected.\n";
    }
#endif
}

void SNMP_sysdesc_query::run_impl(Logger *logger,std::atomic<bool> *stop_program)
{
    fetchSysInfo(logger,stop_program);
}

void SNMP_discover_query::run_impl(Logger *logger,std::atomic<bool> *stop_program)
{
    fetchArpTable(logger,stop_program);
    if(fetch_inerfaces)
        fetchInterfaces(logger,stop_program);
}


#undef SNMP_CRAWLER_DEBUG

#else
#ifndef _WIN32
# warning ucd-snmp library not found - SNMP functionality will be disabled.
#endif
#endif

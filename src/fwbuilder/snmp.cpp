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

#ifdef HAVE_LIBSNMP

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

#include <fwbuilder/physAddress.h>
#include <fwbuilder/IPv4.h>

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
#include <fwbuilder/snmp.h>

#ifdef UCD_SNMP
#include <ucd-snmp/parse.h>
#endif

#include <typeinfo>
#include <algorithm>
#include <memory>
#include <sstream>

/**
 * Define this if you need extra debug output.
 */
#undef SNMP_CRAWLER_DEBUG

using namespace std;
using namespace libfwbuilder;

/*
 *  symbolic OID names - this requires MIB installed on the system,
 *  which may be a problem. In particular, we do not ship MIBs with
 *  windows package so the symbolic OID names won't work there at all

const char *SNMPQuery::SNMP_INTERFACE_ASTATUS= "interfaces.ifTable.ifEntry.ifAdminStatus"  ;
const char *SNMPQuery::SNMP_INTERFACE_OSTATUS= "interfaces.ifTable.ifEntry.ifOperStatus"   ;
const char *SNMPQuery::SNMP_INTERFACE_INDEX  = "interfaces.ifTable.ifEntry.ifIndex"        ;
const char *SNMPQuery::SNMP_INTERFACES_DESCR = "interfaces.ifTable.ifEntry.ifDescr"        ;
const char *SNMPQuery::SNMP_INTERFACES_PHYSA = "interfaces.ifTable.ifEntry.ifPhysAddress"  ;
const char *SNMPQuery::SNMP_INTERFACES_TYPE  = "interfaces.ifTable.ifEntry.ifType"         ;
const char *SNMPQuery::SNMP_ADDR_INDEX_TABLE = "ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex" ;
const char *SNMPQuery::SNMP_NMASK_TABLE      = "ip.ipAddrTable.ipAddrEntry.ipAdEntNetMask" ;
const char *SNMPQuery::SNMP_ADDR_TABLE       = "ip.ipAddrTable.ipAddrEntry.ipAdEntAddr"    ;
const char *SNMPQuery::SNMP_BCAST_TABLE      = "ip.ipAddrTable.ipAddrEntry.ipAdEntBcastAddr" ;
const char *SNMPQuery::SNMP_AT_TABLE_NET     = "at.atTable.atEntry.atNetAddress";
const char *SNMPQuery::SNMP_AT_TABLE_PHYS    = "at.atTable.atEntry.atPhysAddress";
const char *SNMPQuery::IP_MIB_PREFIX         = "IP-MIB::"             ;
const char *SNMPQuery::IP_MIB_ENTIFINDEX_PREFIX = "IP-MIB::ipAdEntIfIndex";

const char *SNMPQuery::RFC1213_MIB_PREFIX    = "RFC1213-MIB::"        ;
const char *SNMPQuery::RFC1213_MIB_PREFIX_IPROUTEDEST_PREFIX = 
						"RFC1213-MIB::ipRouteDest" ;

const char *SNMPQuery::SNMP_SYSNAME          = "system.sysName.0"     ;
const char *SNMPQuery::SNMP_SYSDESCR         = "system.sysDescr.0"    ;
const char *SNMPQuery::SNMP_CONTACT          = "system.sysContact.0"  ;
const char *SNMPQuery::SNMP_LOCATION         = "system.sysLocation.0" ;

const char *SNMPQuery::SNMP_ROUTE_DST_TABLE  = "ip.ipRouteTable.ipRouteEntry.ipRouteDest"    ;
const char *SNMPQuery::SNMP_ROUTE_NM_TABLE   = "ip.ipRouteTable.ipRouteEntry.ipRouteMask"    ;
const char *SNMPQuery::SNMP_ROUTE_TYPE_TABLE = "ip.ipRouteTable.ipRouteEntry.ipRouteType"    ;
const char *SNMPQuery::SNMP_ROUTE_IF_TABLE   = "ip.ipRouteTable.ipRouteEntry.ipRouteIfIndex" ;
const char *SNMPQuery::SNMP_ROUTE_GW_TABLE   = "ip.ipRouteTable.ipRouteEntry.ipRouteNextHop" ;
const int   SNMPQuery::SNMP_DIRECT_ROUTE     = 3;
*/


/* Compiled OIDs */
const char *SNMPQuery::SNMP_INTERFACE_ASTATUS= ".1.3.6.1.2.1.2.2.1.7"  ;
const char *SNMPQuery::SNMP_INTERFACE_OSTATUS= ".1.3.6.1.2.1.2.2.1.8"   ;
const char *SNMPQuery::SNMP_INTERFACE_INDEX  = ".1.3.6.1.2.1.2.2.1.1"        ;
const char *SNMPQuery::SNMP_INTERFACES_DESCR = ".1.3.6.1.2.1.2.2.1.2"        ;
const char *SNMPQuery::SNMP_INTERFACES_PHYSA = ".1.3.6.1.2.1.2.2.1.6"  ;
const char *SNMPQuery::SNMP_INTERFACES_TYPE  = ".1.3.6.1.2.1.2.2.1.3"         ;
const char *SNMPQuery::SNMP_ADDR_INDEX_TABLE = ".1.3.6.1.2.1.4.20.1.2" ;
const char *SNMPQuery::SNMP_NMASK_TABLE      = ".1.3.6.1.2.1.4.20.1.3" ;
const char *SNMPQuery::SNMP_ADDR_TABLE       = ".1.3.6.1.2.1.4.20.1.1"    ;
const char *SNMPQuery::SNMP_BCAST_TABLE      = ".1.3.6.1.2.1.4.20.1.4" ;
const char *SNMPQuery::SNMP_AT_TABLE_NET     = ".1.3.6.1.2.1.3.1.1.3";
const char *SNMPQuery::SNMP_AT_TABLE_PHYS    = ".1.3.6.1.2.1.3.1.1.2";

const char *SNMPQuery::SNMP_SYSNAME          = ".1.3.6.1.2.1.1.5.0"     ;
const char *SNMPQuery::SNMP_SYSDESCR         = ".1.3.6.1.2.1.1.1.0"    ;
const char *SNMPQuery::SNMP_CONTACT          = ".1.3.6.1.2.1.1.4.0"  ;
const char *SNMPQuery::SNMP_LOCATION         = ".1.3.6.1.2.1.1.6.0" ;

const char *SNMPQuery::SNMP_ROUTE_DST_TABLE  = ".1.3.6.1.2.1.4.21.1.1"    ;
const char *SNMPQuery::SNMP_ROUTE_NM_TABLE   = ".1.3.6.1.2.1.4.21.1.11"    ;
const char *SNMPQuery::SNMP_ROUTE_TYPE_TABLE = ".1.3.6.1.2.1.4.21.1.8"    ;
const char *SNMPQuery::SNMP_ROUTE_IF_TABLE   = ".1.3.6.1.2.1.4.21.1.2" ;
const char *SNMPQuery::SNMP_ROUTE_GW_TABLE   = ".1.3.6.1.2.1.4.21.1.7" ;
const int   SNMPQuery::SNMP_DIRECT_ROUTE     = 3;



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
static FWObjectDatabase snmp_tmp_db;


#include <iostream>

SNMPQuery::SNMPQuery(string h,string c, int retries_, long timeout_)
{
    hostname  = h  ;
    community = c  ;
    retries   = retries_;
    timeout   = timeout_;

    descr     = "" ;
    contact   = "" ;
    location  = "" ;
}

void SNMPQuery::init(string h, string c, int retries_, long timeout_)
{
    hostname  = h  ;
    community = c  ;
    retries   = retries_;
    timeout   = timeout_;

    descr     = "" ;
    contact   = "" ;
    location  = "" ;

    char *snmp_out_opt = (char*)("n");
    snmp_out_toggle_options(snmp_out_opt);
}

SNMPQuery::~SNMPQuery()
{
}

void SNMPQuery::fetchAll(Logger *logger,SyncFlag *stop_program) throw(FWException)
{
    if(community.empty())
        throw FWException("No SNMP community specified");

    if(hostname.empty())
        throw FWException("No SNMP hostname specified");

    SNMPConnection c(hostname, community);
    c.connect(retries, timeout);

    CHECK_STOP_AND_THROW_EXCEPTION
    
    fetchSysInfo   (logger,stop_program, &c);
    CHECK_STOP_AND_THROW_EXCEPTION
    
    fetchInterfaces(logger,stop_program, &c);
    CHECK_STOP_AND_THROW_EXCEPTION
    
    fetchArpTable  (logger,stop_program, &c);
    CHECK_STOP_AND_THROW_EXCEPTION

    fetchRoutingTable  (logger,stop_program, &c);
    CHECK_STOP_AND_THROW_EXCEPTION
}

void SNMPQuery::fetchArpTable(Logger *logger,SyncFlag *stop_program, SNMPConnection *connection) throw(FWException)
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
        CHECK_STOP_AND_THROW_EXCEPTION
        *logger << "Walking atPhysAddress table...\n";
        multimap<string, SNMPVariable* > pw=c->walk(SNMP_AT_TABLE_PHYS );
        CHECK_STOP_AND_THROW_EXCEPTION

        try
        {
//            int preflen = strlen(SNMP_AT_TABLE_NET);
            for(multimap<string, SNMPVariable* >::iterator j = nw.begin(); j!=nw.end(); ++j)
            {
                CHECK_STOP_AND_THROW_EXCEPTION
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
void SNMPQuery::fetchRoutingTable(Logger *logger,SyncFlag *stop_program, SNMPConnection *connection) throw(FWException)
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

    CHECK_STOP_AND_THROW_EXCEPTION
    
    vector<SNMPVariable*> v;
    try
    {
        // Get Indices and destinations
        
        multimap<string, SNMPVariable* > w=c->walk(SNMP_ROUTE_DST_TABLE);
        CHECK_STOP_AND_THROW_EXCEPTION
        try
        {
            for(multimap<string, SNMPVariable* >::iterator j = w.begin(); j!=w.end(); ++j)
            {
                CHECK_STOP_AND_THROW_EXCEPTION
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
 
                map<int,Interface>::iterator ici=interfaces.find(ifindex);
                
                Interface *route_intf;
                if(ici==interfaces.end())
                {
                    str << "Route references non-existing interface '"
                        << ifindex << "' in '"
                        << string(SNMP_ROUTE_IF_TABLE)+"."+rname
                        << "' variable. Ignoring interface reference.\n";
                    *logger << str;
                    
                    route_intf=NULL;
                } else
                {
#ifdef SNMP_CRAWLER_DEBUG
                    str << "Found interface # " << ifindex
                        << " associated with the route: "
                        << string(SNMP_ROUTE_IF_TABLE)+"."+rname << "\n";
                    *logger << str;
                    
#endif
                    
                    route_intf=&(*ici).second;
                }
                bool isdef= type!=SNMP_DIRECT_ROUTE &&
                    nm.getLength() == 0 && dst == InetAddr("0.0.0.0");

		if (isdef && route_intf) route_intf->setExt(true);

                IPRoute rt(dst, nm, gw, route_intf, type==SNMP_DIRECT_ROUTE);
                routes.push_back(rt);

                str << "route: " 
                    << rt.getDestination().toString() 
                    << "/" << rt.getNetmask().getLength() 
                    << "  gw " 
                    << rt.getGateway().toString()
                    << "  "
                    << string( (route_intf)?rt.getInterface()->getName():"(interface not found)")
                    << string( (route_intf && rt.getInterface()->isExt())?"(Ext)":"")
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

void SNMPQuery::fetchInterfaces(Logger *logger,SyncFlag *stop_program, SNMPConnection *connection) throw(FWException)
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
    
    CHECK_STOP_AND_THROW_EXCEPTION
            
    multimap<string, SNMPVariable* > w;
    vector<SNMPVariable*>            v;
    try
    {
        *logger << "Index of interface addresses.\n";

        map<long, list<string> > addr;
        w=c->walk(SNMP_ADDR_INDEX_TABLE);
        CHECK_STOP_AND_THROW_EXCEPTION

        for(multimap<string, SNMPVariable* >::iterator j = w.begin(); j!=w.end(); ++j)
        {
            CHECK_STOP_AND_THROW_EXCEPTION
            if((*j).second->type!=SNMPVariable::snmp_int)
            {
                str << "unexpected result type in '"
                    << SNMP_ADDR_INDEX_TABLE << "' table. Skipping it.\n";
                *logger << str;
                
                continue;
            }
            long ind = SNMPVariable::var2Int((*j).second);

	    string iname=(*j).first.substr(strlen(SNMP_ADDR_INDEX_TABLE)+1);

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

        for(multimap<string, SNMPVariable* >::iterator j = w.begin(); j!=w.end(); ++j)
            delete (*j).second;
        w.clear();

        *logger << "Full interfaces info.\n";

        w=c->walk(SNMP_INTERFACE_INDEX);
        CHECK_STOP_AND_THROW_EXCEPTION
        for(multimap<string, SNMPVariable* >::iterator j = w.begin(); j!=w.end(); ++j)
        {
            CHECK_STOP_AND_THROW_EXCEPTION

            // Get index
            long ifindex = SNMPVariable::var2Int((*j).second);
            char oid[1024];

            // Get admin status
            sprintf(oid,"%s.%ld", SNMP_INTERFACE_ASTATUS , ifindex);
            v=c->get(oid);
            if(v.size()!=1)
                throw FWException(string("Unexpected response length for OID: ")+oid);
            int astatus = SNMPVariable::varList2Int(v);
            SNMPVariable::freeVarList(v);

            if(!astatus)
            {
                str << "Interface #" << ifindex
                    << " disabled by admin - skipping.\n";
                *logger << str;
                
                continue;
            }
            
            // Get operational status
            sprintf(oid,"%s.%ld", SNMP_INTERFACE_OSTATUS , ifindex);
            v=c->get(oid);
            if(v.size()!=1)
                throw FWException(string("Unexpected response length for OID: ")+oid);
            int ostatus = SNMPVariable::varList2Int(v);
            SNMPVariable::freeVarList(v);


/* gather all information for interface ifindex and create Interface object */

            list<string> &addlist=addr[ifindex];
            if (addlist.empty())
            {
                str << "Interface #" << ifindex << " has no IP address.\n";
                *logger << str;
                
//                continue;
            }
/*
            map<long, string>::const_iterator ici=addr.find(ifindex);
            if(ici==addr.end())
            {
                *logger << "Interface #" << ifindex << " does not have IP address. Skipping it.\n";
                continue;
            } else
            {
                aprefix=(*ici).second;
            }
*/
            // Get desriptions
            sprintf(oid,"%s.%ld", SNMP_INTERFACES_DESCR , ifindex);
            v=c->get(oid);
            string descr = SNMPVariable::varList2String(v);
            SNMPVariable::freeVarList(v);

            // Get physical address
            sprintf(oid,"%s.%ld", SNMP_INTERFACES_PHYSA , ifindex);
            v=c->get(oid);
            if(v.size()!=1)
                throw FWException(string("Unexpected response length for OID: ")+oid);
            if(v[0]->type!=SNMPVariable::snmp_string)
                throw FWException(string("Unexpected response type for: ")+oid);
            string physa = dynamic_cast<SNMPVariable_String*>(v[0])->toHexString();
            SNMPVariable::freeVarList(v);

            // Get type
            sprintf(oid,"%s.%ld", SNMP_INTERFACES_TYPE , ifindex);
            v=c->get(oid);
            if(v.size()!=1)
                throw FWException(string("Unexpected response length for OID: ")+oid);                
            int itype = SNMPVariable::varList2Int(v);
            SNMPVariable::freeVarList(v);

            interfaces[ifindex]=Interface();
            interfaces[ifindex].setName(descr);
            interfaces[ifindex].setOStatus(ostatus);
            interfaces[ifindex].setInterfaceType(itype);
            interfaces[ifindex].setExt(false);

            snmp_tmp_db.add( &interfaces[ifindex] );

            if (physa!="")
                interfaces[ifindex].setPhysicalAddress(physa);

            for (list<string>::iterator ali=addlist.begin(); ali!=addlist.end(); ali++)
            {
                string aprefix= *ali;

                // get netmasks
                CHECK_STOP_AND_THROW_EXCEPTION
                v=c->get(string(SNMP_NMASK_TABLE)+"."+aprefix);
                if(v.size()!=1)
                    throw FWException("Can't get netmask");
                if(v[0]->type!=SNMPVariable::snmp_ipaddr)
                    throw FWException("Wrong return type for netmask");
                string nm = dynamic_cast<SNMPVariable_IPaddr*>(v[0])->getNetmaskValue().toString();
                SNMPVariable::freeVarList(v);
            
                // get addresss
                v=c->get(string(SNMP_ADDR_TABLE)+"."+aprefix);
                if(v.size()!=1)
                    throw FWException("Can't get IP address");
                if(v[0]->type!=SNMPVariable::snmp_ipaddr)
                    throw FWException("Wrong return type for IP address");
                string ad = dynamic_cast<SNMPVariable_IPaddr*>(v[0])->getInetAddrValue().toString();
                SNMPVariable::freeVarList(v);

                v=c->get(string(SNMP_BCAST_TABLE)+"."+aprefix);
                if(v.size()!=1)
                    throw FWException("Can't get broadcast address");
//                long bcast_bits = SNMPVariable::varList2Int(v);
                SNMPVariable::freeVarList(v);

                IPv4 *ipaddr=interfaces[ifindex].addIPv4();
                ipaddr->setAddress(InetAddr(ad));
                ipaddr->setNetmask(InetAddr(nm));

                str << "Adding interface #" << ifindex 
                    << ": " << ad << "/" << nm
                    << "\n";
                *logger << str;
                
            }
        } // index walk.
        for(multimap<string, SNMPVariable* >::iterator j = w.begin(); j!=w.end(); ++j)
            delete (*j).second;
        w.clear();

    } catch (FWException &ex)
    {

        SNMPVariable::freeVarList(v);

        for(multimap<string, SNMPVariable* >::iterator j = w.begin(); j!=w.end(); ++j)
            delete (*j).second;
        w.clear();

        if(!connection)
            delete c;
        // If program was stopped, show this, rather
        // than error.
        throw;
    }
    if(!connection)
        delete c;
    *logger << "Done fetching interfaces\n";
}

void SNMPQuery::fetchSysInfo(Logger *logger,SyncFlag *stop_program, SNMPConnection *connection) throw(FWException)
{
    std::ostringstream str;
    descr     = "" ;
    contact   = "" ;
    location  = "" ;
    sysname   = "" ;

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
        sysname    = SNMPVariable::varList2String(v);
        SNMPVariable::freeVarList(v);
        CHECK_STOP_AND_THROW_EXCEPTION

        *logger << "Getting Description\n";
        v=c->get(SNMP_SYSDESCR);
        descr    = SNMPVariable::varList2String(v);
        SNMPVariable::freeVarList(v);
        CHECK_STOP_AND_THROW_EXCEPTION
        
        *logger << "Getting Location\n";
        v=c->get(SNMP_LOCATION);
        location = SNMPVariable::varList2String(v);
        SNMPVariable::freeVarList(v);
        CHECK_STOP_AND_THROW_EXCEPTION
        
        *logger << "Getting Contact Info\n";
        v=c->get(SNMP_CONTACT);
        contact  = SNMPVariable::varList2String(v);
        SNMPVariable::freeVarList(v);
        CHECK_STOP_AND_THROW_EXCEPTION
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

const vector<IPRoute> &SNMPQuery::getRoutes()
{
    return routes;
}

const map<InetAddr, string> &SNMPQuery::getArtpTable()
{
    return arptable;
}

const map<int, Interface> &SNMPQuery::getInterfaces()
{
    return interfaces;
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
    connected    = false ;
    session_data = NULL  ;
    peer         = p     ;
    community    = c     ;
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

void SNMPConnection::connect(int retries, long timeout) throw(FWException)
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

void SNMPConnection::disconnect() throw(FWException)
{
    if(!connected)
        throw FWException("SNMPSession: already disconnected");

    snmp_close(session);

    delete session_data->peername;
    delete session_data->community;
    delete session_data;
    session_data = NULL;
    connected    = false;
}

multimap<string, SNMPVariable* > SNMPConnection::walk(const string &variable) throw(FWException)
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
        struct snmp_pdu *response = NULL;
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
                        (memcmp(root, vars->name, rootlen * sizeof(oid))!=0)) {
                        /* not part of this subtree */
                        running = false;
                        continue;
                    }
                    char n[MAX_NAME_LEN];
#ifdef HAVE_SNPRINT_OBJID
                    snprint_objid (n, sizeof(n), vars->name, vars->name_length);
#else
                    sprint_objid (n, vars->name, vars->name_length);
#endif
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
            if (response != NULL)
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

vector<SNMPVariable*> SNMPConnection::get(const string &variable) throw(FWException)
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

SNMPVariable *SNMPVariable::create(struct variable_list *vars) throw(FWException)
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
        return new SNMPVariable_OID(*vars->val.objid);
    case ASN_IPADDRESS:
        return new SNMPVariable_IPaddr(vars->val.string, vars->val_len);
    default: 
        char x[32];
        sprintf(x,"%d", (int)vars->type);
        throw FWException(string("Unknown SNMP variable type: ")+x);
    }
}

string SNMPVariable_Int::toString()
{
    char x[32];
    sprintf(x,"%ld", value);
    return x;
}

string SNMPVariable_Bits::toString()
{
    string res="[";
    for(size_t i=0;i<len;i++)
    {
        char x[8];
        //TODO: now we print it in hex
        // we should print it in binary.
        sprintf(x,"%2x", (unsigned int)value[i]);
        res+=x;
    }
    res+="]";
    return res;
}

string SNMPVariable_IPaddr::toString()
{
    string res="SNMP InetAddr/Netmask[";
    for(size_t i=0;i<len;i++)
    {
        if(i) 
            res+=".";
        char x[8];
        sprintf(x,"%d", (unsigned int)value[i]);
        res+=x;
    }
    res+="]";
    return res;
}

InetAddr SNMPVariable_IPaddr::getInetAddrValue() throw(FWException)
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

InetAddr SNMPVariable_IPaddr::getNetmaskValue() throw(FWException)
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
        sprintf(buf,"%02X",(unsigned int)c);
        res+=buf;
    }
    return res;
}


string SNMPVariable_Counter64::toString()
{
    char x[70];
    sprintf(x,"[%ld:%ld]", (long)low, (long)high);
    return x;
}

string SNMPVariable_OID::toString()
{
    char x[32];
    sprintf(x,"%ld", (long)value);
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

long SNMPVariable::var2Int(SNMPVariable *var) throw(FWException)
{
    if(var->type != SNMPVariable::snmp_int)
        throw FWException("Could not extract integer from non-int SNMP variable.");
    return dynamic_cast<SNMPVariable_Int *>(var)->getIntValue();
}

long SNMPVariable::varList2Int(vector<SNMPVariable*> &v) throw(FWException)
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
                         bool _skip_virtual,
                         bool _do_dns,
                         bool _follow_ptp,
                         unsigned int _dns_threads,
                         int  _snmp_retries, 
                         long _snmp_timeout,
			 int  _dns_retries,
			 int  _dns_timeout,
                         const vector<InetAddrMask> *_include)
{
    init(_seed, _community, _recursive, _skip_virtual, 
         _do_dns, _follow_ptp, _dns_threads, 
	 _snmp_retries,  _snmp_timeout, _dns_retries, _dns_timeout, 
	 _include);
}

SNMPCrawler::~SNMPCrawler()
{
}

void SNMPCrawler::init(const InetAddr &_seed, 
		       const string &_community,
		       bool _recursive,
                       bool _skip_virtual,
                       bool _do_dns,
                       bool _follow_ptp,
                       unsigned int _dns_threads,
		       int _snmp_retries, 
		       long _snmp_timeout,
		       int  _dns_retries,
		       int  _dns_timeout,
		       const vector<InetAddrMask> *_include)
{
    include      = _include      ;
    community    = _community    ;
    snmp_retries = _snmp_retries ;
    snmp_timeout = _snmp_timeout ;
    recursive    = _recursive    ;
    skip_virtual = _skip_virtual ;
    do_dns       = _do_dns       ;
    follow_ptp   = _follow_ptp   ;
    dns_threads  = _dns_threads  ;
    dns_retries  = _dns_retries  ;
    dns_timeout  = _dns_timeout  ;

    queue.clear();
    found.clear();
    networks.clear();

    queue[_seed]="";
}

/**
 * This method attempts to guess what interface might be associated
 * with given route. It could be more than one.
 */
set<Interface> SNMPCrawler::guessInterface(const IPRoute &r, const map<int, Interface> &intf) const
{
    set<Interface> res;
    for(map<int, Interface>::const_iterator i=intf.begin(); i!=intf.end(); ++i)
    {
        InetAddrMask iface_net((*i).second.getAddress(), (*i).second.getNetmask());
        if (iface_net.belongs(r.getGateway()))
            res.insert((*i).second);
    }
    return res;
}

bool SNMPCrawler::included(const InetAddr &a) const
{
    if(!include)
        return true; // no include list provided. All hosts are OK.
    
    for(vector<InetAddrMask>::const_iterator i=include->begin(); i!=include->end(); ++i) {
        if((*i).belongs(a)) 
            return true;
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
const InetAddrMask SNMPCrawler::LOOPBACK_NET(InetAddr::getLoopbackAddr(),
                                          InetAddr("255.0.0.0"));
const InetAddr SNMPCrawler::PTP_NETMASK(InetAddr::getAllOnes());
const InetAddr SNMPCrawler::ZERO_IP("0.0.0.0");

bool SNMPCrawler::isvirtual(const InetAddr &addr, const string &pa) const 
{
    if(!pa.length())
        return false;

    for(map<InetAddr, CrawlerFind>::const_iterator i=found.begin(); i!=found.end(); ++i)
    {
        const CrawlerFind &c = (*i).second ;
        
        for(map<int, Interface>::const_iterator j=c.interfaces.begin(); j!=c.interfaces.end(); ++j)
        {
            try
            {
                physAddress *paddr = (*j).second.getPhysicalAddress();
                if (paddr!=NULL &&
                    pa == paddr->getPhysAddress() &&
                    addr != (*j).second.getAddress())
                    return true;
            } catch(FWException &ex)
            {
                // Exception caused by interface having bad IP address.
                // we just skip this interface and go on
            }
        }
    }
    
    return false;
}

bool SNMPCrawler::point2point(const InetAddrMask &n, const Interface *intf) const
{
    return n.getNetmask()==PTP_NETMASK || point2point(intf);
}

bool SNMPCrawler::point2point(const Interface *intf) const
{
    if(intf)
    {
        int itype=intf->getInterfaceType();
        for(unsigned int i=0;i<(sizeof(PTP_INTERFACE_TYPES)/sizeof(int));i++)
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
    return LOOPBACK_NET.belongs(a) || ZERO_IP==a;
}

bool SNMPCrawler::special(const InetAddrMask &n) const
{
    return n==LOOPBACK_NET ||
        n.getAddress().isBroadcast() ||
        n.getAddress().isMulticast() ||
        n.getAddress().isAny();
}

//TODO: multiple threads (via pool).
void SNMPCrawler::run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException)
{
    std::ostringstream str;
    time_t now=time(NULL);
    str << "SNMPCrawler started at " << asctime(localtime(&now))
        << ". Seed host: " << (*(queue.begin())).first.toString() << "\n";
    *logger << str;
    
    snmp_tmp_db.destroyChildren();

    SNMP_discover_query q;
    do {
        CHECK_STOP_AND_RETURN

        map<InetAddr,string>::iterator i=queue.begin();
        if(i==queue.end())
        {
            break;
        }
        InetAddr task           = (*i).first  ;
        string task_phys_address = (*i).second ;
        queue.erase(i);
        
        str << "\nProcessing " << task.toString() << "\n";
        *logger << str;
        

        // Now in task we have element to probe
        
        q.init(task.toString(), // fake host - IP in dotted notation
               community,
               snmp_retries,  
               snmp_timeout
        );
        
        CrawlerFind *res=new CrawlerFind();
        found[task] = *res; delete res;
        found[task].found_phys_addr = task_phys_address ;
        try
        {
            q.fetchArpTable(logger,stop_program);
            found[task].have_snmpd = true;
        } catch(const FWException &ex)
        {
            // fetch failed
            str << "Failed to fetch ARP table from " << task.toString();
            if (!ex.toString().empty()) str << " : " << ex.toString();
            str << "\n";
            *logger << str;
            
            continue;
        } catch (std::string s) {
            str << "Failed to fetch ARP table from " << task.toString();
            str << " : " << s;
            str << "\n";
            *logger << str;
            
            continue;
        }

        map<InetAddr, string> at=q.getArtpTable();    
        str << "Got " << long(at.size()) << " entries\n";
        *logger << str;
        

        int qplus=0, rplus=0, dplus=0;
        for(map<InetAddr, string>::iterator j=at.begin();j!=at.end();++j)
        {
            CHECK_STOP_AND_RETURN

            InetAddr  c = (*j).first;
            string    pa = (*j).second;
            if(included(c) && !alreadyseen(c) && !isvirtual(c,pa) && !special(c))
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
            str << "Adding "   << qplus << " hosts to queue\n"   ;
            *logger << str;
            
        }
        if (rplus)
        {
            str << "Adding "   << rplus << " hosts to results\n" ;
            *logger << str;
            
        }
        if (dplus)
        {
            *logger << "Skipping " << dplus << " hosts as duplicate, excluded or virtual\n" ;
            *logger << str;
            
        }

        set<InetAddr> interface_broadcasts;

        try
        {
            q.fetchInterfaces(logger,stop_program);
            found[task].have_snmpd = true;

            map<int, Interface> intf = q.getInterfaces();
            for(map<int, Interface>::iterator j=intf.begin(); j!=intf.end(); ++j)
            {
                // If interface is down it will be ignored.
                if(!(*j).second.isUp())
                    continue;

                //InetAddrMask net=(*j).second.getInetAddrMask();
                InetAddrMask net((*j).second.getAddress(),
                              (*j).second.getNetmask());
                interface_broadcasts.insert(net.getBroadcastAddress());

                if(!special(net) && included(net.getAddress()) && !point2point(net, NULL))
                {
                    str << "Network " << net.toString() << " found.\n";
                    *logger << str;
                    
                    networks.insert(net);
                }
            }
//        } catch(const FWException &ex)
        } catch(FWException &ex)
        {
            // fetch failed
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
        } catch(const FWException &ex)
        {
            // fetch failed
            str << "Failed to fetch sysinfo from " << task.toString() << "\n";
            *logger << str;
            
        }

        try
        {
            q.fetchRoutingTable(logger,stop_program);
            found[task].have_snmpd = true;
            
            vector<IPRoute> routes = q.getRoutes();
            
            qplus=0; rplus=0; dplus=0; 
            int nplus=0;

            for(vector<IPRoute>::iterator j=routes.begin(); j!=routes.end(); ++j)
            {
                Interface intf;
                bool have_intf;
                
                const Interface *real_i=(*j).getInterface();
                if(real_i)
                {
                    intf      = *real_i;
                    have_intf = true;
                } else
                {
                    have_intf = false;
                }
                
                InetAddrMask net((*j).getDestination(), (*j).getNetmask());
                
                if(!have_intf)
                {
                    // No interface reported for this route by SNMP 
                    // by we can try to guess it by route information.
                    // Since interface is used.
                    set<Interface> gi=guessInterface(*j, q.getInterfaces());
                    
                    // From all resulting interfaces we select one
                    // using following rules:
                    //
                    // 1. If all interfaces are down we select any of them
                    // since this route will be ignored because of it.
                    //
                    // 2. From interfaces which are UP, we select first one
                    // with Point-to-Point attribute enabled.
                    //
                    for(set<Interface>::const_iterator k=gi.begin(); k!=gi.end(); ++k)
                    {
                        if(!have_intf)
                        {
                            // first interface ever found
                            intf=*k;
                            have_intf = true;
                        } else 
                        {
                            if((*k).isUp())
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
                                if(!intf.isUp())
                                    intf = *k;
                            }
                        }
                        if(have_intf && intf.isUp() && point2point(&intf))
                            break;
                    }
                    if(have_intf)
                        str << "Guessed that network " << net.toString()
                            << " is using interface " << intf.getName() << "\n";
                    *logger << str;
                    
                }

                // If route is associated with an interface which is down,
                // ignore it.
                if(have_intf && !intf.isUp())
                {
                    str << "Skipping route for network " << net.toString()
                        << " which is associated with interface which is currently down.\n";
                    *logger << str;
                    
                    continue; 
                }

                if(!special(net) && included(net.getAddress()) )
                {
                    if(point2point(net, NULL))
                    {
                        InetAddr c=net.getAddress();

                        // For all addresses found in the routing table
                        // we must check if they are broadcast addresses
                        // for some of our interfaces, and if yes, ignore them.
                        // (see task #36520).

                        if(included(c) && !alreadyseen(c) && 
                           !isvirtual(c,"") && !special(c) && 
                           !interface_broadcasts.count(c))
                        {
                            if(recursive && follow_ptp)
                            {
                                qplus++;
                                queue[c]="";
                            } else
                            {
                                rplus++;
                                found[c]=CrawlerFind();
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
                        
                        
                        networks.insert(net);
                        nplus++;
                    }
                }

                InetAddr gw((*j).getGateway());
                if(included(gw) && !alreadyseen(gw) && !isvirtual(gw, "") && !special(gw) && !interface_broadcasts.count(gw))
                {
                    bool isptp=point2point(net, &intf);
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
                str << "Adding "   << qplus
                    << " hosts to queue\n"   ;
                *logger << str;
                
            }
            if (nplus)
            {
                str << "Adding "   << nplus
                    << " networks to results\n"   ;
                *logger << str;
                
            }
            if (rplus)
            {
                str << "Adding "   << rplus
                    << " hosts to results\n" ;
                *logger << str;
                
            }
            if (dplus)
            {
                str << "Skipping " << dplus
                    << " hosts as duplicate, excluded or virtual\n" ;
                *logger << str;
                
            }

        } catch(const FWException &ex)
        {
            // fetch failed
            str << "Failed to fetch routing table from "
                << task.toString() << "\n";
            *logger << str;
            
        }

        // We add interfaces _after_ fetching routing table,
        // since it's updates 'ext' attribute based on rounting
        // table.
        found[task].interfaces=q.getInterfaces();
        
    } while(recursive);

    if(skip_virtual)
        remove_virtual(logger,stop_program);
    
    if(do_dns)
        bacresolve_results(logger,stop_program);
    
    snmp_tmp_db.clear(); // at this point all children have been destroyed anyway

    now=time(NULL);
    str << "SNMPCrawler - done at " << asctime(localtime(&now)) << "\n";
    *logger << str;
}

void SNMPCrawler::remove_virtual(Logger *logger,SyncFlag *stop_program) throw(FWException)
{
    *logger << "Removing virtual IPs.\n";
    for(map<InetAddr, CrawlerFind>::iterator j=found.begin(); j!=found.end(); ++j)
        if(isvirtual((*j).first, (*j).second.found_phys_addr))
            found.erase(j);
}

void SNMPCrawler::bacresolve_results(Logger *logger,SyncFlag *stop_program) throw(FWException)
{
    *logger << "Resolving names\n";
        
    set<InetAddr> resolv_set;
    for(map<InetAddr, CrawlerFind>::iterator j=found.begin(); j!=found.end(); ++j)
        resolv_set.insert((*j).first);
    DNS_bulkBackResolve_query backresq(resolv_set, dns_threads,
				       dns_retries,dns_timeout );
    resolv_set.clear();
        
    try
    {
        backresq.run_impl(logger,stop_program);
    } catch(const FWException &ex)
    {
        *logger << "DNS resolver reported errors.\n";
    }
    *logger << "Finished.\n";

    map<InetAddr, HostEnt> resolv_res=backresq.getResult();
    for(map<InetAddr, HostEnt>::iterator j=resolv_res.begin(); j!=resolv_res.end(); ++j)
    {
        found[(*j).first].dns_ok  = true;
        found[(*j).first].name    = (*j).second.name;
        found[(*j).first].aliases = (*j).second.aliases;
    }
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

void SNMP_interface_query::run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException)
{
    fetchInterfaces(logger,stop_program);
    
    // We try to fetch routing table, to find
    // which interface is "external".
    // We do not fail if this query does not succeed.
    try
    {
        fetchRoutingTable(logger,stop_program);
    } catch(FWException &ex)
    {
        *logger << "Error fetching routing table, external interface will not be detected.\n";
    }
}

void SNMP_sysdesc_query::run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException)
{
    fetchSysInfo(logger,stop_program);
}

void SNMP_discover_query::run_impl(Logger *logger,SyncFlag *stop_program) throw(FWException)
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

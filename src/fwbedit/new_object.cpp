/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: fwbedit.cpp 429 2008-07-31 07:03:39Z vadim $

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

#include "fwbuilder/Constants.h"

#include <qsettings.h>
#include <qdatetime.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <deque>
#include <vector>
#include <string>


#ifndef _WIN32
#  include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>

#include "fwbuilder/Resources.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Group.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"

#include <assert.h>

#include "fwbedit.h"


using namespace libfwbuilder;
using namespace std;


std::map<string,string>  systemGroupPaths;
std::map<string,string>  systemGroupTypes;


void initConstants()
{
    systemGroupPaths[Library::TYPENAME] = "";

    systemGroupPaths[IPv4::TYPENAME] = "Objects/Addresses";
    systemGroupPaths[IPv6::TYPENAME] = "Objects/Addresses";
    systemGroupPaths[DNSName::TYPENAME] = "Objects/DNS Names";
    systemGroupPaths[AddressTable::TYPENAME] = "Objects/Address Tables";
    systemGroupPaths[AddressRange::TYPENAME] = "Objects/Address Ranges";
    systemGroupPaths[ObjectGroup::TYPENAME] = "Objects/Groups";
    systemGroupPaths[Host::TYPENAME] = "Objects/Hosts";
    systemGroupPaths[Network::TYPENAME] = "Objects/Networks";
    systemGroupPaths[NetworkIPv6::TYPENAME] = "Objects/Networks";

    systemGroupPaths[ServiceGroup::TYPENAME] = "Services/Groups";
    systemGroupPaths[CustomService::TYPENAME] = "Services/Custom";
    systemGroupPaths[IPService::TYPENAME] = "Services/IP";
    systemGroupPaths[ICMPService::TYPENAME] = "Services/ICMP";
    systemGroupPaths[TCPService::TYPENAME] = "Services/TCP";
    systemGroupPaths[UDPService::TYPENAME] = "Services/UDP";
    systemGroupPaths[TagService::TYPENAME] = "Services/TagServices";
    systemGroupPaths[UserService::TYPENAME] = "Services/Users";

    systemGroupPaths[Firewall::TYPENAME] = "Firewalls";
    systemGroupPaths[Cluster::TYPENAME] = "Clusters";

    systemGroupPaths[Interval::TYPENAME] = "Time";


    systemGroupTypes["Objects/Addresses"] = ObjectGroup::TYPENAME;
    systemGroupTypes["Objects/Addresses"] = ObjectGroup::TYPENAME;
    systemGroupTypes["Objects/DNS Names"] = ObjectGroup::TYPENAME;
    systemGroupTypes["Objects/Address Tables"] = ObjectGroup::TYPENAME;
    systemGroupTypes["Objects/Address Ranges"] = ObjectGroup::TYPENAME;
    systemGroupTypes["Objects/Groups"] = ObjectGroup::TYPENAME;
    systemGroupTypes["Objects/Hosts"] = ObjectGroup::TYPENAME;
    systemGroupTypes["Objects/Networks"] = ObjectGroup::TYPENAME;
    systemGroupTypes["Objects/Networks"] = ObjectGroup::TYPENAME;

    systemGroupTypes["Services/Groups"] = ServiceGroup::TYPENAME;
    systemGroupTypes["Services/Custom"] = ServiceGroup::TYPENAME;
    systemGroupTypes["Services/IP"] = ServiceGroup::TYPENAME;
    systemGroupTypes["Services/ICMP"] = ServiceGroup::TYPENAME;
    systemGroupTypes["Services/TCP"] = ServiceGroup::TYPENAME;
    systemGroupTypes["Services/UDP"] = ServiceGroup::TYPENAME;
    systemGroupTypes["Services/TagServices"] = ServiceGroup::TYPENAME;
    systemGroupTypes["Services/Users"] = ServiceGroup::TYPENAME;

    systemGroupTypes["Firewalls"] = ObjectGroup::TYPENAME;
    systemGroupTypes["Clusters"] = ObjectGroup::TYPENAME;

    systemGroupTypes["Time"] = IntervalGroup::TYPENAME;

}

void notEnoughAttributesError()
{
    cerr << "Incorrect number of attributes for given object type"
         << endl;
    exit(-1);
}

bool testIPv4(string s)
{
    bool res=false;
    try
    {
        InetAddr( s.c_str() );
        res=true;
    } catch (FWException &ex)
    {    }
    return res;
}

bool testIPv6(string s)
{
    bool res=false;
    try
    {
        InetAddr(AF_INET6, s.c_str());
        res=true;
    } catch (FWException &ex)
    {    }
    return res;
}

void invalidIPv4(string s)
{
    if (!testIPv4(s))
    {
        cout << "\"" << s << "\" - invalid IPv4 address." << endl;
        
        exit(0);
    }
}

void invalidIPv6(string s)
{
    if (!testIPv6(s))
    {
        cout << "\"" << s << "\" - invalid IPv6 address." << endl;
        
        exit(0);
    }
}

void invalidIPv4Orv6(string s)
{
    if (!testIPv4(s) && !testIPv6(s))
    {
        cout << "\"" << s << "\" - invalid IP address." << endl;

        exit(0);
    }
}

bool testPlatform(const string &pl, const string &os)
{
    vector<string> platforms = Resources::getListOfPlatforms();
    operands lst;
    string str;
    if (platforms.empty() || ( platforms.size()==1 && platforms.front()=="unknown" ))
    {
        cout << "Failed to load list of supported platforms" << endl;
        exit(1);
    }
    for (vector<std::string>::iterator i=platforms.begin();i!=platforms.end();i++)
    {
        
        string sos=Resources::platform_res[*i]->getResourceStr("/FWBuilderResources/Target/supported_os");
        if (sos.empty()) return false;
        if (*i!="unknown")
        {
            if (*i==pl )
            {
                int n = splitStr(',',sos,&lst);
                for (int i=0;i<n;i++)
                {
                    str=getNextOpt(lst);
                    if (str==os) return true;
                }
            }
        }
    }  
    
    return false;
}


FWObject* createObject(FWObjectDatabase *objdb,
                       const string &type, const string &parent)
{
    FWObject* obj = nullptr;
    string path;

    obj = objdb->create(type);

    // parent can be either full path to the parent object or
    // just the name of the library. If there are no "/" in the string,
    // then this is the latter case.
    string::size_type n = parent.find("/");
    if (n==string::npos)
        path = "/" + parent + "/" + systemGroupPaths[type];
    else
        path = parent;

    list<FWObject*> parents;
    findObjects(path, objdb, parents);
    if (parents.size())
    {
        FWObject *parent_obj = parents.front();
        if (parent_obj && parent_obj->validateChild(obj))
            parent_obj->add(obj);
    }

    return obj;
}

/*
 * _modObject takes a copy of ops (so the original can be used several times)
 */
void _modObject(FWObject *nobj, const string &comment_txt, operands ops)
{
    string group;
    string addr1;
    string addr2;
    string dnsrec;
    string runtime;
    string path;
    string lib;
    string time1;
    string time2;
    string date1;
    string date2;
    string day1;
    string day2;
    string platform;
    string hostOS;
    string management;
    string addr3;
    string addr4;
    string tcpflags_mask;
    string tcpflags_bits;
    string protocol;
    string bitmap;
    string ICMPtype;
    string ICMPcode;
    string security;
    string addrtype;

    string objtype = nobj->getTypeName();

    if (!comment_txt.empty()) nobj->setComment(comment_txt);

    if (ops.size()==0) return;

    if (objtype==IPv4::TYPENAME || objtype==IPv6::TYPENAME)
    {
        if (objtype==IPv4::TYPENAME)
        {
            addr1 = getNextOpt(ops);
            invalidIPv4(addr1);
            if (ops.size()>0)
            {
                addr2 = getNextOpt(ops);
                invalidIPv4(addr2);
            }
        } else
        {
            addr1 = getNextOpt(ops);
            invalidIPv6(addr1);
            if (ops.size()>0)
            {
                addr2 = getNextOpt(ops);
                // addr2 is mask length
            }
        }

        cout << "Address: " << addr1 << endl;
        if (!addr2.empty()) cout << "Netmask: " << addr2 << endl;

        Address *o = Address::cast(nobj);

        if (objtype==IPv6::TYPENAME)
            o->setAddress(InetAddr(AF_INET6, addr1));
        else
            o->setAddress(InetAddr(addr1));

        if (!addr2.empty())
        {
            if (objtype==IPv6::TYPENAME)
            {
                istringstream istr(addr2);
                int masklen;
                istr >> masklen;
                o->setNetmask(InetAddr(AF_INET6, masklen));
            } else
                o->setNetmask(InetAddr(addr2));
        }
                
    }
    else if (objtype==DNSName::TYPENAME)
    {
        try
        {
            dnsrec = getNextOpt(ops);
            runtime = getNextOpt(ops);
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        cout << "DNS Record: " << dnsrec << endl;
        cout << "Run time: " << runtime << endl;
                
        DNSName *o=DNSName::cast(nobj);
        o->setSourceName(dnsrec);
        o->setRunTime(getBool(runtime));
                
    }
    else if (objtype==AddressRange::TYPENAME)
    {
        try
        {
            addr1 = getNextOpt(ops); invalidIPv4Orv6(addr1);
            addr2 = getNextOpt(ops); invalidIPv4Orv6(addr2);
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        InetAddr range_start(AF_UNSPEC, addr1);
        InetAddr range_end(AF_UNSPEC, addr2);

        if (range_start.addressFamily() != range_end.addressFamily()) {

            cout << "AddressRange start and end address must be of same IP address family: ";
            cout << "start_address: " << range_start.toString() << ", ";
            cout << "end_address: " << range_end.toString();
            cout << endl;

            exit(1);
        }

        cout   << "Range start: " << addr1 << endl
               << "Range end: " << addr2 << endl;

        AddressRange *o=AddressRange::cast(nobj);
        o->setRangeStart(range_start);
        o->setRangeEnd(range_end);
    }
    else if (objtype==ObjectGroup::TYPENAME)
    {
        //ObjectGroup *o=ObjectGroup::cast(nobj);
        ;     
    }
    else if (objtype==Network::TYPENAME)
    {
        try
        {
            addr1 = getNextOpt(ops); invalidIPv4(addr1);
            addr2 = getNextOpt(ops); invalidIPv4(addr2);
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        cout   << "Address: " << addr1 << endl
               << "Netmask: " << addr2 << endl;

        Network *o=Network::cast(nobj);
        o->setAddress(InetAddr(addr1));
        o->setNetmask(InetAddr(addr2));
    }
    else if (objtype==NetworkIPv6::TYPENAME)
    {
        try
        {
            addr1=getNextOpt(ops); invalidIPv6(addr1);
            addr2=getNextOpt(ops); invalidIPv6(addr2);
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        cout   << "Address: " << addr1 << endl
               << "Netmask: " << addr2 << endl;

        NetworkIPv6 *o=NetworkIPv6::cast(nobj);
        o->setAddress(InetAddr(AF_INET6, addr1));
        istringstream istr(addr2);
        int masklen;
        istr >> masklen;
        o->setNetmask(InetAddr(AF_INET6, masklen));
    }
    else if (objtype==Firewall::TYPENAME || objtype==Cluster::TYPENAME)
    {
        try
        {
            platform=getNextOpt(ops);
            hostOS=getNextOpt(ops);
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        cout    << "Platform: " << platform << endl
                << "Host OS: " << hostOS << endl;

        if (testPlatform(platform, hostOS))
        {
            Firewall *o=Firewall::cast(nobj);
            o->setStr("platform",platform);
            o->setStr("host_OS",hostOS);
        }
        else
        {
            cout << "Platform and Host OS combination is invalid."
                 << endl;
        }
                
    }
    else if (objtype==Interval::TYPENAME)
    {
        try
        {
            time1 = getNextOpt(ops);
            date1 = getNextOpt(ops);
            day1 = getNextOpt(ops);
            time2 = getNextOpt(ops);
            date2 = getNextOpt(ops);
            day2 = getNextOpt(ops);                
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        QTime time;
        QDate date;
        int m,h,d,mn,y,dw;
                
        cout    << "Activate a rule on:" << endl
                << "Time: " << time1 << endl
                << "Date: " << date1 << endl
                << "Day of week:" << day1 << endl
                << "Deactivate a rule on:" << endl
                << "Time: " << time2 << endl
                << "Date: " << date2 << endl
                << "Day of week:" << day2 << endl;

        Interval *o=Interval::cast(nobj);
        if (time1 == "")
        {
            m=0; h=0;
        }
        else
        {
            time=QTime::fromString(time1.c_str());
            m=time.minute();
            h=time.hour();
        }
        if (date1 == "")
        {
            mn=2; d=28; y=2935093;
        }
        else
        {
            date=QDate::fromString(date1.c_str(),Qt::ISODate);
            mn=date.month();
            d=date.day();
            y=date.year();
        }
        if (day1 == "")
        {
            dw=-1;
        }
        else
        {
            dw=atoi(day1.c_str());
        }
        o->setStartTime(m,h,d,mn,y,dw);
                
        if (time2 == "")
        {
            m=0; h=0;
        }
        else
        {
            time=QTime::fromString(time2.c_str());
            m=time.minute();
            h=time.hour();
        }
        if (date2 == "")
        {
            mn=2; d=28; y=2935093;
        }
        else
        {
            date=QDate::fromString(date2.c_str(),Qt::ISODate);
            mn=date.month();
            d=date.day();
            y=date.year();
        }
        if (day2 == "")
        {
            dw=-1;
        }
        else
        {
            dw=atoi(day2.c_str());
        }
        o->setEndTime(m,h,d,mn,y,dw);
    }
    else if (objtype==Interface::TYPENAME)
    {
        try
        {
            security=getNextOpt(ops);
            addrtype=getNextOpt(ops);
            management=getNextOpt(ops);
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        if (security=="")
        {
            cout << "Security level is mandatory attribute." << endl;
        }
        else
        {
            cout    << "Security level: " << security << endl
                    << "Address type: " << addrtype << endl
                    << "Management interface: " << management << endl;

            Interface *o = Interface::cast(nobj);
            int sl = atoi(security.c_str());
            o->setSecurityLevel(sl);
            o->setDyn(addrtype=="dynamic");
            o->setUnnumbered(addrtype=="unnumbered");
            o->setManagement(getBool(management));
        }
    }
    else if (objtype==Host::TYPENAME)
    {
        //Host *o=Host::cast(nobj);
        ;
    }
    else if (objtype==TCPService::TYPENAME)
    {
        try
        {
            addr1=getNextOpt(ops);
            addr2=getNextOpt(ops);
            addr3=getNextOpt(ops);
            addr4=getNextOpt(ops);
            tcpflags_mask=getNextOpt(ops);
            tcpflags_bits=getNextOpt(ops);
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        cout    << "Source port range:" << endl
                << "Start: " << addr1 << endl
                << "End: " << addr2 << endl
                << "Destination port range:" << endl
                << "Start: " << addr3 << endl
                << "End: " << addr4 << endl
                << "TCP Flags: " << endl
                << "Mask: " << tcpflags_mask << endl
                << "Settings: " << tcpflags_bits << endl;

        TCPService *o=TCPService::cast(nobj);

        o->setSrcRangeStart(atoi(addr1.c_str()));
        o->setSrcRangeEnd(  atoi(addr2.c_str()));
        o->setDstRangeStart(atoi(addr3.c_str()));
        o->setDstRangeEnd(  atoi(addr4.c_str()));

        o->setBool("urg_flag_mask",
                   tcpflags_mask.find('u')!=string::npos ||
                   tcpflags_mask.find('U')!=string::npos);
        o->setBool("ack_flag_mask",
                   tcpflags_mask.find('a')!=string::npos ||
                   tcpflags_mask.find('A')!=string::npos);
        o->setBool("psh_flag_mask",
                   tcpflags_mask.find('p')!=string::npos ||
                   tcpflags_mask.find('P')!=string::npos);
        o->setBool("rst_flag_mask",
                   tcpflags_mask.find('r')!=string::npos ||
                   tcpflags_mask.find('R')!=string::npos);
        o->setBool("syn_flag_mask",
                   tcpflags_mask.find('s')!=string::npos ||
                   tcpflags_mask.find('S')!=string::npos);
        o->setBool("fin_flag_mask",
                   tcpflags_mask.find('f')!=string::npos ||
                   tcpflags_mask.find('F')!=string::npos);
                
        o->setBool("urg_flag",
                   tcpflags_bits.find('u')!=string::npos ||
                   tcpflags_bits.find('U')!=string::npos);
        o->setBool("ack_flag",
                   tcpflags_bits.find('a')!=string::npos ||
                   tcpflags_bits.find('A')!=string::npos);
        o->setBool("psh_flag",
                   tcpflags_bits.find('p')!=string::npos ||
                   tcpflags_bits.find('P')!=string::npos);
        o->setBool("rst_flag",
                   tcpflags_bits.find('r')!=string::npos ||
                   tcpflags_bits.find('R')!=string::npos);
        o->setBool("syn_flag",
                   tcpflags_bits.find('s')!=string::npos ||
                   tcpflags_bits.find('S')!=string::npos);
        o->setBool("fin_flag",
                   tcpflags_bits.find('f')!=string::npos ||
                   tcpflags_bits.find('F')!=string::npos);
    }
    else if (objtype==UDPService::TYPENAME)
    {
        try
        {
            addr1=getNextOpt(ops);
            addr2=getNextOpt(ops);
            addr3=getNextOpt(ops);
            addr4=getNextOpt(ops);                
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        cout    << "Source port range:" << endl
                << "Start: " << addr1 << endl
                << "End: " << addr2 << endl
                << "Destination port range:"  << endl
                << "Start: " << addr3 << endl
                << "End: " << addr4 << endl;

        UDPService *o=UDPService::cast(nobj);

        o->setSrcRangeStart(atoi(addr1.c_str()));
        o->setSrcRangeEnd(  atoi(addr2.c_str()));
        o->setDstRangeStart(atoi(addr3.c_str()));
        o->setDstRangeEnd(  atoi(addr4.c_str()));
                
    }
    else if (objtype==ICMPService::TYPENAME)
    {
        try
        {
            ICMPtype=getNextOpt(ops);
            ICMPcode=getNextOpt(ops);
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        cout    << "ICMP type: " << ICMPtype << endl
                << "ICMP code: " << ICMPcode << endl;

        ICMPService *o=ICMPService::cast(nobj);
        o->setInt("type",atoi(ICMPtype.c_str()));
        o->setInt("code",atoi(ICMPcode.c_str()));
    }
    else if (objtype==IPService::TYPENAME)
    {
        try
        {
            protocol=getNextOpt(ops);
            bitmap=getNextOpt(ops);
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }

        cout    << "Protocol number: " << protocol << endl
                << "Flags: " << bitmap << endl;

        IPService *o=IPService::cast(nobj);
        o->setInt("protocol_num",atoi(protocol.c_str()));

        o->setBool("lsrr",false);
        o->setBool("ssrr",false);
        o->setBool("rr",false);
        o->setBool("ts",false);
        o->setBool("fragm",false);
        o->setBool("short_fragm",false);
                
        operands flags;
        int n=splitStr('/',bitmap,&flags);
        string tt;
        try
        {
            for (int i=0; i<n;i++)
            {
                tt=getNextOpt(flags);
                if (tt=="lsrr")
                {
                    o->setBool(tt,true);
                } else if (tt=="ssrr")
                {
                    o->setBool(tt,true);
                } else if (tt=="rr")
                {
                    o->setBool(tt,true);
                } else if (tt=="ts")
                {
                    o->setBool(tt,true);
                } else if (tt=="fragm")
                {
                    o->setBool(tt,true);
                } else if (tt=="short_fragm")
                {
                    o->setBool(tt,true);
                }
            }
        } catch (OperandsError &e)
        {
            notEnoughAttributesError();
        }
                
    }
            
}



void newObject(FWObjectDatabase *objdb,
               const string &objtype,
               const string &name,
               const string &comment_txt,
               const string &parent,
               operands &ops)
{
    initConstants();

    if (parent.empty())
    {
        cout << "Parent for the object is undefined." << endl;
        usage();
        exit(1);
    }

    if (objtype.empty() || name.empty())
    {
        cout << "Need object type and name" << endl;
        usage();
        exit(-1);
    }

    cout << "Adding a new object to '"
         << parent <<"': " << endl
         << "Type: " << objtype << endl
         << "Name: " << name << endl;

    FWObject *nobj = createObject(objdb, objtype, parent);

    nobj->setName(name);

    _modObject(nobj, comment_txt, ops);


    if (objtype==Library::TYPENAME)
    {
        FWObject *new_child = createObject(objdb,
                                           ObjectGroup::TYPENAME,
                                           nobj->getPath());
        new_child->setName("Objects");
        cout << new_child->getPath() << endl;

        new_child = createObject(objdb, ServiceGroup::TYPENAME, nobj->getPath());
        new_child->setName("Services");
        cout << new_child->getPath() << endl;

        std::map<string,string>::iterator it;
        for (it=systemGroupPaths.begin(); it!=systemGroupPaths.end(); ++it)
        {
            string path = it->second;
            if (path.empty()) continue;
            string t = systemGroupTypes[path];

            string::size_type n = path.find("/");
            string parent_obj = path.substr(0,n);
            string obj_name = path.substr(n+1);
            if (n==string::npos) parent_obj = "";

            new_child = createObject(
                objdb, t, nobj->getName() + "/" + parent_obj);
            new_child->setName(obj_name);
            cout << new_child->getPath() << endl;
        }
    }

    cout  << endl;
}

void modObject(FWObjectDatabase *objdb,
               const string &object,
               const string &comment_txt,
               operands &ops)
{
    list<FWObject*> objects;
    findObjects(object, objdb, objects);
    if (objects.size()==0)
    {
        cout << "Object " << object << " not found" << endl;
        exit(-1);
    }

    for (list<FWObject*>::iterator it=objects.begin(); it!=objects.end(); ++it)
    {
        FWObject *obj = *it;
        cout << obj->getPath() << endl;
        _modObject(obj, comment_txt, ops);
    }
}

void delObject(FWObjectDatabase *objdb, const string &object)
{
    list<FWObject*> objects;
    findObjects(object, objdb, objects);
    if (objects.size()==0)
    {
        cout << "Object " << object << " not found" << endl;
        exit(-1);
    }

    for (list<FWObject*>::iterator it=objects.begin(); it!=objects.end(); ++it)
    {
        FWObject *obj = *it;
        cout << "Removing object '" << obj->getName()
             << "' from the tree." << endl;
        objdb->removeAllInstances(obj);
    }
}



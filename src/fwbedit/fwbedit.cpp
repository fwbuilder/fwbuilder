/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: fwbedit.cpp 1450 2007-12-05 16:53:10Z vk $

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

#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/Constants.h"

#include <qsettings.h>
#include <qdatetime.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
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

#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#else
#  ifdef _WIN32
#    include <getopt.h>
#  else
#    include <stdlib.h>
#  endif
#endif

#include "fwbuilder/Resources.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Group.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/InterfacePolicy.h"

#include "../common/init.cpp"
#include <assert.h>

using namespace libfwbuilder;
using namespace std;

// can't use 'DELETE' in this enum because it is degined somewhere on windows
typedef enum { NONE, ADDGRP, REMGRP, DELOBJECT, OBJECT, LIST, STRUCT,ATTR, UPGRADE} command;

// need to qualify deque even though we use "using namespace std;"
// to make it compile on windows
typedef std::deque<string> operands;

static command    cmd             = NONE;

bool autoupgrade_flag = false;

string filename="";
string object;
string group;
string objtype;
string addr1;
string addr2;
string dnsrec;
string runtime;
string name;
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
string mask;
string settings;
string protocol;
string bitmap;
string ICMPtype;
string ICMPcode;
string security;
string addrtype;
string parent;

vector<string> platforms;

std::map<string,string>  systemGroupPaths;

FWObjectDatabase       *objdb = NULL;

int fwbdebug = 0;

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string&) const 
    {
        bool res=false;
        cout << _("Data file has been created in the old version of Firewall Builder.") << endl << flush;
        if (autoupgrade_flag)
        {
            cout << _("Do you want to convert it? (Y)") << endl;
            int a = getchar();
            if (a=='y' || a=='Y' || a=='\n' ) res= true;
        }
        else
        {
            cout << _("Use option '-u' to upgrade the file. Alternatively,\nfwbuilder GUI can convert it.") << endl;
        }
        if (res) cout << _("Upgrading the file now ...") << endl;
        return res;
    }
};
void initConstants ( void)
{
    systemGroupPaths[Library::TYPENAME]       = "";

    systemGroupPaths[IPv4::TYPENAME]          = "Objects/Addresses";
    systemGroupPaths[DNSName::TYPENAME]       = "Objects/DNS Names";
    systemGroupPaths[AddressTable::TYPENAME]  = "Objects/Address Tables";
    systemGroupPaths[AddressRange::TYPENAME]  = "Objects/Address Ranges";
    systemGroupPaths[ObjectGroup::TYPENAME]   = "Objects/Groups";
    systemGroupPaths[Host::TYPENAME]          = "Objects/Hosts";
    systemGroupPaths[Network::TYPENAME]       = "Objects/Networks";

    systemGroupPaths[ServiceGroup::TYPENAME]  = "Services/Groups";
    systemGroupPaths[CustomService::TYPENAME] = "Services/Custom";
    systemGroupPaths[IPService::TYPENAME]     = "Services/IP";
    systemGroupPaths[ICMPService::TYPENAME]   = "Services/ICMP";
    systemGroupPaths[TCPService::TYPENAME]    = "Services/TCP";
    systemGroupPaths[UDPService::TYPENAME]    = "Services/UDP";

    systemGroupPaths[Firewall::TYPENAME]      = "Firewalls";

    systemGroupPaths[Interval::TYPENAME]      = "Time";
}
void usage(const char *name)
{
    cout << _("Firewall Builder:  general purpose object tree editing tool") << endl;
    cout << _("Version ") << VERSION << "-" << RELEASE_NUM << endl;
    cout << _("Usage: ") << name
         << _(" -f filename.fwb -u [-a obj,grp] [-r obj,grp] [-d obj] [-s] [-l path] [(-p parent|-L library) -t objtype -n objname [-o object attributes]] ") << endl;
    cout << endl;
    cout << "      " << _("-t objtype : create an object of this type") << endl;
    cout << "      " << _("-L library : specify library when creating a new object") << endl;
    cout << "      " << _("-p obj     : specify parent object when creating a new object") << endl;
    cout << "      " << _("-n name    : specify a name of the new object") << endl;
    cout << "      " << _("-o attribute1[,attribute2...]  :  specify attributes when creating a new object") << endl;
    cout << "      " << _("-a obj,grp :  create reference to object 'obj' in the group 'grp'") << endl;
    cout << "      " << _("-r obj,grp :  remove reference to object 'obj' from the group 'grp'") << endl;
    cout << "      " << _("-d obj     :  delete object 'obj' and remove references to it from") << endl;
    cout << "      " << _("              all rules and groups") << endl;
    cout << "      " << _("-l path    :  print list of objects for 'path'") << endl;
    cout << "      " << _("-s         :  test and repair object tree structure") << endl;
    cout << "      " << _("-u         : autoupgrade of file") << endl;
    cout << endl;
    cout << _("An object and a group can be defined by their ID or ") <<  endl;
    cout << _("by the full path and name in the XML tree") << endl;
    cout << endl;
    cout << _("Object creation syntax:") << endl;
    cout << endl;
    cout << "      " << "-t " <<Firewall::TYPENAME      << " -n obj_name -L User -o platform, host OS" << endl;
    cout << "      " << "-t " <<IPv4::TYPENAME          << " -n obj_name -L User -o IP address" << endl;
    cout << "      " << "-t " <<DNSName::TYPENAME       << " -n obj_name -L User -o DNS record,run time" << endl;
    cout << "      " << "-t " <<AddressRange::TYPENAME  << " -n obj_name -L User -o start address, end address" << endl;
    cout << "      " << "-t " <<ObjectGroup::TYPENAME   <<  endl;
    cout << "      " << "-t " <<Network::TYPENAME       << " -n obj_name -L User -o address,netmask" << endl;
    cout << "      " << "-t " <<Interval::TYPENAME      << " -n obj_name -L User -o start time,start date,start day,end time, end date, end day" << endl;
    cout << "      " << "-t " <<Interface::TYPENAME     << " -n obj_name -L User -o security level,address type (dynamic or unnumbered),management" << endl;
    cout << "      " << "-t " <<Host::TYPENAME          <<  endl;
    cout << "      " << "-t " <<TCPService::TYPENAME    << " -n obj_name -L User -o source port range start,end,Destination port range start,end,UAPRSF,UAPRSF" << endl;
    cout << "      " << "-t " <<UDPService::TYPENAME    << " -n obj_name -L User -o source port range start,end,Destination port range start,end" << endl;
    cout << "      " << "-t " <<ICMPService::TYPENAME   << " -n obj_name -L User -o ICMP type,ICMP code" << endl;
    cout << "      " << "-t " <<IPService::TYPENAME     << " -n obj_name -L User -o protocol number,lsrr/ssrr/rr/ts/fragm/short_fragm" << endl;
}

FWObject *find_object(const string &obj_path,
                      FWObject *root=objdb)
{
    string path=obj_path;
    if (path=="") return root;
    string::size_type n=path.find("/",0);
    string tree_node=path.substr(0,n);

    FWObject::iterator j=std::find_if(root->begin(), root->end(), 
                                      FWObjectNameEQPredicate(tree_node));
    if (j==root->end()) return NULL;
    if ((*j)->getName()==obj_path) return (*j);
    else    return find_object( path.substr(n+1) , (*j) );
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

FWObject *getObject(const char *objstr) throw (FWException)
{
    char errstr[128];
    FWObject *obj=NULL;
    if (strchr(objstr,'/')!=NULL)
    {
/* got full path to the object */
        string path=objstr;
        string::size_type n=path.find("/",0);
        if (n==0 || 
            path.substr(0,n)=="FWObjectDatabase" || 
            path.substr(0,n)=="User")
            obj= find_object( path.substr(n+1) );
        else
            obj= find_object( path );
        if (obj==NULL)
        {
            SNPRINTF(errstr,sizeof(errstr),_("Object %s not found"),objstr );
            throw FWException(errstr);
        }
    } else {
/* got object ID */
        obj=objdb->getById(FWObjectDatabase::getIntId(objstr), true);
        if (obj==NULL) 
        {
            SNPRINTF(errstr,sizeof(errstr),_("Object with ID='%s' not found"),objstr );
            throw FWException(errstr);
        }
    }
    return obj;
}
int splitStr(char ch,string s, operands * ops)
{
   int res=0;
   string::size_type pos;
   ops->clear();
   if (s.length()>0)
   {
       while((pos=s.find_first_of(ch))!=string::npos)
       {
           ops->push_back(s.substr(0,pos));
           s=s.substr(pos+1);
           res++;
       }
       ops->push_back(s);
       res++;
   }
           
   return res;
}
FWObject* createObject(string type,string path)
{
    FWObject* obj=objdb->create(type);
    FWObject* parent=getObject(path.c_str());
    if (parent != NULL)
    {
        parent->add(obj);    
    }
    return obj;
}
string getNextOpt(operands &ops)
{
    string s=*ops.begin();
    ops.pop_front();
    return s;    
}
bool testPlatform(string pl,string os)
{
    platforms=Resources::getListOfPlatforms();
    operands lst;
    string str;
    if (platforms.empty() || ( platforms.size()==1 && platforms.front()=="unknown" ))
    {
        cout << _("Failed to load list of supported platforms") << endl;
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
                int n=splitStr(',',sos,&lst);
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

bool getBool(string s)
{
    return (s.find("y")!=string::npos) || (s.find("Y")!=string::npos) || (s.find("1")!=string::npos);
}

FWObject* testAndFix(string n,string t,FWObject *root)
{
    FWObject *res;
    cout  << n << _(" - ");
    assert(root != NULL);
    res=find_object(n,root);
    if (res==NULL)
    {
        cout << _("Absent ");
        
        res =objdb->create(t.c_str());
        res->setName(n);
        root->add(res);
        cout << _("( Fixed )");
    }
    else
    {
        cout << _("Ok");
    }
    cout << endl;
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

int main(int argc, char * const *argv)
{   
    operands ops;

#ifdef ENABLE_NLS
    setlocale (LC_ALL, "");

    bindtextdomain (PACKAGE, LOCALEDIR);
    textdomain (PACKAGE);
#else
#  ifdef HAVE_SETLOCALE
    setlocale (LC_ALL, "");
#  endif
#endif
    initConstants();
    if (argc<=1)
    {
        usage(argv[0]);
        exit(1);
    }

    int   opt;

    while( (opt=getopt(argc,argv,"f:a:r:d:o:l:L:p:t:n:su")) != EOF )
    {
        int num=0;
        if (optarg!=NULL)
        {
            string str=optarg;
            num=splitStr(',',str,&ops);
        }
        switch(opt)
        {
        case 'f':     filename=getNextOpt(ops); break;
        case 'L':     lib=getNextOpt(ops); break;
        case 'p':     parent=getNextOpt(ops); break;
        case 't':     objtype=getNextOpt(ops);cmd=OBJECT; break;
        case 'n':     name=getNextOpt(ops); break;
        case 'a':     cmd=ADDGRP;      break;
        case 'r':     cmd=REMGRP;      break;
        case 'd':     cmd=DELOBJECT;   break;
        case 'o':     cmd=ATTR;        break;
        case 'l':     cmd=LIST;        break;  
        case 's':     cmd=STRUCT;      break;
        case 'u':     
                      {
                          cmd=(cmd==NONE)?UPGRADE:cmd;
                          autoupgrade_flag=true;  break;
                      }
        default :     usage(argv[0]);  exit(1);
                      
        }
                
        if (cmd==ADDGRP || cmd==REMGRP)
        {
            if (num!=2)
            {
                usage(argv[0]);
                exit(1);
            }
            object=getNextOpt(ops);
            group=getNextOpt(ops);
            break;
        }
        if (cmd==DELOBJECT)
        {
            object=getNextOpt(ops);
            break;
        }
        if (cmd==LIST)
        {
            if (num==1)
            {
                path=getNextOpt(ops);            
            }
            else
            {
                usage(argv[0]);
                exit(1);
            }
            break;
        }
        if (cmd==ATTR)
        {
            if (name=="" || objtype=="" || 
                (parent=="" && lib==""))
            {
                usage(argv[0]);
                exit(1);
            }
            cmd=OBJECT;
            
            if (objtype==IPv4::TYPENAME && num==1)
            {
                addr1=getNextOpt(ops);
                invalidIPv4(addr1);

            }
            else if (objtype==DNSName::TYPENAME && num==2)
            {
                dnsrec=getNextOpt(ops);
                runtime=getNextOpt(ops);
            }
            else if ((objtype==AddressRange::TYPENAME || objtype==Network::TYPENAME) && num==2)
            {
                addr1=getNextOpt(ops);invalidIPv4(addr1);
                addr2=getNextOpt(ops);invalidIPv4(addr2);
            }
            else if (objtype==ObjectGroup::TYPENAME &&       num==0)
            {}
            else if (objtype==Firewall::TYPENAME &&          num==2)
            {
                platform=getNextOpt(ops);
                hostOS=getNextOpt(ops);
            }
            else if (objtype==Interval::TYPENAME &&         num==6)
            {
                time1=getNextOpt(ops);
                date1=getNextOpt(ops);
                day1=getNextOpt(ops);
                time2=getNextOpt(ops);
                date2=getNextOpt(ops);
                day2=getNextOpt(ops);                
            }
            else if (objtype==Interface::TYPENAME &&         num==3)
            {
                security=getNextOpt(ops);
                addrtype=getNextOpt(ops);
                management=getNextOpt(ops);
            }
            else if (objtype==Host::TYPENAME &&               num==0)
            {
            }
            else if (objtype==TCPService::TYPENAME &&         num==6)
            {
                addr1=getNextOpt(ops);
                addr2=getNextOpt(ops);
                addr3=getNextOpt(ops);
                addr4=getNextOpt(ops);
                mask=getNextOpt(ops);
                settings=getNextOpt(ops);
            }
            else if (objtype==UDPService::TYPENAME &&        num==4)
            {
                addr1=getNextOpt(ops);
                addr2=getNextOpt(ops);
                addr3=getNextOpt(ops);
                addr4=getNextOpt(ops);                
            }
            else if (objtype==IPService::TYPENAME &&         num==2)
            {
                protocol=getNextOpt(ops);
                bitmap=getNextOpt(ops);
            }
            else if (objtype==ICMPService::TYPENAME &&        num==2)
            {
                ICMPtype=getNextOpt(ops);
                ICMPcode=getNextOpt(ops);
            }
            
            else
            {
                cout << "Adding objects of the type '" << objtype << "' is not supported or object's attributes are invalid." << endl;
                cmd=NONE;
            }     
            break;
        }
    }


    if(cmd==NONE || filename=="")
    {
        usage(argv[0]);
        exit(1);
    }

    init(argv);

    try 
    {
        new Resources(respath+FS_SEPARATOR+"resources.xml");

        //new Resources(respath);   // creates singleton Resources::global_res

        /* create database */
        objdb = new FWObjectDatabase();

        /* load the data file */
        UpgradePredicate upgrade_predicate; 

        objdb->load(filename,  &upgrade_predicate, librespath);
    
        if (cmd == STRUCT)
        {
            cout << _("Test/repair the structure:") << endl;
            FWObject *root=objdb;
            FWObject *nlib;
            FWObject *grp;
            string lib;
            bool ro_flag;
            
            for (FWObject::iterator i=root->begin();
                    i!=root->end();
                    ++i)
            {
                nlib=*i;
                lib=nlib->getName();
                if (nlib->getId()!=FWObjectDatabase::DELETED_OBJECTS_ID)
                {
                    ro_flag=nlib->isReadOnly();
                    cout << _("Library: ") << lib << ((ro_flag)?"(Read only)":" ") << endl;
                    nlib->setReadOnly(false);
                    
                    grp=testAndFix("Objects",       ObjectGroup::TYPENAME,nlib);
                        testAndFix("Addresses",     ObjectGroup::TYPENAME,grp);
                        testAndFix("DNS Names",     ObjectGroup::TYPENAME,grp);
                        testAndFix("Address Tables",ObjectGroup::TYPENAME,grp);
                        testAndFix("Groups",        ObjectGroup::TYPENAME,grp);
                        testAndFix("Hosts",         ObjectGroup::TYPENAME,grp);
                        testAndFix("Networks",      ObjectGroup::TYPENAME,grp);
                        testAndFix("Address Ranges",ObjectGroup::TYPENAME,grp);
                        
                    grp=testAndFix("Services",      ServiceGroup::TYPENAME,nlib);
                        testAndFix("Groups",        ServiceGroup::TYPENAME,grp);
                        testAndFix("ICMP",          ServiceGroup::TYPENAME,grp);
                        testAndFix("IP",            ServiceGroup::TYPENAME,grp);
                        testAndFix("TCP",           ServiceGroup::TYPENAME,grp);
                        testAndFix("UDP",           ServiceGroup::TYPENAME,grp);
                        testAndFix("Custom",        ServiceGroup::TYPENAME,grp);
                        testAndFix("TagServices",   ServiceGroup::TYPENAME,grp);
                        
                        testAndFix("Firewalls",     ObjectGroup::TYPENAME,nlib);
                        testAndFix("Time",          IntervalGroup::TYPENAME,nlib);

                    nlib->setReadOnly(ro_flag);    
                }
            }
        }
        else if (cmd == LIST)
        {
            FWObject *root=getObject(path.c_str());
            
            cout << _("Objects list for '"<< path << "' :") << endl;
            for(FWObject::iterator i=root->begin(); 
                         i!=root->end();
                         ++i)
            {
               cout << (*i)->getName() << _(" ( ") << (*i)->getId() <<  _(" ) ") << endl;
            }
            return(0);
        }
        else if (cmd == UPGRADE)
        {
            cout << _("File upgraded; current version: ") << libfwbuilder::Constants::getLibraryVersion() << endl;
        }
        else  if (cmd == OBJECT)
        {
            cout << _("Adding a new object into '"<< ((lib!="")?lib:parent) <<"': ") << endl
                 << _("Type: ") << objtype << endl
                 << _("Name: ") << name << endl;
            if (objtype==IPv4::TYPENAME)
            {
                cout << _("Address: ") << addr1 << endl;
                FWObject *nobj = NULL;
                if (lib!="")
                {
                    nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                }
                else if (parent!="")
                {
                    FWObject *target = getObject(parent.c_str());

                    if (Interface::isA(target))
                    {
                        nobj = objdb->create(objtype);   
                        target->add(nobj);
                    }
                }
                else
                {
                    cout << _("Parent for the object is missing.") << endl;
                    exit(1);
                }
                
                if (nobj!=NULL)
                {
                    IPv4 *o=IPv4::cast(nobj);
                    o->setName(name);
                    o->setAddress(InetAddr(addr1));
                }
                
            }
            else if (objtype==DNSName::TYPENAME)
            {
                cout << _("DNS Record: ") << dnsrec << endl;
                cout << _("Run time: ") << runtime << endl;
                
                FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                DNSName *o=DNSName::cast(nobj);
                o->setName(name);
                o->setSourceName(dnsrec);
                o->setRunTime(getBool(runtime));
                
            }
            else if (objtype==AddressRange::TYPENAME)
            {
                 cout   << _("Range start: ") << addr1 << endl
                        << _("Range end: ") << addr2 << endl;
                 FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                 AddressRange *o=AddressRange::cast(nobj);
                 o->setName(name);
                 o->setRangeStart(InetAddr(addr1));
                 o->setRangeEnd(InetAddr(addr2));

            }
            else if (objtype==ObjectGroup::TYPENAME)
            {
                 FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                 ObjectGroup *o=ObjectGroup::cast(nobj);
                 o->setName(name);
                
            }
            else if (objtype==Network::TYPENAME)
            {
                 cout   << _("Address: ") << addr1 << endl
                        << _("Netmask: ") << addr2 << endl;
                 FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                 Network *o=Network::cast(nobj);
                 o->setName(name);
                 o->setAddress(InetAddr(addr1));
                 o->setNetmask(InetAddr(addr2));
            }
            else if (objtype==Firewall::TYPENAME)
            {
                cout    << _("Platform: ") << platform << endl
                        << _("Host OS: ") << hostOS << endl;
                if (testPlatform(platform,hostOS))
                {
                    FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                    Firewall *o=Firewall::cast(nobj);
                    o->setName(name);
                    o->setStr("platform",platform);
                    o->setStr("host_OS",hostOS);
                }
                else
                {
                    cout << _("Platform and Host OS combination is invalid.") << endl;
                }
                
            }
            else if (objtype==Interval::TYPENAME)
            {
                QTime time;
                QDate date;
                int m,h,d,mn,y,dw;
                
                cout    << _("Activate a rule on:") << endl
                        << _("Time: ") << time1 << endl
                        << _("Date: ") << date1 << endl
                        << _("Day of week:") << day1 << endl
                        << _("Deactivate a rule on:") << endl
                        << _("Time: ") << time2 << endl
                        << _("Date: ") << date2 << endl
                        << _("Day of week:") << day2 << endl;
                FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                Interval *o=Interval::cast(nobj);
                o->setName(name);
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
                if (security=="")
                {
                    cout << _("Security level is an obligatory attribute.") << endl;
                }
                else
                {
                    cout    << _("Security level: ") << security << endl
                            << _("Address type: ") << addrtype << endl
                            << _("Management interface: ") << management << endl;
                    FWObject *target = getObject(parent.c_str());
                    if (Host::isA(target) ||  Firewall::isA(target))
                    {
                        FWObject *nobj = objdb->create(objtype);   
                        Interface *o=Interface::cast(nobj);
                        o->setName(name);
                        int sl=atoi(security.c_str());
                        o->setSecurityLevel(sl);
                        o->setExt((sl==0)?true:false);
                        o->setDyn(addrtype=="dynamic");
                        o->setUnnumbered(addrtype=="unnumbered");
                        o->setManagement(getBool(management));
                        
                        target->add(o);
                        if (Firewall::isA(target))
                        {
                            o->add(new InterfacePolicy());

                        }
                    }
                    else
                    {
                        cout << lib << " - " << "invalid parent for an Interface" << endl;
                    }
                }
            }
            else if (objtype==Host::TYPENAME)
            {
                FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                Host *o=Host::cast(nobj);
                o->setName(name);
            }
            else if (objtype==TCPService::TYPENAME)
            {
                cout    << _("Source port range:") << endl
                        << _("Start: ") << addr1 << endl
                        << _("End: ") << addr2 << endl
                        << _("Destination port range:") << endl
                        << _("Start: ") << addr3 << endl
                        << _("End: ") << addr4 << endl
                        << _("TCP Flags: ") << endl
                        << _("Mask: ") << mask << endl
                        << _("Settings: ") << settings << endl;
                FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                TCPService *o=TCPService::cast(nobj);
                o->setName(name);

                o->setSrcRangeStart(atoi(addr1.c_str()));
                o->setSrcRangeEnd(  atoi(addr2.c_str()));
                o->setDstRangeStart(atoi(addr3.c_str()));
                o->setDstRangeEnd(  atoi(addr4.c_str()));

                o->setBool("urg_flag_mask",mask.find('u')!=string::npos || mask.find('U')!=string::npos);
                o->setBool("ack_flag_mask",mask.find('a')!=string::npos || mask.find('A')!=string::npos);
                o->setBool("psh_flag_mask",mask.find('p')!=string::npos || mask.find('P')!=string::npos);
                o->setBool("rst_flag_mask",mask.find('r')!=string::npos || mask.find('R')!=string::npos);
                o->setBool("syn_flag_mask",mask.find('s')!=string::npos || mask.find('S')!=string::npos);
                o->setBool("fin_flag_mask",mask.find('f')!=string::npos || mask.find('F')!=string::npos);
                
                o->setBool("urg_flag",settings.find('u')!=string::npos || settings.find('U')!=string::npos);
                o->setBool("ack_flag",settings.find('a')!=string::npos || settings.find('A')!=string::npos);
                o->setBool("psh_flag",settings.find('p')!=string::npos || settings.find('P')!=string::npos);
                o->setBool("rst_flag",settings.find('r')!=string::npos || settings.find('R')!=string::npos);
                o->setBool("syn_flag",settings.find('s')!=string::npos || settings.find('S')!=string::npos);
                o->setBool("fin_flag",settings.find('f')!=string::npos || settings.find('F')!=string::npos);
            }
            else if (objtype==UDPService::TYPENAME)
            {
                cout    << _("Source port range:") << endl
                        << _("Start: ") << addr1 << endl
                        << _("End: ") << addr2 << endl
                        << _("Destination port range:")  << endl
                        << _("Start: ") << addr3 << endl
                        << _("End: ") << addr4 << endl;
                FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                UDPService *o=UDPService::cast(nobj);
                o->setName(name);

                o->setSrcRangeStart(atoi(addr1.c_str()));
                o->setSrcRangeEnd(  atoi(addr2.c_str()));
                o->setDstRangeStart(atoi(addr3.c_str()));
                o->setDstRangeEnd(  atoi(addr4.c_str()));
                
            }
            else if (objtype==ICMPService::TYPENAME)
            {
                cout    << _("ICMP type: ") << ICMPtype << endl
                        << _("ICMP code: ") << ICMPcode << endl;
                FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                ICMPService *o=ICMPService::cast(nobj);
                o->setName(name);
                o->setInt("type",atoi(ICMPtype.c_str()));
                o->setInt("code",atoi(ICMPcode.c_str()));
            }
            else if (objtype==IPService::TYPENAME)
            {
                cout    << _("Protocol number: ") << protocol << endl
                        << _("Flags: ") << bitmap << endl;
                FWObject *nobj=createObject(objtype,"/"+lib+"/"+systemGroupPaths[objtype]);
                IPService *o=IPService::cast(nobj);
                o->setName(name);
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
                
            }
            
            cout  << endl;
        }
        else
        {
            FWObject *obj=getObject(object.c_str());
            if (cmd==DELOBJECT)
            {
                cout << _("Removing object '") << obj->getName()
                 << _("' from the tree.") << endl;
                objdb->removeAllInstances(obj);
            }
            if (cmd==ADDGRP)
            {
                Group *grp=Group::cast(getObject(group.c_str()));
                cout << _("Adding object '") << obj->getName()
                 << _("' to the group '") << grp->getName()
                 << "'" << endl;
                grp->addRef(obj);
            }
            if (cmd==REMGRP)
            {
                Group *grp=Group::cast(getObject(group.c_str()));
                cout << _("Removing object '") << obj->getName()
                 << _("' from the group '") << grp->getName()
                 << "'" << endl;
                grp->removeRef(obj);
            }
        }

        string bakfile=filename+".bak";
    
        rename(filename.c_str(),bakfile.c_str());
    objdb->saveFile(filename);

    } catch(FWException &ex)  {
    cerr << ex.toString() << endl;
        exit(1);
    } catch (std::string s) {
    cerr << s;
        exit(1);
    } catch (std::exception ex) {
    cerr << ex.what();
        exit(1);
    } catch (...) {
    cerr << _("Unsupported exception");
        exit(1);
    }

    return(0);
}


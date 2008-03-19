/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: OSConfigurator_pix_os_fixups.cpp,v 1.1 2008/03/06 06:49:00 vkurland Exp $

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


#include "OSConfigurator_pix_os.h"
#include "Helper.h"
#include "fwbuilder/Resources.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

#include <list>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <iostream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

/* ********************************************************************
 * 
 *     Generating old school fixup commands for PIX 6.X
 *     and FWSM 2.3
 *
 * ********************************************************************/


/*
 * Copy this method into class PIXAdvancedFWDialog in fwbuilder/src/pix
 */
string OSConfigurator_pix_os::_printFixupCommand(const string &fixup_name,
                                                 const string &sw,
                                                 int   arg1, 
                                                 int   arg2,
                                                 bool  ov)
{
    ostringstream res;

    if (sw=="0")
    {
        if (fixup_name=="dns")
        {
            if (arg1)
            {
                res << "fixup protocol " << fixup_name;
                res << " maximum-length " << arg1;
                res << endl;
            }
        } else
        {
            if (fixup_name=="ftp")
            {
                if (arg1)
                {
                    res << "fixup protocol " << fixup_name << " ";
                    if (ov) res << "strict ";
                    res << arg1;
                    res << endl;
                }
            } else
            {
                if (fixup_name=="mgcp")
                {
                    if (arg1)
                    {
                        res << "fixup protocol " << fixup_name << " ";
                        res << arg1;
                        res << endl;
                    }
                    if (arg2) 
                    {
                        res << "fixup protocol " << fixup_name << " ";
                        res << arg2;
                        res << endl;
                    }
                } else 
                {
                    res << "fixup protocol " << fixup_name << " ";
                    if (arg1) res << arg1;
                    if (arg2 && arg1!=arg2) res << "-" << arg2;
                    res << endl;
                }
            }
        }
    }
    if (sw=="1")
    {
        res << "no fixup protocol " << fixup_name;
        res << endl;
    }

    return res.str();
}

string OSConfigurator_pix_os::_printFixups()
{
    ostringstream  res;
    string platform=fw->getStr("platform");
    string version=fw->getStr("version");

    FWOptions *options=fw->getOptionsObject();
    assert(options!=NULL);


    string lst=Resources::platform_res[platform]->getResourceStr(
            "/FWBuilderResources/Target/options/version_"+version+"/fixups/list");

    string::size_type i,j, k;
    i=0;
    while ( i<lst.size() )
    {
        j=lst.find(",",i);
        string fixup_xml_element=lst.substr(i,j-i);

        i=j+1;

        string f=options->getStr(fixup_xml_element);

        if (!f.empty())
        {
            string fixup_name=fixup_xml_element.substr(0, fixup_xml_element.find("_fixup") );

            while ( (k=fixup_name.find("_"))!=string::npos ) 
                fixup_name.replace(k,1,1,' ');

            string sw;
            int    arg1,arg2;
            string on;
            bool   ov;
            istringstream str(f);

            str >> sw >> arg1 >> arg2 >> on >> ov;

            res << _printFixupCommand(fixup_name, sw, arg1, arg2, ov );
        }
        if (j==string::npos) break;
    }

    return res.str();
}


/* ********************************************************************
 * 
 *     Generating class-map, class and match commands instead of fixups
 *     for PIX 7.0
 *
 * ********************************************************************/

class InspectionProtocol;
std::map<std::string,InspectionProtocol*> protocols;
enum { FIXUP_ENABLE=0, FIXUP_DISABLE=1, FIXUP_SKIP=2 } fixupStatus;

/*
 * par1 and par2 are parameters for the inspection protocol. These are
 * port numbers most of the time, but for some protocols the meaning
 * may be different. For example for dns it is "maximum-length".
 */
class InspectionProtocol {
    public:

    string name;
    string printable_name;
    string ip_proto;
    int    par1,par2;

    InspectionProtocol(const string &fn,
                       const string &prn,
                       const string &pn,
                       int p1,
                       int p2)
    {
        name=fn; printable_name=prn; ip_proto=pn; par1=p1; par2=p2;
        if (protocols.count(fn)==0) protocols[fn]=this;
    }
};

/*
 *  Default ports are defined here jsut like they are filled in the
 *  options by the GUI. If the GUI allows for port range, we specify
 *  port range here, and vice versa. Some of the cases seem to differ
 *  from what Cisco doc specify in the table of the default ports here
 *  http://www.cisco.com/en/US/products/sw/secursw/ps2120/products_upgrade_guides09186a0080369ee2.html
 *  I suppose this is ok since we always can use port range map with
 *  "match" command even if they did not intend it to be like that by
 *  default. However if the GUI returned port numbers that match those
 *  defined in protocolDefinitions, we do not generate 'match' commands
 *  at all and put everything in the "inspection_default" class-map
 *
 *  Here is how this works: constructor of the class InspectionProtocols 
 *  adds object to map 'protocols'. Every initialization of an object
 *  of this class in array protocolDefinitions calls constructor and 
 *  therefore creates an entry in the map 'protocols'. It is done this
 *  way because we can statically initialize an array but cant initialize
 *  std::map  (at least I do not know how)
 *
 *  Note: in PIX 7.0 inspector that corresponds to fixup 'smtp' is
 *  called 'esmtp'
 */
InspectionProtocol protocolDefinitions[] = 
{ 
    InspectionProtocol("ctiqbe",    "ctiqbe",    "tcp",   2748,  0     ),
    InspectionProtocol("dns",       "dns",       "udp",   53,    0     ),
    InspectionProtocol("ftp",       "ftp",       "tcp",   21,    0     ),
    InspectionProtocol("gtp",       "gtp",       "udp",   2123,  3386  ),
    InspectionProtocol("h323_h225", "h323 h225", "tcp",   1720,  1720  ),
    InspectionProtocol("h323_ras",  "h323 ras",  "udp",   1718,  1719  ),
    InspectionProtocol("http",      "http",      "tcp",   80,    80    ),
    InspectionProtocol("icmp_error","icmp",      "icmp",  0,     0     ),
    InspectionProtocol("ils",       "ils",       "tcp",   389,   389   ),
    InspectionProtocol("mgcp",      "mgcp",      "udp",   2427,  2727  ),
    InspectionProtocol("netbios",   "netbios",   "udp",   137,   138   ),
    InspectionProtocol("rpc",       "rpc",       "udp",   111,   0     ),
    InspectionProtocol("rsh",       "rsh",       "tcp",   514,   0     ),
    InspectionProtocol("rtsp",      "rtsp",      "tcp",   554,   0     ),
    InspectionProtocol("sip",       "sip",       "tcp",   5060,  5060  ),
    InspectionProtocol("sip_udp",   "sip",       "udp",   5060,  0     ),
    InspectionProtocol("skinny",    "skinny",    "tcp",   2000,  2000  ),
    InspectionProtocol("smtp",      "esmtp",     "tcp",   25,    25    ),
    InspectionProtocol("sqlnet",    "sqlnet",    "tcp",   1521,  1521  ),
    InspectionProtocol("tftp",      "tftp",      "udp",   69,    0     ),
    InspectionProtocol("xdmcp",     "xdmcp",     "udp",   177,   0     ),
};


/*
 *  status:
 *    0:  enable
 *    1:  disable
 *    2:  skip
 */
class InspectionClassMap {
    public:
    
    string class_map_name;
    string fixup_name;
    string inspect_name;
    int    status;
    int    port1,port2;
    string arg_name;
    int    arg_val;

    InspectionClassMap(const string &fn,int s,int p1,int p2,const string &a,int v)
    {
        status=s; port1=p1; port2=p2; arg_name=a; arg_val=v;
        string ss = fn;
        string::size_type k;
        while ( (k=ss.find(" "))!=string::npos ) 
            ss.replace(k,1,1,'_');
        inspect_name = ss;
        fixup_name = fn;
        class_map_name = string("custom_")+ss+string("_inspection");
    }

    bool isDefault();
    string getIPProtocol();
    string getPrintableName();
    string getMatchCommand();
};

std::list<InspectionClassMap> defaultClassMaps;
std::list<InspectionClassMap> customClassMaps;
std::map<std::string,int>    DefaultInspectionInspectStatements;
std::map<std::string,int>    CustomInspectionInspectStatements;

bool InspectionClassMap::isDefault()
{
    InspectionProtocol *ip = protocols[fixup_name];
    if (ip!=NULL) return (ip->par1==port1 && ip->par2==port2);
    return false;
}

string InspectionClassMap::getIPProtocol()
{
    InspectionProtocol *ip = protocols[fixup_name];
    if (ip!=NULL) return ip->ip_proto;
    return "";
}

string InspectionClassMap::getPrintableName()
{
    InspectionProtocol *ip = protocols[fixup_name];
    if (ip!=NULL) return ip->printable_name;
    return "";
}

string InspectionClassMap::getMatchCommand()
{
    ostringstream  res;
    res << "match port " << getIPProtocol() << " ";
    if (port1!=0 && port2==0)
        res << "eq " << port1;
    if (port1!=0 && port1==port2)
        res << "eq " << port1;
    if (port1!=0 && port2!=0 && port1!=port2)
        res << "range " << port1 << " " << port2;
    res << endl;
    return res.str();
}

string OSConfigurator_pix_os::_printMPF()
{
    ostringstream  res;
    string platform=fw->getStr("platform");
    string version=fw->getStr("version");

    FWOptions *options=fw->getOptionsObject();
    assert(options!=NULL);

    string lst=Resources::platform_res[platform]->getResourceStr(
            "/FWBuilderResources/Target/options/version_"+version+"/fixups/list");

    defaultClassMaps.clear();
    customClassMaps.clear();
    DefaultInspectionInspectStatements.clear();
    CustomInspectionInspectStatements.clear();

    string::size_type i,j;
    i=0;
    while ( i<lst.size() )
    {
        j=lst.find(",",i);
        string fixup_xml_element=lst.substr(i,j-i);

        i=j+1;

        string f=options->getStr(fixup_xml_element);

        if (!f.empty())
        {
            string fixup_name=fixup_xml_element.substr(0, fixup_xml_element.find("_fixup") );

            int    status;
            int    p1,p2;
            string an;
            int    av;
            istringstream str(f);

            str >> status >> p1 >> p2 >> an >> av;

/* We should really fix this in the GUI and pass max length parameter
 * as an/av rather than as port p1
 */
            if (fixup_name=="dns" && p1!=0) { an="maximum-length"; av=p1; p1=53; }

            InspectionClassMap cm(fixup_name,status,p1,p2,an,av);
            if (cm.isDefault()) defaultClassMaps.push_back(cm);
            else                customClassMaps.push_back(cm);
        }
        if (j==string::npos) break;
    }

    res << "class-map inspection_default" << endl;
    res << "  match default-inspection-traffic" << endl;
    res << endl;

    std::list<InspectionClassMap>::iterator i1;

    if (customClassMaps.size()>0)
    {
        for (i1=customClassMaps.begin(); i1!=customClassMaps.end(); i1++)
        {
            res << "class-map " << i1->class_map_name << endl;
            res << "  " << i1->getMatchCommand() << endl;
        }
        res << endl;
    }
    
    res << "policy-map global_policy" << endl;
    if (defaultClassMaps.size()>0)
    {
        res << "  class inspection_default" << endl;
        for (i1=defaultClassMaps.begin(); i1!=defaultClassMaps.end(); i1++)
        {
            string pn = i1->getPrintableName();
            if (i1->status!=FIXUP_SKIP && 
                DefaultInspectionInspectStatements[pn]!=1)
            {
                res << "    ";
                if (i1->status==FIXUP_DISABLE) res << "no ";
                res << "inspect " << pn << endl;
                DefaultInspectionInspectStatements[pn]=1;
            }
        }
    }
    if (customClassMaps.size()>0)
    {
        for (i1=customClassMaps.begin(); i1!=customClassMaps.end(); i1++)
        {
            string pn = i1->getPrintableName();
            if (i1->status!=FIXUP_SKIP &&
                CustomInspectionInspectStatements[pn]!=1)
            {
                res << "  class " << i1->class_map_name << endl;
                res << "    ";
                if (i1->status==FIXUP_DISABLE) res << "no ";
                res << "inspect " << i1->getPrintableName() << endl;
                CustomInspectionInspectStatements[pn]=1;
            }
        }
    }
    res << endl;

    res << "service-policy global_policy global" << endl;

    return res.str();
}


string OSConfigurator_pix_os::getProtocolInspectionCommands()
{
    string platform=fw->getStr("platform");
    string version=fw->getStr("version");

    if (Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/version_"+version+"/fixups/use_mpf"))
        return _printMPF();

    return _printFixups();
}

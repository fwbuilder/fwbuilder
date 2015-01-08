/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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


#include "OSConfigurator_ios.h"
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

string OSConfigurator_ios::myPlatformName() { return "ios"; }

int OSConfigurator_ios::prolog()
{
    string host_os = fw->getStr("host_OS");

    if (host_os!="ios")
	abort("Unsupported OS " + host_os );

    return Compiler::prolog();
}


void OSConfigurator_ios::processFirewallOptions() 
{
//    FWOptions* options=fw->getOptionsObject();
    string s;
//    int    i;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");


    if ( fw->getOptionsObject()->getBool("ios_set_host_name") )
    {
        output << "hostname " << fw->getName() << endl;
        output << endl;
    }

    output << _printNameif();
    output << endl;
    output << _printIPAddress();
    output << endl;
    output << _printLogging();
    output << endl;
}

string OSConfigurator_ios::_printNameif()
{
    ostringstream res;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    string::size_type n;

    list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
    {
	Interface *iface=dynamic_cast<Interface*>(*i);
	assert(iface);

        string nameifCmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/version_")+
            version+"/ios_commands/nameif");
        
        if ((n = nameifCmd.find("%il"))!=string::npos)
            nameifCmd.replace(n,3,iface->getLabel());
        if ((n = nameifCmd.find("%in"))!=string::npos)
            nameifCmd.replace(n,3,iface->getName());
        res << nameifCmd;
    }

    res << endl;

    return res.str();
}

string OSConfigurator_ios::_printIPAddress()
{
    ostringstream res;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    string setAddrCmd;
    string::size_type n;

    if ( fw->getOptionsObject()->getBool("ios_ip_address") )
    {
        list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);
            if (iface->isDyn())
            {
                setAddrCmd = Resources::platform_res[platform]->getResourceStr(
                    string("/FWBuilderResources/Target/options/version_")+
                    version+"/ios_commands/ip_addr_dyn");
            }
            else 
            {
                if (iface->isUnnumbered())
                {
                    setAddrCmd = "";
                } else
                {
                    setAddrCmd = Resources::platform_res[platform]->getResourceStr(
                        string("/FWBuilderResources/Target/options/version_")+
                        version+"/ios_commands/ip_addr_static");
                }
            }

            if ((n = setAddrCmd.find("%il"))!=string::npos)
                setAddrCmd.replace(n,3,iface->getLabel());
            if ((n = setAddrCmd.find("%in"))!=string::npos)
                setAddrCmd.replace(n,3,iface->getName());
            if ((n = setAddrCmd.find("%a"))!=string::npos)
                setAddrCmd.replace(n,2,iface->getAddressPtr()->toString());
            if ((n = setAddrCmd.find("%n"))!=string::npos)
                setAddrCmd.replace(n,2,iface->getNetmaskPtr()->toString());

            res << setAddrCmd;
        }        
    }

    res << endl;

    return res.str();
}


string OSConfigurator_ios::_printLogging()
{
    Helper helper(this);

    ostringstream  str;
    //bool logging_on=false; //UNUSED

    bool iosacl_generate_logging_commands = fw->getOptionsObject()->getBool(
        "iosacl_generate_logging_commands");

    if (iosacl_generate_logging_commands)
    {
        string syslog_host = fw->getOptionsObject()->getStr("iosacl_syslog_host");
        string syslog_facility= fw->getOptionsObject()->getStr("iosacl_syslog_facility");
        string trap_level= fw->getOptionsObject()->getStr("iosacl_logging_trap_level");

        bool buffered = fw->getOptionsObject()->getBool("iosacl_logging_buffered");
        string buffered_level = fw->getOptionsObject()->getStr("iosacl_logging_buffered_level");

        bool console = fw->getOptionsObject()->getBool("iosacl_logging_console");
        string console_level = fw->getOptionsObject()->getStr("iosacl_logging_console_level");

        bool timestamp = fw->getOptionsObject()->getBool("iosacl_logging_timestamp");

        if ( ! timestamp ) str << "no ";
        str << "service timestamp log datetime localtime" << endl;

        if ( ! syslog_host.empty() )
        {
            str << endl;

            str << "logging host " << syslog_host << endl;

            if ( ! syslog_facility.empty() )
                str << "logging facility " << syslog_facility <<  endl;

            if ( ! trap_level.empty() )
                str << "logging trap " << trap_level << endl;

            //logging_on=true;
        }

        if ( ! buffered ) str << "no logging buffered" << endl;
        else
        {
            str << "logging buffered " << buffered_level << endl;
            //logging_on=true;
        }

        if ( ! console )  str << "no logging console" << endl;
        else
        {
            str << "logging console " << console_level << endl;
            //logging_on=true;
        }

        str << endl;
    }
    return str.str();
}

void OSConfigurator_ios::addVirtualAddressForNAT(const Address*)
{
}

void OSConfigurator_ios::addVirtualAddressForNAT(const Network*)
{
}


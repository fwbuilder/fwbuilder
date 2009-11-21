/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "../../config.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <assert.h>
#include <string>
#include <cstring>
#include <iomanip>

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"

#include "CompilerDriver_iosacl.h"
#include "PolicyCompiler_iosacl.h"

#include <QFileInfo>
#include <QDir>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

#ifdef _WIN32
string fs_separator = "\\";
#else
string fs_separator = "/";
#endif


CompilerDriver_iosacl::CompilerDriver_iosacl(FWObjectDatabase *db) :
    CompilerDriver(db)
{
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_iosacl::clone()
{
    return new CompilerDriver_iosacl(objdb);
}

void CompilerDriver_iosacl::printProlog(QTextStream &file, const string &prolog_code)
{
    file << endl;
    file << "#" << endl;
    file << "# Prolog script" << endl;
    file << "#" << endl;
    file << prolog_code << endl;
    file << "#" << endl;
    file << "# End of prolog script" << endl;
    file << "#" << endl;
}

string CompilerDriver_iosacl::safetyNetInstall(Firewall *fw)
{
    ostringstream output;
    if ( fw->getOptionsObject()->getBool("iosacl_acl_substitution") )
    {
        /* Generate short temporary ACL and assign it to all
         * interfaces. This ACL permits IPSEC (IP proto 50 and UDP port 500)
         as well as ssh from given subnet to any.
        */

        string platform = fw->getStr("platform");
        string version = fw->getStr("version");

        string temp_acl = "tmp_acl";
        string temp_acl_addr = fw->getOptionsObject()->getStr(
            "iosacl_acl_temp_addr");

        if (temp_acl_addr.empty())
        {
            cerr << "Missing address for management host or subnet for temporary ACL.\nPlease enter it in the tab 'Script options' in 'Firewall Settings' dialog"
                 << endl;
            exit(-1);
        }

        // if templ_acl_addr is ipv4 address, then we can not create this
        // temporary ACL while compiling ipv6 policy. And vice versa.

        bool create_temp_acl = false;
        bool tmp_acl_ipv6 = false;
        if (temp_acl_addr.find(":")!=string::npos)
        {
            //looks like ipv6
            create_temp_acl = true;
            tmp_acl_ipv6 = true;
        } else
        {
            // not ipv6, assume ipv4
            create_temp_acl = true;
            tmp_acl_ipv6 = false;
        }

        if (create_temp_acl)
        {
            string::size_type slash_idx = temp_acl_addr.find('/');
            string addr = temp_acl_addr;
            string netmask = "255.255.255.255";
            bool tmp_acl_v6 = false;

            // check if addr is v6

            try
            {
                InetAddr addrv6(AF_INET6, temp_acl_addr);
                tmp_acl_v6 = true;
            } catch(FWException &ex)
            {
                // Assume cnf->maddr is ipv4 
                if (slash_idx!=string::npos)
                {
                    addr = temp_acl_addr.substr(0,slash_idx);
                    netmask = temp_acl_addr.substr(slash_idx+1);
                    try
                    {
                        if (netmask.find(".")!=string::npos)
                        {
                            InetAddr nm(netmask);
                            nm.getLength(); // to avoid warning abt unused var
                        } else
                        {
                            int nm_length;
                            istringstream  str(netmask);
                            str >> nm_length;
                            InetAddr nm(nm_length);
                            netmask = nm.toString();
                        }
                    } catch(FWException &ex)
                    {
                        cerr << "Invalid netmask for management subnet: '"+netmask+"'"
                             << endl;
                        exit(-1);
                    }
                }

                try
                {
                    InetAddr a(addr);
                    a.isAny();
                } catch(FWException &ex)
                {
                    cerr << "Invalid address for management subnet: '"+addr+"'"
                         << endl;
                    exit(-1);
                }
            }

            string xml_element = "clear_ip_acl";
            if (tmp_acl_ipv6) xml_element = "clear_ipv6_acl";

            string clearACLcmd = Resources::platform_res[platform]->getResourceStr(
                string("/FWBuilderResources/Target/options/")+
                "version_"+version+"/iosacl_commands/" + xml_element);

            output << endl;

            string addr_family_prefix = "ip";

            string access_group_cmd =
                PolicyCompiler_iosacl::getAccessGroupCommandForAddressFamily(tmp_acl_v6);

            output << "! temporary access list for \"safety net install\""
                   << endl;
            output << endl;

            if (tmp_acl_v6)
            {
                addr_family_prefix = "ipv6";
                output << clearACLcmd << " " << temp_acl << endl;
                output << "ipv6 access-list " << temp_acl << endl;
                if (slash_idx!=string::npos)
                    output << "  permit ipv6 " << addr << " any " << endl;
                else
                    output << "  permit ipv6 host " << addr << " any " << endl;
                output << "  permit icmp any any " << endl;
                output << "  deny ipv6 any any " << endl;
                output << "exit" << endl;
                output << endl;
            } else
            {
                // cisco uses "wildcards" instead of netmasks

                //long nm = InetAddr(netmask).to32BitInt();
                //struct in_addr na;
                //na.s_addr = ~nm;
                InetAddr nnm( ~(InetAddr(netmask)) );
                addr_family_prefix = "ip";
                output << clearACLcmd << " " << temp_acl << endl;
                output << "ip access-list extended " << temp_acl << endl;
                output << "  permit ip "
                       << addr << " " << nnm.toString() << " any " << endl;
                output << "  deny ip any any " << endl;
                output << "exit" << endl;
                output << endl;
            }

            // find management interface
            int nmi = 0;
            list<FWObject*> ll = fw->getByType(Interface::TYPENAME);
            for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
            {
                Interface *intf = Interface::cast( *i );
                if (intf->isManagement())
                {
                    nmi++;
                    output << "interface " << intf->getName() << endl;
                    output << "  no " << addr_family_prefix << " ";
                    output << access_group_cmd;
                    output << " in" << endl;

                    output << "  no " << addr_family_prefix << " ";
                    output << access_group_cmd;
                    output << " out" << endl;

                    output << " " << addr_family_prefix << " ";
                    output << access_group_cmd;
                    output << " " << temp_acl << " in" << endl;
                    output << "exit" << endl;
                }
            }
            if (nmi==0)
            {
                cerr << "One of the interfaces of the firewall must be marked as management interface."
                     << endl;
                exit(-1);
            }

            output << endl;
        }
    }
    return output.str();
}


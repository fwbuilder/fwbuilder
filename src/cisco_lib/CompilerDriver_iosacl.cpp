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

#include "Configlet.h"
#include "CompilerDriver_iosacl.h"
#include "PolicyCompiler_iosacl.h"
#include "ACL.h"
#include "BaseObjectGroup.h"
#include "NamedObjectsAndGroupsSupport.h"

#include <QString>
#include <QFileInfo>
#include <QDir>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

#ifdef _WIN32
static string fs_separator = "\\";
#else
static string fs_separator = "/";
#endif


CompilerDriver_iosacl::CompilerDriver_iosacl(FWObjectDatabase *db) :
    CompilerDriver(db)
{
    safety_net_install_option_name = "iosacl_acl_substitution";
    safety_net_install_acl_addr_option_name = "iosacl_acl_temp_addr";
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_iosacl::clone()
{
    CompilerDriver_iosacl* new_cd = new CompilerDriver_iosacl(objdb);
    if (inEmbeddedMode()) new_cd->setEmbeddedMode();
    return new_cd;
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
    if ( fw->getOptionsObject()->getBool(safety_net_install_option_name) )
    {
        /* Generate short temporary ACL and assign it to all
         * interfaces. This ACL permits IPSEC (IP proto 50 and UDP port 500)
         as well as ssh from given subnet to any.
        */

        string temp_acl_addr = fw->getOptionsObject()->getStr(
            safety_net_install_acl_addr_option_name);

        if (temp_acl_addr.empty())
        {
            QString err = QObject::tr("Missing address for management host or subnet "
                                      "for the temporary ACL.\nPlease enter it in the "
                                      "tab 'Script options' in 'Firewall Settings' dialog");
            abort(fw, nullptr, nullptr, err.toStdString());
        }

        // if templ_acl_addr is ipv4 address, then we can not create this
        // temporary ACL while compiling ipv6 policy. And vice versa.

        bool create_temp_acl = false;
        //bool tmp_acl_ipv6 = false; //UNUSED
        if (temp_acl_addr.find(":")!=string::npos)
        {
            //looks like ipv6
            create_temp_acl = true;
            //tmp_acl_ipv6 = true;
        } else
        {
            // not ipv6, assume ipv4
            create_temp_acl = true;
            //tmp_acl_ipv6 = false;
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
                        QString err = QObject::tr("Invalid netmask for management subnet: "
                                                  "'%1'").arg(netmask.c_str());
                        abort(fw, nullptr, nullptr, err.toStdString());
                    }
                }

                try
                {
                    InetAddr a(addr);
                    a.isAny();
                } catch(FWException &ex)
                {
                    QString err = QObject::tr("Invalid address for management subnet: "
                                              "'%1'").arg(addr.c_str());
                    abort(fw, nullptr, nullptr, err.toStdString());
                }
            }

            Configlet configlet(fw, "cisco", "safety_net_acl");
            configlet.collapseEmptyStrings(true);

            if (tmp_acl_v6)
            {
                configlet.setVariable("ipv4",   false);
                configlet.setVariable("ipv6",   true);
                configlet.setVariable("slash_notation", slash_idx!=string::npos);
                configlet.setVariable("host_addr", slash_idx==string::npos);
                configlet.setVariable("management_addr", addr.c_str());
                configlet.setVariable("management_netm", "");
            } else 
            {
                InetAddr nnm( ~(InetAddr(netmask)) );
                configlet.setVariable("ipv4", true);
                configlet.setVariable("ipv6", false);
                configlet.setVariable("management_addr", addr.c_str());
                configlet.setVariable("management_netm", nnm.toString().c_str());
            }

            // find management interface
            list<FWObject*> ll = fw->getByType(Interface::TYPENAME);
            for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
            {
                Interface *intf = Interface::cast( *i );
                if (intf->isManagement())
                {
                    configlet.setVariable("management_interface",
                                          intf->getName().c_str());

                    FWOptions *ifopt = intf->getOptionsObject();
                    string itype = ifopt->getStr("type");
                    configlet.setVariable("management_interface_is_vlan",
                                          (itype == "8021q"));
                    configlet.setVariable("management_interface_is_not_vlan",
                                          (itype != "8021q"));

                    if (itype == "8021q")
                        configlet.setVariable("management_interface_vlan_id",
                                              ifopt->getInt("vlan_id"));
                    else
                        configlet.setVariable("management_interface_vlan_id", "");

                    break;
                }
            }
            output << configlet.expand().toStdString();
            output << endl;
        }
    }
    return output.str();
}


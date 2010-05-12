/* 

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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

#include "PolicyCompiler_procurve_acl.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleSet.h"

#include <iostream>

#include <assert.h>

#include <QStringList>
#include <QString>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


void PolicyCompiler_procurve_acl::_printClearCommands()
{
    string vers = fw->getStr("version");
    string platform = fw->getStr("platform");

    string xml_element = "clear_ip_acl";
    if (ipv6) xml_element = "clear_ipv6_acl";

    string clearACLCmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+"/procurve_acl_commands/" + xml_element);

    assert( !clearACLCmd.empty());

    if (!inSingleRuleCompileMode())
    {
        // No need to output "clear" commands in single rule compile mode
        if ( fw->getOptionsObject()->getBool("procurve_acl_acl_basic") ||
             fw->getOptionsObject()->getBool("procurve_acl_acl_substitution"))
        {
            for (map<string,ciscoACL*>::iterator i=acls.begin(); i!=acls.end(); ++i)
            {
                ciscoACL *acl = (*i).second;
                output << printAccessGroupCmd(acl, true);
                output << clearACLCmd << " " << acl->workName() << endl;
                output << endl;
            }
            output << endl;
        }
    }
    output << endl;
}
        
string PolicyCompiler_procurve_acl::printAccessGroupCmd(ciscoACL *acl, bool neg)
{
    if (getSourceRuleSet()->isTop())
    {
        QString dir;
        if (acl->direction()=="in"  || acl->direction()=="Inbound")  dir="in";
        if (acl->direction()=="out" || acl->direction()=="Outbound") dir="out";

        QString addr_family_prefix = "ip";
        if (ipv6) addr_family_prefix = "ipv6";

        // ProCurve uses different syntax for vlan ACLs
        Interface *intf = acl->getInterface();
        FWOptions *ifopt = intf->getOptionsObject();
        string itype = ifopt->getStr("type");

        if (itype == "8021q")
        {
            int vlan_id = ifopt->getInt("vlan_id");
            QStringList outp;
            if (neg) outp.push_back("no");
            outp.push_back("vlan");
            outp.push_back(QString("%1").arg(vlan_id));
            outp.push_back(addr_family_prefix);
            outp.push_back(getAccessGroupCommandForAddressFamily(ipv6).c_str());
            outp.push_back(acl->workName().c_str());
            outp.push_back(dir);
            return outp.join(" ").toStdString() + "\n";
        } else
        {
            QStringList outp;
            QStringList outp_combined;
            outp_combined.push_back(
                QString("interface %1").arg(intf->getName().c_str()));

            if (neg) outp.push_back("no");
            outp.push_back(addr_family_prefix);
            outp.push_back(getAccessGroupCommandForAddressFamily(ipv6).c_str());
            outp.push_back(acl->workName().c_str());
            outp.push_back(dir);

            outp_combined.push_back("  " + outp.join(" "));
            outp_combined.push_back("exit");
            outp_combined.push_back("");
            return outp_combined.join("\n").toStdString();
        }
    }
    return "";
}



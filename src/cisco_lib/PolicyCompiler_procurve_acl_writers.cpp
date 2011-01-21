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



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

#include <iostream>

#include <assert.h>


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
        


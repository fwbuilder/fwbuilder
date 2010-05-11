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

#include "config.h"

#include "PolicyCompiler_procurve_acl.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/RuleSet.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


string PolicyCompiler_procurve_acl::myPlatformName() { return "procurve_acl"; }

PolicyCompiler_procurve_acl::PolicyCompiler_procurve_acl(FWObjectDatabase *_db,
                                                         Firewall *fw,
                                                         bool ipv6_policy,
                                                         OSConfigurator *_oscnf) :
    PolicyCompiler_iosacl(_db, fw, ipv6_policy, _oscnf)
{
    comment_symbol = ";";
}

int PolicyCompiler_procurve_acl::prolog()
{
    string platform = fw->getStr("platform");

    if (platform!="procurve_acl")
	abort("Unsupported platform " + platform );

    /* This is optional for PIX (controller by a checkbox in
     * "asvanced" settings dialog) and is hardcoded as "true" for
     * iosacl in PolicyCompiler_cisco::pickACL::processNext(). I do
     * not want a function in the base class PolicyCompiler_cisco be
     * aware of yet another platform, especially one that is not
     * strictly speaking Cisco. Just set this option here which is
     * equivalent to hardcoding it to true.
     * 
     * TODO: use the same method in PolicyCompiler_iosacl
     */
    fw->getOptionsObject()->setBool("generate_out_acl", true);


    return PolicyCompiler::prolog();
}



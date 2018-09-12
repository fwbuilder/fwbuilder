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


#include "CompilerDriverFactory.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWObjectDatabase.h"

#include "CompilerDriver_ipt.h"
#include "CompilerDriver_pf.h"
#include "CompilerDriver_ipf.h"
#include "CompilerDriver_ipfw.h"
#include "CompilerDriver_iosacl.h"
#include "CompilerDriver_pix.h"
#include "CompilerDriver_procurve_acl.h"
#include "CompilerDriver_nxosacl.h"
#include "../juniper_lib/CompilerDriver_junosacl.h"

#include <string>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


CompilerDriver* CompilerDriverFactory::createCompilerDriver(Firewall *fw)
{
    string platform = fw->getStr("platform");
    if (platform == "iptables") return new CompilerDriver_ipt(fw->getRoot());
    if (platform == "pf") return new CompilerDriver_pf(fw->getRoot());
    if (platform == "ipf") return new CompilerDriver_ipf(fw->getRoot());
    if (platform == "ipfw") return new CompilerDriver_ipfw(fw->getRoot());
    if (platform == "iosacl") return new CompilerDriver_iosacl(fw->getRoot());
    if (platform == "nxosacl") return new CompilerDriver_nxosacl(fw->getRoot());
    if (platform == "junosacl") return new CompilerDriver_junosacl(fw->getRoot());
    if (platform == "pix" || platform == "fwsm")
        return new CompilerDriver_pix(fw->getRoot());
    if (platform == "procurve_acl")
        return new CompilerDriver_procurve_acl(fw->getRoot());
    return nullptr;
}


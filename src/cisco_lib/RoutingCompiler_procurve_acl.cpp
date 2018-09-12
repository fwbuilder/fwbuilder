/* 

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: RoutingCompiler_procurve.cpp -1   $

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


#include "RoutingCompiler_procurve_acl.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;


string RoutingCompiler_procurve_acl::myPlatformName() { return "procurve_acl"; }

int RoutingCompiler_procurve_acl::prolog()
{
    int n = RoutingCompiler_cisco::prolog();    

    if (fw->getStr("platform")!="procurve_acl") 
	abort("Unsupported platform " + fw->getStr("platform") );

    return n;
}


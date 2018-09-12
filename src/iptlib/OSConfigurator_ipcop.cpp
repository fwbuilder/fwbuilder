/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: OSConfigurator_linux24.cpp 756 2009-02-06 00:58:01Z vadim $

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



#include "OSConfigurator_ipcop.h"


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string OSConfigurator_ipcop::myPlatformName() { return "ipcop"; }

OSConfigurator_ipcop::OSConfigurator_ipcop(FWObjectDatabase *_db,
                                           Firewall *fw,
                                           bool ipv6_policy) : 
    OSConfigurator_linux24(_db, fw, ipv6_policy)
{
}

void OSConfigurator_ipcop::addVirtualAddressForNAT(const Network*)
{
}

void OSConfigurator_ipcop::addVirtualAddressForNAT(const Address*)
{
}

int OSConfigurator_ipcop::prolog()
{
    return 0;
}

void OSConfigurator_ipcop::epilog()
{
}

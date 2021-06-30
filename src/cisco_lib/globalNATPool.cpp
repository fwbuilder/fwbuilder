/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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

#include "NATCompiler_pix.h"
#include "helpers.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Interface.h"


using namespace libfwbuilder;
using namespace fwcompiler;

globalNATPool::globalNATPool(int id,Interface *ifs,Address *a)
{
    nat_id=id;
    iface=ifs;
    addr= a;
    if (Interface::cast(a)!=nullptr || iface->isDyn()) {
        type=INTERFACE;
    } else {
        type= (AddressRange::cast(a)!=nullptr)?ADDRESS_RANGE:SINGLE_ADDRESS;
    }
}

/*
 *  global pool prints itself only once
 */
ostream& fwcompiler::operator<<(ostream &s,const globalNATPool &pool)
{
    s << "global (" << pool.iface->getLabel() << ") " << pool.pool_no;

    switch (pool.type) {
    case globalNATPool::INTERFACE:
        s << " interface" << endl;
        break;
    case globalNATPool::SINGLE_ADDRESS:
        s << " " << pool.addr->getAddressPtr()->toString() << endl;
        break;
    case globalNATPool::ADDRESS_RANGE:
        AddressRange *ar=AddressRange::cast(pool.addr);
        s << " " 
          << ar->getRangeStart().toString()
          << "-"
          << ar->getRangeEnd().toString()
          << " netmask "
          << pool.iface->getNetmask().toString()
          << endl;
        break;
    }
    return s;
}


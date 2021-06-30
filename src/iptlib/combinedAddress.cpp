/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@vk.crocodile.org>

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

#include <assert.h>
#include <iostream>


#include "combinedAddress.h"

#include <fwbuilder/FWException.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWObjectDatabase.h>

using namespace libfwbuilder;
using namespace std;

const char *combinedAddress::TYPENAME={"combinedAddress"};

combinedAddress::~combinedAddress() {}

std::string combinedAddress::getPhysAddress() const
{
    return physAddress;
}

void combinedAddress::setPhysAddress(const std::string &s)
{
    physAddress = s;
}

bool combinedAddress::isAny() const
{
    return (IPv4::isAny() && physAddress=="");
}

FWObject& combinedAddress::shallowDuplicate(const FWObject *other,
                                            bool preserve_id)
{
    physAddress = dynamic_cast<const combinedAddress*>(other)->physAddress;
    return IPv4::shallowDuplicate(other, preserve_id);
}


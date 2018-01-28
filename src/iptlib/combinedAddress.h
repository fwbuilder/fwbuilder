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


#ifndef __COMB_ADDRESS_HH_FLAG__
#define __COMB_ADDRESS_HH_FLAG__

#include <fwbuilder/FWObject.h>
#include <fwbuilder/IPv4.h>
#include <fwbuilder/physAddress.h>

namespace libfwbuilder
{

class combinedAddress : public IPv4
{
    private:

    std::string physAddress;

    public:

    DECLARE_FWOBJECT_SUBTYPE(combinedAddress);

    combinedAddress() {}
    virtual ~combinedAddress();

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true);
    
    std::string getPhysAddress() const;
    void setPhysAddress(const std::string &s);

    bool isAny() const;
};

}


#endif


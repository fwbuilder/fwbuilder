/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@vk.crocodile.org>

  $Id: Address.h 975 2006-09-10 22:40:37Z vkurland $

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


#ifndef __GEN_ADDRESS_HH_FLAG__
#define __GEN_ADDRESS_HH_FLAG__

#include <fwbuilder/FWObject.h>
#include <fwbuilder/InetAddr.h>
#include <fwbuilder/InetAddrMask.h>

namespace libfwbuilder
{

/**
 * This is base class for all objects that can have an address and can
 * be a child of RuleElementSrc, RuleElementDst and other rule
 * elements holding addresses.  It is never used on it's own, only
 * it's subclasses are used.
 *
 * TODO: we might need to derive ObjectGroup and AddressRange from Address,
 * but this requires lot more testing
 */
class Address : public FWObject , public InetAddrMask
{
    public:

    DECLARE_FWOBJECT_SUBTYPE(Address);

    Address();
    Address(const FWObject *root,bool prepopulate);
    Address(const Address&);
    Address(const std::string& addr,const std::string& mask);
    Address(const std::string &s) throw(FWException);

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true)
        throw(FWException);
 
    virtual FWReference* createRef();

    bool isAny() const;
};

}


#endif


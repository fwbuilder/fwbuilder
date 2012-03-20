/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Zaliva <lord@crocodile.org>

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


#ifndef __GEN_SERVICE_HH_FLAG__
#define __GEN_SERVICE_HH_FLAG__

#include "fwbuilder/FWObject.h"

namespace libfwbuilder
{

/**
 * This is base class for all service objects and other objects that
 * can be children of RuleElementSrv (service rule element).  It is
 * never used on it's own, only it's subclasses are used.
 *
 * TODO: we might need to derive ServiceGroup from Service, but this
 * requires lot more testing
 */
class Service : public FWObject 
{
    private:
    
    public:

    DECLARE_FWOBJECT_SUBTYPE(Service);

    DECLARE_DISPATCH_METHODS(Service);
    
    Service() {}

    virtual FWReference* createRef();
    virtual std::string getProtocolName() const;
    virtual int getProtocolNumber() const;

    bool isAny() const;

    virtual bool isPrimaryObject() const { return true; }

    // All service objects except for ICMPService, ICMP6Service and
    // CustomService can be used in both ipv4 and ipv6 contexts.
    virtual bool isV4Only() { return false; }
    virtual bool isV6Only() { return false; }
};

}


#endif


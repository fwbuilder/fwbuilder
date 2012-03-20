/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#ifndef __ICMP6SERVICE_HH_FLAG__
#define __ICMP6SERVICE_HH_FLAG__

#include "fwbuilder/ICMPService.h"

namespace libfwbuilder
{

class ICMP6Service : public ICMPService
{

private:

public:

    ICMP6Service();
    virtual ~ICMP6Service();

    DECLARE_FWOBJECT_SUBTYPE(ICMP6Service);

    DECLARE_DISPATCH_METHODS(ICMP6Service);
    
    virtual std::string getProtocolName() const;
    virtual int getProtocolNumber() const;

    virtual bool isV4Only() { return false; }
    virtual bool isV6Only() { return true; }
};

}

#endif





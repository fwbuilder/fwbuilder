/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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


#ifndef __ICMP6SERVICE_HH_FLAG__
#define __ICMP6SERVICE_HH_FLAG__

#include <fwbuilder/ICMPService.h>

namespace libfwbuilder
{

class ICMP6Service : public ICMPService
{

private:

public:

    ICMP6Service();
    ICMP6Service(const FWObject *root,bool prepopulate);
    virtual ~ICMP6Service();

    DECLARE_FWOBJECT_SUBTYPE(ICMP6Service);

    virtual std::string getProtocolName();
    virtual int getProtocolNumber();
};

}

#endif





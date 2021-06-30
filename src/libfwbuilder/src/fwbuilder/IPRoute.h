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

#ifndef __IPROUTE_HH_FLAG__
#define __IPROUTE_HH_FLAG__

#include <string>
#include <vector>

#ifndef _WIN32
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#else
#  include <winsock2.h>
#endif

#include "fwbuilder/FWException.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/InterfaceData.h"

namespace libfwbuilder
{

class Interface;
class IPRoute
{
    public:

    IPRoute(const IPRoute &);
    IPRoute(const InetAddr &_dst, const InetAddr &_nm, const InetAddr &_gw,
            const InterfaceData &_intf,  bool _direct);
    virtual ~IPRoute();

    bool isDirect() const { return direct;}
    const InetAddr &getNetmask    () const { return nm;   }
    const InetAddr &getDestination() const { return dst;  }
    const InetAddr &getGateway    () const { return gw;   }

    /**
     * @return interface associated with this route, or
     * nullptr if none.
     */
    const InterfaceData& getInterface() const { return intf; }
    
    private:
    
    InetAddr nm     ;
    InetAddr dst    ;
    InetAddr gw     ;
    InterfaceData intf;
    bool      direct ;
};

}

#endif


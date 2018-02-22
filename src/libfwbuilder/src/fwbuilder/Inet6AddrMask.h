/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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

/*
 * This class is a holder of a pair address / netmask.
 * It can act both as a container for the address/netmask configuration
 * data (such as for an interface) or as a network.
 *
 */

#ifndef __INET6ADDRMASK_HH_FLAG__
#define __INET6ADDRMASK_HH_FLAG__

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
#include "fwbuilder/InetAddrMask.h"

namespace libfwbuilder
{

class Inet6AddrMask : public InetAddrMask
{
protected:

public:

    Inet6AddrMask();
    Inet6AddrMask(const std::string &s);
    Inet6AddrMask(const InetAddr&, const InetAddr&);
    virtual ~Inet6AddrMask();

    virtual std::string toString() const;
};

}

#endif


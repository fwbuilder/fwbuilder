/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

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


#ifndef __DNS_HH_FLAG__
#define __DNS_HH_FLAG__


#ifndef _WIN32

/* do not change the order of these headers! resolv.h should be included
 * before netdb.h, otherwise there seems to be a problem with macro __P
 * which is defined in cdfes.h as '#define __P(args) args __THROW' and
 * gcc 3.3 does not like this */

#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/nameser.h>
#  include <resolv.h>
#  include <netdb.h>

#else

#  ifndef RES_TIMEOUT
#     define RES_TIMEOUT 1
#  endif

#endif // _WIN32

#include "fwbuilder/InetAddr.h"
#include "fwbuilder/FWException.h"

#include <vector>
#include <map>
#include <queue>
#include "fwbuilder/ThreadTools.h"
#include "fwbuilder/BackgroundOp.h"

#ifndef RES_DFLRETRY 
#  define RES_DFLRETRY 1
#endif

namespace libfwbuilder
{

class HostEnt
{
    public:
    
    std::string           name    ;
    std::set<std::string> aliases ;
};

/**
 * This is abstract class
 */
class DNS
{
public:

    DNS() {}

    /**
     * Finds IP adddresses of the host with given host name.
     * This operation does not run in backgound.
     * Returned list is sorted.
     */
    static std::list<InetAddr> getHostByName(const std::string &name,
                                             int type=AF_INET);

    /**
     * Find all host names of host with given IP.
     * This operation does not run in backgound.
     */
    static HostEnt getHostByAddr(const InetAddr &addr, int type=AF_INET);

private:

    static Mutex gethostbyaddr_mutex;
    static Mutex gethostbyname_mutex;

};

}

#endif




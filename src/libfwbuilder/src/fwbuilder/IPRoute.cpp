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



#include "fwbuilder/InetAddr.h"
#include "fwbuilder/IPRoute.h"

#include <stdio.h>
#include <iostream>

#ifndef _WIN32
#  include <sys/types.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

using namespace std;


namespace libfwbuilder
{

IPRoute::IPRoute(const IPRoute &o)
{
    nm     = o.nm;
    dst    = o.dst;
    gw     = o.gw;
    intf   = o.intf;
    direct = o.direct;
}

IPRoute::~IPRoute() {}

IPRoute::IPRoute(const InetAddr &_dst, const InetAddr &_nm,
                 const InetAddr &_gw,
                 const InterfaceData& _intf,  bool _direct)
{
    nm     = _nm;
    dst    = _dst;
    gw     = _gw;
    intf   = _intf;
    direct = _direct;
}

}

    

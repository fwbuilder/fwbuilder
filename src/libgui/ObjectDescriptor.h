/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef __OBJECTDESCRIPTOR_H_
#define __OBJECTDESCRIPTOR_H_

#include <QList>

#include "fwbuilder/InterfaceData.h"
#include "fwbuilder/dns.h"

#include <string>
#include <map>

// avoid #include "snmp.h" since it conflicts with Qt, see #2185
namespace libfwbuilder
{
    class CrawlerFind;
};

class ObjectDescriptor 
{
    public:

    bool    have_snmpd ;
    std::string  descr, contact, location, sysname    ;
    std::string  type;
    bool isSelected;
   

    std::map<int, libfwbuilder::InterfaceData> interfaces ;

    std::string              MAC_addr ;
    libfwbuilder::HostEnt    dns_info ;
    libfwbuilder::InetAddr   addr     ;
    libfwbuilder::InetAddr   netmask  ;
    
    ObjectDescriptor();
    ObjectDescriptor(const ObjectDescriptor& od);

    std::string toString()
    {
        std::ostringstream ost;
        ost << sysname;
        //if(interfaces.size()>1)
        //   ost <<" [" <<interfaces.size() <<"]";
        ost <<" (" << addr.toString() <<")";
        return ost.str();
    }
    
#ifdef HAVE_LIBSNMP
    ObjectDescriptor(const libfwbuilder::CrawlerFind *cf);
#endif

    virtual ~ObjectDescriptor();

    ObjectDescriptor& operator=(const ObjectDescriptor& od);

};

typedef QList<ObjectDescriptor> ObjectDescriptorList;


#endif

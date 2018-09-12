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


#include "global.h"

#include "ObjectDescriptor.h"

// #include snmp.h only after all Qt headers; see #2185
#include "fwbuilder/snmp.h"


using namespace std;
using namespace libfwbuilder;


ObjectDescriptor::ObjectDescriptor() {}

ObjectDescriptor::ObjectDescriptor(const ObjectDescriptor& od)
{
    have_snmpd       = od.have_snmpd;
    descr            = od.descr;
    contact          = od.contact;
    location         = od.location;
    sysname          = od.sysname;
    interfaces       = od.interfaces;
    MAC_addr         = od.MAC_addr;
    dns_info.name    = od.dns_info.name;
    dns_info.aliases = od.dns_info.aliases;
    addr             = od.addr;
    type             = od.type;
    isSelected       = od.isSelected;
    netmask          = od.netmask;

}

#ifdef HAVE_LIBSNMP
ObjectDescriptor::ObjectDescriptor(const libfwbuilder::CrawlerFind *cf)
{
    have_snmpd       = cf->have_snmpd;
    descr            = cf->descr;
    contact          = cf->contact;
    location         = cf->location;
    sysname          = cf->sysname;
    interfaces       = cf->interfaces;
    MAC_addr         = cf->found_phys_addr;
    dns_info.name    = cf->name;
    dns_info.aliases = cf->aliases;
}
#endif

ObjectDescriptor::~ObjectDescriptor()   {};

ObjectDescriptor& ObjectDescriptor::operator=(const ObjectDescriptor& od) {
    have_snmpd       = od.have_snmpd;
    descr            = od.descr;
    contact          = od.contact;
    location         = od.location;
    sysname          = od.sysname;
    interfaces       = od.interfaces;
    MAC_addr         = od.MAC_addr;
    dns_info.name    = od.dns_info.name;
    dns_info.aliases = od.dns_info.aliases;
    addr             = od.addr;
    type             = od.type;
    isSelected       = od.isSelected;
    netmask          = od.netmask;

    return *this;
}


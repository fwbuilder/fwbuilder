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

#include "config.h"

#include "ASA8Object.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Resources.h"

#include <assert.h>
#include <sstream>

#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


ASA8Object::ASA8Object(const FWObject *_obj)
{
    obj = _obj;
    name = sanitizeObjectName(obj->getName().c_str());
}

QString ASA8Object::sanitizeObjectName(const QString &name)
{
    QString qs = name;
    return qs.replace(" ", "_").replace("/", "_").left(64);
}

QString ASA8Object::createNetworkObjectCommand(const Address *addr_obj)
{
    if (addr_obj == NULL) return "";

    QStringList res;

    string addr = addr_obj->getAddressPtr()->toString();
    string netm = addr_obj->getNetmaskPtr()->toString();

    res << QString("object network %1") .arg(name);
    res << QString("  subnet %1 %2").arg(addr.c_str()).arg(netm.c_str());
    res << "";
    return res.join("\n");
}

QString ASA8Object::printPorts(int rs, int re)
{
    QStringList res;

    if (rs<0) rs = 0;
    if (re<0) re = 0;

    if (rs>0 || re>0)
    {
        if (rs==re)  res << "eq" << QString::number(rs);
        else
            if (rs==0 && re!=0) res << "lt" << QString::number(re);
            else
                if (rs!=0 && re==65535) res << "gt" << QString::number(rs);
                else
                    res << "range " << QString::number(rs)
                        << "" << QString::number(re);
    }
    return res.join(" ");
}

QString ASA8Object::createServiceObjectCommand(const Service *serv_obj)
{
    if (serv_obj == NULL) return "";

    QStringList res;

    QString proto_name = serv_obj->getProtocolName().c_str();

    res << QString("object service %1").arg(name);

    QStringList service_line;

    service_line << "service" << proto_name;

    if (TCPService::isA(serv_obj) || UDPService::isA(serv_obj))
    {
	int rs = TCPUDPService::constcast(serv_obj)->getSrcRangeStart();
	int re = TCPUDPService::constcast(serv_obj)->getSrcRangeEnd();
        if (rs != 0 || re != 0)
        {
            service_line << "source" << printPorts(rs, re);
        }

	rs = TCPUDPService::constcast(serv_obj)->getDstRangeStart();
	re = TCPUDPService::constcast(serv_obj)->getDstRangeEnd();
        if (rs != 0 || re != 0)
        {
            service_line << "destination" << printPorts(rs, re);
        }
    }

    if (ICMPService::isA(serv_obj) && serv_obj->getInt("type")!=-1)
    {
        service_line << QString::number(serv_obj->getInt("type"));
    }

    res << service_line.join(" ");
    res << "";
    return res.join("\n");
}


QString ASA8Object::getCommand()
{
    if (Address::constcast(obj)!=NULL)
        return createNetworkObjectCommand(Address::constcast(obj));

    if (Service::constcast(obj)!=NULL)
        return createServiceObjectCommand(Service::constcast(obj));

    return "";
}



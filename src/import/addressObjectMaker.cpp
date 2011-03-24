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

#include "addressObjectMaker.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include "QStringListOperators.h"

#include <string>

#include <QtDebug>


extern int fwbdebug;

using namespace libfwbuilder;
using namespace std;


AddressObjectMaker::~AddressObjectMaker() {}

FWObject* AddressObjectMaker::createObject(ObjectSignature &sig)
{
//    FWObject *obj = findMatchingObject(sig);
//    if (obj) return obj;

    FWObject *obj = NULL;

    if (sig.type_name == AddressRange::TYPENAME)
        obj = createAddressRange(sig.address_range_start, sig.address_range_end);
    else
        obj = createAddress(sig.address, sig.netmask);

    if ( ! sig.object_name.isEmpty())
    {
        obj->setName(sig.object_name.toUtf8().constData());
        registerNamedObject(sig, obj);
    } else
        registerAnonymousObject(sig, obj);

    return obj;
}
        

FWObject* AddressObjectMaker::createAddress(const QString &addr,
                                            const QString &netmask)
{
    QString correct_nm = netmask;
    if (inverted_netmasks)
    {
        InetAddr orig_nm(netmask.toStdString());
        correct_nm = (~orig_nm).toString().c_str();
    }

    try
    {
        InetAddr(correct_nm.toStdString());
    } catch (FWException &ex)
    {
        if (correct_nm.contains('.'))
        {
            // netmask has '.' in it but conversion failed.
            throw ObjectMakerException(
                QString("Error converting netmask '%1'").arg(correct_nm));
        } else
        {
            // no dot in netmask, perhaps it is specified by its length?
            // if netmask is specified by length, need to use special
            // constructor for class Netmask to convert
            bool ok = false;
            int nm_len = correct_nm.toInt(&ok);
            if (ok)
            {
                correct_nm = InetAddr(nm_len).toString().c_str();
            } else
            {
                // could not convert netmask as simple integer
                throw ObjectMakerException(
                    QString("Error converting netmask '%1'").arg(correct_nm));
            }
        }
    }

    ObjectSignature sig;
    sig.address = addr;
    sig.netmask = correct_nm;

    if ( correct_nm == InetAddr::getAllOnes().toString().c_str() )
    {
        QString name;
        try
        {
            sig.type_name = IPv4::TYPENAME;

            FWObject *obj = findMatchingObject(sig);
            if (obj) return obj;

            InetAddr obj_addr(addr.toStdString()); // testing if string converts to an address
            name = QString("h-") + addr;
            Address *a = Address::cast(
                ObjectMaker::createObject(IPv4::TYPENAME, name.toStdString()));
            a->setAddress(obj_addr);
            a->setNetmask(InetAddr(InetAddr::getAllOnes()));
            return a;

        } catch(FWException &ex)
        {
            // address text line can not be converted to ipv4 address.
            // Since parsers do not understand ipv6 yet, assume this
            // is a host address and create DNSName object

            sig.type_name = DNSName::TYPENAME;
            FWObject *obj = findMatchingObject(sig);
            if (obj) return obj;

            name = addr;
            DNSName *da = DNSName::cast(
                ObjectMaker::createObject(DNSName::TYPENAME, name.toStdString()));
            da->setSourceName(addr.toStdString());
            da->setRunTime(true);
            return da;
        }

    } else
    {
        sig.type_name = Network::TYPENAME;

        qDebug() << "Search for " << sig.toString();

        FWObject *obj = findMatchingObject(sig);
        if (obj) return obj;

        QString name = QString("net-") + addr + "/" + correct_nm;
        Network *net = Network::cast(
            ObjectMaker::createObject(Network::TYPENAME, name.toStdString()));
        try
        {
            net->setAddress( InetAddr(addr.toStdString()) );
        } catch (FWException &ex)
        {
            throw ObjectMakerException(
                QString("Error converting address '%1'").arg(addr));
        }

        // we have already verified netmask above
        net->setNetmask( InetAddr(correct_nm.toStdString()) );

        return net;
    }
    return NULL;
}

FWObject* AddressObjectMaker::createAddressRange(const QString &addr1,
                                                 const QString &addr2)
{

    QString name = QString("range-") + addr1 + "-" + addr2;
    AddressRange *ar = AddressRange::cast(
        ObjectMaker::createObject(AddressRange::TYPENAME, name.toStdString()));

    try
    {
        ar->setRangeStart( InetAddr(addr1.toStdString()) );
    } catch (FWException &ex)
    {
        throw ObjectMakerException(
            QString("Error converting address '%1'").arg(addr1));
    }

    try
    {
        ar->setRangeEnd( InetAddr(addr2.toStdString()) );
    } catch (FWException &ex)
    {
        throw ObjectMakerException(
            QString("Error converting address '%1'").arg(addr2));
    }

    return ar;
}



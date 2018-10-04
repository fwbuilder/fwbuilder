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
#include "fwbuilder/AddressTable.h"
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
    FWObject *obj = nullptr;

    if (sig.type_name == AddressRange::TYPENAME)
        obj = createAddressRange(sig);

    if (sig.type_name == AddressTable::TYPENAME)
        obj = createAddressTable(sig);

    if (sig.type_name == DNSName::TYPENAME)
        obj = createDNSName(sig);

    if (obj == nullptr)
        obj = createAddress(sig);

    // Now I should build new signature because actual object type has
    // only been determined in createAddress()

    ObjectSignature new_sig(error_tracker);

    if ( ! sig.object_name.isEmpty())
    {
        obj->setName(sig.object_name.toUtf8().constData());
        obj->dispatch(&new_sig, (void*)(nullptr));
        registerNamedObject(new_sig, obj);
    } else
    {
        obj->dispatch(&new_sig, (void*)(nullptr));
        registerAnonymousObject(new_sig, obj);
    }

    return obj;
}

FWObject* AddressObjectMaker::createAddress(ObjectSignature &sig)
{
    ObjectSignature signature = sig;

    InetAddr netmask(signature.netmask.toStdString());

    if ( netmask == InetAddr::getAllOnes() )
    {
        QString name;
        try
        {
            signature.type_name = IPv4::TYPENAME;

            FWObject *obj = findMatchingObject(signature);
            if (obj) return obj;

            InetAddr obj_addr(sig.address.toStdString()); // testing if string converts to an address
            name = QString("h-") + sig.address;
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

            signature.type_name = DNSName::TYPENAME;
            FWObject *obj = findMatchingObject(signature);
            if (obj) return obj;

            name = sig.address;
            DNSName *da = DNSName::cast(
                ObjectMaker::createObject(DNSName::TYPENAME, name.toStdString()));
            da->setSourceName(sig.address.toStdString());
            da->setRunTime(true);
            return da;
        }

    } else
    {
        signature.type_name = Network::TYPENAME;

        FWObject *obj = findMatchingObject(signature);
        if (obj) return obj;

        QString name = QString("net-%1/%2")
            .arg(signature.address).arg(signature.netmask);
        Network *net = Network::cast(
            ObjectMaker::createObject(Network::TYPENAME, name.toStdString()));
        try
        {
            net->setAddress( InetAddr(sig.address.toStdString()) );
        } catch (FWException &ex)
        {
            error_tracker->registerError(
                QString("Error converting address '%1'").arg(sig.address));
        }

        // we have already verified netmask above
        net->setNetmask(netmask);

        return net;
    }
    return nullptr;
}

FWObject* AddressObjectMaker::createAddressRange(ObjectSignature &sig)
{
    FWObject *obj = findMatchingObject(sig);
    if (obj) return obj;

    QString addr1 = sig.address_range_start;
    QString addr2 = sig.address_range_end;
    QString name = QString("range-%1-%2").arg(addr1).arg(addr2);

    AddressRange *ar = AddressRange::cast(
        ObjectMaker::createObject(AddressRange::TYPENAME, name.toStdString()));

    try
    {
        ar->setRangeStart( InetAddr(addr1.toStdString()) );
    } catch (FWException &ex)
    {
        error_tracker->registerError(
            QString("Error converting address '%1'").arg(addr1));
    }

    try
    {
        ar->setRangeEnd( InetAddr(addr2.toStdString()) );
    } catch (FWException &ex)
    {
        error_tracker->registerError(
            QString("Error converting address '%1'").arg(addr2));
    }

    return ar;
}


FWObject* AddressObjectMaker::createAddressTable(ObjectSignature &sig)
{
    FWObject *obj = findMatchingObject(sig);
    if (obj) return obj;

    AddressTable *at =  AddressTable::cast(
        ObjectMaker::createObject(AddressTable::TYPENAME,
                                  sig.object_name.toUtf8().constData()));
    assert(at!=nullptr);
    at->setRunTime(true);
    at->setSourceName(sig.address_table_name.toStdString());
    return at;
}

FWObject* AddressObjectMaker::createDNSName(ObjectSignature &sig)
{
    FWObject *obj = findMatchingObject(sig);
    if (obj) return obj;

    DNSName *dns_obj =  DNSName::cast(
        ObjectMaker::createObject(DNSName::TYPENAME,
                                  sig.object_name.toUtf8().constData()));
    assert(dns_obj!=nullptr);
    dns_obj->setRunTime(true);
    dns_obj->setSourceName(sig.dns_name.toStdString());
    return dns_obj;
}

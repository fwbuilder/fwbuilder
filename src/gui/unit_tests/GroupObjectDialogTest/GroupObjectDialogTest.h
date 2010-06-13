/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: GroupObjectDialogTest.h 2723 2010-03-16 17:32:18Z a2k $

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

#ifndef IPDIALOGTEST_H
#define IPDIALOGTEST_H

#include <QObject>
#include "fwbuilder/Library.h"
#include "ObjectManipulator.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/CustomService.h"

class GroupObjectDialogTest : public QObject
{
    Q_OBJECT;
    
    libfwbuilder::Library* findUserLibrary();
    template <class FWTYPE> FWTYPE* createObject(QString name);
    ObjectManipulator *om;

    libfwbuilder::IPv4 *address;
    libfwbuilder::IPv6 *address6;
    libfwbuilder::Network *network;
    libfwbuilder::NetworkIPv6 *network6;
    libfwbuilder::AddressRange *range;
    libfwbuilder::AddressTable *table;
    libfwbuilder::DNSName *dns;
    libfwbuilder::IPService *ipservice;
    libfwbuilder::ICMPService *icmpservice;
    libfwbuilder::UDPService *udpservice;
    libfwbuilder::TCPService *tcpservice;
    libfwbuilder::TagService *tagservice;
    libfwbuilder::UserService *userservice;
private slots:
    void initTestCase();
    void testObjectGroup();
    void testServiceGroup();
public slots:
    void checkMessageBox();
    void checkNoMessageBox();
    void checkObjectGroupMenu();
    void checkObjectServiceGroupMenu();
};

#endif // IPDIALOGTEST_H

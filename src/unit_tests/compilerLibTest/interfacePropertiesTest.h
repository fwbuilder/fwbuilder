/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

  $Id: interfaceProperties.h 2043 2009-12-06 01:10:10Z a2k $

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


#ifndef INTERFACEPROPERTIESTEST
#define INTERFACEPROPERTIESTEST

#include <QObject>

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"
#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include <list>
#include <string>



class interfacePropertiesTest: public QObject
{
    Q_OBJECT

    libfwbuilder::FWObjectDatabase *db;
    
    void isValidVlanInterfaceNameLinuxFamilies(interfaceProperties *int_prop);
  
    interfaceProperties* getIntProps(const QString &os);

private slots:
    void init();

    void validateInterfaceNameCommon();
    void validateInterfaceNameLinux();
    void validateInterfaceNameProCurve();
    void validateInterface();
    void isEligibleForCluster();
    void isValidVlanInterfaceNameLinux24();
    void isValidVlanInterfaceNameOpenWRT();
    void isValidVlanInterfaceNameDDWRT1();
    void isValidVlanInterfaceNameDDWRT2();
    void isValidVlanInterfaceNameSecuwall();
    void isValidVlanInterfaceNameBSD();
    void isValidVlanInterfaceNameIOS();
    void isValidVlanInterfaceNamePIX();
    void isValidVlanInterfaceNameProCurve();
    void validateInterfaceProCurve();
    void testManageIpAddresses();
    void testManageIpAddressesCluster();

};

#endif


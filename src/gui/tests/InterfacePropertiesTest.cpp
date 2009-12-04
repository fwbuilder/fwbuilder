/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

  $Id: InterfaceEditorWidget.cpp 2006 2009-12-02 19:10:48Z a2k $

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

#include "InterfacePropertiesTest.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"
#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"
#include "../global.h"

using namespace std;
using namespace libfwbuilder;

InterfacePropertiesTest::InterfacePropertiesTest(QObject *parent) :
    QObject(parent)
{
}

void InterfacePropertiesTest::validateInterface()
{
    QFETCH(QString, host_OS);
    QFETCH(QString, type);
    QFETCH(QString, parent_type);
    QFETCH(bool, unnumbered);
    QFETCH(bool, result);

    libfwbuilder::Cluster cluster;
    libfwbuilder::Interface parent;
    libfwbuilder::Interface iface;
    //parent.add(&iface, false);
    //parent.setStr("type", parent_type.toStdString());
    iface.setUnnumbered(unnumbered);
    iface.setStr("type", type.toStdString());
    cluster.setStr("host_OS", host_OS.toStdString());

    Resources* os_res = Resources::os_res[host_OS.toStdString()];
    string os_family = host_OS.toStdString();
    if (os_res!=NULL)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties * int_prop = interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);
    QString err;

    QVERIFY(int_prop != NULL);

    bool answer = int_prop->validateInterface(dynamic_cast<FWObject*>(&cluster),
                                           dynamic_cast<FWObject*>(&iface), false, err);

    QVERIFY(result == answer);

}

void InterfacePropertiesTest::validateInterface_data()
{

    QTest::addColumn<QString>("host_OS");
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("parent_type");
    QTest::addColumn<bool>("unnumbered");
    QTest::addColumn<bool>("result");

    QTest::newRow("test1") << "linux24" << "ethernet" << "ethernet" << false << true;
    QTest::newRow("test3") << "linux24" << "bonding" << "ethernet" << false << false;
    QTest::newRow("test4") << "linux24" << "bonding" << "ethernet" << true << false;
}

QTEST_MAIN(InterfacePropertiesTest)

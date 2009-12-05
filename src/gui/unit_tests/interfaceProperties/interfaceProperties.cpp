/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

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

#include "interfaceProperties.h"

using namespace std;
using namespace libfwbuilder;
using namespace CppUnit;

void interfacePropertiesTest::validateInterface()
{
    vector<vector<string> > testData;
    vector<string> row;
    row.push_back("linux24");
    row.push_back("ethernet");
    row.push_back("ethernet");
    row.push_back("false");
    row.push_back("true");
    testData.push_back(row);

    row.clear();
    row.push_back("linux24");
    row.push_back("ethernet");
    row.push_back("ethernet");
    row.push_back("true");
    row.push_back("false");
    testData.push_back(row);

    row.clear();
    row.push_back("linux24");
    row.push_back("bonding");
    row.push_back("ethernet");
    row.push_back("false");
    row.push_back("false");
    testData.push_back(row);

    row.clear();
    row.push_back("linux24");
    row.push_back("bridge");
    row.push_back("ethernet");
    row.push_back("false");
    row.push_back("false");
    testData.push_back(row);

    for (unsigned int i = 0; i<testData.size(); i++)
    {
        string host_OS = testData[i][0];
        string parent_type = testData[i][1];
        string type = testData[i][2];
        bool unnumbered = testData[i][3] == "true";
        bool answer = testData[i][4] == "true";

        Cluster cluster;
        Interface parent;
        Interface iface;

        db->add(&cluster);
        db->add(&parent);
        parent.add(&iface, false);
        parent.getOptionsObject()->setStr("type", parent_type);

        iface.setUnnumbered(unnumbered);
        iface.getOptionsObject()->setStr("type", type);
        cluster.setStr("host_OS", host_OS);

        Resources* os_res = Resources::os_res[host_OS];
        string os_family = host_OS;
        if (os_res!=NULL)
            os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

        interfaceProperties * int_prop = interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);

        QString err;

        CPPUNIT_ASSERT(int_prop != NULL);

        bool result = int_prop->validateInterface(dynamic_cast<FWObject*>(&cluster),
                                               dynamic_cast<FWObject*>(&iface), false, err);
        CPPUNIT_ASSERT(result == answer);
        CPPUNIT_ASSERT(err.isEmpty() == answer);
    }
}

void interfacePropertiesTest::setUp()
{
    db = new FWObjectDatabase();
}


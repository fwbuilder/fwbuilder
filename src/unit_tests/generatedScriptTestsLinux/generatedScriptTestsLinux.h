/* 

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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

#ifndef GENERATEDSCRIPTTESTS_LINUX_H
#define GENERATEDSCRIPTTESTS_LINUX_H

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Logger.h"

#include <QObject>
#include <QStringList>


class GeneratedScriptTest : public QObject
{
    Q_OBJECT

    libfwbuilder::FWObjectDatabase *objdb;

    void loadDataFile(const std::string &file_name);
    void runCompiler(const std::string &test_file,
                     const std::string &firewall_object_name,
                     const std::string &generate_file_name);

private slots:
    void init();
    void cleanup();

    void ManifestTest();
    void FwCommentTest();
    void CheckUtilitiesTest();
    void verifyInterfacesTest();
    void configureInterfacesTest();
    void configureInterfacesClusterTest();
    void virtualAddressesForNat1Test();
    void virtualAddressesForNat2Test();
    void runTimeAddressTablesWithIpSet1Test();
    void runTimeAddressTablesWithIpSet2Test();
    void minusDTest();
    void minusOTest1();
    void minusOTest2();
    void minusDminusOTest();
    void outputFileNameOptionTest1();
    void outputFileNameOptionTest2();
    void outputFileNameOptionTest3();
    void vlanNamingTest();
};

#endif // GENERATEDSCRIPTTESTS_LINUX_H

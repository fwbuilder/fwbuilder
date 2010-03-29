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

#include <cppunit/extensions/HelperMacros.h>

#include <QStringList>


class GeneratedScriptTest : public CppUnit::TestFixture
{
    libfwbuilder::FWObjectDatabase *objdb;

    void loadDataFile(const std::string &file_name);
    void runCompiler(const std::string &test_file,
                     const std::string &firewall_object_name,
                     const std::string &generate_file_name);

public:
    void setUp();
    void tearDown();
    void ManifestTest();
    void FwCommentTest();
    void CheckUtilitiesTest();
    void verifyInterfacesTest();
    void configureInterfacesTest();
    void configureInterfacesClusterTest();
    
    CPPUNIT_TEST_SUITE(GeneratedScriptTest);
    CPPUNIT_TEST(ManifestTest);
    CPPUNIT_TEST(FwCommentTest);
    CPPUNIT_TEST(CheckUtilitiesTest);
    CPPUNIT_TEST(verifyInterfacesTest);
    CPPUNIT_TEST(configureInterfacesTest);
    CPPUNIT_TEST(configureInterfacesClusterTest);
    CPPUNIT_TEST_SUITE_END();

};

#endif // GENERATEDSCRIPTTESTS_LINUX_H

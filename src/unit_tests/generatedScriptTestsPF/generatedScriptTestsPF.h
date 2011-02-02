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

#ifndef GENERATEDSCRIPTTESTS_IPFILTER_H
#define GENERATEDSCRIPTTESTS_IPFILTER_H

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
                     const std::string &generate_file_name,
                     const std::string &output_file_option="");

public:
    void setUp();
    void tearDown();
    void ManifestTest_1();
    void ManifestTest_2();
    void ManifestTest_3();
    void ManifestTest_4();
    void ManifestTest_5();
    void ManifestTest_6();
    void ManifestTest_7();
    void ManifestTest_8();
    void ManifestTest_9();
    void ManifestTest_10();

    void FwCommentTest();
    void ActivationCommandsTest_1();
    void ActivationCommandsTest_2();
    void ActivationCommandsTest_3();
    void ActivationCommandsTest_4();
//    void ActivationCommandsTest_5();
    void ActivationCommandsTest_6();
    void ActivationCommandsTest_7();
    void ActivationCommandsTest_8();
    void ActivationCommandsTest_9();
    void ActivationCommandsTest_10();
    
    CPPUNIT_TEST_SUITE(GeneratedScriptTest);

    // The order of tests matters because activation commands tests use
    // files produced in manifest tests
    CPPUNIT_TEST(ManifestTest_1);
    CPPUNIT_TEST(ActivationCommandsTest_1);

    CPPUNIT_TEST(ManifestTest_2);
    CPPUNIT_TEST(ActivationCommandsTest_2);

    CPPUNIT_TEST(ManifestTest_3);
    CPPUNIT_TEST(ActivationCommandsTest_3);

    CPPUNIT_TEST(ManifestTest_4);
    CPPUNIT_TEST(ActivationCommandsTest_4);

//    CPPUNIT_TEST(ManifestTest_5);
//    CPPUNIT_TEST(ActivationCommandsTest_5);

    CPPUNIT_TEST(ManifestTest_6);
    CPPUNIT_TEST(ActivationCommandsTest_6);

    CPPUNIT_TEST(ManifestTest_7);
    CPPUNIT_TEST(ActivationCommandsTest_7);

    CPPUNIT_TEST(ManifestTest_8);
    CPPUNIT_TEST(ActivationCommandsTest_8);

    CPPUNIT_TEST(ManifestTest_9);
    CPPUNIT_TEST(ActivationCommandsTest_9);

    CPPUNIT_TEST(ManifestTest_10);
    CPPUNIT_TEST(ActivationCommandsTest_10);

    CPPUNIT_TEST(FwCommentTest);

    CPPUNIT_TEST_SUITE_END();

};

#endif // GENERATEDSCRIPTTESTS_IPFILTER_H

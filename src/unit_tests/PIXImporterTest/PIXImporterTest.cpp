/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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

#include "PIXImporterTest.h"

#include "global.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <assert.h>

#include "Importer.h"
#include "PIXImporter.h"
#include "FWBTree.h"

#include "fwbuilder/Policy.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Constants.h"

#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QString>
#include <QRegExp>


using namespace std;
using namespace libfwbuilder;

extern string platform;

extern QString findBestVersionMatch(const QString &platform,
                                    const QString &discovered_version);


class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string &) const
    {
        return false;
    }
};

void PIXImporterTest::setUp()
{
    FWBTree *tree = new FWBTree();

    /* create database */
    db = new FWObjectDatabase();

    /* load the data file */
    UpgradePredicate upgrade_predicate;

    db->setReadOnly( false );

    db->load( Constants::getStandardObjectsFilePath(),
              &upgrade_predicate, Constants::getDTDDirectory());

    db->setFileName("");
    lib = Library::cast(tree->createNewLibrary(db));
    lib->setName("User");

    logger = new QueueLogger();

    // this makes the test compile and link. There is a problem with
    // dependencies, the test depends on libimport.a and additionally,
    // PIXImporter.cpp depends on this function that is implemented in
    // platforms.cpp in libgui.a; however since libgui.a comes before
    // libimport.a in linker command line, this function does not get
    // pulled since it is not used anywhere except by this test module
    // and so linking fails. Making this call creates dependency and
    // pulls this function at linking time before libimport.a and its
    // dependencies are considered
    QString version = findBestVersionMatch("pix", "7.0");

}

void PIXImporterTest::compareResults(QueueLogger* logger,
                                  QString expected_result_file_name,
                                  QString obtained_result_file_name)
{
    QString result;
    QStringList obtained_result;

    while (logger->ready())
        result.append(logger->getLine().c_str());
    obtained_result = result.split("\n");

    QFile rw(obtained_result_file_name);
    rw.open(QFile::WriteOnly);
    rw.write(result.toLatin1());
    rw.close();

    QFile rr(expected_result_file_name);
    rr.open(QFile::ReadOnly);
    QString result_file = rr.readAll();
    QStringList expected_result = result_file.split("\n");

    CPPUNIT_ASSERT_MESSAGE(
        QString(
            "Sizes of the generated importer output and test files are different.\n"
            "Expected: %1 (%2)\n"
            "Obtained: %3 (%4)\n"
            "diff -u  %1 %3 | less -S")
        .arg(expected_result_file_name).arg(expected_result.size())
        .arg(obtained_result_file_name).arg(obtained_result.size()).toStdString(),
        expected_result.size() == obtained_result.size());

    int max_idx = max(expected_result.size(), obtained_result.size());
    for (int i=0; i < max_idx; ++i)
    {
        QString err = QString("Line %1:\nExpected: '%2'\nResult: '%3'\n")
            .arg(i).arg(expected_result[i]).arg(obtained_result[i]);
        CPPUNIT_ASSERT_MESSAGE(err.toStdString(), obtained_result[i] == expected_result[i]);
    }
}

void PIXImporterTest::compareFwbFiles(QString expected_result_file_name,
                                   QString obtained_result_file_name)
{
    QString result;
    QStringList obtained_result;

    QFile rr(obtained_result_file_name);
    rr.open(QFile::ReadOnly);
    QString result_file = rr.readAll();
    rr.close();
    obtained_result = result_file.split("\n");
   
    QFile er(expected_result_file_name);
    er.open(QFile::ReadOnly);
    result_file = er.readAll();
    er.close();
    QStringList expected_result = result_file.split("\n");

    // find all lastModified attributes and replace them with identical values
    // because they are always going to be different

    QString err("Sizes of the generated .fwb and test files are different: \n"
                "Expected: %1 (%2)\n"
                "Obtained: %3 (%4)\n"
                "diff -u  %1 %3 | less -S");

    CPPUNIT_ASSERT_MESSAGE(
        err
        .arg(expected_result_file_name).arg(expected_result.size())
        .arg(obtained_result_file_name).arg(obtained_result.size())
        .toStdString(),
        expected_result.size() == obtained_result.size());

    QRegExp last_mod_re("lastModified=\"\\d+\"");
    int max_idx = max(expected_result.size(), obtained_result.size());
    for (int i=0; i < max_idx; ++i)
    {
        QString os = obtained_result[i];
        obtained_result[i] = os.replace(last_mod_re, "lastModified=\"0000000000\"");

        QString es = expected_result[i];
        expected_result[i] = es.replace(last_mod_re, "lastModified=\"0000000000\"");
    }

    for (int i=0; i < max_idx; ++i)
    {
        QString err = QString("Line %1:\nExpected: '%2'\nResult: '%3'\n")
            .arg(i).arg(expected_result[i]).arg(obtained_result[i]);
        CPPUNIT_ASSERT_MESSAGE(err.toStdString(), obtained_result[i] == expected_result[i]);
    }
}

std::string PIXImporterTest::openTestFile(const QString &file_name)
{
    QFile f(file_name);
    f.open(QFile::ReadOnly);
    string buffer = QString(f.readAll()).toStdString();
    f.close();

    return buffer;
}

void PIXImporterTest::PIX_6_Test()
{
    platform = "pix";

    std::istringstream instream(openTestFile("test_data/pix6.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pix6.fwb");

    compareResults(logger, "test_data/pix6.output", "pix6.output");
    compareFwbFiles("test_data/pix6.fwb", "pix6.fwb");
}

void PIXImporterTest::PIX_7_Test()
{
    platform = "pix";

    std::istringstream instream(openTestFile("test_data/pix7.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pix7.fwb");

    compareResults(logger, "test_data/pix7.output", "pix7.output");
    compareFwbFiles("test_data/pix7.fwb", "pix7.fwb");
}

void PIXImporterTest::PIX_7_NAT_Test()
{
    platform = "pix";

    std::istringstream instream(openTestFile("test_data/pix7-nat.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pix7-nat.fwb");

    compareResults(logger, "test_data/pix7-nat.output", "pix7-nat.output");
    compareFwbFiles("test_data/pix7-nat.fwb", "pix7-nat.fwb");
}

void PIXImporterTest::ASA_8_0_Test()
{
    platform = "pix";

    std::istringstream instream(openTestFile("test_data/asa8.0.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("asa8.0.fwb");

    compareResults(logger, "test_data/asa8.0.output", "asa8.0.output");
    compareFwbFiles("test_data/asa8.0.fwb", "asa8.0.fwb");
}

void PIXImporterTest::ASA_8_3_Test()
{
    platform = "pix";

    std::istringstream instream(openTestFile("test_data/asa8.3.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("asa8.3.fwb");

    compareResults(logger, "test_data/asa8.3.output", "asa8.3.output");
    compareFwbFiles("test_data/asa8.3.fwb", "asa8.3.fwb");
}

void PIXImporterTest::ObjectsAndGroupsTest()
{
    platform = "pix";

    std::istringstream instream(openTestFile("test_data/asa8.3-objects-and-groups.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("asa8.3-objects-and-groups.fwb");

    compareResults(logger,
                   "test_data/asa8.3-objects-and-groups.output",
                   "asa8.3-objects-and-groups.output");
    compareFwbFiles("test_data/asa8.3-objects-and-groups.fwb",
                    "asa8.3-objects-and-groups.fwb");
}

void PIXImporterTest::ACLObjectsAndGroupsTest()
{
    platform = "pix";

    std::istringstream instream(openTestFile("test_data/asa8.3-acl-object-groups.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("asa8.3-acl-object-groups.fwb");

    compareResults(logger,
                   "test_data/asa8.3-acl-object-groups.output",
                   "asa8.3-acl-object-groups.output");
    compareFwbFiles("test_data/asa8.3-acl-object-groups.fwb",
                    "asa8.3-acl-object-groups.fwb");
}

void PIXImporterTest::ACLTest()
{
    platform = "pix";

    std::istringstream instream(openTestFile("test_data/asa8.3-acl.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("asa8.3-acl.fwb");

    compareResults(logger, "test_data/asa8.3-acl.output", "asa8.3-acl.output");
    compareFwbFiles("test_data/asa8.3-acl.fwb", "asa8.3-acl.fwb");
}

void PIXImporterTest::NamesTest()
{
    platform = "pix";

    std::istringstream instream(openTestFile("test_data/asa8.0-names.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("asa8.0-names.fwb");

    compareResults(logger, "test_data/asa8.0-names.output", "asa8.0-names.output");
    compareFwbFiles("test_data/asa8.0-names.fwb", "asa8.0-names.fwb");
}

void PIXImporterTest::FWSM_4_1_Test()
{
    std::istringstream instream(openTestFile("test_data/fwsm1.test"));

    Importer* imp = new PIXImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("fwsm1.fwb");

    compareResults(logger, "test_data/fwsm1.output", "fwsm1.output");
    compareFwbFiles("test_data/fwsm1.fwb", "fwsm1.fwb");
}


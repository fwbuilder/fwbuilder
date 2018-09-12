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

#include "PFImporterTest.h"

#include "global.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <assert.h>

#include "Importer.h"
#include "PFImporter.h"
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

void PFImporterTest::setUp()
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
    // PFImporter.cpp depends on this function that is implemented in
    // platforms.cpp in libgui.a; however since libgui.a comes before
    // libimport.a in linker command line, this function does not get
    // pulled since it is not used anywhere except by this test module
    // and so linking fails. Making this call creates dependency and
    // pulls this function at linking time before libimport.a and its
    // dependencies are considered
    QString version = findBestVersionMatch("pf", "4.0");

}

void PFImporterTest::compareResults(QueueLogger* logger,
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

void PFImporterTest::compareFwbFiles(QString expected_result_file_name,
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

std::string PFImporterTest::openTestFile(const QString &file_name)
{
    QFile f(file_name);
    f.open(QFile::ReadOnly);
    string buffer = QString(f.readAll()).toStdString();
    f.close();

    return buffer;
}

void PFImporterTest::macrosTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-macros.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-macros.fwb");

    compareResults(logger, "test_data/pf-macros.output", "pf-macros.output");
    compareFwbFiles("test_data/pf-macros.fwb", "pf-macros.fwb");
}

void PFImporterTest::hostsMatchTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-hosts-matches.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-hosts-matches.fwb");

    compareResults(logger,
                   "test_data/pf-hosts-matches.output",
                   "pf-hosts-matches.output");
    compareFwbFiles("test_data/pf-hosts-matches.fwb",
                    "pf-hosts-matches.fwb");
}

void PFImporterTest::blockReturnTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-block-return-actions.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-block-return-actions.fwb");

    compareResults(logger,
                   "test_data/pf-block-return-actions.output",
                   "pf-block-return-actions.output");
    compareFwbFiles("test_data/pf-block-return-actions.fwb",
                    "pf-block-return-actions.fwb");
}

void PFImporterTest::icmpMatchTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-icmp-matches.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-icmp-matches.fwb");

    compareResults(logger,
                   "test_data/pf-icmp-matches.output",
                   "pf-icmp-matches.output");
    compareFwbFiles("test_data/pf-icmp-matches.fwb",
                    "pf-icmp-matches.fwb");
}

void PFImporterTest::interfaceMatchTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-interface-matches.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-interface-matches.fwb");

    compareResults(logger,
                   "test_data/pf-interface-matches.output",
                   "pf-interface-matches.output");
    compareFwbFiles("test_data/pf-interface-matches.fwb",
                    "pf-interface-matches.fwb");
}

void PFImporterTest::portMatchTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-port-matches.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-port-matches.fwb");

    compareResults(logger,
                   "test_data/pf-port-matches.output",
                   "pf-port-matches.output");
    compareFwbFiles("test_data/pf-port-matches.fwb",
                    "pf-port-matches.fwb");
}

void PFImporterTest::setCommandsTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-set-commands.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    //db->setPredictableIds();
    //db->saveFile("pf-set-commands.fwb");

    compareResults(logger,
                   "test_data/pf-set-commands.output",
                   "pf-set-commands.output");
    //compareFwbFiles("test_data/pf-set-commands.fwb",
    //                "pf-set-commands.fwb");
}

void PFImporterTest::stateMatchTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-state-matches.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-state-matches.fwb");

    compareResults(logger,
                   "test_data/pf-state-matches.output",
                   "pf-state-matches.output");
    compareFwbFiles("test_data/pf-state-matches.fwb",
                    "pf-state-matches.fwb");
}

void PFImporterTest::tcpFlagsMatchTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-tcp-flags-matches.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-tcp-flags-matches.fwb");

    compareResults(logger,
                   "test_data/pf-tcp-flags-matches.output",
                   "pf-tcp-flags-matches.output");
    compareFwbFiles("test_data/pf-tcp-flags-matches.fwb",
                    "pf-tcp-flags-matches.fwb");
}

void PFImporterTest::natCommands()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-nat-rules.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-nat-rules.fwb");

    compareResults(logger,
                   "test_data/pf-nat-rules.output",
                   "pf-nat-rules.output");
    compareFwbFiles("test_data/pf-nat-rules.fwb",
                    "pf-nat-rules.fwb");
}

void PFImporterTest::rdrCommands()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-rdr-rules.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-rdr-rules.fwb");

    compareResults(logger,
                   "test_data/pf-rdr-rules.output",
                   "pf-rdr-rules.output");
    compareFwbFiles("test_data/pf-rdr-rules.fwb",
                    "pf-rdr-rules.fwb");
}

void PFImporterTest::setTimeoutCommands()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-timeouts.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

//    db->setPredictableIds();
//    db->saveFile("pf-timeouts.fwb");

    compareResults(logger, "test_data/pf-timeouts.output", "pf-timeouts.output");
//    compareFwbFiles("test_data/pf-timeouts.fwb",
//                    "pf-timeouts.fwb");
}

void PFImporterTest::scrubCommandsOld()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-scrub-commands-old.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    compareResults(logger,
                   "test_data/pf-scrub-commands-old.output",
                   "pf-scrub-commands-old.output");
}

void PFImporterTest::scrubCommandsNew()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-scrub-commands-new.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    compareResults(logger,
                   "test_data/pf-scrub-commands-new.output",
                   "pf-scrub-commands-new.output");
}

void PFImporterTest::tableDefinitions()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-tables.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-tables.fwb");

    compareResults(logger, "test_data/pf-tables.output", "pf-tables.output");
    compareFwbFiles("test_data/pf-tables.fwb", "pf-tables.fwb");
}


void PFImporterTest::userGroupMatches()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-user-group-matches.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-user-group-matches.fwb");

    compareResults(logger,
                   "test_data/pf-user-group-matches.output",
                   "pf-user-group-matches.output");
    compareFwbFiles("test_data/pf-user-group-matches.fwb",
                    "pf-user-group-matches.fwb");
}

void PFImporterTest::routeToTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-route-to.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-route-to.fwb");

    compareResults(logger, "test_data/pf-route-to.output", "pf-route-to.output");
    compareFwbFiles("test_data/pf-route-to.fwb", "pf-route-to.fwb");
}

void PFImporterTest::routeTo47Test()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/pf-route-to-4.7.conf"));

    Importer* imp = new PFImporter(lib, instream, logger, "test_fw");
    imp->setAddStandardCommentsFlag(true);
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    db->setPredictableIds();
    db->saveFile("pf-route-to-4.7.fwb");

    compareResults(logger, "test_data/pf-route-to-4.7.output", "pf-route-to-4.7.output");
    compareFwbFiles("test_data/pf-route-to-4.7.fwb", "pf-route-to-4.7.fwb");
}





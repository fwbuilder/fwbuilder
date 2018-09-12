/*
 * generatedScriptTestsSecuwall.h - secuwall unit tests
 *
 * Copyright (c) 2010 secunet Security Networks AG
 * Copyright (c) 2010 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2010 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */


#include "generatedScriptTestsSecuwall.h"

#include "CompilerDriver_ipt.h"
#include "Configlet.h"

#include "fwbuilder/IPService.h"
#include "fwbuilder/Constants.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string&) const
    {
        cout << "Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it." << std::endl;
        return false;
    }
};


void GeneratedScriptTest::setUp()
{
    // register protocols we need
    IPService::addNamedProtocol(51, "ah");
    IPService::addNamedProtocol(112, "vrrp");

    Configlet::setDebugging(true);
}

void GeneratedScriptTest::tearDown()
{
}

void GeneratedScriptTest::loadDataFile(const string &file_name)
{
    /* load the data file */
    UpgradePredicate upgrade_predicate;

    objdb->setReadOnly( false );
    objdb->load(file_name, &upgrade_predicate, Constants::getDTDDirectory());
    objdb->setFileName(file_name);
    objdb->reIndex();
}

void GeneratedScriptTest::runCompiler(const std::string &test_file,
                                      const std::string &firewall_object_name,
                                      const std::string &generate_file_name)
{
    loadDataFile(test_file);

    QStringList args;
    args << firewall_object_name.c_str();

    CompilerDriver_ipt driver(objdb);
    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipt initialization failed",
                           driver.prepare(args) == true);
    driver.compile();
    // compiler should have created file secuwall-1.fw and test1 directory with
    // secuwall-specific configuration files.
    QFileInfo fi(generate_file_name.c_str());
    CPPUNIT_ASSERT_MESSAGE("Generated file " + generate_file_name + " not found",
                           fi.exists() == true);
}

void GeneratedScriptTest::assertDirsEqual(const std::string &left_dir,
                                          const std::string &right_dir)
{
    QStringList leftList, rightList;
    QDirIterator leftIt(QString (left_dir.c_str()), QDir::Files, QDirIterator::Subdirectories);
    QDirIterator rightIt(QString (right_dir.c_str()), QDir::Files, QDirIterator::Subdirectories);
    while (leftIt.hasNext() && rightIt.hasNext())
    {
        leftList += leftIt.next();
        rightList += rightIt.next();
    }

    if (leftIt.hasNext())
    {
        CPPUNIT_FAIL("Directory " + left_dir + " contains more files than " + right_dir);
    }
    if (rightIt.hasNext())
    {
        CPPUNIT_FAIL("Directory " + right_dir + " contains more files than " + left_dir);
    }

    leftList.sort();
    rightList.sort();

    QList<QString>::const_iterator i, j;
    for (i = leftList.constBegin(), j = rightList.constBegin();
         i != leftList.constEnd();
         ++i, ++j) {
        assertFilesEqual(QString(*i).toStdString(), QString(*j).toStdString());
    }
}

void GeneratedScriptTest::assertFilesEqual(const std::string &left_filename,
                                           const std::string &right_filename)
{
    bool result = false;
    QFile leftFile(left_filename.c_str());
    QFile rightFile(right_filename.c_str());

    if (leftFile.open(QFile::ReadOnly) && rightFile.open(QFile::ReadOnly))
    {
        result = leftFile.readAll() == rightFile.readAll();
        leftFile.close();
        rightFile.close();
    }

    CPPUNIT_ASSERT_MESSAGE("Files " + left_filename + " and " + right_filename + " differ",
                           result);
}

void GeneratedScriptTest::FilesGenerationTest()
{
    const string fwname ("secuwall-1");
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "secuwall_cluster_1", "secuwall-1.fw");
    delete objdb;

    assertDirsEqual("ref.secuwall-1", "secuwall-1");
}

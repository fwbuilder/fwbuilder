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

#include "../../config.h"

#include "generatedScriptTestsLinux.h"

#include "CompilerDriver_ipt.h"
#include "Configlet.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/IPService.h"

#include <QApplication>
#include <QStringList>
#include <QFileInfo>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


extern string librespath;

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
    objdb->load(file_name, &upgrade_predicate, librespath);
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
    // compiler should have created file test1.fw
    QFileInfo fi(generate_file_name.c_str());
    CPPUNIT_ASSERT_MESSAGE("Generate file " + generate_file_name + " not found",
                           fi.exists() == true);
}

// I can check only certain parts of the top comment. Can't
// compare against "golden" file because some parts of the comment
// are variable, such as date, version and build number

void GeneratedScriptTest::ManifestTest()
{
    objdb = new FWObjectDatabase();

    runCompiler("test1.fwb", "test1", "test1.fw");

    QString res = Configlet::findConfigletInFile("top_comment", "test1.fw");

    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * test1.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test2.fw") == -1);

    delete objdb;
}

void GeneratedScriptTest::FwCommentTest()
{
    objdb = new FWObjectDatabase();

    runCompiler("test1.fwb", "test1", "test1.fw");

    QString res = Configlet::findConfigletInFile("top_comment", "test1.fw");

    // find string from the firewall object comment and compare
    CPPUNIT_ASSERT(res.indexOf("# Firewall object test1 comment"));

    delete objdb;
}

void GeneratedScriptTest::CheckUtilitiesTest()
{
    QStringList utils;

    QStringList test1_utils;
    test1_utils << "find_program $IPTABLES";
    test1_utils << "find_program $MODPROBE";
    test1_utils << "find_program $IP";

    QStringList test2_utils;
    test2_utils << "find_program $IPTABLES";
    test2_utils << "find_program $IPTABLES_RESTORE";
    test2_utils << "find_program $MODPROBE";
    test2_utils << "find_program $IP";


    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test1", "test1.fw");
    QString res = Configlet::findConfigletInFile("check_utilities", "test1.fw");
    foreach(QString line, res.split("\n"))
    {
        if (line.indexOf("find_program ")!=-1)
        {
            utils.push_back(line.trimmed());
        }
    }
    CPPUNIT_ASSERT(utils == test1_utils);
    delete objdb;

    utils.clear();

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test2", "test2.fw");
    res = Configlet::findConfigletInFile("check_utilities", "test2.fw");
    foreach(QString line, res.split("\n"))
    {
        if (line.indexOf("find_program ")!=-1)
        {
            utils.push_back(line.trimmed());
        }
    }
    CPPUNIT_ASSERT(utils == test2_utils);
    delete objdb;

}


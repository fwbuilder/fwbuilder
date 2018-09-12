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


#include "generatedScriptTestsIpfw.h"

#include "CompilerDriver_ipfw.h"
#include "Configlet.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Constants.h"

#include <QApplication>
#include <QStringList>
#include <QFileInfo>
#include <QtDebug>
#include <QDir>
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
                                      const std::string &generate_file_name,
                                      const std::string &output_file_option)
{
    loadDataFile(test_file);

    QStringList args;

    if (!output_file_option.empty())
    {
        args << "-o" << output_file_option.c_str();
    }

    args << firewall_object_name.c_str();

    CompilerDriver_ipfw driver(objdb);

    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipfw initialization failed",
                           driver.prepare(args) == true);
    driver.compile();
    // compiler should have created file generate_file_name
    QFileInfo fi(generate_file_name.c_str());
    CPPUNIT_ASSERT_MESSAGE("Generated file " + generate_file_name + " not found",
                           fi.exists() == true);
}

// I can check only certain parts of the top comment. Can't
// compare against "golden" file because some parts of the comment
// are variable, such as date, version and build number

void GeneratedScriptTest::ManifestTest_1()
{
    QFile::remove("ipfw1.fw");
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipfw1", "ipfw1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipfw1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipfw1.fw") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_2()
{
    /*
     * output script name is set to ipfw2-1.fw in the fw object.
     */
    QFile::remove("ipfw2-1.fw");
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipfw2", "ipfw2-1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipfw2-1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipfw2-1.fw") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_3()
{
    /*
     * output script name is set to ipfw2-1 in the fw object (no extension)
     */
    QFile::remove("ipfw2-1");
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipfw2a", "ipfw2-1");
    QString res = Configlet::findConfigletInFile("top_comment", "ipfw2-1");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipfw2-1") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_4()
{
    /*
     * Compile ipfw2 and ipfw2a adding "-o" option as instDialog does
     */
    objdb = new FWObjectDatabase();
    QString option_o = QDir::currentPath() + "/ipfw2-1.fw";
    QFile::remove(option_o);
    runCompiler("test1.fwb", "ipfw2", "ipfw2-1.fw", option_o.toStdString());
    QString res = Configlet::findConfigletInFile("top_comment", "ipfw2-1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * " + option_o) != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_5()
{
    objdb = new FWObjectDatabase();
    QString option_o = QDir::currentPath() + "/ipfw2-1";
    QFile::remove(option_o);
    runCompiler("test1.fwb", "ipfw2a", "ipfw2-1.fw", option_o.toStdString());
    QString res = Configlet::findConfigletInFile("top_comment", "ipfw2-1");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipfw2-1") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_6()
{
    /*
     * remote ipfw and nat files are configured as /etc/fw/ipfw3-ipfw.conf and
     * /etc/fw/ipfw3-nat.conf in ipfw3
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipfw3", "ipfw3.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipfw3.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipfw3.fw") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_7()
{
    /*
     * remote ipfw and nat files in ipfw4 have spaces in the path
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipfw4", "ipfw4.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipfw4.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipfw4.fw /etc/path\\ with\\ space/ipfw4.fw") != -1);
    delete objdb;
}

// ************************************************************************

void GeneratedScriptTest::FwCommentTest()
{
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipfw1", "ipfw1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipfw1.fw");
    // find string from the firewall object comment and compare
    CPPUNIT_ASSERT(res.indexOf("# Firewall object test1 comment") != -1);
    delete objdb;
}


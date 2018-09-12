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


#include "generatedScriptTestsIpfilter.h"

#include "CompilerDriver_ipf.h"
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

    CompilerDriver_ipf driver(objdb);

    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipf initialization failed",
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
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipf1", "ipf1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipf1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipf1.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf1-ipf.conf") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf1-nat.conf") != -1);
    delete objdb;
}
 
void GeneratedScriptTest::ManifestTest_2()
{
    /*
     * output script name is set to ipf2-1.fw in the fw object. This
     * parameter is used by instDialog and passed to the compiler via
     * "-o" command line option. Calling compiler without this option
     * produces file with standard name.
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipf2", "ipf2-1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipf2-1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipf2-1.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1-ipf.conf") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1-nat.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_3()
{
    /*
     * output script name is set to ipf2a in the fw object. This
     * parameter is used by instDialog and passed to the compiler via
     * "-o" command line option. Calling compiler without this option
     * produces file with standard name.
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipf2a", "ipf2-1");
    QString res = Configlet::findConfigletInFile("top_comment", "ipf2-1");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipf2-1") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1-ipf.conf") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1-nat.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_4()
{
    /*
     * Compile ipf2 and ipf2a adding "-o" option as instDialog does
     */
    objdb = new FWObjectDatabase();
    QString option_o = QDir::currentPath() + "/ipf2-1.fw";
    runCompiler("test1.fwb", "ipf2", "ipf2-1.fw", option_o.toStdString());
    QString res = Configlet::findConfigletInFile("top_comment", "ipf2-1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * " + option_o) != -1);
    QString ipf_file = QDir::currentPath() + "/ipf2-1-ipf.conf";
    QString nat_file = QDir::currentPath() + "/ipf2-1-nat.conf";
    CPPUNIT_ASSERT(res.indexOf("# files:   " + ipf_file) != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   " + nat_file) != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_5()
{
    objdb = new FWObjectDatabase();
    QString option_o = QDir::currentPath() + "/ipf2-1";
    runCompiler("test1.fwb", "ipf2a", "ipf2-1.fw", option_o.toStdString());
    QString res = Configlet::findConfigletInFile("top_comment", "ipf2-1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipf2-1.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1-ipf.conf") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1-nat.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_6()
{
    /*
     * remote ipf and nat files are configured as /etc/fw/ipf3-ipf.conf and
     * /etc/fw/ipf3-nat.conf in ipf3
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipf3", "ipf3.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipf3.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipf3.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf3-ipf.conf /etc/fw/ipf3-ipf.conf") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf3-nat.conf /etc/fw/ipf3-nat.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_7()
{
    /*
     * remote ipf and nat files in ipf4 have spaces in the path
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipf4", "ipf4.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipf4.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipf4.fw /etc/path\\ with\\ space/ipf4.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf4-ipf.conf /etc/path\\ with\\ space/ipf4-ipf.conf") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf4-nat.conf /etc/path\\ with\\ space/ipf4-nat.conf") != -1);
    delete objdb;
}

// ************************************************************************

void GeneratedScriptTest::FwCommentTest()
{
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "ipf1", "ipf1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipf1.fw");
    // find string from the firewall object comment and compare
    CPPUNIT_ASSERT(res.indexOf("# Firewall object test1 comment") != -1);
    delete objdb;
}

// ************************************************************************

void GeneratedScriptTest::ActivationCommandsTest_1()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "ipf1.fw");
    CPPUNIT_ASSERT(res.indexOf("$IPF  -I -f /etc/ipf1-ipf.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_2()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "ipf2-1.fw");
    CPPUNIT_ASSERT(res.indexOf("$IPF  -I -f /etc/ipf2-1-ipf.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_3()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "ipf2-1");
    CPPUNIT_ASSERT(res.indexOf("$IPF  -I -f /etc/ipf2-1-ipf.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_4()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "ipf2-1.fw");
    CPPUNIT_ASSERT(res.indexOf("$IPF  -I -f /etc/ipf2-1-ipf.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_6()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "ipf3.fw");
    CPPUNIT_ASSERT(res.indexOf("$IPF  -I -f /etc/fw/ipf3-ipf.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_7()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "ipf4.fw", 1);
    CPPUNIT_ASSERT(res.indexOf("$IPF  -I -f /etc/path\\ with\\ space/ipf4-ipf.conf") != -1);

    res = Configlet::findConfigletInFile("activation", "ipf4.fw", 2);
    CPPUNIT_ASSERT(res.indexOf("$IPNAT  -f /etc/path\\ with\\ space/ipf4-nat.conf") != -1);

    delete objdb;
}



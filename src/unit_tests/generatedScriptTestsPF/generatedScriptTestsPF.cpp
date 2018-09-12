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


#include "generatedScriptTestsPF.h"

#include "CompilerDriver_pf.h"
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

    CompilerDriver_pf driver(objdb);

    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_pf initialization failed",
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
    QFile::remove("pf1.fw");
    QFile::remove("pf1.conf");
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf1", "pf1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "pf1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * pf1.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   pf1.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_2()
{
    /*
     * output script name is set to ipf2-1.fw in the fw object and
     * conf file name is set to ipf2-1.conf
     */
    QFile::remove("ipf2-1.fw");
    QFile::remove("ipf2-1.conf");
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf2", "ipf2-1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipf2-1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipf2-1.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_3()
{
    /*
     * output script name is set to "ipf2-1" in the fw object (no .fw
     * extension) and conf file name is set to ipf2-1.conf
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf2a", "ipf2-1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "ipf2-1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipf2-1.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_4()
{
    /*
     * Compile pf2 and pf2a adding "-o" option as instDialog
     * does. This oevverides .fw file name but does not override .conf
     * file name
     */
    objdb = new FWObjectDatabase();
    QString option_o = QDir::currentPath() + "/pf2-1.fw";
    runCompiler("test1.fwb", "pf2", "pf2-1.fw", option_o.toStdString());
    QString res = Configlet::findConfigletInFile("top_comment", "pf2-1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * " + option_o) != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_5()
{
    objdb = new FWObjectDatabase();
    QString option_o = QDir::currentPath() + "/pf2-1";
    runCompiler("test1.fwb", "pf2a", "pf2-1.fw", option_o.toStdString());
    QString res = Configlet::findConfigletInFile("top_comment", "pf2-1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * ipf2-1.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   ipf2-1.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_6()
{
    /*
     * remote pf file is configured as /etc/fw/pf3.conf in pf3
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf3", "pf3.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "pf3.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * pf3.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   pf3.conf /etc/fw/pf3.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_7()
{
    /*
     * remote pf and nat files in pf4 have spaces in the path
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf4", "pf4.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "pf4.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * pf4.fw /etc/path\\ with\\ space/pf4.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   pf4.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_8()
{
    /*
     * generated .fw and .conf files have different base names
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf5", "pf5.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "pf5.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * pf5.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   pf5.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_9()
{
    /*
     * generated .fw and .conf files have different base names
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf6", "/tmp/pf6.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "/tmp/pf6.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * /tmp/pf6.fw /etc/fw/pf6.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   /tmp/pf6.conf /etc/pf.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ManifestTest_10()
{
    /*
     * generated .fw and .conf files have different base names
     */
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf7", "tmp/pf7.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "tmp/pf7.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * tmp/pf7.fw /etc/fw/pf7.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   tmp/pf.conf /etc/pf.conf") != -1);
    delete objdb;
}

/*
 * pf8 has an achor and compiler generates two .conf files. The name
 * of the second file is derived from the name of the main .conf file
 * that is is set in fw options
 */
void GeneratedScriptTest::ManifestTest_11()
{
    /*
     * generated .fw and .conf files have different base names
     */
    if (QFile::exists("tmp/pf8.fw")) QFile::remove("tmp/pf8.fw");
    if (QFile::exists("tmp/pf.conf")) QFile::remove("tmp/pf.conf");
    if (QFile::exists("tmp/pf-anchor_1.conf")) QFile::remove("tmp/pf-anchor_1.conf");

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf8", "tmp/pf8.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "tmp/pf8.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * tmp/pf8.fw /etc/fw/pf8.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   tmp/pf.conf /etc/pf.conf") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   tmp/pf-anchor_1.conf /etc/pf-anchor_1.conf") != -1);
    delete objdb;
}

/*
 * pf9 has an achor and compiler generates two .conf files. Names for
 * all generated files are constructed using firewall name and default
 * algorithm
 */
void GeneratedScriptTest::ManifestTest_12()
{
    /*
     * generated .fw and .conf files have different base names
     */
    if (QFile::exists("pf9.fw")) QFile::remove("pf9.fw");
    if (QFile::exists("pf9.conf")) QFile::remove("pf9.conf");
    if (QFile::exists("pf9-anchor_1.conf")) QFile::remove("pf9-anchor_1.conf");

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf9", "pf9.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "pf9.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * pf9.fw /etc/pf9.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   pf9.conf /etc/pf9.conf") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files:   pf9-anchor_1.conf /etc/pf9-anchor_1.conf") != -1);
    delete objdb;
}

// ************************************************************************

void GeneratedScriptTest::FwCommentTest()
{
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "pf1", "pf1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "pf1.fw");
    // find string from the firewall object comment and compare
    CPPUNIT_ASSERT(res.indexOf("# Firewall object test1 comment") != -1);
    delete objdb;
}

// ************************************************************************
//
// $PFCTL                                        \
//     -f                                        \
//    ${FWDIR}/pf1.conf || exit 1

void GeneratedScriptTest::ActivationCommandsTest_1()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "pf1.fw")
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/pf1.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_2()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "ipf2-1.fw")
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/ipf2-1.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_3()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "ipf2-1.fw")
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/ipf2-1.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_4()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "pf2-1.fw")
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/ipf2-1.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_6()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "pf3.fw")
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/fw/pf3.conf") != -1);
    delete objdb;
}

/*
 * object pf4 defines remote name for the .fw file but does not define
 * remote name for the .conf file. The latter is determined by
 * default, by combining local file name with directory defined in
 * firewall_dir option (in the tab "Installer")
 */
void GeneratedScriptTest::ActivationCommandsTest_7()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "pf4.fw")
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/pf4.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_8()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "pf5.fw")
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/pf5.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_9()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "/tmp/pf6.fw")
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/pf.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_10()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "tmp/pf7.fw")
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/pf.conf") != -1);
    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_11()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "tmp/pf8.fw", 1)
        .split(QRegExp("\\s+")).join(" ");

    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/pf.conf") != -1);

    delete objdb;
}

void GeneratedScriptTest::ActivationCommandsTest_12()
{
    objdb = new FWObjectDatabase();
    QString res = Configlet::findConfigletInFile("activation", "pf9.fw", 1)
        .split(QRegExp("\\s+")).join(" ");
    CPPUNIT_ASSERT(res.indexOf("$PFCTL -f /etc/pf9.conf") != -1);

    delete objdb;
}




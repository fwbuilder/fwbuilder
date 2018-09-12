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


#include "generatedScriptTestsLinux.h"

#include "CompilerDriver_ipt.h"
#include "Configlet.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Constants.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QtDebug>

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
    try
    {
        /* load the data file */
        UpgradePredicate upgrade_predicate; 

        objdb->setReadOnly( false );
        objdb->load(file_name, &upgrade_predicate, Constants::getDTDDirectory());
        objdb->setFileName(file_name);
        objdb->reIndex();
    } catch (FWException &ex)
    {
        qDebug() << ex.toString().c_str();
    }
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
    CPPUNIT_ASSERT_MESSAGE("Generated file " + generate_file_name + " not found",
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

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test3", "test3.fw");
    res = Configlet::findConfigletInFile("top_comment", "test3.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * test3.fw firewall.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test1.fw") == -1);
    delete objdb;

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test4", "test4.fw");
    res = Configlet::findConfigletInFile("top_comment", "test4.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * test4.fw /etc/init.d/firewall.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test4.fw firewall.fw") == -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test1.fw") == -1);
    delete objdb;

}

void GeneratedScriptTest::FwCommentTest()
{
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test1", "test1.fw");
    QString res = Configlet::findConfigletInFile("top_comment", "test1.fw");
    // find string from the firewall object comment and compare
    CPPUNIT_ASSERT(res.indexOf("# Firewall object test1 comment") != -1);
    delete objdb;
}

void GeneratedScriptTest::CheckUtilitiesTest()
{
    QStringList utils;

    QStringList test1_utils;
    test1_utils << "find_program which";
    test1_utils << "find_program $IPTABLES";
    test1_utils << "find_program $MODPROBE";
    test1_utils << "find_program $IP";

    QStringList test2_utils;
    test2_utils << "find_program which";
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

void GeneratedScriptTest::verifyInterfacesTest()
{
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test1", "test1.fw");
    QString res = Configlet::findConfigletInFile("verify_interfaces", "test1.fw");
    CPPUNIT_ASSERT(res.indexOf("for i in eth0 eth1 lo eth2 ; do") != -1);
    delete objdb;

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test4", "test4.fw");
    res = Configlet::findConfigletInFile("verify_interfaces", "test4.fw");
    CPPUNIT_ASSERT(res.indexOf("for i in eth0 eth1 lo eth2 eth1.200 vlan110 vlan111 ; do") != -1);
    delete objdb;
}

void GeneratedScriptTest::configureInterfacesTest()
{
    QStringList sample;
    sample << "update_addresses_of_interface \"eth0 192.0.2.1/24\" \"\"";
    sample << "update_addresses_of_interface \"lo 127.0.0.1/8\" \"\"";
    sample << "update_addresses_of_interface \"eth1.200 fe80::20c:29ff:fed2:cca1/64 192.168.1.1/24\" \"\"";
    sample << "update_addresses_of_interface \"vlan110 192.168.2.1/24\" \"\"";
    sample << "update_addresses_of_interface \"vlan111 192.168.3.1/24\" \"\"";


    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test4", "test4.fw");
    // unfortunately function configure_interfaces is not generated by its
    // own configlet
    QString res = Configlet::findConfigletInFile("script_skeleton", "test4.fw");
    int n1 = res.indexOf("configure_interfaces() {");
    CPPUNIT_ASSERT_MESSAGE("Shell function configure_interfaces is missing", n1 != -1);
    int n2 = res.indexOf("}", n1);
    res = res.mid(n1, n2-n1);

    QStringList intf_list;
    foreach(QString line, res.split("\n"))
    {
        if (line.indexOf("update_addresses_of_interface ")!=-1)
        {
            intf_list.push_back(line.trimmed());
        }
    }

    sample.sort();
    intf_list.sort();

    CPPUNIT_ASSERT(sample == intf_list);

    delete objdb;
}

void GeneratedScriptTest::configureInterfacesClusterTest()
{
    QStringList sample_2;
    sample_2 << "update_addresses_of_interface \"eth0 192.0.2.1/24\" \"192.0.2.100/24 192.0.2.101/24\"";
    sample_2 << "update_addresses_of_interface \"eth1 192.168.1.1/24\" \"192.168.1.100/24\"";
    sample_2 << "update_addresses_of_interface \"lo 127.0.0.1/8\" \"\"";
    sample_2 << "update_addresses_of_interface \"eth2 192.168.2.1/24\" \"192.168.2.100/24\"";

    QStringList sample_3;
    sample_3 << "update_addresses_of_interface \"eth0 192.0.2.2/24\" \"192.0.2.100/24 192.0.2.101/24\"";
    sample_3 << "update_addresses_of_interface \"eth1 192.168.1.2/24\" \"192.168.1.100/24\"";
    sample_3 << "update_addresses_of_interface \"lo 127.0.0.1/8\" \"\"";
    sample_3 << "update_addresses_of_interface \"eth2 192.168.2.2/24\" \"192.168.2.100/24\"";

    sample_2.sort();
    sample_3.sort();

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "cluster-2-3", "test2.fw");
    // unfortunately function configure_interfaces is not generated by its
    // own configlet
    QString res = Configlet::findConfigletInFile("script_skeleton", "test2.fw");
    int n1 = res.indexOf("configure_interfaces() {");
    CPPUNIT_ASSERT_MESSAGE("Shell function configure_interfaces is missing", n1 != -1);
    int n2 = res.indexOf("}", n1);
    res = res.mid(n1, n2-n1);

    QStringList intf_list;
    foreach(QString line, res.split("\n"))
    {
        if (line.indexOf("update_addresses_of_interface ")!=-1)
        {
            intf_list.push_back(line.trimmed());
        }
    }

    intf_list.sort();
    CPPUNIT_ASSERT(sample_2 == intf_list);

    intf_list.clear();

    res = Configlet::findConfigletInFile("script_skeleton", "test3.fw");
    n1 = res.indexOf("configure_interfaces() {");
    CPPUNIT_ASSERT_MESSAGE("Shell function configure_interfaces is missing", n1 != -1);
    n2 = res.indexOf("}", n1);
    res = res.mid(n1, n2-n1);

    foreach(QString line, res.split("\n"))
    {
        if (line.indexOf("update_addresses_of_interface ")!=-1)
        {
            intf_list.push_back(line.trimmed());
        }
    }

    intf_list.sort();
    CPPUNIT_ASSERT(sample_3 == intf_list);

    delete objdb;
}

void GeneratedScriptTest::virtualAddressesForNat1Test()
{
    QStringList sample_1;

    sample_1 << "update_addresses_of_interface \"eth0 192.0.2.1/24 192.0.2.100/24 192.0.2.101/24\" \"\"";
    sample_1 << "update_addresses_of_interface \"lo 127.0.0.1/8\" \"\"";

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test5", "test5.fw");
    // unfortunately function configure_interfaces is not generated by its
    // own configlet
    QString res = Configlet::findConfigletInFile("script_skeleton", "test5.fw");
    int n1 = res.indexOf("configure_interfaces() {");
    CPPUNIT_ASSERT_MESSAGE("Shell function configure_interfaces is missing", n1 != -1);
    int n2 = res.indexOf("}", n1);
    res = res.mid(n1, n2-n1);

    QStringList intf_list;
    foreach(QString line, res.split("\n"))
    {
        if (line.indexOf("update_addresses_of_interface ")!=-1)
        {
            intf_list.push_back(line.trimmed());
        }
    }

    intf_list.sort();
    CPPUNIT_ASSERT(sample_1 == intf_list);

    delete objdb;
}

void GeneratedScriptTest::virtualAddressesForNat2Test()
{
    QStringList sample_1;

    sample_1 << "update_addresses_of_interface \"eth0 192.0.2.100/24 192.0.2.101/24\" \"192.0.2.1/24\"";

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test6", "test6.fw");
    // unfortunately function configure_interfaces is not generated by its
    // own configlet
    QString res = Configlet::findConfigletInFile("script_skeleton", "test6.fw");
    int n1 = res.indexOf("configure_interfaces() {");
    CPPUNIT_ASSERT_MESSAGE("Shell function configure_interfaces is missing", n1 != -1);
    int n2 = res.indexOf("}", n1);
    res = res.mid(n1, n2-n1);

    QStringList intf_list;
    foreach(QString line, res.split("\n"))
    {
        if (line.indexOf("update_addresses_of_interface ")!=-1)
        {
            intf_list.push_back(line.trimmed());
        }
    }

    intf_list.sort();
    CPPUNIT_ASSERT(sample_1 == intf_list);

    delete objdb;
}

/*
 * negative test first. test6 does not use ipset module and the configlet should
 * only insert blank function check_run_time_address_table_files()
 */
void GeneratedScriptTest::runTimeAddressTablesWithIpSet1Test()
{
    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test6", "test6.fw");

    QString res = Configlet::findConfigletInFile("run_time_address_tables", "test6.fw");
    CPPUNIT_ASSERT(!res.isEmpty());
    int n1 = res.indexOf("load_run_time_address_table_files() {");
    CPPUNIT_ASSERT(n1 == -1);
    n1 = res.indexOf("check_run_time_address_table_files() {");
    CPPUNIT_ASSERT(n1 != -1);

    delete objdb;
}

void GeneratedScriptTest::runTimeAddressTablesWithIpSet2Test()
{
    QStringList sample_1;

    sample_1 << "reload_address_table \"bad_guys\" \"/etc/fw/bad_guys.dat\"";
    sample_1 << "reload_address_table \"bad_guys_2\" \"/etc/fw/bad_guys.dat\"";

    QStringList sample_2;

    sample_2 << "check_file \"bad_guys\" \"/etc/fw/bad_guys.dat\"";
    sample_2 << "check_file \"bad_guys_2\" \"/etc/fw/bad_guys.dat\"";

    objdb = new FWObjectDatabase();
    runCompiler("test1.fwb", "test7", "test7.fw");

    QString res = Configlet::findConfigletInFile("run_time_address_tables", "test7.fw");
    int n1 = res.indexOf("load_run_time_address_table_files() {");
    CPPUNIT_ASSERT_MESSAGE("Shell function load_run_time_address_table_files is missing", n1 != -1);
    int n2 = res.indexOf("}", n1);
    QString conf = res.mid(n1, n2-n1);

    QStringList cmd_list;
    foreach(QString line, conf.split("\n"))
    {
        if (line.indexOf("reload_address_table ")!=-1)
        {
            cmd_list.push_back(line.trimmed());
        }
    }

    cmd_list.sort();
    CPPUNIT_ASSERT(sample_1 == cmd_list);

    n1 = res.indexOf("check_run_time_address_table_files() {");
    CPPUNIT_ASSERT_MESSAGE("Shell function check_run_time_address_table_files is missing", n1 != -1);
    n2 = res.indexOf("}", n1);
    conf = res.mid(n1, n2-n1);

    cmd_list.clear();
    foreach(QString line, conf.split("\n"))
    {
        if (line.indexOf("check_file ")!=-1)
        {
            cmd_list.push_back(line.trimmed());
        }
    }

    cmd_list.sort();
    CPPUNIT_ASSERT(sample_2 == cmd_list);

    delete objdb;
}

// compiler should place generated script in the directory specified
// with -d option
void GeneratedScriptTest::minusDTest()
{
    QDir current = QDir::current();

    QFile f("/tmp/test1.fw");
    f.remove();

    objdb = new FWObjectDatabase();

    loadDataFile("test1.fwb");

    QStringList args;
    args << "-d" <<  "/tmp" << "test1";

    CompilerDriver_ipt driver(objdb);
    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipt initialization failed",
                           driver.prepare(args) == true);
    driver.compile();
    // compiler should have created file /tmp/test1.fw
    QFileInfo fi("/tmp/test1.fw");
    CPPUNIT_ASSERT_MESSAGE("Generated file /tmp/test1.fw not found",
                           fi.exists() == true);

    QString res = Configlet::findConfigletInFile("top_comment", "/tmp/test1.fw");
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * test1.fw") != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test2.fw") == -1);
    delete objdb;

    QDir::setCurrent(current.path());
}

// if -o option is given, it defines the path and name for the
// generated script
void GeneratedScriptTest::minusOTest1()
{
    QDir current = QDir::current();

    objdb = new FWObjectDatabase();

    qDebug() << "Data load starts";

    loadDataFile("test1.fwb");

    qDebug() << "Data loaded";

    QString full_output_file_name = QString("%1/test1.fw").arg(current.path());

    QStringList args;
    args << "-o" <<  full_output_file_name << "test1";

    if (QFile::exists(full_output_file_name)) QFile::remove(full_output_file_name);

    CompilerDriver_ipt driver(objdb);
    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipt initialization failed",
                           driver.prepare(args) == true);
    driver.compile();
    // compiler should have created file full_output_file_name
    QFileInfo fi(full_output_file_name);
    CPPUNIT_ASSERT_MESSAGE("Generated file " +
                           full_output_file_name.toStdString() + " not found",
                           fi.exists() == true);

    QString res = Configlet::findConfigletInFile(
        "top_comment", full_output_file_name);
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * " + full_output_file_name) != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test2.fw") == -1);
    delete objdb;
}

// same as the previous but different output file name
void GeneratedScriptTest::minusOTest2()
{
    QDir current = QDir::current();

    objdb = new FWObjectDatabase();

    loadDataFile("test1.fwb");

    QString full_output_file_name = QString("%1/foo1.fw").arg(current.path());

    QStringList args;
    args << "-o" <<  full_output_file_name << "test1";

    if (QFile::exists(full_output_file_name)) QFile::remove(full_output_file_name);

    CompilerDriver_ipt driver(objdb);
    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipt initialization failed",
                           driver.prepare(args) == true);
    driver.compile();
    // compiler should have created file full_output_file_name
    QFileInfo fi(full_output_file_name);
    CPPUNIT_ASSERT_MESSAGE("Generated file " +
                           full_output_file_name.toStdString() + " not found",
                           fi.exists() == true);

    QString res = Configlet::findConfigletInFile(
        "top_comment", full_output_file_name);
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * " + full_output_file_name) != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test1.fw") == -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test2.fw") == -1);
    delete objdb;
}

// if both -d and -o are present, -o takes precedence
void GeneratedScriptTest::minusDminusOTest()
{
    QDir current = QDir::current();

    objdb = new FWObjectDatabase();

    loadDataFile("test1.fwb");

    QString full_output_file_name = QString("%1/test1.fw").arg(current.path());

    if (QFile::exists(full_output_file_name)) QFile::remove(full_output_file_name);

    QStringList args;
    args << "-d" <<  "/tmp" << "-o" << full_output_file_name << "test1";

    CompilerDriver_ipt driver(objdb);
    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipt initialization failed",
                           driver.prepare(args) == true);
    driver.compile();
    // compiler should have created file full_output_file_name
    QFileInfo fi(full_output_file_name);
    CPPUNIT_ASSERT_MESSAGE("Generated file " +
                           full_output_file_name.toStdString() + " not found",
                           fi.exists() == true);

    QString res = Configlet::findConfigletInFile(
        "top_comment", full_output_file_name);
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * " + full_output_file_name) != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test2.fw") == -1);
    delete objdb;

    QDir::setCurrent(current.path());
}

// output file name is set in firewall settins to foo.fw
void GeneratedScriptTest::outputFileNameOptionTest1()
{
    QDir current = QDir::current();

    objdb = new FWObjectDatabase();

    loadDataFile("test1.fwb");

    QString full_output_file_name = "foo.fw";
    QStringList args;
    args << "test1-1";

    if (QFile::exists(full_output_file_name)) QFile::remove(full_output_file_name);

    CompilerDriver_ipt driver(objdb);
    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipt initialization failed",
                           driver.prepare(args) == true);
    driver.compile();
    // compiler should have created file full_output_file_name
    QFileInfo fi(full_output_file_name);
    CPPUNIT_ASSERT_MESSAGE("Generated file " +
                           full_output_file_name.toStdString() + " not found",
                           fi.exists() == true);

    QString res = Configlet::findConfigletInFile(
        "top_comment", full_output_file_name);
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * " + full_output_file_name) != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test1.fw") == -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test2.fw") == -1);
    delete objdb;
}

// output file name is set in firewall settins to /tmp/foo.fw
void GeneratedScriptTest::outputFileNameOptionTest2()
{
    QDir current = QDir::current();

    objdb = new FWObjectDatabase();

    loadDataFile("test1.fwb");

    QString full_output_file_name = "/tmp/foo.fw";
    QStringList args;
    args << "test1-2";

    if (QFile::exists(full_output_file_name)) QFile::remove(full_output_file_name);

    CompilerDriver_ipt driver(objdb);
    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipt initialization failed",
                           driver.prepare(args) == true);
    driver.compile();
    // compiler should have created file full_output_file_name
    QFileInfo fi(full_output_file_name);
    CPPUNIT_ASSERT_MESSAGE("Generated file " +
                           full_output_file_name.toStdString() + " not found",
                           fi.exists() == true);

    QString res = Configlet::findConfigletInFile(
        "top_comment", full_output_file_name);
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * " + full_output_file_name) != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test1.fw") == -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test2.fw") == -1);
    delete objdb;
}

// output file name is set in firewall settins to foo.fw but is overridden
// by -o option to a file name with absolute path
void GeneratedScriptTest::outputFileNameOptionTest3()
{
    QDir current = QDir::current();

    objdb = new FWObjectDatabase();

    loadDataFile("test1.fwb");

    QString full_output_file_name = QString("%1/bar.fw").arg(current.path());

    if (QFile::exists(full_output_file_name)) QFile::remove(full_output_file_name);

    QStringList args;
    args << "-o" << full_output_file_name << "test1-1";

    if (QFile::exists(full_output_file_name)) QFile::remove(full_output_file_name);

    CompilerDriver_ipt driver(objdb);
    driver.setEmbeddedMode();
    CPPUNIT_ASSERT_MESSAGE("CompilerDriver_ipt initialization failed",
                           driver.prepare(args) == true);
    driver.compile();
    // compiler should have created file full_output_file_name
    QFileInfo fi(full_output_file_name);
    CPPUNIT_ASSERT_MESSAGE("Generated file " +
                           full_output_file_name.toStdString() + " not found",
                           fi.exists() == true);

    QString res = Configlet::findConfigletInFile(
        "top_comment", full_output_file_name);
    // find manifest and compare
    CPPUNIT_ASSERT(res.indexOf("# files: * " + full_output_file_name) != -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test1.fw") == -1);
    CPPUNIT_ASSERT(res.indexOf("# files: * test2.fw") == -1);
    delete objdb;
}


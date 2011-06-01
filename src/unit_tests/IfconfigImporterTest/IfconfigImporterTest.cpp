/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

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

#include "IfconfigImporterTest.h"

#include "config.h"
#include "global.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <assert.h>

#include "Importer.h"
#include "IfconfigImporter.h"
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

void IfconfigImporterTest::setUp()
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

std::string IfconfigImporterTest::openTestFile(const QString &file_name)
{
    QFile f(file_name);
    f.open(QFile::ReadOnly);
    string buffer = QString(f.readAll()).toStdString();
    f.close();

    return buffer;
}

void IfconfigImporterTest::linuxIfconfigTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/linux_ifconfig.test"));

    IfconfigImporter* imp = new IfconfigImporter(
        lib, instream, logger, "linux24", "test_fw");
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    while (logger->ready())
        qDebug() << logger->getLine().c_str();

    for (list<InterfaceSpec>::iterator it=imp->interfaces.begin();
         it!=imp->interfaces.end(); ++it)
    {
        qDebug() << it->toString();
    }
}

void IfconfigImporterTest::bsdIfconfigTest()
{
    platform = "pf";

    std::istringstream instream(
        openTestFile("test_data/bsd_ifconfig.test"));

    IfconfigImporter* imp = new IfconfigImporter(
        lib, instream, logger, "openbsd", "test_fw");
    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->finalize();

    while (logger->ready())
        qDebug() << logger->getLine().c_str();

    for (list<InterfaceSpec>::iterator it=imp->interfaces.begin();
         it!=imp->interfaces.end(); ++it)
    {
        qDebug() << it->toString();
    }
}


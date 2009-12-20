#include "ImporterTest.h"

#include "../../config.h"
#include "global.h"

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <qstring.h>

#ifndef _WIN32
#  include <unistd.h>
#  include <pwd.h>
#else
#  include <direct.h>
#  include <stdlib.h>
#  include <io.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>

#include "../../Importer.h"
#include "../../IOSImporter.h"
#include "../../IPTImporter.h"

#include "../../FWBTree.h"


#ifdef HAVE_GETOPT_H
  #include <getopt.h>
#else
  #ifdef _WIN32
    #include <getopt.h>
  #else
    #include <stdlib.h>
  #endif
#endif

#include <QDebug>
#include <QFile>
#include <QStringList>



using namespace std;
using namespace libfwbuilder;

extern string platform;

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string &msg) const
    {
        return false;
    }
};

void ImporterTest::setUp()
{
    init();
    QStringList path;
    path << ".." << ".." << ".." << "res" << "resources.xml";
    new Resources(path.join(FS_SEPARATOR).toStdString());
    new FWBTree();

    /* create database */
    db = new FWObjectDatabase();

    /* load the data file */
    UpgradePredicate upgrade_predicate;

    db->setReadOnly( false );
    sysfname = "../../../res/objects_init.xml";
    librespath = ".";
    qDebug() << sysfname.c_str() << librespath.c_str();
    db->load( sysfname, &upgrade_predicate, librespath);
    db->setFileName("");
    lib = Library::cast(db->create(Library::TYPENAME));
    lib->setName("User");
    db->add(lib);

    logger = new QueueLogger();
}

void ImporterTest::IOSImporterTest()
{
    platform = "iosacl";

    QFile f("test_data/test1.conf");
    f.open(QFile::ReadOnly);

    string buffer = QString(f.readAll()).toStdString();
    f.close();

    std::istringstream instream(buffer);

    Importer* imp = new IOSImporter(lib, instream, logger);

    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->run();


    QString result;
    while (logger->ready())
        result.append(logger->getLine().c_str());

    //imp->finalize(); //will crash if uncomment this

    QFile rr("test_data/test1.result");
    rr.open(QFile::ReadOnly);
    QString rightResult = rr.readAll();

    CPPUNIT_ASSERT(result == rightResult);

}

void ImporterTest::IPTImporterTest()
{
    platform = "iptables";

    QFile f("test_data/iptables_test1.conf");
    f.open(QFile::ReadOnly);

    string buffer = QString(f.readAll()).toStdString();
    f.close();

    std::istringstream instream(buffer);

    Importer* imp = new IPTImporter(lib, instream, logger);

    CPPUNIT_ASSERT_NO_THROW( imp->run() );
    imp->run();


    QString result;
    while (logger->ready())
        result.append(logger->getLine().c_str());

    imp->finalize();

    QFile rr("test_data/iptables_test1.result");
    rr.open(QFile::ReadOnly);
    QString rightResult = rr.readAll();

    CPPUNIT_ASSERT(result == rightResult);

}

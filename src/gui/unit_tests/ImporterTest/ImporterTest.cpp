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

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Logger.h"

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
#include <QStringList>
//#include "../../../common/init.cpp"

using namespace std;
using namespace libfwbuilder;

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string &msg) const
    {
        return false;
    }
};

void ImporterTest::testImporter()
{

    init();
    string platform("linux24");

    try
    {
        QStringList path;
        path << ".." << ".." << ".." << "res" << "resources.xml";
        new Resources(path.join(FS_SEPARATOR).toStdString());
        new FWBTree();

        qDebug() << "1";
        /* create database */
        FWObjectDatabase *objdb = new FWObjectDatabase();

        /* load the data file */
        UpgradePredicate upgrade_predicate;

        objdb->setReadOnly( false );
        objdb->load( sysfname, &upgrade_predicate, librespath);
        objdb->setFileName("");
        FWObject *lib = objdb->create(Library::TYPENAME);
        lib->setName("User");
        objdb->add(lib);

        libfwbuilder::QueueLogger *logger = new libfwbuilder::QueueLogger();

        qDebug() << "2";

        string buffer;
        string s;
        while (!cin.eof())
        {
            getline(cin,s);
            buffer += s;
            buffer += '\n';
        }

        std::istringstream instream(buffer);

        Importer* imp;
        if (platform=="iosacl")
            imp = new IOSImporter(lib, instream, logger);

        if (platform == "iptables")
            imp = new IPTImporter(lib, instream, logger);

        CPPUNIT_ASSERT_THROW(imp->run(), ImporterException);
        while (logger->ready())
            cout << logger->getLine();

        imp->finalize();

        cout << endl;
        cout << flush;

    } catch(const FWException &ex)  {
        qDebug() << ex.toString().c_str();
        CPPUNIT_ASSERT(false);
#if __GNUC__ >= 3
/* need to check version because std::ios::failure does not seem to be
 * supported in gcc 2.9.5 on FreeBSD 4.10 */
    } catch (const std::ios::failure &e) {
        CPPUNIT_ASSERT(false);
#endif
    } catch (const std::string &s) {
        CPPUNIT_ASSERT(false);
    } catch (const std::exception &ex) {
        CPPUNIT_ASSERT(false);
    } catch (...) {
        CPPUNIT_ASSERT(false);
    }
}

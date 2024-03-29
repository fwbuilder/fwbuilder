/* 

                         Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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


#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <functional>

#ifdef _WIN32
#  include <direct.h>
#else
#  include <unistd.h>
#endif

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include <cstring>

#include "version.h"
#include "CompilerDriver_pix.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/Constants.h"

#include <QCoreApplication>
#include <QStringList>
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    #include <QTextCodec>
#endif

#include "../common/init.cpp"

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


FWObjectDatabase *objdb = nullptr;


class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string &msg) const 
    { 
        (void)msg.size();  // to make compiler happy about unused parameter
	cout << "Data file has been created in the old version of Firewall Builder.\nLoad it in the GUI to convert it to the new version." << endl;
	return false;
    }
};


void usage(const char *name)
{
    cout << "Firewall Builder:  policy compiler for Cisco PIX firewall (with support for FWSM)" << endl;
    cout << "Copyright 2002-2009 NetCitadel, LLC" << endl;
    cout << "Version " << VERSION << endl;
    cout << "Usage: " << name << " [-tvV] [-f filename.xml] [-d destdir] [-o output.fw] firewall_object_name" << endl;
}


int main(int argc, char **argv)
{   
    QCoreApplication app(argc, argv, false);

    // compilers always write file names into manifest in Utf8
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));
#endif
    
    QStringList args = app.arguments();

    if (args.size()<=1)
    {
        usage(argv[0]);
        exit(1);
    }

    QString last_arg;
    string filename;
    bool only_print_inspection_code = false;

    for (int idx=0; idx < args.size(); idx++)
    {
        QString arg = args.at(idx);
        last_arg = arg;
        if (arg == "-I")
        {
            only_print_inspection_code = true;
            continue;
        }
        if (arg == "-V")
        {
            usage(argv[0]);
            exit(0);
        }
        if (arg == "-f")
        {
            idx++;
            filename = string(args.at(idx).toLatin1().constData());
            continue;
        }
    }

    if (filename.empty())
    {
        usage(argv[0]);
        exit(1);
    }

    init(argv);

    try
    {
        new Resources(Constants::getResourcesFilePath());

/* create database */
        objdb = new FWObjectDatabase();

/* load the data file */
        UpgradePredicate upgrade_predicate; 

        cout << " *** Loading data ...";

        objdb->setReadOnly( false );
        objdb->load( filename, &upgrade_predicate, Constants::getDTDDirectory());
        objdb->setFileName(filename);
        objdb->reIndex();

        cout << " done\n";

        FWObject *slib = objdb->getById(FWObjectDatabase::STANDARD_LIB_ID);
        if (slib && slib->isReadOnly()) slib->setReadOnly(false);

        CompilerDriver_pix *driver = new CompilerDriver_pix(objdb);
        if (!driver->prepare(args))
        {
            usage(argv[0]);
            exit(1);
        }
        if (only_print_inspection_code)
        {
            cout << driver->protocolInspectorCommands();
        } else
            driver->compile();
        //int ret = (driver->getStatus() == BaseCompiler::FWCOMPILER_SUCCESS) ? 0 : 1;
        int ret = driver->getStatus();

        delete driver;
        delete objdb;

        return ret;

    } catch(libfwbuilder::FWException &ex)
    {
        cerr << ex.toString() << endl;
        return 1;
    } catch (std::string s)
    {
        cerr << s << endl;
        return 1;
    }

    return 0;
}

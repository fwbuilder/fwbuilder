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
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <utility>

#include "CompilerDriver_pf.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Constants.h"

#include <QCoreApplication>
#include <QStringList>
#include <QTextCodec>

#include "../common/init.cpp"

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

FWObjectDatabase       *objdb = nullptr;

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string&) const 
    { 
	cout << "Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it." << endl;
	return false;
    }
};

void usage(const char *name)
{
    cout << "Firewall Builder:  policy compiler for OpenBSD PF" << endl;
    cout << "Version " << VERSION << endl;
    cout << "Usage: " << name << " [-x] [-v] [-V] [-f filename.xml] [-o output.fw] [-d destdir] [-D datadir] [-m] [-4|-6] firewall_object_name" << endl;
}


int main(int argc, char **argv)
{   
    QCoreApplication app(argc, argv, false);

    // compilers always write file names into manifest in Utf8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));
    
    QStringList args = app.arguments();

    if (args.size()<=1)
    {
        usage(argv[0]);
        exit(1);
    }

    QString last_arg;
    string filename;

    for (int idx=0; idx < args.size(); idx++)
    {
        QString arg = args.at(idx);
        last_arg = arg;
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

    IPService::addNamedProtocol(112, "carp");
    IPService::addNamedProtocol(240, "pfsync");

    try
    {
        new Resources(Constants::getResourcesFilePath());

	/* create database */
	objdb = new FWObjectDatabase();

	/* load the data file */
	UpgradePredicate upgrade_predicate; 

	cout << " *** Loading data ...";

        objdb->setReadOnly( false );
        objdb->load( Constants::getStandardObjectsFilePath(),
                     &upgrade_predicate, Constants::getDTDDirectory());
        objdb->setFileName("");
        FWObjectDatabase *ndb = new FWObjectDatabase();
        ndb->load(filename, &upgrade_predicate,  Constants::getDTDDirectory());
        objdb->merge(ndb, nullptr);
        delete ndb;
        objdb->setFileName(filename);
        objdb->reIndex();

	cout << " done\n";

        FWObject *slib = objdb->getById(FWObjectDatabase::STANDARD_LIB_ID);
        if (slib && slib->isReadOnly()) slib->setReadOnly(false);

        CompilerDriver_pf *driver = new CompilerDriver_pf(objdb);
        if (!driver->prepare(args))
        {
            usage(argv[0]);
            exit(1);
        }
        driver->compile();
        //int ret = (driver->getStatus() == BaseCompiler::FWCOMPILER_SUCCESS) ? 0 : 1;
        int ret = driver->getStatus();

        delete driver;
        delete objdb;

        return ret;

    } catch(const FWException &ex)
    {
	cerr << ex.toString() << endl;
        return 1;
#if __GNUC__ >= 3
/* need to check version because std::ios::failure does not seem to be
 * supported in gcc 2.9.5 on FreeBSD 4.10 */
    } catch (const std::ios::failure &e) {
        cerr << "Error while opening or writing to the output file" << endl;
        return 1;
#endif
    } catch (const std::string &s) {
	cerr << s;
        return 1;
    } catch (const std::exception &ex) {
	cerr << ex.what();
        return 1;
    } catch (...) {
	cerr << "Unsupported exception";
        return 1;
    }

}










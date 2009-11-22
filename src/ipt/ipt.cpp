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

#include "../../config.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include "CompilerDriver_ipt.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/IPService.h"

#include <QApplication>
#include <QStringList>
#include <QTextCodec>

#include "../common/init.cpp"

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

FWObjectDatabase *objdb = NULL;

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string&) const 
    { 
	cout << "Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it." << std::endl;
	return false;
    }
};

void usage(const char *name)
{
    cout << "Firewall Builder:  policy compiler for "
            "Linux 2.4.x and 2.6.x iptables" << endl;
    cout << "Version " << VERSION << "-" << build_num.toStdString() << endl;
    cout << "Usage: " << name
         << " [-x level] [-v] [-V] [-q] [-f filename.xml] [-d destdir] "
            "[-m] [-4|-6] firewall_object_name" << endl;
}

int main(int argc, char **argv)
{   
    QApplication app(argc, argv, false);

    // compilers always write file names into manifest in Utf8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Utf8"));
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
        if (arg == "-r")
        {
            idx++;
            respath = string(args.at(idx).toLatin1().constData());
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

    // register protocols we need
    IPService::addNamedProtocol(112, "vrrp");

    try
    {
        Resources res(respath + FS_SEPARATOR + "resources.xml");

	/* create database */
	objdb = new FWObjectDatabase();

	/* load the data file */
	UpgradePredicate upgrade_predicate; 

	cout << " *** Loading data ...";
        cout << flush;

        objdb->setReadOnly( false );
        objdb->load( filename, &upgrade_predicate, librespath);
        objdb->setFileName(filename);
        objdb->reIndex();

	cout << " done\n";
        cout << flush;

        FWObject *slib = objdb->findInIndex(FWObjectDatabase::STANDARD_LIB_ID);
        if (slib && slib->isReadOnly()) slib->setReadOnly(false);

        CompilerDriver_ipt driver(objdb);
        if (!driver.prepare(args))
        {
            usage(argv[0]);
            exit(1);
        }
        driver.compile();
        delete objdb;
        return 0;

    } catch(const FWException &ex) 
    {
	cerr << "Error: " << ex.toString() << std::endl;
        /* Cleanup resources */
        delete objdb;
        return 1;

#if __GNUC__ >= 3
/* need to check version because std::ios::failure does not seem to be
 * supported in gcc 2.9.5 on FreeBSD 4.10 */
    } catch (const std::ios::failure &e) {
        cerr << "Error while opening or writing to the output file"
             << std::endl;
        /* Cleanup ressources */
        delete objdb;
        return 1;
#endif

    } catch (const std::string &s) {
	cerr << s << std::endl;
        return 1;
    } catch (const std::exception &ex) {
	cerr << ex.what() << std::endl;
        return 1;
    }
    catch (...) {
	cerr << "Unsupported exception" << std::endl;
        return 1;
    }

}

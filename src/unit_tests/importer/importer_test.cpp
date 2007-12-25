/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: importer_test.cpp 1393 2007-08-06 07:15:24Z vkurland $

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


#include "config.h"
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

#include "../../gui/Importer.h"
#include "../../gui/IOSImporter.h"
#include "../../gui/IPTImporter.h"

#include "../../gui/FWBTree.h"

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

#include "../../common/init.cpp"

int  fwbdebug   = 1;

using namespace std;
using namespace libfwbuilder;

FWObjectDatabase       *objdb = NULL;

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string &msg) const 
    { 
	cout << _("Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it.") << endl;
	return false;
    }
};

void usage()
{
    
    cout << "Firewall Builder:  unit test for policy importer" << endl;
    cout << "importer_test (iosacl|iptables)" << endl;
    cout << "(expecting test config file on stdin)" << endl;
}

int main(int argc, char * const *argv)
{   

#ifdef HAVE_SETLOCALE
    setlocale (LC_ALL, "");
#endif
    
    if (argc<=1)
    {
        usage();
        exit(1);
    }

    init(argv);

    string platform(argv[1]);

    try
    {
        new Resources(respath+FS_SEPARATOR+"resources.xml");
        new FWBTree();

	/* create database */
	objdb = new FWObjectDatabase();

	/* load the data file */
	UpgradePredicate upgrade_predicate; 

	cout << _(" *** Loading standard data file ...");

        objdb->setReadOnly( false );
        objdb->load( sysfname, &upgrade_predicate, librespath);
        objdb->setFileName("");
        FWObject *lib = objdb->create(Library::TYPENAME);
        lib->setName("User");
        objdb->add(lib);
	cout << _(" done\n");

        //objdb->dump(true,true);

        libfwbuilder::QueueLogger *logger = new libfwbuilder::QueueLogger();

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

        try
        {
            imp->run();
        } catch(ImporterException &e)
        {
            *logger << std::string("Parser error:\n");
            *logger << e.toString() << "\n";
        }
        while (logger->ready())
            cout << logger->getLine();

        imp->finalize();

        cout << endl;
        cout << flush;
       
        return 0;

    } catch(const FWException &ex)  {
	cerr << "Error: " << ex.toString() << endl;
        return 1;
#if __GNUC__ >= 3
/* need to check version because std::ios::failure does not seem to be
 * supported in gcc 2.9.5 on FreeBSD 4.10 */
    } catch (const std::ios::failure &e) {
        cerr << "Error while opening or writing to the output file" << endl;
        return 1;
#endif
    } catch (const std::string &s) {
	cerr << s << endl;
        return 1;
    } catch (const std::exception &ex) {
	cerr << ex.what() << endl;
        return 1;
    } catch (...) {
	cerr << _("Unsupported exception") << endl;
        return 1;
    }

}

/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org
   
  $Id: main.cpp 918 2006-03-05 06:07:10Z vkurland $


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


#include "fwbuilder/libfwbuilder-config.h"

// #include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/IPAddress.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/ObjectGroup.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>

#ifndef _WIN32
#  include <sys/types.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

#include <getopt.h>


using namespace libfwbuilder;
using namespace std;
    

extern void removeObjectTest(FWObjectDatabase* db);
extern void IPAddressTest(FWObjectDatabase* db);


char  *filename="test.xml";

FWObjectDatabase       *objdb = NULL;


void usage()
{
    cout << "  test  [-h] [-f testfile.xml] " << endl;
}

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string &msg) const 
    { 
        cout << "Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it." << endl;
        return false;
    }
};


void initAll()
{
    libfwbuilder::init();

    /* create database */
    objdb = new FWObjectDatabase();

    /* load the data file */
    UpgradePredicate upgrade_predicate; 

//    cout << " *** Loading test file  " << filename << " ..\n";
//    FWObjectDatabase::db->load(filename,  &upgrade_predicate, LIBFWBUILDER_TEMPLATE_DIR);
//    cout << "done\n";
}
  
int main(int argc, char * const *argv)
{   
#if defined(WORDS_LITTLEENDIAN)
    cout << "Byte order: WORDS_LITTLEENDIAN" << endl;
#elif defined(WORDS_BIGENDIAN)
    cout << "Byte order: WORDS_BIGENDIAN" << endl;
#else
    cout << "Byte order: undefined" << endl;
#endif

    cout << "Program executable: " << argv[0] << endl;
    
    int c;
    while ((c = getopt (argc , argv , "?hf:")) != EOF )
	switch (c) {
	case '?':
	case 'h':
	    usage();
	    return(0);

	case 'f':
	    filename=strdup(optarg);
	    break;
	}
    
    try   {
        initAll();

        IPAddressTest(objdb);
        removeObjectTest(objdb);

        return 0;

    } catch(FWException &ex)  {
        cerr << ex.toString() << endl;
        return 1;
    } catch (std::string s) {
        cerr << s;
        return 1;
    } catch (std::exception ex) {
        cerr << ex.what();
        return 1;
    } catch (...) {
        cerr << "Unsupported exception";
        return 1;
    }

    return 0;
}










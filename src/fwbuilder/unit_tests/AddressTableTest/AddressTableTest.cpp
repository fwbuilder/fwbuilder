#include "AddressTableTest.h"
#include "fwbuilder/libfwbuilder-config.h"

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <deque>
#include <vector>
#include <map>
#include <set>

#ifndef _WIN32
#  include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#else
#  ifdef _WIN32
#    include <getopt.h>
#  else
#    include <stdlib.h>
#  endif
#endif
#include "fwbuilder/Resources.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Group.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Network.h"


using namespace std;
using namespace libfwbuilder;

typedef enum { NONE, TEST} command;
typedef deque<string> operands;

typedef set <string, less<string> > S;


void AddressTableTest::runTest()
{

    string objtype;

    FWObjectDatabase       *objdb = NULL;

    operands ops;
    int res=1;

 #ifdef ENABLE_NLS
     setlocale (LC_ALL, "");

     bindtextdomain (PACKAGE, LOCALEDIR);
     textdomain (PACKAGE);
 #else
 #  ifdef HAVE_SETLOCALE
     setlocale (LC_ALL, "");
 #  endif
 #endif

     objdb = new FWObjectDatabase();

     FWObject *nlib = objdb->create(Library::TYPENAME,true);
     objdb->add(nlib);
     nlib->setName( "Library" );

     FWObject *o1 = objdb->create(ObjectGroup::TYPENAME,true);
     o1->setName("Objects");
     nlib->add(o1);

     FWObject *root = objdb->create(ObjectGroup::TYPENAME,true);
     root->setName("Address Tables");
     o1->add(root);

     S addrset,addrres;
     addrset.insert("216.193.197.238/255.255.255.255");
     addrset.insert("207.46.20.60/255.255.255.0");
     addrset.insert("207.46.198.3/255.255.255.255");
     addrset.insert("207.46.198.60/255.255.255.255");
     addrset.insert("207.46.199.30/255.255.255.255");
     addrset.insert("207.46.225.60/255.255.255.252");
     addrset.insert("207.46.19.60/255.255.255.255");
     addrset.insert("192.168.105.57/255.255.255.255");
     addrset.insert("192.168.105.69/255.255.255.255");
     addrset.insert("192.168.105.68/255.255.255.255");
     addrset.insert("192.168.100.0/255.255.255.0");
     addrset.insert("192.168.11.0/255.255.255.0");

     AddressTable *o;
     FWObject* nobj;
     objtype=AddressTable::TYPENAME;

     nobj=objdb->create(objtype,true);
     if (root != NULL)
     {
         root->add(nobj);

         o=AddressTable::cast(nobj);

         o->setName("TestADT");
         o->setSourceName("addresstable.txt");

     }

     try
     {
         o->loadFromSource(false, true);
         list<FWObject*>::const_iterator t=o->begin();
         Network *net;
         FWReference *ref;

         for ( ; t!=o->end(); ++t )
         {
             ref=FWReference::cast(*t);
             CPPUNIT_ASSERT(ref!=NULL);
             net=Network::cast(ref->getPointer());
             CPPUNIT_ASSERT(net!=NULL);
             addrres.insert(net->getAddressPtr()->toString() + "/" + net->getNetmaskPtr()->toString());
         }

         CPPUNIT_ASSERT(addrset==addrres);

     }catch(FWException &ex)
     {
         cerr << ex.toString() << endl;
         cout << "Test failed." << endl;
     }

}

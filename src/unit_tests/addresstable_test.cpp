/* 

                          Firewall Builder

                 Copyright (C) 2005 NetCitadel, LLC

  Author:  Illiya Yalovoy     yalovoy@gmail.com

  $Id: addresstable_test.cpp 918 2006-03-05 06:07:10Z vkurland $

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

#include <assert.h>

using namespace libfwbuilder;
using namespace std;

typedef enum { NONE, TEST} command;
typedef deque<string> operands;

typedef set <string, less<string> > S;

string objtype;

FWObjectDatabase       *objdb = NULL;

int main(int argc, char * const *argv)
{   
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



    try 
    {
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
            
        cout << "Start test for Address Tables :" << endl;


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
            o->setFileName("addresstable.txt");

        }
        
        try
        {
            o->loadFile();
            list<FWObject*>::const_iterator t=o->begin();
            //FWObject::tree_iterator t=o->tree_begin();
            Network *net;
            FWReference *ref;
            
            for ( ; t!=o->end(); ++t ) 
            {
                ref=FWReference::cast(*t);
                if (ref==NULL)
                {
                    cerr << "NULL in reference" << endl;
                    exit( 1);
                }
                net=Network::cast(ref->getPointer());
                if (net==NULL) 
                {
                    cerr << "NULL in Network" << endl;
                    exit( 1);
                }
                addrres.insert(net->getAddress().toString() + "/" + net->getNetmask().toString());
            }

            if(addrset==addrres)
            {
                res=0;
                cout << "Test passed" << endl;
            }
            else
            {
                res=1;
                cout << "Test failed" << endl;
            }
            
        }catch(FWException &ex)  
        {
            cerr << ex.toString() << endl;
            cout << "Test failed." << endl;
        }

    } catch(FWException &ex)  {
    cerr << ex.toString() << endl;
        exit(1);
    } catch (std::string s) {
    cerr << s;
        exit(1);
    } catch (std::exception ex) {
    cerr << ex.what();
        exit(1);
    } catch (...) {
    cerr << "Unsupported exception";
        exit(1);
    }
    return res;
}


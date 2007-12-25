/* 

                          Firewall Builder

                 Copyright (C) 2005 NetCitadel, LLC

  Author:  Illiya Yalovoy     yalovoy@gmail.com

  $Id: dns_test.cpp 918 2006-03-05 06:07:10Z vkurland $

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
#include "fwbuilder/DNSName.h"
#include "fwbuilder/ObjectGroup.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace std;

typedef enum { NONE, TEST} command;
typedef deque<string> operands;
typedef multimap <string,string> MM;
typedef multimap <string,string>::iterator MMI;

MM ta;

string objtype;

FWObjectDatabase       *objdb = NULL;



bool testDNS(const string &s,const string &a)
{
    
    pair<MMI,MMI> b=ta.equal_range(s);
    for(MMI i=b.first; i!=b.second;++i)
    {
        if ((*i).second == a) return true;
    }
    return false;
}

int main(int argc, char * const *argv)
{   
    operands ops;

#ifdef ENABLE_NLS
    setlocale (LC_ALL, "");

    bindtextdomain (PACKAGE, LOCALEDIR);
    textdomain (PACKAGE);
#else
#  ifdef HAVE_SETLOCALE
    setlocale (LC_ALL, "");
#  endif
#endif


    int passed = 0;
    int failed = 0;

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
        root->setName("DNS Names");
        o1->add(root);
            
        DNSName *dname;
        IPAddress addr;
            
        cout << "Start test for DNS Names :" << endl;


        ta.insert(make_pair("localhost","127.0.0.1"));
        
        ta.insert(make_pair("www.rebol.com","216.193.197.238"));
        
        ta.insert(make_pair("www.microsoft.com","207.46.20.60"));
        ta.insert(make_pair("www.microsoft.com","207.46.198.30"));
        ta.insert(make_pair("www.microsoft.com","207.46.198.60"));
        ta.insert(make_pair("www.microsoft.com","207.46.199.30"));
        ta.insert(make_pair("www.microsoft.com","207.46.225.60"));
        ta.insert(make_pair("www.microsoft.com","207.46.19.60"));
       
        ta.insert(make_pair("www.cnn.com","64.236.16.20"));
        ta.insert(make_pair("www.cnn.com","64.236.16.52"));
        ta.insert(make_pair("www.cnn.com","64.236.16.84"));
        ta.insert(make_pair("www.cnn.com","64.236.16.116"));
        ta.insert(make_pair("www.cnn.com","64.236.24.4"));
        ta.insert(make_pair("www.cnn.com","64.236.24.12"));
        ta.insert(make_pair("www.cnn.com","64.236.24.20"));
        ta.insert(make_pair("www.cnn.com","64.236.24.28"));

        ta.insert(make_pair("www.google.com","66.249.85.99"));
        ta.insert(make_pair("www.google.com","66.249.85.104"));
        ta.insert(make_pair("www.google.com","66.249.85.147"));
        ta.insert(make_pair("www.google.com","66.249.93.99"));
        ta.insert(make_pair("www.google.com","66.249.93.104"));
        ta.insert(make_pair("www.google.com","66.249.93.147"));
        ta.insert(make_pair("www.google.com","66.102.7.99"));
        ta.insert(make_pair("www.google.com","66.102.7.104"));
        ta.insert(make_pair("www.google.com","66.102.7.147"));

        
        DNSName *o;
        FWObject* nobj;
        objtype=DNSName::TYPENAME;
        
        for (MM::iterator i = ta.begin();
                i != ta.end();
                i=ta.upper_bound((*i).first))
        {

            nobj=objdb->create(objtype,true);
            if (root != NULL)
            {
                root->add(nobj);    
            
                o=DNSName::cast(nobj);
            
                o->setName((*i).first);
                o->setDNSRec((*i).first);
                o->setRunTime(false);

            }
            
        }

        
        for(FWObject::iterator i=root->begin(); 
                     i!=root->end();
                     ++i)
        {
            try
            {
               cout << endl << (*i)->getName() << " ( " << (*i)->getId() <<  " ) " << endl;
               dname=DNSName::cast(*i);
               dname->resolve();
               addr=dname->getAddress();
               
               cout << addr.toString() << " :"; 

               if ( testDNS((*i)->getName(), addr))
               {
                   cout << "Passed." << endl;
                   passed++;
                   
               }
               else
               {
                   cout << "Failed." << endl;
                   failed++;
               }
            }catch(FWException &ex)  
            {
                cerr << ex.toString() << endl;
                failed++;
            }
 
           
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

    cout << endl <<"Test statistics:" << endl;
    cout   <<"Passed : "<< passed << endl;
    cout   <<"Failed : "<< failed << endl;

    return((failed>0)?0:1);
}


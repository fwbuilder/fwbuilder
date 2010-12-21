/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org
   
  $Id: removeObjectTest.cpp 918 2006-03-05 06:07:10Z vkurland $


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

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/ObjectGroup.h"

#include <fstream>
#include <iostream>

using namespace libfwbuilder;
using namespace std;
    

string generateHostName(int n)
{
    char s[32];
    sprintf(s,"Host_%d",n);
    return string(s);
}


/*
 * adds host with the name "Host_N" and ID = "ID_host_N"
 */
FWObject* createHost( FWObjectDatabase *db , int N)
{
    Host *h=Host::cast( db->create(Host::TYPENAME, true) );
    h->setName( generateHostName(N) );
    return h;
}

void deleteObject( FWObjectDatabase *db,FWObject *obj)
{
    db->removeAllInstances(obj);
}

void removeObjectTest(FWObjectDatabase *objdb)
{   
    cout << endl;
    cout << "*** Testing object search and removal" << endl;

    string id50;
    int    N=2000;

    cout << " *** Create " << N << " Host objects  ..\n";	
    for (int i=0; i<N; ++i) {
        FWObject *h=createHost( objdb , i );

        objdb->add(h);

        if (i==50) id50=h->getId();
    }
    cout << "done\n";

    cout << " *** Find host #50 ( id=" << id50 << " )..." << endl;
    FWObject *obj=objdb->getById( id50 , true );
    cout << "done. Obj=" << obj << endl;

	
    /* delete host number 50 */
    cout << " *** Delete host #50 ..." << endl;
    deleteObject( objdb , obj );
    cout << "done\n";
}


/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "fwbuilder/Constants.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Constants.h"

#include "fwbedit.h"
#include "upgradePredicate.h"

#include <iostream>

using namespace libfwbuilder;
using namespace std;


class MergeConflictRes :
    public libfwbuilder::FWObjectDatabase::ConflictResolutionPredicate
{
    int conflict_res;
    public:
    MergeConflictRes(int cr) { conflict_res = cr; }
    virtual bool askUser(libfwbuilder::FWObject*, libfwbuilder::FWObject*)
    { 
        return (conflict_res == 1);
    }
};


void mergeTree(FWObjectDatabase *objdb, const string &mergefile, int conflict_res)
{
    cout << "Merge objects from file " << mergefile << endl;

    UpgradePredicate upgrade_predicate(false); 

    try
    {
        FWObjectDatabase *ndb = new FWObjectDatabase();
        ndb->load(mergefile,  &upgrade_predicate,  Constants::getDTDDirectory());

        FWObject *dobj = ndb->findInIndex(FWObjectDatabase::DELETED_OBJECTS_ID);
        if (dobj) ndb->remove(dobj, false);

        MergeConflictRes mcr(conflict_res);
        objdb->merge(ndb, &mcr);
        delete ndb;

    } catch(FWException &ex)
    {
        cerr << ex.toString() << endl;
    }
}


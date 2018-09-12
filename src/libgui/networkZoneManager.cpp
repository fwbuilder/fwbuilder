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

#include "global.h"

#include "networkZoneManager.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/ObjectGroup.h"

#include <QtDebug>

using namespace libfwbuilder;
using namespace std;


NetworkZoneManager::NetworkZoneManager()
{
}

void NetworkZoneManager::load(FWObjectDatabase *_db)
{
    db = _db;

    netzones_object_names.clear();
    netzones_id_to_index.clear();
    netzones_index_to_id.clear();

    int n = 0;

    netzones_id_to_index[0] = n;
    netzones_index_to_id[n] = 0;
    netzones_object_names.push_back(" None ");

    ++n;

    netzones_id_to_index[FWObjectDatabase::ANY_ADDRESS_ID] = n;
    netzones_index_to_id[n] = FWObjectDatabase::ANY_ADDRESS_ID;
    netzones_object_names.push_back(" Any ");

    ++n;

/* TODO: try to make this widget show object with appropriate icon */

    list<FWObject*> libs = db->getByType( Library::TYPENAME );
    for (list<FWObject*>::iterator l=libs.begin(); l!=libs.end(); ++l)
    {
        FWObject *library= *l;
        FWObject *o1,*o2;

        if ( library->getId()==FWObjectDatabase::DELETED_OBJECTS_ID ) continue;

        o1 = library->findObjectByName(ObjectGroup::TYPENAME, "Objects");
        assert(o1!=nullptr);

        o2 = o1->findObjectByName(ObjectGroup::TYPENAME, "Groups");
        if (o2==nullptr)
        {
            if (fwbdebug)
                qDebug() << "NetworkZoneManager::NetworkZoneManager():"
                         << "missing Groups group in "
                         << FWObjectDatabase::getStringId(o1->getId()).c_str();
            continue;
        }

        for (FWObject::iterator i=o2->begin(); i!=o2->end(); ++i)
        {
            netzones_id_to_index[(*i)->getId()] = n;
            netzones_index_to_id[n] = (*i)->getId();
            netzones_object_names.push_back(
                QObject::tr("Group: ")+ (*i)->getName().c_str() );
            ++n;
        }

        o2=o1->findObjectByName(ObjectGroup::TYPENAME,"Networks");
        if (o2==nullptr)
        {
            if (fwbdebug)
                qDebug() << "NetworkZoneManager::NetworkZoneManager():"
                         << "missing Networks group in "
                         << FWObjectDatabase::getStringId(o1->getId()).c_str();
            continue;
        }

        for (FWObject::iterator i1=o2->begin(); i1!=o2->end(); ++i1)
        {
            netzones_id_to_index[(*i1)->getId()] = n;
            netzones_index_to_id[n] = (*i1)->getId();
            netzones_object_names.push_back(
                QObject::tr("Network: ")+ (*i1)->getName().c_str());
            ++n;
        }
    }
}

int NetworkZoneManager::getListItemIdexByNetzoneId(int id)
{
    return netzones_id_to_index[id];
}

int NetworkZoneManager::getNetzoneIdByListIndex(int idx)
{
    return netzones_index_to_id[idx];
}

void NetworkZoneManager::packComboBox(QComboBox *combobox,
                                      int current_netzone_object_id)
{
    combobox->clear();
    combobox->addItems(netzones_object_names);

    for(int idx=0; idx < combobox->count(); ++idx)
        combobox->setItemData(idx, QVariant(netzones_index_to_id[idx]), Qt::UserRole);

    combobox->setCurrentIndex( netzones_id_to_index[current_netzone_object_id] );

}

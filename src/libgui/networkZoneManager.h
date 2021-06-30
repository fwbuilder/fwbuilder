/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#ifndef __NETWORK_ZONE_MANAGER_H_
#define __NETWORK_ZONE_MANAGER_H_


#include <QStringList>
#include <QMap>
#include <QComboBox>


namespace libfwbuilder
{
    class FWObjectDatabase;
}

class NetworkZoneManager
{
    libfwbuilder::FWObjectDatabase *db;
    QStringList netzones_object_names;
    // netzones_id_to_index : key - object id, value - number in the list
    QMap<int,int> netzones_id_to_index;
    // netzones_index_to_id : key - number in the list, value - obj id
    QMap<int,int> netzones_index_to_id;

public:

    NetworkZoneManager();

    void load(libfwbuilder::FWObjectDatabase *db);
    
    int getListItemIdexByNetzoneId(int id);
    int getNetzoneIdByListIndex(int idx);
    
    void packComboBox(QComboBox *combobox,
                      int current_netzone_object_id);
};

#endif

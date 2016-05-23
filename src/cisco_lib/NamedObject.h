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

#ifndef _ASA8_OBJECT_HH
#define _ASA8_OBJECT_HH

#include "fwbuilder/FWObject.h"

#include <QString>
#include <QSet>


namespace fwcompiler
{

    class NamedObject
    {
        const libfwbuilder::FWObject *obj;
        QString platform;
        QString name;
        static QSet<QString> reserved_words;
        
        QString printPorts(int port_range_start, int port_range_end);
        
protected:
        QString sanitizeObjectName(const QString &name);
        QString createNetworkObjectCommand(const libfwbuilder::Address *addr);
        QString createServiceObjectCommand(const libfwbuilder::Service *addr);

public:

        static std::map<QString,int> name_disambiguation;

        NamedObject(const libfwbuilder::FWObject *obj, const QString &platform);
        virtual ~NamedObject() {}
        virtual QString getCommand();
        virtual QString getCommandWhenObjectGroupMember();
        QString getName() { return name; }
        QString getCommandWord();
        const libfwbuilder::FWObject* getObject() { return obj; }
    };
}

#endif

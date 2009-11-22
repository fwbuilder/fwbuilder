/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

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

#include "FirewallInterfacesList.h"

using namespace libfwbuilder;

FirewallInterfacesList::FirewallInterfacesList(QWidget *parent):
     QTreeWidget(parent)
{
}

void FirewallInterfacesList::addInterface(Interface* interface)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(this, QStringList()
                                                << interface->getName().c_str()
                                                << interface->getParent()->getName().c_str());
    interfaces[interface] = item;
}

void FirewallInterfacesList::removeInterface(Interface* interface)
{/*
    QTreeWidgetItem *item = interfaces[interface];
    this->removeItemWidget(item, 0);
    this->removeItemWidget(item, 1);
    delete interfaces[interface];
    interfaces.remove(interface);*/
}

Interface* FirewallInterfacesList::getInterfaceForItem(QTreeWidgetItem* item)
{
    return this->interfaces.key(item);
}

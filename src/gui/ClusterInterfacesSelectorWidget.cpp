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

#include "ClusterInterfacesSelectorWidget.h"

using namespace libfwbuilder;
using namespace std;

ClusterInterfacesSelectorWidget::ClusterInterfacesSelectorWidget(QWidget *parent):
        QTabWidget(parent)
{
}

void ClusterInterfacesSelectorWidget::setFirewallList(QList<Firewall*> firewalls)
{
    while ( this->count() ) this->removeTab(0);
    fwlist = firewalls;
    set<string> interfaces;
    foreach ( Firewall* fw, firewalls )
    {
        FWObjectTypedChildIterator iter = fw->findByType(Interface::TYPENAME);
        for ( ; iter != iter.end(); ++iter )
        {
            Interface *interface = Interface::cast(*iter);
            interfaces.insert(interface->getName());
        }
    }
    set<string> usedInterfaces;
    foreach ( string name, interfaces )
    {
        int used = 0;
        foreach ( Firewall* fw, firewalls )
        {
            FWObjectTypedChildIterator iter = fw->findByType(Interface::TYPENAME);
            for ( ; iter != iter.end(); ++iter )
            {
                Interface *interface = Interface::cast(*iter);
                if (interface->getName() == name )
                {
                    used++;
                    break;
                }
            }
        }
        if ( used == firewalls.count() )
            usedInterfaces.insert(name);
    }
    foreach(string name, usedInterfaces)
        this->addInterface(QString::fromUtf8(name.c_str()));
}

ClusterInterfaceWidget* ClusterInterfacesSelectorWidget::addNewInterface()
{
    ClusterInterfaceWidget* widget = new ClusterInterfaceWidget(this);
    widget->setFirewallList(this->fwlist);
    this->editors.append(widget);
    this->addTab(widget, tr("New interface"));
    return widget;
}

void ClusterInterfacesSelectorWidget::addInterface(QString name)
{
    ClusterInterfaceWidget* widget = addNewInterface();
    widget->setCurrentInterface(name);
}

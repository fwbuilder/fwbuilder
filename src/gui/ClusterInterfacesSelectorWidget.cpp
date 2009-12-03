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

#include <QDebug>

using namespace libfwbuilder;
using namespace std;

ClusterInterfacesSelectorWidget::ClusterInterfacesSelectorWidget(QWidget *parent):
        QTabWidget(parent)
{
    connect(&newInterface, SIGNAL(clicked()), this, SLOT(addNewInterface()));
    connect(&delInterface, SIGNAL(clicked()), this, SLOT(closeTab()));
    newInterface.setIcon(QIcon(":/Icons/add.png"));
    delInterface.setIcon(QIcon(":/Icons/del.png"));
    newInterface.setToolTip(tr("Add new interface"));
    delInterface.setToolTip(tr("Delete current interface"));
    this->setCornerWidget(&delInterface, Qt::TopRightCorner);
    this->setCornerWidget(&newInterface, Qt::TopLeftCorner);
    this->cornerWidget(Qt::TopRightCorner)->show();
    this->cornerWidget(Qt::TopLeftCorner)->show();
    noTabs = false;
}

ClusterInterfacesSelectorWidget::~ClusterInterfacesSelectorWidget()
{
    this->clear();
}

void ClusterInterfacesSelectorWidget::setFirewallList(QList<Firewall*> firewalls)
{
    while ( this->count() ) this->removeTab(0);
    fwlist = firewalls;
    set<string> interfaces;
    foreach ( Firewall* fw, firewalls )
    {
        foreach( FWObject *obj, fw->getByTypeDeep(Interface::TYPENAME) )
        {
            Interface *iface = Interface::cast(obj);
            interfaces.insert(iface->getName());
        }
    }
    set<string> usedInterfaces;
    foreach ( string name, interfaces )
    {
        int used = 0;
        foreach ( Firewall* fw, firewalls )
        {
            foreach( FWObject *obj, fw->getByTypeDeep(Interface::TYPENAME) )
            {
                Interface *iface = Interface::cast(obj);
                if (iface->getName() == name )
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
    if (noTabs)
    {
        this->removeTab(0);
        noTabs = false;
    }
    ClusterInterfaceWidget* widget = new ClusterInterfaceWidget(this);
    widget->setFirewallList(this->fwlist);
    this->editors.append(widget);
    this->addTab(widget, tr("New interface"));
    return widget;
}

void ClusterInterfacesSelectorWidget::addInterface(const QString& name)
{
    ClusterInterfaceWidget* widget = addNewInterface();
    if (!widget->setCurrentInterface(name))
    {
        this->removeTab(this->indexOf(widget));
        this->editors.removeAll(widget);
        delete widget;
    }
}

void ClusterInterfacesSelectorWidget::closeTab()
{
    this->editors.removeAll(dynamic_cast<ClusterInterfaceWidget*>(this->widget(this->currentIndex())));
    this->removeTab(this->currentIndex());
    if (this->count()==0)
    {
        noTabs = true;
        QLabel *label = new QLabel(tr("This cluster has no interfaces. Add interface using button <img src=\":/Icons/add.png\" width=15 height=15>."), this);
        this->addTab(label, tr("No interfaces"));
    }
}

QList<ClusterInterfaceData> ClusterInterfacesSelectorWidget::getInterfaces()
{
    QList<ClusterInterfaceData> res;
    foreach ( ClusterInterfaceWidget* editor, this->editors )
    {
        if (this->indexOf(editor) != -1)
            res.append(editor->getInterfaceData());
    }
    return res;
}

void ClusterInterfacesSelectorWidget::clear()
{
    QTabWidget::clear();
    foreach(ClusterInterfaceWidget *editor, editors)
        delete editor;
    editors.clear();
    fwlist.clear();
}

bool ClusterInterfacesSelectorWidget::isValid()
{
    foreach (ClusterInterfaceWidget *editor, this->editors)
    {
        if (!editor->isValid()) return false;
    }
    return true;
}

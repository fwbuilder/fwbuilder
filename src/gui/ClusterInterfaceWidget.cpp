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

#include "ClusterInterfaceWidget.h"
#include "ui_ClusterInterfaceWidget.h"

using namespace libfwbuilder;

ClusterInterfaceWidget::ClusterInterfaceWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ClusterInterfaceWidget)
{
    cisw = dynamic_cast<ClusterInterfacesSelectorWidget*>(parent);
    m_ui->setupUi(this);
}

ClusterInterfaceWidget::~ClusterInterfaceWidget()
{
    delete m_ui;
}

void ClusterInterfaceWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ClusterInterfaceWidget::setFirewallList(QList<Firewall*> firewalls)
{
    foreach ( Firewall* fw, firewalls )
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        this->m_ui->interfaces->addLayout(layout);
        QLabel *label = new QLabel(fw->getName().c_str(), this);
        layout->addWidget(label);
        QListWidget *list = new QListWidget(this);
        layout->addWidget(list);
        FWObjectTypedChildIterator iter = fw->findByType(Interface::TYPENAME);
        for ( ; iter != iter.end() ; ++iter )
        {
            Interface *interface = Interface::cast(*iter);
            list->addItem(QString::fromUtf8(interface->getName().c_str()));
        }
        InterfacesList newlist;
        newlist.label = label;
        newlist.layout = layout;
        newlist.list = list;
        newlist.firewall = fw;
        lists[fw] = newlist;
    }
}

void ClusterInterfaceWidget::setCurrentInterface(QString name)
{
    foreach(InterfacesList list, this->lists.values())
    {
        list.list->setCurrentItem(list.list->findItems(name, Qt::MatchCaseSensitive | Qt::MatchExactly).first());
    }
    this->m_ui->name->setText(name);
}

void ClusterInterfaceWidget::nameChanged(QString newname)
{
    cisw->setTabText(cisw->indexOf(this), newname);
}

ClusterInterfaceData ClusterInterfaceWidget::getInterfaceData()
{
    ClusterInterfaceData res;
    res.name = this->m_ui->name->text();
    res.label = this->m_ui->label->text();
    res.comment = this->m_ui->comment->toPlainText();
    foreach(InterfacesList ifacelist, this->lists.values())
    {
        QString selectedInterface = ifacelist.list->currentItem()->text();
        FWObjectTypedChildIterator iter = ifacelist.firewall->findByType(Interface::TYPENAME);
        for ( ; iter!=iter.end(); ++iter )
        {
            Interface* iface = Interface::cast(*iter);
            if (QString::fromUtf8(iface->getName().c_str()) == selectedInterface)
            {
                res.interfaces.append(qMakePair(ifacelist.firewall, iface));
                break;
            }
        }
    }
    return res;
}


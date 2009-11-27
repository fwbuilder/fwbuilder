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

#include <QDebug>

using namespace libfwbuilder;
using namespace std;

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
        os = fw->getStr("host_OS").c_str();
        QVBoxLayout *layout = new QVBoxLayout();
        this->m_ui->interfaces->addLayout(layout);
        QLabel *label = new QLabel(QString::fromUtf8(fw->getName().c_str()), this);
        layout->addWidget(label);
        QTreeWidget *list = new QTreeWidget(this);
        layout->addWidget(list);
        QTreeWidgetItem* firewall = new QTreeWidgetItem(list, QStringList() << QString::fromUtf8(fw->getName().c_str()));
        roots[list] = firewall;
        firewall->setIcon(0, QIcon(":/Icons/Firewall/icon-tree"));
        FWObjectTypedChildIterator iter = fw->findByType(Interface::TYPENAME);
        for ( ; iter != iter.end() ; ++iter )
        {
            Interface *iface = Interface::cast(*iter);
            if (iface->isLoopback()) continue;
            QTreeWidgetItem *ifaceitem = new QTreeWidgetItem(firewall, QStringList() << QString::fromUtf8(iface->getName().c_str()));
            ifaceitem->setIcon(0, QIcon(":/Icons/Interface/icon-tree"));
            ifaceitem->setDisabled(false);
            if (!interfaceSelectable(iface))
                ifaceitem->setFlags(Qt::NoItemFlags);
            else
                ifaceitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            FWObjectTypedChildIterator iter2 = iface->findByType(Interface::TYPENAME);
            for ( ; iter2 != iter2.end() ; ++iter2 )
            {
                if (iface->isLoopback()) return;
                Interface *subiface = Interface::cast(*iter2);
                QTreeWidgetItem *subitem = new QTreeWidgetItem(ifaceitem, QStringList() << QString::fromUtf8(subiface->getName().c_str()));
                subitem->setDisabled(false);
                subitem->setIcon(0, QIcon(":/Icons/Interface/icon-tree"));
                if (!interfaceSelectable(subiface))
                    subitem->setFlags(Qt::NoItemFlags);
                else
                    ifaceitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
        }
        list->topLevelItem(0)->setFlags(list->topLevelItem(0)->flags() & ~Qt::ItemIsSelectable);
        list->expandAll();
        InterfacesList newlist;
        newlist.label = label;
        newlist.layout = layout;
        newlist.list = list;
        newlist.firewall = fw;
        lists[fw] = newlist;
    }
}

bool ClusterInterfaceWidget::setCurrentInterface(QString name)
{
    foreach(InterfacesList list, this->lists.values())
    {
        bool gotItem = false;
        foreach(QTreeWidgetItem *item, list.list->findItems(name, Qt::MatchCaseSensitive | Qt::MatchExactly | Qt::MatchRecursive))
        {
            if ( item == roots[list.list] ) continue;
            if (item->flags() && Qt::ItemIsSelectable)
            {
                list.list->setCurrentItem(item);
                gotItem = true;
                break;
            }
        }
        if (!gotItem) return false;
    }
    this->m_ui->name->setText(name);
    return true;
}

void ClusterInterfaceWidget::nameChanged(QString newname)
{
    cisw->setTabText(cisw->indexOf(this), newname);
}

ClusterInterfaceData ClusterInterfaceWidget::getInterfaceData()
{
    ClusterInterfaceData res;
    res.os = os;
    res.name = this->m_ui->name->text();
    res.label = this->m_ui->label->text();
    res.comment = this->m_ui->comment->toPlainText();
    foreach(InterfacesList ifacelist, this->lists.values())
    {
        QString selectedInterface = ifacelist.list->selectedItems().first()->text(0);
        qDebug() << selectedInterface;
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

bool ClusterInterfaceWidget::interfaceSelectable(libfwbuilder::Interface* iface)
{
    libfwbuilder::Cluster cluster;
//    cluster.add(iface, false);
    cluster.setStr("host_os", os.toStdString());

    Resources* os_res = Resources::os_res[os.toStdString()];
    string os_family = os.toStdString();
    if (os_res!=NULL)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    auto_ptr<interfaceProperties> int_prop(
        interfacePropertiesObjectFactory::getInterfacePropertiesObject(
            os_family));
    QString err;
    return int_prop->validateInterface(dynamic_cast<FWObject*>(&cluster), dynamic_cast<FWObject*>(iface), false, err);
}

bool ClusterInterfaceWidget::isValid()
{
    foreach(InterfacesList ifacelist, this->lists.values())
    {
        QList<QTreeWidgetItem*> items = ifacelist.list->selectedItems();
        if (items.isEmpty())
        {
            QMessageBox::warning(this,"Firewall Builder",
                     tr("Some of the cluster interfaces do not have any "
                        "member firewall interface selected"),
                    "&Continue", QString::null, QString::null, 0, 1 );
            return false;
        }
        if (roots.values().contains(items.first()))
        {
            QMessageBox::warning(this,"Firewall Builder",

                     tr("It is not possible to use firewall as interface "
                        "for cluster"),

                    "&Continue", QString::null, QString::null, 0, 1 );
            return false;
        }
    }
    return true;
}


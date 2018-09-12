/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

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
#include "FWBSettings.h"
#include "ui_ClusterInterfaceWidget.h"

#include <QSpacerItem>
#include <QDebug>


using namespace libfwbuilder;
using namespace std;

ClusterInterfaceWidget::ClusterInterfaceWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ClusterInterfaceWidget)
{
    cisw = dynamic_cast<ClusterInterfacesSelectorWidget*>(parent);
    m_ui->setupUi(this);
    interfaceBox = new QHBoxLayout();
    this->layout()->addItem(interfaceBox);
    this->layout()->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum));
}

ClusterInterfaceWidget::~ClusterInterfaceWidget()
{
    delete m_ui;
    foreach(InterfacesList list, this->lists)
    {
        delete list.list;
        delete list.label;
        delete list.layout;
    }
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
        //qFindChild<QHBoxLayout*>(this, "interfaceBox")
                interfaceBox->addLayout(layout);
        //this->m_ui->interfaceBox->addLayout(layout);

        //create label with firewall name
        QLabel *label = new QLabel(QString::fromUtf8(fw->getName().c_str()), this);
        layout->addWidget(label);

        // create object tree
        QTreeWidget *list = new QTreeWidget(this);
        list->header()->setVisible(false);
        layout->addWidget(list);

        // create firewall item in tree
        QTreeWidgetItem* firewall = new QTreeWidgetItem(list, QStringList() << QString::fromUtf8(fw->getName().c_str()));
        roots[list] = firewall;
        firewall->setIcon(0, QIcon(":/Icons/Firewall/icon-tree"));

        // interfaces and subinterfaces creation
        FWObjectTypedChildIterator iter = fw->findByType(Interface::TYPENAME);
        for ( ; iter != iter.end() ; ++iter )
        {
            Interface *iface = Interface::cast(*iter);
            //if (iface->isLoopback()) continue;
            QTreeWidgetItem *ifaceitem = new QTreeWidgetItem(firewall, QStringList() << QString::fromUtf8(iface->getName().c_str()));
            ifaceitem->setData(0, Qt::UserRole, qVariantFromValue(iface));//QVariant(QVariant::UserType, iface));
            ifaceitem->setIcon(0, QIcon(":/Icons/Interface/icon-tree"));
            ifaceitem->setDisabled(!interfaceSelectable(iface));
            if (!interfaceSelectable(iface))
                // works only for elements which does not have child elements
                ifaceitem->setFlags(ifaceitem->flags() & ~Qt::ItemIsSelectable);
            else
                ifaceitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            FWObjectTypedChildIterator iter2 = iface->findByType(Interface::TYPENAME);
            for ( ; iter2 != iter2.end() ; ++iter2 )
            {
                //if (iface->isLoopback()) return;
                Interface *subiface = Interface::cast(*iter2);
                QTreeWidgetItem *subitem = new QTreeWidgetItem(ifaceitem, QStringList() << QString::fromUtf8(subiface->getName().c_str()));
                subitem->setData(0, Qt::UserRole, qVariantFromValue(subiface));//QVariant(QVariant::UserType, subitem));
                subitem->setDisabled(!interfaceSelectable(subiface));
                subitem->setIcon(0, QIcon(":/Icons/Interface/icon-tree"));
                if (!interfaceSelectable(subiface))
                    subitem->setFlags(subitem->flags() & ~Qt::ItemIsSelectable);
                else
                    ifaceitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
        }
        list->topLevelItem(0)->setFlags(list->topLevelItem(0)->flags() & ~Qt::ItemIsSelectable);
        list->expandAll();

        // add data to map
        InterfacesList newlist;
        newlist.label = label;
        newlist.layout = layout;
        newlist.list = list;
        newlist.firewall = fw;
        lists[fw] = newlist;
    }
}

bool ClusterInterfaceWidget::setCurrentInterface(const QString& name)
{
    string label;
    bool setLabel = true;
    int gotItems = 0;
    foreach(InterfacesList list, this->lists.values())
    {
        foreach(QTreeWidgetItem *item, list.list->findItems(name, Qt::MatchCaseSensitive | Qt::MatchExactly | Qt::MatchRecursive))
        {
            Interface *iface = item->data(0, Qt::UserRole).value<Interface*>();
            if (iface == nullptr) continue;
            if ( item == roots[list.list] ) continue; // skip firewall object
            if ( interfaceSelectable(iface) ) // interface is good for use in cluster
            {
                list.list->setCurrentItem(item);
                gotItems++;
                if (label.length() == 0) label = iface->getLabel();
                else
                    if (label != iface->getLabel())
                        setLabel = false;
                break;
            }
        }
    }
    if (gotItems < this->lists.values().count()) return false;
    this->m_ui->name->setText(name);
    if (setLabel) this->m_ui->label->setText(QString::fromUtf8(label.c_str()));
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
        QTreeWidgetItem *item = ifacelist.list->selectedItems().first();
        Interface* iface = item->data(0, Qt::UserRole).value<Interface*>();
        FWObject *parent_fw = Host::getParentHost(iface);
        res.interfaces.append(qMakePair(Firewall::cast(parent_fw), iface));
    }
    return res;
}

bool ClusterInterfaceWidget::interfaceSelectable(Interface* iface)
{
    libfwbuilder::Cluster cluster;
//    cluster.add(iface, false);
    cluster.setStr("host_OS", os.toStdString());

    Resources* os_res = Resources::os_res[os.toStdString()];
    string os_family = os.toStdString();
    if (os_res!=nullptr)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    unique_ptr<interfaceProperties> int_prop(
        interfacePropertiesObjectFactory::getInterfacePropertiesObject(
            os_family));
    QString err;
    bool res = true;
    if (st->getBool("Objects/Interface/autoconfigureInterfaces"))
        res = int_prop->validateInterface(&cluster, iface, false, err);
    if (res)
        res = int_prop->isEligibleForCluster(iface);

    if (fwbdebug)
        qDebug() << "interface"
                 << iface->getName().c_str()
                 << "can be used in cluster:" << res;
    return res;
}

bool ClusterInterfaceWidget::isValid()
{
    // this->lists is QMap<Firewall*, InterfacesList> where
    // InterfacesList contains information about object trees and
    // labels in this widget. Map this->lists is filled in
    // setFirewallList() from the list of firewalls (new InterfacesList is
    // created for each firewall)
    foreach(InterfacesList ifacelist, this->lists.values())
    {
        // get selected items of object tree - QTreeWidget pointed by
        // ifacelist.list
        QList<QTreeWidgetItem*> items = ifacelist.list->selectedItems();
        if (items.isEmpty()) // nothing is selected in one of interface lists
        {
            QMessageBox::warning(this,"Firewall Builder",
                     tr("Some of the cluster interfaces do not have any "
                        "member firewall interface selected"),
                    "&Continue", QString::null, QString::null, 0, 1 );
            return false;
        }
        if (roots.values().contains(items.first()))
        {
            // firewall is the item selected in one of interface lists
            QMessageBox::warning(this,"Firewall Builder",
                     tr("Please select interface of the member firewall "
                        "rather than the firewall object to be used "
                        "with cluster interface"),
                    "&Continue", QString::null, QString::null, 0, 1 );
            return false;
        }
        if (!interfaceSelectable(ifacelist.list->selectedItems().first()->data(0, Qt::UserRole).value<Interface*>()))
        {
            // selected interface item can not be used in this cluster
            QMessageBox::warning(this,"Firewall Builder",
                     tr("%1 can not be used as cluster interface.")
                        .arg(ifacelist.list->selectedItems().first()->text(0)),
                    "&Continue", QString::null, QString::null, 0, 1 );
            return false;
        }
    }
    // at least one item is selected in all object trees and it is not firewall object
    return true;
}


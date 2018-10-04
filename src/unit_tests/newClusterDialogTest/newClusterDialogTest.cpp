/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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

#include "newClusterDialogTest.h"

#include <QDebug>
#include <QDialog>

#include "ui_newclusterdialog_q.h"

#include "FWWindow.h"
#include "ObjectTreeView.h"
#include "ProjectPanel.h"
#include "StartTipDialog.h"
#include "ObjectTreeView.h"
#include "ObjectTreeViewItem.h"
#include "FWObjectClipboard.h"
#include "FWBApplication.h"

#include "fwbuilder/Policy.h"
#include "fwbuilder/Constants.h"

using namespace libfwbuilder;
using namespace std;

class newClusterDialog_ext: public newClusterDialog
{
public:
    newClusterDialog_ext(QWidget *parent, libfwbuilder::FWObject* obj): newClusterDialog(parent, obj)
    {

    }

    Ui::newClusterDialog_q* getUi()
    {
        return this->m_dialog;
    }

    QStringList getFirewallList()
    {
        QStringList res;
        for (int i=0; i<this->m_dialog->firewallSelector->rowCount(); i++)
        {
            res.append(this->m_dialog->firewallSelector->item(i, 0)->text());
        }
        return res;
    }

};

void newClusterDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();

    QTest::qWait(5000);

    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
}

void newClusterDialogTest::test1()
{
    //init();
    FWObjectDatabase *db = new FWObjectDatabase();
    XMLTools::UpgradePredicate p;
    db->load("test_work.fwb", &p, Constants::getDTDDirectory());
    newClusterDialog_ext *dialog = new newClusterDialog_ext(nullptr, db);
    vector<FWObject*> fws;
    QStringList fwnames;
    foreach(FWObject *obj, db->getByTypeDeep(Firewall::TYPENAME))
    {
        fws.push_back(obj);
        fwnames.append(obj->getName().c_str());
    }

    dialog->setFirewallList(fws);
    dialog->setModal(false);
        dialog->show();

    QStringList dfws = dialog->getFirewallList();
    QVERIFY(dfws.count() == int(fws.size()));
    foreach( QString fwname, dfws)
        QVERIFY(fwnames.contains(fwname));

    QVERIFY(!dialog->getUi()->nextButton->isEnabled());
    QTest::keyClicks(dialog->getUi()->obj_name, "New Cluster");
    QVERIFY(dialog->getUi()->nextButton->isEnabled());

    for (int i=0; i< dialog->getUi()->firewallSelector->rowCount(); i++)
        dynamic_cast<QCheckBox*>(dialog->getUi()->firewallSelector->cellWidget(i, 1))->setChecked(true);

    dialog->getUi()->nextButton->click();
    QVERIFY(dialog->currentPage()==1);

    QList<ClusterInterfaceData> ifaces = dialog->getUi()->interfaceSelector->getInterfaces();
    foreach (ClusterInterfaceData iface, ifaces)
    {
        QVERIFY(iface.interfaces.count() == 2);
        for(int i=0; i < iface.interfaces.count(); i++)
            QVERIFY(iface.interfaces.at(i).second->getName().c_str() == iface.name);
    }


    dialog->getUi()->nextButton->click();
    QVERIFY(dialog->currentPage()==2);

    for (int i=1; i<dialog->getUi()->interfaceEditor->count(); i++)
        dynamic_cast<InterfaceEditorWidget*>(dialog->getUi()->interfaceEditor->widget(i))->setProtocolIndex(2);

    InterfaceEditorWidget* eth0 = dialog->getUi()->interfaceEditor->findChild<InterfaceEditorWidget*>("eth0_widget");
    eth0->setProtocolIndex(0);
    eth0->addNewAddress("123.45.67.89", "24", true);

    QList<EditedInterfaceData> addresses = dialog->getUi()->interfaceEditor->getNewData();
    foreach( EditedInterfaceData iface, addresses)
    {
        if (iface.name == "eth0")
        {
            qDebug() << iface.addresses.values().count();
            QVERIFY(iface.addresses.values().count() == 1);
            QVERIFY(iface.addresses.values().first().address == "123.45.67.89");
            QVERIFY(iface.addresses.values().first().netmask == "24");
            break;
        }
    }

    dialog->getUi()->nextButton->click();
    QVERIFY(dialog->currentPage()==3);

    dialog->getUi()->backButton->click();
    QVERIFY(dialog->currentPage()==2);


    QList<EditedInterfaceData> addresses2 = dialog->getUi()->interfaceEditor->getNewData();
    foreach( EditedInterfaceData iface, addresses2)
    {
        if (iface.name == "eth0")
        {
            QVERIFY(iface.addresses.values().count() == 1);
            QVERIFY(iface.addresses.values().first().address == "123.45.67.89");
            QVERIFY(iface.addresses.values().first().netmask == "24");
        }
    }

    dialog->getUi()->interfaceEditor->setCurrentIndex(1);
    InterfaceEditorWidget* eth1 = dialog->getUi()->interfaceEditor->findChild<InterfaceEditorWidget*>("eth1_widget");
    eth1->setProtocolIndex(0);
    QTableWidget *addrs = eth1->findChild<QTableWidget*>("addresses");
    QVERIFY(addrs != nullptr);
    QPushButton *addaddr = eth1->findChild<QPushButton*>("addAddress");
    QVERIFY(addaddr != nullptr);
    addaddr->click();
    addrs->item(0,0)->setText("98.76.54.32");
    addrs->item(0,1)->setText("24");
    dialog->getUi()->nextButton->click();
    QVERIFY(dialog->currentPage()==3);


    dialog->getUi()->nextButton->click();
    QVERIFY(dialog->currentPage()==4);


    QVERIFY(dialog->getUi()->finishButton->isEnabled());
    dialog->getUi()->finishButton->click();

    Cluster *newc = dialog->getNewCluster();

    QVERIFY(newc != nullptr);
    QVERIFY(Cluster::isA(newc));

    dialog->findChild<QPushButton*>("cancelButton")->click();
    dialog->accept();
    dialog->close();
    dialog->deleteLater();
}

void newClusterDialogTest::test2()
{
    mw->loadFile("test_work.fwb", false);

    FWObjectDatabase *db = mw->db();
    Library *lib = nullptr;

    foreach(FWObject *obj, db->getByTypeDeep(Library::TYPENAME))
    {
        qDebug() << obj->getName().c_str();
        if (obj->getName() == "new_cluster_test") lib = Library::cast(obj);
    }
    QVERIFY(lib != nullptr);

    newClusterDialog_ext *dialog = new newClusterDialog_ext(nullptr, FWBTree().getStandardSlotForObject(lib, Cluster::TYPENAME));
    vector<FWObject*> fws;
    QStringList fwnames;
    foreach(FWObject *obj, db->getByTypeDeep(Firewall::TYPENAME))
    {
        fws.push_back(obj);
        fwnames.append(obj->getName().c_str());
    }

    dialog->setFirewallList(fws);
    dialog->setModal(false);
        dialog->show();

    QStringList dfws = dialog->getFirewallList();
    QVERIFY(dfws.count() == int(fws.size()));
    foreach( QString fwname, dfws)
        QVERIFY(fwnames.contains(fwname));

    QVERIFY(!dialog->getUi()->nextButton->isEnabled());
    QTest::keyClicks(dialog->getUi()->obj_name, "New Cluster");
    QVERIFY(dialog->getUi()->nextButton->isEnabled());

    for (int i=0; i< dialog->getUi()->firewallSelector->rowCount(); i++)
        dynamic_cast<QCheckBox*>(dialog->getUi()->firewallSelector->cellWidget(i, 1))->setChecked(true);

    dialog->getUi()->nextButton->click();
    QVERIFY(dialog->currentPage()==1);

    dialog->getUi()->nextButton->click();
    QVERIFY(dialog->currentPage()==2);

    dynamic_cast<InterfaceEditorWidget*>(dialog->getUi()->interfaceEditor->widget(0))->setProtocolIndex(3);
    dynamic_cast<InterfaceEditorWidget*>(dialog->getUi()->interfaceEditor->widget(1))->setProtocolIndex(3);
    dynamic_cast<InterfaceEditorWidget*>(dialog->getUi()->interfaceEditor->widget(2))->setProtocolIndex(3);
    dynamic_cast<InterfaceEditorWidget*>(dialog->getUi()->interfaceEditor->widget(3))->setProtocolIndex(3);

    dialog->getUi()->nextButton->click();
    QVERIFY(dialog->currentPage()==3);


    QList<QRadioButton*> btns= dialog->getUi()->page_4->findChildren<QRadioButton*>();
    foreach(QRadioButton *btn, btns)
    {
        if (btn->objectName() == "linux-1")
            QTest::mouseClick(btn, Qt::LeftButton);
    }

    dialog->getUi()->nextButton->click();
    QVERIFY(dialog->currentPage()==4);


    QVERIFY(dialog->getUi()->finishButton->isEnabled());
    dialog->getUi()->finishButton->click();

    Cluster *newc = dialog->getNewCluster();

    QVERIFY(newc != nullptr);
    QVERIFY(Cluster::isA(newc));

    Firewall *bak = Firewall::cast(mw->getCurrentLib()->findObjectByName(Firewall::TYPENAME, "linux-1-bak"));
    QVERIFY(bak != nullptr);
    QVERIFY(bak->getInactive() == true);

    Firewall *linux1 = Firewall::cast(mw->getCurrentLib()->findObjectByName(Firewall::TYPENAME, "linux-1"));
    QVERIFY(linux1 != nullptr);
    QVERIFY(linux1->getPolicy()->getChildrenCount() == 1); // there should be only RuleSetOptions object


    dialog->findChild<QPushButton*>("cancelButton")->click();
    dialog->accept();
    dialog->close();
    dialog->deleteLater();
}

QPoint findItemPos(ObjectTreeViewItem *item, ObjectTreeView *tree)
{
    for (int h=10; h<tree->height(); h+=1)
    {
        for (int w=75; w<tree->width(); w+=1)
        {
            if((tree->itemAt(w,h)) == item)
                return QPoint(w, h);
        }
    }
    return QPoint(-1,-1);
}

void newClusterDialogTest::closeContextMenu()
{
    foreach(QWidget *w, QApplication::allWidgets())
    {
        if (w->objectName() == "objectTreeContextMenu")
        {
            qDebug() << w;
            w->hide();
        }
    }
}

void newClusterDialogTest::openContextMenu(ObjectManipulator *om, ObjectTreeViewItem *item, ObjectTreeView *tree, const QString &actionText)
{
    QTimer::singleShot(1000, this, SLOT(closeContextMenu()));
    om->contextMenuRequested(findItemPos(item, tree));
    QMenu *menu;
    foreach(QWidget *w, QApplication::allWidgets())
    {
        if (w->objectName() == "objectTreeContextMenu")
        {
            menu = dynamic_cast<QMenu*>(w);
            break;
        }
    }
    foreach (QObject *act, menu->children())
    {
        QAction *action = dynamic_cast<QAction*>(act);
        if (action == nullptr) continue;
        if (action->text() == actionText)
        {
            QTimer::singleShot(100, this, SLOT(test3_part2()));
            action->trigger();
            break;
        }
    }
}

void newClusterDialogTest::test3()
{
    mw->loadFile("test_work.fwb", false);

    FWObjectDatabase *db = mw->db();
    Library *lib = nullptr;

    foreach(FWObject *obj, db->getByTypeDeep(Library::TYPENAME))
    {
        qDebug() << obj->getName().c_str();
        if (obj->getName() == "new_cluster_test") lib = Library::cast(obj);
    }
    QVERIFY(lib != nullptr);

    mw->show();

    ObjectManipulator *om = mw->activeProject()->findChild<ObjectManipulator*>("om");
    om->openLib(lib);
    QVERIFY ( om->getCurrentLib() == lib);

    ObjectTreeView *tree = mw->getCurrentObjectTree();

    ObjectTreeViewItem *linux1 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("linux-1", Qt::MatchContains | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *linux2 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("linux-2", Qt::MatchContains | Qt::MatchRecursive, 0).first());

    tree->selectionModel()->select(tree->indexAt(findItemPos(linux1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(linux1);
    tree->selectionModel()->select(tree->indexAt(findItemPos(linux2, tree)), QItemSelectionModel::Select);
    openContextMenu(om, linux2, tree, "New cluster from selected firewalls");
}

void newClusterDialogTest::test3_part2()
{
    QTest::qWait(100);
    newClusterDialog *dialog = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<newClusterDialog*>(w) != nullptr)
            dialog = dynamic_cast<newClusterDialog*>(w);
    QVERIFY(dialog != nullptr);

    QPushButton *nextButton = dialog->findChild<QPushButton*>("nextButton");
    QPushButton *finishButton = dialog->findChild<QPushButton*>("finishButton");
    InterfacesTabWidget *interfaceEditor = dialog->findChild<InterfacesTabWidget*>("interfaceEditor");
    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    QVERIFY(nextButton != nullptr);
    QVERIFY(finishButton != nullptr);
    QVERIFY(interfaceEditor != nullptr);
    QVERIFY(obj_name != nullptr);
    QTest::keyClicks(obj_name, "New Cluster");
    QVERIFY(nextButton->isEnabled());

    nextButton->click();
    QVERIFY(dialog->currentPage()==1);

    QTest::qWait(1000);
    nextButton->click();
    QVERIFY(dialog->currentPage()==2);

    dynamic_cast<InterfaceEditorWidget*>(interfaceEditor->widget(0))->setProtocolIndex(3);
    dynamic_cast<InterfaceEditorWidget*>(interfaceEditor->widget(1))->setProtocolIndex(3);
    dynamic_cast<InterfaceEditorWidget*>(interfaceEditor->widget(2))->setProtocolIndex(3);
    dynamic_cast<InterfaceEditorWidget*>(interfaceEditor->widget(3))->setProtocolIndex(3);

    nextButton->click();
    QVERIFY(dialog->currentPage()==3);

    nextButton->click();
    QVERIFY(dialog->currentPage()==4);


    QVERIFY(finishButton->isEnabled());
    finishButton->click();

    Cluster *newc = dialog->getNewCluster();

    QVERIFY(newc != nullptr);
    QVERIFY(Cluster::isA(newc));

}

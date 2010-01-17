#include "newClusterDialogTest.h"

#include <QDebug>
#include <QDialog>

#include "ui_newclusterdialog_q.h"
#include "FWWindow.h"
#include "fwbuilder/Policy.h"
#include "ObjectTreeView.h"
#include "ProjectPanel.h"

using namespace libfwbuilder;
using namespace std;

class newClusterDialog_ext: public newClusterDialog
{
public:
    newClusterDialog_ext(libfwbuilder::FWObject* obj): newClusterDialog(obj)
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

void newClusterDialogTest::test1()
{
    //init();
    FWObjectDatabase *db = new FWObjectDatabase();
    XMLTools::UpgradePredicate p;
    db->load("test_data.fwb", &p, librespath);
    newClusterDialog_ext *dialog = new newClusterDialog_ext(db);
    vector<FWObject*> fws;
    QStringList fwnames;
    foreach(FWObject *obj, db->getByTypeDeep(Firewall::TYPENAME))
    {
        fws.push_back(obj);
        fwnames.append(obj->getName().c_str());
    }

    dialog->setFirewallList(fws);
    dialog->open();

    QStringList dfws = dialog->getFirewallList();
    QVERIFY(dfws.count() == fws.size());
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

    InterfaceEditorWidget* eth0 = qFindChild<InterfaceEditorWidget*>(dialog->getUi()->interfaceEditor, "eth0_widget");
    eth0->addNewAddress("123.45.67.89", "24", true);

    QList<EditedInterfaceData> addresses = dialog->getUi()->interfaceEditor->getNewData();
    foreach( EditedInterfaceData iface, addresses)
    {
        if (iface.name == "eth0")
        {
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
    InterfaceEditorWidget* eth1 = qFindChild<InterfaceEditorWidget*>(dialog->getUi()->interfaceEditor, "eth1_widget");
    eth1->setProtocolIndex(0);
    QTableWidget *addrs = eth1->findChild<QTableWidget*>("addresses");
    QVERIFY(addrs != NULL);
    QPushButton *addaddr = eth1->findChild<QPushButton*>("addAddress");
    QVERIFY(addaddr != NULL);
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

    QVERIFY(newc != NULL);
    QVERIFY(Cluster::isA(newc));
}

void newClusterDialogTest::test2()
{

    mw = new FWWindow();

    mw->loadFile("test_data.fwb", false);

    FWObjectDatabase *db = mw->db();
    Library *lib = NULL;

    foreach(FWObject *obj, db->getByTypeDeep(Library::TYPENAME))
    {
        qDebug() << obj->getName().c_str();
        if (obj->getName() == "new_cluster_test") lib = Library::cast(obj);
    }
    QVERIFY(lib != NULL);

    newClusterDialog_ext *dialog = new newClusterDialog_ext(FWBTree().getStandardSlotForObject(lib, Cluster::TYPENAME));
    vector<FWObject*> fws;
    QStringList fwnames;
    foreach(FWObject *obj, db->getByTypeDeep(Firewall::TYPENAME))
    {
        fws.push_back(obj);
        fwnames.append(obj->getName().c_str());
    }

    dialog->setFirewallList(fws);
    dialog->open();

    QStringList dfws = dialog->getFirewallList();
    QVERIFY(dfws.count() == fws.size());
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

    QVERIFY(newc != NULL);
    QVERIFY(Cluster::isA(newc));

    qDebug() << mw->getCurrentLib()->findObjectByName(Firewall::TYPENAME, "linux-1-bak")->getName().c_str();

}

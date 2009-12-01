/*
 * newClusterDialog.cpp - new Cluster wizard implementation
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#include "FWWindow.h"
#include "newClusterDialog.h"

#include "global.h"
#include "utils_no_qt.h"
#include "utils.h"
#include "platforms.h"

#include "upgradePredicate.h"

#include "ProjectPanel.h"
#include "DialogFactory.h"
#include "FWBTree.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include <qpixmapcache.h>
#include <qfiledialog.h>

#include <QFontDatabase>
#include <QDebug>

#include <iostream>

#define OBJECT_NAME_PAGE 0
#define MANUAL_PAGE      1
#define TEMPLATES_PAGE   2

#define FIREWALLS_PAGE 0
#define INTERFACES_PAGE 1
#define INTERFACEEDITOR_PAGE 2
#define POLICY_PAGE 3
#define SUMMARY_PAGE 4

using namespace libfwbuilder;
using namespace std;


newClusterDialog::newClusterDialog(FWObject *_p)
    : QDialog(), ncl(NULL), fwlist(NULL), tmpldb(NULL)
{
    parent = _p;
    db = parent->getRoot();

    useFirewallList = false;

    m_dialog = new Ui::newClusterDialog_q;
    m_dialog->setupUi(this);

    setControlWidgets(this,
                      m_dialog->stackedWidget,
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);

    unloadTemplatesLib = false;

    connect(m_dialog->templaterBrowseButton, SIGNAL(pressed()),
            this, SLOT(browseTemplate()));
    connect(m_dialog->templateUseStandart, SIGNAL(pressed()),
            this, SLOT(useStandartTemplate()));
    connect(m_dialog->useTemplate, SIGNAL(released()),
            this, SLOT(showHideTemplatePanel()));

    m_dialog->templaterFilePath->setText(tempfname.c_str());
    m_dialog->templaterFrame->setVisible(false);

    // fill in platform (default: iptables)
    setPlatform(m_dialog->platform, "iptables");
    shrinkListOfPlatforms();
    
    // fill in host OS (default: the first item in the list)
    setHostOS(m_dialog->hostOS, "iptables", "");
    shrinkListOfOS();

    setNextEnabled(OBJECT_NAME_PAGE, false);

    m_dialog->obj_name->setFocus();

    showPage(0);
}

void newClusterDialog::browseTemplate()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                       tr("FWBuilder template files"), "",
                       tr("FWBuilder template files (*.xml *.fwb)"));
    if (fileName == "")
    {
        return ;
    }
    m_dialog->templaterFilePath->setText(fileName);
}

void newClusterDialog::useStandartTemplate()
{
    m_dialog->templaterFilePath->setText(tempfname.c_str());
}

void newClusterDialog::showHideTemplatePanel()
{
    if (m_dialog->useTemplate->checkState() == Qt::Checked)
    {
        m_dialog->templaterFrame->setVisible(true);
    }
    else
    {
        m_dialog->templaterFrame->setVisible(false);
    }
}

void newClusterDialog::changed()
{
    int p = currentPage();
    if (p == 0) setNextEnabled(p, !m_dialog->obj_name->text().isEmpty());
}

void newClusterDialog::platformChanged()
{
    setHostOS(m_dialog->hostOS, readPlatform(m_dialog->platform), "");
    shrinkListOfOS();
}

newClusterDialog::~newClusterDialog()
{
    delete m_dialog;
}

bool newClusterDialog::appropriate(const int page) const
{
    int p  = page;

    if (fwbdebug)
    {
        qDebug("newClusterDialog::appropriate p=%d", p);
    }

    switch (p)
    {
    case OBJECT_NAME_PAGE:
    case TEMPLATES_PAGE:
        return true;

    case MANUAL_PAGE:
        return (!m_dialog->useTemplate->isChecked());
    }
    return true;
}


void newClusterDialog::setFirewallList(std::vector<libfwbuilder::FWObject*> data)
{
    m_dialog->interfaceSelector->clear();
    firewallList.clear();
    typedef QPair<Firewall*, bool> fwpair;
    foreach ( FWObject *fw, data )
        firewallList.push_back(Firewall::cast(fw));
    m_dialog->firewallSelector->setFirewallList(firewallList, true);
    useFirewallList = true;
}

void newClusterDialog::showPage(const int page)
{
    FakeWizard::showPage(page);

    int p = currentPage();

    if (fwbdebug)
    {
        qDebug("newClusterDialog::selected p=%d", p);
    }

    switch (p)
    {
    case FIREWALLS_PAGE:
    {
        m_dialog->firewallSelector->clear();
        if (useFirewallList)
        {
            m_dialog->firewallSelector->setFirewallList(firewallList, true);
        }
        else
        {
            list<Firewall*> fwlist;
            mw->findAllFirewalls(fwlist);
            m_dialog->firewallSelector->setFirewallList(fwlist);
        }

        setNextEnabled(FIREWALLS_PAGE, !this->m_dialog->obj_name->text().isEmpty());
        setFinishEnabled(FIREWALLS_PAGE, false);

        break;
    }
    case INTERFACES_PAGE:
    {
        m_dialog->interfaceSelector->clear();
        QList<Firewall*> firewalls;
        typedef QPair<Firewall*, bool> fwpair;
        foreach ( fwpair fw, m_dialog->firewallSelector->getSelectedFirewalls() )
            firewalls.append(fw.first);
        this->m_dialog->interfaceSelector->setFirewallList(firewalls);

        setNextEnabled(INTERFACES_PAGE, true);
        setFinishEnabled(INTERFACES_PAGE, false);
        break;
    }
    case INTERFACEEDITOR_PAGE:
    {
        this->m_dialog->interfaceEditor->setClusterMode(true);
        this->m_dialog->interfaceEditor->clear();

        this->m_dialog->interfaceEditor->setExplanation(
            tr("Depending on the failover protocol, cluster interface "
               "may or may not need an IP address. <b>VRRP</b> and "
               "<b>CARP</b> interfaces should have their own unique "
               "IP addresses different from the addresses of interfaces "
               "of member firewalls. Other failover protocols such as "
               "<b>heartbeat</b> or <b>OpenAIS</b> do not create new "
               "inetrface and therefore do not require additional IP address."
            )
        );

        while (this->m_dialog->interfaceEditor->count())
            this->m_dialog->interfaceEditor->removeTab(0);

        foreach (ClusterInterfaceData iface, this->m_dialog->interfaceSelector->getInterfaces())
        {
            this->m_dialog->interfaceEditor->addClusterInterface(iface);
        }
        setNextEnabled(INTERFACEEDITOR_PAGE, true);
        setFinishEnabled(INTERFACEEDITOR_PAGE, false);
        break;
    }
    case POLICY_PAGE:
    {

        foreach (QRadioButton *btn, radios.keys())
        {
            btn->close();
            delete btn;
        }
        radios.clear();
        QList<QPair<Firewall*, bool> > fws = m_dialog->firewallSelector->getSelectedFirewalls();
        for ( int i = 0; i < fws.count() ; i++ )
        {
            QRadioButton *newbox = new QRadioButton(QString::fromUtf8(fws.at(i).first->getName().c_str()), this);
            qFindChild<QVBoxLayout*>(this->m_dialog->page_4, "policyLayout")->addWidget(newbox);
            radios[newbox] = fws.at(i).first;
        }
        setNextEnabled(POLICY_PAGE, true);
        setFinishEnabled(POLICY_PAGE, false);
        break;
    }
    case SUMMARY_PAGE:
    {
        QFont *monospace = new QFont("Lucida Console");
        if (!monospace->exactMatch())
        {
            monospace->setFixedPitch(true);
            QFontDatabase fontdb;
            foreach (QString family, fontdb.families(QFontDatabase::Latin))
            {
                if (fontdb.isFixedPitch(family, "normal"))
                {
                    monospace->setFamily(family);
                    break;
                }
            }
        }
        this->m_dialog->firewallsList->setFont(*monospace);
        this->m_dialog->interfacesList->setFont(*monospace);
        this->m_dialog->clusterName->setText(this->m_dialog->clusterName->text() + this->m_dialog->obj_name->text());
        QStringList firewalls;
        QList<QPair<Firewall*, bool> > fws = m_dialog->firewallSelector->getSelectedFirewalls();
        QString master;
        for ( int i = 0; i < fws.count() ; i++ )
        {
            if (fws.at(i).second) master = QString::fromUtf8(fws.at(i).first->getName().c_str());
            firewalls.append(QString::fromUtf8(fws.at(i).first->getName().c_str()));
        }
        this->m_dialog->firewallsList->setText(firewalls.join("\n"));
        this->m_dialog->masterLabel->setText(this->m_dialog->masterLabel->text() + master);
        QStringList interfaces;
        foreach (EditedInterfaceData iface, this->m_dialog->interfaceEditor->getNewData())
        {
            QString str;
            if (iface.type == 0) str += tr("regular ");
            if (iface.type == 1) str += tr("dynamic ");
            if (iface.type == 1) str += tr("unnumbered ");
            str += iface.name;
            if (iface.type == 0 && iface.addresses.count() > 0)
            {
                if (iface.addresses.count() == 1)
                    str += tr(" with address: ");
                else
                    str += tr(" with addresses: ");
                QStringList addresses;
                for (int i = 0; i< iface.addresses.values().count(); i++)
                {
                    AddressInfo addr = iface.addresses.values().at(i);
                    QString addrstr;
                    if (i > 0) addrstr.fill(' ', str.length());
                    addrstr += addr.address + "/" + addr.netmask;
                    addresses.append(addrstr);
                }
                str += addresses.join("\n");
            }
            interfaces.append(str);
        }
        this->m_dialog->interfacesList->setText(interfaces.join("\n"));
        bool doCopy = false;
        foreach (QRadioButton* btn, radios.keys())
        {
            if (btn->isChecked() && btn != this->m_dialog->noPolicy)
            {
                QString fwname = QString::fromUtf8(radios[btn]->getName().c_str());
                this->m_dialog->policyLabel->setText(this->m_dialog->policyLabel->text() + fwname);
                doCopy = true;
                break;
            }
        }
        if (!doCopy) this->m_dialog->policyLabel->setVisible(false);

        setNextEnabled(SUMMARY_PAGE, false);
        setFinishEnabled(SUMMARY_PAGE, true);
        break;
    }
    }
    /*
    switch (p)
    {
    case MANUAL_PAGE:
    {
        // This is the page where user configures interfaces manually

        // Fill failover protocols combo box
        list<QStringPair> protocols;
        getFailoverTypesForOS(readHostOS(m_dialog->hostOS).toLatin1().constData(),
                              protocols);
        m_dialog->failover_protocol->clear();
        for (list<QStringPair>::iterator i=protocols.begin(); i!=protocols.end(); i++)
        {
            QString proto = i->first;
            m_dialog->failover_protocol->addItem(proto, proto);
        }
        m_dialog->failover_protocol->setCurrentIndex(0);

        m_dialog->iface_name->setFocus();

        setNextEnabled(MANUAL_PAGE, false);
        setFinishEnabled(MANUAL_PAGE, true);
        break;
    }

    case TEMPLATES_PAGE:
        setFinishEnabled(TEMPLATES_PAGE, true);
        // load templates if not loaded
        if (tmpldb == NULL)
        {
            MessageBoxUpgradePredicate upgrade_predicate(this);

            tmpldb = new FWObjectDatabase();
            tmpldb->setReadOnly(false);
            tmpldb->load(m_dialog->templaterFilePath->text().toAscii().data(),
                         &upgrade_predicate, librespath);
        }
        list<FWObject*> fl;

        FWObjectTypedChildIterator libiter =
            tmpldb->findByType(Library::TYPENAME);
        for (; libiter != libiter.end(); ++libiter)
        {
            findClusters(*libiter, fl, false);
        }

        if (fl.empty())
        {
            return;
        }
        QString icn = ":/Icons/Cluster/icon-tree";

        m_dialog->templateList->clear();

        int n = 0;
        for (list<FWObject*>::iterator m = fl.begin(); m != fl.end(); m++, n++)
        {
            FWObject *o = *m;

            QPixmap pm;
            if (!QPixmapCache::find(icn, pm))
            {
                pm.load(icn);
                QPixmapCache::insert(icn, pm);
            }

            QListWidgetItem *item = new QListWidgetItem(
                QIcon(pm), QString(o->getName().c_str()));
            m_dialog->templateList->addItem(item);

            templates[m_dialog->templateList->item(
                m_dialog->templateList->count()-1)] = o;
        }
        m_dialog->templateList->setCurrentItem(0);
        m_dialog->templateList->setFocus();
    }
    */
}

void newClusterDialog::addInterface()
{
    /*
    QString addr, netm, protocol, secr, vrid;
    getInterfaceAttributes(&addr, &netm, &protocol, &secr, &vrid);

    if (!validateAddressAndMask(addr, netm))
    {
        return;
    }

    QStringList qsl;
    qsl << m_dialog->iface_name->text()
        << m_dialog->iface_label->text()
        << addr
        << netm
        << protocol
        << secr
        << vrid;

    new QTreeWidgetItem(m_dialog->iface_list, qsl);*/
}

void newClusterDialog::selectedInterface(QTreeWidgetItem*cur, QTreeWidgetItem*)
{
    QTreeWidgetItem *itm = cur; //current item
    if (itm)
    {
        m_dialog->iface_name->setText(itm->text(0));
        m_dialog->iface_label->setText(itm->text(1));
        m_dialog->iface_addr->setText(itm->text(2));
        m_dialog->iface_netmask->setText(itm->text(3));

        QString failover_protocol_name = m_dialog->failover_protocol->itemData(
            m_dialog->failover_protocol->currentIndex()).toString();
        if (failover_protocol_name == "vrrp")
        {
            m_dialog->vrrp_secret->setText(itm->text(4));
            m_dialog->vrrp_vrid->setValue(itm->text(5).toInt());
        }
        if (failover_protocol_name == "carp")
        {
            m_dialog->carp_password->setText(itm->text(4));
            m_dialog->carp_vhid->setValue(itm->text(5).toInt());
        }
    }
}


void newClusterDialog::updateInterface()
{
    QTreeWidgetItem *itm = m_dialog->iface_list->currentItem();
    if (itm == NULL)
    {
        return;
    }

    QString addr, netm, protocol, secr, vrid;
    getInterfaceAttributes(&addr, &netm, &protocol, &secr, &vrid);

    if (!validateAddressAndMask(addr, netm))
    {
        return;
    }

    itm->setText(0, m_dialog->iface_name->text());
    itm->setText(1, m_dialog->iface_label->text());
    itm->setText(2, addr);
    itm->setText(3, netm);
    itm->setText(4, protocol);
    itm->setText(5, secr);
    itm->setText(6, vrid);
}

void newClusterDialog::deleteInterface()
{
    /*
    QTreeWidgetItem *itm = m_dialog->iface_list->currentItem();
    if (itm == NULL)
    {
        return;
    }
    m_dialog->iface_list->takeTopLevelItem(
        m_dialog->iface_list->indexOfTopLevelItem(itm));
    */
}

void newClusterDialog::templateSelected(QListWidgetItem *itm)
{
    if (fwbdebug)
    {
        qDebug("newClusterDialog::templateSelected");
    }

    FWObject *o = templates[itm];
    assert(o != NULL);

    Cluster *cl = Cluster::cast(o);

    m_dialog->templateComment->clear();
    m_dialog->templateComment->append(cl->getComment().c_str());
    m_dialog->templateComment->moveCursor(QTextCursor::Start);

    bool haveOutside = false;
    bool haveInside  = false;
    bool haveDMZ     = false;
    list<FWObject*> ll = cl->getByType(Interface::TYPENAME);
    for (FWObject::iterator i = ll.begin(); i != ll.end(); i++)
    {
        Interface *intf = Interface::cast(*i);
        if (intf->getLabel() == "outside")
        {
            haveOutside = true;
            m_dialog->intfOutsideLine->show();
            m_dialog->intfOutsideText->show();
            fillInterfaceData(intf, m_dialog->intfOutsideText);
        }
        if (intf->getLabel() == "inside")
        {
            haveInside = true;
            m_dialog->intfInsideLine->show();
            m_dialog->intfInsideText->show();
            fillInterfaceData(intf, m_dialog->intfInsideText);
        }
        if (intf->getLabel() == "dmz")
        {
            haveDMZ = true;
            m_dialog->intfDMZLine->show();
            m_dialog->intfDMZText->show();
            fillInterfaceData(intf, m_dialog->intfDMZText);
        }
    }

    if (!haveOutside)
    {
        m_dialog->intfOutsideLine->hide();
        m_dialog->intfOutsideText->hide();
    }
    if (!haveInside)
    {
        m_dialog->intfInsideLine->hide();
        m_dialog->intfInsideText->hide();
    }
    if (!haveDMZ)
    {
        m_dialog->intfDMZLine->hide();
        m_dialog->intfDMZText->hide();
    }
}

void newClusterDialog::fillInterfaceData(Interface *intf, QTextBrowser *qte)
{
    qte->clear();
    QString s;

    s += "<table border='0' cellspacing='0' cellpadding='0'>";
    s += "<tr>";
    s += "<td>";
    s +=  tr("Interface: %1 (%2)")
          .arg(intf->getName().c_str())
          .arg(intf->getLabel().c_str());
    s += "</td>";
    s += "</tr>";

    s += "<tr>";
    s += "<td>";
    const InetAddr *addr = intf->getAddressPtr();
    QString addr_str = (addr) ? addr->toString().c_str() : "";
    const InetAddr *netm = intf->getNetmaskPtr();
    QString netm_str = (netm) ? netm->toString().c_str() : "";
    s += QString("%1/%2").arg(addr_str).arg(netm_str);
    s += "</td>";
    s += "</tr>";
    s += "</table>";
    qte->setText(s);
}

bool newClusterDialog::validateAddressAndMask(const QString &addr,
    const QString &netm)
{
    try
    {
        InetAddr(addr.toLatin1().constData());
    }
    catch (FWException &ex)
    {
        QMessageBox::warning(
            this, "Firewall Builder",
            tr("Invalid address '%1/%2'").arg(addr).arg(netm),
            "&Continue", QString::null, QString::null, 0, 1);
        return false;
    }
    try
    {
        bool ok = false ;
        int ilen = netm.toInt(&ok);
        if (ok)
        {
            if (ilen < 0 || ilen > 32)
            {
                QMessageBox::warning(
                    this, "Firewall Builder",
                    tr("Invalid address '%1/%2'").arg(addr).arg(netm),
                    "&Continue", QString::null, QString::null, 0, 1);
                return false;
            }
        }
        else
        {
            InetAddr(netm.toLatin1().constData());
        }

    }
    catch (FWException &ex)
    {
        QMessageBox::warning(
            this, "Firewall Builder",
            tr("Invalid address '%1/%2'").arg(addr).arg(netm),
            "&Continue", QString::null, QString::null, 0, 1);
        return false;
    }
    return true;
}

void newClusterDialog::finishClicked()
{
    createNewCluster();

    /*
    int p = currentPage();

    if (p == TEMPLATES_PAGE)
    {
        QListWidgetItem *itm = m_dialog->templateList->currentItem();
        FWObject *template_cl = templates[itm];
        assert(template_cl != NULL);

        FWObject *no = db->create(Cluster::TYPENAME);

        if (no == NULL)
        {
            QDialog::accept();
            return;
        }

        parent->add(no);
        no->duplicate(template_cl, true);
        no->setName(m_dialog->obj_name->text().toStdString());

        no->setStr("platform",
                   readPlatform(m_dialog->platform).toLatin1().constData());
        no->setStr("host_OS",
                   readHostOS(m_dialog->hostOS).toLatin1().constData());

        ncl = Cluster::cast(no);

    }
    else
    {*/
    // create cluster object

/*
    while (itm != NULL)
    {
        QString name    = itm->text(0);
        QString label   = itm->text(1);
        QString addr    = itm->text(2);
        QString netmask = itm->text(3);
        QString protocol  = itm->text(4);
        QString secret  = itm->text(5);
        QString vrid    = itm->text(6);

        Interface *oi = Interface::cast(db->create(Interface::TYPENAME));
        oi->setName(name.toStdString());

        ncl->add(oi);
        oi->setLabel(label.toLatin1().constData());

        QString addrname = QString("%1:%2:ip")
                           .arg(m_dialog->obj_name->text())
                           .arg(name);
        IPv4 *oa = IPv4::cast(db->create(IPv4::TYPENAME));
        oa->setName(addrname.toStdString());
        oi->add(oa);
        oa->setAddress(InetAddr(addr.toLatin1().constData()));
        bool ok = false ;
        int inetmask = netmask.toInt(&ok);
        if (ok)
        {
            oa->setNetmask(InetAddr(inetmask));
        }
        else
        {
            oa->setNetmask(InetAddr(netmask.toLatin1().constData()));
        }

        // create InterfaceOptions object and enable vrrp settings
        FWOptions *ifopt;
        ifopt = oi->getOptionsObject();
        ifopt->setStr("type", "cluster_interface");


        // create vrrp cluster group for this interface
        ClusterGroup *failover_grp;
        QString grpname = QString("%1:%2:members")
                          .arg(m_dialog->obj_name->text())
                          .arg(name);

                          ==

        failover_grp = ClusterGroup::cast(
            db->create(FailoverClusterGroup::TYPENAME));
        failover_grp->setName(grpname.toStdString());
        oi->add(failover_grp);

//            QString failover_protocol_name = m_dialog->failover_protocol->itemData(
//                m_dialog->failover_protocol->currentIndex()).toString();

        QString failover_protocol_name = protocol;

        failover_grp->setStr(
            "type", failover_protocol_name.toAscii().constData());

            ****

        if (failover_protocol_name == "vrrp")
        {
            FWOptions *grp_opt = failover_grp->getOptionsObject();
            grp_opt->setStr("vrrp_secret", secret.toStdString());
            grp_opt->setStr("vrrp_vrid", vrid.toStdString());
        }

        if (failover_protocol_name == "carp")
        {
            FWOptions *grp_opt = failover_grp->getOptionsObject();
            grp_opt->setStr("carp_password", secret.toStdString());
            grp_opt->setStr("carp_vhid", vrid.toStdString());
        }

//            mw->activeProject()->updateObjName(oi, "", "", false);

        itm_index++;
        itm = m_dialog->iface_list->topLevelItem(itm_index);
    }
    */
    //}

    if (unloadTemplatesLib)
    {
        delete tmpldb;
        tmpldb = NULL;

        unloadTemplatesLib = false;
    }
    QDialog::accept();
}

void newClusterDialog::cancelClicked()
{
    QDialog::reject();
}

void newClusterDialog::nextClicked()
{
    if (currentPage() == FIREWALLS_PAGE)
    {
        if (!this->m_dialog->firewallSelector->isValid()) return;
        if ( this->m_dialog->firewallSelector->getSelectedFirewalls().count() == 0 )
        {
            QMessageBox::critical(
            this, "Firewall Builder",
            tr("You should select at least one firewall to create a cluster"),
            "&Continue", QString::null, QString::null, 0, 1);
            return;
        }
    }
    if (currentPage() == INTERFACES_PAGE)
    {
        if (!this->m_dialog->interfaceSelector->isValid())
            return;
    }
    if (currentPage() == INTERFACEEDITOR_PAGE)
        if (!this->m_dialog->interfaceEditor->isValid())
            return;
    if (nextRelevant(currentPage()) > -1)
    {
        showPage(nextRelevant(currentPage()));
    }
}

void newClusterDialog::backClicked()
{
    if (previousRelevant(currentPage()) > -1)
    {
        showPage(previousRelevant(currentPage()));
    }
}

void newClusterDialog::failoverProtocolChanged()
{
    QString protocol_name = m_dialog->failover_protocol->itemData(
        m_dialog->failover_protocol->currentIndex()).toString();

    QWidget *page = qFindChild<QWidget*>(
        m_dialog->failover_protocol_widget, protocol_name);
    if (page!=NULL)
        m_dialog->failover_protocol_widget->setCurrentWidget(page);
    else
        cerr << "Can't find stack page for protocol "
             << protocol_name.toStdString()
             << endl;
}

void newClusterDialog::getInterfaceAttributes(QString *addr, QString *netm,
                                              QString *proto,
                                              QString *secr, QString *vrid)
{
    *addr = m_dialog->iface_addr->text();
    *netm = m_dialog->iface_netmask->text();
    *proto = "";
    *secr = "";
    *vrid = "";

    QString failover_protocol_name = m_dialog->failover_protocol->itemData(
        m_dialog->failover_protocol->currentIndex()).toString();
    if (failover_protocol_name == "vrrp")
    {
        *secr = m_dialog->vrrp_secret->text();
        *vrid = m_dialog->vrrp_vrid->text();
        if (secr->isEmpty()) *secr = "vrrp_secret";
    }
    if (failover_protocol_name == "carp")
    {
        *secr = m_dialog->carp_password->text();
        *vrid = m_dialog->carp_vhid->text();
        if (secr->isEmpty()) *secr = "carp_secret";
    }

    *proto = failover_protocol_name;

    if (addr->isEmpty())
    {
        *addr = QString(InetAddr::getAny().toString().c_str());
    }
    if (netm->isEmpty())
    {
        *netm = QString(InetAddr::getAny().toString().c_str());
    }
}

// remove platforms that do not support clustering
void newClusterDialog::shrinkListOfPlatforms()
{
    list<int> remove_items;
    for (int idx=0; idx < m_dialog->platform->count(); ++idx)
    {
        QString platform = m_dialog->platform->itemData(idx).toString();
        try
        {
            if (!Resources::getTargetCapabilityBool(platform.toStdString(),
                                                    "supports_cluster"))
                remove_items.push_back(idx);
        } catch (FWException &ex)
        {
            // Unsupported platform or unrecognized drop-down list item
            // (such as an empty string)
            remove_items.push_back(idx);
        }
    }

    while (!remove_items.empty())
    {
        int idx = remove_items.back();
        m_dialog->platform->removeItem(idx);
        remove_items.pop_back();
    }
}

void newClusterDialog::shrinkListOfOS()
{
    list<int> remove_items;
    for (int idx=0; idx < m_dialog->hostOS->count(); ++idx)
    {
        QString host_os = m_dialog->hostOS->itemData(idx).toString();
        try
        {
            if (!Resources::getTargetCapabilityBool(host_os.toStdString(),
                                                    "supports_cluster"))
                remove_items.push_back(idx);
        } catch (FWException &ex)
        {
            // Unsupported platform or unrecognized drop-down list item
            // (such as an empty string)
            remove_items.push_back(idx);
        }
    }

    while (!remove_items.empty())
    {
        int idx = remove_items.back();
        m_dialog->hostOS->removeItem(idx);
        remove_items.pop_back();
    }
}



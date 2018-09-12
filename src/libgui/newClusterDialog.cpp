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

#define FIREWALLS_PAGE 0
#define INTERFACES_PAGE 1
#define INTERFACEEDITOR_PAGE 2
#define POLICY_PAGE 3
#define SUMMARY_PAGE 4

using namespace libfwbuilder;
using namespace std;


newClusterDialog::newClusterDialog(QWidget *parentw, FWObject *_p)
    : QDialog(parentw), ncl(nullptr), fwlist(nullptr), tmpldb(nullptr)
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

    m_dialog->obj_name->setFocus();

    this->noPolicy = new QRadioButton(m_dialog->page_4);
    this->noPolicy->setChecked(true);
    this->spacer = new QSpacerItem(10, 1000);
    this->m_dialog->page_4->layout()->addWidget(noPolicy);
    this->m_dialog->page_4->layout()->addItem(spacer);
    noPolicy->setText(tr("do not use any, i will create new policy and NAT rules"));

    showPage(0);
}

void newClusterDialog::changed()
{
    int p = currentPage();
    if (p == 0) setNextEnabled(p, !m_dialog->obj_name->text().isEmpty());
}

newClusterDialog::~newClusterDialog()
{
    delete m_dialog;
}

void newClusterDialog::setFirewallList(std::vector<libfwbuilder::FWObject*> data, bool select)
{
    m_dialog->interfaceSelector->clear();
    firewallList.clear();
    //typedef QPair<Firewall*, bool> fwpair; // Unused
    foreach ( FWObject *fw, data )
        firewallList.push_back(Firewall::cast(fw));
    m_dialog->firewallSelector->setFirewallList(firewallList, select);
}

void newClusterDialog::showPage(const int page, bool blank)
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
        if (blank)
        {
            m_dialog->firewallSelector->clear();
            m_dialog->firewallSelector->setFirewallList(firewallList, true);
            /*else
            {
                list<Firewall*> fwlist;
                mw->findAllFirewalls(fwlist);
                m_dialog->firewallSelector->setFirewallList(fwlist);
            }*/
        }

        setNextEnabled(FIREWALLS_PAGE, !this->m_dialog->obj_name->text().isEmpty());
        setFinishEnabled(FIREWALLS_PAGE, false);
        m_dialog->nextButton->setDefault(true);
        break;
    }
    case INTERFACES_PAGE:
    {
        if (blank)
        {
            m_dialog->interfaceSelector->clear();
            QList<Firewall*> firewalls;
            typedef QPair<Firewall*, bool> fwpair;
            foreach ( fwpair fw, m_dialog->firewallSelector->getSelectedFirewalls() )
                firewalls.append(fw.first);
            this->m_dialog->interfaceSelector->setFirewallList(firewalls);
        }
        setNextEnabled(INTERFACES_PAGE, true);
        setFinishEnabled(INTERFACES_PAGE, false);
        m_dialog->nextButton->setDefault(true);
        break;
    }
    case INTERFACEEDITOR_PAGE:
    {
        if (blank)
        {
        if (this->m_dialog->interfaceSelector->getInterfaces().count() == 0)
            this->showPage(POLICY_PAGE);

        this->m_dialog->interfaceEditor->setClusterMode(true);
        this->m_dialog->interfaceEditor->clear();

        this->m_dialog->interfaceEditor->setExplanation(
            tr("Depending on the failover protocol, cluster interface "
               "may or may not need an IP address. <b>VRRP</b>, "
               "<b>CARP</b>, <b>heartbeat</b> interfaces should have their "
               "own unique IP addresses different from the member firewall "
               "interfaces. Other failover protocols such as the one used in "
               "<b>Cisco ASA (PIX) firewall</b> do not require additional IP address."
               "<br>"
               "<br>"
               "List of available failover protocols depends on the firewall "
               "platform."
            )
        );

        while (this->m_dialog->interfaceEditor->count())
            this->m_dialog->interfaceEditor->removeTab(0);

        foreach (ClusterInterfaceData iface, this->m_dialog->interfaceSelector->getInterfaces())
        {
            this->m_dialog->interfaceEditor->addClusterInterface(iface);
        }
        }
        setNextEnabled(INTERFACEEDITOR_PAGE, true);
        setFinishEnabled(INTERFACEEDITOR_PAGE, false);
        m_dialog->nextButton->setDefault(true);
        break;
    }
    case POLICY_PAGE:
    {
        if (blank)
        {
        foreach (QRadioButton *btn, copy_rules_from_buttons.keys())
        {
            btn->close();
            delete btn;
        }
        copy_rules_from_buttons.clear();
        QList<QPair<Firewall*, bool> > fws = m_dialog->firewallSelector->getSelectedFirewalls();
        this->m_dialog->page_4->layout()->removeItem(spacer);
        for ( int i = 0; i < fws.count() ; i++ )
        {
            QRadioButton *newbox = new QRadioButton(QString::fromUtf8(fws.at(i).first->getName().c_str()), m_dialog->page_4);
            newbox->setObjectName(fws.at(i).first->getName().c_str());
            this->m_dialog->page_4->layout()->addWidget(newbox);
            copy_rules_from_buttons[newbox] = fws.at(i).first;
        }
        this->m_dialog->page_4->layout()->addItem(spacer);
        }
        setNextEnabled(POLICY_PAGE, true);
        setFinishEnabled(POLICY_PAGE, false);
        m_dialog->nextButton->setDefault(true);
        break;
    }
    case SUMMARY_PAGE:
    {
        QFont *monospace = new QFont("Lucida Console");
        if (!monospace->exactMatch())
        {
            monospace->setFixedPitch(true);
            monospace->setStyleHint(QFont::SansSerif, QFont::PreferAntialias);
            QFontDatabase fontdb;
            foreach (QString family, fontdb.families(QFontDatabase::Latin))
            {
                if (family.startsWith(".")) continue;
                if (fontdb.isFixedPitch(family, "normal"))
                {
                    qDebug() << family;
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
        foreach (QRadioButton* btn, copy_rules_from_buttons.keys())
        {
            if (btn->isChecked() && btn != noPolicy)
            {
                QString fwname = QString::fromUtf8(
                    copy_rules_from_buttons[btn]->getName().c_str());
                this->m_dialog->policyLabel->setText(this->m_dialog->policyLabel->text() + fwname);
                doCopy = true;
                break;
            }
        }
        if (!doCopy) this->m_dialog->policyLabel->setVisible(false);

        setNextEnabled(SUMMARY_PAGE, false);
        setFinishEnabled(SUMMARY_PAGE, true);
        m_dialog->finishButton->setDefault(true);
        break;
    }
    }
}

void newClusterDialog::finishClicked()
{
    m_dialog->nextButton->setFocus(Qt::OtherFocusReason);
    createNewCluster();

    if (unloadTemplatesLib)
    {
        delete tmpldb;
        tmpldb = nullptr;

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
    m_dialog->nextButton->setFocus(Qt::OtherFocusReason);
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
        showPage(previousRelevant(currentPage()), false);
    }
}

int newClusterDialog::previousRelevant(const int page) const
{
    if (page == POLICY_PAGE &&
        this->m_dialog->interfaceEditor->getNewData().count() == 0)
    {
        return INTERFACES_PAGE;
    }
    else return FakeWizard::previousRelevant(page);
}


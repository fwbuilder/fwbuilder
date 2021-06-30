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

#include "FirewallSelectorWidget.h"

#include <QDebug>

using namespace std;
using namespace libfwbuilder;

FirewallSelectorWidget::FirewallSelectorWidget(QWidget *parent):
        QTableWidget(parent)
{
 //   this->setHorizontalHeaderLabels(QStringList() << tr("Title") << tr("Use") << tr("Master"));
}

FirewallSelectorWidget::~FirewallSelectorWidget()
{
    clear();
}

void FirewallSelectorWidget::setFirewallList(list<Firewall*> firewalls, bool select)
{
    QSet<Firewall*> nonuniq;
    foreach (Firewall* fw1, firewalls)
    {
        foreach (Firewall* fw2, firewalls)
        {
            if (fw1 != fw2 && fw1->getName() == fw2->getName() && fw2->getLibraryName() != fw1->getLibraryName())
            {
                nonuniq.insert(fw1);
                nonuniq.insert(fw2);
            }
        }
    }

    this->clear();
    Firewall *fw;
    for (list<Firewall*>::iterator it = firewalls.begin(); it != firewalls.end(); it++)
    {
        fw = *it;
        QTableWidgetItem *title = new QTableWidgetItem(QIcon(":/Icons/Firewall/icon"),
                                                       QString::fromUtf8(fw->getName().c_str()));
        if (nonuniq.contains(fw))
            title->setText((fw->getLibraryName() + " / " + fw->getName()).c_str());
        title->setFlags(Qt::ItemIsEnabled);
        QCheckBox *box = new QCheckBox(this);
        connect(box, SIGNAL(toggled(bool)), this, SLOT(usageChanged(bool)));
        QRadioButton *radio = new QRadioButton(this);
        radio->setEnabled(false);
        TableRow rw;
        rw.firewall = fw;
        rw.master = radio;
        rw.title = title;
        rw.use = box;
        int rowNumber = this->rowCount();
        boxrow[box] = rowNumber;
        radiorow[radio] = rowNumber;
        this->insertRow(rowNumber);
        rows[rowNumber] = rw;
        this->setItem(rowNumber, 0, title);
        this->setCellWidget(rowNumber, 1, box);
        this->setCellWidget(rowNumber, 2, radio);
        if (select) box->setChecked(true);
    }
    this->sortItems(0, Qt::AscendingOrder);
}


void FirewallSelectorWidget::usageChanged(bool st)
{
    int row = this->boxrow[dynamic_cast<QCheckBox*>(sender())];
    if (!st) rows[row].master->setChecked(st);
    rows[row].master->setEnabled(st);
    bool isMaster = false, isSelected = false;
    foreach(TableRow row, this->rows.values())
    {
        if (row.use->isChecked()) isSelected = true;
        if (row.master->isChecked() && row.use->isChecked()) isMaster = true;
    }
    if (isSelected && !isMaster)
    {
        foreach(TableRow row, this->rows.values())
        {
            if (row.use->isChecked())
            {
                row.master->setChecked(true);
                break;
            }
        }
    }
}

QList<QPair<Firewall*, bool> > FirewallSelectorWidget::getSelectedFirewalls()
{
    QList<QPair<Firewall*, bool> > res;
    foreach(TableRow row, this->rows.values())
    {
        if (row.use->isChecked())
            res.append(qMakePair(row.firewall, row.master->isChecked()));
    }
    return res;
}

bool FirewallSelectorWidget::isValid()
{
    QString host_os, platform, version;
    QList<QPair<Firewall*, bool> > fws = this->getSelectedFirewalls();
    if (fws.count() == 0)
    {
        QMessageBox::critical(
                this, "Firewall Builder",
                tr("You should select at least one firewall to use "
                   "with the cluster"),
                "&Continue", QString(), QString(), 0, 1);
        return false;
    }
    for ( int i = 0; i < fws.count(); i++)
    {
        if (host_os.isEmpty()) host_os = fws.at(i).first->getStr("host_OS").c_str();
        else if (host_os != fws.at(i).first->getStr("host_OS").c_str())
        {
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("Host operation systems of chosen firewalls are different"),
                "&Continue", QString(), QString(), 0, 1);
            return false;
        }
        if (platform.isEmpty()) platform = fws.at(i).first->getStr("platform").c_str();
        else if (platform != fws.at(i).first->getStr("platform").c_str())
        {
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("Platforms of chosen firewalls are different"),
                "&Continue", QString(), QString(), 0, 1);
            return false;
        }
#ifdef COMPARE_MEMBER_VERSIONS_FOR_CLUSTER
        if (version.isEmpty()) version = fws.at(i).first->getStr("version").c_str();
        else if (version != fws.at(i).first->getStr("version").c_str())
        {
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("Versions of chosen firewalls are different"),
                "&Continue", QString(), QString(), 0, 1);
            return false;
        }
#endif
    }
    int ok = false;
    // check for at least one same interface in all firwalls
    if ( fws.count() )
    {
        FWObjectTypedChildIterator intrs =  fws.first().first->findByType(Interface::TYPENAME);
        for ( ; intrs!=intrs.end(); ++intrs )
        {
            //if (Interface::cast(*intrs)->isLoopback()) continue;
            string name = Interface::cast(*intrs)->getName();
            int got = 0;
            for ( int j = 0; j < fws.count(); j++)
            {
                FWObjectTypedChildIterator intrs2 =  fws.at(j).first->findByType(Interface::TYPENAME);
                for ( ; intrs2!=intrs2.end(); ++intrs2 )
                {
                    if (Interface::cast(*intrs2)->getName() == name)
                    {
                        got ++;
                        break;
                    }
                }
            }
            if (got == fws.count())
            {
                ok = true;
                break;
            }
        }
    }/*
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
    }*/
    if (!ok)
    {
        QMessageBox::critical(
                this, "Firewall Builder",
                tr("Cluster firewalls should have at least one common inteface"),
                "&Continue", QString(), QString(), 0, 1);
    }
    return ok;
}

void FirewallSelectorWidget::clear()
{
    //QTableWidget::clear();
    for (int i =0; i < this->rows.keys().count(); i++)
    {
        this->removeRow(i);
        boxrow.remove(rows[i].use);
        radiorow.remove(rows[i].master);
        delete this->rows[i].master;
        delete this->rows[i].use;
    }
    boxrow.clear();
    radiorow.clear();
    rows.clear();
    this->setRowCount(0);
//    QTableWidget::clear();
}


void FirewallSelectorWidget::resizeEvent(QResizeEvent*)
{
    int total = this->viewport()->width();
    if (total < 100) total = int(this->width() * 0.95);
    int controls;
    if ( total/3 > 100 ) controls = 100; else controls = total/3;
    this->setColumnWidth(0, total - controls*2);
    this->setColumnWidth(1, controls);
    this->setColumnWidth(2, controls);
}

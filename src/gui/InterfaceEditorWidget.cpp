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

#include "InterfaceEditorWidget.h"
#include "ui_InterfaceEditorWidget.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include <QDebug>

using namespace libfwbuilder;
using namespace std;

InterfaceEditorWidget::InterfaceEditorWidget(QWidget *parent, Interface *interface) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditorWidget)
{
    tabw = dynamic_cast<QTabWidget*>(parent);
    this->interface = interface;
    m_ui->setupUi(this);
    setProtocolVisible(false);
    this->m_ui->name->setText(interface->getName().c_str());
    this->m_ui->label->setText(interface->getLabel().c_str());
    if (interface->getPhysicalAddress() != NULL)
       m_ui->mac->setText(interface->getPhysicalAddress()->getPhysAddress().c_str());
    this->m_ui->comment->setPlainText(interface->getComment().c_str());
    if ( this->interface->isDyn() )
        this->m_ui->type->setCurrentIndex(1);
    if ( this->interface->isUnnumbered() )
        this->m_ui->type->setCurrentIndex(2);
    FWObjectTypedChildIterator adriter = interface->findByType(IPv4::TYPENAME);
    for ( ; adriter != adriter.end(); ++adriter )
    {
        Address *addr = Address::cast(*adriter);
        int row = addNewAddress();
        fwaddrs[row] = addr;
        rows[row].first->setText(addr->getAddressPtr()->toString().c_str());
        rows[row].second->setText(addr->getNetmaskPtr()->toString().c_str());
    }
    FWObjectTypedChildIterator adriter2 = interface->findByType(IPv6::TYPENAME);
    for ( ; adriter2 != adriter2.end(); ++adriter2 )
    {
        Address *addr = Address::cast(*adriter2);
        int row = addNewAddress();
        fwaddrs[row] = addr;
        rows[row].first->setText(addr->getAddressPtr()->toString().c_str());
        rows[row].second->setText(addr->getNetmaskPtr()->toString().c_str());
    }
}


InterfaceEditorWidget::InterfaceEditorWidget(QWidget *parent, ClusterInterfaceData data) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditorWidget)
{
    tabw = dynamic_cast<QTabWidget*>(parent);
    m_ui->setupUi(this);
    this->interface = NULL;
    this->m_ui->name->setText(data.name);
    this->m_ui->label->setText(data.label);
    this->m_ui->comment->setText(data.comment);

    set<AddressInfo> addrs;
    for ( int i =0; i < data.interfaces.count(); i++ )
    {
        Interface *intr = data.interfaces.at(i).second;
        FWObjectTypedChildIterator adriter = intr->findByType(IPv4::TYPENAME);
        for ( ; adriter != adriter.end(); ++adriter )
        {
            Address *addr = Address::cast(*adriter);
            AddressInfo newdata;
            newdata.address = addr->getAddressPtr()->toString().c_str();
            newdata.netmask = addr->getNetmaskPtr()->toString().c_str();
            newdata.ipv4 = true;
            addrs.insert(newdata);
        }
        FWObjectTypedChildIterator adriter2 = intr->findByType(IPv6::TYPENAME);
        for ( ; adriter2 != adriter2.end(); ++adriter2 )
        {
            Address *addr = Address::cast(*adriter2);
            AddressInfo newdata;
            newdata.address = addr->getAddressPtr()->toString().c_str();
            newdata.netmask = addr->getNetmaskPtr()->toString().c_str();
            newdata.ipv4 = false;
            addrs.insert(newdata);
        }
    }
    foreach(AddressInfo addr, addrs)
    {
        int row = addNewAddress();
        fwaddrs[row] = NULL;
        rows[row].first->setText(addr.address);
        rows[row].second->setText(addr.netmask);
        types[row]->setCurrentIndex(addr.ipv4==true?0:1);
    }

    QString host_os = data.os;
    list<QStringPair> types;
    getFailoverTypesForOS(host_os, types);
    QStringList typenames;
    foreach(QStringPair pair, types)
        if (pair.first != "none")
            typenames << pair.second;
    this->m_ui->protocol->clear();
    this->m_ui->protocol->insertItems(0, typenames);
}

void InterfaceEditorWidget::setData(InterfaceData *data)
{
    this->m_ui->name->setText(data->name.c_str());
    this->m_ui->label->setText(data->label.c_str());
    this->m_ui->mac->setText(data->mac_addr.c_str());
    this->m_ui->comment->clear();
    if ( data->isDyn )
        this->m_ui->type->setCurrentIndex(1);
    else if ( data->isUnnumbered )
        this->m_ui->type->setCurrentIndex(2);
    else
        this->m_ui->type->setCurrentIndex(0);
    foreach(QPushButton *btn, this->buttons.keys())
        btn->click();
    if ( !data->isDyn && !data->isUnnumbered )
    {
        foreach( InetAddrMask* addr, data->addr_mask )
        {
            int row = addNewAddress();
            types[row]->setCurrentIndex(addr->getAddressPtr()->isV6());
            rows[row].first->setText(addr->getAddressPtr()->toString().c_str());
            rows[row].second->setText(addr->getNetmaskPtr()->toString().c_str());
        }
    }
}

InterfaceEditorWidget::InterfaceEditorWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditorWidget)
{
    tabw = dynamic_cast<QTabWidget*>(parent);
    this->interface = NULL;
    m_ui->setupUi(this);
    setProtocolVisible(false);
    this->m_ui->name->setText(tr("New interface"));
    this->m_ui->label->clear();
    this->m_ui->comment->clear();
    addNewAddress();
}

void InterfaceEditorWidget::deleteAddress()
{
    QPushButton *button = dynamic_cast<QPushButton*>(sender());
    QPair<QTableWidgetItem*, QTableWidgetItem*> items = buttons[button];
    buttons.remove(button);
    int row = items.first->row();
    this->m_ui->addresses->removeRow(row);
    delete types[row];
    types.remove(row);
    rows.remove(row);
    fwaddrs.remove(row);
}

InterfaceEditorWidget::~InterfaceEditorWidget()
{
    delete m_ui;
    foreach(QPushButton* btn, buttons.keys())
        delete btn;
    foreach(QComboBox* box, types.values())
        delete box;
}

int InterfaceEditorWidget::addNewAddress()
{
    int row = this->m_ui->addresses->rowCount();
    fwaddrs[row] = NULL;
    this->m_ui->addresses->insertRow(row);
    QTableWidgetItem *addrItem = new QTableWidgetItem();
    QTableWidgetItem *netItem = new QTableWidgetItem();
    this->m_ui->addresses->setItem(row, 0, addrItem);
    this->m_ui->addresses->setItem(row, 1, netItem);
    QPushButton *button = new QPushButton(this->m_ui->addresses);
    button->setText(tr("Remove"));
    connect(button, SIGNAL(clicked()), this, SLOT(deleteAddress()));
    this->m_ui->addresses->setCellWidget(row, 3, button);
    QComboBox *box = new QComboBox();
    box->addItem("IPv4");
    box->addItem("IPv6");
    this->m_ui->addresses->setCellWidget(row, 2, box);
    buttons[button] = qMakePair(addrItem, netItem);
    rows[row] = qMakePair(addrItem, netItem);
    types[row] = box;
    return row;
}

void InterfaceEditorWidget::changeEvent(QEvent *e)
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

void InterfaceEditorWidget::nameEdited(QString newname)
{
    tabw->setTabText(tabw->indexOf(this), newname);
}


Interface* InterfaceEditorWidget::getInterface()
{
    return this->interface;
}

EditedInterfaceData InterfaceEditorWidget::getInterfaceData()
{
    EditedInterfaceData res;
    res.name = this->m_ui->name->text();
    res.label = this->m_ui->label->text();
    res.comment = this->m_ui->comment->toPlainText();
    res.type = this->m_ui->type->currentIndex();
    res.protocol = this->m_ui->protocol->currentText();

    res.mac = this->m_ui->mac->text();
    for ( int i = 0; i < this->m_ui->addresses->rowCount(); i++ )
    {
        AddressInfo info;
        info.address = rows[i].first->text();
        info.netmask = rows[i].second->text();
        info.ipv4 = types[i]->currentIndex() == 0;
        res.addresses.insert(fwaddrs[i], info);
    }
    return res;
}

void InterfaceEditorWidget::typeChanged(int type)
{
    if (type != 0)
    {
        while ( this->m_ui->addresses->rowCount() > 0 )
            this->m_ui->addresses->removeRow(0);
        foreach ( QPushButton* btn, buttons.keys() )
            delete btn;
        foreach ( QComboBox* box, types.values() )
            delete box;
        types.clear();
        buttons.clear();
        fwaddrs.clear();
        rows.clear();
        this->m_ui->addresses->setEnabled(false);
        this->m_ui->addAddress->setEnabled(false);
    }
    else
    {
        this->m_ui->addresses->setEnabled(true);
        this->m_ui->addAddress->setEnabled(true);
    }
}

bool InterfaceEditorWidget::isValid()
{
    // type 0 == regular
    if ( (this->m_ui->type->currentIndex() == 0) &&
         (this->m_ui->addresses->rowCount() == 0) )
    {
        QMessageBox::warning(this,"Firewall Builder",
                 tr("You should enter at least one address for "
                    "regular interface %1").arg(this->m_ui->name->text()),
                "&Continue", QString::null, QString::null, 0, 1 );
        return false;
    }
    for (int i = 0; i < this->m_ui->addresses->rowCount(); i++)
    {
        QString address = this->m_ui->addresses->item(i, 0)->text();
        QString netmask = this->m_ui->addresses->item(i, 1)->text();
        if ( !validateAddress(address, netmask, this->m_ui->type->currentIndex() == 0, types[i]->currentIndex() == 1) )
            return false;
    }
    return true;
}

bool InterfaceEditorWidget::validateAddress(const QString &addr,
                                            const QString &netm,
                                            bool regular,
                                            bool ipv6)
{
    if ( regular && ( addr.isEmpty() || netm.isEmpty() ) )
    {
        QMessageBox::warning( this, "Firewall Builder",
                              tr("Empty addrres or netmask field"),
                              "&Continue", QString::null, QString::null, 0, 1);
        return false;
    }

    try
    {
        if (ipv6) InetAddr(AF_INET6, addr.toLatin1().constData());
        else InetAddr(addr.toLatin1().constData());
    }
    catch (FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Illegal address '%1/%2'").arg(addr).arg(netm),
            "&Continue", QString::null, QString::null, 0, 1 );
        return false;
    }
    try
    {
        bool ok = false ;
        int ilen = netm.toInt (&ok);
        if (ok)
        {
            if (ilen < 0 || (!ipv6 && (ilen > 32)) || (ilen>64) )
            {
                QMessageBox::warning(
                    this,"Firewall Builder",
                    tr("Illegal netmask '%1/%2'").arg(addr).arg(netm),
                    "&Continue", QString::null, QString::null, 0, 1 );
                return false;
            }
        }
        else
        {
            if (ipv6) InetAddr(AF_INET6, netm.toLatin1().constData());
            else InetAddr(netm.toLatin1().constData());
        }

    }
    catch (FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Illegal netmask '%1/%2'").arg(addr).arg(netm),
            "&Continue", QString::null, QString::null, 0, 1 );
        return false;
    }
    return true;
}

void InterfaceEditorWidget::resizeEvent ( QResizeEvent * )
{
    int total = this->m_ui->addresses->viewport()->width();
    if (total < 100) total = this->m_ui->addresses->width() * 0.95;
    int controls;
    if ( total/4 > 130 ) controls = 130; else controls = total/4;
    this->m_ui->addresses->setColumnWidth(0, (total - controls*2)/2);
    this->m_ui->addresses->setColumnWidth(1, (total - controls*2)/2);
    this->m_ui->addresses->setColumnWidth(2, controls);
    this->m_ui->addresses->setColumnWidth(3, controls);
}

void InterfaceEditorWidget::addressChanged(int row, int col)
{
    if ( row < 0 || col < 0 || rows.isEmpty() || row > m_ui->addresses->rowCount() || col > 1 ) return;

    if (!rows.keys().contains(row)) return;
    QString address = this->rows[row].first->text();
    QString netmask = this->rows[row].second->text();
    if ( address.isEmpty() || netmask.isEmpty() ) return;
    bool regular = this->m_ui->type->currentIndex() == 0;
    bool ipv6 = this->types[row]->currentIndex() == 1;
    if (!validateAddress(address, netmask, regular, ipv6))
        this->m_ui->addresses->editItem(this->m_ui->addresses->item(row, col));
}

void InterfaceEditorWidget::setProtocolVisible(bool st)
{
    this->m_ui->protocol->setVisible(st);
    this->m_ui->protocolLabel->setVisible(st);
}

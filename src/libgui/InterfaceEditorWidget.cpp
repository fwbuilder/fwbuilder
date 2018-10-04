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

#include "InterfaceEditorWidget.h"
#include "ui_InterfaceEditorWidget.h"
#include "FWObjectPropertiesFactory.h"

#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include <QLineEdit>
#include <QTimer>
#include <QDebug>

using namespace libfwbuilder;
using namespace std;


InterfaceEditorWidget::InterfaceEditorWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditorWidget)
{
    tabw = dynamic_cast<QTabWidget*>(parent);
    this->interfacep = nullptr;
    m_ui->setupUi(this);
    setClusterMode(false);
    this->m_ui->name->setText(""); // blank interface name
    this->m_ui->label->clear();

    this->m_ui->comment->clear();
//    addNewAddress();
}

InterfaceEditorWidget::InterfaceEditorWidget(QWidget *parent, Interface *iface) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditorWidget)
{
    tabw = dynamic_cast<QTabWidget*>(parent);
    this->interfacep = iface;
    m_ui->setupUi(this);
    setClusterMode(false);
    this->m_ui->name->setText(interfacep->getName().c_str());
    this->m_ui->label->setText(interfacep->getLabel().c_str());

//    if (iface->getPhysicalAddress() != nullptr)
//       m_ui->mac->setText(iface->getPhysicalAddress()->getPhysAddress().c_str());

    this->m_ui->comment->setPlainText(iface->getComment().c_str());

    if ( this->interfacep->isDyn() ) this->m_ui->type->setCurrentIndex(1);
    if ( this->interfacep->isUnnumbered() ) this->m_ui->type->setCurrentIndex(2);

    FWObjectTypedChildIterator adriter = iface->findByType(IPv4::TYPENAME);
    for ( ; adriter != adriter.end(); ++adriter )
    {
        Address *addr = Address::cast(*adriter);
        int row = addNewAddress(addr->getAddressPtr()->toString().c_str(),
                                addr->getNetmaskPtr()->toString().c_str(),
                                addr->getAddressPtr()->isV4());
        fwaddrs[row] = addr;
    }
    FWObjectTypedChildIterator adriter2 = iface->findByType(IPv6::TYPENAME);
    for ( ; adriter2 != adriter2.end(); ++adriter2 )
    {
        Address *addr = Address::cast(*adriter2);
        int row = addNewAddress(addr->getAddressPtr()->toString().c_str(),
                                addr->getNetmaskPtr()->toString().c_str(),
                                addr->getAddressPtr()->isV4());
        fwaddrs[row] = addr;
    }
}


InterfaceEditorWidget::InterfaceEditorWidget(QWidget *parent,
                                             ClusterInterfaceData data) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditorWidget)
{
    clusterMode = true;
    tabw = dynamic_cast<QTabWidget*>(parent);
    m_ui->setupUi(this);
    this->interfacep = nullptr;
    this->m_ui->name->setText(data.name);
    this->m_ui->label->setText(data.label);

    this->m_ui->comment->setText(data.comment);

    setHostOS(data.os);

    list<QStringPair> types;
    getFailoverTypesForOS(os, types);
    QStringList typenames;
    QString lastProtocol = st->getNewClusterFailoverProtocol();
    int toSelect = 0;
    foreach(QStringPair pair, types)
    {
        typenames << pair.second;
        if (pair.second == lastProtocol)
            toSelect = typenames.count() - 1;
    }
    this->m_ui->protocol->clear();
    this->m_ui->protocol->insertItems(0, typenames);
    this->m_ui->protocol->setCurrentIndex(toSelect);
    this->protocolChanged(this->m_ui->protocol->currentText());
}

void InterfaceEditorWidget::setData(InterfaceData *data)
{
    this->m_ui->name->setText(data->name.c_str());
    this->m_ui->label->setText(data->label.c_str());

//    this->m_ui->mac->setText(data->mac_addr.c_str());

    this->m_ui->comment->clear();

    if ( data->isDyn ) this->m_ui->type->setCurrentIndex(1);
    else
        if ( data->isUnnumbered ) this->m_ui->type->setCurrentIndex(2);
        else this->m_ui->type->setCurrentIndex(0);

    foreach(QPushButton *btn, this->buttons.keys())
        btn->click();

    if ( !data->isDyn && !data->isUnnumbered )
    {
        foreach( InetAddrMask* addr, data->addr_mask )
        {
            addNewAddress(addr->getAddressPtr()->toString().c_str(),
                          addr->getNetmaskPtr()->toString().c_str(),
                          !addr->getAddressPtr()->isV6());
        }
    }
}

void InterfaceEditorWidget::deleteAddress()
{
    QPushButton *button = dynamic_cast<QPushButton*>(sender());
    QPair<QTableWidgetItem*, QTableWidgetItem*> items = buttons[button];
    buttons.remove(button);
    int row = items.first->row();
    this->m_ui->addresses->removeRow(row);
    delete types[row];
    // shift elements up in these maps
    int idx = row;
    for (; idx < (rows.size() - 1); idx++)
    {
        types[row] = types[row+1];
        rows[row] = rows[row+1];
        fwaddrs[row] = fwaddrs[row+1];
    }
    types.remove(idx);
    rows.remove(idx);
    fwaddrs.remove(idx);
    this->addressChanged(-1, -1);
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
    fwaddrs[row] = nullptr;
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


int InterfaceEditorWidget::addNewAddress(QString address, QString netmask, bool ipv4)
{
    int row = addNewAddress();
    types[row]->setCurrentIndex(!ipv4);
    rows[row].first->setText(address);
    rows[row].second->setText(netmask);
    return row;
}

void InterfaceEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
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
    return this->interfacep;
}

EditedInterfaceData InterfaceEditorWidget::getInterfaceData()
{
    EditedInterfaceData res;
    res.name = this->m_ui->name->text();
    res.label = this->m_ui->label->text();
    res.comment = this->m_ui->comment->toPlainText();
    res.type = this->m_ui->type->currentIndex();
    res.protocol = this->m_ui->protocol->currentText();

//    res.mac = this->m_ui->mac->text();

    bool noAddrs = false;
    // if (clusterMode)
    //     noAddrs = Resources::os_res[os.toStdString()]->getResourceBool(
    //                 "/FWBuilderResources/Target/protocols/"
    //                 + this->m_ui->protocol->currentText().toLower().toStdString() + "/no_ip_ok");
    if (this->m_ui->protocol->currentText() == "None") noAddrs = true;
    if (!noAddrs)
        for ( int i = 0; i < this->m_ui->addresses->rowCount(); i++ )
        {
            AddressInfo info;
            if (rows[i].first == nullptr) continue; // deleted row
            info.address = rows[i].first->text();
            info.netmask = rows[i].second->text();
            info.ipv4 = types[i]->currentIndex() == 0;
            res.addresses.insert(fwaddrs[i], info);
        }
    return res;
}

void InterfaceEditorWidget::typeChanged(int type)
{
    if (clusterMode) return;
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
#if 0
    // do not do this check in the wizard because there are too many
    // combinations: most protocols can work with and without an
    // address on the cluster interfaces and only one (VRRP) requires
    // it. Unfortunately attribute in the OS resource file only tells
    // when it is ok to have no address, but does not tell when it must
    // be there. And we do this check in the compiler anyway.  Tcikets
    // #1180, #1172
    
    bool no_addr_ok = true;
    if (clusterMode)
    {
        no_addr_ok = Resources::os_res[os.toStdString()]->getResourceBool(
                        "/FWBuilderResources/Target/protocols/"
                        + this->m_ui->protocol->currentText().toLower().toStdString() + "/no_ip_ok")
                     || this->m_ui->protocol->currentText() == "None";
    }

    if (clusterMode && no_addr_ok && this->m_ui->addresses->rowCount() != 0)
    {
        QMessageBox::warning(
            this, "Firewall Builder",
            tr("Failover protocol %1 does not require IP address for interface %2")
            .arg(this->m_ui->protocol->currentText())
            .arg(this->m_ui->name->text()),
            "&Continue", QString::null, QString::null, 0, 1 );
        return false;
    }

    if (!no_addr_ok && this->m_ui->addresses->rowCount() == 0)
    {
        if ( (this->m_ui->type->currentIndex() == 0) &&
             (this->m_ui->addresses->rowCount() == 0) )
        {
            QMessageBox::warning(
                this, "Firewall Builder",
                tr("Failover protocol %1 requires an IP address for interface %2")
                .arg(this->m_ui->protocol->currentText())
                .arg(this->m_ui->name->text()),
                "&Continue", QString::null, QString::null, 0, 1 );
            return false;
        }
    }

#endif

    if (this->m_ui->name->text().isEmpty())
    {
        setError("Firewall Builder",
                 tr("<html>Interface name can not be blank."
                 "<br/> "
                 "<br/>"
                 "Interface name must match the name of the physical interface, "
                 "such as 'eth0', 'fxp0', 'ethernet0', etc</html>"));
        return false;
    }

    for (int i = 0; i < this->m_ui->addresses->rowCount(); i++)
    {
        if (types[i] == nullptr) continue; // deleted row
        QString address = this->m_ui->addresses->item(i, 0)->text();
        QString netmask = this->m_ui->addresses->item(i, 1)->text();
        if ( !validateAddress(
                 address, netmask,
                 this->m_ui->type->currentIndex() == 0,
                 types[i]->currentIndex() == 1) )
        {
            return false;
        }
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
        setError("Firewall Builder", tr("Empty address or netmask field"));
        return false;
    }

    try
    {
        if (ipv6) InetAddr(AF_INET6, addr.toLatin1().constData());
        else InetAddr(addr.toLatin1().constData());
    }
    catch (FWException &ex)
    {
        setError("Firewall Builder",
                 tr("Invalid address '%1/%2'").arg(addr).arg(netm));
        return false;
    }
    try
    {
        bool ok = false ;
        int ilen = netm.toInt (&ok);
        if (ok)
        {
            if (ilen < 0 || (ipv6 && ilen > 128) || (!ipv6 && ilen > 32))
            {
                setError("Firewall Builder",
                         tr("Invalid netmask '%1/%2'").arg(addr).arg(netm));
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
        setError("Firewall Builder",
                 tr("Invalid netmask '%1/%2'").arg(addr).arg(netm));
        return false;
    }
    return true;
}

void InterfaceEditorWidget::resizeEvent ( QResizeEvent * )
{
    int total = this->m_ui->addresses->viewport()->width();
    if (total < 100) total = int(this->m_ui->addresses->width() * 0.95);
    int controls;
    if ( total/4 > 130 ) controls = 130; else controls = total/4;
    this->m_ui->addresses->setColumnWidth(0, (total - controls*2)/2);
    this->m_ui->addresses->setColumnWidth(1, (total - controls*2)/2);
    this->m_ui->addresses->setColumnWidth(2, controls);
    this->m_ui->addresses->setColumnWidth(3, controls);
}

void InterfaceEditorWidget::addressChanged(int row, int col)
{
    // if (m_ui->addresses->rowCount() >= 1)
    //     m_ui->addAddress->setText(tr("Add another address"));
    // else
        m_ui->addAddress->setText(tr("Add address"));

    if ( row < 0 || col < 0 ||
         rows.isEmpty() || row > m_ui->addresses->rowCount() || col > 1 ) return;

    if (!rows.keys().contains(row)) return;
    QString address = this->rows[row].first->text();
    QString netmask = this->rows[row].second->text();
    if ( address.isEmpty() || netmask.isEmpty() ) return;
    bool regular = this->m_ui->type->currentIndex() == 0;
    bool ipv6 = this->types[row]->currentIndex() == 1;
    if (!validateAddress(address, netmask, regular, ipv6))
        this->m_ui->addresses->editItem(this->m_ui->addresses->item(row, col));
}

void InterfaceEditorWidget::setClusterMode(bool st)
{
    clusterMode = st;
    this->m_ui->name->setEnabled(!st);
    this->m_ui->protocol->setVisible(st);
    this->m_ui->protocolLabel->setVisible(st);
//    this->m_ui->mac->setVisible(!st);
//    this->m_ui->macLabel->setVisible(!st);
    this->m_ui->type->setVisible(!st);
    this->m_ui->typeLabel->setVisible(!st);
}

void InterfaceEditorWidget::protocolChanged(QString name)
{
    if (clusterMode)
    {
        bool noaddr = (name == "None");
        if (noaddr)
            while ( this->m_ui->addresses->rowCount() )
                this->m_ui->addresses->removeRow(0);

        this->m_ui->addresses->setEnabled(!noaddr);
        this->m_ui->addAddress->setEnabled(!noaddr);
        st->setNewClusterFailoverProtocol(name);
    }

}

void InterfaceEditorWidget::setExplanation(const QString& text)
{
    this->m_ui->explanation->setText(text);
    this->m_ui->explanation->setFont(QApplication::font());
}

void InterfaceEditorWidget::setProtocolIndex(int idx)
{
    this->m_ui->protocol->setCurrentIndex(idx);
}

void InterfaceEditorWidget::setHostOS(const QString &s)
{
    os = s;

    QString name_prompt =
        FWObjectPropertiesFactory::getInterfaceNameExamplesForHostOS(os);

    if (fwbdebug)
        qDebug() << "Interface name prompt:" << name_prompt;

    this->m_ui->name->setPlaceholderText(name_prompt);
    this->m_ui->label->setPlaceholderText("outside, inside, etc (optional)");

    this->m_ui->name->setToolTip(name_prompt);
    this->m_ui->label->setToolTip("outside, inside, etc (optional)");
}

void InterfaceEditorWidget::setError(const QString &title,
                                     const QString &text)
{
    errorTitle = title;
    errorText = text;
}

void InterfaceEditorWidget::showError()
{
    QMessageBox::warning(this, errorTitle, errorText, "&Continue",
                         QString::null, QString::null, 0, 1);
}

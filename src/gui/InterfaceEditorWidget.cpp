#include "InterfaceEditorWidget.h"
#include "ui_InterfaceEditorWidget.h"
#include "fwbuilder/IPv4.h"

InterfaceEditorWidget::InterfaceEditorWidget(QWidget *parent, libfwbuilder::Interface *interface) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditorWidget)
{
    tabw = dynamic_cast<QTabWidget*>(parent);
    this->interface = interface;
    m_ui->setupUi(this);
    this->m_ui->name->setText(interface->getName().c_str());
    this->m_ui->label->setText(interface->getLabel().c_str());
    if (interface->getPhysicalAddress() != NULL)
       m_ui->mac->setText(interface->getPhysicalAddress()->getPhysAddress().c_str());
    this->m_ui->comment->setPlainText(interface->getComment().c_str());
    if ( this->interface->isDyn() )
        this->m_ui->type->setCurrentIndex(1);
    if ( this->interface->isUnnumbered() )
        this->m_ui->type->setCurrentIndex(2);
    libfwbuilder::FWObjectTypedChildIterator adriter = interface->findByType(libfwbuilder::IPv4::TYPENAME);
    for ( ; adriter != adriter.end(); ++adriter )
    {
        libfwbuilder::Address *addr = libfwbuilder::Address::cast(*adriter);
        int row = addNewAddress();
        fwaddrs[row] = addr;
        rows[row].first->setText(addr->getAddressPtr()->toString().c_str());
        rows[row].second->setText(addr->getNetmaskPtr()->toString().c_str());
    }
}

InterfaceEditorWidget::InterfaceEditorWidget(QWidget *parent, libfwbuilder::InterfaceData* data)
{
    tabw = dynamic_cast<QTabWidget*>(parent);
    this->interface = NULL;
    m_ui->setupUi(this);
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
    if ( !data->isDyn && !data->isUnnumbered )
    {
        foreach( libfwbuilder::InetAddrMask* addr, data->addr_mask )
        {
            int row = addNewAddress();
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


libfwbuilder::Interface* InterfaceEditorWidget::getInterface()
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
        if (types[i]->currentIndex() != 0) continue;
        QString address = this->m_ui->addresses->item(i, 0)->text();
        QString netmask = this->m_ui->addresses->item(i, 1)->text();
        if ( !validateAddress(address, netmask, types[i]->currentIndex() == 0) )
            return false;
    }
    return true;
}

bool InterfaceEditorWidget::validateAddress(const QString &addr,
                                            const QString &netm,
                                            bool regular)
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
        libfwbuilder::InetAddr(addr.toLatin1().constData());
    }
    catch (libfwbuilder::FWException &ex)
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
            if (ilen < 0 || ilen > 32)
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
            libfwbuilder::InetAddr(netm.toLatin1().constData());
        }

    }
    catch (libfwbuilder::FWException &ex)
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
    int controls;
    if ( total/4 > 130 ) controls = 130; else controls = total/4;
    this->m_ui->addresses->setColumnWidth(0, (total - controls*2)/2);
    this->m_ui->addresses->setColumnWidth(1, (total - controls*2)/2);
    this->m_ui->addresses->setColumnWidth(2, controls);
    this->m_ui->addresses->setColumnWidth(3, controls);
}

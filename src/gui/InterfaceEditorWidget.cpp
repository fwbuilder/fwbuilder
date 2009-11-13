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
    this->m_ui->comment->setPlainText(interface->getComment().c_str());
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

InterfaceEditorWidget::InterfaceEditorWidget(QWidget *parent, libfwbuilder::FWObjectDatabase* db) :
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
    for ( int i = 0; i < this->m_ui->addresses->rowCount(); i++ )
    {
        AddressInfo info;
        info.address = rows[i].first->text();
        info.netmask = rows[i].second->text();
        info.ipv4 = types[i]->currentIndex() == 0;
        res.addresses[fwaddrs[i]] = info;
    }
    return res;
}

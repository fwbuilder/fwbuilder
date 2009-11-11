#include "AddressEditor.h"
#include "ui_AddressEditor.h"
#include "fwbuilder/IPv4.h"

#include <QDebug>

AddressEditor::AddressEditor(libfwbuilder::Address *address, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::AddressEditor)
{
    this->address = address;
    m_ui->setupUi(this);
    this->m_ui->address->setText(address->getAddressPtr()->toString().c_str());
    this->m_ui->netmask->setText(address->getNetmaskPtr()->toString().c_str());
}

AddressEditor::AddressEditor(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::AddressEditor)
{
    this->address = NULL;
    m_ui->setupUi(this);
}

AddressEditor::~AddressEditor()
{
    delete m_ui;
}

void AddressEditor::changeEvent(QEvent *e)
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

AddressInfo AddressEditor::getEditedData()
{
    AddressInfo info;
    info.address = this->m_ui->address->text();
    info.netmask = this->m_ui->netmask->text();
    info.ipv4 = this->m_ui->ipv4->isChecked();
    return info;
}

libfwbuilder::Address* AddressEditor::getAddress()
{
    return this->address;
}

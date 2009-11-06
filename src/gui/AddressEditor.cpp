#include "AddressEditor.h"
#include "ui_AddressEditor.h"

AddressEditor::AddressEditor(QWidget *parent, libfwbuilder::Address *address) :
    QWidget(parent),
    m_ui(new Ui::AddressEditor)
{
    this->address = address;
    m_ui->setupUi(this);
    this->m_ui->address->setText(address->getAddressPtr()->toString().c_str());
    this->m_ui->netmask->setText(address->getNetmaskPtr()->toString().c_str());
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

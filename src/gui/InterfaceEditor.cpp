#include "InterfaceEditor.h"
#include "ui_InterfaceEditor.h"
#include "fwbuilder/IPv4.h"

#include <QDebug>
InterfaceEditor::InterfaceEditor(QWidget *parent, libfwbuilder::Interface *interface) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditor)
{
    this->interface = interface;
    m_ui->setupUi(this);
    this->m_ui->name->setText(interface->getName().c_str());
    this->m_ui->label->setText(interface->getLabel().c_str());
    this->m_ui->comment->setText(interface->getComment().c_str());
    while ( this->m_ui->tabWidget->count() ) this->m_ui->tabWidget->removeTab(0);
    libfwbuilder::FWObjectTypedChildIterator adriter = interface->findByType(libfwbuilder::IPv4::TYPENAME);
    for ( ; adriter != adriter.end(); ++adriter )
    {
        this->m_ui->tabWidget->addTab(new AddressEditor(this, libfwbuilder::Address::cast(*adriter)), libfwbuilder::Address::cast(*adriter)->getName().c_str());
    }
}

InterfaceEditor::~InterfaceEditor()
{
    delete m_ui;
}

void InterfaceEditor::changeEvent(QEvent *e)
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

void InterfaceEditor::nameEdited(QString newname)
{
    interface->setName(newname.toStdString());
}

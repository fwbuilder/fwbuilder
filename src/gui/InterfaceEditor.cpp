#include "InterfaceEditor.h"
#include "ui_InterfaceEditor.h"
#include "fwbuilder/IPv4.h"

InterfaceEditor::InterfaceEditor(QWidget *parent, libfwbuilder::Interface *interface) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditor)
{
    tabw = dynamic_cast<QTabWidget*>(parent);
    this->interface = interface;
    m_ui->setupUi(this);
    setupUI();
    this->m_ui->name->setText(interface->getName().c_str());
    this->m_ui->label->setText(interface->getLabel().c_str());
    this->m_ui->comment->setPlainText(interface->getComment().c_str());
    while ( this->m_ui->tabWidget->count() ) this->m_ui->tabWidget->removeTab(0);
    libfwbuilder::FWObjectTypedChildIterator adriter = interface->findByType(libfwbuilder::IPv4::TYPENAME);
    for ( ; adriter != adriter.end(); ++adriter )
        this->m_ui->tabWidget->addTab(new AddressEditor(libfwbuilder::Address::cast(*adriter), this),
                                      libfwbuilder::Address::cast(*adriter)->getName().c_str());
}

InterfaceEditor::InterfaceEditor(QWidget *parent, libfwbuilder::FWObjectDatabase* db) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditor)
{
    tabw = dynamic_cast<QTabWidget*>(parent);
    this->interface = NULL;
    m_ui->setupUi(this);
    setupUI();
    this->m_ui->name->setText(tr("New interface"));
    this->m_ui->label->clear();
    this->m_ui->comment->clear();
    while ( this->m_ui->tabWidget->count() ) this->m_ui->tabWidget->removeTab(0);
    addNewAddress();
}

void InterfaceEditor::setupUI()
{
    addAddr = new QToolButton(this);
    addAddr->setIcon(QIcon(":/Icons/AddressTable/icon"));
    addAddr->setToolTip(tr("Add new address"));
    connect(addAddr, SIGNAL(clicked()), this, SLOT(addNewAddress()));
    this->m_ui->tabWidget->setCornerWidget(addAddr, Qt::TopRightCorner);
}

InterfaceEditor::~InterfaceEditor()
{
    delete m_ui;
    delete addAddr;
}

void InterfaceEditor::addNewAddress()
{
    this->m_ui->tabWidget->addTab(new AddressEditor(this), "ip");
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
    tabw->setTabText(tabw->indexOf(this), newname);
}

libfwbuilder::Interface* InterfaceEditor::getInterface()
{
    return this->interface;
}

EditedInterfaceData InterfaceEditor::getInterfaceData()
{
    EditedInterfaceData res;
    res.name = this->m_ui->name->text();
    res.label = this->m_ui->label->text();
    res.comment = this->m_ui->comment->toPlainText();
    for ( int i = 0; i < this->m_ui->tabWidget->count(); i++ )
    {
        AddressEditor *addr = dynamic_cast<AddressEditor*>(this->m_ui->tabWidget->widget(i));
        res.addresses[addr->getAddress()] = addr->getEditedData();
    }
    return res;
}

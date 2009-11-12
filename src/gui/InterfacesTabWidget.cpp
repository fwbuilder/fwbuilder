#include "InterfacesTabWidget.h"
#include "ui_InterfacesTabWidget.h"

InterfacesTabWidget::InterfacesTabWidget(QWidget *parent) :
    QTabWidget(parent),
    m_ui(new Ui::InterfacesTabWidget)
{
    m_ui->setupUi(this);
    clear();
    newInterface = new QToolButton();
    delInterface = new QToolButton();
    connect(newInterface, SIGNAL(clicked()), this, SLOT(addNewInterface()));
    connect(delInterface, SIGNAL(clicked()), this, SLOT(closeTab()));
    newInterface->setIcon(QIcon(":/Icons/add.png"));
    delInterface->setIcon(QIcon(":/Icons/del.png"));
    newInterface->setToolTip(tr("Add new interface"));
    delInterface->setToolTip(tr("Delete current interface"));
    this->setCornerWidget(delInterface, Qt::TopRightCorner);
    this->setCornerWidget(newInterface, Qt::TopLeftCorner);
    this->cornerWidget(Qt::TopRightCorner)->show();
    this->cornerWidget(Qt::TopLeftCorner)->show();
}

InterfacesTabWidget::~InterfacesTabWidget()
{
    delete m_ui;
    delete newInterface;
    delete delInterface;
}

void InterfacesTabWidget::changeEvent(QEvent *e)
{
    QTabWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QMap<libfwbuilder::Interface*, EditedInterfaceData> InterfacesTabWidget::getData()
{
    QMap<libfwbuilder::Interface*, EditedInterfaceData> res;
    for ( int i = 0; i < this->count(); i++ )
    {
        InterfaceEditorWidget *intEditor = dynamic_cast<InterfaceEditorWidget*>(this->widget(i));
        res[intEditor->getInterface()] = intEditor->getInterfaceData();
    }
    return res;
}

void InterfacesTabWidget::addInterface(libfwbuilder::Interface *interface)
{
    this->addTab(new InterfaceEditorWidget(this, interface), interface->getName().c_str());
}

void InterfacesTabWidget::addNewInterface()
{
    libfwbuilder::FWObjectDatabase *root = libfwbuilder::Interface::cast(*(currentTemplate->findByType(libfwbuilder::Interface::TYPENAME)))->getRoot();
    addTab(new InterfaceEditorWidget(this, root), tr("New interface"));
    setCurrentIndex(count() - 1);
}

void InterfacesTabWidget::setTemplate(libfwbuilder::FWObject* obj)
{
    currentTemplate = obj;
}

void InterfacesTabWidget::clear()
{
    while ( this->count() ) this->removeTab(0);
}

void InterfacesTabWidget::closeTab()
{
    int idx = this->currentIndex();
    QWidget *w = this->widget(idx);
    this->removeTab(idx);
    delete w;
}

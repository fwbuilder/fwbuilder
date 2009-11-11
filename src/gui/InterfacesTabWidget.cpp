#include "InterfacesTabWidget.h"
#include "ui_InterfacesTabWidget.h"

InterfacesTabWidget::InterfacesTabWidget(QWidget *parent) :
    QTabWidget(parent),
    m_ui(new Ui::InterfacesTabWidget)
{
    m_ui->setupUi(this);
    clear();
    newInterface = new QToolButton();
    connect(newInterface, SIGNAL(clicked()), this, SLOT(addNewInterface()));
    newInterface->setIcon(QIcon(":/Icons/Interface/icon"));
    newInterface->setToolTip(tr("Add new interface"));
    this->setCornerWidget(newInterface, Qt::TopRightCorner);
    this->cornerWidget(Qt::TopRightCorner)->show();
}

InterfacesTabWidget::~InterfacesTabWidget()
{
    delete m_ui;
    delete newInterface;
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
}

void InterfacesTabWidget::setTemplate(libfwbuilder::FWObject* obj)
{
    currentTemplate = obj;
}

void InterfacesTabWidget::clear()
{
    while ( this->count() ) this->removeTab(0);
}

void InterfacesTabWidget::closeTab(int idx)
{
    QWidget *w = this->widget(idx);
    this->removeTab(idx);
    delete w;
}

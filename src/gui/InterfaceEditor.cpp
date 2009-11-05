#include "InterfaceEditor.h"
#include "ui_InterfaceEditor.h"

InterfaceEditor::InterfaceEditor(QWidget *parent, libfwbuilder::Interface *interface) :
    QWidget(parent),
    m_ui(new Ui::InterfaceEditor)
{
    this->interface = interface;
    m_ui->setupUi(this);
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

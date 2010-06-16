#include "WorkflowIcons.h"
#include "ClickableLabel.h"
#include "ui_WorkflowIcons.h"

WorkflowIcons::WorkflowIcons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkflowIcons_q)
{
    ui->setupUi(this);
}

WorkflowIcons::~WorkflowIcons()
{
    delete ui;
}

void WorkflowIcons::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void WorkflowIcons::itemClicked()
{
    ClickableLabel *item = dynamic_cast<ClickableLabel*>(sender());
    if (item->objectName() == "newFirewall")
        emit newFirewall();
    else if (item->objectName() == "importConfig")
        emit importConfig();
    else if (item->objectName() == "watchTutorial")
        emit watchTutorial("getting_started");
}

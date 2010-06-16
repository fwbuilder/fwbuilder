#include "WorkflowIcons.h"
#include "ClickableLabel.h"
#include "ui_WorkflowIcons.h"
#include "ProjectPanel.h"
#include "FWWindow.h"
#include "ObjectManipulator.h"
#include <fwbuilder/Firewall.h>
#include "global.h"
#include <QDebug>

WorkflowIcons::WorkflowIcons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkflowIcons_q)
{
    ui->setupUi(this);

}

void WorkflowIcons::setUpSignals(QWidget *panel)
{
    ObjectManipulator *om = panel->findChild<ObjectManipulator*>();
    QAction *newFirewall = om->findChild<QAction*>(QString("newObject_") + libfwbuilder::Firewall::TYPENAME);
    connect(ui->newFirewall, SIGNAL(clicked()), newFirewall, SLOT(trigger()));

    QAction *import = panel->parent()->parent()->parent()->findChild<QAction*>("policyImportAction");
    connect(ui->importConfig, SIGNAL(clicked()), import, SLOT(trigger()));

    connect(ui->action_getting_started, SIGNAL(clicked()), panel->parent()->parent()->parent(), SLOT(showTutorial()));
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

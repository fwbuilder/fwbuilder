/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: WorkflowIcons.cpp® 2786 2010-04-01 14:05:36Z a2k $

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include "global.h"

#include "ClickableLabel.h"
#include "FWWindow.h"
#include "ObjectManipulator.h"
#include "ProjectPanel.h"
#include "WorkflowIcons.h"
#include "ui_WorkflowIcons.h"

#include <fwbuilder/Firewall.h>

#include <QDebug>
#include <QDesktopServices>
#include <QUrl>


WorkflowIcons::WorkflowIcons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkflowIcons_q)
{
    ui->setupUi(this);
}

void WorkflowIcons::setUpSignals(QWidget *panel)
{
    ObjectManipulator *om = panel->findChild<ObjectManipulator*>();
    connect(ui->newFirewall, SIGNAL(clicked()), om, SLOT(newFirewallSlot()));

    // global variable mw is null when this is running
    QObject *mainWindow = dynamic_cast<ProjectPanel*>(panel)->getWindow();
    QAction *import = mainWindow->findChild<QAction*>("policyImportAction");
    connect(ui->importConfig, SIGNAL(clicked()), import, SLOT(trigger()));
    connect(om, SIGNAL(libraryAccessChanged(bool)), this, SLOT(libraryAccessChanged(bool)));

    connect(ui->action_getting_started, SIGNAL(clicked()),
            this, SLOT(openTutorial()));
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

void WorkflowIcons::libraryAccessChanged(bool writable)
{
    ui->newFirewall->setEnabled(writable);
    ui->importConfig->setEnabled(writable);
}

void WorkflowIcons::openTutorial()
{
    // if we want to show tutorial included with the package:
    //    mw->showTutorial("getting_started");

    // if we want to open the page with video tutorials in the standard browser
    QDesktopServices::openUrl(QUrl("http://www.fwbuilder.org/4.0/videos.html",
                                   QUrl::StrictMode));
}

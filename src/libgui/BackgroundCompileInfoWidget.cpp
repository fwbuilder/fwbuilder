/*

                          Firewall Builder

                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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

#include <QLabel>
#include <QStatusBar>
#include <QProgressBar>

#include "instDialog.h"
#include "FWWindow.h"
#include "BackgroundCompileInfoWidget.h"

BackgroundCompileInfoWidget::BackgroundCompileInfoWidget(QWidget *parent, instDialog *instDialog, FWWindow *fwwindow) :
    QWidget(parent), m_fwwindow(fwwindow)
{
    connect(instDialog, SIGNAL(autoCompileDone()), this, SLOT(autoCompileIsDone()));

    m_label = new QLabel(tr("Compiling firewalls in background: "));
    m_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_progressBar = new QProgressBar();
    m_progressBar->setMaximumWidth(100);
    m_progressBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QHBoxLayout *hboxlayout = new QHBoxLayout();
    hboxlayout->setSpacing(0);
    hboxlayout->setMargin(0);
    hboxlayout->addWidget(m_label);
    hboxlayout->addWidget(m_progressBar);
    setLayout(hboxlayout);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(instDialog, SIGNAL(currentFirewallsBarMaximumValueChanged(int)), m_progressBar, SLOT(setMaximum(int)));
    connect(instDialog, SIGNAL(currentFirewallsBarvalueChanged(int)), m_progressBar, SLOT(setValue(int)));

    m_fwwindow->statusBar()->addPermanentWidget(this);
}

void BackgroundCompileInfoWidget::autoCompileIsDone()
{
    m_fwwindow->statusBar()->removeWidget(this);
    deleteLater();
}

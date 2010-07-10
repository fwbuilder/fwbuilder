/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id$

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

#include "FirewallCodeViewer.h"
#include "ui_FirewallCodeViewer.h"
#include <QFile>
#include <QDebug>
#include <QTextBrowser>

FirewallCodeViewer::FirewallCodeViewer(QStringList files, QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirewallCodeViewer_q)
{
    ui->setupUi(this);
    this->files = files;
    foreach(QString file, files)
        ui->fileSelector->addItem(file.split("/").last());
    fileSelected(0);
    ui->path->setText(name);
}

FirewallCodeViewer::~FirewallCodeViewer()
{
    delete ui;
}

void FirewallCodeViewer::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FirewallCodeViewer::fileSelected(int idx)
{
    if (pages.contains(idx))
    {
        ui->pages->setCurrentIndex(pages[idx]);
    }
    else
    {
        QFile f(this->files.at(idx));
        f.open(QFile::ReadOnly);
        QTextBrowser *browser = new QTextBrowser(ui->pages);
        browser->setPlainText(f.readAll());
        ui->pages->addWidget(browser);
        pages.insert(idx, ui->pages->indexOf(browser));
        ui->pages->setCurrentWidget(browser);
    }
}

void FirewallCodeViewer::hideCloseButton()
{
    ui->closeButton->hide();
}

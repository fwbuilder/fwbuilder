/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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

#include "QFileInfo"
#include "global.h"
#include "FWBSettings.h"
#include "QProcess"
#include "QMessageBox"
#include "PrefsDialog.h"
#include "ProjectPanel.h"

#include <QMenu>
#include <QDir>
#include <QFileDialog>

FirewallCodeViewer::FirewallCodeViewer(QStringList files, QString name, ProjectPanel *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirewallCodeViewer_q),
    m_project(project)
{
    ui->setupUi(this);
    this->files = files;
    foreach(QString file, files)
        ui->fileSelector->addItem(file.split("/").last());
    fileSelected(0);
    ui->path->setText(name);

    QMenu *menu = new QMenu();
    QAction *showDiff = new QAction(tr("Show diff"), this);
    QAction *showDiffSelectFile = new QAction(tr("Show diff with custom file..."), this);
    menu->addAction(showDiff);
    menu->addAction(showDiffSelectFile);
    ui->diffBtn->setMenu(menu);
    ui->diffBtn->setDefaultAction(showDiff);

    connect(showDiff, SIGNAL(triggered()), this, SLOT(showDiff()));
    connect(showDiffSelectFile, SIGNAL(triggered()), this, SLOT(showDiffSelectFile()));
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

void FirewallCodeViewer::showDiff(const QString &sourceFileName, const QString &destinationFileName)
{
    if (st->getDiffPath().isEmpty()) {
        int ret = QMessageBox::warning(this,
                             tr("Could not start diff program"),
                             tr("You have not configured an external diff program yet.\n"
                                "Do you want to do it now?"),
                             QMessageBox::Yes,
                             QMessageBox::No
                             );
        if (ret == QMessageBox::Yes) {
            PrefsDialog pd(this);
            pd.selectTab("Diff");
            pd.exec();
        }
        return;
    }

    if (!QFileInfo(destinationFileName).isFile()) return;

    QStringList args;

    if (!QFileInfo(sourceFileName).isFile()) {
        QString newSourceFileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Select source file for diff..."),
                    st->getOpenFileDir());

        if (!QFileInfo(newSourceFileName).isFile()) return;

        args << newSourceFileName;
    } else {
        args << sourceFileName;
    }

    args << destinationFileName;

    QProcess *process = new QProcess();
    connect(process, SIGNAL(finished(int)), process, SLOT(deleteLater()));
    connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    process->start(st->getDiffPath(), args);
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

void FirewallCodeViewer::processError(QProcess::ProcessError error)
{
    if (error == QProcess::FailedToStart) {
        int ret = QMessageBox::warning(this,
                             tr("Could not start diff program"),
                             tr("Could not start the configured diff program.\n"
                                "Do you want to check the preferences?"),
                             QMessageBox::Yes,
                             QMessageBox::No
                             );
        if (ret == QMessageBox::Yes) {
            PrefsDialog pd(this);
            pd.selectTab("Diff");
            pd.exec();
        }
    }
}

void FirewallCodeViewer::hideCloseButton()
{
    ui->closeButton->hide();
}

void FirewallCodeViewer::showDiff()
{
    QString fileName = this->files.at(ui->fileSelector->currentIndex());
    QString autoCompiledFileName;

    QDir tempDir(m_project->getTemporaryDirPath());

    if (tempDir.exists())
        autoCompiledFileName = QString(tempDir.absolutePath()).append("/").append(fileName.split("/").last());

    showDiff(autoCompiledFileName, fileName);
}

void FirewallCodeViewer::showDiffSelectFile()
{
    showDiff(this->files.at(ui->fileSelector->currentIndex()), "");
}

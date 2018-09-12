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

#include "global.h"

#include "TutorialDialog.h"
#include "ui_TutorialDialog.h"

#include <QDebug>
#include <QFile>
#include "FWBApplication.h"

TutorialDialog * TutorialDialog::dialog = nullptr;

TutorialDialog::TutorialDialog(QString tutorial, QWidget *parent) :
    QDialog(nullptr),
    ui(new Ui::TutorialDialog_q)
{
    Q_UNUSED(parent)

    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    ui->contents->setOpenExternalLinks(true);

    dialog = this;
    this->initializeTutorial(tutorial);
}

void TutorialDialog::showTutorial(QString tutorial)
{
    if (dialog != nullptr)
    {
        dialog->initializeTutorial(tutorial);
        dialog->showNormal();
        dialog->raise();
    }
    else
    {
        (new TutorialDialog(tutorial))->show();
    }
}

void TutorialDialog::initializeTutorial(QString tutorial)
{
    this->tutorial = tutorial;
    doc = new QTextDocument(this);

    QString stylefile = QString(":/Tutorial/") + this->tutorial + "/stylesheets/style.css";
    QFile f(stylefile);
    if (f.exists())
    {
        f.open(QFile::ReadOnly);
        css_stylesheet = f.readAll();
        doc->setDefaultStyleSheet(css_stylesheet);
        //ui->contents->setStyleSheet(stylesheet);
    }
    ui->contents->setDocument(doc);
    currentPage = 0;
    showPage(currentPage);

}

TutorialDialog::~TutorialDialog()
{
    delete ui;
}

void TutorialDialog::changeEvent(QEvent *e)
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

void TutorialDialog::next()
{
    currentPage++;
    showPage(currentPage);
}

void TutorialDialog::previous()
{
    currentPage--;
    showPage(currentPage);
}

void TutorialDialog::reset()
{
    currentPage = 0;
    showPage(currentPage);
}

void TutorialDialog::showPage(int page)
{
    QString filename = QString(":/Tutorial/") + this->tutorial + "/html/page" + QString::number(page) + ".html";
    if (fwbdebug) qDebug() << filename;
    QFile src(filename);
    src.open(QFile::ReadOnly);
    QString text = src.readAll();
    doc->setHtml(text);

    ui->contents->scrollToAnchor("top");

    bool nextPageExists = 
        QFile::exists(QString(":/Tutorial/") + this->tutorial +
                      "/html/page" + QString::number(page+1) + ".html");
    bool prevPageExists =
        QFile::exists(QString(":/Tutorial/") + this->tutorial +
                      "/html/page" + QString::number(page-1) + ".html");

    ui->next->setEnabled(nextPageExists);
    ui->prev->setEnabled(prevPageExists);
}

/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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
#include "FWBSettings.h"

#include "IC_FileNamePage.h"

#include <QString>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>


IC_FileNamePage::IC_FileNamePage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::IC_FileNamePage_q;
    m_dialog->setupUi(this);

    registerField("fileName*", m_dialog->fileName);
}

void IC_FileNamePage::selectFile()
{
    QString s = QFileDialog::getOpenFileName(
        this,
        "Choose file",
        st->getOpenFileDir(),
        "All files (*)");

    if (s.isEmpty()) return;
    st->setOpenFileDir(s);

    m_dialog->fileName->setText(s);
}

bool IC_FileNamePage::validatePage()
{
    QString file_name = m_dialog->fileName->text();
    QFileInfo f(file_name);

    if ( ! f.exists())
    {
        QMessageBox::critical( nullptr , "Firewall Builder",
                               tr("File %1 does not exist").arg(file_name),
                               QString(),QString());
        return false;
    }

    if ( ! f.isReadable())
    {
        QMessageBox::critical( nullptr , "Firewall Builder",
                               tr("Can not read file %1").arg(file_name),
                               QString(),QString());
        return false;
    }

    return true;
}


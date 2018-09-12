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

#include "TextFileEditor.h"
#include "FWBSettings.h"

#include <qmessagebox.h>
#include <qtextedit.h>
#include <qpushbutton.h>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCloseEvent>
#include <QtDebug>


using namespace std;


TextFileEditor::TextFileEditor(QWidget *parent,
                               const QString &file_name,
                               const QString &title) : QDialog(parent)
{
    this->file_name = file_name;

    m_dialog = new Ui::TextFileEditor_q;
    m_dialog->setupUi(static_cast<QDialog*>(this));

    if (!title.isEmpty()) setWindowTitle(title);
}

bool TextFileEditor::load()
{
    QFile rf(file_name);
    if ( ! rf.exists())
    {
        if (QMessageBox::warning(
                this, "Firewall Builder",
                tr("The file %1 does not exist but it will be created "
                   "when you save your changes.").arg(file_name),
                tr("&Open the file"), tr("&Cancel"), QString::null, 0, 1 ) == 1)
            return false;

        return true;
    }

    QFileInfo fi(file_name);
    if ( ! fi.isWritable())
    {
        switch (
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("The file is read-only, you can't save the changes."),
                tr("&View the file"), tr("&Cancel"), QString::null, 0, 1 ))
        {
        case 0:  // open read-only
            m_dialog->editor->setReadOnly(true);
            m_dialog->ok_button->hide();
            m_dialog->cancel_button->setText(tr("Close"));
            break;

        default:  // cancel
            return false;
        }
    }

    if (rf.open(QIODevice::ReadOnly))
    {
        original_data = rf.readAll();
        m_dialog->editor->setPlainText(original_data);
        rf.close();
    } else
    {
        m_dialog->editor->setPlainText(rf.errorString());
    }

    return true;
}

TextFileEditor::~TextFileEditor()
{
    delete m_dialog;
}

void TextFileEditor::save()
{
    QFile owf(file_name);
    if ( ! owf.exists())
    {
        if (owf.open(QIODevice::WriteOnly) &&
            owf.write(m_dialog->editor->toPlainText().toLatin1().constData()) >= 0)
        {
            owf.close();
            QDialog::accept();
            return;
        } else
            QMessageBox::critical(
                this,"Firewall Builder",
                tr("Error saving data to file '%1': %2")
                .arg(file_name).arg(owf.errorString()),
                "&Continue", QString::null, QString::null, 0, 1 );
        return;
    }
    
    QString tmp_file_name = file_name + ".tmp";

    QFile wf(tmp_file_name);
    if (wf.open(QIODevice::WriteOnly) &&
        wf.write(m_dialog->editor->toPlainText().toLatin1().constData()) >= 0)
    {
        wf.close();
        QFile old_file(file_name);
        if (old_file.remove() && wf.rename(tmp_file_name, file_name))
        {
            QDialog::accept();
            return;
        } else
            QMessageBox::critical(
                this,"Firewall Builder",
                tr("Can not rename file %1 to %2: %3")
                .arg(tmp_file_name).arg(file_name).arg(wf.errorString()),
                "&Continue", QString::null, QString::null, 0, 1 );

    } else
        QMessageBox::critical(
            this,"Firewall Builder",
            tr("Error saving data to a temporary file '%1': %2")
            .arg(tmp_file_name).arg(wf.errorString()),
            "&Continue", QString::null, QString::null, 0, 1 );
}

void TextFileEditor::closeEvent(QCloseEvent* ev)
{
    if (m_dialog->editor->toPlainText() != original_data)
    {
        switch (
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("Dialog contains modified data. Do you want to save it?"),
                tr("&Save"), tr("&Discard"), tr("&Cancel"),
                0,    // enter: button 0 
                2 ))  // escape: button 2
        {
        case 0:
            save();
            QDialog::closeEvent(ev);
            break;

        case 1:
            QDialog::closeEvent(ev);
            break;

        case 2:
            ev->ignore();
            return;
        }
    }
    QDialog::closeEvent(ev);
}


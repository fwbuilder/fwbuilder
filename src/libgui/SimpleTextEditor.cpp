/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "SimpleTextEditor.h"
#include "FWBSettings.h"

#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qtextedit.h>
#include <qpushbutton.h>

#include <fstream>

using namespace std;

SimpleTextEditor::SimpleTextEditor(QWidget *parent,
                                   const QString &txt,
                                   bool enableLoadFromFile,
                                   const QString &title) : QDialog(parent)
{
    m_dialog = new Ui::SimpleTextEditor_q;
    m_dialog->setupUi(static_cast<QDialog*>(this));

    if (enableLoadFromFile) m_dialog->inputFromFileButton->show();
    else                    m_dialog->inputFromFileButton->hide();

    if (!title.isEmpty()) setWindowTitle(title);
    //editor->setTextFormat(QTextEdit::PlainText);
    m_dialog->editor->setPlainText(txt);
}

SimpleTextEditor::~SimpleTextEditor()
{
    delete m_dialog;
}

QString SimpleTextEditor::text()
{
    return m_dialog->editor->toPlainText();
}


void SimpleTextEditor::loadFromFile()
{
    if (QMessageBox::warning(this, tr("Firewall Builder"),
                             tr("Warning: loading from file discards "
                                "current contents of the script."),
                             "&Load", "&Cancel", QString::null, 0, 1 ) != 0)
    {
        return;
    }

    QString filename = QFileDialog::getOpenFileName(this, tr("Choose file"),
                                                    st->getOpenFileDir());

    if (filename.isEmpty()) return;
    st->setOpenFileDir(filename);

    ifstream ifile(filename.toLatin1().constData());
    if (!ifile)
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Could not open file %1").arg(filename),
            "&Continue", QString::null, QString::null, 0, 1 );
        return;
    }

    m_dialog->editor->clear();
    char buf[1024];
    while (ifile.getline(buf,1024))
    {
        m_dialog->editor->append( buf );
    }
}


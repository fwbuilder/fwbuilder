/*

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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
#include "utils.h"
#include "ProjectPanel.h"
#include "AddressTableDialog.h"
#include "TextFileEditor.h"
#include "FWBSettings.h"
#include "FWWindow.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <QUndoStack>


#include <iostream>

using namespace std;
using namespace libfwbuilder;

AddressTableDialog::AddressTableDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::AddressTableDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

AddressTableDialog::~AddressTableDialog()
{
    delete m_dialog;
}

void AddressTableDialog::loadFWObject(FWObject *o)
{
    obj=o;
    AddressTable *s = dynamic_cast<AddressTable*>(obj);
    assert(s!=nullptr);


    init = true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->commentKeywords->loadFWObject(o);

    m_dialog->filename->setText( s->getSourceName().c_str() );
    m_dialog->r_compiletime->setChecked(s->isCompileTime() );
    m_dialog->r_runtime->setChecked(s->isRunTime() );

    //BrowseButton->setEnabled(s->isCompileTime() );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->filename->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->filename);

    updateButtons();

    init = false;
}

void AddressTableDialog::updateButtons()
{
    m_dialog->editButton->setEnabled( ! m_dialog->filename->text().isEmpty());
}

void AddressTableDialog::validate(bool *res)
{
    *res=true;

#ifndef NDEBUG
    AddressTable *s = dynamic_cast<AddressTable*>(obj);
    assert(s!=nullptr);
#endif

    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}

void AddressTableDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    AddressTable *s = dynamic_cast<AddressTable*>(new_state);
    assert(s!=nullptr);

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    QByteArray cs = m_dialog->filename->text().toLocal8Bit();
    s->setSourceName( (const char *)cs );
    s->setRunTime(m_dialog->r_runtime->isChecked() );

    updateButtons();

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
   
}


static void doReminderAboutDataDir()
{
    if (st->isReminderAboutDataDirSuppressed()) return;

    QMessageBox msgBox;
    msgBox.setText("The file you selected is inside the "
                   "'data directory' global preference.  The path of the "
                   "file has been converted to use the variable %DATADIR% "
                   "so that expansion will happen properly within rules.");

    msgBox.setWindowModality(Qt::ApplicationModal);
    msgBox.setWindowFlags(Qt::Window |
                          Qt::WindowTitleHint |
                          Qt::CustomizeWindowHint |
                          Qt::WindowCloseButtonHint);

    msgBox.setWindowTitle("Data directory conversion");

    QCheckBox cb("Do not show this again", &msgBox);
    msgBox.addButton(&cb, QMessageBox::ResetRole);
    msgBox.addButton(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);
    msgBox.setIcon(QMessageBox::Information);

    /* Hack alert!  Disconnect signals from the checkbox so that
       QMessageBox doesn't know when it gets clicked, and treat it
       like an "OK" action. */
    cb.disconnect();

    msgBox.exec();
    if (cb.isChecked()) st->suppressReminderAboutDataDir(true);
}


void AddressTableDialog::browse()
{
    // build a dialog that will let user select existing file or enter
    // a name even if the file does not exist

    QString s = QFileDialog::getOpenFileName(
        this,
        tr("Choose a file or type the name to create new"),
        st->getOpenFileDir(mw->getCurrentFileName()),
        tr("All files (*)"));

    if (s.isEmpty()) return;
    st->setOpenFileDir(s);

    QString dataDir = st->getDataDir();
    if (!dataDir.isEmpty()) {
        QString dataDirPath = QFileInfo(dataDir).canonicalFilePath();
        QString filePath = QFileInfo(s).canonicalFilePath();
        if (filePath.length() > 0 && filePath.startsWith(dataDirPath)) {
            int truncateLen = dataDirPath.length();
            if (dataDirPath.at(truncateLen-1) == '/' ||
                dataDirPath.at(truncateLen-1) == '\\') {
                truncateLen--;
            }
            s = filePath.replace(0, truncateLen, "%DATADIR%");
            doReminderAboutDataDir();
        }
    }

    m_dialog->filename->setText(s);
    // assign focus to the "file name" input field so that it
    // generates signal editFinished when user clicks
    // elsewhere. We use this signal to call changed() which in
    // turn calls applyChanges() to save data
    m_dialog->filename->setFocus(Qt::OtherFocusReason);
    updateButtons();
}

void AddressTableDialog::editFile( void )
{
    QString filePath = m_dialog->filename->text();
    if (filePath.startsWith("%DATADIR%")) {
        QString dataDir = st->getDataDir();
        if (dataDir.isEmpty()) {
            QMessageBox::critical(this, "Firewall Builder",
                                  tr("Data directory setting is blank "
                                     "and path contains %DATADIR% variable"));
            return;
        }
        filePath.replace(0, 9, dataDir);
    }

    TextFileEditor editor(this, filePath);
    if (editor.load())
        editor.exec();  // its modal dialog
}


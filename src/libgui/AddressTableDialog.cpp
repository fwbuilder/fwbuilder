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

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "ProjectPanel.h"
#include "AddressTableDialog.h"
#include "SimpleTextView.h"
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
    obj=NULL;
}

AddressTableDialog::~AddressTableDialog()
{
    delete m_dialog;
}

void AddressTableDialog::getHelpName(QString *str)
{
    *str = "AddressTableDialog";
}

void AddressTableDialog::loadFWObject(FWObject *o)
{
    obj=o;
    AddressTable *s = dynamic_cast<AddressTable*>(obj);
    assert(s!=NULL);


    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    m_dialog->filename->setText( s->getSourceName().c_str() );
    m_dialog->r_compiletime->setChecked(s->isCompileTime() );
    m_dialog->r_runtime->setChecked(s->isRunTime() );

    //BrowseButton->setEnabled(s->isCompileTime() );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->filename->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->filename);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);

    init=false;
}

void AddressTableDialog::validate(bool *res)
{
    *res=true;
    AddressTable *s = dynamic_cast<AddressTable*>(obj);
    assert(s!=NULL);

    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}



void AddressTableDialog::applyChanges()
{
    std::auto_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    AddressTable *s = dynamic_cast<AddressTable*>(new_state);
    assert(s!=NULL);

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    new_state->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );
    QByteArray cs = m_dialog->filename->text().toLocal8Bit();
    s->setSourceName( (const char *)cs );
    s->setRunTime(m_dialog->r_runtime->isChecked() );

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
   
}

void AddressTableDialog::browse()
{

    QString dir;
    dir=st->getWDir();
    if (dir.isEmpty())  dir=st->getOpenFileDir();
    if (dir.isEmpty())  dir="~";

    QString s = QFileDialog::getOpenFileName(this,
                    "Choose a file",
                    dir,
                    "All files (*)");

    if (!s.isEmpty())
    {
        m_dialog->filename->setText(s);
    }
}
void AddressTableDialog::preview( void )
{
    SimpleTextView tv(this);
    tv.setName(m_dialog->obj_name->text());

    QFile f;
    QTextStream ts;
    QString filePath = m_dialog->filename->text();

    if (QDir::isRelativePath(filePath))
        f.setFileName(getFileDir(mw->getCurrentFileName()) + "/" + filePath);
    else
        f.setFileName(filePath);

    if (f.exists())
    {
        if(f.open(QIODevice::ReadOnly ))
        {
            ts.setDevice(&f);
            tv.setText(ts.readAll());
            f.close();
        }
    }
    else
    {
        tv.setText("File not found.");
    }
    tv.exec();
}


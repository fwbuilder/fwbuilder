/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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
#include "utils.h"

#include "FWBApplication.h"
#include "newGroupDialog.h"

#include <qlineedit.h>
#include <qapplication.h>

#include <iostream>

using namespace libfwbuilder;
using namespace std;

newGroupDialog::~newGroupDialog()
{
    delete m_dialog;
}

newGroupDialog::newGroupDialog(QWidget *parent, FWObjectDatabase *_db) : QDialog(parent)
{
    db = _db;

    m_dialog = new Ui::newGroupDialog_q;
    m_dialog->setupUi(this);

    fillLibraries(m_dialog->libs, db, true);   // only read-write libs
    m_dialog->obj_name->setFocus();
}

void newGroupDialog::accept()
{
/* create a group */

    if (m_dialog->obj_name->text().isEmpty())
    {
        app->beep();
        return;
    }
    QDialog::accept();
}

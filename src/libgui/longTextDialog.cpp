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

#include "longTextDialog.h"

#include <qtextbrowser.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qlabel.h>

#include <iostream>

using namespace libfwbuilder;
using namespace std;

longTextDialog::~longTextDialog()
{
    delete m_dialog;
}

longTextDialog::longTextDialog(QWidget *p,
                               const QString &txt,const QString &ltxt)
    : QDialog(p)
{
    m_dialog = new Ui::longTextDialog_q;
    m_dialog->setupUi(this);

    setWindowTitle("Firewall Builder");

    m_dialog->dlgText->setText(txt);

    m_dialog->icn->setPixmap( QMessageBox::standardIcon(QMessageBox::Critical) );

    m_dialog->dlgLongText->setText(ltxt);
}

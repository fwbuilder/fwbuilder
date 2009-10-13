/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "../../config.h"
#include "global.h"

#include "pfsyncOptionsDialog.h"
#include "FWWindow.h"

#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"

#include <qmessagebox.h>

using namespace std;
using namespace libfwbuilder;


pfsyncOptionsDialog::pfsyncOptionsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::pfsyncOptionsDialog_q;
    m_dialog->setupUi(this);
    obj = o;

    FWOptions *gropt = FWOptions::cast(obj);
    assert(gropt != NULL);
    
    data.registerOption(m_dialog->syncpeer,
                        gropt,
                        "syncpeer");

    data.loadAll();
}

pfsyncOptionsDialog::~pfsyncOptionsDialog()
{
    delete m_dialog;
}

void pfsyncOptionsDialog::accept()
{
    data.saveAll();
    QDialog::accept();
}

void pfsyncOptionsDialog::reject()
{
    QDialog::reject();
}

bool pfsyncOptionsDialog::validate()
{
    return true;
}


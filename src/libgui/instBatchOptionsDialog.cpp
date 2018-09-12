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

#include "definitions.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"

#include "instBatchOptionsDialog.h"

#include <qstring.h>
#include <qlineedit.h>
#include <qlabel.h>

#include <stdlib.h>

using namespace std;
using namespace libfwbuilder;

instBatchOptionsDialog::instBatchOptionsDialog(QWidget *parent, instConf *_cnf) :
    instOptionsDialog(parent, _cnf)
{
    m_dialog->dialogTitleLine->setText(QString("<p align=\"center\"><b><font size=\"+2\">")+
                             tr("Batch install options")+
                             QString("</font></b></p>")
    );
    // must reset alt address in the dialog even though it is
    // hidden. This is because we read it in instDialog::doInstallPage
    // regardless of wether we perform batch install or not.
    m_dialog->altAddress->setText("");
    m_dialog->altAddressLabel->hide();
    m_dialog->altAddress->hide();

}


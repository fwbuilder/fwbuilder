/*

                          Firewall Builder

                 Copyright (C) 2005 NetCitadel, LLC

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

#include "StartTipDialog.h"
#include "FWBSettings.h"
#include "Help.h"

#include <QCheckBox>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QLocale>


using namespace std;
using namespace libfwbuilder;

StartTipDialog::StartTipDialog()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);

    m_dialog = new Ui::StartTipDialog_q;
    m_dialog->setupUi(this);

    m_dialog->textview->setText(Help::getHelpFileContents("main"));
}

void StartTipDialog::close()
{
    if (m_dialog->donotshow->isChecked())
        st->setBool("UI/NoStartTip", true);
    QDialog::close();
}

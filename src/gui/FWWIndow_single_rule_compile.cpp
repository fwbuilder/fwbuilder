/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  alek@codeminders.com
           refactoring and bugfixes: vadim@fwbuilder.org

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

#include <ui_FWBMainWindow_q.h>
#include "FWWindow.h"

#include "fwbuilder/Rule.h"


using namespace Ui;
using namespace libfwbuilder;
using namespace std;



void FWWindow::singleRuleCompile(Rule *rule)
{
    showEditor();
    QSize old_size = m_mainWindow->objectEditorStack->size();
    oe->openOpt(rule, ObjectEditor::optRuleCompile);
    m_mainWindow->objectEditorStack->setCurrentIndex(oe->getCurrentDialogIndex());
    //m_panel->objectEditorFrame->show();
    m_mainWindow->objectEditorStack->resize(old_size);

}



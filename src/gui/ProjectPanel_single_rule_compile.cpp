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
#include "utils.h"
#include "utils_no_qt.h"

#include <fwbuilder/Cluster.h>
#include <fwbuilder/Firewall.h>
#include <fwbuilder/RuleSet.h>
#include <fwbuilder/Policy.h>
#include <fwbuilder/NAT.h>
#include <fwbuilder/Routing.h>
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"

#include "FWBSettings.h"
#include "FWBTree.h"
#include "FWObjectPropertiesFactory.h"
#include "FWWindow.h"
#include "FindObjectWidget.h"
#include "FindWhereUsedWidget.h"
#include "ProjectPanel.h"
#include "RCS.h"
#include "RuleSetView.h"
#include "findDialog.h"
#include "events.h"

#include <QTimer>
#include <QStatusBar>
#include <QFileInfo>


using namespace Ui;
using namespace libfwbuilder;
using namespace std;



void ProjectPanel::singleRuleCompile(Rule *rule)
{
    showEditor();
    QSize old_size = m_panel->objectEditorStack->size();
    oe->openOpt(rule, ObjectEditor::optRuleCompile);
    m_panel->objectEditorStack->setCurrentIndex(oe->getCurrentDialogIndex());
    //m_panel->objectEditorFrame->show();
    m_panel->objectEditorStack->resize(old_size);

}



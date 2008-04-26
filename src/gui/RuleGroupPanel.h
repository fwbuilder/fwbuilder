/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: RuleGroupPanel.h,v 1.4 2007/01/06 22:03:25 vkurland Exp $

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
#include <ui_rulegrouppanel.h>
#include <qframe.h>

class RuleSetView ;

class RuleGroupPanel : public QFrame, public Ui::RuleGroupPanel
{
    Q_OBJECT 

public:

    int row  ;
    RuleSetView * rsv ;
    RuleGroupPanel (QWidget * parent,RuleSetView * rsv, int row) ;
public slots:
    void showHideRuleGroup();
public:
    virtual void mousePressEvent ( QMouseEvent * event );
};

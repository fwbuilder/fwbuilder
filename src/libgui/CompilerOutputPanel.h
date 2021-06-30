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

#ifndef __COMPILEROUTPUTPANEL_H__
#define __COMPILEROUTPUTPANEL_H__

#include "BaseObjectDialog.h"
#include <ui_compileroutputpanel_q.h>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Firewall.h"

#include <QRegExp>

#include <list>


class ProjectPanel;

class CompilerOutputPanel : public BaseObjectDialog
{
    Q_OBJECT;

    libfwbuilder::RoutingRule *rule;
    Ui::CompilerOutputPanel_q *m_widget;
    std::list<QRegExp> error_re;
    std::list<QRegExp> warning_re;

 public:

    CompilerOutputPanel(QWidget* parent);
    ~CompilerOutputPanel();

public slots:

    virtual void changed();
    virtual void applyChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
    virtual void closeEvent(QCloseEvent *e);

protected:
    virtual void resizeEvent ( QResizeEvent * event );

};

#endif

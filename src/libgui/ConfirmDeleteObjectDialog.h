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


#ifndef __CONFIRMDELETEOBJECTDIALOG_H_
#define __CONFIRMDELETEOBJECTDIALOG_H_

#include <ui_confirmdeleteobjectdialog_q.h>

#include "FindWhereUsedWidget.h"

#include  <vector>
#include  <map>

namespace libfwbuilder {
    class FWObject;
}

class QListWidgetItem;

class ConfirmDeleteObjectDialog : public QDialog
{
    Q_OBJECT
 private:
        libfwbuilder::FWObject *object;
        FindWhereUsedWidget * fwu;
        std::map<QListWidgetItem *,libfwbuilder::FWObject *> listItemsMapping;
        Ui::ConfirmDeleteObjectDialog_q *m_dialog;
 public:
    ConfirmDeleteObjectDialog(QWidget*p );
    ~ConfirmDeleteObjectDialog();

public slots:
   void load(std::vector<libfwbuilder::FWObject *> objs);
   void findForObject(libfwbuilder::FWObject *obj);
    
 signals:
};

#endif 

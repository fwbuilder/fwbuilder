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


#ifndef  __ASKLIBFORCOPYDIALOG_H_
#define  __ASKLIBFORCOPYDIALOG_H_

#include "global.h"

#include <ui_asklibforcopydialog_q.h>

namespace libfwbuilder{
  class FWObject;
  class FWObjectDatabase;
}

class AskLibForCopyDialog : public QDialog {

    Q_OBJECT
private:
    libfwbuilder::FWObjectDatabase *m_db;
    std::vector<libfwbuilder::FWObject*>          idxToLibs;
    libfwbuilder::FWObject *m_curr;
    Ui::asklibforcopydialog_q *m_dialog;
   
    AskLibForCopyDialog( QWidget *parent, libfwbuilder::FWObjectDatabase *db, 
        libfwbuilder::FWObject *curr = 0);
    int getIdxForLib(libfwbuilder::FWObject* lib);
    void loadObjects();
    int addLib( libfwbuilder::FWObject *lib);
    libfwbuilder::FWObject *getChoosenLib();
public:
    ~AskLibForCopyDialog();
    static libfwbuilder::FWObject *askLibForCopyDialog( QWidget *parent, 
        libfwbuilder::FWObjectDatabase *db, libfwbuilder::FWObject *curr = 0);
};

#endif

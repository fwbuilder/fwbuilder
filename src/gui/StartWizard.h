/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: StartWizard.h,v 1.1 2004/04/03 23:21:39 vkurland Exp $

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


#ifndef  __STARTWIZARD_H_
#define  __STARTWIZARD_H_

#include <ui_startwizard_q.h>
#include "RCS.h"
#include "fakeWizard.h"
#include <QDialog>

#include <qstring.h>

class StartWizard : public QDialog, public FakeWizard
{

    Q_OBJECT
    
    bool     wantRCS;
    bool     oldfile;
    bool     newfile;
    QString  fname;
    
    Ui::startWizard_q *m_dialog;
    
 public:

    StartWizard();
    ~StartWizard();

 public slots:
        
     virtual void openFile();
     virtual void newFile();
     virtual void selected(const QString &title);
     virtual void finishClicked();
     virtual void cancelClicked();
     
};

#endif

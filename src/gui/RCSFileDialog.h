/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: RCSFileDialog.h,v 1.3 2006/10/17 06:09:25 vkurland Exp $

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


#ifndef __RCSFILEDIALOG_H_
#define __RCSFILEDIALOG_H_

#include "RCS.h"

#include <qfiledialog.h>
#include <qlistview.h>

class RCSFileDialog : public QFileDialog {

    Q_OBJECT

 public:

    RCSFileDialog( const QString& dirName, const QString& filter = QString::null,
                   QWidget* parent=0, const char* name=0, bool modal = FALSE );
    RCSFileDialog( QWidget* parent=0, const char* name=0, bool modal = FALSE );

    RCS*     getSelectedRev();

 protected slots:

     virtual void accept();
};

#endif

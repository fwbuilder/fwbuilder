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


#ifndef __LIBEXPORTDIALOG_H_
#define __LIBEXPORTDIALOG_H_

#include <ui_libexport_q.h>

#include <QDialog>



#include <map>
#include <list>

class RCSFilePreview;

namespace libfwbuilder {
    class FWObject;
    class FWReference;
};

class LibExportDialog : public  QDialog{

    Q_OBJECT

    void init();


 public:
    std::map<int, libfwbuilder::FWObject*>   mapOfLibs;
    LibExportDialog(QWidget* parent=0, const char* name=0, bool modal = false );
    ~LibExportDialog();
    
    Ui::LibExport_q *m_dialog;
 protected slots:

};

#endif

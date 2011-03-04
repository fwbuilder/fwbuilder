/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef IC_FILENAMEPAGE_H
#define IC_FILENAMEPAGE_H

#include "ui_ic_filenamepage_q.h"

class IC_FileNamePage : public QWizardPage
{
    Q_OBJECT;

    Ui::IC_FileNamePage_q *m_dialog;
    
public:
    IC_FileNamePage(QWidget *parent);
    virtual ~IC_FileNamePage() {}

    virtual bool validatePage();

public slots:
    void selectFile();
};

#endif // IC_FILENAMEPAGE_H

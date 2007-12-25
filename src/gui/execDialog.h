/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: execDialog.h,v 1.4 2006/02/18 05:26:30 vkurland Exp $

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


#ifndef __EXECDIALOG_H_
#define __EXECDIALOG_H_

#include <ui_execdialog_q.h>

#include <qlist.h>
#include <qstring.h>
#include <qprocess.h>
#include <qwidget.h>

class execDialog : public QDialog
{
    Q_OBJECT

    QProcess proc;
    int      res;
    Ui::execDialog_q *m_dialog;
    QStringList arguments;
    
public:
    execDialog(QWidget *parent,const QStringList &args, const QString &closeButtonText="");
    ~execDialog();
    int run();

public slots:

    virtual void readFromStdout();
    virtual void stopProcess();
    virtual void processExited();
    virtual void saveLog();
};

#endif

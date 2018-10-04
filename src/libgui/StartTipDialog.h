/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>

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


#ifndef __STARTTIPDIALOG_H_
#define __STARTTIPDIALOG_H_


#include "ui_starttipdialog_q.h"

#include <QStringList>
#include <QString>
#include <QDialog>

class StartTipDialog : public QDialog
{
    Q_OBJECT;

    QStringList tips;
    int current_tip;
    bool first_run;
    
    void showTip(const QString &txt, bool new_tip=true);
    void showTip(int tip_idx);
    QString getRandomTip();
    
public:
    Ui::StartTipDialog_q *m_dialog;

    StartTipDialog(QWidget *parent = nullptr);
    
    virtual ~StartTipDialog();

    void run();

public slots:
    void nextTip();
    void prevTip();

    virtual void close();
};

#endif 

/* 

                          Firewall Builder

                 Copyright (C) 2005 NetCitadel, LLC

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


#ifndef __SIMPLETEXTVIEW_H_
#define __SIMPLETEXTVIEW_H_

#include <ui_simpletextview_q.h>
#include <QDialog>



class SimpleTextView : public QDialog
{
    Q_OBJECT
 private:
 
 public:
    Ui::SimpleTextView_q *m_dialog;
     
    SimpleTextView(QWidget *parent) : QDialog(parent)
    {
        m_dialog = new Ui::SimpleTextView_q;
        m_dialog->setupUi(this);
    };
    
    virtual ~SimpleTextView() { delete m_dialog; };
    virtual void setText(QString s);
    virtual void setName(QString s);


public slots:
    
 signals:
    
};

#endif 

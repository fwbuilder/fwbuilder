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


#ifndef __COLORLABELMENUITEM_H_
#define __COLORLABELMENUITEM_H_

#include <ui_colorlabelmenuitem_q.h>

class QToolButton;

class ColorLabelMenuItem : public QWidget
{
    Q_OBJECT

    void setup(QToolButton *btn, const QString &c, const QString &t);
    QString color;
    
 public:
    Ui::colorLabelMenuItem_q *m_widget;
    ColorLabelMenuItem(QWidget *parent);
    ~ColorLabelMenuItem() { delete m_widget; };
    
public slots:
    virtual void colorClicked();

    virtual void noneColorClicked();
    virtual void redColorClicked();
    virtual void orangeColorClicked();
    virtual void yellowColorClicked();
    virtual void greenColorClicked();
    virtual void blueColorClicked();
    virtual void purpleColorClicked();
    virtual void grayColorClicked();

signals:
    void returnColor(const QString &c);
};

#endif // __COLORLABELMENUITEM_H

/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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


#ifndef __PREFSDIALOG_H_
#define __PREFSDIALOG_H_

#include "config.h"
#include <ui_prefsdialog_q.h>

#include "FWBSettings.h"
#include "listOfLibraries.h"

#include <list>
#include <string>
#include <map>
#include <qfont.h>

#include <qstring.h>

class QPushButton;


class PrefsDialog : public QDialog
{
    Q_OBJECT

    void setButtonColor(QPushButton *btn,const QString &colorCode);
    void changeColor(QPushButton *btn,FWBSettings::LabelColors colorCode);

    std::map<int,QString> colors;
    Ui::prefsDialog_q *m_dialog;

    QFont rulesFont;
    QFont treeFont;
    QFont uiFont;
    void changeFont(QFont *font);
 public:
    PrefsDialog(QWidget *parent);
    ~PrefsDialog();
    
public slots:
    virtual void accept();
    virtual void findWDir();
    virtual void addLibrary();
    virtual void remLibrary();
    virtual void findSSH();
    virtual void libClick(QTreeWidgetItem* itm, int col);
    virtual void changeRedColor();
    virtual void changeOrangeColor();
    virtual void changeYellowColor();
    virtual void changeGreenColor();
    virtual void changeBlueColor();
    virtual void changePurpleColor();
    virtual void changeGrayColor();
    virtual void changeIconSize25();
    virtual void changeIconSize16();
    virtual void changeShowIcons();
    virtual void changeRulesFont();
    virtual void changeTreeFont();
};

#endif // __PREFSDIALOG_H

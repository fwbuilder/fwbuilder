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

#ifndef __ADDRESSTABLEEDITOR_H__
#define __ADDRESSTABLEEDITOR_H__

#include <ui_textfileeditor_q.h>

class TextFileEditor : public QDialog
{
    Q_OBJECT;

    QString file_name;
    QString original_data;
    
 public:
    Ui::TextFileEditor_q *m_dialog;
    
    TextFileEditor(QWidget *parent,
                   const QString &file_name,
                   const QString &title="");
    ~TextFileEditor();

    // load data, return true if successful
    bool load();
    
protected:
    virtual void closeEvent(QCloseEvent *ev);
    
public slots:
     virtual void save();
};

#endif

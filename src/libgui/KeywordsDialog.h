/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Theron Tock

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


#ifndef __KeywordsDialog_h_
#define __KeywordsDialog_h_

#include "fwbuilder/FWObject.h"
#include <ui_keywordsdialog_q.h>
#include <QDialog>

class QStringListModel;

class KeywordsDialog : public QDialog
{
 private:
    Q_OBJECT;

    Ui_KeywordsDialog_q m_ui;
    libfwbuilder::FWObject *m_obj;

    QStringListModel *m_allModel;
    QStringListModel *m_currModel;
    QSet<QString> m_allKeywords;
    QSet<QString> m_currKeywords;

 public:
    KeywordsDialog(libfwbuilder::FWObject *obj, QWidget *parent = 0);
    ~KeywordsDialog();

    QStringList getKeywords();

 public slots:
    void gotAddClick();
    void gotRemoveClick();
    void gotAllKeywordsDoubleClick(const QModelIndex &);
    void gotNewKeywordClick();

 public:
    static bool validateKeyword(QWidget *parent, const QString &keyword);
    
};

#endif  /* KeywordsDialog_h_ */

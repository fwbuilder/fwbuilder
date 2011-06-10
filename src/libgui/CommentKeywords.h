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


#ifndef __CommentKeywords_h_
#define __CommentKeywords_h_

#include "fwbuilder/FWObject.h"
#include <ui_commentkeywords_q.h>

class CommentKeywords : public QWidget
{
 private:
    Q_OBJECT;

    Ui_CommentKeywords_q m_ui;
    libfwbuilder::FWObject *m_obj;
    QStringList m_keywords;

    void updateKeywordsLabel();

 public:
    CommentKeywords(QWidget *parent = 0);

    void applyChanges(libfwbuilder::FWObject *newObj);
    void loadFWObject(libfwbuilder::FWObject *obj);

    void setReadOnlyComment(const QString &comment);

 signals:
    void changed();

 public slots:
    virtual void keywordsClicked();
    
};

#endif  /* CommentKeywords_h_ */

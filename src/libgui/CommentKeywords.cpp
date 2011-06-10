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

#include "CommentKeywords.h"
#include "utils.h"
#include "fwbuilder/FWObject.h"
#include "KeywordsDialog.h"

using namespace std;
using namespace libfwbuilder;


CommentKeywords::CommentKeywords(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
}


void CommentKeywords::applyChanges(FWObject *newObj)
{
    newObj->setComment(m_ui.comment->getText().toUtf8().constData());

    newObj->clearKeywords();
    foreach (QString keyword, m_keywords) {
        newObj->addKeyword(keyword.toUtf8().constData());
    }
}


void CommentKeywords::loadFWObject(FWObject *obj)
{
    m_obj = obj;
    m_keywords.clear();

    set<string> keywords = obj->getKeywords();
    set<string>::const_iterator iter;
    for (iter = keywords.begin(); iter != keywords.end(); ++iter) {
        m_keywords.append(QString::fromUtf8((*iter).c_str()));
    }

    QString comment = QString::fromUtf8(obj->getComment().c_str());
    m_ui.comment->setTextDefault(comment, "<i>Enter comment here</i>");
    updateKeywordsLabel();

    m_ui.comment->setReadOnly(obj->isReadOnly());
    setDisabledPalette(m_ui.comment);

    m_ui.keywordsButton->setEnabled(!obj->isReadOnly());
}


void CommentKeywords::setReadOnlyComment(const QString &comment)
{
    m_ui.comment->setText(comment);
    m_ui.comment->setReadOnly(true);
    m_ui.keywordsButton->setEnabled(false);
}


void CommentKeywords::keywordsClicked()
{
    KeywordsDialog dialog(m_obj);
    if (dialog.exec() == QDialog::Accepted) {
        m_keywords = dialog.getKeywords();
        emit changed();
    }
    updateKeywordsLabel();
}


void CommentKeywords::updateKeywordsLabel()
{
    QString result;
    m_keywords = sortStrings(m_keywords);
    foreach (QString keyword, m_keywords) {
        if (!result.isEmpty()) result += ", ";
        result += keyword;
    }

    if (result.isEmpty()) result = "<i>No keywords</i>";
    m_ui.keywordsLabel->setText(result);
}

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

#include "KeywordsDialog.h"
#include "utils.h"
#include "fwbuilder/FWObject.h"
#include <QStringListModel>
#include <QMessageBox>

using namespace std;
using namespace libfwbuilder;

KeywordsDialog::KeywordsDialog(FWObject *obj, QWidget *parent)
    : QDialog(parent),
      m_obj(obj)

{
    m_ui.setupUi(this);
    m_ui.newKeywordButton->setDefault(true);

    connect(m_ui.addKeywordButton, SIGNAL(clicked()),
            this, SLOT(gotAddClick()));
    connect(m_ui.removeKeywordButton, SIGNAL(clicked()),
            this, SLOT(gotRemoveClick()));
    connect(m_ui.newKeywordButton, SIGNAL(clicked()),
            this, SLOT(gotNewKeywordClick()));
    connect(m_ui.allKeywordsListView,
            SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(gotAllKeywordsDoubleClick(const QModelIndex &)));
            
    const set<string> &keywords = m_obj->getKeywords();
    const set<string> &allKeywords = m_obj->getAllKeywords();
    set<string>::const_iterator iter;
    for (iter = keywords.begin(); iter != keywords.end(); ++iter) {
        m_currKeywords.insert(QString::fromUtf8((*iter).c_str()));
    }
    for (iter = allKeywords.begin(); iter != allKeywords.end(); ++iter) {
        m_allKeywords.insert(QString::fromUtf8((*iter).c_str()));
    }

    m_allModel = new QStringListModel(sortStrings(m_allKeywords.values()));
    m_ui.allKeywordsListView->setModel(m_allModel);

    m_currModel = new QStringListModel(sortStrings(m_currKeywords.values()));
    m_ui.currKeywordsListView->setModel(m_currModel);
}


KeywordsDialog::~KeywordsDialog()
{
    delete m_allModel;
    delete m_currModel;
}


QStringList KeywordsDialog::getKeywords()
{
    return m_currModel->stringList();
}


void KeywordsDialog::gotAddClick()
{
    QStringList all = m_allModel->stringList();
    QItemSelection selection =
        m_ui.allKeywordsListView->selectionModel()->selection();

    foreach (QItemSelectionRange range, selection) {
        for (int ii = range.top(); ii <= range.bottom(); ii++) {
            m_currKeywords.insert(all.at(ii));
        }
    }

    m_currModel->setStringList(sortStrings(m_currKeywords.values()));
}

void KeywordsDialog::gotRemoveClick()
{
    QStringList curr = m_currModel->stringList();
    QItemSelection selection =
        m_ui.currKeywordsListView->selectionModel()->selection();

    foreach (QItemSelectionRange range, selection) {
        for (int ii = range.top(); ii <= range.bottom(); ii++) {
            m_currKeywords.remove(curr.at(ii));
        }
    }

    m_currModel->setStringList(sortStrings(m_currKeywords.values()));
}


void KeywordsDialog::gotAllKeywordsDoubleClick(const QModelIndex &)
{
    gotAddClick();
}


void KeywordsDialog::gotNewKeywordClick()
{
    QString newKeyword = m_ui.newKeywordLineEdit->text().simplified();
    if (!validateKeyword(this, newKeyword)) return;

    m_currKeywords.insert(newKeyword);
    m_allKeywords.insert(newKeyword);

    m_currModel->setStringList(sortStrings(m_currKeywords.values()));
    m_allModel->setStringList(sortStrings(m_allKeywords.values()));

    m_ui.newKeywordLineEdit->clear();
}


bool KeywordsDialog::validateKeyword(QWidget *parent, const QString &keyword)
{
    if (keyword.isEmpty()) return false;
    if (keyword.contains(',')) {
        QMessageBox::warning(parent, "Firewall Builder",
                             tr("Keyword cannot contain a comma"), "&OK",
                             QString(), QString(), 0, 1);
        return false;
    }

    return true;
}

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


#include "DynamicGroupDialog.h"
#include "events.h"
#include "FWBTree.h"
#include "FWCmdChange.h"
#include "FWObjectPropertiesFactory.h"
#include "FWWindow.h"
#include "fwbuilder/DynamicGroup.h"
#include <qpixmapcache.h>
#include <QHeaderView>
#include <QMouseEvent>

using namespace std;
using namespace libfwbuilder;

DynamicItemDelegate::DynamicItemDelegate(DynamicGroupDialog *dialog,
                                         QObject *parent)
    : QItemDelegate(parent),
      m_dialog(dialog)
{
}


QWidget *DynamicItemDelegate::createEditor(QWidget *parent,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    Q_UNUSED(option)

    if (index.column() == 0) {
        QToolButton *button = new QToolButton(parent);
        QPixmap pixmap;
        if (!QPixmapCache::find(":/Icons/neg", pixmap)) {
            pixmap.load(":/Icons/neg");
            QPixmapCache::insert(":/Icons/neg", pixmap);
        }
        button->setIcon(QIcon(pixmap));
        button->setProperty("row", QVariant(index.row()));
        connect(button, SIGNAL(clicked()),
                m_dialog, SLOT(deleteFilterClicked()));
        
        return button;
    } else {
        QComboBox *combo = new QComboBox(parent);
        connect(combo, SIGNAL(activated(int)), this, SLOT(comboActivated(int)));
        return combo;
    }
}


void DynamicItemDelegate::comboActivated(int abc)
{
    Q_UNUSED(abc)

    /* Don't wait until we lose focus on the combobox */
    emit commitData(dynamic_cast<QWidget *>(sender()));
}


void DynamicItemDelegate::setEditorData(QWidget *editor,
                                        const QModelIndex &index) const
{
    if (index.column() == 0) return;

    QString value = index.model()->data(index, Qt::EditRole).toString();

    QComboBox *combo = static_cast<QComboBox *>(editor);
    combo->clear();
    if (index.column() == 1) {
        if (value == DynamicGroup::TYPE_NONE) {
            combo->addItem("None selected", DynamicGroup::TYPE_NONE);
            combo->setCurrentIndex(0);
        }
        combo->addItem("Any type", DynamicGroup::TYPE_ANY);
        if (value == DynamicGroup::TYPE_ANY) {
            combo->setCurrentIndex(combo->count() - 1);
        }

        combo->insertSeparator(2);

        QList<const char *> types = FWBTree::getObjectTypes();
        foreach (const char *type, types) {
            combo->addItem(FWBTree().getTranslatableObjectTypeName(type), type);
            if (value == type) {
                combo->setCurrentIndex(combo->count() - 1);
            }
        }
    } else if (index.column() == 2) {
        if (value == DynamicGroup::KEYWORD_NONE) {
            combo->addItem("None selected", DynamicGroup::KEYWORD_NONE);
            combo->setCurrentIndex(0);
        }
        
        combo->addItem("Any keyword", DynamicGroup::KEYWORD_ANY);
        if (value == DynamicGroup::KEYWORD_ANY) {
            combo->setCurrentIndex(combo->count() - 1);
        }
        combo->insertSeparator(2);

        QStringList list;
        const set<string> &keywords =
            m_dialog->getCurrentObj()->getAllKeywords();
        set<string>::const_iterator iter;
        for (iter = keywords.begin(); iter != keywords.end(); ++iter) {
            list.append(QString::fromUtf8((*iter).c_str()));
        }

        foreach (QString item, sortStrings(list)) {
            combo->addItem(item, item);
            if (item == value) {
                combo->setCurrentIndex(combo->count() - 1);
            }
        }
    }
}


void DynamicItemDelegate::setModelData(QWidget *editor,
                                       QAbstractItemModel *model,
                                       const QModelIndex &index) const
{
    if (index.column() == 0) return;

    QComboBox *combo = static_cast<QComboBox *>(editor);

    QString value = combo->itemData(combo->currentIndex()).toString();
    model->setData(index, value, Qt::EditRole);
}

/*****************************************************************/

DynamicGroupDialog::DynamicGroupDialog(QWidget *parent) :
    BaseObjectDialog(parent),
    m_reloadObjFilter(false)
{
    m_ui.setupUi(this);
    connectSignalsOfAllWidgetsToSlotChange();

    m_model = new QStandardItemModel;
    connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(changed()));

    DynamicItemDelegate *delegate =
        new DynamicItemDelegate(this, m_ui.criteriaView);
    m_ui.criteriaView->setItemDelegate(delegate);
    m_ui.criteriaView->setModel(m_model);
    m_ui.criteriaView->resizeColumnsToContents();
    m_ui.criteriaView->verticalHeader()->hide();

    QStringList headers;
    headers << "Name" << "Properties";
    m_ui.matchedView->setHeaderLabels(headers);
    connect(m_ui.matchedView, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(gotItemDoubleClicked(QTreeWidgetItem *, int)));
}


void DynamicGroupDialog::applyChanges()
{
    if (obj->isReadOnly()) return;

    FWCmdChange *change = new FWCmdChange(m_project, obj);
    DynamicGroup *newState = DynamicGroup::cast(change->getNewState());

    newState->setName(m_ui.nameEdit->text().toUtf8().constData());
    m_ui.commentKeywords->applyChanges(newState);

    list<string> filters;
    int rows = m_model->rowCount();
    for (int ii = 0; ii < rows; ii++) {
        QString type = m_model->data(m_model->index(ii, 1)).toString();
        QString keyword = m_model->data(m_model->index(ii, 2)).toString();

        string filter;
        DynamicGroup::makeFilter(filter, type.toUtf8().constData(),
                                keyword.toUtf8().constData());
        filters.push_back(filter);
    }
    newState->setFilter(filters);

    if (obj->cmp(newState, true)) {
        delete change;
    } else {
        m_project->undoStack->push(change);
    }

    m_loadedObjFilter = filters;
}


void DynamicGroupDialog::loadObjFilter()
{
    DynamicGroup *objGroup = dynamic_cast<DynamicGroup *>(obj);

    /* We remove the rows instead of calling clear so the headers
       don't flicker. */
    //m_model->clear();
    while (m_model->rowCount() > 0) {
        m_model->removeRow(0);
    }

    QStringList headers;
    headers << "Del" << "Type" << "Keyword";
    m_model->setHorizontalHeaderLabels(headers);

    const list<string> &filter = objGroup->getFilter();
    list<string>::const_iterator iter;
    for (iter = filter.begin(); iter != filter.end(); ++iter) {
        string type, keyword;
        objGroup->splitFilter(*iter, type, keyword);

        QList<QStandardItem *> items;
        items << new QStandardItem(QString());
        items << new QStandardItem(QString::fromUtf8(type.c_str()));
        items << new QStandardItem(QString::fromUtf8(keyword.c_str()));
        m_model->appendRow(items);

        int row = m_model->rowCount() - 1;
        m_ui.criteriaView->openPersistentEditor(m_model->index(row, 0));
        m_ui.criteriaView->openPersistentEditor(m_model->index(row, 1));
        m_ui.criteriaView->openPersistentEditor(m_model->index(row, 2));
    }

    QHeaderView *header = m_ui.criteriaView->horizontalHeader();
    /* Try to force at least some minimum size, as the
       ResizeToContents doesn't always seem to work */
    header->resizeSection(0, 35);
    header->setStretchLastSection(true);
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_reloadObjFilter = false;
    m_loadedObjFilter = filter;
    m_loadedAllKeywords = obj->getAllKeywords();
}


void DynamicGroupDialog::loadFWObject(FWObject *o)
{
    DynamicGroup *objGroup = dynamic_cast<DynamicGroup *>(o);

    const list<string> &filter = objGroup->getFilter();
    if (obj != o || m_reloadObjFilter || m_loadedObjFilter != filter ||
        m_loadedAllKeywords != o->getAllKeywords()) {
        obj = o;
        loadObjFilter();
    }

    m_ui.nameEdit->setText(QString::fromUtf8(o->getName().c_str()));
    m_ui.commentKeywords->loadFWObject(o);

    FWObjectDatabase *root = obj->getRoot();
    m_ui.matchedView->clear();
    m_ui.matchedView->setDB(root);

    FWObject::tree_iterator tree_iter;
    for (tree_iter = root->tree_begin();
         tree_iter != root->tree_end(); ++tree_iter) {
        FWObject *elem = (*tree_iter);
        if (elem == root) continue;

        if (!objGroup->isMemberOfGroup(elem)) continue;

        QTreeWidgetItem *item = new QTreeWidgetItem(m_ui.matchedView);
        item->setText(0, QString::fromUtf8(elem->getName().c_str()));
        item->setText(1, FWObjectPropertiesFactory::getObjectProperties(elem));
        item->setData(0, Qt::UserRole, QVariant(elem->getId()));
        QString icon = ":/Icons/";
        icon += elem->getTypeName().c_str();
        icon += "/icon-ref";

        QPixmap pixmap;
        if (!QPixmapCache::find(icon, pixmap)) {
            pixmap.load(icon);
            QPixmapCache::insert(icon, pixmap);
        }
        item->setIcon(0, QIcon(pixmap));

        m_ui.matchedView->addTopLevelItem(item);
    }
}


void DynamicGroupDialog::validate(bool *result)
{
    Q_UNUSED(result)
}


void DynamicGroupDialog::addMatchClicked()
{
    int newRow = m_model->rowCount();
    QList<QStandardItem *> items;
    items << new QStandardItem("")
          << new QStandardItem(DynamicGroup::TYPE_NONE)
          << new QStandardItem(DynamicGroup::KEYWORD_NONE);
    m_model->insertRow(newRow, items);

    m_ui.criteriaView->openPersistentEditor(m_model->index(newRow, 0));
    m_ui.criteriaView->openPersistentEditor(m_model->index(newRow, 1));
    m_ui.criteriaView->openPersistentEditor(m_model->index(newRow, 2));

    m_ui.criteriaView->resizeColumnsToContents();

    QHeaderView *header = m_ui.criteriaView->horizontalHeader();
    header->setStretchLastSection(true);

    m_ui.criteriaView->scrollToBottom();

    /* How come insertRow() doesn't do this for us? */
    emit changed();
}


void DynamicGroupDialog::deleteFilterClicked()
{
    QToolButton *button = dynamic_cast<QToolButton *>(sender());

    int row = button->property("row").toInt();
    m_model->removeRow(row);

    m_reloadObjFilter = true;
    emit changed();
}


void DynamicGroupDialog::gotItemDoubleClicked(QTreeWidgetItem *item, int)
{
    int objId = item->data(0, Qt::UserRole).toInt();
    FWObject *o = m_project->db()->findInIndex(objId);
    if (o == nullptr) return;

    QCoreApplication::postEvent(m_project, new showObjectInTreeEvent(o->getRoot()->getFileName().c_str(), objId));
    QCoreApplication::postEvent(mw, new openObjectInEditorEvent(o->getRoot()->getFileName().c_str(), objId));
}

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

#include "global.h"

#include "ObjectSelectorWidget.h"
#include "ObjectDescriptor.h"
#include "FilterDialog.h"

#include <QProgressDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


ObjectSelectorWidget::ObjectSelectorWidget(QWidget *parent) : QWidget(parent)
{
    m_dialog = new Ui::ObjectSelectorWidget_q;
    m_dialog->setupUi(this);

    flt_obj = new Filter();
    flt_obj_d = new FilterDialog(this);
    flt_obj_d->setFilter(flt_obj);
}

ObjectSelectorWidget::~ObjectSelectorWidget()
{
    delete flt_obj;
    delete flt_obj_d;
}

void ObjectSelectorWidget::init(const QList<ObjectDescriptor> &objects)
{
    this->objects = objects;
    fillListOfObjects();
}

void ObjectSelectorWidget::updateObjectsToUse()
{
    objects_to_use.clear();
    QListWidgetItem* item = m_dialog->objectList->item(0);
    while (item != nullptr)
    {
        QString name = item->text().split(" ")[0];
        QString addr = item->data(Qt::UserRole).toString();
        objects_to_use << name << addr;
        item = m_dialog->objectList->item(m_dialog->objectList->row(item)+1);
    }
}

void ObjectSelectorWidget::fillListOfObjects()
{
    m_dialog->objectResultList->clear();

    foreach(ObjectDescriptor od, objects)
    {
        QString addr = od.addr.toString().c_str();
        QString name = QString::fromUtf8(od.sysname.c_str());
        
        if ( flt_obj->test(od) )
        {
            QString item_text("%1 %2");
            QListWidgetItem *itm = new QListWidgetItem(item_text.arg(name).arg(addr));
            itm->setData(Qt::UserRole, QVariant(addr));
            m_dialog->objectResultList->addItem(itm);
        }
    }
    updateObjectsToUse();
    emit selectionChanged();
}

void ObjectSelectorWidget::addFilter()
{
    flt_obj_d->exec();
    fillListOfObjects();
}

void ObjectSelectorWidget::removeFilter()
{
    flt_obj->clear();
    fillListOfObjects();
}

void ObjectSelectorWidget::selectAllResults()
{
    m_dialog->objectResultList->selectAll();
}

void ObjectSelectorWidget::unselectAllResults()
{
    m_dialog->objectResultList->clearSelection();
}

void ObjectSelectorWidget::selectAllUsed()
{
    m_dialog->objectList->selectAll();
}

void ObjectSelectorWidget::unselectAllUsed()
{
    m_dialog->objectList->clearSelection();
}

void ObjectSelectorWidget::addObject()
{
    QListWidgetItem* item = (QListWidgetItem*)m_dialog->objectResultList->item(0);
    int i = 0;

    while (item)
    {
        if (item->isSelected() &&
            m_dialog->objectList->findItems(
                item->text(), Qt::MatchExactly).size() == 0)
        {
            QString addr = item->data(Qt::UserRole).toString();
            QListWidgetItem *item2 = new QListWidgetItem(item->text());
            item2->setData(Qt::UserRole, QVariant(addr));
            m_dialog->objectList->addItem(item2);
        }

        i++;
        item = (QListWidgetItem*)m_dialog->objectResultList->item(i);
    }
    updateObjectsToUse();
    emit selectionChanged();
}

void ObjectSelectorWidget::removeObject()
{
    QListWidgetItem* item1 = m_dialog->objectList->item(0);
    QListWidgetItem* item2;

    while (item1!=nullptr)
    {
        item2 = m_dialog->objectList->item(m_dialog->objectList->row(item1)+1);
        if (item1->isSelected())
        {
            delete item1;
        }
        item1 = item2;
    }
    updateObjectsToUse();
    emit selectionChanged();
}


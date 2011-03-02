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

#include "../../config.h"
#include "global.h"

#include "ChooseObjectsPage.h"
#include "HostsFile.h"

#include "ObjectDescriptor.h"
#include "FilterDialog.h"

#include <QProgressDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


ChooseObjectsPage::ChooseObjectsPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ChooseObjectsPage_q;
    m_dialog->setupUi(this);

    flt_obj = new Filter();
    flt_obj_d = new FilterDialog(this);
    flt_obj_d->setFilter(flt_obj);

    registerField("objectsToUse*", this, "objectsToUse");

//    setField("objects_to_use", objects_to_use);
}

void ChooseObjectsPage::initializePage()
{
    if (fwbdebug)
        qDebug() << "ChooseObjectsPage::initializePage()"
                 << "file name" << field("fileName").toString();

    try
    {
        HostsFile importer(field("fileName").toString());
        importer.parse();
        reverse_hosts = importer.getAll();

        fillListOfObjects();
    } catch (FWException &ex)
    {
        QMessageBox::critical( NULL , "Firewall Builder",
                               ex.toString().c_str(),
                               QString::null,QString::null);
    }
}

bool ChooseObjectsPage::validatePage()
{
    if (fwbdebug) qDebug() << "ChooseObjectsPage::validatePage()";
    return true;
}

void ChooseObjectsPage::updateObjectsToUse()
{
    QListWidgetItem* item = m_dialog->objectList->item(0);
    while (item != NULL)
    {
        QString name = item->text().split(" ")[0];
        QString addr = item->data(Qt::UserRole).toString();
        objects_to_use << name << addr;
        item = m_dialog->objectList->item(m_dialog->objectList->row(item)+1);
    }
    //setField("objects_to_use", objects_to_use);
}

bool ChooseObjectsPage::isComplete() const
{
    if (fwbdebug) qDebug() << "ChooseObjectsPage::isComplete()";
    return (objects_to_use.count() > 0);
}

void ChooseObjectsPage::fillListOfObjects()
{
    m_dialog->objectResultList->clear();
    map<InetAddr, QStringList>::iterator i;
    for (i=reverse_hosts.begin(); i!=reverse_hosts.end(); ++i)
    {
        ObjectDescriptor od;
        od.addr = i->first;
        od.sysname = i->second.front().toUtf8().constData();

        QString addr = i->first.toString().c_str();
        QString name = i->second.front();
        
        if ( flt_obj->test(od) )
        {
            QString item_text("%1 %2");
            QListWidgetItem *itm = new QListWidgetItem(item_text.arg(name).arg(addr));
            itm->setData(Qt::UserRole, QVariant(addr));
            m_dialog->objectResultList->addItem(itm);
        }
    }
    updateObjectsToUse();
    emit completeChanged();
}

void ChooseObjectsPage::addFilter()
{
    flt_obj_d->exec();
    fillListOfObjects();
}

void ChooseObjectsPage::removeFilter()
{
    flt_obj->clear();
    fillListOfObjects();
}

void ChooseObjectsPage::selectAllResults()
{
    m_dialog->objectResultList->selectAll();
}

void ChooseObjectsPage::unselectAllResults()
{
    m_dialog->objectResultList->clearSelection();
}

void ChooseObjectsPage::selectAllUsed()
{
    m_dialog->objectList->selectAll();
}

void ChooseObjectsPage::unselectAllUsed()
{
    m_dialog->objectList->clearSelection();
}

void ChooseObjectsPage::addObject()
{
    QListWidgetItem* item = (QListWidgetItem*)m_dialog->objectResultList->item(0);
    int i = 0;

    while (item)
    {
        if (item->isSelected())
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
    emit completeChanged();
}

void ChooseObjectsPage::removeObject()
{
    QListWidgetItem* item1 = m_dialog->objectList->item(0);
    QListWidgetItem* item2;

    while (item1!=0)
    {
        item2 = m_dialog->objectList->item(m_dialog->objectList->row(item1)+1);
        if (item1->isSelected())
        {
            delete item1;
        }
        item1 = item2;
    }
    updateObjectsToUse();
    emit completeChanged();
}


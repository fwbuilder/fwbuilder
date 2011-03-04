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

#ifndef __OBJECTSELECTORWIDGET_H_
#define __OBJECTSELECTORWIDGET_H_

#include "ui_objectselectorwidget_q.h"

#include "ObjectDescriptor.h"

#include "fwbuilder/InetAddr.h"

#include <QStringList>

class Filter;
class FilterDialog;


class ObjectSelectorWidget : public QWidget
{
    Q_OBJECT;

    Ui::ObjectSelectorWidget_q *m_dialog;

    Filter * flt_obj;
    FilterDialog * flt_obj_d;
    QList<ObjectDescriptor> objects;
    QStringList objects_to_use;

    // configure this as a proprty so it can be accessed as a field after
    // registering with registerField(). Now it can be accessed from
    // other pages of the wizard
    Q_PROPERTY(QStringList objectsToUse READ getObjectsToUse WRITE setObjectsToUse);
    
public:
    ObjectSelectorWidget(QWidget *parent);
    virtual ~ObjectSelectorWidget();

    void init(const QList<ObjectDescriptor> &objects);

    int count() { return objects_to_use.count(); }
    
    void fillListOfObjects();
    void updateObjectsToUse();

    QStringList getObjectsToUse() { return objects_to_use; }
    void setObjectsToUse(const QStringList &lst) { objects_to_use = lst; }
    
public slots:

    void addFilter();
    void removeFilter();
    void selectAllResults();
    void unselectAllResults();
    void selectAllUsed();
    void unselectAllUsed();
    void addObject();
    void removeObject();

signals:
    void selectionChanged();
};


#endif

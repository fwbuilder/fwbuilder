/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

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

#ifndef CLUSTERINTERFACEWIDGET_H
#define CLUSTERINTERFACEWIDGET_H

#include <qglobal.h>

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTreeWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>

#include "global.h"

#include <fwbuilder/Resources.h>
#include <fwbuilder/Cluster.h>
#include <fwbuilder/Firewall.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/InterfaceData.h>
#include <interfaceProperties.h>
#include <interfacePropertiesObjectFactory.h>

#include <memory>

class ClusterInterfacesSelectorWidget;
struct ClusterInterfaceData;

#include "ClusterInterfacesSelectorWidget.h"

Q_DECLARE_METATYPE(libfwbuilder::Interface*)

namespace Ui {
    class ClusterInterfaceWidget;
}

struct InterfacesList
{
    QVBoxLayout *layout;
    QTreeWidget *list;
    QLabel *label;
    libfwbuilder::Firewall *firewall;
};

class ClusterInterfaceWidget : public QWidget {
    Q_OBJECT
public:
    ClusterInterfaceWidget(QWidget *parent = 0);
    ~ClusterInterfaceWidget();
    void setFirewallList(QList<libfwbuilder::Firewall*>);
    bool setCurrentInterface(const QString&);
    ClusterInterfaceData getInterfaceData();
    bool interfaceSelectable(libfwbuilder::Interface*);
    bool isValid();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ClusterInterfaceWidget *m_ui;
    QMap<libfwbuilder::Firewall*, InterfacesList> lists;
    ClusterInterfacesSelectorWidget *cisw;
    QString os;
    QMap<QTreeWidget*, QTreeWidgetItem*> roots;
    QHBoxLayout* interfaceBox;

public slots:
    void nameChanged(QString);
};

#endif // CLUSTERINTERFACEWIDGET_H

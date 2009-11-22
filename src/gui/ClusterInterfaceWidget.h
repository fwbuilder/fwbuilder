/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

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

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QVBoxLayout>

#include <fwbuilder/Firewall.h>
#include <fwbuilder/Interface.h>

class ClusterInterfacesSelectorWidget;
struct ClusterInterfaceData;

#include "ClusterInterfacesSelectorWidget.h"

namespace Ui {
    class ClusterInterfaceWidget;
}

struct InterfacesList
{
    QVBoxLayout *layout;
    QListWidget *list;
    QLabel *label;
    libfwbuilder::Firewall *firewall;
};

class ClusterInterfaceWidget : public QWidget {
    Q_OBJECT
public:
    ClusterInterfaceWidget(QWidget *parent = 0);
    ~ClusterInterfaceWidget();
    void setFirewallList(QList<libfwbuilder::Firewall*>);
    void setCurrentInterface(QString);
    ClusterInterfaceData getInterfaceData();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ClusterInterfaceWidget *m_ui;
    QMap<libfwbuilder::Firewall*, InterfacesList> lists;
    ClusterInterfacesSelectorWidget *cisw;

public slots:
    void nameChanged(QString);
};

#endif // CLUSTERINTERFACEWIDGET_H

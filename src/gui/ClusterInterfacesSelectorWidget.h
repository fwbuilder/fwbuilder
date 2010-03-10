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

#ifndef CLUSTERINTERFACESSELECTORWIDGET_H
#define CLUSTERINTERFACESSELECTORWIDGET_H

#include <QTabWidget>
#include <QToolButton>
#include <QSet>
#include <fwbuilder/Firewall.h>
#include <fwbuilder/Interface.h>

class ClusterInterfaceWidget;

#include "ClusterInterfaceWidget.h"

struct ClusterInterfaceData
{
    QString os;
    QString name;
    QString label;
    QString comment;
    QList<QPair<libfwbuilder::Firewall*, libfwbuilder::Interface*> > interfaces;
};

class ClusterInterfacesSelectorWidget : public QTabWidget
{
    Q_OBJECT
public:
    ClusterInterfacesSelectorWidget(QWidget *parent = 0);
    ~ClusterInterfacesSelectorWidget();
    void setFirewallList(QList<libfwbuilder::Firewall*>);
    QList<ClusterInterfaceData> getInterfaces();
    bool isValid();

private:
    QList<ClusterInterfaceWidget*> editors;
    QList<libfwbuilder::Firewall*> fwlist;
    QToolButton newInterface;
    QToolButton delInterface;
    bool noTabs;

public slots:
    ClusterInterfaceWidget* addNewInterface();
    void addInterface(const QString& name);
    void closeTab();
    void clear();

};

#endif // CLUSTERINTERFACESSELECTORWIDGET_H

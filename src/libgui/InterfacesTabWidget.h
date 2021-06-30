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

#ifndef INTERFACESTABWIDGET_H
#define INTERFACESTABWIDGET_H

#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QHBoxLayout>

#include "fwbuilder/Interface.h"
#include "fwbuilder/InterfaceData.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "InterfaceEditorWidget.h"
#include "ClusterInterfaceWidget.h"

namespace Ui {
    class InterfacesTabWidget;
}

struct AddressInfo;

struct EditedInterfaceData
{
    QString name;
    QString label;
    QString comment;
    QString mac;
    int type; // 0 - regular, 1 - dynamic, 2 - unnumbered
    QString protocol;
    QMultiMap<libfwbuilder::Address*, AddressInfo > addresses;
};

class InterfacesTabWidget : public QTabWidget {
    Q_OBJECT;

public:
    InterfacesTabWidget(QWidget *parent = 0);
    ~InterfacesTabWidget();
    QMap<libfwbuilder::Interface*, EditedInterfaceData> getData();
    QList<EditedInterfaceData> getNewData();
    QList<libfwbuilder::Interface*> getDeletedInterfaces();
    bool isValid();
    void setCornerWidgetsVisible(bool);
    void setClusterMode(bool);
    void setHostOS(const QString &s);
    
protected:
    void changeEvent(QEvent *e);

private:
    Ui::InterfacesTabWidget *m_ui;
    QToolButton newInterface;
    QToolButton delInterface;
    QWidget newInterfaceWidget;
    QHBoxLayout newInterfaceLayout;
    libfwbuilder::FWObject *currentTemplate;
    QList<libfwbuilder::Interface*> deleted;
    bool clusterMode;
    QString explanation;
    bool noTabs;
    QString host_OS;
    
public slots:
    void addInterface(libfwbuilder::Interface*);
    void clear();
    void addNewInterface();
    void setTemplate(libfwbuilder::FWObject*);
    void closeTab();
    void addInterfaceFromData(libfwbuilder::InterfaceData*);
    void addClusterInterface(ClusterInterfaceData);
    void setExplanation(const QString&);
    virtual void addTab(QWidget*, const QString&);
    
};

#endif // INTERFACESTABWIDGET_H

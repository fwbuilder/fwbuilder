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

#ifndef INTERFACEEDITORWIDGET_H
#define INTERFACEEDITORWIDGET_H

#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include "InterfacesTabWidget.h"
#include "FWBSettings.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>

#include <QTableWidgetItem>
#include <QMap>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>

#include "fwbuilder/Resources.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/StateSyncClusterGroup.h"

namespace Ui {
    class InterfaceEditorWidget;
}

struct EditedInterfaceData;
struct ClusterInterfaceData;

struct AddressInfo
{
    bool ipv4;
    QString address;
    QString netmask;
};

class InterfaceEditorWidget : public QWidget {
    Q_OBJECT;

public:
    InterfaceEditorWidget(QWidget *parent, libfwbuilder::InterfaceData* data);
    InterfaceEditorWidget(QWidget *parent, libfwbuilder::Interface *iface);
    InterfaceEditorWidget(QWidget *parent, ClusterInterfaceData data);
    InterfaceEditorWidget(QWidget *parent);
    ~InterfaceEditorWidget();
    void setData(libfwbuilder::InterfaceData *data);
    libfwbuilder::Interface* getInterface();
    EditedInterfaceData getInterfaceData();
    bool isValid();
    void updateColumnsSizes();
    void setClusterMode(bool);
    void setProtocolIndex(int);
    void setHostOS(const QString &s);

private:
    QTabWidget *tabw;
    QToolButton *addAddr, *delAddr;
    libfwbuilder::Interface *interfacep;
    Ui::InterfaceEditorWidget *m_ui;
    QMap<QPushButton*, QPair<QTableWidgetItem*, QTableWidgetItem*> > buttons;
    QMap<int, QPair<QTableWidgetItem*, QTableWidgetItem*> > rows;
    QMap<int, QComboBox*> types;
    QMap<int, libfwbuilder::Address*> fwaddrs;
    bool validateAddress(const QString &addr, const QString &netm, bool regular, bool ipv4);
    bool clusterMode;
    QString os, errorTitle, errorText;
    void setError(const QString &title, const QString &text);


public slots:
    int addNewAddress();
    int addNewAddress(QString address, QString netmask, bool ipv4 = true);
    void deleteAddress();
    void nameEdited(QString);
    void typeChanged(int);
    void addressChanged(int, int);
    void protocolChanged(QString);
    void setExplanation(const QString&);
    void showError();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent ( QResizeEvent * );
};

#endif // INTERFACEEDITORWIDGET_H

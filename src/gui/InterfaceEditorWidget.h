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

#ifndef INTERFACEEDITORWIDGET_H
#define INTERFACEEDITORWIDGET_H

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include "InterfacesTabWidget.h"

#include <QtGui/QWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QTableWidgetItem>
#include <QMap>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>

#include "fwbuilder/Interface.h"

namespace Ui {
    class InterfaceEditorWidget;
}

struct EditedInterfaceData;

struct AddressInfo
{
    bool ipv4;
    QString address;
    QString netmask;
};

class InterfaceEditorWidget : public QWidget {
    Q_OBJECT
public:
    InterfaceEditorWidget(QWidget *parent, libfwbuilder::InterfaceData* data, int v);
    InterfaceEditorWidget(QWidget *parent, libfwbuilder::Interface *interface);
    InterfaceEditorWidget(QWidget *parent);
    ~InterfaceEditorWidget();
    void setData(libfwbuilder::InterfaceData *data);
    libfwbuilder::Interface* getInterface();
    EditedInterfaceData getInterfaceData();
    bool isValid();
    void updateColumnsSizes();

protected:
    void changeEvent(QEvent *e);

private:
    QTabWidget *tabw;
    QToolButton *addAddr, *delAddr;
    libfwbuilder::Interface *interface;
    Ui::InterfaceEditorWidget *m_ui;
    QMap<QPushButton*, QPair<QTableWidgetItem*, QTableWidgetItem*> > buttons;
    QMap<int, QPair<QTableWidgetItem*, QTableWidgetItem*> > rows;
    QMap<int, QComboBox*> types;
    QMap<int, libfwbuilder::Address*> fwaddrs;
    bool validateAddress(const QString &addr, const QString &netm, bool regular, bool ipv4);


public slots:
    int addNewAddress();
    void deleteAddress();
    void nameEdited(QString);
    void typeChanged(int);
    void addressChanged(int, int);

protected:
    virtual void resizeEvent ( QResizeEvent * );
};

#endif // INTERFACEEDITORWIDGET_H

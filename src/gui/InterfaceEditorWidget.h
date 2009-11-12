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
    InterfaceEditorWidget(QWidget *parent, libfwbuilder::Interface *interface);
    InterfaceEditorWidget(QWidget *parent, libfwbuilder::FWObjectDatabase* db);
    ~InterfaceEditorWidget();
    libfwbuilder::Interface* getInterface();
    EditedInterfaceData getInterfaceData();

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

public slots:
    int addNewAddress();
    void deleteAddress();
    void nameEdited(QString);
};

#endif // INTERFACEEDITORWIDGET_H

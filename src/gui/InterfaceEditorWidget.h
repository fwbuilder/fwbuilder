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

using namespace libfwbuilder;

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
    InterfaceEditorWidget(QWidget *parent, Interface *interface);
    InterfaceEditorWidget(QWidget *parent, InterfaceData* data);
    InterfaceEditorWidget(QWidget *parent);
    ~InterfaceEditorWidget();
    Interface* getInterface();
    EditedInterfaceData getInterfaceData();
    bool isValid();

protected:
    void changeEvent(QEvent *e);

private:
    QTabWidget *tabw;
    QToolButton *addAddr, *delAddr;
    Interface *interface;
    Ui::InterfaceEditorWidget *m_ui;
    QMap<QPushButton*, QPair<QTableWidgetItem*, QTableWidgetItem*> > buttons;
    QMap<int, QPair<QTableWidgetItem*, QTableWidgetItem*> > rows;
    QMap<int, QComboBox*> types;
    QMap<int, Address*> fwaddrs;
    bool validateAddress(const QString &addr, const QString &netm, bool regular);

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

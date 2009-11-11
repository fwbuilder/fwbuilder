#ifndef INTERFACEEDITORWIDGET_H
#define INTERFACEEDITORWIDGET_H

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include "AddressEditor.h"
#include "InterfacesTabWidget.h"

#include <QtGui/QWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>

#include "fwbuilder/Interface.h"

namespace Ui {
    class InterfaceEditorWidget;
}

struct EditedInterfaceData;

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
    QToolButton *addAddr;
    void setupUI();
    libfwbuilder::Interface *interface;
    Ui::InterfaceEditorWidget *m_ui;

public slots:
    void nameEdited(QString);
    void addNewAddress();
    void closeTab(int);
};

#endif // INTERFACEEDITORWIDGET_H

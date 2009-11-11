#ifndef INTERFACEEDITOR_H
#define INTERFACEEDITOR_H

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include "AddressEditor.h"
#include "InterfacesEditor.h"

#include <QtGui/QWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>

#include "fwbuilder/Interface.h"

namespace Ui {
    class InterfaceEditor;
}

struct EditedInterfaceData;

class InterfaceEditor : public QWidget {
    Q_OBJECT
public:
    InterfaceEditor(QWidget *parent, libfwbuilder::Interface *interface);
    InterfaceEditor(QWidget *parent, libfwbuilder::FWObjectDatabase* db);
    ~InterfaceEditor();
    libfwbuilder::Interface* getInterface();
    EditedInterfaceData getInterfaceData();

protected:
    void changeEvent(QEvent *e);

private:
    QTabWidget *tabw;
    QToolButton *addAddr;
    void setupUI();
    libfwbuilder::Interface *interface;
    Ui::InterfaceEditor *m_ui;

public slots:
    void nameEdited(QString);
    void addNewAddress();
};

#endif // INTERFACEEDITOR_H

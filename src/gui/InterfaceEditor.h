#ifndef INTERFACEEDITOR_H
#define INTERFACEEDITOR_H

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include "AddressEditor.h"
#include "newFirewallDialog.h"

#include <QtGui/QWidget>

#include "fwbuilder/Interface.h"

namespace Ui {
    class InterfaceEditor;
}

class InterfaceEditor : public QWidget {
    Q_OBJECT
public:
    InterfaceEditor(QWidget *parent, libfwbuilder::Interface *interface);
    ~InterfaceEditor();
    libfwbuilder::Interface* getInterface();
    EditedInterfaceData getInterfaceData();

protected:
    void changeEvent(QEvent *e);

private:
    libfwbuilder::Interface *interface;
    Ui::InterfaceEditor *m_ui;

public slots:
    void nameEdited(QString);
};

#endif // INTERFACEEDITOR_H

#ifndef INTERFACEEDITOR_H
#define INTERFACEEDITOR_H

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include <QtGui/QWidget>

#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/BackgroundOp.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

namespace Ui {
    class InterfaceEditor;
}

class InterfaceEditor : public QWidget {
    Q_OBJECT
public:
    InterfaceEditor(QWidget *parent, libfwbuilder::Interface *interface);
    ~InterfaceEditor();

protected:
    void changeEvent(QEvent *e);

private:
    libfwbuilder::Interface *interface;
    Ui::InterfaceEditor *m_ui;
};

#endif // INTERFACEEDITOR_H

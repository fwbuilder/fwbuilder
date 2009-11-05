#ifndef ADDRESSEDITOR_H
#define ADDRESSEDITOR_H

#include <QtGui/QWidget>

namespace Ui {
    class AddressEditor;
}

#include "fwbuilder/Address.h"

class AddressEditor : public QWidget {
    Q_OBJECT
public:
    AddressEditor(QWidget *parent, libfwbuilder::Address *address);
    ~AddressEditor();

protected:
    void changeEvent(QEvent *e);

private:
    libfwbuilder::Address* address;
    Ui::AddressEditor *m_ui;
};

#endif // ADDRESSEDITOR_H

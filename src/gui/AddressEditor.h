#ifndef ADDRESSEDITOR_H
#define ADDRESSEDITOR_H

#include <QtGui/QWidget>
#include <QButtonGroup>

namespace Ui {
    class AddressEditor;
}

#include "fwbuilder/Address.h"

struct AddressInfo
{
    bool ipv4;
    QString address;
    QString netmask;
};

class AddressEditor : public QWidget {
    Q_OBJECT
public:
    AddressEditor(libfwbuilder::Address *address, QWidget *parent);
    AddressEditor(QWidget *parent);
    ~AddressEditor();
    AddressInfo getEditedData();
    libfwbuilder::Address* getAddress();

protected:
    void changeEvent(QEvent *e);

private:
    libfwbuilder::Address* address;
    Ui::AddressEditor *m_ui;
};

#endif // ADDRESSEDITOR_H

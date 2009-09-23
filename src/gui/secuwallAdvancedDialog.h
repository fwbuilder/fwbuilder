/*
 * secuwallAdvancedDialog.h - secunet wall advanced host OS settings dialog
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#ifndef __SECUWALLADVANCEDDIALOG_H_
#define __SECUWALLADVANCEDDIALOG_H_

#include <ui_secuwalladvanceddialog_q.h>
#include "DialogData.h"
#include <QDialog>

namespace libfwbuilder
{
    class FWObject;
};

class secuwallAdvancedDialog : public QDialog
{
    Q_OBJECT

    libfwbuilder::FWObject *obj;
    DialogData data;
    Ui::secuwallAdvancedDialog_q *m_dialog;

public:
    secuwallAdvancedDialog(QWidget *parent, libfwbuilder::FWObject *o);
    ~secuwallAdvancedDialog();

private:
    /** validate user input */
    bool validate();

    /**
     * validate a given IP address / netmask
     *
     * valid: 192.168.1.1
     * valid: 192.168.1.1/32
     * valid: 192.168.1.1/255.255.255.0
     *
     * @param addr address to verify
     * @return true if valid, false if not
     */
    bool validateAddress(const QString &addr);

protected slots:
    virtual void accept();
    virtual void reject();
    virtual void help();
    void additionalChanged(int state);
    void buttonBrowseClicked();
    void buttonOpenURLClicked();
};

#endif // __SECUWALLADVANCEDDIALOG_H


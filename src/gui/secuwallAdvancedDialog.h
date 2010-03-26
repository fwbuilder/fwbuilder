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

namespace libfwbuilder {
    class FWObject;
};

class secuwallAdvancedDialog : public QDialog
{
    Q_OBJECT

    libfwbuilder::FWObject *obj;
    DialogData data;
protected:
    Ui::secuwallAdvancedDialog_q *m_dialog;

 public:
    secuwallAdvancedDialog(QWidget *parent,libfwbuilder::FWObject *o);
    ~secuwallAdvancedDialog();

protected slots:

    virtual void accept();
    virtual void reject();
    virtual void help();

    virtual void editProlog();
    virtual void editEpilog();


public slots:
    virtual void switchLOG_ULOG();
};

#endif // __SECUWALLADVANCEDDIALOG_H

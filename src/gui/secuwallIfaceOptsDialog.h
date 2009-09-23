/*
 * secuwallIfaceOptsDialog.h - Interface options dialog for secunet wall
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

#ifndef __SECUWALLIFACEOPTSDIALOG_H_
#define __SECUWALLIFACEOPTSDIALOG_H_

#include <ui_secuwallifaceoptsdialog_q.h>
#include "DialogData.h"
#include <QDialog>

namespace libfwbuilder
{
    class FWObject;
};

class secuwallIfaceOptsDialog : public QDialog
{
    Q_OBJECT

public:
    secuwallIfaceOptsDialog(QWidget *parent, libfwbuilder::FWObject *o);
    ~secuwallIfaceOptsDialog();

private:
    libfwbuilder::FWObject *obj;
    DialogData data;
    Ui::secuwallIfaceOptsDialog_q *m_dialog;
    bool cluster_interface;

protected slots:
    virtual void accept();
    virtual void reject();
    virtual void help();
    void typeChanged(const QString&);
};

#endif // __SECUWALLIFACEOPTSDIALOG_H_


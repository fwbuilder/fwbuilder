/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id$

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef __VLANONLYIFACEOPTSDIALOG_H_
#define __VLANONLYIFACEOPTSDIALOG_H_

#include <ui_vlanonlyifaceoptsdialog_q.h>
#include "DialogData.h"
#include <QDialog>

namespace libfwbuilder
{
    class FWObject;
};

class vlanOnlyIfaceOptsDialog : public QDialog
{
    Q_OBJECT

public:
    vlanOnlyIfaceOptsDialog(QWidget *parent, libfwbuilder::FWObject *o);
    ~vlanOnlyIfaceOptsDialog();

private:
    libfwbuilder::FWObject *obj;
    DialogData data;
    Ui::vlanOnlyIfaceOptsDialog_q *m_dialog;
    bool cluster_interface;

    /** validate user input for different interface types */
    bool validate();

protected slots:
    virtual void accept();
    virtual void reject();
    virtual void help();
    void typeChanged(const QString &new_type);
};

#endif // __VLANONLYIFACEOPTSDIALOG_H_


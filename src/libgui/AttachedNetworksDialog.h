/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#ifndef __ATTACHEDNETWORKSDIALOG_H_
#define __ATTACHEDNETWORKSDIALOG_H_

#include <ui_attachednetworksdialog_q.h>
#include "BaseObjectDialog.h"
#include <QWidget>

#include "fwbuilder/FWObject.h"


class ProjectPanel;

namespace libfwbuilder
{
    class InetAddr;
};

class AttachedNetworksDialog : public BaseObjectDialog
{
    Q_OBJECT;

    Ui::AttachedNetworksDialog_q *m_dialog;

    void addAddressToList(const libfwbuilder::InetAddr *ip_addr,
                          const libfwbuilder::InetAddr *ip_netm);
    
 public:
    AttachedNetworksDialog(QWidget *parent);
    ~AttachedNetworksDialog();
    
public slots:
    virtual void applyChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
};

#endif // ATTACHEDNETWORKSDIALOG_H

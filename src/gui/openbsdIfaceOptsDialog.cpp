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


#include "openbsdIfaceOptsDialog.h"
#include "platforms.h"

#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Firewall.h"

#include "FWWindow.h"
#include "Help.h"

#include <qmessagebox.h>

using namespace std;
using namespace libfwbuilder;

openbsdIfaceOptsDialog::openbsdIfaceOptsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::openbsdIfaceOptsDialog_q;
    m_dialog->setupUi(this);
    obj = o;

    FWOptions *ifopt = (Interface::cast(obj))->getOptionsObject();
    cluster_interface = (Cluster::cast(obj->getParent()) != NULL);

    setInterfaceTypes(m_dialog->iface_type, Interface::cast(obj),
                      ifopt->getStr("type").c_str());

    // Using "type" control only for subinterfaces
    // and main interfaces of the firewall objects
    if (cluster_interface)
    {
        m_dialog->iface_type->hide();
        m_dialog->iface_type_label->hide();
    } else
    {
        m_dialog->iface_type->show();
        m_dialog->iface_type_label->show();
    }

    data.registerOption(m_dialog->vlan_id, ifopt, "vlan_id");

    data.loadAll();

    // special actions for different iface types
    // VLAN (8021q)
    typeChanged("");
}

openbsdIfaceOptsDialog::~openbsdIfaceOptsDialog()
{
    delete m_dialog;
}

/*
 * store all data in the object
 */
void openbsdIfaceOptsDialog::accept()
{
    // validate user input before saving
    if (!validate())  return;

    FWOptions *ifopt = (Interface::cast(obj))->getOptionsObject();
    assert(ifopt != NULL);

    if (cluster_interface)
    {
        ifopt->setStr("type", "cluster_interface");
    } else
    {
        QString new_type = m_dialog->iface_type->itemData(
            m_dialog->iface_type->currentIndex()).toString();
        ifopt->setStr("type", new_type.toStdString());
    }

    data.saveAll();

    QDialog::accept();
}

void openbsdIfaceOptsDialog::reject()
{
    QDialog::reject();
}

void openbsdIfaceOptsDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
                            m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = new Help(this, "Interface OpenBSD");
    h->setSource(QUrl("openbsdIfaceOptsDialog.html#" + anchor));
    h->show();
}

void openbsdIfaceOptsDialog::typeChanged(const QString&)
{
    QString new_type = m_dialog->iface_type->itemData(
        m_dialog->iface_type->currentIndex()).toString();

    // enable VLAN ID line edit for type VLAN
    if (new_type == "8021q")
    {
        m_dialog->options_stack->setCurrentIndex(1);
        return;
    }

    // page 0 is empty
    m_dialog->options_stack->setCurrentIndex(0);
}

bool openbsdIfaceOptsDialog::validate()
{
    return true;
}


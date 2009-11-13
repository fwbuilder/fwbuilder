/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "../../config.h"
#include "global.h"
#include "platforms.h"

#include "newFirewallDialog.h"
#include "FWBSettings.h"
#include "FWBTree.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"


using namespace libfwbuilder;
using namespace std;


void newFirewallDialog::createFirewallFromTemplate()
{
    QListWidgetItem *itm = m_dialog->templateList->currentItem();
    FWObject *template_fw=templates[itm];
    assert (template_fw!=NULL);

    string platform = readPlatform(m_dialog->platform).toAscii().constData();
    string host_os = readHostOS(m_dialog->hostOS).toAscii().constData();


    FWObject *no;
    no = db->create(Firewall::TYPENAME);

    if (no==NULL)
    {
        QDialog::accept();
        return;
    }

    parent->add(no);
    no->duplicate(template_fw, true);
    no->setName(m_dialog->obj_name->text().toStdString());

    nfw = Firewall::cast(no);

    no->setStr("platform", platform);
    Resources::setDefaultTargetOptions(platform , nfw);
    no->setStr("host_OS", host_os);
    Resources::setDefaultTargetOptions(host_os , nfw);
}


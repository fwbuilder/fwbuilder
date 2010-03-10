/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

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


#include "IconSetter.h"
#include "FWBTree.h"
#include "utils.h"

void IconSetter::setObjectIcon(libfwbuilder::FWObject *obj, QPixmap *pm,
                               int icon_size)
{
    QString icn_alias;
    QString icn_sfx;

    switch (icon_size)
    {
    case 0: icn_sfx = "icon-tree"; break;
    case 2: icn_sfx = "icon-big"; break;
    default: icn_sfx = "icon"; break;
    }

    if (obj->getRO())
        icn_alias = ":/Icons/lock";
    else
    {
        if (FWBTree().isSystem(obj))
            icn_alias = ":/Icons/SystemGroup/" + icn_sfx;
        else
            icn_alias = QString(":/Icons/") + obj->getTypeName().c_str() + "/" + icn_sfx;
    }

    LoadPixmap(icn_alias, *pm);
}

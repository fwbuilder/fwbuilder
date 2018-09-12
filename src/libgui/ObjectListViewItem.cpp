/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  alek@codeminders.com

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


#include "ObjectListViewItem.h"

#include "fwbuilder/TCPUDPService.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"

#include <QTreeWidget>

using namespace libfwbuilder;

bool ObjectListViewItem::operator< ( const QTreeWidgetItem & other ) const
{
    QTreeWidget * widget = treeWidget() ;
    if (widget==nullptr) return false;

    int col = widget->sortColumn ();
    if (col==1)
    {
        FWObject *right = ((ObjectListViewItem*)(&other))->getFWObject();
        FWObject *left = this->getFWObject();
        TCPUDPService * rtcpudp = TCPUDPService::cast(right);
        TCPUDPService * ltcpudp = TCPUDPService::cast(left);

        IPService * rip = IPService::cast(right);
        IPService * lip = IPService::cast(left);

        ICMPService * ricmp = ICMPService::cast(right);
        ICMPService * licmp = ICMPService::cast(left);
        if (rtcpudp != nullptr && ltcpudp != nullptr)
        {
            int ls = ltcpudp->getDstRangeStart();
            int rs = rtcpudp->getDstRangeStart();
            if (ls<rs) 
                return true ;
            else
            {
                if (ls==rs)
                {
                    int le = ltcpudp->getDstRangeEnd();
                    int re = rtcpudp->getDstRangeEnd();
                    if (le<re) 
                        return true ;
                    else
                        return false ;
                }
                return false ;
            }
        }
        if (rip != nullptr && lip != nullptr)
        {
            int lpn = lip->getProtocolNumber();
            int rpn = rip->getProtocolNumber();
            return (lpn < rpn);
        }
        if (ricmp != nullptr && licmp != nullptr)
        {
            int lpn = licmp->getInt("code");
            int rpn = ricmp->getInt("code");
            return (lpn < rpn); 
        }
                    
    }
    return QTreeWidgetItem::operator < (other);
}

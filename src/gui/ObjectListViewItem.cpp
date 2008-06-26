#include "ObjectListViewItem.h"

#include "fwbuilder/TCPUDPService.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include <QTreeWidget>
using namespace libfwbuilder;

bool ObjectListViewItem::operator< ( const QTreeWidgetItem & other ) const
{
    QTreeWidget * widget = treeWidget () ;
    if (widget!=NULL)
    {
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
            if (rtcpudp != NULL && ltcpudp != NULL)
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
            if (rip != NULL && lip != NULL)
            {
                int lpn = lip->getProtocolNumber();
                int rpn = rip->getProtocolNumber();
                return (lpn < rpn);
            }
            if (ricmp != NULL & licmp != NULL)
            {
                int lpn = licmp->getInt("code");
                int rpn = ricmp->getInt("code");
                return (lpn < rpn); 
            }
                    
        }
        else
        {
            return QTreeWidgetItem::operator < (other);
        }
    }
}
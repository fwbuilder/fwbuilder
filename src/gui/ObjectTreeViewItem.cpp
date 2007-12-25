
#include "global.h"

#include "ObjectTreeViewItem.h"

#include "fwbuilder/Firewall.h"

#include <qfont.h>
#include <qpainter.h>

using namespace std;
using namespace libfwbuilder;

QVariant ObjectTreeViewItem::data (int column, int role) const
{
    if (role == Qt::FontRole)
    {
        QFont usual = QTreeWidgetItem::data(column, role).value<QFont>();
        
        FWObject * obj=getFWObject();
        Firewall * o=NULL;
    
        if (obj!=NULL || getProperty("type")==Firewall::TYPENAME) 
        {
            o=Firewall::cast( obj );
        }
    
        if (o!=NULL)
        {
            bool mf= !o->getInactive() && (o->needsInstall()) ; 
            usual.setBold (mf);
            return QVariant(usual);
        }
        else
            return QVariant(usual);
    }
        return QTreeWidgetItem::data(column, role);
}


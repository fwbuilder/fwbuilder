
#include "global.h"

#include "InstallFirewallViewItem.h"

#include "fwbuilder/Firewall.h"

#include <qfont.h>

using namespace std;
using namespace libfwbuilder;

InstallFirewallViewItem::InstallFirewallViewItem(QTreeWidget* parent,
    const QString & st, bool slt ): 
    QTreeWidgetItem(parent,QStringList(st)) 
{
    showLastTimes=slt;
}
    
QVariant InstallFirewallViewItem::data (int column, int role) const
{
    /*int statCol=(showLastTimes)?4:1;

    if (role == Qt::FontRole)
    {
        QFont usual = QTreeWidgetItem::data(column, role).value<QFont>();

        if (text(statCol).contains("..."))
        {
            usual.setBold (true);
            return QVariant(usual);
        } else if (text(statCol)==QObject::tr("Failure"))
            return QVariant(usual);
        
        if (column==statCol)
        {
            usual.setBold (true);
            return QVariant(usual);
        }
    }

    if (role == Qt::ForegroundRole)
    {
        QBrush usual = QTreeWidgetItem::data(column, role).value<QBrush>();

        if (text(statCol)==QObject::tr("Failure"))
        {
            usual.setColor(Qt::red);
            return QVariant(usual);
        }
        
        if ((column==statCol) && (text(statCol)==QObject::tr("Success")))
        {
            usual.setColor(Qt::green);
            return QVariant(usual);
        }
    }*/

    return QTreeWidgetItem::data(column, role);
}






#include "global.h"

#include "ColorCheckViewItem.h"

#include "fwbuilder/Firewall.h"

#include <qfont.h>
#include <qpainter.h>

using namespace std;
using namespace libfwbuilder;

void ColorCheckViewItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
    if (!p) return;
    QColorGroup c=cg;

    if (checkState() == Qt::Checked)
    {
        c.setBrush(QColorGroup::Base,QColor(240,255,240));
    }
    //QListWidgetItem::paintCell(p,c,column,width,align); TAPIR: this one was commented when no such method had been found for the QListWidgetItem class
}


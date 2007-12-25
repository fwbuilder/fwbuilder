/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: RuleSetView.cpp,v 1.163 2007/05/30 04:24:55 vkurland Exp $

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




#include "config.h"
#include "global.h"
#include "utils.h"
#include "FWWindow.h"
#include "RuleSetView.h"
#include "ObjectManipulator.h"
//#include "ObjectEditor.h" CWP
#include "platforms.h"
#include "inplaceComboBox.h"
#include "FWObjectDrag.h"
#include "FWObjectClipboard.h"
#include "findDialog.h"
#include "ColorLabelMenuItem.h"
#include "FWBSettings.h"
#include "SimpleTextEditor.h"
#include "SimpleIntEditor.h"
#include "ActionsDialog.h"
#include "FWObjectPropertiesFactory.h"
#include "ObjectTreeView.h"
#include "FindObjectWidget.h"

//#include "askrulenumberdialog_q.h" CWP

#include <iostream>
#include <sstream>

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"

#include "fwbuilder/InterfacePolicy.h"

#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Interface.h"

#include <qapplication.h>
#include <qpainter.h>
#include <qrect.h>
#include <qtextedit.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qdrag.h>
#include <qspinbox.h>
#include <qaction.h>
#include <qtooltip.h>
#include <qcursor.h>
#include <qstyle.h>
#include <qtimer.h>
#include <qpixmapcache.h>

using namespace libfwbuilder;
using namespace std;


class mouseEventFilter : public QObject
{
    protected:
    bool eventFilter( QObject *object, QEvent *event)
    {
        if (event->type() == QEvent::MouseButtonPress )
        {
            cerr << "event type=" << event->type() << endl;
            return true;
        }
        else 
            return false;
    }
};

mouseEventFilter mef;

bool headerMouseEventInterceptor::eventFilter( QObject *object, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu )
    {
        QContextMenuEvent *e = (QContextMenuEvent*)(event);
        
        int row = rsv->rowAt( rsv->contentsY()+e->pos().y() );
        rsv->contextMenu(row, -1, e->globalPos());

//        rsv->headerMousePressEvent(e->globalPos());
    }
    return false;
}

/**  CWP skipped from here
RuleObjToolTip::RuleObjToolTip(RuleSetView *w) : QToolTip(w->viewport(),0)
{
    rsv=w;
}

void RuleObjToolTip::maybeTip(const QPoint &pos)
{
    if (st->getObjTooltips())
    {
        int cx,cy;

        rsv->viewportToContents(pos.x(),pos.y(),cx,cy);

        int row = rsv->rowAt(cy);
        int col = rsv->columnAt(cx);

        QRect   cr;
        QString t="";
        
        cr=rsv->cellGeometry(row,col);
        
        if ( RuleSetView::Options == rsv->getColType(col) )
        {
            Rule *rule = rsv->getRule(row);
            if (PolicyRule::cast(rule)!=NULL )
            {
                if (! isDefaultPolicyRuleOptions( rule->getOptionsObject() ))
                    t= FWObjectPropertiesFactory::getPolicyRuleOptions(rule);
            }
            if (NATRule::cast(rule)!=NULL )
            {
                if (! isDefaultNATRuleOptions( rule->getOptionsObject() ))
                    t= FWObjectPropertiesFactory::getNATRuleOptions(rule);
            }
        }
        else if( RuleSetView::Direction == rsv->getColType(col) )
        {
            PolicyRule *rule = PolicyRule::cast( rsv->getRule(row) );
            if (rule!=NULL)
                t = rule->getDirectionAsString().c_str();
        }
        else if( RuleSetView::Action == rsv->getColType(col) )
        {
            PolicyRule *rule = PolicyRule::cast( rsv->getRule(row) );
            if (rule!=NULL)
                t= FWObjectPropertiesFactory::getRuleActionPropertiesRich(rule);
        }
        else
        {
            FWObject *obj = rsv->getObj(row,col,cy,&cr);
            if (obj==NULL)  
                return;
            t=FWObjectPropertiesFactory::getObjectPropertiesDetailed(obj,true,true);
        }
        cr.moveTopLeft( rsv->contentsToViewport( cr.topLeft() ) );
        tip(cr,t);
    }
} to here */


RuleSetView::RuleSetView( int r, int c, QWidget *parent ) : QTable( r, c, parent ), hme(this)
{
    kbdGoingUp = false;
    RuleElementSpacing=4;
    changingSelection = false;
    dragging = false;
    changingRules = false;

//    verticalHeader()->setLabel(0, "0");
//    horizontalHeader()->setLabel(0, "0");

    new RuleObjToolTip(this);

    setDragEnabled(true);

    ncols=c;
    selectedObject = NULL;

    setFocusPolicy( StrongFocus );

    items.setAutoDelete( TRUE );
    widgets.setAutoDelete( TRUE );

    setSelectionMode( MultiRow );

    setAcceptDrops( TRUE );

    setCaption( tr( "A Rule Set" ) );
    setLeftMargin( fontMetrics().width( "W999W" ) );

    horizontalHeader()->setStretchEnabled(false);
    verticalHeader()->setStretchEnabled(false);

    horizontalHeader()->setResizeEnabled(true);
    verticalHeader()->setResizeEnabled(false);

    horizontalHeader()->setClickEnabled(false);
    verticalHeader()->setClickEnabled(true);

    horizontalHeader()->setMovingEnabled(true);
/*
    connect( horizontalHeader(), SIGNAL( clicked(int) ),
             this, SLOT( horizontalHeaderClicked(int) ) );

    connect( verticalHeader(), SIGNAL( clicked(int) ),
             this, SLOT( verticalHeaderClicked(int) ) );
*/

    connect( this, SIGNAL( contextMenuRequested(int,int,const QPoint&) ),
             this, SLOT( contextMenu(int,int,const QPoint&) ) );

    connect( this, SIGNAL( doubleClicked(int,int,int,const QPoint&) ),
             this, SLOT( doubleClicked(int,int,int,const QPoint&) ) );

    connect( this, SIGNAL( selectionChanged() ),
             this, SLOT( selectionChanged() ) );

    connect( this, SIGNAL( contentsMoving(int,int) ),
             this, SLOT( contentsMoving(int,int) ) );

    verticalHeader()->installEventFilter( &hme );
    unselect();
}

RuleSetView::~RuleSetView()
{
}

Firewall* RuleSetView::getFirewall()
{
    FWObject *f=ruleset;
    while (f!=NULL && !Firewall::isA(f)) f=f->getParent();
    assert(f!=NULL);
    return Firewall::cast(f);
}

void RuleSetView::contentsMoving(int x, int y)
{
    if (fwbdebug) qDebug("RuleSetView::contentsMoving  x=%d y=%d",x,y);
}

void RuleSetView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{

//    if (fwbdebug)
//        qDebug("RuleSetView::drawContents  cx=%d cy=%d cw=%d ch=%d",cx,cy,cw,ch);

    QTable::drawContents(p,cx,cy,cw,ch);
}

void RuleSetView::hideEvent(QHideEvent *ev)
{
    QString k = settingsKey();
    QString v;

    for (int col=0; col<ncols; col++)  
        v = v + QString("%1 ").arg( columnWidth(col) );
/* disabled because we need to remember the width for columns in
 * association with their names, not just sequentially. Since
 * different firewall types have different number of columns, doing it
 * just by numbers causes the last column to disappear when user
 * switches from a firewall with 6 columns to firewall with 7 columns 
 * (e.g. from pf to iptables)
 */
//    st->setStr(k,v);
}


QString RuleSetView::settingsKey()
{
    return QString("/RuleSets/") + name() + "_Columns";
}

void RuleSetView::setRuleNumber(int row, libfwbuilder::Rule *rule)
{
    QIconSet icn;

    if (rule!=NULL && rule->isDisabled())
    {
//        icn = QPixmap::fromMimeSource( Resources::global_res->getResourceStr(
//                               "/FWBuilderResources/UI/Icons/neg").c_str());

        QString icn_file = Resources::global_res->getResourceStr(
            "/FWBuilderResources/UI/Icons/neg").c_str();
        QPixmap pm;
        if ( ! QPixmapCache::find( icn_file, pm) )
        {
            pm = QPixmap::fromMimeSource( icn_file );
            QPixmapCache::insert( icn_file, pm);
        }
        icn = QIconSet(pm);
    } else
        icn = QIconSet();

    int s = verticalHeader()->sectionSize(row);
    verticalHeader()->setLabel( row, icn, QString::number(row) );
    verticalHeader()->resizeSection( row , s );
}

void RuleSetView::fixRulePosition(Rule *rule, FWObject *parent, int pos)
{
    if ( rule->isReadOnly())
    {
        // need to temporary break the lock
        // since several parents could be read-only, do it recursively
        FWObject *o = (parent!=NULL) ? parent : rule;
        while ( o!=NULL && !o->getBool("ro") ) o = o->getParent();
        if (o)
        {
            o->setReadOnly(false);
            fixRulePosition(rule, o, pos);
            o->setReadOnly(true);
        } else
        {
            rule->checkReadOnly();   // should be read-write by now
            rule->setPosition(pos);
        }
    } else
        rule->setPosition(pos);
}


void RuleSetView::init()
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

    horizontalHeader()->adjustHeaderSize();

    int          row=0;
    map<int,int> colW;
    bool         userColWidth=false;

    QPainter p(this);

    QString k = settingsKey();
    QString v = st->getStr(k);
    if (!v.isEmpty())
    {
        userColWidth=true;
        for (int col=0; col<ncols; col++)  
            colW[col]=v.section(' ',col,col).toInt();
    } else
    {
        for (int col=0; col<ncols; col++)
        {
            QString lbl = horizontalHeader()->label(col);
            QRect br=p.boundingRect(0, 0, 1000, 1000,
                                    Qt::AlignLeft|Qt::AlignVCenter,
                                    lbl );
            colW[col]=br.width() + 10;
        }
    }

    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++,row++)
    {
        ruleIndex[row] = *i;
        dirtyRows[row] = 1;

        setRuleNumber(row, Rule::cast( *i ));
        if (Rule::cast( *i )->getPosition()!=row)
        {
            fixRulePosition(Rule::cast( *i ), NULL, row);
        }
//        adjustRow(row);

        int h=20;
        for (int col=0; col<ncols; col++)
        {
            QRect cr = calculateCellSize(row,col);
            h = QMAX(h, cr.height());
            if (!userColWidth)
                colW[col]=QMAX(colW[col],cr.width());
        }
        adjustRow_int(row,h);
    }

    for (int col=0; col<ncols; col++) setColumnWidth(col,colW[col]);

    updateContents();

    QApplication::restoreOverrideCursor();
}

void RuleSetView::iinit()
{
    setNumRows( ruleset->size() );

    QString icn = Resources::global_res->getResourceStr("/FWBuilderResources/UI/Icons/Accept").c_str();

    if (fwbdebug)
        qDebug("RuleSetView::iinit()   icn=%s",icn.ascii());

    QPixmap pm;
    if ( ! QPixmapCache::find( icn, pm) )
    {
        pm = QPixmap::fromMimeSource( icn );
        QPixmapCache::insert( icn, pm);
    }

    pixmap_h    = pm.height();
    pixmap_w    = pm.width();

    QPainter p(this);
    QRect br = p.boundingRect(0, 0, 1000, 1000,
                              Qt::AlignLeft|Qt::AlignVCenter,"WMWM" );
    text_h   = br.height();
    item_h   = ( (pixmap_h>text_h)?pixmap_h:text_h ) + RuleElementSpacing;

    FWObject *f = getFirewall();

// f is a pointer at firewall object
    supports_logging      =false;
    supports_rule_options =false;
    supports_time         =false;

    try {
        supports_logging=
            Resources::getTargetCapabilityBool(f->getStr("platform"),
                                               "logging_in_policy");
        supports_rule_options=
            Resources::getTargetCapabilityBool(f->getStr("platform"),
                                               "options_in_policy");
        supports_time=
            Resources::getTargetCapabilityBool(f->getStr("platform"),
                                               "supports_time");
    } catch (FWException &ex)    {    }

    return;
}

void RuleSetView::clear()
{


}


/* CWP skipped from here
void RuleSetView::focusInEvent(QFocusEvent* ev)
{
    if (fwbdebug) qDebug("RuleSetView::focusInEvent");

    //om->unselect();
    QTable::focusInEvent(ev);
    repaintSelections();

    if (
        (ev->reason()==QFocusEvent::Tab ||
         ev->reason()==QFocusEvent::Backtab) &&
        oe->isVisible()
    )
        mw->requestEditorOwnership(this,NULL,ObjectEditor::optNone,true);
}

void RuleSetView::focusOutEvent(QFocusEvent* ev)
{
    if (fwbdebug)
    {
        qDebug("RuleSetView::focusOutEvent.  QStyle::SH_ItemView_ChangeHighlightOnFocus=%d backgroundMode=%d",
               style().styleHint( QStyle::SH_ItemView_ChangeHighlightOnFocus, this ),
               backgroundMode());

    }

    QTable::focusOutEvent(ev);
    repaintSelections();
}


and to here*/

void RuleSetView::adjustColumn( int col )
{
//    int w = horizontalHeader()->sectionSize(col);
    QString lbl = horizontalHeader()->label(col);
    QPainter p(this);
    QRect br=p.boundingRect(0, 0, 1000, 1000,
                            Qt::AlignLeft|Qt::AlignVCenter,
                            lbl );

    int     w = br.width() + 10;
    
    int row=0;
    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++,row++)
    {
        QRect cr = calculateCellSize(row,col);
        w=QMAX(w,cr.width());
    }
    setColumnWidth(col,w);
}

void RuleSetView::adjustRow_int( int row, int h )
{

/ * make sure the row is no smaller than a label in the left header,
 * and no smaller than the "strut" (the minimal size of the gui
         * element as defined in QApplication)
 * /
         QHeader * leftHeader = verticalHeader();
 h = QMAX(h, leftHeader->fontMetrics().height() + 2);
 h = QMAX(h, QApplication::globalStrut().height());

/ * setRowHeight causes redraw. Beware of loops 'cause we call adjustRow from
 * cellPaint! * / 
        setRowHeight(row, h);

    dirtyRows[row]=0;
}

void RuleSetView::adjustRow( int row )
{
    int h = 20;

    for (int col=0; col<ncols; col++)
    {
        QRect cr = calculateCellSize(row,col);

        h = QMAX(h, cr.height());
        int w = cr.width();

        if (columnWidth(col)<w) setColumnWidth(col,w);
    }

    adjustRow_int(row,h);
}


QRect RuleSetView::calculateCellSize( int row, int col )
{
    int h = 20;
    int re_size;

//    if (fwbdebug)
//        qDebug("RuleSetView::calculateCellSize: row=%d col=%d",
//               row,col);

    QPainter p(this);

    Rule *rule = Rule::cast( ruleIndex[row] );

    int hc=0;
    int wc=0;
    switch (getColType(col))
    {
    case Time:
    {
        RuleElement *re = getRE(rule,col);
        if (re==NULL)
        {
/* broken rule element, fix it */
            FWObject *nre=mw->db()->create("When");
            assert(nre!=NULL);
            rule->add(nre);
        }
    }
/* continue in Object */

    case Object:
    {
        RuleElement *re = getRE(rule,col);
        if (re==NULL) return QRect(0,0,0,0);
        re_size = re->size();
        for (FWObject::iterator j=re->begin(); j!=re->end(); j++)
        {
            FWObject *o1= *j;
            FWObject *o2 = o1;
            string o1ref = "";
            if (FWReference::cast(o1)!=NULL)
            {
                o1ref = FWReference::cast(o1)->getPointerId();
                o2=FWReference::cast(o1)->getPointer();
            }
            QString ot = objectText(re,o2);
            QRect br=p.boundingRect(0, 0, 1000, 1000,
                                    Qt::AlignLeft|Qt::AlignVCenter,
                                    ot);
            hc += item_h;
            int itmW = RuleElementSpacing/2 + pixmap_w + 
                RuleElementSpacing + br.width();
            wc  = QMAX(wc, itmW);
        }
        break;
    }

    case Action:
    {
/* possible actions:
   "Accept", "Deny", "Reject", "Accounting", "Tag",
   "Pipe", "Classify", "Custom", "Continue"
*/
        QString ac =
            FWObjectPropertiesFactory::getRuleActionProperties(
                PolicyRule::cast(rule));
        QRect br=p.boundingRect(0, 0, 1000, 1000,
                                Qt::AlignLeft|Qt::AlignVCenter,ac );
        hc = item_h;
        wc = RuleElementSpacing/2 + pixmap_w + RuleElementSpacing + br.width();
        break;
    }

    case Direction:
    {
        hc = item_h;
        wc = RuleElementSpacing/2 + pixmap_w + RuleElementSpacing;
        break;
#if 0
/* possible directions: "Inbound", "Outbound" , "Both" */
        QRect br=p.boundingRect(0, 0, 1000, 1000,
                                Qt::AlignLeft|Qt::AlignVCenter,tr("Outbound ") );
        hc = item_h;
        wc = RuleElementSpacing/2 + pixmap_w + RuleElementSpacing + br.width();
        break;
#endif
    }

    case Options:
        hc = item_h;
        wc = RuleElementSpacing/2 + pixmap_w + RuleElementSpacing + pixmap_w;
        break;

    case Comment:
    {
        QRect br=p.boundingRect(0, 0, 1000, 1000,
                                Qt::AlignLeft|Qt::AlignVCenter,
                                QString::fromUtf8(rule->getComment().c_str()) );
        hc = br.height() + RuleElementSpacing;
        wc = RuleElementSpacing/2 + br.width();
        break;
    }

    case Metric:
    {
        QRect br=p.boundingRect(0, 0, 1000, 1000,
                                Qt::AlignLeft|Qt::AlignVCenter,
                                QString::fromUtf8(RoutingRule::cast(rule)->getMetricAsString().c_str()) );
        hc = br.height() + RuleElementSpacing;
        wc = RuleElementSpacing/2 + br.width();
        break;
    }

    default:
        break;
    }

    h = QMAX(h, hc);

    wc = QMAX(wc, QApplication::globalStrut().width());
    wc += RuleElementSpacing/2;  // some padding

    return QRect(0,0,wc,h);
}

QPixmap RuleSetView::getPixmap(FWObject *obj, PixmapAttr pmattr) const
{
//    QPixmap pm;
    string icn = "icon";
    if (pmattr == Neg)  icn="icon-neg";
    if (pmattr == Ref)  icn="icon-ref";
    if (pmattr == Tree) icn="icon-tree";

//    return QPixmap::fromMimeSource( 
//        Resources::global_res->getObjResourceStr(obj, icn).c_str() );

    QString icn_file = Resources::global_res->getObjResourceStr(obj, icn).c_str();
    QPixmap pm;
    if ( ! QPixmapCache::find( icn_file, pm) )
    {
        pm = QPixmap::fromMimeSource( icn_file );
        QPixmapCache::insert( icn_file, pm);
    }

    return pm;
}

/*
 *  insertWidget and cellwidget are only used when widgets are put in
 *  cells, which only happens if we enable cell editing. Which we do not.
 */
void RuleSetView::insertWidget( int r, int c, QWidget *w )
{
    widgets.replace( indexOf( r, c ), w );
}

QWidget* RuleSetView::cellWidget( int r, int c ) const
{
    return widgets.find( indexOf( r, c ) );
}

/* CWP DONE
QString RuleSetView::objectText(RuleElement *re,FWObject *obj)
{
    if (re->isAny())
    {
        if (RuleElementTSrc::isA(re) ||
            RuleElementTDst::isA(re) ||
            RuleElementTSrv::isA(re)) return QString(tr("Original"));
        if (RuleElementRDst::isA(re)) return QString(tr("Default"));
        if (RuleElementRGtw::isA(re) ||
            RuleElementRItf::isA(re))  return QString("");
        if (RuleElementItf::isA(re))   return QString(tr("All"));
        return QString(tr("Any"));
    }

    if (Interface::isA(obj))
    {
        QString lbl= Interface::cast(obj)->getLabel().c_str();
        if ( !lbl.isEmpty() ) return lbl;
    }
    if (obj->getName() == "Any") return QString(tr("Any"));
    else return QString::fromUtf8(obj->getName().c_str());
}*/

void RuleSetView::paintCell(QPainter *pntr,
                            int row,
                            int col,
                            const QRect &cr,
                            bool selected,
                            const QColorGroup &cg)
{
    int re_size;

/* row may point at an empty row where there is no rule yet. This
 * happens if this method is called to redraw the table when we call
 * setNumRows
 */

//    if (fwbdebug)
//        qDebug("Draw cell: row=%d col=%d  current palette=%d",
//               row,col,palette().serialNumber());

    if (ruleIndex.count(row)==0) return;

    if (dirtyRows[row]!=0) 
    { 
//        if (fwbdebug) qDebug("RuleSetView::paintCell dirty row %d",row);

        dirtyRows[row]=0;
        adjustRow(row); //    this causes repaint 
        return;
    }

    QString     rclr;
    Rule *rule = Rule::cast( ruleIndex[row] );
    if (rule==NULL) return;
    
    FWOptions  *ropt = rule->getOptionsObject();
    assert(ropt!=NULL);
    rclr = ropt->getStr("color").c_str();

    QPixmap bufferpixmap;
    QString bpmname = QString("rulesetcell_%1_%2").arg(cr.width()).arg(cr.height());
    if ( ! QPixmapCache::find( bpmname, bufferpixmap) )
    {
        bufferpixmap = QPixmap( cr.width() , cr.height() );
        QPixmapCache::insert( bpmname, bufferpixmap);
    }

    //bufferpixmap.resize( cr.width() , cr.height() );
    bufferpixmap.fill( cg.base() );

    QPainter p( &bufferpixmap );

    QRect r = cellRect(row,col);

    int x  = r.left() + RuleElementSpacing/2;
    int y  = r.top();

    if (!rclr.isEmpty())
    {
        QRect rect(0, y, cr.width(), cr.height() );
        p.fillRect(rect, QColor(rclr));
    }

    p.drawLine( cr.width()-1, 0, cr.width()-1, cr.height()-1 );
    p.drawLine( 0, cr.height()-1, cr.width()-1, cr.height()-1 );

    p.drawLine( cr.width(), 1, cr.width(), cr.height() );
    p.drawLine( 1, cr.height(), cr.width(), cr.height() );

//    if (selected)   penColor=cg.highlightedText();
//    else            penColor=cg.text();

    const BackgroundMode bgmode = backgroundMode();
    const QColorGroup::ColorRole crole = QPalette::backgroundRoleFromMode( bgmode );

    bool  sel = (row==currentRow() && col==currentColumn());

    if (getColType(col)==Object || getColType(col)==Time)
    {
        RuleElement *re = getRE(row,col);
        if (re==NULL) return;
        re_size = re->size();
        for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
        {
            FWObject *o1= *i;
            if (FWReference::cast(o1)!=NULL)
                o1=FWReference::cast(o1)->getPointer();
            if (sel && o1==selectedObject)
            {
                QRect rect(0, y, cr.width(), item_h );
                if (hasFocus())
                {
                    p.fillRect(rect, cg.brush( QColorGroup::Highlight ));
                    p.setPen( cg.highlightedText() );
                } else
                {
                    p.fillRect(rect, cg.brush( crole ));
                    p.setPen( cg.text() );
                }
            } else
            {
                p.setPen( cg.text() );
            }
            x = r.left()+1;

            //QPixmap pm = getPixmap(o1 , re->getNeg()?Neg:Normal );

            string icn = "icon";
            if (re->getNeg()) icn = "icon-neg";

            QString icn_file =
                Resources::global_res->getObjResourceStr(o1, icn).c_str();
            QPixmap pm;
            if ( ! QPixmapCache::find( icn_file, pm) )
            {
                pm = QPixmap::fromMimeSource( icn_file );
                QPixmapCache::insert( icn_file, pm);
            }

            p.drawPixmap( x, y + RuleElementSpacing/2, pm );

            x += pm.width()+1;

            p.drawText( x, y + RuleElementSpacing/2,
                         cr.width()-pm.width()-1, item_h,
                         Qt::AlignLeft|Qt::AlignVCenter, objectText(re,o1) );
            
            FWObject *mwSelObj = selectedObject;
            std::vector<libfwbuilder::FWObject*> om_selected_objects =
                om->getCurrentObjectTree()->getSelectedObjects();

            if (mwSelObj==NULL && om_selected_objects.size()!=0)
                mwSelObj = om_selected_objects.front();

            if (    (!sel || numSelections()==0) &&
                    mwSelObj!= NULL &&
                    mwSelObj->getId() != "sysid0" &&
                    mwSelObj->getId() != "sysid1" &&
                    mwSelObj->getId() != "sysid2" &&
                    mwSelObj == o1)
            {
                p.setPen(Qt::red);
                p.drawLine( 1, y+1, cr.width()-3, y+1 );
                p.drawLine( cr.width()-3, y+1, cr.width()-3, y+item_h-3 );

                p.drawLine( 1, y+item_h-3, cr.width()-3, y+item_h-3 );
                p.drawLine( 1, y+1, 1, y+item_h-3 );

            } 

            y += item_h;
        }
    } else
    {
        if (sel)
        {
            QRect rect(0, 0, cr.width(), cr.height() );
            if (hasFocus())
            {
                p.fillRect(rect, cg.brush( QColorGroup::Highlight ));
                p.setPen( cg.highlightedText() );
            } else
            {
                p.fillRect(rect, cg.brush( crole ));
                p.setPen( cg.text() );
            }
        } else
        {
            p.setPen( cg.text() );
        }

        switch (getColType(col))
        {
        case Action:
        {
            PolicyRule *rule = PolicyRule::cast( ruleIndex[row] );
            if (rule==NULL) return;

            QString platform=getPlatform();
            string act = rule->getActionAsString();
            QString icn = Resources::global_res->getResourceStr("/FWBuilderResources/UI/Icons/"+ act ).c_str();
            QString res="";
            //FWOptions *ropt = rule->getOptionsObject();
            res = FWObjectPropertiesFactory::getRuleActionProperties(rule);

            assert(icn!="");
            QPixmap pm;
            if ( ! QPixmapCache::find( icn, pm) )
            {
                pm = QPixmap::fromMimeSource( icn );
                QPixmapCache::insert( icn, pm);
            }

            p.drawPixmap( x,y + RuleElementSpacing/2, pm );
            x += pm.width()+1;
            QRect br=p.boundingRect(x, y, 1000, 1000,
                                    Qt::AlignLeft|Qt::AlignVCenter,
                                    res.ascii() );
            p.drawText( x, y, br.width(), pm.height(),
                        Qt::AlignLeft|Qt::AlignVCenter, res.ascii() );
            break;
        }
        case Direction:
        {
            PolicyRule *rule = PolicyRule::cast( ruleIndex[row] );
            if (rule==NULL) return;

            string dir = rule->getDirectionAsString();
            if (dir.empty()) dir = "Both";
            QString icn = Resources::global_res->getResourceStr(
                "/FWBuilderResources/UI/Icons/"+ dir ).c_str();
            assert(icn!="");
            QPixmap pm;
            if ( ! QPixmapCache::find( icn, pm) )
            {
                pm = QPixmap::fromMimeSource( icn );
                QPixmapCache::insert( icn, pm);
            }

            p.drawPixmap( x,y + RuleElementSpacing/2, pm );
            x += pm.width()+1;
//        QRect br=p.boundingRect(x, y, 1000, 1000,
//                                Qt::AlignLeft|Qt::AlignVCenter,dir.c_str() );
//        p.drawText( x, y, br.width(), pm.height(),
//                    Qt::AlignLeft|Qt::AlignVCenter, dir.c_str() );
            break;
        }
        case Options:
        {
            /* both policy and routing rules have options. so cast to Rule here. */
            Rule *rule = Rule::cast( ruleIndex[row] );
            if (rule==NULL) return;

            /* is this a policy rule? only policy rules have the logging option. */
            PolicyRule  *policyRule  = PolicyRule::cast( rule );
            NATRule     *natRule     = NATRule::cast( rule );
            RoutingRule *routingRule = RoutingRule::cast( rule );

            if (policyRule && policyRule->getLogging())
            {
                QString icn = Resources::global_res->getResourceStr("/FWBuilderResources/UI/Icons/Log" ).c_str();
                assert(icn!="");
//                QPixmap pm = QPixmap::fromMimeSource( icn.c_str() );
                QPixmap pm;
                if ( ! QPixmapCache::find( icn, pm) )
                {
                    pm = QPixmap::fromMimeSource( icn );
                    QPixmapCache::insert( icn, pm);
                }

                p.drawPixmap( x,y + RuleElementSpacing/2, pm );
                x += pm.width()+2;
            }
            if (
                (policyRule  && ! isDefaultPolicyRuleOptions( rule->getOptionsObject())) ||
                (routingRule && ! isDefaultRoutingRuleOptions( rule->getOptionsObject())) ||
                (natRule     && ! isDefaultNATRuleOptions( rule->getOptionsObject()))
            )
            {
                QString icn = Resources::global_res->getResourceStr("/FWBuilderResources/UI/Icons/Options" ).c_str();
                assert(icn!="");
//                QPixmap pm = QPixmap::fromMimeSource( icn.c_str() );
                QPixmap pm;
                if ( ! QPixmapCache::find( icn, pm) )
                {
                    pm = QPixmap::fromMimeSource( icn );
                    QPixmapCache::insert( icn, pm);
                }

                p.drawPixmap( x,y + RuleElementSpacing/2, pm );
            }
            break;
        }
        case Comment:
        {
/* comments are found in both policy and nat rules, so we cast to Rule here */
            Rule *rule = Rule::cast( ruleIndex[row] );
            if (rule==NULL) return;

            QRect br=p.boundingRect(x, y, 1000, 1000,
                                    Qt::AlignLeft|Qt::AlignVCenter,
                                    QString::fromUtf8(rule->getComment().c_str()) );
            p.drawText( x, y + RuleElementSpacing/2,
                        br.width(),
                        br.height(),
                        Qt::AlignLeft|Qt::AlignVCenter,
                        QString::fromUtf8(rule->getComment().c_str()) );

            break;
        }
        case Metric:
        {
            RoutingRule *rule = RoutingRule::cast( ruleIndex[row] );
            if (rule==NULL) return;

            p.drawText( x, y, cr.width()-2, RuleElementSpacing*2+pixmap_h,
                        Qt::AlignHCenter|Qt::AlignVCenter,
                        QString::fromUtf8(rule->getMetricAsString().c_str()) );

            break;
        }
        default:
            break;
        }  // switch
    }

    p.end();

    pntr->drawPixmap( 0, 0, bufferpixmap );
//    bitBlt(pntr->device(), 0, 0, &bufferpixmap, 0, 0, bufferpixmap.width(), bufferpixmap.height());
    return;
}

QString RuleSetView::getPlatform()
{
    return  getFirewall()->getStr("platform").c_str();
}

    
libfwbuilder::PolicyRule* RuleSetView::getRule(int row)
{
    return PolicyRule::cast( ruleIndex[row] );
}
/* CWP DONE
RuleSetView::REType  RuleSetView::getColType(int col) const
{
    map<int,REType>::const_iterator i = colTypes.find(col);
    return i->second;
}*/

/***********************************************************
 *    I do not use in-place editing anymore,
 */
QWidget *RuleSetView::createEditor( int row, int col, bool initFromCell ) const
{
    return NULL;
}

void RuleSetView::selectRE( int row, int col)
{
    mw->selectRules();

    if (row!=currentRow() || col!=currentColumn())
    {
        selectedObject = NULL;
        updateCell(currentRow(),currentColumn());
    }
}

void RuleSetView::selectRE(libfwbuilder::FWReference *ref)
{
    mw->selectRules();

//    selectedObject = ref->getPointer();
    setSelectedObject( ref->getPointer() );

/* need to find row and column this object is in and show it */
    FWObject *re = ref->getParent();
    Rule     *r  = Rule::cast(re->getParent());
    assert(r!=NULL);

    int row      = r->getPosition();
    int col;
    for (col=0; col<ncols; ++col)
        if (re==getRE(r,col))
        {
            setCurrentCell(row,col);
            ensureCellVisible(row,col);
            updateCell(row,col);
            break;
        }
}

void RuleSetView::unselect()
{
//    selectedObject = NULL;
//    setCurrentCell(-1,-1);
    clearSelection();
    selectedObject=NULL;
    updateCell(currentRow(),currentColumn());
    updateContents();
}

bool RuleSetView::isSelected()
{
    return (selectedObject!=NULL);
}

void RuleSetView::horizontalHeaderClicked(int col)
{
}

void RuleSetView::verticalHeaderClicked(int row)
{
}

void RuleSetView::doubleClicked(int row,int col,int btn,const QPoint &pos)
{
    if (row<0) return;
    editSelected();
}

/*
 *  pos is in coordinates relative to the header
 */
void RuleSetView::headerMousePressEvent(const QPoint &pos)
{
    int row = rowAt( pos.y() );
    contextMenu(row, -1, pos);
}

void RuleSetView::selectionChanged()
{

    if (fwbdebug)
    {
        qDebug("RuleSetView::selectionChanged()");
        qDebug("changingSelection   =%d",changingSelection);
        qDebug("numSelections       =%d",numSelections());
    }

    if (changingSelection || numSelections()==0) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

/*
 * we get multiple selections when user uses Ctrl+mouse click to
 * select several rows. Reset multiple selections so we end up with a
 * single one. In effect, Ctrl-click should behave like normal click,
 * that is, it should select a rule the user clicked on with or
 * without Ctrl.  Multiple rules can be selected with
 * Shift-click. Shift-click creates one large selection.
 */
    while (numSelections()>1)
    {
        for (int i=0; i<numSelections(); ++i)
        {
            QTableSelection sel=selection(i);
            if (sel.isActive())
            {
                if (fwbdebug)
                {
                    qDebug("sel %d: tRow=%d",i,sel.topRow());
                    qDebug("        bRow=%d",sel.bottomRow());
                }

                if (i==0)
                {
                    removeSelection(sel);
                    break;
                }
            }
        }
    }

    QTableSelection sel=selection(0);

    if (fwbdebug)
    {
        qDebug("Reset selection:");
        qDebug("      sel.isActive()=%d",sel.isActive());
        qDebug("              topRow=%d",sel.topRow());
        qDebug("           bottomRow=%d",sel.bottomRow());
        qDebug("             leftCol=%d",sel.leftCol());
        qDebug("            rightCol=%d",sel.rightCol());
    }

/*
 * this fixes bug #1030538 "incorrect highlighting when selecting
 * multiple rules"
 */
    updateHeaderStates();
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();

        int selectionSize=lastSelectedRule-firstSelectedRule+1;

        mw->editCopyAction->setEnabled(true);
        mw->editCutAction->setEnabled(true);
        mw->editPasteAction->setEnabled(true);
        mw->editDeleteAction->setEnabled(true);

        mw->copyRuleAction->setEnabled( selectionSize==1 );
        mw->cutRuleAction->setEnabled( selectionSize==1 );
        mw->pasteRuleAboveAction->setEnabled( selectionSize==1 );
        mw->pasteRuleBelowAction->setEnabled( selectionSize==1 );
        
    }

}
/*CWP DONE
FWObject* RuleSetView::getSelectedObject()
{
    return selectedObject;
}


void RuleSetView::openObjectInTree(FWObject *obj)
{
    if (gui_experiment1) return;
    
    FWObject *oo = obj;
    if (obj==NULL || Rule::cast(obj)!=NULL)
        oo = getFirewall();
    selectedObject=oo;
    mw->info(oo);

    setUpdatesEnabled(false);
    //bool f = hasFocus();
    om->setFocus();
    om->openObject(oo);
    om->clearFocus();
    setFocus();
    setUpdatesEnabled(true);
    updateContents();
}

void RuleSetView::setSelectedObject(FWObject* obj)
{
    if (fwbdebug)
        qDebug("RuleSetView::setSelectedObject  obj='%s' currentRow()=%d currentColumn()=%d",
               (obj)?obj->getName().c_str():"",
               currentRow(),
               currentColumn());

    prevSelectedObject = selectedObject;
    prevSelectedObjectRow = selectedObjectRow;
    prevSelectedObjectCol = selectedObjectCol;

    selectedObject = obj;
    selectedObjectRow = currentRow();
    selectedObjectCol = currentColumn();

    openObjectInTree(selectedObject);
}*/

void RuleSetView::restoreSelection(bool same_widget)
{
    if (fwbdebug)
    {
        if (prevSelectedObject)
            qDebug("RuleSetView::restoreSelection()  same_widget=%d prevSelectedObject=%s prevSelectedObjectRow=%d prevSelectedObjectCol=%d",
                   same_widget,
                   prevSelectedObject->getName().c_str(),
                   prevSelectedObjectRow,
                   prevSelectedObjectCol);
        else
            qDebug("RuleSetView::restoreSelection()  prevSelectedObject==NULL");
        FWObject *o = oe->getOpened();
        qDebug("RuleSetView::restoreSelection() oe->getOpened=%s",
               (o)?o->getName().c_str():"NULL");
    }

    if (same_widget)
    {
        selectedObject = prevSelectedObject;
        selectedObjectRow = prevSelectedObjectRow;
        selectedObjectCol = prevSelectedObjectCol;
    }
    setCurrentCell(selectedObjectRow,selectedObjectCol);
    updateCell(selectedObjectRow, selectedObjectCol);

    openObjectInTree(selectedObject);
}


/*
 * Entry point for all menu items 'edit' and doubleclick
 */
void RuleSetView::editSelected()
{
    if (!oe->isVisible()) oe->show();
    switchObjectInEditor(currentColumn());
}

bool RuleSetView::switchObjectInEditor(int col,bool validate)
{
    if (fwbdebug)
        qDebug("RuleSetView::switchObjectInEditor  col=%d  validate=%d",col,validate);

        
    if (!isTreeReadWrite(this,ruleset)) return false;
    if ( currentRow()==-1 || currentColumn()==-1 ) return false;
   
    FWObject *Object=NULL;
    ObjectEditor::OptType Operation=ObjectEditor::optNone;

    /*
     *  We need to know WHAT we are going to edit
    
     1. Object
     2. OptType
    
     * Object == null, OptType = optNone => blank
     * Object == Rule, OptType = optNone => Rule Options
     * Object == Rule, OptType != optNone => Virtual Object (Action, Comment ...)
     * Object != Rule, OptType = optNone => Regular Object Editor
    
     Then we compare our object 'obj' and OptType with what we already
     have in ObjectEditor/ If they are the same, then we do nothing,
     otherwise we open obj in the  Object Editor

    */

    int crn=currentRow();
    switch (getColType(col))
    {
    case Comment:
        Object=ruleIndex[crn];
        Operation=ObjectEditor::optComment;
        break;

    case Metric:
        Object=ruleIndex[crn];
        Operation=ObjectEditor::optMetric;
        break;

    case Direction:
        break;

    case Action:
    {
        PolicyRule *rule = PolicyRule::cast( ruleIndex[currentRow()] );
        Object=rule;
        Operation=ObjectEditor::optAction;
        break;
    }
    case Options:
    {
        /* both policy and routing rules have options. so cast to Rule here. */
        Rule *rule = Rule::cast( ruleIndex[currentRow()] );
        assert(rule);
        Object=rule;
        Operation=ObjectEditor::optNone;
        break;
    }

    default:
    {
        if ( selectedObject!=NULL)
        {
            Object=selectedObject;
            break;
        }
    }
    }

    if (!mw->requestEditorOwnership(this,Object,Operation,validate))
        return false;

    if (fwbdebug)
        qDebug("RuleSetView::switchObjectInEditor  editor ownership granted");

    if (Object==oe->getOpened() && Operation==oe->getOpenedOpt())
    {
        if (fwbdebug)
            qDebug("RuleSetView::switchObjectInEditor  same object is already opened in the editor");
        return true;
    }

    if (fwbdebug)
        qDebug("RuleSetView::switchObjectInEditor  opening object in the editor");
    
    if (Object == NULL)
    {
        oe->blank();
    } else if (Operation==ObjectEditor::optNone)
    {
        oe->open(Object);
    } else if(Rule::cast(Object)!=NULL)
    {
        oe->openOpt(Object,Operation);
    }

    return true;
}

/* CWP DONE
void RuleSetView::contextMenu(int row, int col, const QPoint &pos)
{
    QTableSelection sel=selection(0);

    if (fwbdebug)
{
        qDebug("RuleSetView::contextMenu()");
        qDebug("Selection:  isActive=%d",sel.isActive());
        qDebug("              topRow=%d",sel.topRow());
        qDebug("           bottomRow=%d",sel.bottomRow());
        qDebug("             leftCol=%d",sel.leftCol());
        qDebug("            rightCol=%d",sel.rightCol());
}

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    if (sel.isActive())
{
/ * if we have an active selection but user called context menu
 * outside selected rows, reset selection. Otherwise work with existing
 * selection which may include several rows.
 * /
        if (row<sel.topRow() || row>sel.bottomRow())
{
    clearSelection();
    sel= QTableSelection(row,0,row,ncols-1);
    addSelection( sel );
    setCurrentCell(row,0);
    updateHeaderStates();
}
} else
{
    clearSelection();
    sel= QTableSelection(row,0,row,ncols-1);
    addSelection( sel );
    setCurrentCell(row,0);
    updateHeaderStates();
}

    firstSelectedRule=sel.topRow();
    lastSelectedRule=sel.bottomRow();

    if (row<0 && ruleset->size()==0)
{
    QPopupMenu *popup=new QPopupMenu(this);
    addPopupMenuItem( this, popup, "", tr("Insert Rule"),
                      SLOT( insertRule() ) );
    addPopupMenuItem( this, popup, "", tr("Paste Rule"),
                      SLOT( pasteRuleAbove() ) );
    popup->exec( pos );
    delete popup;
    return;
}

    if (row<0 && ruleset->size()!=0)
{
    / * this is when user clicks under the last rule * /

    setCurrentCell(ruleset->size()-1,0);

    QPopupMenu *popup=new QPopupMenu(this);
    addPopupMenuItem( this, popup, "", tr("Paste Rule"),
                      SLOT( pasteRuleBelow() ) );
    popup->exec( pos );
    delete popup;
    return;
}

//    QPoint   rp = mapFromGlobal( pos);
//    QHeader *hh = horizontalHeader();
//    QHeader *vh = verticalHeader();    
//    int nx = rp.x()-vh->width()-1;
//    int ny = rp.y()-hh->height()-1;
//    objectClicked(row,col,0,QPoint(nx,ny));

    lastPopupMenuAction=None;

    QPopupMenu *popup=new QPopupMenu(this);

    switch (getColType(col))
{
    case Action:
    {
        Firewall *f = getFirewall();
        string platform=f->getStr("platform"); 
        QString action_name;
       
        if (Resources::isTargetActionSupported(platform,"Accept"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Accept,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Accept",
                              action_name,
                              SLOT( changeActionToAccept()     ) );
        }        
        if (Resources::isTargetActionSupported(platform,"Deny"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Deny,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Deny",
                              action_name,
                              SLOT( changeActionToDeny()       ) );
        }
        if (Resources::isTargetActionSupported(platform,"Reject"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Reject,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Reject",
                              action_name,
                              SLOT( changeActionToReject()     ) );
        }
        if (Resources::isTargetActionSupported(platform,"Accounting"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Accounting,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Accounting",
                              action_name,
                              SLOT( changeActionToAccounting() ) );
        }
        if (Resources::isTargetActionSupported(platform,"Pipe"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Pipe,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Pipe",
                              action_name,
                              SLOT( changeActionToPipe() ) );
        }
        if (Resources::isTargetActionSupported(platform,"Tag"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Tag,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Tag",
                              action_name,
                              SLOT( changeActionToTag() ) );
        }
        if (Resources::isTargetActionSupported(platform,"Classify"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Classify,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Classify",
                              action_name,
                              SLOT( changeActionToClassify() ) );
        }
        if (Resources::isTargetActionSupported(platform,"Custom"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Custom,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Custom",
                              action_name,
                              SLOT( changeActionToCustom() ) );
        }
        if (Resources::isTargetActionSupported(platform,"Branch"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Branch,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Branch",
                              action_name,
                              SLOT( changeActionToBranch()     ) );
        }
        if (Resources::isTargetActionSupported(platform,"Route"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Route,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Route",
                              action_name,
                              SLOT( changeActionToRoute()     ) );
        }
        if (Resources::isTargetActionSupported(platform,"Continue"))
        {
            action_name = getActionNameForPlatform(PolicyRule::Continue,
                    platform.c_str());
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Continue",
                              action_name,
                              SLOT( changeActionToContinue()     ) );
        }
        
        popup->insertSeparator ();
        int paramID;
        paramID = addPopupMenuItem( this, popup,
                                    "",
                                    tr("Parameters"),
                                    SLOT( editSelected() ) );
        
        PolicyRule *rule = PolicyRule::cast( ruleIndex[row] );
        if (rule!=NULL)
        {
            string act = rule->getActionAsString();
            if (Resources::getActionEditor(platform,act)=="None")
                popup->setItemEnabled(paramID, false);
        }

        break;
    }
    case Direction:
    {
        addPopupMenuItem( this, popup,
                          "/FWBuilderResources/UI/Icons/Inbound",
                          tr("Inbound"),
                          SLOT( changeDirectionToIn()      ) );
        addPopupMenuItem( this, popup,
                          "/FWBuilderResources/UI/Icons/Outbound",
                          tr("Outbound"),
                          SLOT( changeDirectionToOut()     ) );
        addPopupMenuItem( this, popup,
                          "/FWBuilderResources/UI/Icons/Both",
                          tr("Both"),
                          SLOT( changeDirectionToBoth()    ) );

        break;
    }
    case Options:
    {
        addPopupMenuItem( this, popup,
                          "/FWBuilderResources/UI/Icons/Options",
                          tr("Rule Options"),
                          SLOT( editSelected()          ) );
        if (fwbdebug) qDebug(ruleset->getTypeName().c_str());
        if (ruleset->getTypeName() == Policy::TYPENAME) {
        
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Log",
                              tr("Logging On"),
                              SLOT( changeLogToOn()            ) );
            addPopupMenuItem( this, popup,
                              "/FWBuilderResources/UI/Icons/Blank",
                              tr("Logging Off"),
                              SLOT( changeLogToOff()          ) );
        }
        break;
    }
    case Object:
    case Time:
    {
        RuleElement *re = getRE(row,col);
        if(re==NULL) return;

        int editID = popup->insertItem(
                tr("Edit")   , this , SLOT( editSelected() ) );
        popup->insertSeparator();
        int copyID = popup->insertItem(
                tr("Copy")   , this , SLOT( copySelectedObject() ) );
        int cutID  = popup->insertItem(
                tr("Cut")    , this , SLOT( cutSelectedObject() ) );
        popup->insertItem( tr("Paste")  , this , SLOT( pasteObject() ) );
//        popup->insertSeparator();
        int delID  =popup->insertItem(
                tr("Delete") ,  this , SLOT( deleteSelectedObject() ) );
        popup->insertSeparator();
        int fndID = popup->insertItem(
                tr("Where used") , this , SLOT( findWhereUsedSlot()));
        int revID = popup->insertItem(
                tr("Reveal in tree") ,this , SLOT( revealObjectInTree() ) );
        popup->insertSeparator();
        int negID  = popup->insertItem(
                tr("Negate") , this , SLOT( negateRE() ) );

        if (selectedObject == NULL || re->isAny())
            popup->setItemEnabled(editID, false);
        popup->setItemEnabled(copyID, !re->isAny());
        popup->setItemEnabled(cutID,  !re->isAny());
        popup->setItemEnabled(delID,  !re->isAny());

        string cap_name;
        if (InterfacePolicy::cast(ruleset)!=NULL) cap_name="negation_in_interface_policy";
        if (Policy::cast(ruleset)!=NULL)          cap_name="negation_in_policy";
        if (NAT::cast(ruleset)!=NULL)             cap_name="negation_in_nat";

        Firewall *f = getFirewall();

        bool supports_neg=false;
        try  {
            supports_neg=Resources::getTargetCapabilityBool(f->getStr("platform"),
                    cap_name);
        } catch (FWException &ex)
        {
            QMessageBox::critical( NULL , "Firewall Builder",
                                   ex.toString().c_str(),
                                   QString::null,QString::null);
        }
        popup->setItemEnabled(negID, supports_neg &&  !re->isAny());
        popup->setItemEnabled(fndID, !re->isAny());
        popup->setItemEnabled(revID, !re->isAny());

        break;
    }

    case RuleOp:
    {
//        setCurrentCell(row,0);
        setFocus();

        Rule *rule = Rule::cast(ruleIndex[row]);
        if (rule==NULL)
        {
            addPopupMenuItem( this, popup, "", tr("Insert Rule"),       SLOT( insertRule()          ) );
        } else
        {
            int rn = rule->getPosition();
            int selectionSize=lastSelectedRule-firstSelectedRule+1;
            QLabel *l;
            if (selectionSize!=1)
            {
                l=new QLabel(QString("  ")+tr("Rules: %1-%2").arg(firstSelectedRule).arg(lastSelectedRule), popup);
            } else
            {
                l=new QLabel(QString("  ")+tr("Rule: %1").arg(rn), popup);
            }
            l->setAlignment( Qt::AlignHCenter );
            popup->insertItem( l );
            popup->insertSeparator();
            l=new QLabel(QString("  ")+tr("Color Label:"), popup);
            popup->insertItem( l );
            ColorLabelMenuItem *cl = new ColorLabelMenuItem(popup);
            popup->insertItem(cl);

            connect( cl,   SIGNAL( returnColor(const QString&) ),
                     this, SLOT( setRuleColor(const QString&) ) );

            popup->insertSeparator();

            QString itemLbl;

            addPopupMenuItem( this, popup, "", tr("Insert Rule"),
                              SLOT( insertRule() ) );
            addPopupMenuItem( this, popup, "", tr("Add Rule Below"),
                              SLOT( addRuleAfterCurrent() ) );

            if (selectionSize==1) itemLbl=tr("Remove Rule");
            else                  itemLbl=tr("Remove Rules");
            addPopupMenuItem( this, popup, "", itemLbl,
                              SLOT( removeRule()));
            if (selectionSize==1) itemLbl=tr("Move Rule");
            else                  itemLbl=tr("Move Rules");
            addPopupMenuItem( this, popup, "", itemLbl,
                              SLOT( moveRule()));

            popup->insertSeparator();

            addPopupMenuItem( this, popup, "", tr("Copy Rule"),
                              SLOT( copyRule() ) );
            addPopupMenuItem( this, popup, "", tr("Cut Rule"),
                              SLOT( cutRule() ) );
            addPopupMenuItem( this, popup, "", tr("Paste Rule Above"),
                              SLOT( pasteRuleAbove() ) );
            addPopupMenuItem( this, popup, "", tr("Paste Rule Below"),
                              SLOT( pasteRuleBelow() ) );

            popup->insertSeparator();
            Rule *r = Rule::cast( ruleIndex[row] );
            if (r->isDisabled())
            {
                if (selectionSize==1) itemLbl=tr("Enable Rule");
                else                  itemLbl=tr("Enable Rules");
                addPopupMenuItem( this, popup, "", itemLbl,
                                  SLOT( enableRule() ) );
            }else{
                if (selectionSize==1) itemLbl=tr("Disable Rule");
                else                  itemLbl=tr("Disable Rules");
                addPopupMenuItem( this, popup, "", itemLbl,
                                  SLOT( disableRule() ) );
            }
        }
        break;
    }

    case Comment:
        popup->insertItem( tr("Edit")   , this , SLOT( editSelected() ) );
        break;

    case Metric:
        popup->insertItem( tr("Edit")   , this , SLOT( editSelected() ) );
        break;

    default:
        popup->insertItem( tr("Edit")   , this , SLOT( editRE() ) );
        break;
}

    

    popup->exec( pos );

    delete popup;
}*/


void RuleSetView::revealObjectInTree()
{
    if ( selectedObject!=NULL)
        om->openObject(selectedObject);
}

void RuleSetView::findWhereUsedSlot()
{
    if ( selectedObject!=NULL)
        mw->findWhereUsed(selectedObject);
}

void RuleSetView::setRuleColor(const QString &c)
{
    if (!isTreeReadWrite(this,ruleset)) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
//        removeSelection(0);
//        verticalHeader()->update();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }

    if ( firstSelectedRule!=-1 )
    {
        for (int i=firstSelectedRule; i<=lastSelectedRule; ++i)
        {
            Rule *rule = Rule::cast( ruleIndex[i] );
            FWOptions *ropt = rule->getOptionsObject();
            ropt->setStr("color",c.latin1());
            dirtyRows[i]=1;
            adjustRow(i);   // this causes repaint
        }
    }
}

void RuleSetView::changeAction(PolicyRule::Action act)
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        PolicyRule *rule = PolicyRule::cast( ruleIndex[currentRow()] );
        FWOptions *ruleopt = rule->getOptionsObject();
        PolicyRule::Action old_act=rule->getAction();
        RuleSet *subset = NULL;
        if (old_act==PolicyRule::Branch)
            subset = rule->getBranch();

        if (act!=old_act)
        {
            if (old_act==PolicyRule::Branch)
                mw->removePolicyBranchTab( subset );

            rule->setAction( act );
            if (!changingRules)
                om->updateLastModifiedTimestampForOneFirewall(getFirewall());
        }

        ruleopt->setBool("stateless", getStatelessFlagForAction(rule));

        oe->actionChanged(rule);    
    }
}

void RuleSetView::changeActionToAccept()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall action: changeActionToAccept"); 
    changeAction( PolicyRule::Accept );
}

void RuleSetView::changeActionToDeny()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall changed: changeActionToDeny"); 
    changeAction( PolicyRule::Deny );
}

void RuleSetView::changeActionToReject()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall changed: changeActionToReject"); 
    changeAction( PolicyRule::Reject );
}

void RuleSetView::changeActionToAccounting()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall changed: changeActionToAccounting"); 
    changeAction( PolicyRule::Accounting );
}

void RuleSetView::changeActionToPipe()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall changed: changeActionToPipe"); 
    changeAction( PolicyRule::Pipe );
}

void RuleSetView::changeActionToTag()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall changed: changeActionToTag"); 
    changeAction( PolicyRule::Tag );
}

void RuleSetView::changeActionToClassify()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall changed: changeActionToClassify"); 
    changeAction( PolicyRule::Classify );
}

void RuleSetView::changeActionToCustom()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall changed: changeActionToCustom"); 
    changeAction( PolicyRule::Custom );
}

void RuleSetView::changeActionToRoute()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall changed: changeActionToRoute"); 
    changeAction( PolicyRule::Route );
}

void RuleSetView::changeActionToContinue()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug) qDebug("Firewall changed: changeActionToContinue"); 
    changeAction( PolicyRule::Continue );
}

void RuleSetView::changeActionToBranch()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        if (fwbdebug) qDebug("Firewall action: changeActionToBranch"); 
        changeAction( PolicyRule::Branch );

        addRuleBranch( PolicyRule::cast( ruleIndex[currentRow()] ) );
    }
}

void RuleSetView::addRuleBranch(PolicyRule *rule)
{
    if (fwbdebug) qDebug("RuleSetView::addRuleBranch"); 

    FWOptions *ropt = rule->getOptionsObject();
    QString branchName = ropt->getStr("branch_name").c_str();
    if (branchName.isEmpty())
    {
        QString bn = QString("rule%1_branch").arg(rule->getPosition());
        ropt->setStr("branch_name",bn.ascii());
    }
    RuleSet *subset = rule->getBranch();
    if (subset==NULL)
    {
        // can change action only for the policy rule, therefore
        // branch can only be a Policy (i.e. can not be NAT)
        subset = new Policy();
        rule->add(subset);
    }
    mw->addPolicyBranchTab(subset);
}


void RuleSetView::changeDitection(PolicyRule::Direction dir)
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        PolicyRule *rule = PolicyRule::cast( ruleIndex[currentRow()] );
        PolicyRule::Direction old_dir=rule->getDirection();
        if (dir!=old_dir)
        {
            if (!changingRules)
                om->updateLastModifiedTimestampForOneFirewall(getFirewall());
            rule->setDirection( dir );
        }
    }
}

void RuleSetView::changeDirectionToIn()
{
    if (fwbdebug) qDebug("Firewall changed: changeDirectionToIn"); 
    changeDitection( PolicyRule::Inbound );
}

void RuleSetView::changeDirectionToOut()
{
    if (fwbdebug) qDebug("Firewall changed: changeDirectionToOut"); 
    changeDitection( PolicyRule::Outbound );
}

void RuleSetView::changeDirectionToBoth()
{
    if (fwbdebug) qDebug("Firewall changed: changeDirectionToBoth"); 
    changeDitection( PolicyRule::Both );
}

void RuleSetView::updateCurrentCell()
{
//    setCurrentCell( currentRow(), currentColumn() );
    updateCell( currentRow(),currentColumn());
}

void RuleSetView::updateAll()
{
    int r=0;
    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++,r++)
        dirtyRows[r] = 1;

    updateContents();
}

void RuleSetView::changeLogToOn()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        PolicyRule *rule = PolicyRule::cast( ruleIndex[currentRow()] );
        if (fwbdebug) qDebug("Firewall changed: changeLogToOn"); 
        if (!changingRules)
            om->updateLastModifiedTimestampForOneFirewall(getFirewall());
        rule->setLogging( true );
    }
}

void RuleSetView::changeLogToOff()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        PolicyRule *rule = PolicyRule::cast( ruleIndex[currentRow()] );
        if (fwbdebug) qDebug("Firewall changed: changeLogToOff"); 
        if (!changingRules)
            om->updateLastModifiedTimestampForOneFirewall(getFirewall());
        rule->setLogging( false );
    }
}
//done till here
void RuleSetView::copySelectedObject()
{
    if ( selectedObject!=NULL)
    {
        FWObjectClipboard::obj_clipboard->clear();
        FWObjectClipboard::obj_clipboard->add( selectedObject );
    }
}

void RuleSetView::cutSelectedObject()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( selectedObject!=NULL)
    {
        FWObjectClipboard::obj_clipboard->clear();
        FWObjectClipboard::obj_clipboard->add( selectedObject );
        deleteSelectedObject();
    }
}

void RuleSetView::deleteSelectedObject()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( selectedObject!=NULL)
    {
        deleteObject(currentRow(),currentColumn(),selectedObject);
        setSelectedObject(NULL);
    }
}

void RuleSetView::deleteObject(int row, int col, FWObject *obj)
{
    RuleElement *re = getRE(row,col);
    if (re==NULL || re->isAny()) return;
    string id = obj->getId();

    if (fwbdebug)
    {
        qDebug("RuleSetView::deleteObject row=%d col=%d id=%s",
               row,col,id.c_str()); 
        qDebug("obj = %p",re->getRoot()->findInIndex(id));
        int rc = obj->ref()-1;  obj->unref();
        qDebug("obj->ref_counter=%d",rc);
    }

    if (!changingRules)
        om->updateLastModifiedTimestampForOneFirewall(getFirewall());
    
    re->removeRef(obj);

    if (re->isAny()) re->setNeg(false);

    if (fwbdebug)
    {
        qDebug("RuleSetView::deleteObject re->size()=%d",re->size()); 
        qDebug("obj = %p",re->getRoot()->findInIndex(id));
        int rc = obj->ref()-1;  obj->unref();
        qDebug("obj->ref_counter=%d",rc);
    }

    dirtyRows[row]=1;
    adjustColumn(col);
    adjustRow(row);
    updateCell(row,col);
    mw->findObjectWidget->reset();
}

bool RuleSetView::insertObject(int row, int col, FWObject *obj)
{
    if (fwbdebug)
      qDebug("RuleSetView::insertObject  -- insert object %s",
             obj->getName().c_str());

    if (!isTreeReadWrite(this,ruleset)) return false;

    if (getColType(col)!=Object && getColType(col)!=Time) return false;

    RuleElement *re = getRE(row,col);
    assert (re!=NULL);

    if (fwbdebug)
      qDebug("RuleSetView::insertObject  -- validate");

    if (! re->validateChild(obj) )
    {
      if (fwbdebug)
        qDebug("RuleSetView::insertObject  -- validation failed");
    
        if (RuleElementRItf::cast(re)) 
            
            QMessageBox::information( NULL , "Firewall Builder",
                    "A single interface belonging to this firewall is expected in this field.",
                    QString::null,QString::null);
        
        else if (RuleElementRGtw::cast(re))
            
            QMessageBox::information( NULL , "Firewall Builder",
                    "A single ip adress is expected here. You may also insert a host or a network adapter leading to a single ip adress.",
                    QString::null,QString::null);
        
        return false;
    }

    if (re->getAnyElementId()==obj->getId()) return false;

    if (fwbdebug)
      qDebug("RuleSetView::insertObject  -- check for duplicates");

    if (! re->isAny())
    {
/* avoid duplicates */
        string cp_id=obj->getId();
        list<FWObject*>::iterator j;
        for(j=re->begin(); j!=re->end(); ++j)     
        {
            FWObject *o=*j;
            if(cp_id==o->getId()) return false;

            FWReference *ref;
            if( (ref=FWReference::cast(o))!=NULL &&
                cp_id==ref->getPointerId()) return false;
        }
    }

    if (fwbdebug)
      qDebug("RuleSetView::insertObject  -- add reference");

    re->addRef(obj);
    dirtyRows[row]=1;

    if (fwbdebug)
      qDebug("RuleSetView::insertObject  -- adjust and update table cell");

    adjustColumn(col);
    adjustRow(row);
    updateCell(row,col);

    if (fwbdebug) qDebug("Firewall changed: insertObject"); 
    if (!changingRules)
        om->updateLastModifiedTimestampForOneFirewall(getFirewall());
    return true;
}

void RuleSetView::pasteObject()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    vector<string>::iterator i;
    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        FWObject *co= mw->db()->findInIndex(*i);
        if (Rule::cast(co)!=NULL)  pasteRuleAbove();
        else
        {
            if (currentRow()>=0)
                insertObject(currentRow(),currentColumn(),co);
        }
    }

/*
    if (FWObjectClipboard::obj_clipboard->getObject()!=NULL)
        insertObject(currentRow(),currentColumn(),
                     FWObjectClipboard::obj_clipboard->getObject() );
*/
}

void RuleSetView::negateRE()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        RuleElement *re = getRE(currentRow(),currentColumn());
        if (re==NULL) return;
        if (fwbdebug) qDebug("Firewall changed: negateRE"); 
        re->toggleNeg();
        if (!changingRules)
            om->updateLastModifiedTimestampForOneFirewall(getFirewall());

        updateCell(currentRow(),currentColumn());
    }
}

void RuleSetView::editRE()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    editCell(currentRow(),currentColumn());
}


/* CWP DONE
FWObject* RuleSetView::getObj(int row, int col, int mouse_y_pos, QRect *objr)
{
    RuleElement *re = getRE(row,col);
    if (re==NULL) return NULL;

    QRect cr=cellGeometry(row,col);

/ *
 * n     is the number of objects in the cell
 * y_rel is a distance of the mouse cursor from the top of the cell
 * h     is the cell height
 * /
    int   y_rel = mouse_y_pos-cr.y();
    int   y_obj = cr.y();
    int   on=0;
    int   oy=0;
    FWObject *o1=NULL;
    FWObject *obj=NULL;
    FWObject *prev=NULL;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++,on++)
    {
        o1= *i;
        if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
        if (y_rel>oy && y_rel<oy+item_h)
        {
            obj=o1;
            break;
        }
        oy+=item_h;
        y_obj+=item_h;
        prev=o1;
    }
    if (obj==NULL) obj=prev;

    cr.setY(y_obj);
    cr.setHeight(item_h);

    if (objr!=NULL) *objr=cr;

    return obj;
}

void RuleSetView::contentsMousePressEvent( QMouseEvent* ev )
{
    if (fwbdebug)
        qDebug("RuleSetView::contentsMousePressEvent");

    dragging = true;

//    mw->selectRules();

    int row=rowAt(ev->y());
    int col=columnAt(ev->x());

    FWObject *obj=getObj(row,col,ev->y());
    bool needUpdate= (row==currentRow() && col==currentColumn() && selectedObject!=obj);

    if (fwbdebug)
        qDebug("RuleSetView::contentsMousePressEvent  obj=%s  row=%d  col=%d  needUpdate=%d",
               (obj)?obj->getName().c_str():"NULL",row,col,needUpdate);

    if (fwbdebug)
        qDebug("RuleSetView::contentsMousePressEvent  1 currentRow=%d  currentColumn=%d",
               currentRow(),currentColumn());

    QTable::contentsMousePressEvent(ev);

    if (fwbdebug)
        qDebug("RuleSetView::contentsMousePressEvent  2 currentRow=%d  currentColumn=%d",
               currentRow(),currentColumn());

    setSelectedObject(obj);
    updateCell(row,col);
}


void RuleSetView::contentsMouseReleaseEvent( QMouseEvent* ev )
{
    if (fwbdebug)
        qDebug("RuleSetView::contentsMouseReleaseEvent");
    
    if (oe->isVisible() && !switchObjectInEditor( columnAt(ev->x()) ))
    {
        ev->accept();
    } else
        QTable::contentsMouseReleaseEvent(ev);
}


void RuleSetView::contentsMouseMoveEvent( QMouseEvent* ev )
{
    QTable::contentsMouseMoveEvent(ev);
    return;

    if (dragging)
    {
        QDragObject* d = dragObject();
        d->dragMove();
        dragging =false;
    }
}*/

void RuleSetView::keyPressEvent( QKeyEvent* ev )
{
    mw->selectRules();

    RuleElement *re;

    if (ev->key()==Qt::Key_Left || ev->key()==Qt::Key_Right)
    {
        int shift= (ev->key()==Qt::Key_Left) ? -1 : 1;

/* keyboard 'Left' or 'Right', switch to the object with the same
 * number in the cell to the left or to the right
 */
        int objno=0;
        re = getRE(currentRow(),currentColumn());
        if (re!=NULL)
        {
            // in this loop we count objects in objno
            for (FWObject::iterator i=re->begin(); i!=re->end(); ++i,++objno)
            {
                FWObject *o1= *i;
                if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
                if (o1==selectedObject) break;
            }
        }
        re = getRE(currentRow(),currentColumn() + shift);
        if (re==NULL)
        {
            if (oe->isVisible() && !switchObjectInEditor(currentColumn() + shift))
            {
                ev->accept();
            } else
                QTable::keyPressEvent(ev);
            openObjectInTree(getFirewall());
            
            return;
        }

        FWObject *newObj = NULL;
        int n=0;
        for (FWObject::iterator i=re->begin(); i!=re->end(); ++i,++n)
        {
            FWObject *o1= *i;
            if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
            if (n==objno)
            {
                newObj = o1;
                break;
            }
        }
        if (newObj==NULL)
        {
            FWObject *o1=re->back();
            if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
            newObj = o1;
        }

        setSelectedObject(newObj);
        if (oe->isVisible() && !switchObjectInEditor(currentColumn()+shift))
        {
            ev->accept();
        } else
            QTable::keyPressEvent(ev);
        return;
    }

    if (ev->key()==Qt::Key_Down || ev->key()==Qt::Key_Up)
    {
        re = getRE(currentRow(),currentColumn());

        FWObject          *prev=NULL;
        FWObject          *o1  =NULL;
        FWObject::iterator i;
        if (re)
        {
            for (i=re->begin(); i!=re->end(); ++i)
            {
                o1= *i;
                if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
                if (ev->key()==Qt::Key_Up   && o1==selectedObject)   break;
                if (ev->key()==Qt::Key_Down && prev==selectedObject) break;
                prev=o1;
            }
        } else
        {
/* going up and down in a column that does not contain objects (action/direction etc) */
            QTable::keyPressEvent(ev);  // to make new row current
            if (oe->isVisible() && !switchObjectInEditor(currentColumn()))
            {
                ev->accept();
            }
            openObjectInTree(getFirewall());
            return;
        }

        if (ev->key()==Qt::Key_Up && prev==NULL)
        {
/* keyboard 'Up',  switch to the last object in the cell above */
            if (currentRow()-1<0)
            {
                return;
            }

            re = getRE(currentRow()-1,currentColumn());
            if (re!=NULL)  // can be NULL if currentRow is 0
            {
                o1=re->back();
                if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
                setSelectedObject(o1);
                if (oe->isVisible() && !switchObjectInEditor(currentColumn()))
                {
                    ev->accept();
                    return;
                }
            }
            QTable::keyPressEvent(ev);
            return;
        }

        if (ev->key()==Qt::Key_Down && i==re->end())
        {
/* keyboard 'Down',  switch to the first object in the cell below */
            if (currentRow()+1>=int(ruleset->size()))
            {
                return;
            }

            re = getRE(currentRow()+1,currentColumn());
            if (re!=NULL)
            {
                o1=re->front();
                if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
                setSelectedObject(o1);
                if (oe->isVisible() && !switchObjectInEditor(currentColumn()))
                {
                    ev->accept();
                    return;
                }
            }
            QTable::keyPressEvent(ev);
            return;
        }
/* switching to another object in the same cell */
        setSelectedObject((ev->key()==Qt::Key_Up) ? prev : o1);
        if (oe->isVisible() && !switchObjectInEditor(currentColumn()))
        {
            ev->accept();
            return;
        }

        updateCell(currentRow(),currentColumn());

        ev->accept();
        return;
    }

    if (ev->key()==Qt::Key_Delete)
    {
        deleteSelectedObject();
    }
    
    QTable::keyPressEvent(ev);
}

QDragObject* RuleSetView::dragObject()
{
    FWObject *obj = selectedObject;
    if (obj==NULL) return NULL;

    QString icn =
        Resources::global_res->getObjResourceStr(obj, "icon").c_str();

    list<FWObject*> dragobj;
    dragobj.push_back(obj);

    FWObjectDrag    *drag = new FWObjectDrag(dragobj, this, NULL);

//    QPixmap          pm   = QPixmap::fromMimeSource( icn_filename );
    QPixmap pm;
    if ( ! QPixmapCache::find( icn, pm) )
    {
        pm = QPixmap::fromMimeSource( icn );
        QPixmapCache::insert( icn, pm);
    }

    drag->setPixmap( pm,
                     QPoint( pm.rect().width() / 2,
                             pm.rect().height() / 2 ) );

    return drag;
}

void RuleSetView::dragEnterEvent( QDragEnterEvent *ev)
{
    ev->acceptAction( QTextDrag::canDecode(ev) );
}

void RuleSetView::contentsDragEnterEvent( QDragEnterEvent *ev)
{
    ev->acceptAction( QTextDrag::canDecode(ev) );
}

void RuleSetView::dragMoveEvent( QDragMoveEvent *ev)
{
    QWidget *fromWidget = ev->source();
    // The source of DnD object must be the same instance of fwbuilder
    if (fromWidget)
    {
        if (FWObjectDrag::canDecode(ev) && !ruleset->isReadOnly())
        {
            QHeader *hh = horizontalHeader();
            QHeader *vh = verticalHeader();
        
            int  row = rowAt( ev->pos().y() + contentsY() - hh->height() );
            int  col = columnAt( ev->pos().x() + contentsX() - vh->width() );

            if (col<0 || ( getColType(col)!=Object && getColType(col)!=Time) )
            {
                ev->acceptAction(false);
                return;
            }

            RuleElement *re = getRE(row,col);
            if (re==NULL)
            {
                ev->acceptAction(false);
                return;
            }

            bool  acceptE = true;
            list<FWObject*> dragol;

            /*
             * See bug 1226069  Segfault: Drag&Drop between two instances
             *
             * There is a problem with this code: Since we pass "live" pointer to
             * FWObject* object in the drag event, drag&drop should not be used to
             * pass objects between different instances of the GUI running at the
             * same time. Trying to do so causes receiving program to crash
             * because it tries to access an object using memory pointer that it
             * obtained from another program. Apparently C++ standard does not
             * define behavior of the program in case it tries to access memory
             * using bad pointer, so it does not throw any exception we could
             * catch. Real solution, of course, is to pass object's ID together
             * with some identification for the data file in the drag event and
             * then on receiving end scan the tree looking for this
             * object. Unfortunately this is too slow with our current API desing.
             *
             * TODO: The problem requires more detailed investigation, possibly
             * including API redesign.
             */

            if (FWObjectDrag::decode(ev, dragol))
            {
                for (list<FWObject*>::iterator i=dragol.begin();
                     i!=dragol.end(); ++i)
                {
                    FWObject *dragobj = NULL;
                    dragobj = dynamic_cast<FWObject*>(*i);
                    if(dragobj!=NULL)
                        acceptE &= re->validateChild(dragobj);
                }
                ev->acceptAction( acceptE );
                return;
            }
        }
    }
    ev->accept(false);
}


void RuleSetView::dropEvent( QDropEvent *ev)
{
    if (!isTreeReadWrite(this,ruleset)) return;

    QHeader *hh = horizontalHeader();
    QHeader *vh = verticalHeader();

    int  row = rowAt( ev->pos().y() + contentsY() - hh->height() );
    int  col = columnAt( ev->pos().x() + contentsX() - vh->width() );

    if (row<0 || col<0) return;

/* without this check the user can drag and drop an object inside the
 * same rule element. This is bad because it is considered a change,
 * even though nothing really changes. With this check, we can not
 * drag and drop an object from the tree into a selected cell... 

    if (row==currentRow() && col==currentColumn()) return;
 */

    if (fwbdebug)
    {
        qDebug("RuleSetView::dropEvent  drop event mode=%d", ev->action());
        qDebug("                        src widget = %p", ev->source());
        qDebug("                              this = %p", this   );
    }

    list<FWObject*> dragol;
    if (FWObjectDrag::decode(ev, dragol))
    {
        for (list<FWObject*>::iterator i=dragol.begin();
             i!=dragol.end(); ++i)
        {
            FWObject *dragobj = *i;
            assert(dragobj!=NULL);

            if (ev->source()!=this)
            {
                insertObject(row,col,dragobj);
            } else
            {
                switch (ev->action())
                {
                case QDropEvent::Move:
                    if (insertObject(row,col,dragobj) )
                        deleteObject(currentRow(),currentColumn(),dragobj);
                    break;

                default:
                    insertObject(row,col,dragobj);
                    break;
                }
            }
        }
        ev->acceptAction();
    }
}

void RuleSetView::removeRule()
{
    if (!hasFocus()) return;
    if (!isTreeReadWrite(this,ruleset)) return;
/* we call removeRule in a loop. Set flag changingRules to true to prevent
 * removeRule from calling updateLastModifiedTimestampForAllFirewalls each time
 */
    changingRules = true;

    mw->findObjectWidget->reset();
    
    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
        clearSelection();
//        removeSelection(0);
        verticalHeader()->update();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }

/* remove rules firstSelectedRule through lastSelectedRule */

    if ( firstSelectedRule!=-1 )
    {
        clearSelection();
        verticalHeader()->update();
        if (fwbdebug) qDebug("Firewall changed: removeRule"); 

        setUpdatesEnabled(false);
        for (int rn=lastSelectedRule; rn>=firstSelectedRule; --rn)
        {
            if (oe->isVisible() && oe->getOpened()==ruleIndex[rn]) oe->close();

            bool delete_branch_tab = false;
            PolicyRule *r = PolicyRule::cast( ruleIndex[rn] );
            RuleSet *subset = NULL;
            if (r)
            {
                if (r->getAction()==PolicyRule::Branch)
                {
                    subset = r->getBranch();
                    delete_branch_tab = true;
                }
            }

            if ( ruleset->deleteRule(rn) )
            {
                if (delete_branch_tab)
                    mw->removePolicyBranchTab( subset );

                int lastN=ruleIndex.size()-1;
                ruleIndex.erase(rn);

                for (int i=rn; i<lastN; ++i)   ruleIndex[i]=ruleIndex[i+1];

                removeRow(rn);

                for (int row=rn; row<lastN; ++row)
                    setRuleNumber(row, Rule::cast( ruleIndex[row] ));

            }
        }
        setUpdatesEnabled(true);

        setCurrentCell( firstSelectedRule, currentColumn() );
        updateContents();

        changingRules = false;
        om->updateLastModifiedTimestampForOneFirewall(getFirewall());
    }
}

/*CWP DONE:
void RuleSetView::insertRule()
{
//    if (!hasFocus()) return;       // <-- can insert rule even if does not have focus
    if (!isTreeReadWrite(this,ruleset)) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
        removeSelection(0);
        verticalHeader()->update();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }

    changingRules = true;
    insertRule(firstSelectedRule,NULL);
    changingRules = false;
    om->updateLastModifiedTimestampForOneFirewall(getFirewall());
}*/

void RuleSetView::addRuleAfterCurrent()
{
    if (!hasFocus()) return;
    if (!isTreeReadWrite(this,ruleset)) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
        removeSelection(0);
        verticalHeader()->update();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }

    changingRules = true;
    insertRule(lastSelectedRule+1,NULL);
    changingRules = false;
    om->updateLastModifiedTimestampForOneFirewall(getFirewall());
}

/*CWP DONE:
Rule* RuleSetView::insertRule(int pos, FWObject *r)
{
    if (r!=NULL && 
        ruleset->getTypeName()==Policy::TYPENAME && 
        r->getTypeName()!=PolicyRule::TYPENAME)  return NULL;
    if (r!=NULL && 
        ruleset->getTypeName()==NAT::TYPENAME    && 
        r->getTypeName()!=NATRule::TYPENAME   )  return NULL;

    if (pos<0) pos=0;

//    insertRows( pos , 1 );

    Rule *newrule=NULL;
    if ( ruleset->getRuleSetSize()==0) newrule=ruleset->insertRuleAtTop();
    else
    {
        if (pos==ruleset->getRuleSetSize())
        {
            newrule=ruleset->appendRuleAtBottom();
        } else
            newrule=ruleset->insertRuleBefore(pos);
        assert(newrule!=NULL);
    }

    if (fwbdebug && PolicyRule::cast(r)!=NULL)
        qDebug(QString("RuleSetView::insertRule: r->direction=%1")
               .arg(PolicyRule::cast(r)->getDirectionAsString().c_str()));

    PolicyRule *newrule_as_policy_rule = PolicyRule::cast(newrule);

    if (newrule_as_policy_rule)
    {
        newrule_as_policy_rule->setLogging(supports_logging);
        newrule_as_policy_rule->setAction(PolicyRule::Deny);
        newrule_as_policy_rule->setDirection(PolicyRule::Both);
        FWOptions *ruleopt = newrule_as_policy_rule->getOptionsObject();
        ruleopt->setBool("stateless",
                         getStatelessFlagForAction(newrule_as_policy_rule));
    }

    if (r!=NULL)  copyRuleContent(newrule,Rule::cast(r));

    for (int i=ruleIndex.size(); i>pos; --i)  ruleIndex[i]=ruleIndex[i-1];
    ruleIndex[pos] = newrule;

    if (fwbdebug && PolicyRule::cast(r)!=NULL)
        qDebug(QString("RuleSetView::insertRule: newrule->direction=%1")
               .arg(PolicyRule::cast(newrule)->getDirectionAsString().c_str()));


    insertRows( pos , 1 );

    for (int i=ruleIndex.size(); i>=pos; --i)
        setRuleNumber(i, Rule::cast(ruleIndex[i]));

    if (newrule_as_policy_rule!=NULL &&
        newrule_as_policy_rule->getAction()==PolicyRule::Branch )
        addRuleBranch( newrule_as_policy_rule );

    dirtyRows[pos]=1;
////    adjustRow(pos);

    setCurrentCell( pos, currentColumn() );
    updateCell(pos,currentColumn());

    if (fwbdebug) qDebug("Firewall changed: insertRule"); 

    return newrule;
}

void RuleSetView::copyRuleContent(Rule *dst, Rule *src)
{
    string id=dst->getId();
    int     p=dst->getPosition();

    if ( src->isDisabled() ) dst->disable();
    else                     dst->enable();

    dst->shallowDuplicate(src,false);

//    map<string, string>::const_iterator i;
//    for(i=dst->dataBegin(); i!=dst->dataEnd(); ++i) {
//        string f= (*i).first;
//        dst->setStr(f, src->getStr(f) );
//    }

    dst->setComment( src->getComment() );

    list<FWObject*>::iterator j;
    for(j=dst->begin(); j!=dst->end(); ++j)
    {
        string    dtype= (*j)->getTypeName();
        FWObject *selem= src->getFirstByType(dtype);
        if (selem!=NULL) 
            (*j)->duplicate(selem);
    }

    if (id!="") dst->setId(id);
    dst->setPosition(p);
} */

void RuleSetView::moveRule()
{
    if (!hasFocus()) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }
    int selectionSize=lastSelectedRule-firstSelectedRule+1;

/* calculate acceptable range of rule numbers for the dialog */
    int minRN=0;
    int maxRN=ruleset->size()-selectionSize;

    askRuleNumberDialog_q d(this);
    d.newRuleNum->setMinValue(minRN);
    d.newRuleNum->setMaxValue(maxRN);
    
    if (d.exec()==QDialog::Accepted)
    {
        int newN = d.newRuleNum->value();
        int nn   = newN;
        if (firstSelectedRule==nn) return;

        clearSelection();
//        removeSelection(0);
        verticalHeader()->update();

        setUpdatesEnabled(false);

        if (firstSelectedRule>nn)
        {  // moving block of rules up
            for (int i=firstSelectedRule; i<=lastSelectedRule; i++)
            {
                int j=i;
                while (j!=nn)
                {
                    if (!ruleset->moveRuleUp(j)) return;

                    FWObject *r=ruleIndex[j];
                    ruleIndex[j]=ruleIndex[j-1];
                    ruleIndex[j-1]=r;

                    swapRows(j-1,j);

                    dirtyRows[j-1]=1;
                    dirtyRows[j]=1;

                    Rule *rule = Rule::cast( ruleIndex[j-1] );
                    setRuleNumber(j-1,rule);
                    rule = Rule::cast( ruleIndex[j] );
                    setRuleNumber(j,rule);

                    --j;
                }
                nn++;
            }
        } else 
        {   // moving block of rules down
            for (int i=lastSelectedRule; i>=firstSelectedRule; i--)
            {
                int j=i;
                while (j!=nn+selectionSize-1)
                {
                    if (!ruleset->moveRuleDown(j)) return;

                    FWObject *r=ruleIndex[j];
                    ruleIndex[j]=ruleIndex[j+1];
                    ruleIndex[j+1]=r;

                    swapRows(j+1,j);

                    dirtyRows[j+1]=1;
                    dirtyRows[j]=1;

                    Rule *rule = Rule::cast( ruleIndex[j+1] );
                    setRuleNumber(j+1,rule);
                    rule = Rule::cast( ruleIndex[j] );
                    setRuleNumber(j,rule);

                    ++j;
                }
                nn--;
            }
        }


        setUpdatesEnabled(true);

        setCurrentCell( newN, currentColumn() );
        selectRE( newN , currentColumn() );
        updateContents();

        changingRules = false;
        om->updateLastModifiedTimestampForOneFirewall(getFirewall());
    }
}

void RuleSetView::moveRuleUp()
{
    if (!hasFocus()) return;
    int rn=currentRow();
/* swap rule rn and rn-1 */

    if (rn==0) return;

    if (ruleset->moveRuleUp(rn))
    {
        FWObject *r=ruleIndex[rn];
        ruleIndex[rn]=ruleIndex[rn-1];
        ruleIndex[rn-1]=r;

        swapRows(rn-1,rn);

        dirtyRows[rn-1]=1;
        dirtyRows[rn]=1;

        setCurrentCell( rn-1, currentColumn() );
        selectRE( rn-1 , currentColumn() );
    }
}

void RuleSetView::moveRuleDown()
{
    if (!hasFocus()) return;
    int rn=currentRow();
/* swap rule rn and rn+1 */

    if (rn==ruleset->getRuleSetSize()-1) return;

    if (ruleset->moveRuleDown(rn))
    {
        FWObject *r=ruleIndex[rn];
        ruleIndex[rn]=ruleIndex[rn+1];
        ruleIndex[rn+1]=r;

        swapRows(rn+1,rn);

        dirtyRows[rn+1]=1;
        dirtyRows[rn]=1;

        setCurrentCell( rn+1, currentColumn() );
        selectRE( rn+1 , currentColumn() );
    }
}


void RuleSetView::copyRule()
{
    if (!hasFocus()) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
//        removeSelection(0);
//        verticalHeader()->update();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }

    if ( firstSelectedRule!=-1 )
    {
        FWObjectClipboard::obj_clipboard->clear();
        for (int i=firstSelectedRule; i<=lastSelectedRule; ++i)
        {
            FWObject *rule = ruleIndex[i];

            if (fwbdebug && PolicyRule::cast(rule)!=NULL)
                qDebug(QString("RuleSetView::copyRule: direction=%1")
                       .arg(PolicyRule::cast(rule)->getDirectionAsString().c_str()));

            FWObjectClipboard::obj_clipboard->add( rule );
        }
    }
}

void RuleSetView::cutRule()
{
    copyRule();
    removeRule();
}

void RuleSetView::pasteRuleAbove()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    changingRules = true;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
        removeSelection(0);
        verticalHeader()->update();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }
    if (fwbdebug) qDebug("Firewall: pasteRuleAbove"); 
    
/* pick rules in reverse order */
    vector<string>::reverse_iterator i;
    for (i= FWObjectClipboard::obj_clipboard->rbegin();
         i!=FWObjectClipboard::obj_clipboard->rend(); ++i)
    {
        FWObject *co= mw->db()->findInIndex(*i);
        if (!Rule::cast(co)) continue;
        insertRule( firstSelectedRule, co);
    }

    changingRules = false;
    om->updateLastModifiedTimestampForOneFirewall(getFirewall());
}
    

void RuleSetView::pasteRuleBelow()
{    
    if (!isTreeReadWrite(this,ruleset)) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    changingRules = true;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
        removeSelection(0);
        verticalHeader()->update();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }

    int n=0;
    vector<string>::iterator i;
    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i,++n)
    {
        FWObject *co= mw->db()->findInIndex(*i);
        if (!Rule::cast(co)) continue;
        insertRule( lastSelectedRule+1+n, co);
    }

    changingRules = false;
    om->updateLastModifiedTimestampForOneFirewall(getFirewall());

//    if (FWObjectClipboard::obj_clipboard->getObject()!=NULL)
//        insertRule( rn+1, Rule::cast(FWObjectClipboard::obj_clipboard->getObject()) );
}

void RuleSetView::enableRule()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    changingRules = true;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }

    if ( firstSelectedRule!=-1 )
    {
        for (int rn=lastSelectedRule; rn>=firstSelectedRule; --rn)
        {
            Rule *r = Rule::cast( ruleIndex[rn] );
            if (fwbdebug) qDebug("Firewall changed: enableRule"); 
            r->enable();
            setRuleNumber(rn,r);
        }
    }

    changingRules = false;
    om->updateLastModifiedTimestampForOneFirewall(getFirewall());
}

void RuleSetView::disableRule()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    int firstSelectedRule=-1;
    int lastSelectedRule=-1;

    changingRules = true;

    QTableSelection sel=selection(0);
    if (sel.isActive())
    {
        firstSelectedRule=sel.topRow();
        lastSelectedRule=sel.bottomRow();
    } else
    {
        firstSelectedRule=currentRow();
        lastSelectedRule=currentRow();
    }

    if ( firstSelectedRule!=-1 )
    {
        for (int rn=lastSelectedRule; rn>=firstSelectedRule; --rn)
        {
            Rule *r = Rule::cast( ruleIndex[rn] );
            if (fwbdebug) qDebug("Firewall changed: disableRule"); 
            r->disable();
            setRuleNumber(rn,r);
        }
    }

    changingRules = false;
    om->updateLastModifiedTimestampForOneFirewall(getFirewall());
}





PolicyView::PolicyView(Policy *p, QWidget *parent) : RuleSetView(1, 7, parent)
{
    setName("PolicyView");
    ruleset=p;
    iinit();
    init();
}

void PolicyView::init()
{
    ncols=7 +
        ((supports_time)?1:0) + 
        ((supports_logging && supports_rule_options)?1:0);

    setNumCols(ncols);

    colTypes[-1]=RuleOp;

    int col=0;
    horizontalHeader()->setLabel( col, tr( "Source" ) );      // 0
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Destination" ) ); // 1
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Service" ) );     // 2
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Interface" ) );   // 3
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Direction" ) );   // 4
    colTypes[col++]=Direction;

    horizontalHeader()->setLabel( col, tr( "Action" ) );      // 5
    colTypes[col++]=Action;

    if (supports_time)
    {
        horizontalHeader()->setLabel( col, tr( "Time" ) );    // 6
        colTypes[col++]=Time;
    }

    if (supports_logging && supports_rule_options)
    {
        horizontalHeader()->setLabel( col, tr( "Options" ) );
        colTypes[col++]=Options;
    }

    horizontalHeader()->setLabel( col, tr( "Comment" ) );
    colTypes[col]=Comment;
//    setColumnStretchable(col, true);

    RuleSetView::init();
}

RuleElement* PolicyView::getRE( int row, int col ) 
{
    if (row<0) return NULL;

    if (ruleIndex.count(row)==0) return NULL;
    PolicyRule *r = PolicyRule::cast( ruleIndex[row] );
    if(r==NULL) return NULL;
    return getRE(r, col);
}

RuleElement* PolicyView::getRE( Rule* r, int col ) 
{
    string ret;

    switch (getColType(col))
    {
    case Object:
        switch (col)
        {
        case 0: ret=RuleElementSrc::TYPENAME; break;
        case 1: ret=RuleElementDst::TYPENAME; break;
        case 2: ret=RuleElementSrv::TYPENAME; break;
        case 3: ret=RuleElementItf::TYPENAME; break;
        }
        break;
    case Time:
        ret=RuleElementInterval::TYPENAME; break;
    default: return NULL;
    }

    return RuleElement::cast( r->getFirstByType(ret) );
}

InterfacePolicyView::InterfacePolicyView(InterfacePolicy *p, QWidget *parent) :
    RuleSetView(1,7,parent)
{
    setName("InterfacePolicyView");
    ruleset=p;
    iinit();
    init();
}

void InterfacePolicyView::init()
{
    ncols=6 +
        ((supports_time)?1:0) + 
        ((supports_logging && supports_rule_options)?1:0);

    setNumCols(ncols);

    colTypes[-1]=RuleOp;

    int col=0;
    horizontalHeader()->setLabel( col, tr( "Source" ) );      // 0
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Destination" ) ); // 1
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Service" ) );     // 2
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Direction" ) );   // 3
    colTypes[col++]=Direction;

    horizontalHeader()->setLabel( col, tr( "Action" ) );      // 4
    colTypes[col++]=Action;

    if (supports_time)
    {
        horizontalHeader()->setLabel( col, tr( "Time" ) );    // 5
        colTypes[col++]=Time;
    }

    if (supports_logging && supports_rule_options)
    {
        horizontalHeader()->setLabel( col, tr( "Options" ) );
        colTypes[col++]=Options;
    }

    horizontalHeader()->setLabel( col, tr( "Comment" ) );
    colTypes[col]=Comment;
//    setColumnStretchable(col, true);

//    ncols=col;

    RuleSetView::init();
}

RuleElement* InterfacePolicyView::getRE( int row, int col ) 
{
    if (row<0) return NULL;
    PolicyRule *r = PolicyRule::cast( ruleIndex[row] );
    assert(r!=NULL);
    return getRE(r,col);
}

RuleElement* InterfacePolicyView::getRE( Rule *r, int col ) 
{
    string ret;

    switch (getColType(col))
    {
    case Object:
        switch (col)
        {
        case 0: ret=RuleElementSrc::TYPENAME; break;
        case 1: ret=RuleElementDst::TYPENAME; break;
        case 2: ret=RuleElementSrv::TYPENAME; break;
        }
        break;
    case Time:
        ret=RuleElementInterval::TYPENAME; break;
    default: return NULL;
    }

    return RuleElement::cast( r->getFirstByType(ret) );
}





NATView::NATView(NAT *p, QWidget *parent) : RuleSetView(1,8,parent)
{
    setName("NATView");
    ruleset=p;
    iinit();
    init();
}

void NATView::init()
{
    colTypes[-1]=RuleOp;

    ncols=8;

    int col=0;
    horizontalHeader()->setLabel( col, tr( "Original Src" ) );
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Original Dst" ) );
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Original Srv" ) );
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Translated Src" ) );
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Translated Dst" ) );
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Translated Srv" ) );
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Options" ) );
    colTypes[col++]=Options;

    horizontalHeader()->setLabel( col, tr( "Comment" ) );
    colTypes[col]=Comment;
//    setColumnStretchable(col, true);

//    ncols=col;

    RuleSetView::init();
}

RuleElement* NATView::getRE( int row, int col ) 
{
    if (row<0) return NULL;
    NATRule *r = NATRule::cast( ruleIndex[row] );
    assert(r!=NULL);
    return getRE(r,col);
}

RuleElement* NATView::getRE( Rule *r, int col ) 
{
    string ret;

    switch (getColType(col))
    {
    case Object:
        switch (col)
        {
        case 0: ret=RuleElementOSrc::TYPENAME; break;
        case 1: ret=RuleElementODst::TYPENAME; break;
        case 2: ret=RuleElementOSrv::TYPENAME; break;
        case 3: ret=RuleElementTSrc::TYPENAME; break;
        case 4: ret=RuleElementTDst::TYPENAME; break;
        case 5: ret=RuleElementTSrv::TYPENAME; break;
        }
        break;
    default: return NULL;
    }

    return RuleElement::cast( r->getFirstByType(ret) );
}



RoutingView::RoutingView(Routing *p, QWidget *parent) : RuleSetView(1,5,parent)
{
    setName("RoutingView");
    ruleset=p;
    iinit();
    init();
}

void RoutingView::init()
{
    colTypes[-1]=RuleOp;
    ncols=6;
    setNumCols(ncols);

    int col=0;
    horizontalHeader()->setLabel( col, tr( "Destination" ) );
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Gateway" ) );
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Interface" ) );
    colTypes[col++]=Object;

    horizontalHeader()->setLabel( col, tr( "Metric" ) );
    colTypes[col++]=Metric;

    horizontalHeader()->setLabel( col, tr( "Options" ) );
    colTypes[col++]=Options;

    horizontalHeader()->setLabel( col, tr( "Comment" ) );
    colTypes[col]=Comment;

//    setColumnStretchable(col, true);

//    ncols=col;

    RuleSetView::init();
}

RuleElement* RoutingView::getRE( int row, int col ) 
{
    if (row<0) return NULL;
    RoutingRule *r = RoutingRule::cast( ruleIndex[row] );
    assert(r!=NULL);
    return getRE(r,col);
}

RuleElement* RoutingView::getRE( Rule *r, int col ) 
{
    string ret;

    switch (getColType(col))
    {
    case Object:
        switch (col)
        {
        case 0: ret=RuleElementRDst::TYPENAME; break;
        case 1: ret=RuleElementRGtw::TYPENAME; break;
        case 2: ret=RuleElementRItf::TYPENAME; break;
        }
        break;
    default: return NULL;
    }

    return RuleElement::cast( r->getFirstByType(ret) );
}


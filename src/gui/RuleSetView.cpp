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


#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils.h"
#include "FWWindow.h"
#include "RuleSetView.h"
#include "ObjectEditor.h"
#include "platforms.h"
#include "FWObjectDrag.h"
#include "FWObjectClipboard.h"
#include "findDialog.h"
#include "FWBSettings.h"
#include "SimpleTextEditor.h"
#include "SimpleIntEditor.h"
#include "ActionsDialog.h"
#include "FWObjectPropertiesFactory.h"
#include "ObjectTreeView.h"
#include "FindObjectWidget.h"

#include <ui_askrulenumberdialog_q.h>

#include <iostream>
#include <sstream>
#include <list>

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
#include <qpushbutton.h>
#include <qlabel.h>
#include <qinputdialog.h>
#include "ProjectPanel.h"
#include "RuleRowInfo.h"
#include "RuleGroupPanel.h"
#include <QMdiSubWindow>
#include <QMdiArea>

using namespace libfwbuilder;
using namespace std;

int QMAX(int a, int b)
{
    return (a>b)?a:b;
}

int QMIN(int a, int b)
{
    return (a<b)?a:b;
}


RuleTableModel::RuleTableModel(const int rows, const int columns, RuleSetView *ruleView):
        QAbstractTableModel(static_cast<QWidget*>(ruleView)), m_rowCount(rows), m_columnCount(columns),
        ruleSetView(ruleView)
{};
RuleTableModel::~RuleTableModel() {};

int RuleTableModel::rowCount ( const QModelIndex & ) const
{
    return m_rowCount;
}

int RuleTableModel::columnCount ( const QModelIndex & ) const
{
    return m_columnCount;
}

void RuleTableModel::setColumnCount ( const int &value )
{
    m_columnCount = value;
    reset();
    //need to reset model for RuleSetView::init (one i)
}

void RuleTableModel::setRowCount ( const int &value )
{
    m_rowCount = value;
    reset();
    //need to reset model for RuleSetView::iinit (two is)
}

QVariant RuleTableModel::data ( const QModelIndex &, int /*role*/) const
{
    return QVariant();
}

QVariant RuleTableModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::SizeHintRole)
            return QSize(40, 25);
        if (role == Qt::DisplayRole)
            return QString(header.value(section));
    }

    if (orientation == Qt::Vertical)
    {
        if (role == Qt::SizeHintRole)
            return QSize(45, ruleSetView->getRowHeight(section));
        if (role == Qt::DisplayRole)
        {
            Rule * rule = Rule::cast (ruleSetView->ruleIndex[section]);
            if (rule==NULL)
            {
                return QString ("");
            }
            int counter =0;
            for (int i = section; i>=0; i--)
            {
                if (ruleSetView->ruleIndex[i]==NULL)
                    counter++;
            }
            return QString::number(section-counter);
        }
        if (role == Qt::FontRole)
        {
            //QFont f = QAbstractTableModel::headerData(section, orientation, role ).value<QFont>();
            QFont f = st->getRulesFont();

            if ((section >= ruleSetView->firstSelectedRule) && (section <= ruleSetView->lastSelectedRule))
                f.setBold(true);

            return f;
        }
        if (role == Qt::DecorationRole)
            if (ruleSetView->rulesDisabled[section])
                return QVariant(ruleSetView->negIcon);
            else
                return QVariant(QIcon());
    }

    return QAbstractTableModel::headerData(section, orientation, role );
}

bool RuleTableModel::setHeader ( QStringList qsl )
{
    header = qsl;
    m_columnCount = qsl.size();
    return true;
}

void RuleTableModel::insertRow( const int before_pos )
{
    m_rowCount++;

    ruleSetView->freezeRowSizing();

    ruleSetView->rowHeights.push_back(0);
    for (int i = static_cast<int>(ruleSetView->rowHeights.size())-1; i >= before_pos; i--)
        ruleSetView->rowHeights[i+1] = ruleSetView->rowHeights[i];
    ruleSetView->rowHeights[before_pos] = 30; //standard size

    //we add a row here and system resets below rows' sizes
    //so we had to freeze our sizes for restoring them later

    QAbstractTableModel::beginInsertRows( QModelIndex(), before_pos, before_pos );
    QAbstractTableModel::insertRow(before_pos);
    QAbstractTableModel::endInsertRows();

    //somehow QAbstractItemModel breaks all the sizes after "before_pos" row
    //so we restore them
    if (before_pos > 0)
    for (int i = before_pos-1; i < m_rowCount; i++)
        //step back a little because of another QTableView bug
        ruleSetView->verticalHeader()->resizeSection(i,
            ruleSetView->rowHeights[i]);
    else
    for (int i = before_pos; i < m_rowCount; i++)
        ruleSetView->verticalHeader()->resizeSection(i,
            ruleSetView->rowHeights[i]);

    ruleSetView->unfreezeRowSizing();
    //after this all we have to do is to set final size for row
    //number "before_pos" and update() the table
}

void RuleTableModel::swapRows( const int row1, const int row2 )
{
    int h = ruleSetView->rowHeights[row1];
    ruleSetView->rowHeights[row1] = ruleSetView->rowHeights[row2];
    ruleSetView->rowHeights[row2] = h;

    //just swap rules' sizes and then update() the table by yourself
}

void RuleTableModel::removeRows ( const int row1, const int row2 )
{
    //(row2-row1)+1  rows deleted
    m_rowCount -= (row2-row1)+1;

    ruleSetView->freezeRowSizing();

    for (int i = row1; i < static_cast<int>(ruleSetView->rowHeights.size())-((row2-row1)+1); i++)
        ruleSetView->rowHeights[i] = ruleSetView->rowHeights[i+1+(row2-row1)];
    for (int i = row1; i <= row2; i++)
        ruleSetView->rowHeights.pop_back();

    QAbstractTableModel::beginRemoveRows( QModelIndex(), row1, row2 );
    QAbstractTableModel::removeRows(row1, (row2-row1)+1);
    QAbstractTableModel::endRemoveRows();

    for (int i = row1; i < m_rowCount; i++)
        ruleSetView->verticalHeader()->resizeSection(i,
        ruleSetView->rowHeights[i]);

    ruleSetView->unfreezeRowSizing();
}



RuleDelegate::RuleDelegate(RuleSetView *parent): QAbstractItemDelegate(parent),
    ruleSetView(parent)
{
}

QRect RuleDelegate::cellGeometry(const int row, const int col) const
{
    if ((row < 0) || (col < 0))
        return QRect(-1,-1,-1,-1);

    int left = 0;
    int top = 0;
    QRect cr;

    for (int i = 0; i < row; i++)
        top += ruleSetView->getRowHeight(i);
    cr.setTop(top);
    cr.setHeight(ruleSetView->getRowHeight(row) - 0); //-0 for fitting purposes

    for (int i = 0; i < col; i++)
        left += ruleSetView->getColumnWidth(i);
    cr.setLeft(left);
    cr.setWidth(ruleSetView->getColumnWidth(col));

    return cr;
}

QRect RuleDelegate::cellRect(const int row, const int col) const
{
    if (col==0)
        return QRect(QPoint(0,0),QPoint(10,10));
    return QRect(QPoint(0,0),cellGeometry(row,col).size());
}

void RuleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &,
               const QModelIndex &index) const
{
    if (ruleSetView->ruleIndex[index.row()]==NULL&&index.column()==1)
    {
        QRect size = cellGeometry(index.row(), index.column());
        int w = 0 ;

        for (int i = 1; i < ruleSetView->ruleModel->columnCount(index); i++)
        {
            w += ruleSetView->getColumnWidth(i);
        }
        size.setWidth (w);
        size.setHeight (ruleSetView->getRowHeight(index.row()));
        ruleSetView->paintCell( painter,
                            index.row(),
                            index.column(),
                            size,
                            ruleSetView->itemSelectionRange.contains(index),
                            QPalette() );

        return ;
    }

    ruleSetView->paintCell( painter,
                            index.row(),
                            index.column(),
                            cellGeometry(index.row(), index.column()),
                            ruleSetView->itemSelectionRange.contains(index),
                            QPalette() );
}

QSize RuleDelegate::sizeHint(const QStyleOptionViewItem &,
                   const QModelIndex & ) const
{
    //return QSize(getColumnWidth(index.column()), getRowHeight(index.row()));
    return QSize(30, 19);
}

void RuleSetView::setColumnWidth( const int col, const int width )
{
    if (col < 0)
        return;
/*    if (col == 0)
    {
    while (static_cast<unsigned int>(col+1) > columnWidths.size())
        columnWidths.push_back(30);
    columnWidths[col] = 10;

        return ;
    }*/
    while (static_cast<unsigned int>(col+1) > columnWidths.size())
        columnWidths.push_back(30);
    columnWidths[col] = width;
}

int RuleSetView::getColumnWidth( const int col ) const
{
    if (col < 0)
        return -1;
    if (static_cast<unsigned int>(col+1) > columnWidths.size())
        return 30;
    return columnWidths[col];
}

void RuleSetView::setRowHeight( const int row, const int height )
{
    if (row < 0)
        return;
    while (static_cast<unsigned int>(row+1) > rowHeights.size())
        rowHeights.push_back(30);
    rowHeights[row] = height;
}

int RuleSetView::getRowHeight( const int row ) const
{
    if (row < 0)
        return -1;
    if (static_cast<unsigned int>(row+1) > rowHeights.size())
        return 14;
    return rowHeights[row];
}

class mouseEventFilter : public QObject
{
    protected:
        bool eventFilter( QObject *, QEvent *event)
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

bool headerMouseEventInterceptor::eventFilter( QObject *, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu )
    {
        QContextMenuEvent *e = (QContextMenuEvent*)(event);

        int row = rsv->rowAt( e->pos().y() );

        rsv->contextMenu(row, -1, e->globalPos());
    }

    return false;
}

void RuleSetView::freezeRowSizing()
{
    rowSizingFrozen = true;
}

void RuleSetView::unfreezeRowSizing()
{
    rowSizingFrozen = false;
}

void RuleSetView::horzSectionResized( int index, int /*oldsize*/, int newsize )
{
    if (rowSizingFrozen) return; //we don't change sizes while they're frozen
    setColumnWidth(index, newsize);
}

void RuleSetView::vertSectionResized( int index, int /*oldsize*/, int newsize )
{
    setRowHeight(index, newsize);
}

void LoadPixmap(const QString path, QPixmap &where)
{
    if ( ! QPixmapCache::find( path, where ) )
    {
        where.load( path );
        if (fwbdebug) qDebug("Loading pixmap %s", path.toAscii().constData());
        if (where.width() == 0)
            if (fwbdebug) qDebug("Loading failed");
        QPixmapCache::insert( path, where );
    }
}

QPixmap LoadPixmap(const QString path)
{
    QPixmap p;
    LoadPixmap(path, p);
    return p;
}

RuleSetView::RuleSetView(ProjectPanel *project, int r, int c, QWidget *parent): 
              QTableView( /*r, c,*/ parent ), hme(this),
        ncols(c), RuleElementSpacing(4), selectedObject(0), 
        selectedObjectCol(0), prevSelectedObject(0),
        kbdGoingUp(false), changingSelection(false), changingRules(false),
        firstSelectedRule(-1), lastSelectedRule(-1), ruleModel(new RuleTableModel(r, c, this)), 
        ruleDelegate(new RuleDelegate(this)), m_project(project)
{
    setFont(st->getRulesFont());
    rowSizingFrozen = false;

    setModel(ruleModel);
    setItemDelegate(ruleDelegate);

    setCurrentCell(0,0);

    setDragEnabled(true);
    setAcceptDrops(true);

    setFocusPolicy( Qt::StrongFocus );

    setSelectionMode( QAbstractItemView::ContiguousSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );

    int lm, tm, rm, bm;
    getContentsMargins(&lm, &tm, &rm, &bm);
    setContentsMargins(fontMetrics().width( "W999W" ), tm, rm, bm);

    horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    verticalHeader()->setResizeMode(QHeaderView::Fixed);

    horizontalHeader()->setClickable(false);
    verticalHeader()->setClickable(true);

    horizontalHeader()->setMovable(false);

    setContextMenuPolicy( Qt::CustomContextMenu );

    setDragDropMode(QAbstractItemView::DragDrop);

    QString icn_file = ":/Icons/neg";

    QPixmap pm;
    LoadPixmap(icn_file, pm);
    negIcon = QIcon(pm);

    connect( horizontalHeader(), SIGNAL( sectionResized ( int, int, int ) ),
             this, SLOT( horzSectionResized ( int, int, int ) ) );

    connect( verticalHeader(),   SIGNAL( sectionResized ( int, int, int ) ),
             this, SLOT( vertSectionResized ( int, int, int ) ) );

    connect( this, SIGNAL( customContextMenuRequested(const QPoint&) ),
             this, SLOT( contextMenuRequested(const QPoint&) ) );

    connect( this, SIGNAL( doubleClicked(const QModelIndex&) ),
             this, SLOT( itemDoubleClicked(const QModelIndex&) ) );

    verticalHeader()->installEventFilter( &hme );
    unselect();
}

RuleSetView::~RuleSetView()
{
}

bool RuleSetView::showCommentTip(QPoint pos, QHelpEvent *he)
{
    if (!st->getClipComment())
        return false;
    int col = columnAt(pos.x() - verticalHeader()->width());
    if((pos.y() >= horizontalHeader()->height()) && RuleSetView::Comment == getColType(col))
    {

        QString t="";
        int row = rowAt(pos.y() - horizontalHeader()->height());

        QRect   cr;

        cr=ruleDelegate->cellGeometry(row,col);
        PolicyRule *rule = PolicyRule::cast( getRule(row) );
        if (rule!=NULL)
        {
            Rule *rule = Rule::cast(ruleIndex[row]);
            t= QString::fromUtf8(rule->getComment().c_str());
        }
        cr = QRect(
                cr.left() - horizontalOffset() - 2,
                cr.top() - verticalOffset() - 2,
                cr.width() + 4,
                cr.height() + 4);

        QRect global = QRect(
            viewport()->mapToGlobal(cr.topLeft()), viewport()->mapToGlobal(cr.bottomRight()));
        QToolTip::showText(mapToGlobal( he->pos() ), t, this, global);
        return true;
    }
    return false;
}

bool RuleSetView::event ( QEvent * event )
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *he = (QHelpEvent*) event;
        QPoint pos = he->pos();
        if (showCommentTip(pos, he))
            return true;
        if ((st->getObjTooltips()) && (pos.y() >= horizontalHeader()->height()))
        {
            int col = columnAt(pos.x() - verticalHeader()->width());
            int row = rowAt(pos.y() - horizontalHeader()->height());

            if ((row < 0) || (col < 0)) return true;

            QRect   cr;
            QString t="";

            QPoint contentsMouse = viewport()->mapFromGlobal(mapToGlobal(pos));
            contentsMouse.setY(contentsMouse.y() + verticalOffset() + 3);//+3 for fitting purposed

            cr=ruleDelegate->cellGeometry(row,col);

            if ( RuleSetView::Options == getColType(col) )
            {
                Rule *rule = getRule(row);
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
            else if( RuleSetView::Direction == getColType(col) )
            {
                PolicyRule *rule = PolicyRule::cast( getRule(row) );
                if (rule!=NULL)
                    t = rule->getDirectionAsString().c_str();
            }
            else if( RuleSetView::Action == getColType(col) )
            {
                PolicyRule *rule = PolicyRule::cast( getRule(row) );
                if (rule!=NULL)
                    t= FWObjectPropertiesFactory::getRuleActionPropertiesRich(rule);
            }
            else
            {
                FWObject *obj = getObj(row,col,contentsMouse.y(),&cr);
                if (obj==NULL)
                    return true;
                t=FWObjectPropertiesFactory::getObjectPropertiesDetailed(obj,true,true);
            }

            cr = QRect(
                cr.left() - horizontalOffset() - 2,
                cr.top() - verticalOffset() - 2,
                cr.width() + 4,
                cr.height() + 4);

            QRect global = QRect(
                viewport()->mapToGlobal(cr.topLeft()), viewport()->mapToGlobal(cr.bottomRight()));

            QToolTip::showText(mapToGlobal( he->pos() ), t, this, global);
        }

        return true;
    }

    return QTableView::event(event);
}

void RuleSetView::contextMenuRequested ( const QPoint &p )
{
    if (fwbdebug)
        qDebug("RuleSetView::contextMenuRequested at %d x %d",p.x(),p.y());
    contextMenu(rowAt(p.y()), columnAt(p.x()), viewport()->mapToGlobal(p));
}

void RuleSetView::currentChanged( const QModelIndex &current )
{
    if (fwbdebug)
        qDebug("RuleSetView::currentChanged to row %d, col %d",current.row(),current.column());
    changeCurrentCell(current.row(), current.column());
}

void RuleSetView::updateCell( const int row, const int col )
{
    if ((row < 0) || (col < 0))
        return;

    /*QRect r = ruleDelegate->cellGeometry(row,col);
    setDirtyRegion( QRegion( r.left() - horizontalOffset(), r.top() - verticalOffset(),
                              r.right() - horizontalOffset(), r.bottom() - verticalOffset() ) );
    update();*/

    QModelIndex ind = ruleModel->index(row,col);
    setCurrentCell(row, col);
    dataChanged(ind, ind);
    QVector <ProjectPanel*> pps = getAllMdiProjectPanel();
    for (int i = 0 ; i < pps.size();i++)
    {
        ProjectPanel*pp= pps[i];
        if (pp!=m_project)
        {
            if (pp->getCurrentRuleSetView()!=NULL)
                pp->getCurrentRuleSetView()->updateAll();
        }
    }
}

void RuleSetView::setName(QString)
{
    //do nothing
}

int RuleSetView::currentRow() const
{
    return m_currentRow;
}

int RuleSetView::currentColumn() const
{
    return m_currentColumn;
}

void RuleSetView::setCurrentRow(const int value)
{
    m_currentRow = value;
}

void RuleSetView::setCurrentColumn(const int value)
{
    m_currentColumn = value;
}

void RuleSetView::setCurrentCell(const int row, const int col)
{
    setCurrentRow(row);
    setCurrentColumn(col);
}

void RuleSetView::changeCurrentCell(const int row, const int col, bool fullrefresh)
{
    QModelIndex ind = ruleModel->index(currentRow(),currentColumn());
    setCurrentCell(row, col);
    dataChanged(ind, ind);

    ind = ruleModel->index(currentRow(),currentColumn());
    selectRow(row);
    selectionModel()->setCurrentIndex(ind, QItemSelectionModel::NoUpdate);
    dataChanged(ind, ind);

    if (fullrefresh)
        setCurrentIndex(ruleModel->index(row,col));
}

void RuleSetView::unselect()
{
    clearSelection();
    selectedObject=NULL;

    updateCell(currentRow(),currentColumn());
}

Firewall* RuleSetView::getFirewall()
{
    FWObject *f=ruleset;
    while (f!=NULL && !Firewall::isA(f)) f=f->getParent();
    assert(f!=NULL);
    return Firewall::cast(f);
}

void RuleSetView::hideEvent(QHideEvent *)
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

    m_project->unselectRules();
}

QString RuleSetView::settingsKey()
{
    return QString("/RuleSets/") + objectName() + "_Columns";
}

void RuleSetView::setRuleNumber(int row, libfwbuilder::Rule *rule)
{
    QIcon icn;

    if (rule!=NULL && rule->isDisabled())
        rulesDisabled[row] = true;
    else
        rulesDisabled[row] = false;

    verticalHeader()->headerDataChanged(Qt::Vertical, row, row);
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

RuleRowInfo* RuleSetView::getRuleRowInfoByGroupName (QString name)
{
    if (name=="")
        name="New Group";
    for (int i = 0 ; i < rowsInfo.size(); i++)
    {
        if (rowsInfo[i]->groupName==name)
            return rowsInfo[i];
    }
    return NULL;
}

int RuleSetView::getRuleRowInfoIndexByGroupName (QString name)
{
    for (int i = 0 ; i < rowsInfo.size(); i++)
    {
        if (rowsInfo[i]->groupName==name)
            return i;
    }
    return -1;

}

void RuleSetView::init()
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

    //horizontalHeader()->adjustSize();

    int          row=0;
    map<int,int> colW;
    bool         userColWidth=false;
    rowsInfo.clear();
    QFontMetrics p(st->getRulesFont());

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
            QString lbl = ruleModel->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();//horzHeaderLabels->stringList()[col];


            QRect br=p.boundingRect(QRect(0,0,1000,1000),
                                    Qt::AlignLeft|Qt::AlignVCenter,
                                    lbl );
            if (col==0)
            {
                colW[0]=20;
                continue ;
            }
            colW[col]=br.width() + 10;
        }
    }
   row=0;
    QString memberRow ;

    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++,row++)
    {
        Rule * r = Rule::cast( *i );
       
        ruleIndex[row] = *i;
        rowsInfo.push_back (NULL);
        setRuleNumber(row, r);
        if (Rule::cast( *i )->getPosition()!=row)
        {
            fixRulePosition(r, NULL, row);
        }
//        adjustRow(row);

        int h=16;
        for (int col=0; col<ncols; col++)
        {
            QRect cr = calculateCellSize(row,col);
            h = QMAX(h, cr.height());
            if (!userColWidth)
                colW[col]=QMAX(colW[col],cr.width());
            
        }
        adjustRow_int(row,h);
    }

    for (int col=0; col<ncols; col++)
    {
        if (col==0)
        {
            horizontalHeader()->setResizeMode (0, QHeaderView::Fixed);
            horizontalHeader()->resizeSection(0, 20);
            colW[0]=20;
            continue ;
        }
        setColumnWidth(col,colW[col]);
        horizontalHeader()->resizeSection(col, colW[col]);
    }

    //updateContents();
    updateGroups();
    update();

    QApplication::restoreOverrideCursor();
}

void RuleSetView::addRuleGroupPanel (int row)
{
    RuleRowInfo * rri = rowsInfo[row];
    if (rri==NULL)
        return ;
    setSpan (row,1,0,ruleModel->columnCount(this->model()->index(row,0))-1);
 //   rri->index = &model()->index(row,0);
    if (rri->isBeginRow)
    {
        bool hide = isRowHidden(row+1);
        RuleGroupPanel * grouppanel = new RuleGroupPanel(NULL,this,row); 
        grouppanel->ruleGroupName->setText(rri->groupName);
        rri->isHide=hide;
        if (!hide)
        {
            grouppanel->showHideRuleGroupButton->setCheckState(Qt::Unchecked);
            grouppanel->showHideRuleGroupButton->setText ("Collapse Group");
        }
        else
        {
            grouppanel->showHideRuleGroupButton->setCheckState(Qt::Checked);
            grouppanel->showHideRuleGroupButton->setText ("Expand Group");
        }
        setIndexWidget (model()->index(row,1),grouppanel);
    }
    else
    {
        this->hideRow(row);
        //setIndexWidget (model()->index(row,1),new QFrame());
    }
}

void RuleSetView::refreshGroups ()
{
}

void RuleSetView::updateGroups ()
{
 //   for (int col=1; col<ncols; col++)
 //  {
 //   }

    for (int i = 0 ; i < rowsInfo.size(); i++)
    {
        setSpan (i,1,0,1);
    }

    reset ();
    setColumnWidth(0,20);
    horizontalHeader()->setResizeMode (0, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(0, 20);
    QMap <QString, QString> groupColors ;
    for (int i = 0 ; i < rowsInfo.size(); i++)
    {
        setSpan (i,1,0,1);
        if (ruleIndex[i]==NULL)
        {        
            if (rowsInfo[i]->isBeginRow)
            {
                groupColors[rowsInfo[i]->groupName]=rowsInfo[i]->color ;
                if (groupColors[rowsInfo[i]->groupName]=="")
                {
                    Rule * r = Rule::cast(ruleIndex[i+1]);
                    QString color = r->getOptionsObject()->getStr("color").c_str();
                    if (color!="")
                    {
                        if (color=="#FFFFFF") color = "";
                        groupColors[rowsInfo[i]->groupName]=color;
                    }
                }
            }
            removeRuleIndex(i);
            rowsInfo.remove (i);
            ruleModel->removeRows(i,i);
            i--;
        }
    }


    
        QString memberRow ;

    bool beginGroup = false ;
    QString group;
    for (int i = 0 ; i < rowsInfo.size(); i++)
    {
        Rule * r ;
        r = Rule::cast(ruleIndex[i]);
        group = r->getRuleGroupName().c_str();
        QString color = groupColors[group];
        if (color!="")
        {

                FWOptions *ropt = r->getOptionsObject();
                ropt->setStr("color",color.toLatin1().constData());
    
        }
        else
        {
                if (group!="")
                {
                    color = r->getOptionsObject()->getStr("color").c_str();
                    if (color!="")
                    {
                        if (color=="#FFFFFF") color="";
                        groupColors[group]=color;
                    }
                }
        }    
        if (group!=memberRow)
        {
            ruleModel->insertRow(i);
            if (!beginGroup)
            {
                insertRuleIndex(i);
                ruleIndex[i] = NULL ;
                beginGroup = true ;
                memberRow = group;
                RuleRowInfo * rri = new RuleRowInfo(memberRow,true,false);
                rri->color = groupColors[memberRow];
                rowsInfo.insert (i,rri);
                //QPanel * p = new QPanel(this);
                addRuleGroupPanel(i);

            }
            else
            {
                insertRuleIndex(i);
                ruleIndex[i] = NULL ;
                beginGroup = false ;
                //setSpan (row,0,0,8);
                //setIndexWidget (model()->index(row,0),new QFrame());
                rowsInfo.insert (i,new RuleRowInfo(memberRow,false,false));
                memberRow = "";
                addRuleGroupPanel(i);  
                i--;              
            }
            i++;
        }

    }

    if (group!="")
    {
        ruleModel->insertRow(rowsInfo.size());
        //insertRuleIndex(rowsInfo.size());
        ruleIndex[rowsInfo.size()] = NULL ;
        beginGroup = false ;
        rowsInfo.push_back (new RuleRowInfo(memberRow,false,false));
        memberRow = group;
        addRuleGroupPanel(rowsInfo.size()-1);
    }


}

void RuleSetView::deleteRuleGroupPanel (int row)
{
 //   RuleRowInfo * rri = rowsInfo[row];
    
    /*QWidget * w = indexWidget(*rri->index);
    if (w!=NULL)
    {
        delete w ;
    }*/
    setIndexWidget (model()->index(row,0),NULL);

    //setSpan (row,0,0,1);
    //rowSpan(row,8);
}

QSize RuleSetView::getPMSize()
{
    if (!st->getShowIconsInRules()){
        pixmap_h    = 0;
        pixmap_w    = 0;
        return QSize();
    }
    QString icn="icon-ref";
    if (FWBSettings::SIZE16X16 == st->getIconsInRulesSize())
        icn="icon-tree";
//    return QPixmap::fromMimeSource(
//        Resources::global_res->getObjResourceStr(obj, icn).c_str() );

    QString icn_file = QString(":/Icons/AddressTable/")+icn;

    if (fwbdebug)
        qDebug("RuleSetView::getPMSize()   icn=%s",icn.toAscii().constData());
    QPixmap pm;
    LoadPixmap(icn_file, pm);

    pixmap_h    = pm.height();
    pixmap_w    = pm.width();
    return pm.size();
}

void RuleSetView::iinit()
{
    ruleModel->setRowCount( ruleset->size() );

    QSize sz = getPMSize();

    QFontMetrics p(st->getRulesFont());
    QRect br = p.boundingRect(QRect(0, 0, 1000, 1000),
                              Qt::AlignLeft|Qt::AlignVCenter,"WMWM" );
    text_h   = br.height();
    item_h   = ( (pixmap_h>text_h)?pixmap_h:text_h ) + RuleElementSpacing;
    //qDebug("pixmap_h = %d text_h = %d item_h = %d", pixmap_h, text_h, item_h);

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

    update();

    return;
}

void RuleSetView::clear()
{


}


QRect RuleSetView::calculateCellSize( int row, int col )
{
    int h = 16;
    int re_size;
    getPMSize();
//    if (fwbdebug)
//        qDebug("RuleSetView::calculateCellSize: row=%d col=%d",
//               row,col);

    //QPainter p(this);
    QFontMetrics p(st->getRulesFont());
    QRect br = p.boundingRect(QRect(0, 0, 1000, 1000),
                              Qt::AlignLeft|Qt::AlignVCenter,"WMWM" );
    text_h   = br.height();

    item_h = ((pixmap_h>text_h) ? pixmap_h:text_h) + RuleElementSpacing;
    Rule *rule = Rule::cast( ruleIndex[row] );

    int hc=0;
    int wc=0;
    switch (getColType(col))
    {
        case Time:
        {
            RuleElement *re = getRE(rule,col);
            if (rule==NULL)
            {   
                break ;
            }
            if (re==NULL)
            {
                /* broken rule element, fix it */
                FWObject *nre=m_project->db()->create("When");
                assert(nre!=NULL);
                rule->add(nre);
                break ;
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
                if (o2!=NULL)
                {
                QString ot = objectText(re,o2);
                QRect br=p.boundingRect(QRect(0, 0, 1000, 1000),
                                        Qt::AlignLeft|Qt::AlignVCenter,
                                        ot);
                
                hc += item_h;
                int itmW = RuleElementSpacing/2 + pixmap_w +
                        RuleElementSpacing + br.width();
                wc  = QMAX(wc, itmW);

                }
            }
            
            break;
        }

        case Action:
        {
/* possible actions:
            "Accept", "Deny", "Reject", "Accounting", "Tag",
            "Pipe", "Classify", "Custom", "Continue"
*/
            PolicyRule *ruleP = PolicyRule::cast( rule );
            if (ruleP==NULL)
                break ;

            string act = ruleP->getActionAsString();
            QString res="";
            res = FWObjectPropertiesFactory::getRuleActionProperties(ruleP);
            if (st->getShowDirectionText())
            {
                QString res2 = act.c_str();
                res2 += " " ;
                res =  res2 + res ;
            }

            QString ac = res;
                    //FWObjectPropertiesFactory::getRuleActionProperties(
                    //PolicyRule::cast(rule));
            QRect br=p.boundingRect(QRect(0, 0, 1000, 1000),
                                    Qt::AlignLeft|Qt::AlignVCenter, ac );
            hc = item_h;
            wc = RuleElementSpacing/2 + pixmap_w + RuleElementSpacing + br.width();
            break;
        }

        case Direction:
        {
            PolicyRule *ruleP = PolicyRule::cast( rule );
            if (ruleP==NULL)
                break ;
            QString ac = ruleP->getDirectionAsString().c_str();
                   // FWObjectPropertiesFactory::getRuleActionProperties(
                   // PolicyRule::cast(rule));
            QRect br=p.boundingRect(QRect(0, 0, 1000, 1000),
                                    Qt::AlignLeft|Qt::AlignVCenter, ac );

            hc = item_h;
            wc = RuleElementSpacing/2 + pixmap_w + RuleElementSpacing + br.width();
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
            if (rule==NULL)
            {
                hc = item_h;
                wc = RuleElementSpacing/2;
                break ;
            }
            QRect br=p.boundingRect(QRect(0,0,1000,1000),
                                    Qt::AlignLeft|Qt::AlignVCenter,
                                    QString::fromUtf8(rule->getComment().c_str()) );

            hc = st->getClipComment() ? item_h : (br.height() + RuleElementSpacing/2);
            wc = RuleElementSpacing/2 + br.width();
            break;
        }

        case Metric:
        {
            if (rule==NULL)
            {
                hc = item_h;
                wc = RuleElementSpacing/2;
                break ;
            }
            QRect br=p.boundingRect(QRect(0, 0, 1000, 1000),
                                    Qt::AlignLeft|Qt::AlignVCenter,
                                    QString::fromUtf8(RoutingRule::cast(rule)->getMetricAsString().c_str()) );
            hc = item_h; //br.height() + RuleElementSpacing;
            wc = RuleElementSpacing/2 + br.width();
            break;
        }

        default:
            break;
    }

    h = QMAX(h, hc);
    h = QMAX(h, pixmap_h+RuleElementSpacing);
    if (col==0)
    {
        QRect(0,0,10,h);
    }
    wc = QMAX(wc, QApplication::globalStrut().width());
    wc += RuleElementSpacing/2;  // some padding
    
    return QRect(0,0,wc,h);
}

RuleSetView::REType  RuleSetView::getColType(int col) const
{
    map<int,REType>::const_iterator i = colTypes.find(col);
    return i->second;
}


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
}

QPixmap RuleSetView::getPixmap(FWObject *obj, PixmapAttr pmattr) const
{
//    QPixmap pm;
    string icn = "icon";
    if (pmattr == Neg)  icn="icon-neg";
    if (pmattr == Ref)  icn="icon-ref";
    if (pmattr == Tree) icn="icon-tree";
    if (pmattr == NegTree) icn="icon-neg-tree";
//    return QPixmap::fromMimeSource(
//        Resources::global_res->getObjResourceStr(obj, icn).c_str() );

    QString icn_file = (":/Icons/"+obj->getTypeName()+"/"+icn).c_str();// = Resources::global_res->getObjResourceStr(obj, icn).c_str();
    QPixmap pm;
    LoadPixmap(icn_file, pm);
    //qDebug("here %s", icn_file.toAscii().constData());

    return pm;
}

void RuleSetView::repaintSelection()
{
    setDirtyRegion( QRegion( 0, 0, width(), height() ) );
}

void RuleSetView::paintCell(QPainter *pntr,
                            int row,
                            int col,
                            const QRect &cr,
                            bool,
                            const QPalette &cg)
{
    int re_size;

/* row may point at an empty row where there is no rule yet. This
    * happens if this method is called to redraw the table when we call
    * setNumRows
 */

    /*if (fwbdebug)
        qDebug("Draw cell: row=%d col=%d  current palette=%d",
               row,col,palette().serialNumber());*/

    if (ruleIndex.count(row)==0) return;

    QString     rclr;
    Rule *rule = Rule::cast( ruleIndex[row] );
    int groupEnd = getDownNullRuleIndex(row); 
    
    if (col==0)
    {
//        qDebug("1");
//        pntr->drawLine (cr.left()+10,cr.top()+20,cr.left()+30,cr.top());

        QPixmap bufferpixmap;
        QString bpmname = QString("rulesetcell_%1_%2").arg(cr.width()).arg(cr.height());
        if ( ! QPixmapCache::find( bpmname, bufferpixmap) )
        {
            bufferpixmap = QPixmap( cr.width() , cr.height() );
            QPixmapCache::insert( bpmname, bufferpixmap);
        }
    
        bufferpixmap.fill( cg.base().color() );
    
        QPainter p( &bufferpixmap );
        QFont font = st->getRulesFont();
        p.setFont(font);

        if (rule==NULL)
        {
                RuleRowInfo * rri = rowsInfo[row];
                    
                    if (rri->isBeginRow)
                    {
                        p.drawRect((cr.width()-1)/2-4,(cr.height()-1)/2-4,8,8);
                        p.drawLine( (cr.width()-1)/2+7, (cr.height()-1)/2, cr.width()-1, (cr.height()-1)/2 );    
                        if (!isRowHidden(row+1))
                        {
                                p.drawLine( (cr.width()-1)/2, (cr.height()-1)/2+7, (cr.width()-1)/2, cr.height()-1 );
                                p.drawLine( (cr.width()-1)/2-2, (cr.height()-1)/2,(cr.width()-1)/2+2, (cr.height()-1)/2);
                        }
                        else
                        {
                                p.drawLine( (cr.width()-1)/2-2, (cr.height()-1)/2,(cr.width()-1)/2+2, (cr.height()-1)/2);
                                p.drawLine( (cr.width()-1)/2, (cr.height()-1)/2-2,(cr.width()-1)/2, (cr.height()-1)/2+2);
                        }    

                    }
                
        }
        else
        {
                QString group = rule->getRuleGroupName ().c_str();
                if (group != "")
                {
                    if (groupEnd!=-1&&groupEnd==row+1)
                    {
                        p.drawLine( (cr.width()-1)/2, (cr.height()-1)/2, cr.width()-1, (cr.height()-1)/2 );
                        p.drawLine( (cr.width()-1)/2, (cr.height()-1)/2, (cr.width()-1)/2, 0 );
                    }
                    else
                    {
                    p.drawLine( (cr.width()-1)/2, 0, (cr.width()-1)/2, cr.height()-1 );
                    }
                }
        }
    p.end();

    pntr->drawPixmap( cr.left() - horizontalOffset(), cr.top() - verticalOffset(), bufferpixmap );

        return ;
    }
    if (rule==NULL) 
    {

        RuleRowInfo * rri = rowsInfo[row];

        QPixmap bufferpixmap;
        QString bpmname = QString("rulesetcell_%1_%2").arg(cr.width()).arg(cr.height());
        if ( ! QPixmapCache::find( bpmname, bufferpixmap) )
        {
            bufferpixmap = QPixmap( cr.width() , cr.height() );
            QPixmapCache::insert( bpmname, bufferpixmap);
        }
    
        bufferpixmap.fill( cg.base().color() );
    
        QPainter p( &bufferpixmap );
        QFont font = st->getRulesFont();
        p.setFont(font);
        if (!rri->color.isEmpty())
        {
            QRect rect(0, 0, cr.width(), cr.height() );
            p.fillRect(rect, QColor(rri->color));
        }

        p.setPen(Qt::green);

        if (rri->isBeginRow)
        {
            p.drawLine( 1, 1, 1, cr.height() );
            p.drawLine( 1, 1, cr.width()-3, 1 );
            p.drawLine( cr.width()-3, 1, cr.width()-3, cr.height() );
        }

        if (isRowHidden(row+1))
        {
            p.drawLine( 1, cr.height()-3,  cr.width() , cr.height()-3);
                                                           
        }
        
        p.end();

        pntr->drawPixmap( cr.left() - horizontalOffset(), cr.top() - verticalOffset(), bufferpixmap );

                /*
                p.drawLine( (cr.width()-1)/2, (cr.height()-1)/2, cr.width()-1, (cr.height()-1)/2 );
                if (rri->isBeginRow)
                    p.drawLine( (cr.width()-1)/2, (cr.height()-1)/2, (cr.width()-1)/2, cr.height()-1 );
                else
                    p.drawLine( (cr.width()-1)/2, (cr.height()-1)/2, (cr.width()-1)/2, 0 );
                 */

        

    
        return;
    }

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
    bufferpixmap.fill( cg.base().color() );

    QPainter p( &bufferpixmap );
    QFont font = st->getRulesFont();
    p.setFont(font);



    QRect r = ruleDelegate->cellRect(row,col);

    static int lastrow = 0;
    if (lastrow != row)
    {
        lastrow = row;
        /*if (fwbdebug)
            qDebug("RuleSetView::paintCell real row %d height is %d", row, r.height());*/
    }

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

    /*const BackgroundMode bgmode = backgroundMode();
    const QColorGroup::ColorRole crole = QPalette::backgroundRoleFromMode( bgmode );*/

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
            if (o1==NULL)
                continue ;
            QRect rect(0, y, cr.width()-1, item_h-1 );
            if (sel && o1==selectedObject)
            {
                if (hasFocus())
                {
                    p.fillRect(rect, cg.brush( QPalette::Highlight ));
                    p.setPen( cg.highlightedText().color() );
                } else
                {
                    p.fillRect(rect, cg.brush( QPalette::Base ));//cg.brush( crole ));
                    p.setPen( cg.text().color() );
                }
            } else
            {
                p.setPen( cg.text().color() );
            }
            x = r.left()+1;

            QSize sz = drawIconInRule(p, x, y, re, o1);

            x += sz.width()+1;


            p.drawText( x, y + RuleElementSpacing/2,
                        cr.width()-sz.width()-1, item_h,
                        Qt::AlignLeft|Qt::AlignVCenter, objectText(re,o1) );

            FWObject *mwSelObj = selectedObject;
            std::vector<libfwbuilder::FWObject*> om_selected_objects =
                    m_project->getCurrentObjectTree()->getSelectedObjects();

            if (mwSelObj==NULL && om_selected_objects.size()!=0)
                mwSelObj = om_selected_objects.front();

            if (    (!sel) &&
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
        QRect rect(0, 0, cr.width()-1, cr.height()-1 );
        if (sel)
        {
            if (hasFocus())
            {
                p.fillRect(rect, cg.brush( QPalette::Highlight ));
                p.setPen( cg.highlightedText().color() );
            } else
            {
                p.fillRect(rect, cg.brush( QPalette::Base )); // crole
                p.setPen( cg.text().color() );
            }
        } else
        {
            p.setPen( cg.text().color() );
        }

        switch (getColType(col))
        {
            case Action:
            {
                PolicyRule *rule = PolicyRule::cast( ruleIndex[row] );
                if (rule==NULL) return;

                QString platform=getPlatform();
                string act = rule->getActionAsString();
                QString icn = chooseIcon((":/Icons/" + act).c_str()); //for example :/Icons/Continue
                QString res="";
            //FWOptions *ropt = rule->getOptionsObject();
                res = FWObjectPropertiesFactory::getRuleActionProperties(rule);
                QPixmap pm;
                LoadPixmap(icn, pm);
                int height = pm.height(); 
                p.drawPixmap( x,y + RuleElementSpacing/2, pm );
                x += pm.width()+1;
                if (st->getShowDirectionText())
                {
                    QString res2 = act.c_str();
                    res2 += " " ;
                    res =  res2 + res ;
                }
                QRect br=p.boundingRect(QRect(x, y, 1000, 1000),
                                        Qt::AlignLeft|Qt::AlignVCenter,
                                        res.toAscii().constData() );
                if (br.height()>height)
                    height=br.height();
                p.drawText( x, y+ RuleElementSpacing/2, br.width(), height,
                            Qt::AlignLeft|Qt::AlignVCenter, res.toAscii().constData() );
                break;
            }
            case Direction:
            {
                PolicyRule *rule = PolicyRule::cast( ruleIndex[row] );
                if (rule==NULL) return;

                string dir = rule->getDirectionAsString();
		        QString dir_ = rule->getDirectionAsString().c_str();
                if (dir.empty()) 
			        dir = "Both";
		//dir="Both-tree";
		         
                QString icn = chooseIcon((":/Icons/" + dir).c_str());
                QString res ="";
                
                QPixmap pm;
                LoadPixmap(icn, pm);
                int height = pm.height(); 
                if (st->getShowDirectionText())
                {
                    res = dir.c_str() ;
                }

                p.drawPixmap( x,y + RuleElementSpacing/2, pm );
                x += pm.width()+1;

                QRect br=p.boundingRect(QRect(x, y, 1000, 1000),
                                        Qt::AlignLeft|Qt::AlignVCenter,
                                        res.toAscii().constData() );
                if (br.height()>height)
                    height=br.height();
                p.drawText( x, y+ RuleElementSpacing/2, br.width(), height,
                            Qt::AlignLeft|Qt::AlignVCenter, res.toAscii().constData() );

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
                    QString icn = chooseIcon(":/Icons/Log");//Resources::global_res->getResourceStr("/FWBuilderResources/UI/Icons/Log" ).c_str();
                    //assert(icn!="");

                    QPixmap pm;
                    LoadPixmap(icn, pm);

                    p.drawPixmap( x,y + RuleElementSpacing/2, pm );
                    x += pm.width()+2;
                }
                if (
                    (policyRule  && ! isDefaultPolicyRuleOptions( rule->getOptionsObject())) ||
                    (routingRule && ! isDefaultRoutingRuleOptions( rule->getOptionsObject())) ||
                    (natRule     && ! isDefaultNATRuleOptions( rule->getOptionsObject()))
                   )
                {
                    QString icn = chooseIcon(":/Icons/Options");
                    //QString icn = ":/Icons/Options";//There isn't icon 16x16 for options...

                    QPixmap pm;
                    LoadPixmap(icn, pm);

                    p.drawPixmap( x,y + RuleElementSpacing/2, pm );
                }
                break;
            }
            case Comment:
            {
                drawComment(p, row, col, cr);
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



    QString group = rule->getRuleGroupName ().c_str();
    if (group!= "")
    {
        if (groupEnd!=-1&&groupEnd==row+1)
        {
            p.setPen(Qt::green);
            if (col==1)
            {
                p.drawLine( 1, 0, 1, cr.height()-1 );
            }
            p.drawLine( 1, cr.height()-3,  cr.width()-3 , cr.height()-3);
            if (col==ncols-1)
            {
                p.drawLine( cr.width()-3, 0, cr.width()-3, cr.height()-3 );
    
            }            
    //p.drawLine( cr.width()-3, 0, cr.width()-3, cr.height()-1 );
        }
        else
        {
            p.setPen(Qt::green);
    
            if (col==1)
            {
                p.drawLine( 1, 0, 1, cr.height()-3 );
            }
            if (col==ncols-1)
            {
                p.drawLine( cr.width()-3, 0, cr.width()-3, cr.height()-3 );
    
            }

        } 
        

    }



    p.end();

    pntr->drawPixmap( cr.left() - horizontalOffset(), cr.top() - verticalOffset(), bufferpixmap );

    return;
}

void RuleSetView::drawComment(QPainter &p, int row, int col, const QRect &cr)
{
    /* comments are found in both policy and nat rules, so we cast to Rule here */
    Rule *rule = Rule::cast( ruleIndex[row] );
    if (rule==NULL) return;

    QRect r = ruleDelegate->cellRect(row,col);

    int x  = r.left() + RuleElementSpacing/2;
    int y  = r.top();
    QString comm = QString::fromUtf8(rule->getComment().c_str());
    QRect br=p.boundingRect(QRect(x, y, 1000, 1000),
                            Qt::AlignLeft|Qt::AlignVCenter,
                            comm);
    if (st->getClipComment() && text_h > 0)
    {
        QStringList strs = comm.split('\n');

        int h = cr.height()/text_h;
        if (h >= 1 && h<= strs.size())
        {
            std::list<QString> lst(strs.begin(), strs.begin()+h);
            comm = QStringList(QStringList::fromStdList(lst)).join("\n");
        }
    }
    p.drawText( x, y + RuleElementSpacing/2,
                    br.width(),
                    br.height(),
                    Qt::AlignLeft|Qt::AlignTop,
                    comm);
}

QString RuleSetView::chooseIcon(QString icn)
{
    if (!st->getShowIconsInRules())
        return QString();
    if (FWBSettings::SIZE16X16 == st->getIconsInRulesSize())
    {
        if (icn.contains("-ref"))
            return icn.replace("-ref", "-ref-tree");
        if (icn.contains("-neg"))
            return icn.replace("-neg", "-neg-tree");
/*	if (icn.contains("Both"))
	    return icn+="-tree";
	if (icn.contains("Inbound"))
	    return icn+="-tree";
	if (icn.contains("Outbound"))
	    return icn+="-tree";*/

        return icn+"-tree";
    }
    return icn;
}

QSize RuleSetView::drawIconInRule(QPainter &p, int x, int y, RuleElement *re, FWObject *o1)
{

    if (!st->getShowIconsInRules())
        return QSize();
    QPixmap pm;
    if (FWBSettings::SIZE16X16 == st->getIconsInRulesSize())
    {
        if (!re->getNeg())
        {
            pm = getPixmap(o1, Tree);
        }
        else
        {
            pm = getPixmap(o1, NegTree);
        }
    }
    if (FWBSettings::SIZE25X25 == st->getIconsInRulesSize())
    {
        if (!re->getNeg())
        {
            pm = getPixmap(o1, Normal);
        }
        else
        {
            pm = getPixmap(o1, Neg);
        }
    }

    
    if (!re->isAny())
        p.drawPixmap( x, y + RuleElementSpacing/2, pm );

    return pm.size();
}

QString RuleSetView::getPlatform()
{
    return  getFirewall()->getStr("platform").c_str();
}


libfwbuilder::PolicyRule* RuleSetView::getRule(int row)
{
    return PolicyRule::cast( ruleIndex[row] );
}

void RuleSetView::selectRE( int row, int col)
{
    m_project->selectRules();

    if (row!=currentRow() || col!=currentColumn())
    {
        selectedObject = NULL;
        updateCell(currentRow(),currentColumn());
    }
}

void RuleSetView::selectRE(libfwbuilder::FWReference *ref)
{
    m_project->selectRules();

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
        scrollTo( ruleModel->index(row,col), QAbstractItemView::EnsureVisible);
        updateCell(row,col);
        break;
    }
}

void RuleSetView::itemDoubleClicked(const QModelIndex & index)
{
    if (!index.isValid()) return;
    if (index.row()<0) return;
    editSelected();
}

void RuleSetView::selectionChanged(const QItemSelection &, const QItemSelection &)
{
    if (selectionModel()->selection().empty())
    {
        if (fwbdebug) qDebug("RuleSetView::selectionChanged We've just got an empty selection :(");

        firstSelectedRule = -1;
        lastSelectedRule = -1;

        //unselect();
        setCurrentIndex(QModelIndex());

        m_project->unselectRules();

        return;
    }

    itemSelectionRange = selectionModel()->selection()[0];

    firstSelectedRule=itemSelectionRange.top();
    lastSelectedRule=itemSelectionRange.bottom();

    if (fwbdebug) qDebug("RuleSetView::selectionChanged New selection rows: %d - %d", firstSelectedRule, lastSelectedRule);

    if (lastSelectedRule < firstSelectedRule)
    {
        int i = lastSelectedRule;
        lastSelectedRule = firstSelectedRule;
        firstSelectedRule = i;
    }

    int selectionSize=lastSelectedRule-firstSelectedRule+1;

    mw->m_mainWindow->editCopyAction->setEnabled(true);
    mw->m_mainWindow->editCutAction->setEnabled(true);
    mw->m_mainWindow->editPasteAction->setEnabled(true);
    mw->m_mainWindow->editDeleteAction->setEnabled(true);

    mw->m_mainWindow->copyRuleAction->setEnabled( selectionSize==1 );
    mw->m_mainWindow->cutRuleAction->setEnabled( selectionSize==1 );
    mw->m_mainWindow->pasteRuleAboveAction->setEnabled( selectionSize==1 );
    mw->m_mainWindow->pasteRuleBelowAction->setEnabled( selectionSize==1 );

    m_project->selectRules();
}

void RuleSetView::adjustColumn( int col )
{
    QString lbl = ruleModel->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();

    QFontMetrics p(st->getRulesFont());//(this);
    QRect br=p.boundingRect(QRect(0, 0, 1000, 1000),
                            Qt::AlignLeft|Qt::AlignVCenter,
                            lbl );

    int     w = br.width() + 10;

    int row=0;
    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++,row++)
    {
        QRect cr = calculateCellSize(row,col);
        w=QMAX(w,cr.width());
    }

    horizontalHeader()->resizeSection(col, w);
}

void RuleSetView::adjustRow_int( int row, int h )
{
/* make sure the row is no smaller than a label in the left header,
 * and no smaller than the "strut" (the minimal size of the gui
         * element as defined in QApplication)
 */
    QHeaderView * leftHeader = verticalHeader();

    h = QMAX(h, leftHeader->fontMetrics().height() + 2);
    h = QMAX(h, QApplication::globalStrut().height());
    if (h == leftHeader->sectionSize(row))
        //if new height is equal to old function vertSectionResized is
        //not called and row heights are not set
        setRowHeight(row, h);
    verticalHeader()->resizeSection(row, h);
}

void RuleSetView::adjustRow( int row )
{
    int h = 18;

    for (int col=0; col<ncols; col++)
    {
        QRect cr = calculateCellSize(row,col);

        h = QMAX(h, cr.height());
        int w = cr.width();

        if (columnWidth(col)<w)
            horizontalHeader()->resizeSection(col, w);

    }

    adjustRow_int(row,h);
}

Rule* RuleSetView::insertRule(int pos, FWObject *r)
{
    if (r!=NULL &&
        ruleset->getTypeName()==Policy::TYPENAME &&
        r->getTypeName()!=PolicyRule::TYPENAME)  return NULL;
    if (r!=NULL &&
        ruleset->getTypeName()==NAT::TYPENAME    &&
        r->getTypeName()!=NATRule::TYPENAME   )  return NULL;

    if (pos<0) pos=0;


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
                .arg(PolicyRule::cast(r)->getDirectionAsString().c_str()).toAscii().constData());

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

    Rule * oldr = Rule::cast(ruleIndex[pos]);
    for (int i=ruleIndex.size(); i>pos; --i)  
        ruleIndex[i]=ruleIndex[i-1];
    //RuleRowInfo * info = getRuleRowInfoByGroupName(newrule->getRuleGroupName().c_str());
    //if (info!=NULL)
    //{
    //    info->rules.push_back (newrule);
    //}
    if (oldr!=NULL)
    {
        newrule->setRuleGroupName (oldr->getRuleGroupName());
    }
    ruleIndex[pos] = newrule;

    if (fwbdebug && PolicyRule::cast(r)!=NULL)
        qDebug(QString("RuleSetView::insertRule: newrule->direction=%1")
                .arg(PolicyRule::cast(newrule)->getDirectionAsString().c_str()).toAscii().constData());


    for (int i=ruleIndex.size(); i>=pos; --i)
        setRuleNumber(i, Rule::cast(ruleIndex[i]));

    if (newrule_as_policy_rule!=NULL &&
        newrule_as_policy_rule->getAction()==PolicyRule::Branch )
        addRuleBranch( newrule_as_policy_rule );

    ruleModel->insertRow(pos);
    rowsInfo.insert(pos,NULL);
    adjustRow(pos);
    update();
    updateGroups();
    setCurrentCell( pos, currentColumn() );
    updateCell(pos,currentColumn());

    if (fwbdebug) qDebug("Firewall changed: insertRule");

    return newrule;
}

void RuleSetView::insertRule()
{
//    if (!hasFocus()) return;       // <-- can insert rule even if does not have focus
    if (!isTreeReadWrite(this,ruleset)) return;

    changingRules = true;
    if (firstSelectedRule > -1)
    {
        insertRule(firstSelectedRule,NULL);
        changeCurrentCell(firstSelectedRule+1,currentColumn(), true);
    }
    else
        insertRule(0,NULL);

    changingRules = false;
    mw->updateLastModifiedTimestampForOneFirewall(getFirewall());
}

void RuleSetView::insertRuleAboveFirstGroup()
{
    insertRule(0,NULL);
    updateGroups();
}

FWObject* RuleSetView::getSelectedObject()
{
    return selectedObject;
}

void RuleSetView::addRuleBranch(PolicyRule *rule)
{
    if (fwbdebug) qDebug("RuleSetView::addRuleBranch");

    FWOptions *ropt = rule->getOptionsObject();
    QString branchName = ropt->getStr("branch_name").c_str();
    if (branchName.isEmpty())
    {
        QString bn = QString("rule%1_branch").arg(rule->getPosition());
        ropt->setStr("branch_name",bn.toAscii().constData());
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
}

FWObject* RuleSetView::getObj(int row, int col, int mouse_y_pos, QRect *objr)
{
    RuleElement *re = getRE(row,col);
    if (re==NULL) return NULL;

    QRect cr=ruleDelegate->cellGeometry(row,col);

/*
    * n     is the number of objects in the cell
    * y_rel is a distance of the mouse cursor from the top of the cell
    * h     is the cell height
 */
    int   y_rel = mouse_y_pos-cr.top();
    int   y_obj = cr.top();
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

void RuleSetView::mouseMoveEvent( QMouseEvent* ev )
{
    if (startingDrag)
    {
        QDrag* drag = dragObject();

        if (drag)
            drag->start(Qt::CopyAction | Qt::MoveAction); //just start dragging

        startingDrag = false;

        return;
    }

    QTableView::mouseMoveEvent(ev);
}

void RuleSetView::mousePressEvent( QMouseEvent* ev )
{
    if (fwbdebug)
        qDebug("RuleSetView::contentsMousePressEvent");

    int row=rowAt(ev->y());
    int col=columnAt(ev->x());
    
    if (col==0)
    {
        if (row<rowsInfo.size()&&row>=0)
        {
            if (rowsInfo[row]!=NULL)
            {
                QWidget * wgt = indexWidget(model()->index(row,1));
                RuleGroupPanel * rfp = dynamic_cast<RuleGroupPanel *>(wgt);
                if (rfp!=NULL)
                {
                    showHideRuleGroup(rfp);
                }
            }
       }
    }


    FWObject *obj=getObj(row,col,ev->y()+verticalOffset());
    bool needUpdate= (row==currentRow() && col==currentColumn() && selectedObject!=obj);

    if (fwbdebug)
        qDebug("RuleSetView::contentsMousePressEvent  obj=%s  row=%d  col=%d  needUpdate=%d",
               (obj)?obj->getName().c_str():"NULL",row,col,needUpdate);

    if (fwbdebug)
        qDebug("RuleSetView::contentsMousePressEvent  1 currentRow=%d  currentColumn=%d",
               currentRow(),currentColumn());

    QTableView::mousePressEvent(ev);

    if ((row > 0) && (col > 0))
        mw->selectRules();
    else
    {
        mw->unselectRules();
        setCurrentIndex(QModelIndex());
        //unselect();
    }

    if (fwbdebug)
        qDebug("RuleSetView::contentsMousePressEvent  2 currentRow=%d  currentColumn=%d",
               currentRow(),currentColumn());

    startingDrag = (row==currentRow() && col==currentColumn() && selectedObject==obj);

    changeCurrentCell(row, col, true); //forget old selection by setting View's current cell
    setSelectedObject(obj);

    //verticalHeader()->update();
}

void RuleSetView::mouseReleaseEvent( QMouseEvent* ev )
{
    QTableView::mouseReleaseEvent(ev);

    if (fwbdebug)
        qDebug("RuleSetView::contentsMouseReleaseEvent");

    if (mw->isEditorVisible() && !switchObjectInEditor( columnAt(ev->x()) ))
    {
        ev->accept();
    };
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
}

void RuleSetView::insertRuleIndex (int idx)
{
   for ( int i=ruleIndex.size(); i>idx; --i) 
    { 
        ruleIndex[i]=ruleIndex[i-1];
    }
 
}
void RuleSetView::removeRuleIndex (int idx)
{
    
   for ( int i=idx; i<ruleIndex.size()-1; ++i) 
    { 
        ruleIndex[i]=ruleIndex[i+1];
    }
 //   ruleIndex.remove (ruleIndex.size());
}

int RuleSetView::getUpNullRuleIndex (int idx)
{
    for (int i=idx; i>=0; --i) 
    { 
        if (rowsInfo[i]!=NULL)
        {
            return i;
            
        }
    }
    return -1 ;

}

int RuleSetView::getDownNullRuleIndex (int idx)
{

    for ( int i=idx; i<rowsInfo.size(); ++i) 
    { 
        if (rowsInfo[i]!=NULL)
        {
            return i;
            
        }
    }
    return -1;
}


void RuleSetView::openObjectInTree(FWObject *obj)
{
    if (fwbdebug)
        qDebug("RuleSetView::openObjectInTree happens");

    if (gui_experiment1) return;

    FWObject *oo = obj;
    if (obj==NULL || Rule::cast(obj)!=NULL)
        oo = getFirewall();
    selectedObject=oo;
    mw->info(oo);

    setUpdatesEnabled(false);
    //bool f = hasFocus();
    mw->setManipulatorFocus();
    mw->openObject(oo);
    mw->clearManipulatorFocus();
    setFocus();
    setUpdatesEnabled(true);
    update();
}

void RuleSetView::newGroup()
{
    bool ok = false ;
    QString text = QInputDialog::getText(this, tr(""),
                                          tr("Enter group name:"), QLineEdit::Normal,
                                          "New Group", &ok);
     if (ok && !text.isEmpty())
    {
        QString postfix = "";
        int counter = 0 ;
        for (int i =0 ; i < rowsInfo.size(); i++)
        {
            RuleRowInfo * rri = rowsInfo[i];
            if (rri!=NULL)
            {
                if (text+postfix==rri->groupName)
                {
                    counter++;
                    postfix = "-"+QString().setNum(counter);
                }
            }
        }
        int row = firstSelectedRule;
        int count = lastSelectedRule - firstSelectedRule +1;
        createGroup(row,count,text+postfix);   
    } 
}

void RuleSetView::renameGroup()
{
    bool ok = false ;
    QString oldGroup = "";
    if (rowsInfo[ firstSelectedRule ] != NULL)
    {
        oldGroup = rowsInfo[ firstSelectedRule  ]->groupName;
    }
    
    QString text = QInputDialog::getText(this, "Rename group",
                                          tr("Enter group name:"), QLineEdit::Normal,
                                          oldGroup, &ok);
     if (ok && !text.isEmpty())
    {
        if (oldGroup==""||text=="")
            return ;
        QString postfix = "";
        for (int i =0 ; i < rowsInfo.size(); i++)
        {
            Rule * r = Rule::cast(ruleIndex[i]);
            if (r!=NULL)
            {
                if (r->getRuleGroupName ().c_str() ==  oldGroup)
                {
                    r->setRuleGroupName (text.toAscii().data());
                }
            }
        }
        updateGroups();
   
    } 
}


void RuleSetView::addToGroupAbove ()
{
    int row = firstSelectedRule;
    int count = lastSelectedRule - firstSelectedRule +1;
    int top = getUpNullRuleIndex(row);
    RuleRowInfo * ru = rowsInfo[top];
    if (!ru->isBeginRow)
    {
        top = getUpNullRuleIndex(top-1);
        ru = rowsInfo[top];
    }
    for (int i = 0; i< count ; i++)
    {
        Rule * r = Rule::cast(ruleIndex[row+i]);
        r->setRuleGroupName (ru->groupName.toAscii().data());
        ruleIndex[row+i] =r ;
        if (ru->isHide)
            showHideRuleGroup(dynamic_cast<RuleGroupPanel*>(indexWidget(model()->index(top,1))));
    }
    updateGroups();
}

void RuleSetView::addToGroupBelow()
{
    int row = firstSelectedRule;
    int bottom = getDownNullRuleIndex(row);
    RuleRowInfo * ru = rowsInfo[bottom];
    int count = lastSelectedRule - firstSelectedRule +1;
    for (int i = 0; i< count ; i++)
    {
        Rule * r = Rule::cast(ruleIndex[row+i]);
        r->setRuleGroupName (ru->groupName.toAscii().data());
        ruleIndex[row+i] =r ;
        if (ru->isHide)
            showHideRuleGroup(dynamic_cast<RuleGroupPanel*>(indexWidget(model()->index(bottom,1))));
    }
    updateGroups();
}

void RuleSetView::showHideRuleGroup(RuleGroupPanel * rgp)
{
    if (rgp==NULL)
        return ;
    QString act = rgp->showHideRuleGroupButton->text();
    int row = rgp->row ;
    RuleRowInfo * rrf = rowsInfo[row];
    int bottom = getDownNullRuleIndex(row+1);
    int count = bottom-row-1;
    if (act=="Collapse Group")
    {
        for (int i = row+1; i< row+1+count;i++)
        {
            hideRow(i);
        }
        rrf->isHide=true;
        //rgp->showHideRuleGroupButton->setText("Expand Group");
    }

    if (act=="Expand Group")
    {
        for (int i = row+1; i< row+1+count;i++)
        {
            showRow(i);
            adjustRow(i);
            //setRowHeight(i,rowHeights[i]);
            dataChanged (model()->index(i,0),model()->index(i,8));
        }
        rrf->isHide=false;
        //rgp->showHideRuleGroupButton->setText("Collapse Group");
    }
 //   refreshGroups();
    updateGroups();   
}

void RuleSetView::removeFromGroup()
{
    int row = firstSelectedRule;
    int count = lastSelectedRule - firstSelectedRule+1;

    removeFromGroup(row,count);
}

void RuleSetView::createGroup (int row, int count, QString groupName)
{
    for (int i =0 ; i<count; i++)
        Rule::cast(ruleIndex[row+i])->setRuleGroupName (groupName.toAscii().data());
    updateGroups();
}

void RuleSetView::removeFromGroup (int row, int count)
{
    for (int i = row ; i < row+count ; i++)
    {
        Rule * r = Rule::cast(ruleIndex[i]);
        if (r!=NULL)
        {
            r->setRuleGroupName("");
            FWOptions *ropt = r->getOptionsObject();
            ropt->setStr("color","");
    
            ruleIndex[i]=r;
        }
    }
    updateGroups();
}


void RuleSetView::contextMenu(int row, int col, const QPoint &pos)
{
    if (fwbdebug)
        qDebug("RuleSetView::contextMenu() at row=%d, col=%d", row, col);

    if (fwbdebug)
        qDebug("RuleSetView::contextMenu() selected rows: %d - %d", firstSelectedRule, lastSelectedRule);

    if (col > 0)
        setCurrentCell(row,col);
    else
        setCurrentCell(row,0);

    //if the row is not selected actually, we select it
    if ((row < firstSelectedRule) || (row > lastSelectedRule))
    {
        selectRow(row);
        firstSelectedRule = row;
        lastSelectedRule = row;
    }

    if (row<0 && ruleset->size()==0)
    {
        QMenu *popup=new QMenu(this);

        popup->addAction( tr("Insert Rule"), this, SLOT( insertRule() ));
        popup->addAction( tr("Paste Rule"), this, SLOT( pasteRuleAbove() ));

        popup->exec( pos );
        delete popup;
        return;
    }

    if (row<0 && ruleset->size()!=0)
    {
        /* this is when user clicks under the last rule */

        setCurrentCell(ruleset->size()-1,0);

        QMenu *popup=new QMenu(this);
        popup->addAction( tr("Paste Rule"), this, SLOT( pasteRuleBelow() ));

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

    QMenu *popup=new QMenu(this);
    Rule *r = Rule::cast(ruleIndex[row]);
    if (r!=NULL)
    {
        if (r->getRuleGroupName()=="")
        {
            //qDebug ((QString("").setNum(firstSelectedRule)).toAscii().data());
            //qDebug ((QString("").setNum(lastSelectedRule)).toAscii().data());

            popup->addAction( tr("New group"), this, SLOT( newGroup() ));
            int top = getUpNullRuleIndex(firstSelectedRule);
            if (top!=-1&&top==firstSelectedRule-1)
            {
                RuleRowInfo * ri= rowsInfo[top];
                QString label = tr("Add to the group ");
                label += ri->groupName;
                 popup->addAction( label, this, SLOT( addToGroupAbove() ));
            }
            int bottom = getDownNullRuleIndex(lastSelectedRule);
            if (bottom!=-1&&bottom==lastSelectedRule+1)
            {
                RuleRowInfo * ri= rowsInfo[bottom];
                QString label = tr("Add to the group ");
                label += ri->groupName;
                popup->addAction( label, this, SLOT( addToGroupBelow() ));
          
            }
        }
        else
        {
            popup->addAction( tr("Remove from the group"), this, SLOT( removeFromGroup() ));
        }
        popup->addSeparator ();
    }
    else
    {
            if (row==0)
                popup->addAction( tr("Insert Rule Above"), this, SLOT( insertRuleAboveFirstGroup() ));

            popup->addAction( tr("Rename group"), this, SLOT( renameGroup() ));
            popup->addSeparator();

            QMenu *subcolor = popup->addMenu( tr("Change color") );

            QPixmap pcolor(16,16);
            pcolor.fill(QColor(255,255,255));
            subcolor->addAction( QIcon(pcolor), tr("No color"), this, SLOT ( setColorEmpty() ));

            pcolor.fill(st->getLabelColor(FWBSettings::RED));
            subcolor->addAction( QIcon(pcolor), tr("Red"), this, SLOT ( setColorRed() ));

            pcolor.fill(st->getLabelColor(FWBSettings::ORANGE));
            subcolor->addAction( QIcon(pcolor), tr("Orange"), this, SLOT ( setColorOrange() ));

            pcolor.fill(st->getLabelColor(FWBSettings::YELLOW));
            subcolor->addAction( QIcon(pcolor), tr("Yellow"), this, SLOT ( setColorYellow() ));

            pcolor.fill(st->getLabelColor(FWBSettings::GREEN));
            subcolor->addAction( QIcon(pcolor), tr("Green"), this, SLOT ( setColorGreen() ));

            pcolor.fill(st->getLabelColor(FWBSettings::BLUE));
            subcolor->addAction( QIcon(pcolor), tr("Blue"), this, SLOT ( setColorBlue() ));

            pcolor.fill(st->getLabelColor(FWBSettings::PURPLE));
            subcolor->addAction( QIcon(pcolor), tr("Purple"), this, SLOT ( setColorPurple() ));

            pcolor.fill(st->getLabelColor(FWBSettings::GRAY));
            subcolor->addAction( QIcon(pcolor), tr("Gray"), this, SLOT ( setColorGray() ));

            popup->exec( pos );

            delete popup;
            return ;

    }
        


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
                popup->addAction( QIcon(LoadPixmap(":/Icons/Accept")),
                                  action_name, this, SLOT( changeActionToAccept() ));
            }
            if (Resources::isTargetActionSupported(platform,"Deny"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Deny,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Deny")),
                                  action_name, this, SLOT( changeActionToDeny() ));
            }
            if (Resources::isTargetActionSupported(platform,"Reject"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Reject,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Reject")),
                                  action_name, this, SLOT( changeActionToReject() ));
            }
            if (Resources::isTargetActionSupported(platform,"Accounting"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Accounting,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Accounting")),
                                  action_name, this, SLOT( changeActionToAccounting() ));
            }
            if (Resources::isTargetActionSupported(platform,"Pipe"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Pipe,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Pipe")),
                                  action_name, this, SLOT( changeActionToPipe() ));
            }
            if (Resources::isTargetActionSupported(platform,"Tag"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Tag,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Tag")),
                                  action_name, this, SLOT( changeActionToTag() ));
            }
            if (Resources::isTargetActionSupported(platform,"Classify"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Classify,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Classify")),
                                  action_name, this, SLOT( changeActionToClassify() ));
            }
            if (Resources::isTargetActionSupported(platform,"Custom"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Custom,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Custom")),
                                  action_name, this, SLOT( changeActionToCustom() ));
            }
            if (Resources::isTargetActionSupported(platform,"Branch"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Branch,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Branch")),
                                  action_name, this, SLOT( changeActionToBranch() ));
            }
            if (Resources::isTargetActionSupported(platform,"Route"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Route,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Route")),
                                  action_name, this, SLOT( changeActionToRoute() ));
            }
            if (Resources::isTargetActionSupported(platform,"Continue"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Continue,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Continue")),
                                  action_name, this, SLOT( changeActionToContinue() ));
            }

            popup->addSeparator ();
            QAction *paramID;
            paramID = popup->addAction( tr("Parameters"), this, SLOT( editSelected() ));

            PolicyRule *rule = PolicyRule::cast( ruleIndex[row] );
            if (rule!=NULL)
            {
                string act = rule->getActionAsString();
                if (Resources::getActionEditor(platform,act)=="None")
                    paramID->setEnabled(false);
            }

            break;
        }
        case Direction:
        {
            popup->addAction( QIcon(LoadPixmap(":/Icons/Inbound")),
                              tr("Inbound"), this, SLOT( changeDirectionToIn() ));
            popup->addAction( QIcon(LoadPixmap(":/Icons/Outbound")),
                              tr("Outbound"), this, SLOT( changeDirectionToOut() ));
            popup->addAction( QIcon(LoadPixmap(":/Icons/Both")),
                              tr("Both"), this, SLOT( changeDirectionToBoth() ));

            break;
        }
        case Options:
        {
            popup->addAction( QIcon(LoadPixmap(":/Icons/Options")),
                              tr("Rule Options"), this, SLOT( editSelected() ));

            if (fwbdebug) qDebug(ruleset->getTypeName().c_str());
            if (ruleset->getTypeName() == Policy::TYPENAME) {

                popup->addAction( QIcon(LoadPixmap(":/Icons/Log")),
                                  tr("Logging On"), this, SLOT( changeLogToOn() ));
                popup->addAction( QIcon(LoadPixmap(":/Icons/Blank")),
                                  tr("Logging Off"), this, SLOT( changeLogToOff() ));
            }
            break;
        }
        case Object:
        case Time:
        {
            RuleElement *re = getRE(row,col);
            if(re==NULL) return;

            QAction *editID = popup->addAction(
                    tr("Edit")   , this , SLOT( editSelected() ) );
            popup->addSeparator();
            QAction *copyID = popup->addAction(
                    tr("Copy")   , this , SLOT( copySelectedObject() ) );
            QAction *cutID  = popup->addAction(
                    tr("Cut")    , this , SLOT( cutSelectedObject() ) );
            popup->addAction( tr("Paste")  , this , SLOT( pasteObject() ) );
//        popup->addSeparator();
            QAction *delID  =popup->addAction(
                    tr("Delete") ,  this , SLOT( deleteSelectedObject() ) );
            popup->addSeparator();
            QAction *fndID = popup->addAction(
                    tr("Where used") , this , SLOT( findWhereUsedSlot()));
            QAction *revID = popup->addAction(
                    tr("Reveal in tree") ,this , SLOT( revealObjectInTree() ) );
            popup->addSeparator();
            QAction *negID  = popup->addAction(
                    tr("Negate") , this , SLOT( negateRE() ) );

            if (selectedObject == NULL || re->isAny())
                editID->setEnabled(false);
            copyID->setEnabled(!re->isAny());
            cutID->setEnabled(!re->isAny());
            delID->setEnabled(!re->isAny());

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
            negID->setEnabled(supports_neg &&  !re->isAny());
            fndID->setEnabled(!re->isAny());
            revID->setEnabled(!re->isAny());

            break;
        }

        case RuleOp:
        {
//        setCurrentCell(row,0);
            setFocus();

            Rule *rule = Rule::cast(ruleIndex[row]);
            if (rule==NULL)
            {
                //popup->addAction( tr("Insert Rule"), this, SLOT( insertRule() ) );
            } else
            {
                //int rn = rule->getPosition();
                int selectionSize=lastSelectedRule-firstSelectedRule+1;

                if (lastSelectedRule > firstSelectedRule)
                popup->addAction( tr("Rules %1-%2").
                    arg(firstSelectedRule).arg(lastSelectedRule) )->setEnabled(false);
                else
                popup->addAction( tr("Rule %1").
                    arg(firstSelectedRule) )->setEnabled(false);

                QMenu *subcolor = popup->addMenu( tr("Change color") );

                QPixmap pcolor(16,16);
                pcolor.fill(QColor(255,255,255));
                subcolor->addAction( QIcon(pcolor), tr("No color"), this, SLOT ( setColorEmpty() ));

                pcolor.fill(st->getLabelColor(FWBSettings::RED));
                subcolor->addAction( QIcon(pcolor), tr("Red"), this, SLOT ( setColorRed() ));

                pcolor.fill(st->getLabelColor(FWBSettings::ORANGE));
                subcolor->addAction( QIcon(pcolor), tr("Orange"), this, SLOT ( setColorOrange() ));

                pcolor.fill(st->getLabelColor(FWBSettings::YELLOW));
                subcolor->addAction( QIcon(pcolor), tr("Yellow"), this, SLOT ( setColorYellow() ));

                pcolor.fill(st->getLabelColor(FWBSettings::GREEN));
                subcolor->addAction( QIcon(pcolor), tr("Green"), this, SLOT ( setColorGreen() ));

                pcolor.fill(st->getLabelColor(FWBSettings::BLUE));
                subcolor->addAction( QIcon(pcolor), tr("Blue"), this, SLOT ( setColorBlue() ));

                pcolor.fill(st->getLabelColor(FWBSettings::PURPLE));
                subcolor->addAction( QIcon(pcolor), tr("Purple"), this, SLOT ( setColorPurple() ));

                pcolor.fill(st->getLabelColor(FWBSettings::GRAY));
                subcolor->addAction( QIcon(pcolor), tr("Gray"), this, SLOT ( setColorGray() ));

                popup->addSeparator();

                QString itemLbl;

                popup->addAction( tr("Insert Rule"), this,
                                  SLOT( insertRule() ) );
                popup->addAction( tr("Add Rule Below"), this,
                                  SLOT( addRuleAfterCurrent() ) );

                if (selectionSize==1) itemLbl=tr("Remove Rule");
                else                  itemLbl=tr("Remove Rules");
                popup->addAction( itemLbl, this,
                                  SLOT( removeRule()));
                if (selectionSize==1) itemLbl=tr("Move Rule");
                else                  itemLbl=tr("Move Rules");
                popup->addAction( itemLbl, this,
                                  SLOT( moveRule()));

                popup->addSeparator();

                popup->addAction( tr("Copy Rule"), this,
                                  SLOT( copyRule() ) );
                popup->addAction( tr("Cut Rule"), this,
                                  SLOT( cutRule() ) );
                popup->addAction( tr("Paste Rule Above"), this,
                                  SLOT( pasteRuleAbove() ) );
                popup->addAction( tr("Paste Rule Below"), this,
                                  SLOT( pasteRuleBelow() ) );

                popup->addSeparator();
                Rule *r = Rule::cast( ruleIndex[row] );
                if (r->isDisabled())
                {
                    if (selectionSize==1) itemLbl=tr("Enable Rule");
                    else                  itemLbl=tr("Enable Rules");
                    popup->addAction( itemLbl, this,
                                      SLOT( enableRule() ) );
                }else{
                    if (selectionSize==1) itemLbl=tr("Disable Rule");
                    else                  itemLbl=tr("Disable Rules");
                    popup->addAction( itemLbl, this,
                                      SLOT( disableRule() ) );
                }
            }
            break;
        }

        case Comment:
            popup->addAction( tr("Edit")   , this , SLOT( editSelected() ) );
            break;

        case Metric:
            popup->addAction( tr("Edit")   , this , SLOT( editSelected() ) );
            break;

        default:
            popup->addAction( tr("Edit")   , this , SLOT( editRE() ) );
            break;
    }



    popup->exec( pos );

    delete popup;
}

void RuleSetView::revealObjectInTree()
{
    if ( selectedObject!=NULL)
        mw->openObject(selectedObject);
}

void RuleSetView::findWhereUsedSlot()
{
    if ( selectedObject!=NULL)
        mw->findWhereUsed(selectedObject);
}

void RuleSetView::setColorEmpty()
{
    setRuleColor("");
}

void RuleSetView::setColorRed()
{
    setRuleColor(st->getLabelColor(FWBSettings::RED));
}

void RuleSetView::setColorBlue()
{
    setRuleColor(st->getLabelColor(FWBSettings::BLUE));
}

void RuleSetView::setColorOrange()
{
    setRuleColor(st->getLabelColor(FWBSettings::ORANGE));
}

void RuleSetView::setColorPurple()
{
    setRuleColor(st->getLabelColor(FWBSettings::PURPLE));
}

void RuleSetView::setColorGray()
{
    setRuleColor(st->getLabelColor(FWBSettings::GRAY));
}

void RuleSetView::setColorYellow()
{
    setRuleColor(st->getLabelColor(FWBSettings::YELLOW));
}

void RuleSetView::setColorGreen()
{
    setRuleColor(st->getLabelColor(FWBSettings::GREEN));
}

void RuleSetView::setRuleColor(const QString &c)
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (ruleIndex[firstSelectedRule]==NULL)
    {
        RuleRowInfo * rri = rowsInfo[firstSelectedRule];
        rri->color =c;
        if (rri->color=="")
            rri->color="#FFFFFF";
        updateGroups();
        return ;
    }

    if ( firstSelectedRule!=-1 )
    {
        for (int i=firstSelectedRule; i<=lastSelectedRule; ++i)
        {
            Rule *rule = Rule::cast( ruleIndex[i] );
            if (rule!=NULL)
            {
                FWOptions *ropt = rule->getOptionsObject();
                ropt->setStr("color",c.toLatin1().constData());
    
                adjustRow(i);   // this causes repaint
            }
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
                mw->updateLastModifiedTimestampForOneFirewall(getFirewall());
        }

        ruleopt->setBool("stateless", getStatelessFlagForAction(rule));

        mw->actionChangedEditor(rule);
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
                mw->updateLastModifiedTimestampForOneFirewall(getFirewall());
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

void RuleSetView::changeLogToOn()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        PolicyRule *rule = PolicyRule::cast( ruleIndex[currentRow()] );
        if (fwbdebug) qDebug("Firewall changed: changeLogToOn");
        if (!changingRules)
            mw->updateLastModifiedTimestampForOneFirewall(getFirewall());
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
            mw->updateLastModifiedTimestampForOneFirewall(getFirewall());
        rule->setLogging( false );
    }
}

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
        FWObjectClipboard::obj_clipboard->add( selectedObject, m_project );
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
        mw->updateLastModifiedTimestampForOneFirewall(getFirewall());

    re->removeRef(obj);

    if (re->isAny()) re->setNeg(false);

    if (fwbdebug)
    {
        qDebug("RuleSetView::deleteObject re->size()=%d", int(re->size()));
        qDebug("obj = %p",re->getRoot()->findInIndex(id));
        int rc = obj->ref()-1;  obj->unref();
        qDebug("obj->ref_counter=%d",rc);
    }

    adjustColumn(col);
    adjustRow(row);
    updateCell(row,col);
    m_project->findObjectWidget->reset();
}

bool RuleSetView::insertObjectFromOther(int row, int col, FWObject *obj)
{
    QString n=QString::fromUtf8(obj->getName().c_str());
    if (!m_project->hasObject(obj))
    {
        obj = m_project->copyObj2Tree(obj->getTypeName().c_str(), n, obj);
        if (!obj) return false;
    }
    return insertObject(row, col, obj);
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

    if (fwbdebug)
        qDebug("RuleSetView::insertObject  -- adjust and update table cell");

    adjustColumn(col);
    adjustRow(row);
    updateCell(row, col);

    if (fwbdebug) qDebug("Firewall changed: insertObject");
    if (!changingRules)
        m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    return true;
}

void RuleSetView::pasteObject()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    vector<string>::iterator i;
    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        FWObject *co= m_project->db()->findInIndex(*i);
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
            m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());

        updateCell(currentRow(),currentColumn());
    }
}

QVector <ProjectPanel*> RuleSetView::getAllMdiProjectPanel ()
{
    QVector <ProjectPanel*> ret ;
    QList<QMdiSubWindow *> subWindowList = mw->getMdiArea()->subWindowList();
    if (m_project->getRCS()==NULL)
        return ret ;
    ret.push_back (m_project);
    QString fileName = m_project->getRCS()->getFileName();
    for (int i = 0 ; i < subWindowList.size();i++)
    {
        ProjectPanel * pp = dynamic_cast <ProjectPanel *>(subWindowList[i]->widget());
        if (pp!=NULL)
        {
            if (pp==this->m_project)
                continue ;
            if (pp->getFileName () == fileName)
            {
                ret.push_back (pp);
            }
        }
    }
    return ret ;

}

void RuleSetView::editRE()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (fwbdebug)
        qDebug("RuleSetView::editRE  no special editor found for current cell");

    openPersistentEditor ( ruleModel->index(currentRow(),currentColumn()) );
}

void RuleSetView::keyPressEvent( QKeyEvent* ev )
{
    m_project->selectRules();

    RuleElement *re;

    int oldRow = currentRow();
    int oldColumn = currentColumn();

    if (ev->key()==Qt::Key_Left || ev->key()==Qt::Key_Right)
    {
        int shift= (ev->key()==Qt::Key_Left) ? -1 : 1;
        if ((currentColumn() + shift < 0) || (currentColumn() + shift >= ruleModel->columnCount()))
            return;

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
            if (m_project->isEditorVisible() && !switchObjectInEditor(currentColumn() + shift))
            {
                ev->accept();
            } else
                //QTableView::keyPressEvent(ev);
                changeCurrentCell(currentRow(), currentColumn()+shift);

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

        changeCurrentCell(currentRow(),currentColumn() + shift);

        if (m_project->isEditorVisible() && !switchObjectInEditor(currentColumn()))
        {
            ev->accept();
        } /*else
            QTableView::keyPressEvent(ev);*/
        return;
    }

    if (ev->key()==Qt::Key_Down || ev->key()==Qt::Key_Up ||
        ev->key()==Qt::Key_PageDown || ev->key()==Qt::Key_PageUp ||
        ev->key()==Qt::Key_End || ev->key()==Qt::Key_Home)
    {
        re = getRE(currentRow(),currentColumn());

        FWObject          *prev=NULL;
        FWObject          *o1  =NULL;
        FWObject::iterator i;

        int newrow = currentRow();
        if (ev->key()==Qt::Key_Up)
            newrow--;
        if (ev->key()==Qt::Key_Down)
            newrow++;

        if (ev->key()==Qt::Key_PageUp)
        {
            newrow = rowAt(rowViewportPosition(newrow) - viewport()->height());
            if (newrow == -1)
                newrow = 0;
        }

        if (ev->key()==Qt::Key_PageDown)
        {
            newrow = rowAt(rowViewportPosition(newrow) + viewport()->height())-1;
            if (newrow < 0)
                newrow = ruleModel->rowCount()-1;
        }

        if (ev->key()==Qt::Key_Home)
            newrow = 0;
        if (ev->key()==Qt::Key_End)
            newrow = ruleModel->rowCount()-1;

        if ((newrow < 0) || (newrow >= ruleModel->rowCount()))
            return;

        if (ev->key()==Qt::Key_PageDown || ev->key()==Qt::Key_PageUp ||
            ev->key()==Qt::Key_End || ev->key()==Qt::Key_Home)
        {
            re = getRE(newrow,currentColumn());
            if (re!=NULL)  // can be NULL if currentRow is 0
            {
                o1=re->front();
                if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
                setSelectedObject(o1);
                if (m_project->isEditorVisible() && !switchObjectInEditor(currentColumn()))
                {
                    ev->accept();
                    return;
                }
            }
            changeCurrentCell(newrow, currentColumn());
            return;
        }

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
            //QTableView::keyPressEvent(ev);  // to make new row current
            changeCurrentCell(newrow, currentColumn());
            if (m_project->isEditorVisible() && !switchObjectInEditor(currentColumn()))
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
                if (m_project->isEditorVisible() && !switchObjectInEditor(currentColumn()))
                {
                    ev->accept();
                    return;
                }
            }
            changeCurrentCell(newrow, currentColumn());
            //QTableView::keyPressEvent(ev);
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
                if (m_project->isEditorVisible() && !switchObjectInEditor(currentColumn()))
                {
                    ev->accept();
                    return;
                }
            }
            changeCurrentCell(newrow, currentColumn());
            //QTableView::keyPressEvent(ev);
            return;
        }
        /* switching to another object in the same cell */
        setSelectedObject((ev->key()==Qt::Key_Up) ? prev : o1);
        if (m_project->isEditorVisible() && !switchObjectInEditor(currentColumn()))
        {
            ev->accept();
            return;
        }

        //updateCell(currentRow(),currentColumn());

        ev->accept();
        return;
    }

    if (ev->key()==Qt::Key_Delete)
    {
        deleteSelectedObject();
    }

    updateCell(currentRow(),currentColumn());
    updateCell(oldRow,oldColumn);

    QTableView::keyPressEvent(ev);
}

QDrag* RuleSetView::dragObject()
{
    FWObject *obj = selectedObject;
    if (obj==NULL) return NULL;

    QString icn = (":/Icons/"+obj->getTypeName()+"/icon").c_str();

    list<FWObject*> dragobj;
    dragobj.push_back(obj);

    FWObjectDrag    *drag = new FWObjectDrag(dragobj, this, NULL);

    QPixmap pm = LoadPixmap(icn);

    drag->setPixmap( pm );

    return drag;
}

void RuleSetView::dragEnterEvent( QDragEnterEvent *ev)
{
    if (fwbdebug)
        qDebug("RuleSetView::dragEnterEvent");
    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
}

void RuleSetView::dragMoveEvent( QDragMoveEvent *ev)
{
    if (fwbdebug)
        qDebug("RuleSetView::dragMoveEvent");
    QWidget *fromWidget = ev->source();

    // The source of DnD object must be the same instance of fwbuilder
    if (fromWidget)
    {
        if (ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) && !ruleset->isReadOnly())
        {
            if (ev->keyboardModifiers() & Qt::ControlModifier)
                ev->setDropAction(Qt::CopyAction);
            else
                ev->setDropAction(Qt::MoveAction);

            int  row = rowAt( ev->pos().y() );
            int  col = columnAt( ev->pos().x() );

            if (col<0 || ( getColType(col)!=Object && getColType(col)!=Time) )
            {
                ev->setAccepted(false);
                return;
            }

            RuleElement *re = getRE(row,col);
            if (re==NULL)
            {
                ev->setAccepted(false);
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
                ev->setAccepted( acceptE );
                return;
            }
        }
    }

    ev->setAccepted(false);
}


void RuleSetView::dropEvent( QDropEvent *ev)
{
    if (fwbdebug)
        qDebug("RuleSetView::dropEvent");
    if (!isTreeReadWrite(this,ruleset)) return;

    int  row = rowAt( ev->pos().y() );
    int  col = columnAt( ev->pos().x() );

    int oldRow = currentRow(), oldCol = currentColumn();

    if (row<0 || col<0) return;

/* without this check the user can drag and drop an object inside the
    * same rule element. This is bad because it is considered a change,
    * even though nothing really changes. With this check, we can not
    * drag and drop an object from the tree into a selected cell...

    if (row==currentRow() && col==currentColumn()) return;
 */

    if (fwbdebug)
    {
        qDebug("RuleSetView::dropEvent  drop event mode=%d", ev->proposedAction());
        qDebug("                        src widget = %p", ev->source());
        qDebug("                              this = %p", this   );
    }

    list<FWObject*> dragol;
    if (!FWObjectDrag::decode(ev, dragol)) return;

    for (list<FWObject*>::iterator i=dragol.begin();
            i!=dragol.end(); ++i)
    {
        FWObject *dragobj = *i;
        assert(dragobj!=NULL);

        if (ev->source()!=this)
        {
            insertObjectFromOther(row, col, dragobj);
        } else
        {
            clearSelection();
            if (ev->keyboardModifiers() & Qt::ControlModifier)
            {
                if (insertObject(row,col,dragobj)) //copy
                    changeCurrentCell(row, col, true);
            }
            else //move
            {
                if (insertObject(row,col,dragobj) )
                {
                    deleteObject(oldRow,oldCol,dragobj);

                    changeCurrentCell(row, col, true);
                }
            }

        }
    }
    ev->accept();
}

void RuleSetView::removeRule()
{
    if (!hasFocus()) return;
    if (!isTreeReadWrite(this,ruleset)) return;
/* we call removeRule in a loop. Set flag changingRules to true to prevent
    * removeRule from calling updateLastModifiedTimestampForAllFirewalls each time
 */
    changingRules = true;

    m_project->findObjectWidget->reset();

    /* remove rules firstSelectedRule through lastSelectedRule */

    if ( firstSelectedRule!=-1 )
    {
        if (fwbdebug) qDebug("Firewall changed: removeRule");

        setUpdatesEnabled(false);
        for (int rn=lastSelectedRule; rn>=firstSelectedRule; --rn)
        {
            if (m_project->isEditorVisible() && m_project->getOpenedEditor()==ruleIndex[rn]) 
                m_project->closeEditor();

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
                ruleIndex.remove (rn);//erase(rn);
                rowsInfo.remove(rn);
                for (int i=rn; i<lastN; ++i)   ruleIndex[i]=ruleIndex[i+1];

                for (int row=rn; row<lastN; ++row)
                    setRuleNumber(row, Rule::cast( ruleIndex[row] ));

            }
        }

        ruleModel->removeRows( firstSelectedRule, lastSelectedRule );
        
        setUpdatesEnabled(true);

        clearSelection();

        setCurrentCell( firstSelectedRule, currentColumn() );
        update();
        updateGroups();
        changingRules = false;
        m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    }
}

void RuleSetView::addRuleAfterCurrent()
{
    if (!hasFocus()) return;
    if (!isTreeReadWrite(this,ruleset)) return;

    changingRules = true;
    insertRule(lastSelectedRule+1,NULL);
    changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    updateGroups();
}

void RuleSetView::moveRule()
{
    if (!hasFocus()) return;

    int selectionSize=lastSelectedRule-firstSelectedRule+1;

    /* calculate acceptable range of rule numbers for the dialog */
    int minRN=0;
    int maxRN=ruleset->size()-selectionSize;

    Ui::askRuleNumberDialog_q d;
    QDialog ddialog;
    d.setupUi(&ddialog);
    d.newRuleNum->setMinimum(minRN);
    d.newRuleNum->setMaximum(maxRN);

    if (ddialog.exec()==QDialog::Accepted)
    {
        int newN = d.newRuleNum->value();
        int nn   = newN;
        if (firstSelectedRule==nn) return;

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

                    //swapping row sizes j-1 and j
                    int t = verticalHeader()->sectionSize(j);
                    verticalHeader()->resizeSection(j,
                        verticalHeader()->sectionSize(j-1));
                    verticalHeader()->resizeSection(j-1, t);

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

                    //swapping row sizes j+1 and j
                    int t = verticalHeader()->sectionSize(j+1);
                    verticalHeader()->resizeSection(j+1,
                        verticalHeader()->sectionSize(j));
                    verticalHeader()->resizeSection(j, t);

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

        selectRE( newN , currentColumn() );
        changeCurrentCell( newN, currentColumn(), true );
        updateGroups();
        changingRules = false;
        m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
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

        ruleModel->swapRows(rn-1,rn);

        setCurrentCell( rn-1, currentColumn() );
        selectRE( rn-1 , currentColumn() );

        update();
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

        ruleModel->swapRows(rn+1,rn);

        setCurrentCell( rn+1, currentColumn() );
        selectRE( rn+1 , currentColumn() );

        update();
        updateGroups();
    }
}


void RuleSetView::copyRule()
{
    if (!hasFocus()) return;

    /*int firstSelectedRule=-1;
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
    }*/

    if ( firstSelectedRule!=-1 )
    {
        FWObjectClipboard::obj_clipboard->clear();
        for (int i=firstSelectedRule; i<=lastSelectedRule; ++i)
        {
            FWObject *rule = ruleIndex[i];

            if (fwbdebug && PolicyRule::cast(rule)!=NULL)
                qDebug(QString("RuleSetView::copyRule: direction=%1")
                        .arg(PolicyRule::cast(rule)->getDirectionAsString().c_str())
                        .toAscii().constData());

            FWObjectClipboard::obj_clipboard->add( rule, m_project );
        }
    }
    updateGroups();
}

void RuleSetView::cutRule()
{
    copyRule();
    removeRule();
}

void RuleSetView::pasteRuleAbove()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    /*int firstSelectedRule=-1;
    int lastSelectedRule=-1;*/

    changingRules = true;

    if (fwbdebug) qDebug("Firewall: pasteRuleAbove");

    /* pick rules in reverse order */
    vector<string>::reverse_iterator i;
    for (i= FWObjectClipboard::obj_clipboard->rbegin();
         i!=FWObjectClipboard::obj_clipboard->rend(); ++i)
    {
        FWObject *co= m_project->db()->findInIndex(*i);
        if (!Rule::cast(co)) continue;
        insertRule( firstSelectedRule, co);
    }

    changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    updateGroups();
}


void RuleSetView::pasteRuleBelow()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    /*int firstSelectedRule=-1;
    int lastSelectedRule=-1;*/

    changingRules = true;

    /*QTableSelection sel=selection(0);
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
    }*/

    int position;
    if (lastSelectedRule != -1)
        position = lastSelectedRule;
    else
        position = currentRow();

    int n=0;
    vector<string>::iterator i;
    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i,++n)
    {
        FWObject *co= m_project->db()->findInIndex(*i);
        if (!Rule::cast(co)) continue;
        insertRule( position+1+n, co);
    }

    changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    updateGroups();
//    if (FWObjectClipboard::obj_clipboard->getObject()!=NULL)
//        insertRule( rn+1, Rule::cast(FWObjectClipboard::obj_clipboard->getObject()) );
}

void RuleSetView::enableRule()
{
    if (!isTreeReadWrite(this,ruleset)) return;

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

    //changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
}

void RuleSetView::disableRule()
{
    if (!isTreeReadWrite(this,ruleset)) return;


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

    //changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
}

void RuleSetView::editSelected()
{
    if (!m_project->isEditorVisible()) m_project->showEditor();
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

    if (!m_project->requestEditorOwnership(this,Object,Operation,validate))
        return false;

    if (fwbdebug)
        qDebug("RuleSetView::switchObjectInEditor  editor ownership granted");

    if (Object==m_project->getOpenedEditor() && Operation==m_project->getOpenedOptEditor())
    {
        if (fwbdebug)
            qDebug("RuleSetView::switchObjectInEditor  same object is already opened in the editor");
        return true;
    }

    if (fwbdebug)
        qDebug("RuleSetView::switchObjectInEditor  opening object in the editor");

    if (Object == NULL)
    {
        m_project->blankEditor();
    } else if (Operation==ObjectEditor::optNone)
    {
        m_project->openEditor(Object);
    } else if(Rule::cast(Object)!=NULL)
    {
        m_project->openOptEditor(Object,Operation);
    }

    return true;
}

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
        FWObject *o = m_project->getOpenedEditor();
        qDebug("RuleSetView::restoreSelection() m_project->getOpenedEditor=%s",
               (o)?o->getName().c_str():"NULL");
    }

    if (same_widget)
    {
        selectedObject = prevSelectedObject;
        selectedObjectRow = prevSelectedObjectRow;
        selectedObjectCol = prevSelectedObjectCol;
    }
    changeCurrentCell(selectedObjectRow,selectedObjectCol);

    openObjectInTree(selectedObject);
}

void RuleSetView::updateAll()
{
    int r=0;
    init();
    QFontMetrics p(st->getRulesFont());
    QRect br = p.boundingRect(QRect(0, 0, 1000, 1000),
                              Qt::AlignLeft|Qt::AlignVCenter,"WMWM" );
    text_h   = br.height();
    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++,r++)
        adjustRow(r);
        //dirtyRows[r] = 1;

    repaint();
    updateGroups();
//    update();
}

void RuleSetView::updateCurrentCell()
{
    updateCell(m_currentRow, m_currentColumn);
}


PolicyView::PolicyView(ProjectPanel *project, Policy *p, QWidget *parent) : 
      RuleSetView(project, 1, 7, parent)
{
    setName("PolicyView");
    ruleset=p;
    iinit();
    init();
}

void PolicyView::init()
{
    //ncols=7 +
    ncols=8 +
        ((supports_time)?1:0) +
        ((supports_logging && supports_rule_options)?1:0);

    ruleModel->setColumnCount(ncols);

    colTypes[-1]=RuleOp;

    int col=0;
    QStringList qsl;

    qsl << "";      // -1 :)
    colTypes[col++]=Object;

    qsl << "Source";      // 0
    colTypes[col++]=Object;

    qsl << "Destination"; // 1
    colTypes[col++]=Object;

    qsl << "Service";     // 2
    colTypes[col++]=Object;

    qsl << "Interface";   // 3
    colTypes[col++]=Object;

    qsl << "Direction";   // 4
    colTypes[col++]=Direction;

    qsl << "Action";      // 5
    colTypes[col++]=Action;

    if (supports_time)
    {
        qsl << "Time";    // 6
        colTypes[col++]=Time;
    }

    if (supports_logging && supports_rule_options)
    {
        qsl << "Options";
        colTypes[col++]=Options;
    }

    qsl << "Comment";
    colTypes[col]=Comment;

    ruleModel->setHeader(qsl);

    /*horizontalHeader()->setModel(static_cast<QAbstractItemModel*>(horzHeaderLabels));
    verticalHeader()->setModel(static_cast<QAbstractItemModel*>(vertHeaderLabels));*/
//    setColumnStretchable(col, true);

    RuleSetView::init();
}

RuleElement* PolicyView::getRE( int row, int col )
{
    if (row<0) return NULL;
    if (ruleIndex[row]==NULL)
        return NULL;
    if (ruleIndex.count(row)==0) return NULL;
    PolicyRule *r = PolicyRule::cast( ruleIndex[row] );
    if(r==NULL) return NULL;
    return getRE(r, col);
}

RuleElement* PolicyView::getRE( Rule* r, int col )
{
    string ret;
    if (col==0)
        return NULL;
    if (r==NULL)
        return NULL;
    switch (getColType(col))
    {
    case Object:
        switch (col)
        {
        case 1: ret=RuleElementSrc::TYPENAME; break;
        case 2: ret=RuleElementDst::TYPENAME; break;
        case 3: ret=RuleElementSrv::TYPENAME; break;
        case 4: ret=RuleElementItf::TYPENAME; break;
        /*case 0: ret=RuleElementSrc::TYPENAME; break;
        case 1: ret=RuleElementDst::TYPENAME; break;
        case 2: ret=RuleElementSrv::TYPENAME; break;
        case 3: ret=RuleElementItf::TYPENAME; break;*/
        }
        break;
    case Time:
        ret=RuleElementInterval::TYPENAME; break;
    default: return NULL;
    }

    return RuleElement::cast( r->getFirstByType(ret) );
}

InterfacePolicyView::InterfacePolicyView(ProjectPanel *project, InterfacePolicy *p, QWidget *parent) :
    RuleSetView(project, 1,7,parent)
{
    setName("InterfacePolicyView");
    ruleset=p;
    iinit();
    init();
}

void InterfacePolicyView::init()
{
    ncols=7 +
        ((supports_time)?1:0) +
        ((supports_logging && supports_rule_options)?1:0);

    ruleModel->setColumnCount(ncols);

    colTypes[-1]=RuleOp;

    int col=0;
    QStringList qsl;
    qsl << "";      // -1
    colTypes[col++]=Object;    

    qsl << "Source";      // 0
    colTypes[col++]=Object;

    qsl << "Destination"; // 1
    colTypes[col++]=Object;

    qsl << "Service";     // 2
    colTypes[col++]=Object;

    qsl << "Direction";   // 3
    colTypes[col++]=Direction;

    qsl << "Action";      // 4
    colTypes[col++]=Action;

    if (supports_time)
    {
        qsl << "Time";    // 5
        colTypes[col++]=Time;
    }

    if (supports_logging && supports_rule_options)
    {
        qsl << "Options";
        colTypes[col++]=Options;
    }

    qsl << "Comment";
    colTypes[col]=Comment;

    ruleModel->setHeader(qsl);
//    setColumnStretchable(col, true);

//    ncols=col;

    RuleSetView::init();
}

RuleElement* InterfacePolicyView::getRE( int row, int col )
{
    if (row<0) return NULL;
    if (ruleIndex[row]==NULL)
        return NULL;
    PolicyRule *r = PolicyRule::cast( ruleIndex[row] );
    assert(r!=NULL);
    return getRE(r,col);
}

RuleElement* InterfacePolicyView::getRE( Rule *r, int col )
{
    string ret;
    if (col==0)
        return NULL;
    if (r==NULL)
        return NULL;
    switch (getColType(col))
    {
    case Object:
        switch (col)
        {
        case 1: ret=RuleElementSrc::TYPENAME; break;
        case 2: ret=RuleElementDst::TYPENAME; break;
        case 3: ret=RuleElementSrv::TYPENAME; break;
        }
        break;
    case Time:
        ret=RuleElementInterval::TYPENAME; break;
    default: return NULL;
    }

    return RuleElement::cast( r->getFirstByType(ret) );
}




NATView::NATView(ProjectPanel *project, NAT *p, QWidget *parent) : 
      RuleSetView(project, 1,8,parent)
{
    setName("NATView");
    ruleset=p;
    iinit();
    init();
}

void NATView::init()
{
    colTypes[-1]=RuleOp;

    ncols=9;
    ruleModel->setColumnCount(9);

    int col=0;
    QStringList qsl;
    qsl << "";      // -1
    colTypes[col++]=Object;

    qsl << "Original Src";
    colTypes[col++]=Object;

    qsl << "Original Dst";
    colTypes[col++]=Object;

    qsl << "Original Srv";
    colTypes[col++]=Object;

    qsl << "Translated Src";
    colTypes[col++]=Object;

    qsl << "Translated Dst";
    colTypes[col++]=Object;

    qsl << "Translated Srv";
    colTypes[col++]=Object;

    qsl << "Options";
    colTypes[col++]=Options;

    qsl << "Comment";
    colTypes[col++]=Comment;

    ruleModel->setHeader(qsl);
//    setColumnStretchable(col, true);

//    ncols=col;

    RuleSetView::init();
}

RuleElement* NATView::getRE( int row, int col )
{
    if (row<0) return NULL;
    if (ruleIndex[row]==NULL)
        return NULL;
    NATRule *r = NATRule::cast( ruleIndex[row] );
    assert(r!=NULL);
    return getRE(r,col);
}

RuleElement* NATView::getRE( Rule *r, int col )
{
    string ret;
    if (col==0)
        return NULL;
    if (r==NULL)
        return NULL;
    switch (getColType(col))
    {
        case Object:
            switch (col)
            {
                case 1: ret=RuleElementOSrc::TYPENAME; break;
                case 2: ret=RuleElementODst::TYPENAME; break;
                case 3: ret=RuleElementOSrv::TYPENAME; break;
                case 4: ret=RuleElementTSrc::TYPENAME; break;
                case 5: ret=RuleElementTDst::TYPENAME; break;
                case 6: ret=RuleElementTSrv::TYPENAME; break;
            }
            break;
            default: return NULL;
    }

    return RuleElement::cast( r->getFirstByType(ret) );
}

RoutingView::RoutingView(ProjectPanel *project, Routing *p, QWidget *parent) : 
      RuleSetView(project, 1,5,parent)
{
    setName("RoutingView");
    ruleset=p;
    iinit();
    init();
}

void RoutingView::init()
{
    colTypes[-1]=RuleOp;
    ncols=7;
    ruleModel->setColumnCount(ncols);

    int col=0;

    QStringList qsl;
    qsl << "";
    colTypes[col++]=Object;

    qsl << "Destination";
    colTypes[col++]=Object;

    qsl << "Gateway";
    colTypes[col++]=Object;

    qsl << "Interface";
    colTypes[col++]=Object;

    qsl << "Metric";
    colTypes[col++]=Metric;

    qsl << "Options";
    colTypes[col++]=Options;

    qsl << "Comment";
    colTypes[col]=Comment;

    ruleModel->setHeader(qsl);
//    setColumnStretchable(col, true);

//    ncols=col;

    RuleSetView::init();
}

RuleElement* RoutingView::getRE( int row, int col )
{
    if (row<0) return NULL;
    if (ruleIndex[row]==NULL)
        return NULL;
    RoutingRule *r = RoutingRule::cast( ruleIndex[row] );
    assert(r!=NULL);
    return getRE(r,col);
}

RuleElement* RoutingView::getRE( Rule *r, int col )
{
    string ret;
    if (col==0)
        return NULL;
    if (r==NULL)
        return NULL;
    switch (getColType(col))
    {
        case Object:
            switch (col)
            {
                case 1: ret=RuleElementRDst::TYPENAME; break;
                case 2: ret=RuleElementRGtw::TYPENAME; break;
                case 3: ret=RuleElementRItf::TYPENAME; break;
            }
            break;
            default: return NULL;
    }

    return RuleElement::cast( r->getFirstByType(ret) );
}




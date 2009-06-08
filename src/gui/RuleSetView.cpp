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


  Notes:

  to get rule object for given row number, use

    Rule *rule = ruleIndex[pos];

  or just

    Rule *rule = getRule(int row);


  To find table row for given rule, use

    int row = getRow(rule);


*/


#include "../../config.h"
#include "global.h"
#include "utils.h"

#include "ActionsDialog.h"
#include "FWBSettings.h"
#include "FWObjectClipboard.h"
#include "FWObjectDrag.h"
#include "FWObjectPropertiesFactory.h"
#include "FWWindow.h"
#include "FindObjectWidget.h"
#include "ObjectEditor.h"
#include "ObjectTreeView.h"
#include "ProjectPanel.h"
#include "RuleRowInfo.h"
#include "RuleSetView.h"
#include "SimpleIntEditor.h"
#include "SimpleTextEditor.h"
#include "findDialog.h"
#include "platforms.h"
#include "FWBTree.h"

#include <ui_askrulenumberdialog_q.h>

#include <iostream>
#include <sstream>
#include <list>

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
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
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QScrollBar>

using namespace libfwbuilder;
using namespace std;

// #define DEBUG_RULE_GROUPS 1
// #define DRAW_RULE_GROUP_FRAME 1
// #define DEBUG_PAINT_CELL 1

int QMAX(int a, int b)
{
    return(a>b)?a:b;
}

int QMIN(int a, int b)
{
    return(a<b)?a:b;
}

RuleSetView* RuleSetViewFactory::getRuleSetViewByType(ProjectPanel *project,
                                                      RuleSet *ruleset,
                                                      QWidget *parent)
{
    if (Policy::isA(ruleset))
        return new PolicyView(project, Policy::cast(ruleset), parent);
    if (NAT::isA(ruleset))
        return new NATView(project, NAT::cast(ruleset), parent);
    if (Routing::isA(ruleset))
        return new RoutingView(project, Routing::cast(ruleset), parent);
    return NULL;
}


RuleTableModel::RuleTableModel(const int rows, const int columns,
                               RuleSetView *ruleView) :
    QAbstractTableModel(static_cast<QWidget*>(ruleView)),
    m_rowCount(rows),
    m_columnCount(columns),
    ruleSetView(ruleView)
{};

RuleTableModel::~RuleTableModel() {};

int RuleTableModel::rowCount( const QModelIndex & ) const
{
    return m_rowCount;
}

int RuleTableModel::columnCount( const QModelIndex & ) const
{
    return m_columnCount;
}

void RuleTableModel::setColumnCount( const int &value )
{
    m_columnCount = value;
    reset();
    //need to reset model for RuleSetView::init(one i)
}

void RuleTableModel::setRowCount( const int &value )
{
    m_rowCount = value;
    reset();
    //need to reset model for RuleSetView::iinit(two is)
}

QVariant RuleTableModel::data( const QModelIndex &, int /*role*/) const
{
    return QVariant();
}

QVariant RuleTableModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role ) const
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
        Rule *rule =  ruleSetView->ruleIndex[section];

        if (role == Qt::SizeHintRole)
            return QSize(45, ruleSetView->getRowHeight(section));
        if (role == Qt::DisplayRole)
        {
            if (rule==NULL)
            {
                return QString("");
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

            if ((section >= ruleSetView->firstSelectedRow) &&
                (section <= ruleSetView->lastSelectedRow))
                f.setBold(true);

            return f;
        }
        if (role == Qt::DecorationRole)
        {
            if (rule && rule->isDisabled())
                return QVariant(ruleSetView->negIcon);
            else
                return QVariant(QIcon());
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role );
}

bool RuleTableModel::setHeader( QStringList qsl )
{
    header = qsl;
    m_columnCount = qsl.size();
    return true;
}

void RuleTableModel::insertRow( const int before_pos )
{
    if (fwbdebug) qDebug("RuleTableModel::insertRow before_pos=%d", before_pos);

    m_rowCount++;

    ruleSetView->freezeRowSizing();

    ruleSetView->rowHeights.push_back(0);
    for (int i = ruleSetView->rowHeights.size()-2;
         i >= before_pos; i--)
        ruleSetView->rowHeights[i+1] = ruleSetView->rowHeights[i];
    ruleSetView->rowHeights[before_pos] = 30; //standard size

    //we add a row here and system resets below rows' sizes
    //so we had to freeze our sizes for restoring them later

    QAbstractTableModel::beginInsertRows(QModelIndex(),
                                         before_pos, before_pos );
    QAbstractTableModel::insertRow(before_pos);
    QAbstractTableModel::endInsertRows();

    //somehow QAbstractItemModel breaks all the sizes after "before_pos" row
    //so we restore them
    if (before_pos > 0)
        for (int i = before_pos-1; i < m_rowCount; i++)
            //step back a little because of another QTableView bug
            ruleSetView->verticalHeader()->resizeSection(
                i, ruleSetView->rowHeights[i]);
    else
        for (int i = before_pos; i < m_rowCount; i++)
            ruleSetView->verticalHeader()->resizeSection(
                i, ruleSetView->rowHeights[i]);

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

void RuleTableModel::removeRows( const int row1, const int row2 )
{
    //(row2-row1)+1  rows deleted
    m_rowCount -= (row2-row1)+1;

    ruleSetView->freezeRowSizing();

    for (int i = row1;
         i < static_cast<int>(ruleSetView->rowHeights.size())-((row2-row1)+1);
         i++)
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

void RuleDelegate::paint(QPainter *painter,
                         const QStyleOptionViewItem &,
                         const QModelIndex &index) const
{
    QRect size = cellGeometry(index.row(), index.column());

    if (ruleSetView->ruleIndex[index.row()]==NULL)
    {
        if (index.column() == 1)
        {
            // Correct geometry rect for the rule group head row
            // Basically, we expand one cell for the whole row
            int w = 0 ;
            for (int i = 1; i < ruleSetView->ruleModel->columnCount(index); i++)
                w += ruleSetView->getColumnWidth(i);

            size.setWidth(w);
        }
    }

    ruleSetView->paintCell( painter,
                            index.row(),
                            index.column(),
                            size,
                            ruleSetView->itemSelectionRange.contains(index),
                            QPalette() );
}

QSize RuleDelegate::sizeHint(const QStyleOptionViewItem &,
                   const QModelIndex & ) const
{
    //return QSize(getColumnWidth(index.column()), getRowHeight(index.row()));
    return QSize(30, 19);
}

/*****************************************************************

    RuleSetView

 *****************************************************************/


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
        if (where.width() == 0)
            qDebug("pixmap load failed: %s", path.toAscii().constData());
        QPixmapCache::insert( path, where );
    }
}

QPixmap LoadPixmap(const QString path)
{
    QPixmap p;
    LoadPixmap(path, p);
    return p;
}

RuleSetView::RuleSetView(ProjectPanel *project, int , int c, QWidget *parent): 
              QTableView( /*r, c,*/ parent ),
              hme(this),
              ncols(c),
              RuleElementSpacing(4),
              selectedObject(0), 
              selectedObjectCol(0),
              prevSelectedObject(0),
              kbdGoingUp(false),
              changingSelection(false),
              changingRules(false),
              firstSelectedRow(-1),
              lastSelectedRow(-1),
              m_project(project)
{
    setFont(st->getRulesFont());
    rowSizingFrozen = false;

    setShowGrid(true);

    prevCurrentRow = 0;
    prevCurrentCol = 0;

    ruleModel = NULL;
    ruleDelegate = NULL;
    createRuleModel();

    setCurrentCell(0,0);

    setDragEnabled(true);
    setAcceptDrops(true);

    setFocusPolicy( Qt::StrongFocus );

    setSelectionMode( QAbstractItemView::ContiguousSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

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

    connect( horizontalHeader(), SIGNAL( sectionResized( int, int, int ) ),
             this, SLOT( horzSectionResized( int, int, int ) ) );

    connect( verticalHeader(),   SIGNAL( sectionResized( int, int, int ) ),
             this, SLOT( vertSectionResized( int, int, int ) ) );

    connect( this, SIGNAL( customContextMenuRequested(const QPoint&) ),
             this, SLOT( contextMenuRequested(const QPoint&) ) );

    connect( this, SIGNAL( doubleClicked(const QModelIndex&) ),
             this, SLOT( itemDoubleClicked(const QModelIndex&) ) );

    verticalHeader()->installEventFilter( &hme );
//    unselect();
}

RuleSetView::~RuleSetView()
{
}

/*
 *  QTableView updates scroll bars (among other things) in this
 *  virtual slot. We want vertical scroller to be by-pixel but their
 *  implementation sets step to approximately average row height. This
 *  causes problems when rows have very different height (some are
 *  small, some are very tall) because scroll step becomes too big.
 */
void RuleSetView::updateGeometries()
{
    QTableView::updateGeometries();
    verticalScrollBar()->setSingleStep(20);
}

bool RuleSetView::showCommentTip(const QPoint &pos, QHelpEvent *he)
{
    if (!st->getClipComment()) return false;

    int col = columnAt(pos.x() - verticalHeader()->width());
    if ((pos.y() >= horizontalHeader()->height()) &&
       RuleSetView::Comment == getColType(col))
    {

        QString t="";
        int row = rowAt(pos.y() - horizontalHeader()->height());

        QRect   cr;

        cr = ruleDelegate->cellGeometry(row, col);
        PolicyRule *rule = PolicyRule::cast( getRule(row) );
        if (rule!=NULL)
        {
            Rule *rule = ruleIndex[row];
            t= QString::fromUtf8(rule->getComment().c_str());
        }
        cr = QRect(
                cr.left() - horizontalOffset() - 2,
                cr.top() - verticalOffset() - 2,
                cr.width() + 4,
                cr.height() + 4);

        QRect global = QRect(
            viewport()->mapToGlobal(cr.topLeft()),
            viewport()->mapToGlobal(cr.bottomRight()));
        QToolTip::showText(mapToGlobal( he->pos() ), t, this, global);
        return true;
    }
    return false;
}

bool RuleSetView::event( QEvent * event )
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
            QString tooltip_text = "";

            QPoint contentsMouse = viewport()->mapFromGlobal(mapToGlobal(pos));
            contentsMouse.setY(contentsMouse.y() + verticalOffset() + 3);

            cr = ruleDelegate->cellGeometry(row, col);

            if ( RuleSetView::GroupHandle == getColType(col) )
            {
                tooltip_text = getFullRuleGroupTitle(row);
            } else if ( RuleSetView::Options == getColType(col) )
            {
                Rule *rule = getRule(row);
                if (PolicyRule::cast(rule)!=NULL )
                {
                    if (!isDefaultPolicyRuleOptions(rule->getOptionsObject()))
                        tooltip_text = 
                            FWObjectPropertiesFactory::getPolicyRuleOptions(
                                rule);
                }
                if (NATRule::cast(rule)!=NULL )
                {
                    if (!isDefaultNATRuleOptions( rule->getOptionsObject()))
                        tooltip_text =
                            FWObjectPropertiesFactory::getNATRuleOptions(rule);
                }
            }
            else if( RuleSetView::Direction == getColType(col) )
            {
                PolicyRule *rule = PolicyRule::cast( getRule(row) );
                if (rule!=NULL)
                    tooltip_text = rule->getDirectionAsString().c_str();
            }
            else if( RuleSetView::Action == getColType(col) )
            {
                PolicyRule *rule = PolicyRule::cast( getRule(row) );
                if (rule!=NULL)
                    tooltip_text =
                        FWObjectPropertiesFactory::getRuleActionPropertiesRich(
                            rule);
            }
            else
            {
                FWObject *obj = getObj(row,col,contentsMouse.y(),&cr);
                if (obj==NULL)
                    return true;
                tooltip_text = 
                    FWObjectPropertiesFactory::getObjectPropertiesDetailed(
                        obj,true,true);
            }

            cr = QRect(
                cr.left() - horizontalOffset() - 2,
                cr.top() - verticalOffset() - 2,
                cr.width() + 4,
                cr.height() + 4);

            QRect global = QRect(
                viewport()->mapToGlobal(cr.topLeft()),
                viewport()->mapToGlobal(cr.bottomRight()));

            QToolTip::showText(
                mapToGlobal(he->pos()), tooltip_text, this, global);
        }

        return true;
    }

    return QTableView::event(event);
}

void RuleSetView::contextMenuRequested( const QPoint &p )
{
    contextMenu(rowAt(p.y()), columnAt(p.x()), viewport()->mapToGlobal(p));
}

void RuleSetView::currentChanged( const QModelIndex &current )
{
    changeCurrentCell(current.row(), current.column());
}

/*
 * Should not do anything with other MDI subwindows from a subwindow.
 */
void RuleSetView::updateCell(const int row, const int col)
{
    if ((row < 0) || (col < 0))
        return;

    QModelIndex ind = ruleModel->index(row, col);
    setCurrentCell(row, col);
    dataChanged(ind, ind);
}

void RuleSetView::setName(const QString&)
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

void RuleSetView::scrollToCurrent()
{
    scrollTo( ruleModel->index(currentRow(), currentColumn()),
              QAbstractItemView::EnsureVisible);
}

void RuleSetView::setCurrentCell(const int row, const int col)
{
    setCurrentRow(row);
    setCurrentColumn(col);
}

void RuleSetView::changeCurrentCell(const int row, const int col,
                                    bool fullrefresh)
{
    QModelIndex ind = ruleModel->index(currentRow(), currentColumn());
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
    updateCell(currentRow(), currentColumn());
}

Firewall* RuleSetView::getFirewall()
{
    FWObject *f=ruleset;
    while (f!=NULL && !Firewall::isA(f)) f=f->getParent();
    // f can be NULL if user is looking at deleted ruleset which is a child
    // of the library DeletedObjects
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

void RuleSetView::setRuleNumber(int row, Rule *rule)
{
    rulesDisabled[row] = (rule!=NULL && rule->isDisabled());
    verticalHeader()->headerDataChanged(Qt::Vertical, row, row);
}

void RuleSetView::fixRulePosition(Rule *rule, FWObject *parent, int pos)
{
    if ( rule->isReadOnly())
    {
        // need to temporary break the lock
        // since several parents could be read-only, do it recursively
        FWObject *o = (parent!=NULL) ? parent : rule;
        while ( o!=NULL && !o->getRO() ) o = o->getParent();
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

RuleRowInfo* RuleSetView::getRuleRowInfoByGroupName(const QString &name)
{
    if (fwbdebug) qDebug("RuleSetView::getRuleRowInfoByGroupName name=%s",
                         name.toAscii().constData());
//    if (name == "") name = "New Group";
    for (int i = 0 ; i < rowsInfo.size(); i++)
        if (rowsInfo[i] && rowsInfo[i]->groupName == name)  return rowsInfo[i];
    return NULL;
}

int RuleSetView::getRuleRowInfoIndexByGroupName(const QString &name)
{
    for (int i = 0 ; i < rowsInfo.size(); i++)
        if (rowsInfo[i] && rowsInfo[i]->groupName == name) return i;
    return -1;
}

QString RuleSetView::getFullRuleGroupTitle(int row)
{
    Rule *rule = getRule(row);
    QString groupTitle;
    QString groupName;
    if (rule==NULL)
    {
        // rule group head row
        RuleRowInfo *rri = rowsInfo[row];
        if (rri) groupName = rri->groupName;
    } else
        groupName = QString::fromUtf8(rule->getRuleGroupName().c_str());

    if (!groupName.isEmpty())
    {
        int count = rulesInGroup[groupName];
        groupTitle = groupName;
        groupTitle += " (" ;
        groupTitle += QString().setNum(count);
        groupTitle += " rules)";
    }
    return groupTitle;
}

void RuleSetView::init()
{
    if (fwbdebug) qDebug("RuleSetView::init()");

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    resetRuleModel();

    //horizontalHeader()->adjustSize();

    int row = 0;
    map<int,int> colW;
    bool userColWidth = false;

    ruleIndex.clear();
    rowsInfo.clear();

    QFontMetrics p(st->getRulesFont());
    QString k = settingsKey();
    QString v = st->getStr(k);
    if (!v.isEmpty())
    {
        userColWidth=true;
        for (int col=0; col<ncols; col++)
            colW[col] = v.section(' ',col,col).toInt();
    } else
    {
        for (int col=0; col<ncols; col++)
        {
            QString lbl = ruleModel->headerData(col,
                                                Qt::Horizontal,
                                                Qt::DisplayRole).toString();

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

    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++, row++)
    {
        Rule *r = Rule::cast( *i );

        ruleIndex[row] = r;
        rowsInfo.push_back(NULL);
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
            horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
            horizontalHeader()->resizeSection(0, 20);
            colW[0]=20;
            continue ;
        }
        setColumnWidth(col,colW[col]);
        horizontalHeader()->resizeSection(col, colW[col]);
    }

#if DEBUG_RULE_GROUPS
    if (fwbdebug)
    {
        qDebug("End of init:");
        debugPrintRows();
    }
#endif

    updateGroups();

    // Note:     restoreCollapsedGroups() calls updateGroups()
    // cant call restoreCollapsedGroups() from here because it would
    // cause infinite loop.
    QTimer::singleShot(0, this, SLOT(restoreCollapsedGroups()));

#if DEBUG_RULE_GROUPS
    if (fwbdebug)
    {
        qDebug("After updateGroups:");
        debugPrintRows();
    }
#endif

    update();

    QApplication::restoreOverrideCursor();

    if (fwbdebug) qDebug("RuleSetView::init() done");
}

void RuleSetView::debugPrintRows()
{
    for (int i1=0; i1<ruleIndex.size(); i1++)
        qDebug("ruleIndex[%d]=%p", i1, ruleIndex[i1]);

    qDebug(" ");

    return;

    for (int i = 0 ; i < rowsInfo.size(); i++)
    {
        if (rowsInfo[i]!=NULL)
        {
            QString output;
            QTextStream res(&output, QIODevice::WriteOnly);
        
            res << "groupName=";
            res << rowsInfo[i]->groupName;
            res << " ";
            res << "isFirstRow=";
            res << rowsInfo[i]->isFirstRow;
            res << " ";
            res << "collapsed=";
            res << rowsInfo[i]->collapsedGroup;

            qDebug("rowsInfo[%d]: %s", i, output.toAscii().constData());
        } else
            qDebug("rowsInfo[%d]: NULL", i);

    }
    qDebug(" ");
    qDebug("ruleModel->rowCount()=%d", ruleModel->rowCount());
    qDebug(" ");
}

void RuleSetView::refreshGroups()
{
}

void RuleSetView::updateGroups()
{
    if (fwbdebug) qDebug("RuleSetView::updateGroups  ruleset name=%s",
                         ruleset->getName().c_str());

    reset();

    QStringList collapsed_groups;
    if (m_project!=NULL) // For printing without creating ProjectPanel
    {
        // we do not have access to project panel and therefore to the file
        // name when we print from command line. Will always print all
        // rule groups expanded when print from command line
        QString filename = m_project->getRCS()->getFileName();
        st->getCollapsedRuleGroups(
            filename,
            getFirewall()->getName().c_str(),
            ruleset->getName().c_str(),
            collapsed_groups);
    }

    setColumnWidth(0, 20);
    horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(0, 20);

    groupColors.clear();
    for (int i = 0 ; i < rowsInfo.size(); i++)
    {
        if (ruleIndex.count(i)!=0 && ruleIndex[i]==NULL)
        {        
            if (rowsInfo[i]->isFirstRow)
            {
                groupColors[rowsInfo[i]->groupName] = rowsInfo[i]->color ;
                if (groupColors[rowsInfo[i]->groupName]=="")
                {
                    Rule * r = ruleIndex[i+1];
                    if (r)
                    {
                        FWOptions *ro = r->getOptionsObject();
                        QString color = ro->getStr("color").c_str();
                        if (color!="")
                        {
                            if (color=="#FFFFFF") color = "";
                            groupColors[rowsInfo[i]->groupName]=color;
                        }
                    }
                }
            }
            removeRuleIndex(i);
            rowsInfo.remove(i);
            ruleModel->removeRows(i, i);
            i--;
        }
    }
    
    if (fwbdebug) qDebug("RuleSetView::updateGroups checkpoint 1");

    QString memberRow = "";
    bool beginGroup = false ;
    QString group;

    rulesInGroup.clear();
    for (int i = 0 ; i < rowsInfo.size(); i++)
    {
        if (ruleIndex[i]!=NULL)
        {
            Rule * r = ruleIndex[i];
            QString groupName = QString::fromUtf8(r->getRuleGroupName().c_str());

#if DEBUG_RULE_GROUPS
            if (fwbdebug) qDebug("row %d: group %s",
                                 i, groupName.toAscii().constData());
#endif

            int count = rulesInGroup[groupName];
            count++;
            rulesInGroup[groupName] = count ;
        }
    }

    if (fwbdebug) qDebug("RuleSetView::updateGroups checkpoint 2");

    for (int i = 0 ; i < rowsInfo.size(); i++)
    {
        Rule * r = ruleIndex[i];
        group = QString::fromUtf8(r->getRuleGroupName().c_str());
        QString color = groupColors[group];

        RuleRowInfo *rri;

        if (group!=memberRow)
        {
            ruleModel->insertRow(i);
            if (!beginGroup)
            {
                insertRuleIndex(i);
                ruleIndex[i] = NULL ;
                beginGroup = true ;
                memberRow = group;
                rri = new RuleRowInfo(memberRow, true, false);
                color = r->getOptionsObject()->getStr("color").c_str();
                if (color=="#FFFFFF") color="";
                groupColors[memberRow] = color;
                rri->color = color;
                rowsInfo.insert(i, rri);
            }
            else
            {
                insertRuleIndex(i);
                ruleIndex[i] = NULL ;
                beginGroup = false ;
                rri = new RuleRowInfo(memberRow, false, false);
                rowsInfo.insert (i, rri);
                memberRow = "";
                i--;              
            }
            i++;
        }
    }

    if (group!="")
    {
        ruleModel->insertRow(rowsInfo.size());
        ruleIndex[rowsInfo.size()] = NULL ;
        beginGroup = false ;
        rowsInfo.push_back(new RuleRowInfo(memberRow,false,false));
        memberRow = group;
    }

    for (int row = 0 ; row < rowsInfo.size(); row++)
    {
        RuleRowInfo *rri = rowsInfo[row];
        if (rri)
        {
            rri->collapsedGroup = 
                bool(collapsed_groups.count(rri->groupName) > 0);
            if (rri->isFirstRow) setSpan(row, 1, 1, ncols-1);
            else
            {
                hideRow(row);
            }
        } else
            setSpan(row, 1, 1, 1);
    }

    if (fwbdebug) qDebug("RuleSetView::updateGroups done");
}

QSize RuleSetView::getPMSize()
{
    if (!st->getShowIconsInRules())
    {
        pixmap_h = 0;
        pixmap_w = 0;
        return QSize();
    }

    QString icn="icon-ref";
    if (FWBSettings::SIZE16X16 == st->getIconsInRulesSize()) icn="icon-tree";

    QString icn_file = QString(":/Icons/AddressTable/")+icn;

    QPixmap pm;
    LoadPixmap(icn_file, pm);

    pixmap_h = pm.height();
    pixmap_w = pm.width();
    return pm.size();
}

void RuleSetView::resetRuleModel()
{
#if 0
    if (ruleModel) delete ruleModel;
    ruleModel = NULL;
    if (ruleDelegate) delete ruleDelegate;
    ruleDelegate = NULL;
    createRuleModel();
#endif
}

void RuleSetView::createRuleModel()
{
    if (ruleModel==NULL && ruleDelegate==NULL) 
    {
        ruleModel = new RuleTableModel(1, 1, this);
        setModel(ruleModel);
        ruleDelegate = new RuleDelegate(this);
        setItemDelegate(ruleDelegate);
    }
}

void RuleSetView::iinit()
{
    createRuleModel();

    ruleModel->setRowCount( ruleset->size() );

    QSize sz = getPMSize();

    QFontMetrics p(st->getRulesFont());
    QRect br = p.boundingRect(QRect(0, 0, 1000, 1000),
                              Qt::AlignLeft|Qt::AlignVCenter,"WMWM" );
    text_h   = br.height();
    item_h   = ( (pixmap_h>text_h)?pixmap_h:text_h ) + RuleElementSpacing;
    //qDebug("pixmap_h = %d text_h = %d item_h = %d", pixmap_h, text_h, item_h);

    FWObject *fw = getFirewall();

// f is a pointer at firewall object
    supports_logging      = false;
    supports_rule_options = false;
    supports_time         = false;
    supports_routing_itf  = false;

    if (fw)
    {
        try {
            supports_logging =
                Resources::getTargetCapabilityBool(fw->getStr("platform"),
                                                   "logging_in_policy");
            supports_rule_options =
                Resources::getTargetCapabilityBool(fw->getStr("platform"),
                                                   "options_in_policy");
            supports_time =
                Resources::getTargetCapabilityBool(fw->getStr("platform"),
                                                   "supports_time");
            supports_routing_itf =
                Resources::getTargetCapabilityBool(fw->getStr("platform"),
                                                   "supports_routing_itf");
        } catch(FWException &ex)    {    }
    }

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

    //QPainter p(this);
    QFontMetrics p(st->getRulesFont());
    QRect br = p.boundingRect(QRect(0, 0, 1000, 1000),
                              Qt::AlignLeft|Qt::AlignVCenter,"WMWM" );
    text_h   = br.height();

    item_h = ((pixmap_h>text_h) ? pixmap_h:text_h) + RuleElementSpacing;
    Rule *rule = ruleIndex[row];

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
            if (ac == "Undefined") ac = "Both";
            QRect br = p.boundingRect(QRect(0, 0, 1000, 1000),
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

/**
 * Returns column number for the given column type.
 * Note that for some types this operation is ambiguous because we have
 * several columns with the same type. In that case return the number
 * of the first column that matches.
 */
int RuleSetView::getColByType(REType type) const
{
    map<int,REType>::const_iterator i;
    for (i=colTypes.begin(); i!=colTypes.end(); ++i)
        if (i->second == type) return i->first;
    return -1;
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
    string icn = "icon";
    if (pmattr == Neg)  icn="icon-neg";
    if (pmattr == Ref)  icn="icon-ref";
    if (pmattr == Tree) icn="icon-tree";
    if (pmattr == NegTree) icn="icon-neg-tree";

    QString icn_file = (":/Icons/"+obj->getTypeName()+"/"+icn).c_str();
    QPixmap pm;
    LoadPixmap(icn_file, pm);

    return pm;
}

void RuleSetView::repaintSelection() {}

void RuleSetView::paintEvent(QPaintEvent * event)
{
    QTableView::paintEvent(event);

    // There seems to be a bug in QT 4.4.1 (not sure of 4.4.0,
    // definitely not in 4.3.x) which causes QTableView::paintEvent to
    // sometimes skip the last row of the table when it redraws
    // contents if rows have very different height. This looks like
    // the last row comes out blank when user scrolls the table
    // up. The last row is finally redrawn when most of it is already
    // visible. The error appears to be in QTableView::paintEvent and
    // is hard to work around. Even if this is fixed in 4.4.2 (to be
    // verified still), some distros ship 4.4.1 so the problem can not
    // be ignored.
    // This is known to happen on Ubuntu Hardy, as well as Mac and Windows
    // if the program is linked with QT 4.4.1
    //
    // The following fragment is a hack that always forces redraw of
    // the last row. This should have some performance impact but it
    // is hard to measure. The problem with this hack is that table
    // grid is drawn in QTableView::paintEvent and gets overwritten by
    // drawing functions in paintCell(). To avoid that we should be
    // careful to dra only inside the cell rectangle in paintCell()
    // 
    //
    int last_visual_row = verticalHeader()->visualIndexAt(height());
    if (last_visual_row >= 0)
    {
#ifdef DEBUG_PAINT_CELL
        if (fwbdebug) qDebug("RuleSetView::paintEvent: last_visual_row=%d",
                             last_visual_row);
#endif
        QPainter painter(viewport());
        for (int col=0; col < ncols; col++)
            ruleDelegate->paint(&painter, QStyleOptionViewItem(),
                                ruleModel->index(last_visual_row, col));
    }
}

void RuleSetView::paintCell(QPainter *pntr,
                            int row,
                            int col,
                            const QRect &cr,
                            bool selected,
                            const QPalette &cg)
{
    int re_size;

/* row may point at an empty row where there is no rule yet. This
 * happens if this method is called to redraw the table when we call
 * setNumRows
 */
#ifdef DEBUG_PAINT_CELL
    if (fwbdebug)
        qDebug("RuleSetView::paintCell: row=%d col=%d cr=%dx%dx%dx%d",
               row, col, cr.top(), cr.left(), cr.width(), cr.height());
#endif

    if (ruleIndex.count(row)==0) return;

    if (col==0)
    {
        drawRuleGroupHandle(pntr, row, col, cr, selected, cg);
        return;
    }

    Rule *rule = ruleIndex[row];
    RuleRowInfo * rri = rowsInfo[row];

    if (rule==NULL && rri && rri->isFirstRow) 
    {
        /* Rule group head */
        drawRuleGroupHead(pntr, row, col, cr, selected, cg);
        return;
    }

    FWOptions  *ropt = rule->getOptionsObject();
    assert(ropt!=NULL);
    QString gclr = getGroupColorForRule(rule);
    QString rclr = ropt->getStr("color").c_str();

    // group color, if set, overrides rule color
    if (!gclr.isEmpty()) rclr = gclr;

    QPixmap bufferpixmap;
    QString bpmname = QString("rulesetcell_%1_%2").
        arg(cr.width()).arg(cr.height());

    if ( ! QPixmapCache::find( bpmname, bufferpixmap) )
    {
        bufferpixmap = QPixmap( cr.width()-1 , cr.height()-1 );
        QPixmapCache::insert( bpmname, bufferpixmap);
    }

    //bufferpixmap.resize( cr.width() , cr.height() );
    bufferpixmap.fill(cg.base().color() );

    QPainter p( &bufferpixmap );
    QFont font = st->getRulesFont();
    p.setFont(font);

    QRect r = ruleDelegate->cellRect(row,col);
    int x  = r.left() + RuleElementSpacing/2;
    int y  = r.top();

    if (!rclr.isEmpty())
    {
        QRect rect(0, y, cr.width(), cr.height() );
        p.fillRect(rect, QColor(rclr));
    }

#if 0
// there is no need to draw table grid because we call setShowGrid(true)
    p.drawLine( cr.width()-1, 0, cr.width()-1, cr.height()-1 );
    p.drawLine( 0, cr.height()-1, cr.width()-1, cr.height()-1 );

    p.drawLine( cr.width(), 1, cr.width(), cr.height() );
    p.drawLine( 1, cr.height(), cr.width(), cr.height() );
#endif

    bool  sel = (row==currentRow() && col==currentColumn());

    if (getColType(col)==Object || getColType(col)==Time)
    {
        RuleElement *re = getRE(row, col);
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
                    p.fillRect(rect, cg.brush( QPalette::Mid ));//cg.brush( crole ));
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
                        Qt::AlignLeft|Qt::AlignVCenter, objectText(re, o1) );
#ifdef DEBUG_PAINT_CELL
            if (fwbdebug)
                qDebug("RuleSetView::paintCell: row %d rule %d object %s",
                       row, rule->getPosition(), o1->getName().c_str());
#endif

            FWObject *mwSelObj = selectedObject;
            std::vector<libfwbuilder::FWObject*> om_selected_objects ;
            if (m_project!=NULL) // For printing without creating ProjectPanel
                om_selected_objects = m_project->getCurrentObjectTree()->getSelectedObjects();

            if (mwSelObj==NULL && om_selected_objects.size()!=0)
                mwSelObj = om_selected_objects.front();

            if (    (!sel) &&
                      mwSelObj!= NULL &&
                    mwSelObj->getId() != FWObjectDatabase::ANY_ADDRESS_ID &&
                    mwSelObj->getId() != FWObjectDatabase::ANY_SERVICE_ID &&
                    mwSelObj->getId() != FWObjectDatabase::ANY_INTERVAL_ID &&
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
                p.fillRect(rect, cg.brush( QPalette::Mid )); // crole
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
                            Qt::AlignLeft|Qt::AlignVCenter,
                            res.toAscii().constData() );
                break;
            }
            case Direction:
            {
                PolicyRule *rule = PolicyRule::cast( ruleIndex[row] );
                if (rule==NULL) return;
                
                string dir = rule->getDirectionAsString();
                if (dir.empty() || dir == "Undefined") 
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

                QRect br = p.boundingRect(QRect(x, y, 1000, 1000),
                                          Qt::AlignLeft|Qt::AlignVCenter,
                                          res.toAscii().constData() );
                if (br.height()>height) height=br.height();

                p.drawText( x, y+ RuleElementSpacing/2, br.width(), height,
                            Qt::AlignLeft|Qt::AlignVCenter,
                            res.toAscii().constData() );

                break;
            }
            case Options:
            {
                /* both policy and routing rules have options. so cast
                 * to Rule here. */
                Rule *rule = ruleIndex[row];
                if (rule==NULL) return;

                /* is this a policy rule? only policy rules have the
                 * logging option. */
                PolicyRule  *policyRule  = PolicyRule::cast( rule );
                NATRule     *natRule     = NATRule::cast( rule );
                RoutingRule *routingRule = RoutingRule::cast( rule );

                if (policyRule && policyRule->getLogging())
                {
                    QString icn = chooseIcon(":/Icons/Log");
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

                p.drawText(
                    x, y, cr.width()-2, RuleElementSpacing*2+pixmap_h,
                    Qt::AlignHCenter|Qt::AlignVCenter,
                    QString::fromUtf8(rule->getMetricAsString().c_str()) );

                break;
            }
            default:
                break;
        }  // switch
    }


#ifdef DRAW_RULE_GROUP_FRAME
    QString group = QString::fromUtf8(rule->getRuleGroupName ().c_str());
    if (group!= "")
    {
        if (groupEnd!=-1 && groupEnd==row+1)
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
#endif


    p.end();

    pntr->drawPixmap( cr.left() - horizontalOffset(),
                      cr.top() - verticalOffset(), bufferpixmap );

    return;
}

void RuleSetView::drawRuleGroupHead(QPainter *pntr, int row, int col,
                                    const QRect &cr,
                                    bool, const QPalette &cg)
{
    QString rclr;
    QPixmap bufferpixmap;
    QString bpmname = 
        QString("rulesetcell_%1_%2_%3").
        arg(ruleset->getName().c_str()).arg(row).arg(col);

    if ( ! QPixmapCache::find( bpmname, bufferpixmap) )
    {
        bufferpixmap = QPixmap( cr.width()-1 , cr.height()-1 );
        QPixmapCache::insert( bpmname, bufferpixmap);
    }    

    bufferpixmap.fill(cg.mid().color());
    QPainter p( &bufferpixmap );

    if (col == 1)
    {
        QFont font = st->getRulesFont();
        p.setFont(font);

        RuleRowInfo * rri = rowsInfo[row];

#ifdef DRAW_RULE_GROUP_FRAME
        p.setPen(Qt::green);
        if (rri->isFirstRow)
        {
            p.drawLine( 1, 1, 1, cr.height() );
            p.drawLine( 1, 1, cr.width()-3, 1 );
            p.drawLine( cr.width()-3, 1, cr.width()-3, cr.height() );
        }
#endif

        if (rri->collapsedGroup)   //   isRowHidden(row+1))
            p.drawLine( 1, cr.height()-3,  cr.width() , cr.height()-3);

        QString groupTitle = getFullRuleGroupTitle(row);
        QRect r = ruleDelegate->cellRect(row, col);

        int x  = r.left() + RuleElementSpacing/2;
        int y  = r.top();
        QRect br = p.boundingRect(QRect(x, y, 1000, 1000),
                                  Qt::AlignLeft|Qt::AlignVCenter,
                                  groupTitle);
        p.drawText( x, y + RuleElementSpacing/2,
                    br.width(),
                    br.height(),
                    Qt::AlignLeft|Qt::AlignTop,
                    groupTitle);
    }

    p.end();
    pntr->drawPixmap( cr.left() - horizontalOffset(),
                      cr.top() - verticalOffset(), bufferpixmap );
}

void RuleSetView::drawRuleGroupHandle(QPainter *pntr, int row, int,
                                      const QRect &cr,
                                      bool, const QPalette &cg)
{
    // Leftmost column, we draw rule group expand/collapse control
    // element here.
    QString rclr;
    Rule *rule = ruleIndex[row];
    int groupEnd = getDownNullRuleIndex(row); 

    QPixmap bufferpixmap;
    QString bpmname = QString("rulesetcell_%1_%2").
        arg(cr.width()).arg(cr.height());

    if ( ! QPixmapCache::find( bpmname, bufferpixmap) )
    {
        bufferpixmap = QPixmap( cr.width()-1 , cr.height()-1 );
        QPixmapCache::insert( bpmname, bufferpixmap);
    }
    
    bufferpixmap.fill(cg.base().color() );
    
    QPainter p( &bufferpixmap );
    QFont font = st->getRulesFont();
    p.setFont(font);

    if (rule==NULL)
    {
        RuleRowInfo * rri = rowsInfo[row];
                    
        if (rri->isFirstRow)
        {
            p.drawRect((cr.width()-1)/2-4,(cr.height()-1)/2-4,8,8);
            p.drawLine( (cr.width()-1)/2+7, (cr.height()-1)/2,
                        cr.width()-1, (cr.height()-1)/2 );    
            if (!isRowHidden(row+1))
            {
                p.drawLine(
                    (cr.width()-1)/2, (cr.height()-1)/2+7,
                    (cr.width()-1)/2, cr.height()-1 );
                p.drawLine(
                    (cr.width()-1)/2-2, (cr.height()-1)/2,
                    (cr.width()-1)/2+2, (cr.height()-1)/2);
            }
            else
            {
                p.drawLine(
                    (cr.width()-1)/2-2, (cr.height()-1)/2,
                    (cr.width()-1)/2+2, (cr.height()-1)/2);
                p.drawLine(
                    (cr.width()-1)/2, (cr.height()-1)/2-2,
                    (cr.width()-1)/2, (cr.height()-1)/2+2);
            }    

        }
    }
    else
    {
        QString group = QString::fromUtf8(rule->getRuleGroupName().c_str());
        if (group != "")
        {
            if (groupEnd!=-1 && groupEnd==row+1)
            {
//                    p.drawLine( (cr.width()-1)/2, (cr.height()-1)/2,
//                                cr.width()-1, (cr.height()-1)/2 );
//                    p.drawLine( (cr.width()-1)/2, (cr.height()-1)/2,
//                                (cr.width()-1)/2, 0 );

                p.drawLine( (cr.width()-1)/2, cr.height()-4,
                            cr.width()-1, cr.height()-4 );
                p.drawLine( (cr.width()-1)/2, cr.height()-4,
                            (cr.width()-1)/2, 0 );

            }
            else
            {
                p.drawLine( (cr.width()-1)/2, 0, (cr.width()-1)/2,
                            cr.height()-1 );
            }
        }
    }
    p.end();

    pntr->drawPixmap( cr.left() - horizontalOffset(),
                      cr.top() - verticalOffset(), bufferpixmap );
    return ;

}

QSize RuleSetView::drawIconInRule(QPainter &p,
                                  int x, int y, RuleElement *re, FWObject *o1)
{

    if (!st->getShowIconsInRules()) return QSize();

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
    
    if (!re->isAny()) p.drawPixmap( x, y + RuleElementSpacing/2, pm );

    return pm.size();
}

void RuleSetView::drawComment(QPainter &p, int row, int col, const QRect &cr)
{
    /* comments are found in both policy and nat rules, so we cast to
     * Rule here */
    Rule *rule = ruleIndex[row];
    if (rule==NULL) return;
    QString comm = QString::fromUtf8(rule->getComment().c_str());

    QRect r = ruleDelegate->cellRect(row,col);

    int x  = r.left() + RuleElementSpacing/2;
    int y  = r.top();
    QRect br = p.boundingRect(QRect(x, y, 1000, 1000),
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

QString RuleSetView::chooseIcon(const QString &_icn)
{
    QString icn = _icn;
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

QString RuleSetView::getPlatform()
{
    FWObject *f = getFirewall();
    if (f)
        return  f->getStr("platform").c_str();
    return "";
}


Rule* RuleSetView::getRule(int row)
{
    return Rule::cast( ruleIndex[row] );
}

int RuleSetView::getRow(Rule *rule)
{
    int row = 0;
    if (rule)
    {
        for (; row < rowsInfo.size(); row++)
        {
            Rule *r = getRule(row);
            if (r && rule->getId()==r->getId()) break;
        }
    }
    return row;
}


void RuleSetView::selectRE( int row, int col)
{
    if (fwbdebug)
        qDebug("RuleSetView::selectRE  row=%d col=%d", row, col);

    m_project->selectRules();

    if (row!=currentRow() || col!=currentColumn())
    {
        selectedObject = NULL;
        setCurrentCell(row, col);
        QTimer::singleShot(0, this, SLOT(scrollToCurrent()));
    }
}

void RuleSetView::selectRE(FWReference *ref)
{
    /* need to find row and column this object is in and show it */
    RuleElement *re = RuleElement::cast(ref->getParent());
    assert(re);
    selectRE(re, ref->getPointer());
}

void RuleSetView::selectRE(Rule *rule, int col)
{
    QString group_name = QString(rule->getRuleGroupName().c_str());
    int row = getRow(rule);
    if (fwbdebug)
        qDebug("RuleSetView::selectRE  rule: id=%s type=%s, "
               "row=%d col=%d group=%s",
               FWObjectDatabase::getStringId(rule->getId()).c_str(),
               rule->getTypeName().c_str(),
               row, col, group_name.toAscii().constData());

    expandRuleGroupByName(group_name);
    selectRE(row, col);
}

void RuleSetView::selectRE(RuleElement *re, FWObject *obj)
{
    Rule *rule = Rule::cast(re->getParent());
    assert(rule!=NULL);
    int col=0;
    for (col=0; col<ncols; ++col)
        if (re==getRE(rule,col)) break;

    selectRE(rule, col);
    setSelectedObject(obj);
}

void RuleSetView::itemDoubleClicked(const QModelIndex & index)
{
    if (!index.isValid()) return;
    if (index.row()<0) return;
    editSelected();
}

void RuleSetView::selectionChanged(const QItemSelection &,
                                   const QItemSelection &)
{
    if (selectionModel()->selection().empty())
    {
        firstSelectedRow = -1;
        lastSelectedRow = -1;

        //unselect();
        setCurrentIndex(QModelIndex());

        m_project->unselectRules();

        return;
    }

    itemSelectionRange = selectionModel()->selection()[0];

    firstSelectedRow = itemSelectionRange.top();
    lastSelectedRow = itemSelectionRange.bottom();

    if (fwbdebug)
        qDebug("RuleSetView::selectionChanged New selection rows: %d - %d",
               firstSelectedRow, lastSelectedRow);

    if (lastSelectedRow < firstSelectedRow)
    {
        int i = lastSelectedRow;
        lastSelectedRow = firstSelectedRow;
        firstSelectedRow = i;
    }

    int selectionSize = lastSelectedRow-firstSelectedRow+1;

    mw->m_mainWindow->editCopyAction->setEnabled(true);
    mw->m_mainWindow->editCutAction->setEnabled(true);
    mw->m_mainWindow->editPasteAction->setEnabled(true);
    mw->m_mainWindow->editDeleteAction->setEnabled(true);
    mw->m_mainWindow->moveRuleAction->setEnabled( true );
    mw->m_mainWindow->removeRuleAction->setEnabled( true );
    mw->m_mainWindow->addRuleAfterCurrentAction->setEnabled( true );

    mw->m_mainWindow->copyRuleAction->setEnabled( selectionSize==1 );
    mw->m_mainWindow->cutRuleAction->setEnabled( selectionSize==1 );
    mw->m_mainWindow->pasteRuleAboveAction->setEnabled( selectionSize==1 );
    mw->m_mainWindow->pasteRuleBelowAction->setEnabled( selectionSize==1 );

    m_project->selectRules();
}

void RuleSetView::adjustColumn( int col )
{
    QString lbl = ruleModel->headerData(col,
                                        Qt::Horizontal,
                                        Qt::DisplayRole).toString();

    QFontMetrics p(st->getRulesFont());//(this);
    QRect br = p.boundingRect(QRect(0, 0, 1000, 1000),
                              Qt::AlignLeft|Qt::AlignVCenter,
                              lbl );

    int w = br.width() + 10;

    int row = 0;
    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++,row++)
    {
        QRect cr = calculateCellSize(row,col);
        w = QMAX(w,cr.width());
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

/**
 *  insert rule at given position. 
 */
Rule* RuleSetView::insertRule(Rule *next_to_rule, insertRuleOp rule_op,
                              FWObject *old_rule)
{
    int table_row = getRow(next_to_rule);
    int next_to_position = (next_to_rule) ? next_to_rule->getPosition() : 0;

    if (fwbdebug)
    {
        qDebug("RuleSetView::insertRule next_to_rule=%p next_to_position=%d "
               "rule_op=%d table_row=%d ruleset->getRuleSetSize()=%d "
               "rowsInfo.size()=%d ruleIndex.size()=%d ",
               next_to_rule, next_to_position, rule_op, table_row,
               ruleset->getRuleSetSize(), rowsInfo.size(), ruleIndex.size());
        debugPrintRows();
    }

    if (old_rule!=NULL &&
        ruleset->getTypeName()==Policy::TYPENAME &&
        old_rule->getTypeName()!=PolicyRule::TYPENAME)  return NULL;
    if (old_rule!=NULL &&
        ruleset->getTypeName()==NAT::TYPENAME    &&
        old_rule->getTypeName()!=NATRule::TYPENAME   )  return NULL;

    if (table_row<0) table_row = 0;

    int new_table_row = table_row;
    list<int> old_group_header_rows;

    Rule *newrule = NULL;
    if ( ruleset->getRuleSetSize()==0 || next_to_rule==NULL)
    {
        // inserting into an empty ruleset or at the top of any ruleset
        // if next_to_rule==NULL
        newrule = ruleset->insertRuleAtTop();
        new_table_row = table_row;
    } else
    {
        switch (rule_op)
        {
        case appendAfter:
            newrule = ruleset->appendRuleAfter(next_to_position);
            new_table_row = table_row + 1;
            break;

        case insertBefore:
            newrule = ruleset->insertRuleBefore(next_to_position);
            new_table_row = table_row;
            break;
        }
    }
    assert(newrule!=NULL);

    if (fwbdebug)
        qDebug("RuleSetView::insertRule new_table_row=%d", new_table_row);

    for (int i=ruleIndex.size(); i>new_table_row; --i)  
    {
        if (ruleIndex[i] == NULL) old_group_header_rows.push_back(i);
        ruleIndex[i] = ruleIndex[i-1];
    }
    ruleIndex[new_table_row] = newrule;

    initRule(newrule, Rule::cast(old_rule));
    if (next_to_rule!=NULL)
    {
        string group_name = next_to_rule->getRuleGroupName();
        newrule->setRuleGroupName(group_name);
    }

    for (int i=ruleIndex.size()-1; i>=table_row; --i)
        setRuleNumber(i, ruleIndex[i]);

    ruleModel->insertRow(table_row);
    rowsInfo.insert(table_row, NULL);

    adjustRow(table_row);
    adjustRow(new_table_row);

    for (list<int>::iterator i=old_group_header_rows.begin(); 
         i!=old_group_header_rows.end(); ++i)
    {
        int row = *i;
        updateCell(row, 3);

//        for (int col=0; col<ncols; col++)
//        {
//            QModelIndex ind = ruleModel->index(row, col);
//            update(ind);
//        }
    }

    update();
    updateGroups();

    if (fwbdebug)
    {
        qDebug(" ");
        debugPrintRows();
    }

    setCurrentCell( table_row, currentColumn() );
    updateCell(table_row, currentColumn());
    mw->reloadAllWindowsWithFile(m_project);

    return newrule;
}

void RuleSetView::initRule(Rule *new_rule, Rule *old_rule)
{
    PolicyRule *newrule_as_policy_rule = PolicyRule::cast(new_rule);

    if (newrule_as_policy_rule)
    {
        newrule_as_policy_rule->setLogging(supports_logging);
        newrule_as_policy_rule->setAction(PolicyRule::Deny);
        newrule_as_policy_rule->setDirection(PolicyRule::Both);
        FWOptions *ruleopt = newrule_as_policy_rule->getOptionsObject();
        ruleopt->setBool("stateless",
                         getStatelessFlagForAction(newrule_as_policy_rule));
    }

    if (old_rule!=NULL)  copyRuleContent(new_rule, old_rule);
}

void RuleSetView::insertRule()
{
    if (!isTreeReadWrite(this,ruleset)) return;
    if (getFirewall()==NULL) return;

    changingRules = true;
    if (firstSelectedRow > -1)
    {
        Rule *current_rule_at_pos = ruleIndex[firstSelectedRow];
        insertRule(current_rule_at_pos, insertBefore, NULL);
        changeCurrentCell(firstSelectedRow+1, currentColumn(), true);
    }
    else
        insertRule(NULL, insertBefore, NULL);

    changingRules = false;
    mw->updateLastModifiedTimestampForOneFirewall(getFirewall());
}

void RuleSetView::addRuleAfterCurrent()
{
    if (getFirewall()==NULL) return;
    if (!hasFocus()) return;
    if (!isTreeReadWrite(this,ruleset)) return;

    changingRules = true;
    Rule *current_rule_at_pos = ruleIndex[lastSelectedRow];

    insertRule(current_rule_at_pos, appendAfter, NULL);
    changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    updateGroups();
}

void RuleSetView::insertRuleAboveFirstGroup()
{
    insertRule(NULL, insertBefore, NULL);
    updateGroups();
}

void RuleSetView::removeRule()
{
    if (getFirewall()==NULL) return;

    if (!hasFocus()) return;
    if (!isTreeReadWrite(this,ruleset)) return;

/* we call removeRule in a loop. Set flag changingRules to true to prevent
    * removeRule from calling updateLastModifiedTimestampForAllFirewalls each time
 */
    changingRules = true;

    m_project->findObjectWidget->reset();

    /* remove rules firstSelectedRow through lastSelectedRow */

    if ( firstSelectedRow!=-1 )
    {
        if (fwbdebug) qDebug("removeRule");

        setUpdatesEnabled(false);
        for (int rn=lastSelectedRow; rn>=firstSelectedRow; --rn)
        {
            if (ruleIndex.count(rn)==0 || ruleIndex[rn]==NULL) continue;

            int rule_n = ruleIndex[rn]->getPosition();
            if (fwbdebug) qDebug("rule #%d", rule_n);

            if (m_project->isEditorVisible() &&
                m_project->getOpenedEditor()==ruleIndex[rn])
                m_project->closeEditor();

            if ( ruleset->deleteRule(rule_n) )
            {
                if (fwbdebug) qDebug("ruleset removed rule successfully");

                int lastN = ruleIndex.size()-1;
                ruleIndex.remove(rn); //erase(rn);
                rowsInfo.remove(rn);
                for (int i=rn; i<lastN; ++i)   ruleIndex[i] = ruleIndex[i+1];

                for (int row=rn; row<lastN; ++row)
                    setRuleNumber(row, ruleIndex[row]);
            } else
            {
                if (fwbdebug) qDebug("ruleset failed to remove rule");
            }
        }

        ruleModel->removeRows(firstSelectedRow, lastSelectedRow);
        
        setUpdatesEnabled(true);

        clearSelection();

        setCurrentCell( firstSelectedRow, currentColumn() );
        update();
        updateGroups();
        changingRules = false;
        m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
        mw->reloadAllWindowsWithFile(m_project);
    }
}

FWObject* RuleSetView::getSelectedObject()
{
    return selectedObject;
}

void RuleSetView::copyRuleContent(Rule *dst, Rule *src)
{
    int id = dst->getId();
    int p = dst->getPosition();

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

    if (id > -1) dst->setId(id);
    dst->setPosition(p);
}

FWObject* RuleSetView::getObj(int row, int col, int mouse_y_pos, QRect *objr)
{
    RuleElement *re = getRE(row,col);
    if (re==NULL) return NULL;

    QRect cr = ruleDelegate->cellGeometry(row,col);

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
    int row = rowAt(ev->y());
    int col = columnAt(ev->x());
    if (row == -1) return ;    
    if (col == 0 && row < rowsInfo.size() && row >= 0 && rowsInfo[row] != NULL)
        showHideRuleGroup(row);

    FWObject *obj = getObj(row, col, ev->y()+verticalOffset());

    QTableView::mousePressEvent(ev);

    if (fwbdebug)
        qDebug("RuleSetView::contentsMousePressEvent  "
               "obj=%s  row=%d  col=%d",
               (obj)?obj->getName().c_str():"NULL", row, col);

    if ((row > 0) && (col > 0)) mw->selectRules();
    else
    {
        mw->unselectRules();
        setCurrentIndex(QModelIndex());
        //unselect();
    }

    startingDrag = (row==currentRow() &&
                    col==currentColumn() &&
                    selectedObject==obj);

    //forget old selection by setting View's current cell
    changeCurrentCell(row, col, true);
    setSelectedObject(obj);
}

void RuleSetView::mouseReleaseEvent( QMouseEvent* ev )
{
    QTableView::mouseReleaseEvent(ev);

    if (fwbdebug) qDebug("RuleSetView::contentsMouseReleaseEvent");

    if (ruleset->size()!=0 &&
        mw->isEditorVisible() && !switchObjectInEditor( columnAt(ev->x()) ))
    {
        ev->accept();
    };
}

void RuleSetView::setSelectedObject(FWObject* obj)
{
    if (fwbdebug)
        qDebug("RuleSetView::setSelectedObject  obj='%s' "
               "currentRow()=%d currentColumn()=%d",
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

void RuleSetView::removeRuleIndex(int idx)
{
    if (fwbdebug)
        qDebug("RuleSetView::removeRuleIndex idx=%d  ruleIndex.size()=%d",
               idx, ruleIndex.size());

    for ( int i=idx; i<ruleIndex.size()-1; ++i) 
        ruleIndex[i] = ruleIndex[i+1];

    if (fwbdebug) qDebug("RuleSetView::removeRuleIndex done");

 //   ruleIndex.remove (ruleIndex.size());
}

int RuleSetView::getUpNullRuleIndex(int idx)
{
    for (int i=idx; i>=0; --i) 
    { 
        if (rowsInfo[i]!=NULL)  return i;
    }
    return -1 ;
}

int RuleSetView::getDownNullRuleIndex(int idx)
{

    for ( int i=idx; i<rowsInfo.size(); ++i) 
    { 
        if (rowsInfo[i]!=NULL) return i;
    }
    return -1;
}


void RuleSetView::openObjectInTree(FWObject *obj)
{
    if (gui_experiment1) return;

    FWObject *oo = obj;
    if (obj==NULL || Rule::cast(obj)!=NULL)  oo = getFirewall();
    if (oo==NULL) return;

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
    if (!isTreeReadWrite(this,ruleset)) return;

    bool ok = false ;
    QString text = QInputDialog::getText(
        this, tr(""), tr("Enter group name:"),
        QLineEdit::Normal, "New Group", &ok);

    if (ok && !text.isEmpty())
    {
        QString postfix = "";
        int counter = 0 ;
        for (int i =0 ; i < rowsInfo.size(); i++)
        {
            RuleRowInfo * rri = rowsInfo[i];
            if (rri!=NULL)
            {
                if (text+postfix == rri->groupName)
                {
                    counter++;
                    postfix = "-"+QString().setNum(counter);
                }
            }
        }
        int row = firstSelectedRow;
        int count = lastSelectedRow - firstSelectedRow +1;
        createGroup(row, count, text+postfix);   
    } 
}

void RuleSetView::createGroup(int row, int count, const QString &groupName)
{
    if (!isTreeReadWrite(this,ruleset)) return;

    for (int idx=0 ; idx<count; idx++)
    {
        Rule *rule = ruleIndex[row + idx];
        if (rule)
            rule->setRuleGroupName(groupName.toUtf8().data());
    }
    // Note that ProjectPanel::reopenFirewall destroys all RuleSetView
    // objects and creates new ones. Save stored inside RuleSetView
    // object does not survive call to reopenFirewall()
    QTimer::singleShot(0, m_project, SLOT(reopenFirewall()) );
}

void RuleSetView::renameGroup()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    bool ok = false ;
    QString oldGroup = "";
    if (rowsInfo[ firstSelectedRow ] != NULL)
    {
        oldGroup = rowsInfo[ firstSelectedRow  ]->groupName;
    }
    
    QString text = QInputDialog::getText(
        this, "Rename group",
        tr("Enter group name:"), QLineEdit::Normal,
        oldGroup, &ok);
    if (ok && !text.isEmpty())
    {
        if (oldGroup=="" || text=="") return ;
        QString postfix = "";
        for (int i =0 ; i < rowsInfo.size(); i++)
        {
            Rule * r = ruleIndex[i];
            if (r!=NULL)
            {
                if (QString::fromUtf8(r->getRuleGroupName().c_str()) ==
                    oldGroup)
                {
                    r->setRuleGroupName(text.toUtf8().data());
                }
            }
        }
        updateGroups();
    } 
}


void RuleSetView::addToGroupAbove ()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    int row = firstSelectedRow;
    int count = lastSelectedRow - firstSelectedRow +1;
    int top = getUpNullRuleIndex(row);
    RuleRowInfo *ru = rowsInfo[top];

    if (fwbdebug)
        qDebug("RuleSetView::addToGroupAbove  row=%d count=%d", row, count);

    if (!ru->isFirstRow)
    {
        top = getUpNullRuleIndex(top-1);
        ru = rowsInfo[top];
    }

    for (int i = 0; i< count ; i++)
    {
        Rule *r = ruleIndex[row+i];
        if (r)
        {
            r->setRuleGroupName(ru->groupName.toUtf8().data());
            //ruleIndex[row+i] =r ;
            if (ru->collapsedGroup) showHideRuleGroup(row);
        }
    }
    QTimer::singleShot(0, m_project, SLOT(reopenFirewall()));

//    updateGroups();
}

void RuleSetView::addToGroupBelow()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    int row = firstSelectedRow;
    int bottom = getDownNullRuleIndex(row);
    RuleRowInfo *ru = rowsInfo[bottom];
    int count = lastSelectedRow - firstSelectedRow +1;

    if (fwbdebug)
        qDebug("RuleSetView::addToGroupBelow  row=%d", row);

    for (int i = 0; i< count ; i++)
    {
        Rule *r = ruleIndex[row+i];
        if (r)
        {
            r->setRuleGroupName (ru->groupName.toUtf8().data());
            //ruleIndex[row+i] =r ;
            if (ru->collapsedGroup) showHideRuleGroup(row);
        }
    }
    QTimer::singleShot( 0, m_project, SLOT(reopenFirewall()) );

//    updateGroups();
}

void RuleSetView::showHideRuleGroup(int row)
{
    if (fwbdebug) qDebug("RuleSetView::showHideRuleGroup  row=%d", row);

    RuleRowInfo *rri = rowsInfo[row];
    if (rri)
    {
        if (rri->collapsedGroup) expandRuleGroup(row);
        else collapseRuleGroup(row);
    }
    updateGroups();   
}

void RuleSetView::collapseRuleGroup(int row)
{
    RuleRowInfo *rrf = rowsInfo[row];
    int bottom = getDownNullRuleIndex(row+1);

    if (fwbdebug) qDebug("RuleSetView::collapseRuleGroup row=%d bottom=%d",
                         row, bottom);
    rrf->collapsedGroup = true;

    for (int ridx = bottom-1; ridx > row; ridx--)
    {
        hideRow(ridx);
    }
    dataChanged(model()->index(row, 0),
                model()->index(model()->rowCount(), model()->columnCount()));

    saveCollapsedGroups();
}

void RuleSetView::expandRuleGroup(int row)
{
    RuleRowInfo *rrf = rowsInfo[row];
    int bottom = getDownNullRuleIndex(row+1);

    if (fwbdebug) qDebug("RuleSetView::expandRuleGroup row=%d bottom=%d",
                         row, bottom);
    rrf->collapsedGroup = false;

    for (int i = row+1; i< bottom; i++)
    {
        showRow(i);
        adjustRow(i);
    }
    dataChanged(model()->index(row, 0),
                model()->index(model()->rowCount(), model()->columnCount()));

    saveCollapsedGroups();
}

void RuleSetView::removeFromGroup()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    int row = firstSelectedRow;
    int count = lastSelectedRow - firstSelectedRow+1;

    removeFromGroup(row,count);
}

void RuleSetView::removeFromGroup(int row, int count)
{
    if (!isTreeReadWrite(this,ruleset)) return;

    for (int i = row ; i < row+count ; i++)
    {
        Rule * r = ruleIndex[i];
        if (r!=NULL && !r->isReadOnly())
        {
            r->setRuleGroupName("");
            ruleIndex[i]=r;
        }
    }
    QTimer::singleShot( 0, m_project, SLOT(reopenFirewall()) );
}


QString RuleSetView::getGroupColorForRule(Rule *rule)
{
    QString group = QString::fromUtf8(rule->getRuleGroupName().c_str());
    return groupColors[group];
}

/*
 * Collapse rule group identified by name. Since group name does not
 * have to be unique, find and collapse all groups with the same name.
 */ 
void RuleSetView::collapseRuleGroupByName(const QString &name)
{
    for (int i = 0 ; i < rowsInfo.size(); i++)
        if (rowsInfo[i] && rowsInfo[i]->groupName == name) 
            collapseRuleGroup(i);
}

/*
 * Expand rule group identified by name. Since group name does not
 * have to be unique, find and expand all groups with the same name.
 */ 
void RuleSetView::expandRuleGroupByName(const QString &name)
{
    for (int i = 0 ; i < rowsInfo.size(); i++)
        if (rowsInfo[i] && rowsInfo[i]->groupName == name) 
            expandRuleGroup(i);
}

void RuleSetView::contextMenu(int row, int col, const QPoint &pos)
{
    if (getFirewall()==NULL) return;

    if (col > 0)
        setCurrentCell(row,col);
    else
        setCurrentCell(row,0);

    //if the row is not selected actually, we select it
    if ((row < firstSelectedRow) || (row > lastSelectedRow))
    {
        selectRow(row);
        firstSelectedRow = row;
        lastSelectedRow = row;
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

    QMenu *popup = new QMenu(this);
    Rule *r = ruleIndex[row];
    if (r!=NULL)
    {
        if (r->getRuleGroupName()=="")
        {
            //qDebug ((QString("").setNum(firstSelectedRow)).toAscii().data());
            //qDebug ((QString("").setNum(lastSelectedRow)).toAscii().data());

            popup->addAction( tr("New group"), this, SLOT( newGroup() ));
            int top = getUpNullRuleIndex(firstSelectedRow);
            if (top != -1 && top == (firstSelectedRow - 1))
            {
                RuleRowInfo * ri= rowsInfo[top];
                QString label = tr("Add to the group ");
                label += ri->groupName;
                popup->addAction( label, this, SLOT( addToGroupAbove() ));
            }
            int bottom = getDownNullRuleIndex(lastSelectedRow);
            if (bottom != -1 && bottom == (lastSelectedRow + 1))
            {
                RuleRowInfo * ri= rowsInfo[bottom];
                QString label = tr("Add to the group ");
                label += ri->groupName;
                popup->addAction( label, this, SLOT( addToGroupBelow() ));
          
            }
        }
        else
        {
            // Check if the rule is in the middle of a group.
            // Note that rules above or below may belong to another group,
            // in which case we provide "remove from group" menu item.
            Rule *rule_above = getRule(row - 1);
            Rule *rule_below = getRule(row + 1);
            if (rule_above == NULL || rule_below == NULL ||
                r->getRuleGroupName() != rule_above->getRuleGroupName() ||
                r->getRuleGroupName() != rule_below->getRuleGroupName())
            {
                popup->addAction( tr("Remove from the group"), this,
                                  SLOT( removeFromGroup() ));
            }
        }
        popup->addSeparator ();
    }
    else
    {
        if (row==0)
            popup->addAction( tr("Insert Rule Above"), this,
                              SLOT( insertRuleAboveFirstGroup() ));

        popup->addAction( tr("Rename group"), this, SLOT( renameGroup() ));
        popup->addSeparator();

        QMenu *subcolor = popup->addMenu(tr("Change color") );

        QPixmap pcolor(16,16);
        pcolor.fill(QColor(255,255,255));
        subcolor->addAction(QIcon(pcolor), tr("No color"),
                            this, SLOT(setColorEmpty() ));

        pcolor.fill(st->getLabelColor(FWBSettings::RED));
        subcolor->addAction(QIcon(pcolor),
                            st->getLabelText(FWBSettings::RED),
                            this, SLOT(setColorRed() ));

        pcolor.fill(st->getLabelColor(FWBSettings::ORANGE));
        subcolor->addAction(QIcon(pcolor),
                            st->getLabelText(FWBSettings::ORANGE),
                            this, SLOT(setColorOrange() ));

        pcolor.fill(st->getLabelColor(FWBSettings::YELLOW));
        subcolor->addAction(QIcon(pcolor),
                            st->getLabelText(FWBSettings::YELLOW),
                            this, SLOT(setColorYellow() ));

        pcolor.fill(st->getLabelColor(FWBSettings::GREEN));
        subcolor->addAction(QIcon(pcolor),
                            st->getLabelText(FWBSettings::GREEN),
                            this, SLOT(setColorGreen() ));

        pcolor.fill(st->getLabelColor(FWBSettings::BLUE));
        subcolor->addAction(QIcon(pcolor),
                            st->getLabelText(FWBSettings::BLUE),
                            this, SLOT(setColorBlue() ));

        pcolor.fill(st->getLabelColor(FWBSettings::PURPLE));
        subcolor->addAction(QIcon(pcolor),
                            st->getLabelText(FWBSettings::PURPLE),
                            this, SLOT(setColorPurple() ));

        pcolor.fill(st->getLabelColor(FWBSettings::GRAY));
        subcolor->addAction(QIcon(pcolor),
                            st->getLabelText(FWBSettings::GRAY),
                            this, SLOT(setColorGray() ));

        popup->exec(pos );

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
                                  action_name,
                                  this, SLOT( changeActionToAccept() ));
            }
            if (Resources::isTargetActionSupported(platform,"Deny"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Deny,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Deny")),
                                  action_name,
                                  this, SLOT( changeActionToDeny() ));
            }
            if (Resources::isTargetActionSupported(platform,"Reject"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Reject,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Reject")),
                                  action_name,
                                  this, SLOT( changeActionToReject() ));
            }
            if (Resources::isTargetActionSupported(platform,"Accounting"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Accounting,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Accounting")),
                                  action_name,
                                  this, SLOT( changeActionToAccounting() ));
            }
            if (Resources::isTargetActionSupported(platform,"Pipe"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Pipe,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Pipe")),
                                  action_name,
                                  this, SLOT( changeActionToPipe() ));
            }
            if (Resources::isTargetActionSupported(platform,"Tag"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Tag,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Tag")),
                                  action_name,
                                  this, SLOT( changeActionToTag() ));
            }
            if (Resources::isTargetActionSupported(platform,"Classify"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Classify,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Classify")),
                                  action_name,
                                  this, SLOT( changeActionToClassify() ));
            }
            if (Resources::isTargetActionSupported(platform,"Custom"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Custom,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Custom")),
                                  action_name,
                                  this, SLOT( changeActionToCustom() ));
            }
            if (Resources::isTargetActionSupported(platform,"Branch"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Branch,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Branch")),
                                  action_name,
                                  this, SLOT( changeActionToBranch() ));
            }
            if (Resources::isTargetActionSupported(platform,"Route"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Route,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Route")),
                                  action_name,
                                  this, SLOT( changeActionToRoute() ));
            }
            if (Resources::isTargetActionSupported(platform,"Continue"))
            {
                action_name = getActionNameForPlatform(PolicyRule::Continue,
                        platform.c_str());
                popup->addAction( QIcon(LoadPixmap(":/Icons/Continue")),
                                  action_name,
                                  this, SLOT( changeActionToContinue() ));
            }

            popup->addSeparator ();
            QAction *paramID;
            paramID = popup->addAction( tr("Parameters"),
                                        this, SLOT( editSelected() ));

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
                              tr("Inbound"),
                              this, SLOT( changeDirectionToIn() ));
            popup->addAction( QIcon(LoadPixmap(":/Icons/Outbound")),
                              tr("Outbound"),
                              this, SLOT( changeDirectionToOut() ));
            popup->addAction( QIcon(LoadPixmap(":/Icons/Both")),
                              tr("Both"),
                              this, SLOT( changeDirectionToBoth() ));

            break;
        }
        case Options:
        {
            popup->addAction( QIcon(LoadPixmap(":/Icons/Options")),
                              tr("Rule Options"),
                              this, SLOT( editSelected() ));

            if (fwbdebug) qDebug(ruleset->getTypeName().c_str());
            if (ruleset->getTypeName() == Policy::TYPENAME) {

                popup->addAction( QIcon(LoadPixmap(":/Icons/Log")),
                                  tr("Logging On"),
                                  this, SLOT( changeLogToOn() ));
                popup->addAction( QIcon(LoadPixmap(":/Icons/Blank")),
                                  tr("Logging Off"),
                                  this, SLOT( changeLogToOff() ));
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
            if (Policy::cast(ruleset)!=NULL) cap_name="negation_in_policy";
            if (NAT::cast(ruleset)!=NULL) cap_name="negation_in_nat";

            Firewall *f = getFirewall();

            bool supports_neg=false;
            try  {
                supports_neg = Resources::getTargetCapabilityBool(
                    f->getStr("platform"), cap_name);
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

            Rule *rule = ruleIndex[row];
            if (rule==NULL)
            {
                //popup->addAction( tr("Insert Rule"), this, SLOT( insertRule() ) );
            } else
            {
                //int rn = rule->getPosition();
                int selectionSize=lastSelectedRow-firstSelectedRow+1;

                if (lastSelectedRow > firstSelectedRow)
                {
                    int first_selected_rule_num = -1;
                    int last_selected_rule_num = -1;
                    for (int r=firstSelectedRow; r <= lastSelectedRow; ++r)
                    {
                        Rule *selected_rule = getRule(r);
                        if (selected_rule)
                        {
                            int n = selected_rule->getPosition();
                            if (first_selected_rule_num < 0)
                                first_selected_rule_num = n;
                            last_selected_rule_num = n;
                        }
                    }
                    popup->addAction( tr("Rules %1-%2").
                                      arg(first_selected_rule_num).
                                      arg(last_selected_rule_num) )->setEnabled(false);
                } else
                    popup->addAction( tr("Rule %1").
                                      arg(rule->getPosition()))->setEnabled(false);

                QMenu *subcolor = popup->addMenu( tr("Change color") );

                QPixmap pcolor(16,16);
                pcolor.fill(QColor(255,255,255));
                subcolor->addAction( QIcon(pcolor), tr("No color"),
                                     this, SLOT ( setColorEmpty() ));

                pcolor.fill(st->getLabelColor(FWBSettings::RED));
                subcolor->addAction( QIcon(pcolor),
                                     st->getLabelText(FWBSettings::RED),
                                     this, SLOT ( setColorRed() ));

                pcolor.fill(st->getLabelColor(FWBSettings::ORANGE));
                subcolor->addAction( QIcon(pcolor),
                                     st->getLabelText(FWBSettings::ORANGE),
                                     this, SLOT ( setColorOrange() ));

                pcolor.fill(st->getLabelColor(FWBSettings::YELLOW));
                subcolor->addAction( QIcon(pcolor),
                                     st->getLabelText(FWBSettings::YELLOW),
                                     this, SLOT ( setColorYellow() ));

                pcolor.fill(st->getLabelColor(FWBSettings::GREEN));
                subcolor->addAction( QIcon(pcolor),
                                     st->getLabelText(FWBSettings::GREEN),
                                     this, SLOT ( setColorGreen() ));

                pcolor.fill(st->getLabelColor(FWBSettings::BLUE));
                subcolor->addAction( QIcon(pcolor),
                                     st->getLabelText(FWBSettings::BLUE),
                                     this, SLOT ( setColorBlue() ));

                pcolor.fill(st->getLabelColor(FWBSettings::PURPLE));
                subcolor->addAction( QIcon(pcolor),
                                     st->getLabelText(FWBSettings::PURPLE),
                                     this, SLOT ( setColorPurple() ));

                pcolor.fill(st->getLabelColor(FWBSettings::GRAY));
                subcolor->addAction( QIcon(pcolor),
                                     st->getLabelText(FWBSettings::GRAY),
                                     this, SLOT ( setColorGray() ));

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
                Rule *r =  ruleIndex[row];
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

    if (ruleIndex[firstSelectedRow]==NULL)
    {
        if (fwbdebug)
            qDebug("RuleSetView::setRuleColor changing color for group c=%s",
                   c.toAscii().constData());
        RuleRowInfo * rri = rowsInfo[firstSelectedRow];
        rri->color = c;
        if (rri->color=="") rri->color="#FFFFFF";
        // Find first rule of the group
        Rule *rule = NULL;
        for (int row = firstSelectedRow; row < ruleIndex.size() && rule==NULL;
             row++)
        {
            rule = ruleIndex[row];
        }
        if (rule)
        {
            FWOptions *ropt = rule->getOptionsObject();
            ropt->setStr("color", c.toLatin1().constData());
            updateGroups();
        }
    } else
    {
        if ( firstSelectedRow!=-1 )
        {
            if (fwbdebug)
                qDebug("RuleSetView::setRuleColor changing color for rule c=%s",
                       c.toAscii().constData());

            for (int i=firstSelectedRow; i<=lastSelectedRow; ++i)
            {
                Rule *rule = ruleIndex[i];
                if (rule!=NULL)
                {
                    FWOptions *ropt = rule->getOptionsObject();
                    ropt->setStr("color", c.toLatin1().constData());

                    // need to call dataChanged to trigger row repaint.
                    // At least on Mac OS X with QT 4.4.1 calling update()
                    // was insufficient.
                    dataChanged(model()->index(i, 0),
                                model()->index(i, model()->columnCount()));
                    adjustRow(i);
                }
            }
        }
    }

    update();
    mw->reloadAllWindowsWithFile(m_project);
}

void RuleSetView::changeAction(PolicyRule::Action act)
{
    if (getFirewall()==NULL) return;

    if (!isTreeReadWrite(this, ruleset)) return;

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
            rule->setAction( act );
            if (!changingRules)
                mw->updateLastModifiedTimestampForOneFirewall(getFirewall());
        }

        ruleopt->setBool("stateless", getStatelessFlagForAction(rule));

        mw->actionChangedEditor(rule);
        mw->reloadAllWindowsWithFile(m_project);
    }
}

void RuleSetView::changeActionToAccept()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Accept );
}

void RuleSetView::changeActionToDeny()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Deny );
}

void RuleSetView::changeActionToReject()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Reject );
}

void RuleSetView::changeActionToAccounting()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Accounting );
}

void RuleSetView::changeActionToPipe()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Pipe );
}

void RuleSetView::changeActionToTag()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Tag );
}

void RuleSetView::changeActionToClassify()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Classify );
}

void RuleSetView::changeActionToCustom()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Custom );
}

void RuleSetView::changeActionToRoute()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Route );
}

void RuleSetView::changeActionToContinue()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    changeAction( PolicyRule::Continue );
}

void RuleSetView::changeActionToBranch()
{
    if (!isTreeReadWrite(this,ruleset)) return;
    changeAction( PolicyRule::Branch );
}

void RuleSetView::changeDitection(PolicyRule::Direction dir)
{
    if (getFirewall()==NULL) return;

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
            mw->reloadAllWindowsWithFile(m_project);
        }
    }
}

void RuleSetView::changeDirectionToIn()
{
    changeDitection( PolicyRule::Inbound );
}

void RuleSetView::changeDirectionToOut()
{
    changeDitection( PolicyRule::Outbound );
}

void RuleSetView::changeDirectionToBoth()
{
    changeDitection( PolicyRule::Both );
}

void RuleSetView::changeLogToOn()
{
    if (getFirewall()==NULL) return;

    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        PolicyRule *rule = PolicyRule::cast( ruleIndex[currentRow()] );
        if (!changingRules)
            mw->updateLastModifiedTimestampForOneFirewall(getFirewall());
        rule->setLogging( true );
        mw->reloadAllWindowsWithFile(m_project);
    }
}

void RuleSetView::changeLogToOff()
{
    if (getFirewall()==NULL) return;

    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        PolicyRule *rule = PolicyRule::cast( ruleIndex[currentRow()] );
        if (!changingRules)
            mw->updateLastModifiedTimestampForOneFirewall(getFirewall());
        rule->setLogging( false );
        mw->reloadAllWindowsWithFile(m_project);
    }
}

void RuleSetView::copySelectedObject()
{
    if ( selectedObject!=NULL)
    {
        FWObjectClipboard::obj_clipboard->clear();
        FWObjectClipboard::obj_clipboard->add( selectedObject, m_project );
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
    if (getFirewall()==NULL) return;

    RuleElement *re = getRE(row,col);
    if (re==NULL || re->isAny()) return;
    int id = obj->getId();

    if (fwbdebug)
    {
        qDebug("RuleSetView::deleteObject row=%d col=%d id=%s",
               row, col, FWObjectDatabase::getStringId(id).c_str());
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
    mw->reloadAllWindowsWithFile(m_project);
}

bool RuleSetView::insertObject(int row, int col, FWObject *obj)
{
    if (getFirewall()==NULL) return false;

    if (fwbdebug)
        qDebug("RuleSetView::insertObject  -- insert object %s",
               obj->getName().c_str());
    
    if (!isTreeReadWrite(this,ruleset)) return false;

    if (getColType(col)!=Object && getColType(col)!=Time) return false;

    RuleElement *re = getRE(row,col);
    assert (re!=NULL);

    if (fwbdebug) qDebug("RuleSetView::insertObject  -- validate");

    if (! re->validateChild(obj) )
    {
        if (fwbdebug)qDebug("RuleSetView::insertObject  -- validation failed");

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

    if (fwbdebug) qDebug("RuleSetView::insertObject  -- check for duplicates");

    if ( !re->isAny())
    {
        /* avoid duplicates */
        int cp_id = obj->getId();
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

    if (fwbdebug) qDebug("RuleSetView::insertObject  -- add reference");

    re->addRef(obj);

    if (fwbdebug)
        qDebug("RuleSetView::insertObject  -- adjust and update table cell");

    adjustColumn(col);
    adjustRow(row);
    updateCell(row, col);
    mw->reloadAllWindowsWithFile(m_project);

    if (!changingRules)
        m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    return true;
}

void RuleSetView::pasteObject()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    vector<std::pair<int,ProjectPanel*> >::iterator i;
    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        ProjectPanel *proj_p = i->second;
        FWObject *co = proj_p->db()->findInIndex(i->first);

        if (Rule::cast(co)!=NULL)  pasteRuleAbove();
        else
        {
            if (currentRow()>=0)
            {
                bool need_to_reload_tree = false;
                if (proj_p!=m_project)
                {
                    // object is being copied from another project file
                    FWObject *target = m_project->getFWTree()->getStandardSlotForObject(
                        ruleset->getLibrary(), co->getTypeName().c_str());

                    map<int,int> map_ids;
                    co = m_project->db()->recursivelyCopySubtree(
                        target, co, map_ids);
                    need_to_reload_tree = true;
                }
                insertObject(currentRow(),currentColumn(),co);

                if (need_to_reload_tree)
                {
                    m_project->m_panel->om->loadObjects();
                    m_project->m_panel->om->openObject(co);
                    // but still need to reopen this ruleset
                    m_project->m_panel->om->openObject(ruleset);
                }
            }
        }
    }

    mw->reloadAllWindowsWithFile(m_project);

/*
    if (FWObjectClipboard::obj_clipboard->getObject()!=NULL)
    insertObject(currentRow(),currentColumn(),
    FWObjectClipboard::obj_clipboard->getObject() );
*/
}

void RuleSetView::negateRE()
{
    if (getFirewall()==NULL) return;

    if (!isTreeReadWrite(this,ruleset)) return;

    if ( currentRow()!=-1 && currentColumn()!=-1 )
    {
        RuleElement *re = getRE(currentRow(),currentColumn());
        if (re==NULL) return;
        re->toggleNeg();
        if (!changingRules)
            m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());

        updateCell(currentRow(),currentColumn());
        mw->reloadAllWindowsWithFile(m_project);
    }
}

QVector <ProjectPanel*> RuleSetView::getAllMdiProjectPanel ()
{
    QVector <ProjectPanel*> ret ;
    QList<QMdiSubWindow *> subWindowList ;
    if (mw!=NULL)
        subWindowList = mw->getMdiArea()->subWindowList();
    if (m_project == NULL || m_project->getRCS()==NULL)
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
    openPersistentEditor( ruleModel->index(currentRow(),currentColumn()) );
}

void RuleSetView::keyPressEvent( QKeyEvent* ev )
{
    if (getFirewall()==NULL) return;

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
            if ((currentRow()+1) >= int(ruleModel->rowCount()))
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

    updateCell(currentRow(), currentColumn());
    updateCell(oldRow, oldColumn);

    QTableView::keyPressEvent(ev);
}

QDrag* RuleSetView::dragObject()
{
    FWObject *obj = selectedObject;
    if (obj==NULL) return NULL;

    QString icn = (":/Icons/"+obj->getTypeName()+"/icon").c_str();

    list<FWObject*> dragobj;
    dragobj.push_back(obj);

    FWObjectDrag *drag = new FWObjectDrag(dragobj, this, NULL);

    QPixmap pm = LoadPixmap(icn);

    drag->setPixmap( pm );

    return drag;
}

void RuleSetView::dragEnterEvent( QDragEnterEvent *ev)
{
    if (fwbdebug) qDebug("RuleSetView::dragEnterEvent");
    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
}

void RuleSetView::dragMoveEvent( QDragMoveEvent *ev)
{
    QWidget *fromWidget = ev->source();

    // The source of DnD object must be the same instance of fwbuilder
    if (!fromWidget)
    {
        ev->setAccepted(false);
        return;
    }
   
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
         * v3.0: we do not permit d&d of an object from one data
         * file to another. mostly just to avoid confusing side
         * effects because such operation requires copy of all
         * object's dependencies. This means simple d&d operation
         * can in fact silently copy whole bunch of objects into
         * the tree, which is may not be even visible for the user
         * if parts of the tree are collapsed or obscured by other
         * windows.
         */

        if (FWObjectDrag::decode(ev, dragol))
        {
            for (list<FWObject*>::iterator i=dragol.begin();
                 i!=dragol.end(); ++i)
            {
                FWObject *dragobj = NULL;
                dragobj = dynamic_cast<FWObject*>(*i);
                if(dragobj!=NULL)
                {
                    acceptE &= (dragobj->getRoot()==ruleset->getRoot());
                    acceptE &= re->validateChild(dragobj);
                }
            }
            ev->setAccepted( acceptE );
            return;
        }
    }

    ev->setAccepted(false);
}


void RuleSetView::dropEvent(QDropEvent *ev)
{
    if (fwbdebug) qDebug("RuleSetView::dropEvent");

    QWidget *fromWidget = ev->source();

    // The source of DnD object must be the same instance of fwbuilder
    if (!fromWidget)
    {
        ev->setAccepted(false);
        return;
    }
   

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
        qDebug("RuleSetView::dropEvent  drop event mode=%d",
               ev->proposedAction());
        qDebug("                        src widget = %p", ev->source());
        qDebug("                              this = %p", this   );
    }

    list<FWObject*> dragol;
    if (!FWObjectDrag::decode(ev, dragol)) return;

    for (list<FWObject*>::iterator i=dragol.begin(); i!=dragol.end(); ++i)
    {
        FWObject *dragobj = *i;
        assert(dragobj!=NULL);

        if (fwbdebug) qDebug("RuleSetView::dropEvent dragobj=%s",
                             dragobj->getName().c_str());

        if (ev->source()!=this)
        {
            // since ev->source()!=this, this is d&d of an object from
            // the tree into rule.  Do not allow d&d from a different
            // file since it requires copying of the dependencies
            // which is a side-effect and might be confusing for the
            // user.
            if (ruleset->getRoot()!=dragobj->getRoot()) return;
            if (insertObject(row, col, dragobj))
                changeCurrentCell(row, col, true);
        } else
        {
            // since ev->source()==this, this is d&d of an object from
            // one rule to another.
            
            clearSelection();
            if (ev->keyboardModifiers() & Qt::ControlModifier)
            {
                if (insertObject(row, col, dragobj)) //copy
                    changeCurrentCell(row, col, true);
            }
            else //move
            {
                if (insertObject(row, col, dragobj) )
                {
                    deleteObject(oldRow, oldCol, dragobj);
                    changeCurrentCell(row, col, true);
                }
            }
        }
    }
    ev->accept();
}

void RuleSetView::moveRule()
{
    if (!isTreeReadWrite(this,ruleset)) return;

    if (getFirewall()==NULL) return;

    if (!hasFocus()) return;

    int selectionSize = lastSelectedRow - firstSelectedRow + 1;

    /* calculate acceptable range of rule numbers for the dialog */
    int minRN = 0;
    int maxRN = ruleset->size() - selectionSize;

    Ui::askRuleNumberDialog_q d;
    QDialog ddialog;
    d.setupUi(&ddialog);
    d.newRuleNum->setMinimum(minRN);
    d.newRuleNum->setMaximum(maxRN);

    if (ddialog.exec()==QDialog::Accepted)
    {
        int newN = d.newRuleNum->value();
        int nn   = newN;
        int old_first_rule_n = ruleIndex[firstSelectedRow]->getPosition();
        
        if (old_first_rule_n==nn) return;

        setUpdatesEnabled(false);

        /* nn is new rule number
         * old_first_rule_n is old rule number
         * Remember the difference between rule numbers and row numbers!
         */
        if (fwbdebug)
            qDebug("Moving rules #%d(+%d) to position %d",
                   old_first_rule_n, selectionSize, nn);

        if (old_first_rule_n > nn)
        {  // moving block of rules up
            for (int i=firstSelectedRow; i<=lastSelectedRow; i++)
            {
                int rule_n = ruleIndex[i]->getPosition();
                int current_rule_num = rule_n;

                if (fwbdebug)
                    qDebug("Row %d: Moving rule #%d up to position %d",
                           i, rule_n, nn);

                while (current_rule_num != nn)
                {
                    if (!ruleset->moveRuleUp(current_rule_num)) return;
                    --current_rule_num;
                }
                nn++;
            }
        } else
        {   // moving block of rules down

            for (int i=lastSelectedRow; i>=firstSelectedRow; i--)
            {
                int rule_n = ruleIndex[i]->getPosition();
                int current_rule_num = rule_n;

                if (fwbdebug)
                    qDebug("Row %d: Moving rule #%d down to position %d",
                           i, rule_n, nn+selectionSize-1);

                while (current_rule_num != nn+selectionSize-1)
                {
                    if (!ruleset->moveRuleDown(current_rule_num)) return;
                    ++current_rule_num;
                }
                nn--;
            }
        }

        setUpdatesEnabled(true);

        clearSelection();

        /* when we move rules, especially when we move a block of *
         rules and when rule groups are used, data structures
         ruleIndex, rowsInfo and other get really screwed up. This is
         because in the current design beginning and end of a rule
         group are indicated by NULL entries in ruleIndex, which was a
         bad idea. Under circumstances, it is easier to schedule
         complete refresh of the rule set view.
        */

        QTimer::singleShot( 0, m_project, SLOT(reopenFirewall()) );
        mw->reloadAllWindowsWithFile(m_project);

        //update();
        //updateGroups();
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
        Rule *r = ruleIndex[rn];
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
        Rule *r = ruleIndex[rn];
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

    if ( firstSelectedRow!=-1 )
    {
        FWObjectClipboard::obj_clipboard->clear();
        for (int i=firstSelectedRow; i<=lastSelectedRow; ++i)
        {
            FWObject *rule = getRule(i);

            if (fwbdebug)
                qDebug("Adding rule to clipboard, row=%d rule=%p", i, rule);

            if (rule)
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
    if (fwbdebug)
        qDebug("RuleSetView::pasteRuleAbove rowsInfo.size()=%d "
               "ruleIndex.size()=%d firstSelectedRow=%d",
               rowsInfo.size(), ruleIndex.size(), firstSelectedRow);

    if (getFirewall()==NULL) return;
    if (!isTreeReadWrite(this,ruleset)) return;

    changingRules = true;

    /* pick rules in reverse order */
    vector<std::pair<int,ProjectPanel*> >::reverse_iterator i;
    for (i= FWObjectClipboard::obj_clipboard->rbegin();
         i!=FWObjectClipboard::obj_clipboard->rend(); ++i)
    {
        ProjectPanel *proj_p = i->second;
        FWObject *co = proj_p->db()->findInIndex(i->first);
        if (!Rule::cast(co)) continue;
        if (proj_p!=m_project)
        {
            // rule is being copied from another project file
            map<int,int> map_ids;
            co = m_project->db()->recursivelyCopySubtree(ruleset, co, map_ids);
            // Note that FWObjectDatabase::recursivelyCopySubtree adds
            // a copy it creates to the end of the list of children of
            // the object passed as its first arg., which is in this
            // case ruleset. This works only if we paste rule at the
            // bottom of ruleset, otherwise need to move them to the
            // proper location.
            co->ref();
            ruleset->remove(co);
        }
        if (firstSelectedRow<0 || ruleIndex.count(firstSelectedRow)==0)
            insertRule(NULL, insertBefore, co);
        else
        {
            Rule *current_rule_at_pos = ruleIndex[firstSelectedRow];
            insertRule(current_rule_at_pos, insertBefore, co);
        }
    }

    changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    updateGroups();
}


void RuleSetView::pasteRuleBelow()
{
    if (fwbdebug)
        qDebug("RuleSetView::pasteRuleBelow rowsInfo.size()=%d "
               "ruleIndex.size()=%d firstSelectedRow=%d",
               rowsInfo.size(), ruleIndex.size(), firstSelectedRow);

    if (getFirewall()==NULL) return;
    if (!isTreeReadWrite(this,ruleset)) return;

    changingRules = true;

    int position;
    if (lastSelectedRow != -1)
        position = lastSelectedRow;
    else
        position = currentRow();

    int n=0;
    vector<std::pair<int,ProjectPanel*> >::iterator i;
    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i,++n)
    {
        ProjectPanel *proj_p = i->second;
        FWObject *co = proj_p->db()->findInIndex(i->first);
        if (!Rule::cast(co)) continue;
        if (proj_p!=m_project)
        {
            // rule is being copied from another project file
            map<int,int> map_ids;
            co = m_project->db()->recursivelyCopySubtree(ruleset, co, map_ids);
            // Note that FWObjectDatabase::recursivelyCopySubtree adds
            // a copy it creates to the end of the list of children of
            // the object passed as its first arg., which is in this
            // case ruleset. This works only if we paste rule at the
            // bottom of ruleset, otherwise need to move them to the
            // proper location.
            co->ref();
            ruleset->remove(co);
        }
        int target_position = position + n;
        if (ruleIndex.count(target_position)==0)
            insertRule(NULL, appendAfter, co);
        else
        {
            Rule *current_rule_at_pos = ruleIndex[target_position];
            insertRule(current_rule_at_pos, appendAfter, co);
        }
    }

    changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    updateGroups();
}

void RuleSetView::enableRule()
{
    if (getFirewall()==NULL) return;
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( firstSelectedRow!=-1 )
    {
        for (int rn=lastSelectedRow; rn>=firstSelectedRow; --rn)
        {
            Rule *r = ruleIndex[rn];
            if (fwbdebug) qDebug("Firewall changed: enableRule");
            r->enable();
            setRuleNumber(rn,r);
        }
    }
    update();
    //changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    mw->reloadAllWindowsWithFile(m_project);
}

void RuleSetView::disableRule()
{
    if (getFirewall()==NULL) return;
    if (!isTreeReadWrite(this,ruleset)) return;

    if ( firstSelectedRow!=-1 )
    {
        for (int rn=lastSelectedRow; rn>=firstSelectedRow; --rn)
        {
            Rule *r = ruleIndex[rn];
            if (fwbdebug) qDebug("Firewall changed: disableRule");
            r->disable();
            setRuleNumber(rn,r);
        }
    }
    update();
    //changingRules = false;
    m_project->updateLastModifiedTimestampForOneFirewall(getFirewall());
    mw->reloadAllWindowsWithFile(m_project);
}

void RuleSetView::editSelected()
{
    if (!m_project->isEditorVisible()) m_project->showEditor();
    switchObjectInEditor(currentColumn());
}

bool RuleSetView::switchObjectInEditor(int col,bool validate)
{
    if (fwbdebug)
        qDebug("RuleSetView::switchObjectInEditor  col=%d  validate=%d",
               col,validate);


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

    Rule *rule = getRule(crn);
    if (rule==NULL) return false; // rule group header

    switch (getColType(col))
    {
        case Comment:
            Object = ruleIndex[crn];
            Operation = ObjectEditor::optComment;
            break;

        case Metric:
            Object = ruleIndex[crn];
            Operation = ObjectEditor::optMetric;
            break;

        case Direction:
            break;

        case Action:
        {
            PolicyRule *rule = PolicyRule::cast( ruleIndex[currentRow()] );
            Object = rule;
            Operation = ObjectEditor::optAction;
            break;
        }
        case Options:
        {
            /* both policy and routing rules have options. so cast to
             * Rule here. */
            Rule *rule = ruleIndex[currentRow()];
            assert(rule!=NULL);
            Object = rule;
            Operation = ObjectEditor::optNone;
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

    if (Object==m_project->getOpenedEditor() &&
        Operation==m_project->getOpenedOptEditor())
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

    if (fwbdebug) qDebug("RuleSetView::switchObjectInEditor  done");

    return true;
}

void RuleSetView::saveCurrentRowColumn(int &row, int &column)
{
    row = currentRow();
    column = currentColumn();
    if (fwbdebug) qDebug("RuleSetView::saveCurrentRowColumn: "
                         "row=%d, column=%d",
                         row, column);
}

void RuleSetView::restoreCurrentRowColumn(int row, int column)
{
    if (fwbdebug) qDebug("RuleSetView::restoreCurrentRowColumn: "
                         "row=%d, column=%d",
                         row, column);
    setCurrentCell(row, column);
    scrollToCurrent();
    update();
}

void RuleSetView::restoreSelection(bool same_widget)
{
    if (fwbdebug)
    {
        if (prevSelectedObject)
            qDebug("RuleSetView::restoreSelection()  "
                   "same_widget=%d prevSelectedObject=%s "
                   "prevSelectedObjectRow=%d prevSelectedObjectCol=%d",
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
    changeCurrentCell(selectedObjectRow, selectedObjectCol);

    openObjectInTree(selectedObject);
}

void RuleSetView::saveCollapsedGroups()
{
    QStringList collapsed_groups;
    QString filename = m_project->getRCS()->getFileName();
    for (int i =0 ; i < rowsInfo.size(); i++)
    {
        RuleRowInfo *rri = rowsInfo[i];
        if (rri && rri->isFirstRow && rri->collapsedGroup)
            collapsed_groups.push_back(rri->groupName);
    }
    st->setCollapsedRuleGroups(
        filename,
        getFirewall()->getName().c_str(),
        ruleset->getName().c_str(),
        collapsed_groups);
}

void RuleSetView::restoreCollapsedGroups()
{
    freezeRowSizing();

    QStringList collapsed_groups;
    QString filename = m_project->getRCS()->getFileName();
    st->getCollapsedRuleGroups(
        filename,
        getFirewall()->getName().c_str(),
        ruleset->getName().c_str(),
        collapsed_groups);
    for (int row = 0 ; row < rowsInfo.size(); row++)
    {
        RuleRowInfo *rri = rowsInfo[row];
        if (rri &&
            rri->isFirstRow &&
            collapsed_groups.count(rri->groupName) >0)
        {
            collapseRuleGroup(row);
        }
    }

    //mw->unselectRules();
    setCurrentIndex(QModelIndex());
    //changeCurrentCell(0, 0, true);

    unfreezeRowSizing();
}



void RuleSetView::updateAll()
{
    if (fwbdebug) qDebug("RuleSetView::updateAll()");

    int r = 0;
//    init();

    QFontMetrics p(st->getRulesFont());
    QRect br = p.boundingRect(QRect(0, 0, 1000, 1000),
                              Qt::AlignLeft|Qt::AlignVCenter,"WMWM" );
    text_h = br.height();

    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++,r++)
        adjustRow(r);
        //dirtyRows[r] = 1;

    repaint();
    updateGroups();
}

void RuleSetView::updateCurrentCell()
{
    updateCell(m_currentRow, m_currentColumn);
}


PolicyView::PolicyView(ProjectPanel *project, Policy *p, QWidget *parent) : 
      RuleSetView(project, 1, 7, parent)
{
    setName("PolicyView");
    ruleset = p;

    bool old_dirty_flag = ruleset->isDirty();
    iinit();
    init();
    ruleset->setDirty(old_dirty_flag);

    if (fwbdebug) qDebug("PolicyView constructor: done");
}

void PolicyView::init()
{
    //ncols=7 +
    ncols = 8 +
        ((supports_time)?1:0) +
        ((supports_logging && supports_rule_options)?1:0);

    ruleModel->setColumnCount(ncols);

    colTypes[-1] = RuleOp;

    int col=0;
    QStringList qsl;

    qsl << "";
    colTypes[col++] = GroupHandle;

    qsl << getReadableRuleElementName(RuleElementSrc::TYPENAME);      // 0
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName(RuleElementDst::TYPENAME); // 1
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName(RuleElementSrv::TYPENAME);     // 2
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName(RuleElementItf::TYPENAME);   // 3
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName("Direction");   // 4
    colTypes[col++] = Direction;

    qsl << getReadableRuleElementName("Action");      // 5
    colTypes[col++] = Action;

    if (supports_time)
    {
        qsl << getReadableRuleElementName(RuleElementInterval::TYPENAME); // 6
        colTypes[col++] = Time;
    } 

    if (supports_logging && supports_rule_options)
    {
        qsl << getReadableRuleElementName("Options");
        colTypes[col++] = Options;
    } 

    qsl << getReadableRuleElementName("Comment");
    colTypes[col] = Comment;

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




NATView::NATView(ProjectPanel *project, NAT *p, QWidget *parent) : 
      RuleSetView(project, 1,8,parent)
{
    setName("NATView");
    ruleset = p;

    bool old_dirty_flag = ruleset->isDirty();
    iinit();
    init();
    ruleset->setDirty(old_dirty_flag);
}

void NATView::init()
{
    colTypes[-1] = RuleOp;

    ncols=9;
    ruleModel->setColumnCount(9);

    int col = 0;
    QStringList qsl;
    qsl << "";
    colTypes[col++] = GroupHandle;

    qsl << getReadableRuleElementName(RuleElementOSrc::TYPENAME);
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName(RuleElementODst::TYPENAME);
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName(RuleElementOSrv::TYPENAME);
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName(RuleElementTSrc::TYPENAME);
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName(RuleElementTDst::TYPENAME);
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName(RuleElementTSrv::TYPENAME);
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName("Options");
    colTypes[col++] = Options;

    qsl << getReadableRuleElementName("Comment");
    colTypes[col++] = Comment;

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
    ruleset = p;

    bool old_dirty_flag = ruleset->isDirty();
    iinit();
    init();
    ruleset->setDirty(old_dirty_flag);
}

void RoutingView::init()
{
    colTypes[-1] = RuleOp;
    ncols=7;
    ruleModel->setColumnCount(ncols);

    int col = 0;

    QStringList qsl;
    qsl << "";
    colTypes[col++] = GroupHandle;

    qsl << getReadableRuleElementName(RuleElementRDst::TYPENAME);
    colTypes[col++] = Object;

    qsl << getReadableRuleElementName(RuleElementRGtw::TYPENAME);
    colTypes[col++] = Object;

    if (supports_routing_itf)
    {
        qsl << getReadableRuleElementName(RuleElementRItf::TYPENAME);
        colTypes[col++] = Object;
    }

    qsl << getReadableRuleElementName("Metric");
    colTypes[col++] = Metric;

    qsl << getReadableRuleElementName("Options");
    colTypes[col++] = Options;

    qsl << getReadableRuleElementName("Comment");
    colTypes[col] = Comment;

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




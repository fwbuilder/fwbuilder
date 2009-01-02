/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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

#include "fwbuilder/Rule.h"

#include <QPalette>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QIcon>
#include <QItemDelegate>
#include <QHeaderView>
#include <qtableview.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qtooltip.h>

#include <vector>
#include <map>

class QMenu;

using namespace std;

namespace libfwbuilder {
    class FWObject;
    class Firewall;
    class FWReference;
    class RuleElement;
    class Rule;
    class RuleSet;
    class Policy;
    class NAT;
    class Routing;
    class RuleElement;
    class NATRule;
    class RoutingRule;
};

class RuleRowInfo;
class RuleSetView;
class ProjectPanel;

/**
 * this class is used to intercept mouse clicks on the vertical header
 * of the table so we could open a context menu
 */
class headerMouseEventInterceptor : public QObject
{
    RuleSetView *rsv;

    public:
        headerMouseEventInterceptor(RuleSetView *_rsv) { rsv=_rsv; }

    protected:
        bool eventFilter( QObject *object, QEvent *event);
};

class RuleTableModel;

class RuleDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
            
    friend class RuleSetView;
    friend class RuleTableModel;

public:
    RuleDelegate(RuleSetView *parent);

    QRect cellRect(const int row, const int col) const;
    QRect cellGeometry(const int row, const int col) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;

public slots:
    

private:
    RuleSetView *ruleSetView;
};


class RuleTableModel : public QAbstractTableModel
{
    friend class RuleSetView;
    friend class RuleDelegate;
    
public:
    
    RuleTableModel(const int rows, const int columns, RuleSetView *ruleView);
    ~RuleTableModel();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;

    void setRowCount(const int &value);
    void setColumnCount(const int &value);

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    bool setHeader(QStringList qsl);

    void removeRows(const int row1, const int row2); 
    void swapRows(const int row1, const int row2);
    void insertRow(const int before_pos); 

protected:
    int m_rowCount;
    int m_columnCount;
    QStringList header;
    RuleSetView *ruleSetView;
};


class RuleSetView : public QTableView
{
    friend class headerMouseEventInterceptor;
    friend class RuleTableModel;
    friend class RuleDelegate;
    
    Q_OBJECT
    QVector <ProjectPanel*> getAllMdiProjectPanel();

public slots:
    
    void selectionChanged(const QItemSelection&, const QItemSelection&);
    void restoreSelection(bool same_widget);

    void currentChanged( const QModelIndex &current );
    void restoreCurrentRowColumn(int row, int column);

    void itemDoubleClicked(const QModelIndex & index);
    void contextMenu(int row, int col, const QPoint &pos);
    void contextMenuRequested( const QPoint &p );
    
    void newGroup();
    void renameGroup();
    void addToGroupAbove ();
    void addToGroupBelow();
    void removeFromGroup();
    void insertRuleAboveFirstGroup();    
    void showHideRuleGroup(int row);

    void editSelected();
    void copySelectedObject();
    void cutSelectedObject();
    void deleteSelectedObject();
    void pasteObject();
    void findWhereUsedSlot();
    void revealObjectInTree(); 

    void negateRE();
    void editRE(); 

    void changeActionToAccept();
    void changeActionToDeny();
    void changeActionToReject();
    void changeActionToAccounting();
    void changeActionToPipe();
    void changeActionToTag();
    void changeActionToClassify();
    void changeActionToCustom();
    void changeActionToBranch();
    void changeActionToRoute();
    void changeActionToContinue();

    void changeDirectionToIn();
    void changeDirectionToOut();
    void changeDirectionToBoth();
    void changeLogToOn();
    void changeLogToOff(); 
    
    void setColorEmpty();
    void setColorRed();
    void setColorBlue();
    void setColorOrange();
    void setColorPurple();
    void setColorGray();
    void setColorYellow();
    void setColorGreen();
    
    void insertRule();
    void removeRule();
    void addRuleAfterCurrent();
    void moveRule();
    void moveRuleUp();
    void moveRuleDown();

    void copyRule();
    void cutRule();
    void pasteRuleAbove();
    void pasteRuleBelow();

    void saveCollapsedGroups();
    void restoreCollapsedGroups();

    void enableRule();
    void disableRule();

    void setRuleColor(const QString &c);
    void setRuleNumber(int row, libfwbuilder::Rule *rule);
    
    void horzSectionResized( int logicalIndex, int oldSize, int newSize );
    void vertSectionResized( int logicalIndex, int oldSize, int newSize );

    void scrollToCurrent();

    virtual void updateGeometries();

public:
     
     libfwbuilder::RuleSet *ruleset;
     QMap<QString, QString> groupColors;
     QMap<QString,int> rulesInGroup;
     QItemSelectionRange itemSelectionRange;

     virtual void paintCell(QPainter *p,int row, int col,
                            const QRect &cr, bool selected, const QPalette &cg);
     void drawRuleGroupHead(QPainter *p,int row, int col,
                            const QRect &cr, bool selected, const QPalette &cg);
     void drawRuleGroupHandle(QPainter *p,int row, int col,
                            const QRect &cr, bool selected, const QPalette &cg);
     QString getGroupColorForRule(libfwbuilder::Rule *rule);
    
     void setName(const QString &qs);
     void setCurrentCell(const int row, const int col);
     void changeCurrentCell(const int row, const int col,
                            bool fullrefresh = false);

     //set new current cell and update new and old cells
     //fullrefresh forces method to change QTableView's currentIndex
     //which interrupts any kind of selection (with shift and others)
     
     int currentRow() const;
     int currentColumn() const;
     
     void setCurrentRow(const int value);
     void setCurrentColumn(const int value);

     enum REType{ GroupHandle,
                  RuleOp,
                  Object,
                  Action,
                  Direction,
                  Options,
                  Time,
                  Comment,
                  Metric };

 protected:

    void debugPrintRows();

    void resetRuleModel();
    void createRuleModel();

    void collapseRuleGroup(int row);
    void expandRuleGroup(int row);
    void collapseRuleGroupByName(const QString &name);
    void expandRuleGroupByName(const QString &name);
    
    //these functions are added in the porting process
    //they are needed to work with stored cell sizes (columnWidths,
    //rowHeights)
    void freezeRowSizing();
    void unfreezeRowSizing();
     
    bool rowSizingFrozen; //is vertical sizing freezed?
    //it is needed when we insert a row to the table
     
    int getColumnWidth( const int col ) const;
    int getRowHeight( const int row ) const;
    
    void setColumnWidth( const int col, const int width );
    void setRowHeight( const int row, const int height );

    bool event( QEvent * event );
     
    vector<int> columnWidths;
    vector<int> rowHeights;
    map<int,bool> rulesDisabled;
    QIcon negIcon;
     
    int m_currentRow, m_currentColumn;

    enum PixmapAttr      { Normal,  Neg,     Ref,       Tree, NegTree };
    enum PopupMenuAction { None,    EditObj, EditRE,    NegateRE };
    enum insertRuleOp    { appendAfter, insertBefore };
    
    headerMouseEventInterceptor hme;

    

/*
 * ruleIndex maps row number to rule object.
 * Note that the index is not rule number but row number. These become
 * different when we use rule groups.
 *
 * ruleIndex should provide for a fast direct access to elements, as
 * well as for a reasonably fast adding and removal in an arbitrary
 * place so that all element would shift up or down correspondingly. A
 * map is a compromise, it provides for fast direct access but slow
 * adding and removal (because I need to manually shift elements in a
 * loop).
 */
    //std::map<int,libfwbuilder::FWObject*> ruleIndex;
    QMap<int, libfwbuilder::Rule*> ruleIndex;
    QVector<RuleRowInfo*> rowsInfo ;

    int ncols;

    //this bool var is needed for starting drag when user moves the mouse
    //but not when he just clicks selected record
    bool startingDrag;
 
    bool supports_time;
    bool supports_logging;
    bool supports_rule_options;
    bool supports_routing_itf;
    
    int RuleElementSpacing;

    int pixmap_h;
    int pixmap_w;
    int text_h;
    int item_h;
    std::map<int,int> dirtyRows;
    std::map<int,REType> colTypes;

    libfwbuilder::FWObject *selectedObject;
    int selectedObjectRow;
    int selectedObjectCol;
    
    libfwbuilder::FWObject *prevSelectedObject;
    int prevSelectedObjectRow;
    int prevSelectedObjectCol;

    int prevCurrentRow;
    int prevCurrentCol;

    bool kbdGoingUp;
    bool changingSelection;
    bool changingRules;

    PopupMenuAction lastPopupMenuAction;

    void iinit();
    QString settingsKey();

    void insertRuleIndex(int idx);
    void removeRuleIndex(int idx);
    void updateGroups();
    void refreshGroups();

    int getUpNullRuleIndex(int idx);
    int getDownNullRuleIndex(int idx);

    RuleRowInfo* getRuleRowInfoByGroupName(const QString &name);
    int getRuleRowInfoIndexByGroupName(const QString &name);
    QString getFullRuleGroupTitle(int row);

    void createGroup(int row, int count, const QString &groupName);
    void removeFromGroup(int row, int count);
    void addToUpGroup(int row);
    void addToDownGroup(int row);
    
    void adjustRow_int( int row, int h );

    virtual void mousePressEvent( QMouseEvent* ev );
    virtual void mouseMoveEvent( QMouseEvent* ev );
    virtual void mouseReleaseEvent( QMouseEvent* ev );

    virtual QDrag* dragObject();
    
    virtual void keyPressEvent( QKeyEvent* ev );
    virtual void dragMoveEvent( QDragMoveEvent *ev);
    virtual void dragEnterEvent( QDragEnterEvent *ev);
    //virtual void contentsDragEnterEvent( QDragEnterEvent *ev);
    virtual void dropEvent( QDropEvent *ev); 
    virtual void hideEvent(QHideEvent *ev);
    virtual void paintEvent(QPaintEvent * event);
    
    virtual void adjustRow( int row );
    virtual void adjustColumn( int col );

    QString getPlatform(); 

    QRect calculateCellSize( int row, int col );

//    RuleSetOptions* getOpt() const { return opt; }

    bool    insertObject(int row, int col, libfwbuilder::FWObject *obj);
    void    deleteObject(int row, int col, libfwbuilder::FWObject *obj); 

    void    copyRuleContent(libfwbuilder::Rule *dst, libfwbuilder::Rule *src);

    void    headerMousePressEvent(const QPoint &pos);
    void    changeDitection(libfwbuilder::PolicyRule::Direction dir);
    void    changeAction(libfwbuilder::PolicyRule::Action act);

    void    setSelectedObject(libfwbuilder::FWObject* obj); 

 public:
     
    RuleSetView(ProjectPanel *project, int r, int c, QWidget *parent);
    virtual ~RuleSetView();
    virtual void init();
    
    int firstSelectedRow;
    int lastSelectedRow;
    
    RuleTableModel *ruleModel;
    RuleDelegate *ruleDelegate;

    //this function is very special for updating selection
    //it don't needs a focus on RuleSetView and repaints all the widget
    void repaintSelection();
    
    void clear();
    void unselect();

    libfwbuilder::FWObject* getSelectedObject(); 
    libfwbuilder::Firewall* getFirewall();

    libfwbuilder::Rule* insertRule(libfwbuilder::Rule *next_to_rule,
                                   insertRuleOp rule_op,
                                   libfwbuilder::FWObject *old_rule);
    void initRule(libfwbuilder::Rule *new_rule,
                  libfwbuilder::Rule *old_rule);
    
    libfwbuilder::FWObject* getObj(int row, int col,
                                   int mouse_y_pos, QRect *cr=NULL); 
    libfwbuilder::Rule* getRule(int row);
    int getRow(libfwbuilder::Rule *rule);
    
    void selectRE(int row, int col);

    /**
     * selects rule element a reference 'ref' belongs to
     */
    void selectRE(libfwbuilder::FWReference *ref);

    /**
     * makes specified column of a rule current.
     */
    void selectRE(libfwbuilder::Rule *rule, int col);

    /**
     * makes specified rule element current and selects object <obj> in it.
     */
    void selectRE(libfwbuilder::RuleElement *re, libfwbuilder::FWObject *obj);
    
    void updateCell(const int row, const int col);
    void updateAll();
    void updateCurrentCell();

    void saveCurrentRowColumn(int &row, int &column);

    void editCurrentObject();
    bool switchObjectInEditor(int col,bool validate=true);

    /**
     * shows and selects object obj in the tree, or, if obj is a rule,
     * shows corresponding firewall object. Makes sure keyboard focus
     * returns back to the ruleset.
     */
    void openObjectInTree(libfwbuilder::FWObject *obj); 

    REType  getColType(int col) const;
    int     getColByType(REType) const;

    
private:
    ProjectPanel *m_project;
    virtual libfwbuilder::RuleElement* getRE(int row, int col ) = 0;
    virtual libfwbuilder::RuleElement* getRE(libfwbuilder::Rule* r, int col) =0;
    
    QPixmap getPixmap(libfwbuilder::FWObject *obj,
                      PixmapAttr pmattr = Normal) const;
    QString objectText(libfwbuilder::RuleElement *re,
                       libfwbuilder::FWObject *obj); 
 
    QSize getPMSize();

    void fixRulePosition(libfwbuilder::Rule *r,
                         libfwbuilder::FWObject *parent, int pos); 
    QSize drawIconInRule(QPainter &p, int x, int y,
                         libfwbuilder::RuleElement *re, 
                         libfwbuilder::FWObject *o1);
    QString chooseIcon(const QString &icn);
    bool showCommentTip(const QPoint &pos, QHelpEvent *he);
    void drawComment(QPainter &p, int row, int col, const QRect &cr);
};


class PolicyView : public RuleSetView
{
    virtual libfwbuilder::RuleElement* getRE(int row, int col);
    virtual libfwbuilder::RuleElement* getRE(libfwbuilder::Rule* r, int col);

 public:

    PolicyView(ProjectPanel *project, libfwbuilder::Policy *p, QWidget *parent);
    virtual ~PolicyView() {}

    virtual void init();
};

class NATView : public RuleSetView
{
    virtual libfwbuilder::RuleElement* getRE(int row, int col ) ;
    virtual libfwbuilder::RuleElement* getRE(libfwbuilder::Rule* r, int col);

 public:

    NATView(ProjectPanel *project, libfwbuilder::NAT *p, QWidget *parent);
    virtual ~NATView() {}

    virtual void init();
};

class RoutingView : public RuleSetView
{
    virtual libfwbuilder::RuleElement* getRE(int row, int col);
    virtual libfwbuilder::RuleElement* getRE(libfwbuilder::Rule* r, int col);

 public:

    RoutingView(ProjectPanel *project,
                libfwbuilder::Routing *p, QWidget *parent);
    virtual ~RoutingView() {}

    virtual void init();
};

class RuleSetViewFactory
{
public:
    static RuleSetView* getRuleSetViewByType(ProjectPanel *project,
                                             libfwbuilder::RuleSet *ruleset,
                                             QWidget *parent);
};


#include "fwbuilder/Rule.h"

#include <qtablewidget.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qtooltip.h>

#include <vector>
#include <map>

class QPopupMenu;

namespace libfwbuilder {
    class FWObject;
    class Firewall;
    class FWReference;
    class RuleElement;
    class Rule;
    class RuleSet;
    class Policy;
    class InterfacePolicy;
    class NAT;
    class Routing;
    class RuleElement;
    class NATRule;
    class RoutingRule;
};

class RuleSetView;

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


class RuleObjToolTip : public QToolTip {

    RuleSetView *rsv;
 public:
    RuleObjToolTip(RuleSetView *w);
    virtual ~RuleObjToolTip() {}
    virtual void maybeTip(const QPoint &p);
};


class RuleSetView : public QTableWidget
{
    friend class headerMouseEventInterceptor;
    
    Q_OBJECT

 public slots:

    void contentsMoving(int x, int y);
    void horizontalHeaderClicked(int col);
    void verticalHeaderClicked(int row);
    void selectionChanged();
    void restoreSelection(bool same_widget);

    void contextMenu(int row, int col, const QPoint &pos);
    void doubleClicked(int row,int col,int btn,const QPoint &pos);

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

    void insertRule();
    void addRuleAfterCurrent();
    void removeRule();
    void moveRule();
    void moveRuleUp();
    void moveRuleDown();

    void copyRule();
    void cutRule();
    void pasteRuleAbove();
    void pasteRuleBelow();


    void enableRule();
    void disableRule();

    void setRuleColor(const QString &c);
    void setRuleNumber(int row, libfwbuilder::Rule *rule);

 public:

    enum REType          { RuleOp,
                           Object,
                           Action,
                           Direction,
                           Options,
                           Time,
                           Comment,
                           Metric };

 protected:

    enum PixmapAttr      { Normal,  Neg,     Ref,       Tree };
    enum PopupMenuAction { None,    EditObj, EditRE,    NegateRE };

    headerMouseEventInterceptor           hme;

    libfwbuilder::RuleSet                *ruleset;

/*
 * ruleIndex should provide for a fast direct access to elements, as
 * well as for a reasonably fast adding and removal in an arbitrary
 * place so that all element would shift up or down correspondingly. A
 * map is a compromise, it provides for fast direct access but slow
 * adding and removal (because I need to manually shift elements in a
 * loop).
 */
    std::map<int,libfwbuilder::FWObject*> ruleIndex;
    int                                   ncols;

    bool                                  supports_time;
    bool                                  supports_logging;
    bool                                  supports_rule_options;
    
    int                                   RuleElementSpacing;

    int                                   pixmap_h;
    int                                   pixmap_w;
    int                                   text_h;
    int                                   item_h;
    std::map<int,int>                     dirtyRows;

    std::map<int,REType>                  colTypes;

    libfwbuilder::FWObject               *selectedObject;
    int                                   selectedObjectRow;
    int                                   selectedObjectCol;
    
    libfwbuilder::FWObject               *prevSelectedObject;
    int                                   prevSelectedObjectRow;
    int                                   prevSelectedObjectCol;

    bool                                  kbdGoingUp;
    bool                                  changingSelection;
    bool                                  dragging;
    bool                                  changingRules;
 
    PopupMenuAction                       lastPopupMenuAction;

    void    iinit();
    QString settingsKey();

    void adjustRow_int( int row, int h );

    virtual void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    
    virtual QDrag* dragObject();
    virtual void contentsMousePressEvent( QMouseEvent* ev );
    virtual void contentsMouseMoveEvent( QMouseEvent* ev );
    virtual void contentsMouseReleaseEvent( QMouseEvent* ev );
    virtual void keyPressEvent( QKeyEvent* ev );
    virtual void dragMoveEvent( QDragMoveEvent *ev);
    virtual void dragEnterEvent( QDragEnterEvent *ev);
    virtual void contentsDragEnterEvent( QDragEnterEvent *ev);
    virtual void dropEvent( QDropEvent *ev);
    virtual void hideEvent(QHideEvent *ev);
    
    virtual void focusInEvent(QFocusEvent*);
    virtual void focusOutEvent(QFocusEvent*);
    
    virtual void adjustRow( int row );
    virtual void adjustColumn( int col );

    void addRuleBranch(libfwbuilder::PolicyRule *rule);
    
    bool isDirection (int col);
    bool isAction    (int col);
    bool isOptions   (int col);
    
    QString getPlatform();
    
    QRect calculateCellSize( int row, int col );
    
    virtual void resizeData( int ) {}
    virtual QTableItem *item( int r, int c ) const { return items.find( c( r, c ) ); }
    virtual void setItem( int r, int c, QTableItem *i ) { items.replace( indexOf( r, c ), i ); }
    virtual void clearCell( int r, int c ) { items.remove( indexOf( r, c ) ); }
    virtual void takeItem( QTableItem *item )
    {
        items.setAutoDelete( FALSE );
        items.remove( indexOf( item->row(), item->col() ) );
        items.setAutoDelete( TRUE );
    }
    virtual void insertWidget( int r, int c, QWidget *w );
    virtual QWidget *cellWidget( int r, int c ) const;
    virtual void clearCellWidget( int r, int c )
    {
        QWidget *w = widgets.take( indexOf( r, c ) );
        if ( w )
            w->deleteLater();
    }
    virtual void paintCell(QPainter *p,int row,int col, const QRect &cr,bool selected, const QColorGroup &cg);
    
    virtual QWidget *createEditor( int row, int col, bool initFromCell ) const;

//    RuleSetOptions* getOpt() const { return opt; }

    bool    insertObject(int row, int col, libfwbuilder::FWObject *obj);
    void    deleteObject(int row, int col, libfwbuilder::FWObject *obj);

    void    copyRuleContent(libfwbuilder::Rule *dst, libfwbuilder::Rule *src);

    void    headerMousePressEvent(const QPoint &pos);
    void    changeDitection(libfwbuilder::PolicyRule::Direction dir);
    void    changeAction(libfwbuilder::PolicyRule::Action act);

    void    setSelectedObject(libfwbuilder::FWObject* obj);

 public:

    RuleSetView( int r, int c, QWidget *parent);
    virtual ~RuleSetView();
    virtual void init();

    void clear();
    
    void unselect();
    bool isSelected();
    libfwbuilder::FWObject* getSelectedObject();
    libfwbuilder::Firewall* getFirewall();

    libfwbuilder::Rule* insertRule(int pos, libfwbuilder::FWObject *r);

    libfwbuilder::FWObject* getObj(int row, int col,
                                   int mouse_y_pos, QRect *cr=NULL);
    libfwbuilder::PolicyRule* getRule(int row);

    void selectRE( int row, int col);

    /**
     * selects rule element a reference 'ref' belongs to
     */
    void selectRE(libfwbuilder::FWReference *ref);

    void updateCurrentCell();
    void updateAll();

    void editCurrentObject();
    bool switchObjectInEditor(int col,bool validate=true);

    /**
     * shows and selects object obj in the tree, or, if obj is a rule,
     * shows corresponding firewall object. Makes sure keyboard focus
     * returns back to the ruleset.
     */
    void openObjectInTree(libfwbuilder::FWObject *obj);

    REType  getColType(int col) const;

private:

    QIntDict<QTableItem> items;
    QIntDict<QWidget> widgets;

//    virtual libfwbuilder::RuleElement* getRE( int row, int col ) const = 0;
//    virtual libfwbuilder::RuleElement* getRE( libfwbuilder::Rule* r, int col ) const = 0;
    virtual libfwbuilder::RuleElement* getRE( int row, int col )  = 0;
    virtual libfwbuilder::RuleElement* getRE( libfwbuilder::Rule* r, int col )  = 0;
    
    QPixmap getPixmap(libfwbuilder::FWObject *obj, PixmapAttr pmattr = Normal) const;
    QString objectText(libfwbuilder::RuleElement *re,libfwbuilder::FWObject *obj);

    void fixRulePosition(libfwbuilder::Rule *r, libfwbuilder::FWObject *parent, int pos);
};



class PolicyView : public RuleSetView
{
    virtual libfwbuilder::RuleElement* getRE( int row, int col ) ;
    virtual libfwbuilder::RuleElement* getRE( libfwbuilder::Rule* r, int col ) ;

 public:

    PolicyView(libfwbuilder::Policy *p, QWidget *parent);
    virtual ~PolicyView() {}

    virtual void init();
    
};

class InterfacePolicyView : public RuleSetView
{
    virtual libfwbuilder::RuleElement* getRE( int row, int col ) ;
    virtual libfwbuilder::RuleElement* getRE( libfwbuilder::Rule* r, int col ) ;

 public:

    InterfacePolicyView(libfwbuilder::InterfacePolicy *p, QWidget *parent);
    virtual ~InterfacePolicyView() {}

    virtual void init();
    

};

class NATView : public RuleSetView
{
    virtual libfwbuilder::RuleElement* getRE( int row, int col ) ;
    virtual libfwbuilder::RuleElement* getRE( libfwbuilder::Rule* r, int col ) ;

 public:

    NATView(libfwbuilder::NAT *p, QWidget *parent);
    virtual ~NATView() {}

    virtual void init();
    

};

class RoutingView : public RuleSetView
{
    virtual libfwbuilder::RuleElement* getRE( int row, int col ) ;
    virtual libfwbuilder::RuleElement* getRE( libfwbuilder::Rule* r, int col ) ;

 public:

    RoutingView(libfwbuilder::Routing *p, QWidget *parent);
    virtual ~RoutingView() {}

    virtual void init();
    

};


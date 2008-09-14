/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  alek@codeminders.com
           refactoring and bugfixes: vadim@fwbuilder.org

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

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"

#include <fwbuilder/RuleSet.h>
#include <fwbuilder/Policy.h>
#include <fwbuilder/NAT.h>
#include <fwbuilder/Routing.h>

#include "FWBSettings.h"
#include "FWBTree.h"
#include "FWObjectPropertiesFactory.h"
#include "FWWindow.h"
#include "FindObjectWidget.h"
#include "FindWhereUsedWidget.h"
#include "ProjectPanel.h"
#include "RCS.h"
#include "RuleSetView.h"
#include "findDialog.h"
#include "listOfLibraries.h"

#include <QMdiSubWindow>
#include <QMdiArea>
#include <QTimer>
#include <QStatusBar>
#include <QFileInfo>


using namespace Ui;
using namespace libfwbuilder;
using namespace std;

void ProjectPanel::initMain(FWWindow *main)
{
    mainW = main;
    closing = false ;

    // mdiWindow changes state several times right after it is opened,
    // but we call saveState to store splitter position and its geometry
    // when state changes. Flag "ready" is false after ProjectPanel is created
    // and until FWWindow decides that ProjectPanel is ready for operation.
    // Do not load or save state if flag ready is false.
    ready = false;

    int total_width = DEFAULT_H_SPLITTER_POSITION;
    int total_height = DEFAULT_V_SPLITTER_POSITION;

    if (mainW)
    {
        total_width = mainW->width();
        total_height = mainW->height();
    }

    setMainSplitterPosition(DEFAULT_H_SPLITTER_POSITION,
                            total_width - DEFAULT_H_SPLITTER_POSITION);
    setObjInfoSplitterPosition(DEFAULT_V_SPLITTER_POSITION,
                               total_height - DEFAULT_V_SPLITTER_POSITION);


    enableAvtoSaveState=true ;
    oldState=-1;
    if (st->getInfoStyle()!=0) m_panel->oi->show();
    else m_panel->oi->hide();
    
    findObjectWidget = new FindObjectWidget(
        m_panel->auxiliaryPanel, "findObjectWidget" );
    findObjectWidget->setFocusPolicy( Qt::NoFocus );
    m_panel->auxiliaryPanel->layout()->addWidget( findObjectWidget );
    connect( findObjectWidget, SIGNAL( close() ),
             this, SLOT( closeAuxiliaryPanel() ) );

    findWhereUsedWidget = new FindWhereUsedWidget(
        m_panel->auxiliaryPanel, "findWhereUsedWidget");
    findWhereUsedWidget->setFocusPolicy( Qt::NoFocus );
    m_panel->auxiliaryPanel->layout()->addWidget( findWhereUsedWidget );
    findWhereUsedWidget->hide();
    connect( findWhereUsedWidget, SIGNAL( close() ),
             this, SLOT( closeAuxiliaryPanel() ) );
    connect( m_panel->infoStyleButton, SIGNAL( clicked() ),
             this, SLOT( changeInfoStyle() ) );
    connect(m_panel->mainSplitter, SIGNAL(splitterMoved(int,int)),
            this,SLOT(splitterMoved(int,int)));
    connect(m_panel->objInfoSplitter, SIGNAL(splitterMoved(int,int)),
            this,SLOT(splitterMoved(int,int)));

    m_panel->auxiliaryPanel->hide();
    initOE();
    initFD();
}

ProjectPanel::ProjectPanel(QWidget *parent): 
    QWidget(parent), // , Qt::WindowSystemMenuHint|Qt::Window),
    mainW(0),
    rcs(0),
    addOnLibs(new listOfLibraries()), 
    objectTreeFormat(new FWBTree),
    systemFile(true),
    safeMode(false),
    editingStandardLib(false),
    editingTemplateLib(false),
    ruleSetRedrawPending(false),
    objdb(0),
    editorOwner(0), 
    oe(0),
    fd(0),
    shownInInfo(0),
    autosaveTimer(new QTimer(static_cast<QObject*>(this))), ruleSetTabIndex(0),
    visibleFirewall(0),
    visibleRuleSet(0),
    lastFirewallIdx(-2),
    changingTabs(false),
    noFirewalls(tr("No firewalls defined")),
    m_panel(0),
    findObjectWidget(0), 
    findWhereUsedWidget(0)
{
    if (fwbdebug) qDebug("ProjectPanel constructor");
    m_panel = new Ui::ProjectPanel_q();
    m_panel->setupUi(this);
    firstTimeNormal = st->getInt("Window/maximized");
}


ProjectPanel::~ProjectPanel()
{
    if (rcs) delete rcs;
    delete m_panel;
}

void ProjectPanel::info(FWObject *obj, bool forced)
{
    if (obj==NULL)
        return ;
    if (st->getInfoStyle()!=0 && (shownInInfo!=obj || forced))
    {
        m_panel->oi->clear();

        QString s="";
        if (st->getInfoStyle()==2)
        {
            s=FWObjectPropertiesFactory::getObjectPropertiesDetailed(obj)
                + QString("<hr height=\"0\">");
            m_panel->oi->setText(s);
        }

        s=QString::fromUtf8(obj->getComment().c_str());
        m_panel->oi->append(s);
        m_panel->oi->moveCursor(QTextCursor::Start);

        shownInInfo = obj;
    }

//    unselectRules();
}

QString ProjectPanel::getPageTitle()
{
    QString caption = rcs->getFileName().section("/",-1,-1);
    if (rcs->isInRCS()) caption = caption + ", rev " + rcs->getSelectedRev();
    if (rcs->isRO()) caption = caption + " " + tr("(read-only)");
    return caption;
}

void ProjectPanel::setStartupFileName(const QString &fn) 
{ 
    startupFileName = fn; 
}

RuleElement* ProjectPanel::getRE(Rule* r, int col )
{
    string ret;
    switch (col)
    {
        case 0: ret=RuleElementSrc::TYPENAME; break;//Object
        case 1: ret=RuleElementDst::TYPENAME; break;//Object
        case 2: ret=RuleElementSrv::TYPENAME; break;//Object
        case 3: ret=RuleElementItf::TYPENAME; break;//Object
        case 4: ret=RuleElementInterval::TYPENAME; break;//Time
        default: return NULL;
    }

    return RuleElement::cast( r->getFirstByType(ret) );
}

/*
 * info styles go like this:
 * 0 - collapsed
 * 1 - opened
 * 2 - opened, more information
 *  etc.
 *
 */
void ProjectPanel::changeInfoStyle()
{
    shownInInfo = NULL;
    switch (st->getInfoStyle())
    {
    case 0:
        st->setInfoStyle(1);
        m_panel->oi->show();
        break;
    case 1:
        st->setInfoStyle(2);
        m_panel->oi->show();
        break;
    case 2:
        st->setInfoStyle(0);
        m_panel->oi->hide();
        break;
    }

/* TODO: now need to send signal to the dialog telling it to change
 * according to the style
 */

    info();
}

void ProjectPanel::restoreRuleSetTab()
{
    if (fwbdebug) qDebug("ProjectPanel::()");
    m_panel->ruleSets->setCurrentIndex(ruleSetTabIndex);

}

void ProjectPanel::loadObjects()
{
    m_panel->om->loadObjects();
}

void ProjectPanel::loadObjects(FWObjectDatabase*)
{
    m_panel->om->loadObjects();
}

void ProjectPanel::clearObjects()
{
    m_panel->om->clearObjects();
}



void ProjectPanel::initOE()
{
    oe  = new ObjectEditor((QWidget*)m_panel->objectEditorStack, this);
    oe->setCloseButton(m_panel->closeObjectEditorButton);
    oe->setApplyButton(m_panel->applyObjectEditorButton);
    oe->hide();
}

void ProjectPanel::initFD()
{
    fd  = new findDialog(this, this);
    fd->hide();
}

void ProjectPanel::clearFirewallTabs()
{
    if (fwbdebug) qDebug("ProjectPanel::clearFirewallTabs");

    m_panel->ruleSets->hide();

    while (m_panel->ruleSets->count()!=0)
    {
        QWidget *p = m_panel->ruleSets->widget(0);
        m_panel->ruleSets->removeWidget(
            m_panel->ruleSets->widget(m_panel->ruleSets->indexOf(p)));
        delete p;
    }
    m_panel->rulesetname->setText("");
    m_panel->ruleSets->show();
    ruleSetViews.clear();
}

void ProjectPanel::ensureObjectVisibleInRules(FWReference *obj)
{
    FWObject *p=obj;
    while (p && RuleSet::cast(p)==NULL ) p=p->getParent();
    if (p==NULL) return;  // something is broken
    openRuleSet(p);
    getCurrentRuleSetView()->selectRE( obj );    
}

/*
 * Ensure rule is visible and highlight given column
 */
void ProjectPanel::ensureRuleIsVisible(Rule *rule, int col)
{
    FWObject *p = rule;

    while (p && RuleSet::cast(p)==NULL ) p=p->getParent();
    if (p==NULL) return;  // something is broken

    RuleSetView *rsv = ruleSetViews[p];

    if (rsv==NULL)
    {
        if (fwbdebug)
            qDebug("ProjectPanel::ensureRuleIsVisible : orphan rule set found");
        return;
    }

    m_panel->ruleSets->setCurrentIndex(m_panel->ruleSets->indexOf(rsv));
    rsv->selectRE( rule->getPosition(), col );
}

void ProjectPanel::updateRuleSetViewSelection()
{
    RuleSetView* rv = dynamic_cast<RuleSetView*>(
        m_panel->ruleSets->currentWidget());
    if (rv!=NULL)
        rv->repaintSelection();
}

void ProjectPanel::updateRuleSetView()
{
    RuleSetView* rv=dynamic_cast<RuleSetView*>(
        m_panel->ruleSets->currentWidget());
    if (rv!=NULL) rv->updateAll();
}

void ProjectPanel::updateRuleOptions()
{
    RuleSetView* rv = dynamic_cast<RuleSetView*>(
        m_panel->ruleSets->currentWidget());
    if (rv!=NULL) rv->updateCurrentCell();
}

void ProjectPanel::updateTreeViewItemOrder()
{
    //this is for case when tree becomes to be resorted
    //if we do not reopen parent item, some of child
    //items mix incorrectly (maybe bug of QT?)
    m_panel->om->reopenCurrentItemParent();
}

RuleSetView * ProjectPanel::getCurrentRuleSetView () 
{
    return dynamic_cast<RuleSetView*>(m_panel->ruleSets->currentWidget ());
}


void ProjectPanel::reopenFirewall()
{
    if (fwbdebug)  qDebug("ProjectPanel::reopenFirewall()");

    if (ruleSetRedrawPending) return;

    int currentPage = m_panel->ruleSets->currentIndex();

    changingTabs=true;

    clearFirewallTabs();
    
/*    if (firewalls.size()==0 || visibleFirewall==NULL)
    {
        changingTabs=false;
        return;
    }
*/
    QStatusBar *sb = mainW->statusBar();
    sb->showMessage( tr("Building policy view...") );
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
    if (fwbdebug)  qDebug("ProjectPanel::reopenFirewall()   adding Policy tab");
/*
    RuleSetView *rsv;
    Policy *pol=Policy::cast(visibleFirewall->getFirstByType(Policy::TYPENAME));
    m_panel->ruleSets->addTab( rsv=new PolicyView(this, pol,NULL) , tr("Policy") );
    ruleSetViews[pol]=rsv;
*/
// let the GUI process events to display new tab
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
    if (visibleRuleSet==NULL)
        return ;

    for (int i =0 ; i < m_panel->ruleSets->count (); i++)
    {
        m_panel->ruleSets->removeWidget(m_panel->ruleSets->widget(i));
    }
    m_panel->rulesetname->setTextFormat(Qt::RichText);
    QString name = "<B>";
    FWObject * p = visibleRuleSet->getParent();
    name += p->getName().c_str();
    name += " / ";
    name += visibleRuleSet->getName().c_str();
    name += "</B>";
    Policy *rule = Policy::cast(visibleRuleSet);
    m_panel->rulesetname->setText(name );

    if (rule!=NULL)
    {
        m_panel->ruleSets->addWidget( new PolicyView(this, rule,NULL) );
    }

    NAT *nat  = NAT::cast(visibleRuleSet);
    if (nat!=NULL)
    {
        m_panel->ruleSets->addWidget( new NATView(this, nat,NULL) );
    }

    Routing *r = Routing::cast(visibleRuleSet);
    if (r!=NULL)
    {
        m_panel->ruleSets->addWidget( new RoutingView(this, r,NULL) );
    }

    sb->clearMessage();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
    if (fwbdebug)  qDebug("ProjectPanel::reopenFirewall()   all tabs are done");

    m_panel->ruleSets->setCurrentIndex( currentPage );

    changingTabs=false;
    mainW->setEnabledAfterRF();
    m_panel->ruleSets->show();
}

int  ProjectPanel::findFirewallInList(FWObject *f)
{
    vector<FWObject*>::iterator i;
    int n=0;
    for (i=firewalls.begin(); i!=firewalls.end(); i++,n++)
    {
        if ( (*i)->getId()==f->getId() ) return n;
    }
    return -1;
}

void ProjectPanel::updateFirewallName(FWObject *, const QString &)
{
    if (visibleRuleSet==NULL)
        return ;

    QString name = "<B>";
    FWObject * p = visibleRuleSet->getParent();
    name += p->getName().c_str();
    name += " / ";
    name += visibleRuleSet->getName().c_str();
    name += "</B>";
    m_panel->rulesetname->setText(name );
}

void ProjectPanel::openRuleSet(FWObject * obj)
{
    blankEditor();
    RuleSet * rs = RuleSet::cast(obj);
    if (rs!= visibleRuleSet)
    {
        visibleRuleSet = rs;
        scheduleRuleSetRedraw();
    }
}

void ProjectPanel::selectRules()
{
//    `unselect();
    RuleSetView* rv = dynamic_cast<RuleSetView*>(
        m_panel->ruleSets->currentWidget());
    rv->setFocus();
}

void ProjectPanel::unselectRules()
{
    bool havePolicies = (m_panel->ruleSets->count()!=0);

/* commented this out so that when I hit "Edit" in the object's pop-down
 * menu in a rule, ruleset wont lose focus when object editor is opened.
 * If rule set loses focus, the object's background turns from "selected" color
 * to white and user loses context (which object is shown in the object editor)
 */
    if (havePolicies)
    {
        RuleSetView* rv=dynamic_cast<RuleSetView*>(m_panel->ruleSets->currentWidget());

        if (rv && rv->getSelectedObject()!=getSelectedObject())
        {
            rv->unselect();
            rv->clearFocus();
        }
    }
    mainW->disableActions(havePolicies);
}

void ProjectPanel::editCopy()
{
    if (isManipulatorSelected()) copyObj();
    else
        if (m_panel->ruleSets->count()!=0)
            dynamic_cast<RuleSetView*>(m_panel->ruleSets->currentWidget())->copySelectedObject();
}

void ProjectPanel::editCut()
{
    if (isManipulatorSelected()) cutObj();
    else
        if (m_panel->ruleSets->count()!=0)
            dynamic_cast<RuleSetView*>(m_panel->ruleSets->currentWidget())->cutSelectedObject();
}

void ProjectPanel::editDelete()
{
    if (isManipulatorSelected()) deleteObj();
    //else
    //    if (ruleSets->count()!=0)
    //        dynamic_cast<RuleSetView*>(m_panel->ruleSets->currentWidget())->deleteSelectedObject();
}

void ProjectPanel::editPaste()
{
    if (isManipulatorSelected()) pasteObj();
    else
        if (m_panel->ruleSets->count()!=0)
            dynamic_cast<RuleSetView*>(
                m_panel->ruleSets->currentWidget())->pasteObject();
}

QString ProjectPanel::getDestDir(const QString &fname)
{
    QString destdir = "";

    if (st->getWDir().isEmpty())
    {
        if (fname.isEmpty())
        {
/* need some reasonable default working directory.
 * on Unix will use current dir.
 * on Windows will use user's document dir.
 */
#if defined(Q_OS_WIN32) || defined(Q_OS_MACX)
            destdir = userDataDir.c_str();
#else
            destdir = "";
#endif
        } else
        {
            if (QFileInfo(fname).isDir()) destdir=fname;
            else
                destdir = fname.left( fname.lastIndexOf('/',-1) );
        }
    } else
    {
        destdir=st->getWDir();
    }
    return destdir;
}

void ProjectPanel::setFileName(const QString &fname)
{
    systemFile = false;
    rcs->setFileName(fname);
    db()->setFileName(fname.toLatin1().constData());

    setWindowTitle(getPageTitle());
}

//wrapers for some ObjectManipulator functions
FWObject* ProjectPanel::getOpened()
{
    return m_panel->om->getOpened();
}

FWObject* ProjectPanel::getCurrentLib()
{
    return m_panel->om->getCurrentLib();
}


void ProjectPanel::loadDataFromFw(Firewall *fw)
{
    m_panel->om->loadObjects();

    if (fw)
    {
        m_panel->om->updateObjName(fw,"", false);
        m_panel->om->editObject(fw);
    }
}

FWObject* ProjectPanel::createObject(const QString &objType,
                                     const QString &objName,
                                     FWObject *copyFrom)
{
    return m_panel->om->createObject(objType, objName, copyFrom);
}

FWObject* ProjectPanel::createObject(FWObject *parent,
                                     const QString &objType,
                                     const QString &objName,
                                     FWObject *copyFrom)
{
    return m_panel->om->createObject(parent, objType, objName, copyFrom);
}

void ProjectPanel::moveObject(FWObject *target,
                              FWObject *obj)
{
    m_panel->om->moveObject(target, obj);
}

void ProjectPanel::moveObject(const QString &targetLibName,
                              FWObject *obj)
{
    m_panel->om->moveObject(targetLibName, obj);
}

void ProjectPanel::autorename(FWObject *obj,
                              const std::string &objtype,
                              const std::string &namesuffix)
{
    m_panel->om->autorename(obj, objtype, namesuffix);
}


void ProjectPanel::updateLibColor(FWObject *lib)
{
    m_panel->om->updateLibColor(lib);
}

void ProjectPanel::updateLibName(FWObject *lib)
{
    m_panel->om->updateLibName(lib);
}

void ProjectPanel::updateObjName(FWObject *obj,
                                 const QString &oldName,
                                 bool  askForAutorename)
{
    m_panel->om->updateObjName(obj, oldName, askForAutorename);
}

void ProjectPanel::updateObjName(FWObject *obj,
                                 const QString &oldName,
                                 const QString &oldLabel,
                                 bool  askForAutorename)
{
    m_panel->om->updateObjName(obj, oldName, oldLabel, askForAutorename);
}


void ProjectPanel::updateLastModifiedTimestampForOneFirewall(FWObject *o)
{
    m_panel->om->updateLastModifiedTimestampForOneFirewall(o);
}

void ProjectPanel::updateLastModifiedTimestampForAllFirewalls(FWObject *o)
{
    m_panel->om->updateLastModifiedTimestampForAllFirewalls(o);
}

void ProjectPanel::updateLastInstalledTimestamp(FWObject *o)
{
    m_panel->om->updateLastInstalledTimestamp(o);
}

void ProjectPanel::updateLastCompiledTimestamp(FWObject *o)
{
    m_panel->om->updateLastCompiledTimestamp(o);
}


FWObject* ProjectPanel::pasteTo(FWObject *target, FWObject *obj)
{
    return m_panel->om->pasteTo(target, obj, true);
}

void ProjectPanel::delObj(FWObject *obj,bool openobj)
{
    m_panel->om->delObj(obj, openobj);
}

ObjectTreeView* ProjectPanel::getCurrentObjectTree()
{
    return m_panel->om->getCurrentObjectTree();
}

void ProjectPanel::openObject(QTreeWidgetItem *otvi)
{
    m_panel->om->openObject(otvi);
}

void ProjectPanel::openObject(FWObject *obj)
{
    m_panel->om->openObject(obj);
}

bool ProjectPanel::editObject(FWObject *obj)
{
    return m_panel->om->editObject(obj);
}

void ProjectPanel::findAllFirewalls (std::list<Firewall *> &fws)
{
    m_panel->om->findAllFirewalls (fws);
}

FWObject* ProjectPanel::duplicateObject(FWObject *target,
                                        FWObject *obj,
                                        const QString &name,
                                        bool  askForAutorename)
{
    return m_panel->om->duplicateObject(target, obj, name, askForAutorename);
}

void ProjectPanel::showDeletedObjects(bool f)
{
    m_panel->om->showDeletedObjects(f);
}

void ProjectPanel::select()
{
    m_panel->om->select();
}

void ProjectPanel::unselect()
{
    m_panel->om->unselect();
}

void ProjectPanel::info()
{
    info(m_panel->om->getSelectedObject(), true);
}

void ProjectPanel::setManipulatorFocus()
{
    m_panel->om->setFocus();
}

void ProjectPanel::clearManipulatorFocus()
{
    m_panel->om->clearFocus();
}

void ProjectPanel::copyObj()
{
    m_panel->om->copyObj();
}

bool ProjectPanel::isManipulatorSelected()
{
    return m_panel->om->isSelected();
}

void ProjectPanel::cutObj()
{
    m_panel->om->cutObj();
}

void ProjectPanel::pasteObj()
{
    m_panel->om->pasteObj();
}


void ProjectPanel::newObject()
{
    m_panel->om->newObject();
}

void ProjectPanel::deleteObj()
{
    m_panel->om->deleteObj();
}

FWObject* ProjectPanel::getSelectedObject()
{
    return m_panel->om->getSelectedObject();
}

void ProjectPanel::reopenCurrentItemParent()
{
    m_panel->om->reopenCurrentItemParent();
}

void ProjectPanel::back()
{
    m_panel->om->back();
}

void ProjectPanel::lockObject()
{
    m_panel->om->lockObject();
}

void ProjectPanel::unlockObject()
{
    m_panel->om->unlockObject();
}



//wrapers for some Object Editor functions
bool ProjectPanel::isEditorVisible()
{
    return oe->isVisible();
}

bool ProjectPanel::isEditorModified()
{
    return oe->isModified();
}

void ProjectPanel::showEditor()
{
    oe->show();
}

void ProjectPanel::hideEditor()
{
    oe->hide();
}

void ProjectPanel::closeEditor()
{
    oe->close();
}

void ProjectPanel::openEditor(FWObject *o)
{
    oe->open(o);
}

void ProjectPanel::openOptEditor(FWObject *o, ObjectEditor::OptType t)
{
    oe->openOpt(o, t);
}

void ProjectPanel::blankEditor()
{
    oe->blank();
}


FWObject* ProjectPanel::getOpenedEditor()
{
    return oe->getOpened();
}

ObjectEditor::OptType ProjectPanel::getOpenedOptEditor()
{
    return oe->getOpenedOpt();
}

void ProjectPanel::selectObjectInEditor(FWObject *o)
{
    oe->selectObject(o);
}

void ProjectPanel::actionChangedEditor(FWObject *o)
{
    oe->actionChanged(o);
}

bool ProjectPanel::validateAndSaveEditor()
{
    return oe->validateAndSave();
}

void ProjectPanel::setFDObject(FWObject *o)
{
    fd->setObject(o);
    fd->show();
}
void ProjectPanel::resetFD()
{
    fd->reset();
}


void ProjectPanel::insertRule()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->insertRule();
}

void ProjectPanel::addRuleAfterCurrent()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->addRuleAfterCurrent();
}

void ProjectPanel::removeRule()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->removeRule();
}

void ProjectPanel::moveRule()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->moveRule();
}

void ProjectPanel::moveRuleUp()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->moveRuleUp();
}

void ProjectPanel::moveRuleDown()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->moveRuleDown();
}

void ProjectPanel::copyRule()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->copyRule();
}

void ProjectPanel::cutRule()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->cutRule();
}

void ProjectPanel::pasteRuleAbove()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->pasteRuleAbove();
}

void ProjectPanel::pasteRuleBelow()
{
    if (visibleRuleSet==NULL || m_panel->ruleSets->count()==0) return;
    getCurrentRuleSetView()->pasteRuleBelow();
}

//rcs
void ProjectPanel::startupLoad()
{
    if (fwbdebug) qDebug("ProjectPanel::startupLoad: "
                         "startup: load everything.");

    int sa = st->getStartupAction();

    if (safeMode)  load(NULL);
    else
    {
        if (startupFileName.isEmpty() && sa==1) // load last edited
        {
            startupFileName = st->getLastEdited();
        }

        if ( !startupFileName.isEmpty() )
        {
            try
            {
                RCS *rcs=new RCS(startupFileName);
                rcs->co();
                load(NULL, rcs);

                //loadLastOpenedLib();
            } catch (FWException &ex)
            {
                qDebug("Exception: %s",ex.toString().c_str());
                load(NULL);
            }
        } else
        {
            load(NULL); // load standard objects
            //loadLastOpenedLib(startupFileName);
        }
    }

    QString id = st->getStr("UI/visibleFirewall");
    int i_id = FWObjectDatabase::getIntId(id.toLatin1().constData());
    FWObject *show_fw=NULL;
    if ( !id.isEmpty() ) show_fw = db()->getById(i_id, true);

    id = st->getStr("UI/visibleObject");
    i_id = FWObjectDatabase::getIntId(id.toLatin1().constData());
    FWObject *show_obj=NULL;
    if ( !id.isEmpty() ) show_obj = db()->getById(i_id, true);
    if ( sa==1 && !safeMode )
    {
        if (show_fw)
        {
            if (fwbdebug)
                qDebug("open firewall %s",show_fw->getName().c_str());
        }

        if (show_obj)
        {
            if (fwbdebug)
                qDebug("open object %s",show_obj->getName().c_str());
        }
    }
}


bool ProjectPanel::editingLibrary()
{
    return (rcs!=NULL &&
        ( rcs->getFileName().endsWith(".fwl")) );
}

void ProjectPanel::createRCS(const QString &filename)
{
    rcs = new RCS(filename);
    systemFile = true;
}


QString ProjectPanel::getCurrentFileName()
{
    if (rcs!=NULL)  return rcs->getFileName();
    return "";
}

RCS * ProjectPanel::getRCS()
{
    return rcs;
}

void ProjectPanel::findObject(FWObject *o)
{
    findWhereUsedWidget->hide();
    if (fwbdebug) qDebug("ProjectPanel::findObject");
    findObjectWidget->findObject(o);
    m_panel->auxiliaryPanel->show();

}

void ProjectPanel::closeAuxiliaryPanel()
{
    m_panel->auxiliaryPanel->hide();
}

void ProjectPanel::closeEditorPanel()
{
    m_panel->objectEditorFrame->hide();
}

void ProjectPanel::openEditorPanel()
{
    m_panel->objectEditorFrame->adjustSize();
    m_panel->objectEditorFrame->show();

}

void ProjectPanel::search()
{
    findWhereUsedWidget->hide();
    m_panel->auxiliaryPanel->show();
    findObjectWidget->show();
}


void ProjectPanel::compile()
{
    if (isEditorVisible() &&
        !requestEditorOwnership(NULL,NULL,ObjectEditor::optNone,true))
        return;

    fileSave();
    mainW->compile();
}

void ProjectPanel::compile(set<Firewall*> vf)
{
    if (isEditorVisible() &&
        !requestEditorOwnership(NULL,NULL,ObjectEditor::optNone,true))
        return;

    fileSave();
    mainW->compile(vf);
}

void ProjectPanel::install(set<Firewall*> vf)
{
    mainW->install(vf);
}

void ProjectPanel::install()
{
    mainW->install();
}

void ProjectPanel::rollBackSelectionSameWidget()
{
    editorOwner->setFocus();
    emit restoreSelection_sign(true);
}

void ProjectPanel::rollBackSelectionDifferentWidget()
{
    editorOwner->setFocus();
    emit restoreSelection_sign(false);
}


QString ProjectPanel::printHeader()
{
    QString headerText = rcs->getFileName().section("/",-1,-1);
    if (rcs->isInRCS())
        headerText = headerText + ", rev " + rcs->getSelectedRev();
    return headerText;
}

void ProjectPanel::releaseEditor()
{
    disconnect( SIGNAL(restoreSelection_sign(bool)) );
}

void ProjectPanel::connectEditor(QWidget *w)
{
    connect(this, SIGNAL(restoreSelection_sign(bool)),
            w, SLOT(restoreSelection(bool)));
}

bool ProjectPanel::requestEditorOwnership(QWidget *w,
                                          FWObject *obj,
                                          ObjectEditor::OptType otype,
                                          bool validate)
{
    if (!isEditorVisible()) return false;

    if(obj == getOpenedEditor() &&
       otype == getOpenedOptEditor() &&
       w == editorOwner )
    {
        releaseEditor();
        editorOwner = w;
        connectEditor(editorOwner);
        return true;
    }

    if (validate && !validateAndSaveEditor())
    {
        /*
         * roll back selection in the widget that currently
         * owns the editor. Signal restoreSelection_sign
         * is still connected to the previous owner
         */
        if (w == editorOwner )
            QTimer::singleShot(0, this, SLOT(rollBackSelectionSameWidget()));
        else
            QTimer::singleShot(0,this,SLOT(rollBackSelectionDifferentWidget()));
        return false;
    }

    if (w)
    {
        releaseEditor();
        editorOwner = w;
        connectEditor(editorOwner);
    }
    return true;
}

// NOTE: add-on libraries are obsolete in v3.0.0
// TODO: remove this.
listOfLibraries *ProjectPanel::getAddOnLibs()
{
    return addOnLibs;
}

bool ProjectPanel::isSystem(FWObject *obj)
{
    return objectTreeFormat->isSystem(obj);
}

bool ProjectPanel::isStandardId(FWObject *obj)
{
    return objectTreeFormat->isStandardId(obj);
}
    
bool ProjectPanel::validateForInsertion(FWObject *target, FWObject *obj)
{
    return objectTreeFormat->validateForInsertion(target, obj);
}

bool ProjectPanel::getCopyMenuState(const QString &objPath)
{
    return objectTreeFormat->getCopyMenuState(objPath);
}

bool ProjectPanel::getCutMenuState(const QString &objPath)
{
    return objectTreeFormat->getCutMenuState(objPath);
}

bool ProjectPanel::getPasteMenuState(const QString &objPath)
{
    return objectTreeFormat->getPasteMenuState(objPath);
}

bool ProjectPanel::getDeleteMenuState(const QString &objPath)
{
    return objectTreeFormat->getDeleteMenuState(objPath);
}

FWObject* ProjectPanel::createNewLibrary(FWObjectDatabase *db)
{
    return objectTreeFormat->createNewLibrary(db);
}

void ProjectPanel::scheduleRuleSetRedraw()
{
    if (!ruleSetRedrawPending)
    {
        ruleSetRedrawPending = true;
        redrawRuleSets();
        //QTimer::singleShot( 0, this, SLOT(redrawRuleSets()) );
    }
}

void ProjectPanel::redrawRuleSets()
{
    ruleSetRedrawPending = false;
    reopenFirewall();
}

void ProjectPanel::findWhereUsed(FWObject * obj)
{
    findObjectWidget->hide();
    m_panel->auxiliaryPanel->show();
    findWhereUsedWidget->find(obj);
}

void ProjectPanel::showEvent( QShowEvent *ev)
{ 
    if (fwbdebug)
    {
        qDebug("ProjectPanel::showEvent");
        int wf = windowFlags();
        qDebug("windowFlags=%x", wf);
    }
    QWidget::showEvent(ev);
}


void ProjectPanel::hideEvent( QHideEvent *ev)
{
    if (fwbdebug) qDebug("ProjectPanel::hideEvent");
    QWidget::hideEvent(ev);
}


void ProjectPanel::closeEvent( QCloseEvent * ev)
{   
    if (fwbdebug) qDebug("ProjectPanel::closeEvent");

//    if (!closing)
    saveState();
    //storeLastOpenedLib();

    if (fwbdebug)
        qDebug("ProjectPanel::closeEvent check in and delete RCS object");

    if (saveIfModified() && checkin(true))
    {
        if (rcs)
        { 
            delete rcs;
            rcs = 0;
        }
    } else
    {
        ev->ignore();
        return;
    }
    closing = true ;
    QWidget::closeEvent(ev);

    if (fwbdebug)
        qDebug("ProjectPanel::closeEvent main window houskeeping tasks");

    mw->updateWindowTitle();

    QTimer::singleShot( 0, mw, SLOT(projectWindowClosed()) );

    if (fwbdebug) qDebug("ProjectPanel::closeEvent all done");
}

QString ProjectPanel::getFileName()
{
    if (rcs!=NULL)
    {
        QString FileName = rcs->getFileName();
        return FileName;
    }
    else
        return "";
}

void ProjectPanel::splitterMoved(int , int)
{
}

void ProjectPanel::resizeEvent(QResizeEvent*)
{
}

ProjectPanel * ProjectPanel::clone(ProjectPanel * cln)
{
    cln->mainW = mainW;
    cln->rcs = rcs;
    cln->addOnLibs = addOnLibs;
    //cln->objectTreeFormat = objectTreeFormat;
    cln->systemFile = systemFile;
    cln->safeMode = safeMode;
    cln->editingStandardLib = editingStandardLib;
    cln->editingTemplateLib = editingTemplateLib;
    cln->ruleSetRedrawPending = ruleSetRedrawPending;
    cln->startupFileName = startupFileName;
    //cln->objdb = objdb;
    cln->editorOwner = editorOwner;
    cln->oe = oe;
    cln->fd = fd;
    cln->shownInInfo = shownInInfo;
    cln->autosaveTimer = autosaveTimer;
    //cln->ruleSetViews = ruleSetViews;
    cln->ruleSetTabIndex = ruleSetTabIndex;
    cln->visibleFirewall = visibleFirewall;
    cln->firewalls = firewalls;
    cln->lastFirewallIdx = lastFirewallIdx;
    cln->changingTabs = changingTabs;
    cln->noFirewalls = noFirewalls;
    cln->mdiWindow = mdiWindow ;
    cln->m_panel = m_panel;
    cln->findObjectWidget = findObjectWidget;
    cln->findWhereUsedWidget = findWhereUsedWidget;
    cln->copySet = copySet;
    return cln;
}

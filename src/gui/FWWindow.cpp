/*

                          Firewall Builder

                 Copyright (C) 2003, 2006 NetCitadel, LLC

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


#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"

#include "FWWindow.h"
#include "ObjectTreeView.h"
#include "ObjectManipulator.h"
#include "FWObjectClipboard.h"
#include "FWBTree.h"
#include "FWBSettings.h"
#include "FWObjectPropertiesFactory.h"
#include "upgradePredicate.h"
#include "listOfLibraries.h"
#include "ObjConflictResolutionDialog.h"
#include "RuleSetView.h"
#include "ObjectEditor.h"
#include "execDialog.h"
#include "PrefsDialog.h"
#include "LibExportDialog.h"
#include "findDialog.h"
#include "DiscoveryDruid.h"
#include "FindObjectWidget.h"
#include "FindWhereUsedWidget.h"
#include "longTextDialog.h"

#include "FWBAboutDialog.h"
#include "debugDialog.h"
#include "filePropDialog.h"

#include "instConf.h"
#include "instDialog.h"

#include "fwbuilder/FWReference.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/InterfacePolicy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/dns.h"
//#include "fwbuilder/crypto.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/RuleElement.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/RuleSet.h"

#include "fwbuilder/FWObject.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <memory.h>

#include <algorithm>

#ifndef _WIN32
#  include <unistd.h>     // for access(2)
#else
#  undef index
#endif

#include <qaction.h>
#include <qlistwidget.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qheaderview.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qstringlist.h>
#include <qmenu.h>
#include <qtoolbutton.h>

#include <qlayout.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qsplitter.h>
#include <qtimer.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qprinter.h>
#include <qstackedwidget.h>
#include <qlistwidget.h>
#include <qeventloop.h>
#include <qtextstream.h>
#include <QCloseEvent>
#include <QShowEvent>
#include <QList>
#include <QHideEvent>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>
#include "ProjectPanel.h"
using namespace libfwbuilder;
using namespace std;
using namespace Ui;

FWWindow::FWWindow() : QMainWindow(),   // QMainWindow(NULL, Qt::Desktop),
    m_space(0),
    instd(0), autosaveTimer(new QTimer(static_cast<QObject*>(this))), 
    instDialogOnScreenTimer(new QTimer(static_cast<QObject*>(this))), 
    printer(0), searchObject(0), replaceObject(0)
{
    if (fwbdebug)
    {
        qDebug("FWWindow constructor");
        qDebug("windowFlags=%x", int(windowFlags()));
    }

    setUnifiedTitleAndToolBarOnMac(true);

    m_mainWindow = new Ui::FWBMainWindow_q();
    m_mainWindow->setupUi(dynamic_cast<QMainWindow*>(this));

    m_space = new QMdiArea(this);
    setCentralWidget(m_space);
    ProjectPanel *proj = newProjectPanel();
    showSub(proj);
    
    setSafeMode(false);
    setStartupFileName("");

    printer = new QPrinter(QPrinter::HighResolution);

    connect(instDialogOnScreenTimer, SIGNAL(timeout()), this, SLOT(killInstDialog()));
    instDialogOnScreenTimer->start(1000);


    connect( m_mainWindow->newObjectAction, SIGNAL( triggered() ),
             this,              SLOT(newObject() ) );

    connect( m_mainWindow->backAction, SIGNAL( triggered() ),
             this,         SLOT(back() ) );

    connect( m_mainWindow->findAction, SIGNAL( triggered() ),
             this,       SLOT(search()) );

    connect( m_mainWindow->ObjectMenu, SIGNAL (aboutToShow() ),
            this,     SLOT( prepareObjectMenu()  ));
    connect( m_space, SIGNAL(subWindowActivated (QMdiSubWindow *)),this, SLOT(changeActiveSubwindow())); 
    if (fwbdebug)
        qDebug("/FWWindow constructor");

    recreateWindowsMenu();    
//    findObject->setMinimumSize( QSize( 0, 0 ) );
}

FWWindow::~FWWindow()
{
    delete m_mainWindow;
}

ProjectPanel *FWWindow::newProjectPanel()
{
    ProjectPanel *projectW = new ProjectPanel(m_space);
    projectW->initMain(this);
    
    return projectW;
}

void FWWindow::showSub(ProjectPanel *projectW)
{
    QMdiSubWindow *sub = new QMdiSubWindow;
    projectW->mdiWindow = sub;
    sub->setWidget(projectW);
    sub->setAttribute(Qt::WA_DeleteOnClose);
    m_space->addSubWindow(sub);
    QIcon p(":Icons/Firewall/icon-tree");
    sub->setWindowIcon(p); 

    connect(sub,
            SIGNAL(windowStateChanged(Qt::WindowStates,Qt::WindowStates)),
            projectW,
            SLOT(stateChanged(Qt::WindowStates, Qt::WindowStates )));
         

    if (st->getInt("Window/maximized"))
        projectW->setWindowState(Qt::WindowMaximized);

    sub->show();
}

ProjectPanel* FWWindow::activeProject()
{
    QMdiSubWindow *w = m_space->currentSubWindow();
    if (w)
        return dynamic_cast<ProjectPanel*>(w->widget());
    return 0;
}

void FWWindow::killInstDialog()
{
    if (instd!=NULL && !instd->isVisible())
    {
        if (fwbdebug) qDebug("killing instDialog...");
        delete instd;
        instd = NULL;
    }
}

void FWWindow::updateWindowTitle ()
{
    if (activeProject())
    {
        setWindowTitle("Firewall Builder " + activeProject()->getFileName());
    }
    else
    {
        setWindowTitle("Firewall Builder");
    }
}

void FWWindow::startupLoad()
{
    if (activeProject())
    {
        activeProject()->startupLoad();
        activeProject()->readyStatus(true);
        activeProject()->loadState();
    }
}

void FWWindow::helpAbout()
{
    FWBAboutDialog ad;
    ad.exec();
}

void FWWindow::debug()
{
    debugDialog dd(this);
    dd.exec();
}

void FWWindow::info(FWObject *obj, bool forced)
{
    if (fwbdebug)
        qDebug("FWWindow::info called");
    if (activeProject())
        activeProject()->info(obj, forced);
}

bool FWWindow::saveIfModified()
{
    if (activeProject())
        return activeProject()->saveIfModified();
    return false;
}

QString FWWindow::getDestDir(const QString &fname)
{
   if (activeProject())
       return activeProject()->getDestDir(fname);
   return "";
}

QString FWWindow::chooseNewFileName(const QString &fname,
                                    bool checkPresence,const QString &title)
{  
    if (activeProject())
        return activeProject()->chooseNewFileName(fname, checkPresence, title);
    return "";
}

void FWWindow::setFileName(const QString &fname)
{
    if (activeProject())
        activeProject()->setFileName(fname);
}

void FWWindow::fileProp()
{
    if (activeProject())
        activeProject()->fileProp();
}

void FWWindow::fileNew()
{
    ProjectPanel *proj = newProjectPanel();
    if (proj->fileNew())
    {
    	showSub(proj);
        proj->startupLoad();
    }
    else
    {
        delete proj ;  
    }
}

void FWWindow::addToRCSActionSetEn(bool en)
{
    m_mainWindow->addToRCSAction->setEnabled(en);
}

void FWWindow::fileDiscardActionSetEn(bool en)
{
    m_mainWindow->fileDiscardAction->setEnabled(en);
}

void FWWindow::fileCommitActionSetEn(bool en)
{
    m_mainWindow->fileCommitAction->setEnabled(en);
}

void FWWindow::fileSaveActionSetEn(bool en)
{
    m_mainWindow->fileSaveAction->setEnabled(en);
}

void FWWindow::fileOpen()
{
    std::auto_ptr<ProjectPanel> proj(newProjectPanel());
    if (proj->fileOpen())
    {
        if (activeProject()!=NULL)
        {
            if (activeProject()->getRCS()!=NULL)
            {
                if (activeProject()->getRCS()->getFileName()=="")
                {
                    m_space->removeSubWindow(m_space->currentSubWindow());
                }	
            }
            else
            {
                m_space->removeSubWindow(m_space->currentSubWindow());
            }
        }
        showSub(proj.get());
        proj->readyStatus(true);
        proj->loadState();
        proj.release();
    }
    recreateWindowsMenu();
}

void FWWindow::fileClose()
{
    if (activeProject())
        activeProject()->fileClose();
    recreateWindowsMenu();
}

void FWWindow::fileSave()
{
    if (activeProject())
        activeProject()->fileSave();
}

void FWWindow::fileSaveAs()
{
    if (activeProject())
        activeProject()->fileSaveAs();
}

void FWWindow::fileExit()
{
    if (activeProject())
    {
        QList<QMdiSubWindow *> subWindowList = m_space->subWindowList();
        for (int i = 0 ; i < subWindowList.size(); i++)
        {
            ProjectPanel * project = dynamic_cast<ProjectPanel*>(subWindowList[i]->widget ());
            if (project!=NULL)
            {
                project->fileClose();
            }
        }
 //       activeProject()->fileExit();
    }
 //   else
        qApp->quit();
}

void FWWindow::fileCommit()
{
    if (activeProject())
        activeProject()->fileCommit();
}

/*
 * discard changes done to the file and check out clean copy of the
 * head revision from RCS
 */
void FWWindow::fileDiscard()
{
    if (activeProject())
        activeProject()->fileDiscard();
}

void FWWindow::fileAddToRCS()
{
    if (activeProject())
        activeProject()->fileAddToRCS();
}

bool FWWindow::editingLibrary()
{
    if (activeProject())
        return activeProject()->editingLibrary();
    return false;
}

void FWWindow::toolsDiscoveryDruid()
{
    DiscoveryDruid druid(this);
    druid.exec();
}

void FWWindow::importPolicy()
{
    DiscoveryDruid druid(this, true);
    druid.exec();
}

void FWWindow::load(QWidget *dialogs_parent)
{
    if (activeProject())
        activeProject()->load(dialogs_parent);
}

libfwbuilder::FWObject* FWWindow::getVisibleFirewalls()
{ 
    if (activeProject())
        return activeProject()->getVisibleFirewall(); 
    return 0;
}

void FWWindow::load(QWidget *dialogs_parent,RCS *_rcs)
{
    if (activeProject())
        activeProject()->load(dialogs_parent, _rcs);
}

bool FWWindow::checkin(bool unlock)
{
    if (activeProject())
        return activeProject()->checkin(unlock);
    return false;
}

void FWWindow::save()
{
    if (activeProject())
        activeProject()->save();
}

void FWWindow::loadLibrary(const string &libfpath)
{
    if (activeProject())
        activeProject()->loadLibrary(libfpath);
}

void FWWindow::fileImport()
{
    if (activeProject())
        activeProject()->fileImport();
}


void FWWindow::fileCompare()
{
    if (activeProject())
        activeProject()->fileCompare();
}

void FWWindow::findExternalRefs(FWObject *lib,
                                       FWObject *root,
                                       list<FWReference*> &extRefs)
{
    if (activeProject())
        activeProject()->findExternalRefs(lib, root, extRefs);
}

void FWWindow::setSafeMode(bool f)
{
    if (activeProject())
        activeProject()->setSafeMode(f);
}

void FWWindow::setStartupFileName(const QString &fn)
{
    if (activeProject())
        activeProject()->setStartupFileName(fn);
}

bool FWWindow::exportLibraryTest(list<FWObject*> &selectedLibs)
{
/* VERY IMPORTANT: External library file must be self-contained,
 * otherwise it can not be exported.
 *
 * check if selected libraries have references to objects in other
 * libraries (not exported to the same file). Exporting such libraries
 * pulls in other ones because of these references. This is confusing
 * because it means we end up with multiple copies of such objects (in
 * exported library file and in user's data file). When user imports
 * this library and opens their file, it is impossible to say which
 * library an object belongs to.
 *
 * This is prohibited. We check if exported set of libraries has such
 * references and refuse to export it. The user is supposed to clean
 * it up by either moving objects into the library they are trying to
 * export, or by rearranging objects. The only exception for this is
 * library "Standard", which is assumed to be always present so we can
 * have references to objects in it.
 */
    if (activeProject())
        return activeProject()->exportLibraryTest(selectedLibs);
    return false;
}

void FWWindow::exportLibraryTo(QString fname,list<FWObject*> &selectedLibs, bool rof)
{
    if (activeProject())
        activeProject()->exportLibraryTo(fname,selectedLibs, rof);
}

void FWWindow::fileExport()
{
    if (activeProject())
        activeProject()->fileExport();
}

void FWWindow::setActionsEnabled(bool en)
{
    m_mainWindow->insertRuleAction->setEnabled(en);
    m_mainWindow->moveRuleAction->setEnabled(en );
    m_mainWindow->moveRuleUpAction->setEnabled(en );
    m_mainWindow->moveRuleDownAction->setEnabled(en );
    m_mainWindow->addRuleAfterCurrentAction->setEnabled(en );
    m_mainWindow->removeRuleAction->setEnabled(en );
    m_mainWindow->copyRuleAction->setEnabled(en );
    m_mainWindow->cutRuleAction->setEnabled(en );
    m_mainWindow->pasteRuleAboveAction->setEnabled(en );
    m_mainWindow->pasteRuleBelowAction->setEnabled(en );

    m_mainWindow->compileAction->setEnabled(en );
    m_mainWindow->installAction->setEnabled(en );
}

int  FWWindow::findFirewallInList(FWObject *f)
{
    if (activeProject())
        return activeProject()->findFirewallInList(f);
    return -1;
}

void FWWindow::ensureObjectVisibleInRules(FWReference *obj)
{
    if (activeProject())
        activeProject()->ensureObjectVisibleInRules(obj);
}

/*
 * Make rule visible and highlight given column
 */
void FWWindow::ensureRuleIsVisible(Rule *rule, int col)
{
    if (activeProject())
        activeProject()->ensureRuleIsVisible(rule, col);
}

void FWWindow::updateRuleSetViewSelection()
{
    if (activeProject())
        activeProject()->updateRuleSetViewSelection();
}

void FWWindow::updateTreeViewItemOrder()
{
    //this is for case when tree becomes to be resorted
    //if we do not reopen parent item, some of child
    //items mix incorrectly (maybe bug of QT?)
    if (activeProject())
        activeProject()->updateTreeViewItemOrder();
}

void FWWindow::updateRuleSetView()
{
    if (activeProject())
        activeProject()->updateRuleSetView();
}

void FWWindow::updateRuleOptions()
{
    if (activeProject())
        activeProject()->updateRuleOptions();
}

void FWWindow::updateFirewallName(FWObject *obj,const QString &str)
{
    if (activeProject())
        activeProject()->updateFirewallName(obj, str);
}

void FWWindow::scheduleRuleSetRedraw()
{
    if (activeProject())
        activeProject()->scheduleRuleSetRedraw();
}

void FWWindow::redrawRuleSets()
{
    if (activeProject())
        activeProject()->redrawRuleSets();
}

void FWWindow::reopenFirewall()
{
    if (activeProject())
        activeProject()->reopenFirewall();
}

void FWWindow::setEnabledAfterRF()
{
    m_mainWindow->insertRuleAction->setEnabled( true );
    m_mainWindow->moveRuleAction->setEnabled( true );
    m_mainWindow->moveRuleUpAction->setEnabled( true );
    m_mainWindow->moveRuleDownAction->setEnabled( true );
    m_mainWindow->addRuleAfterCurrentAction->setEnabled( false );
    m_mainWindow->removeRuleAction->setEnabled( false );
    m_mainWindow->copyRuleAction->setEnabled( false );
    m_mainWindow->cutRuleAction->setEnabled( false );
    m_mainWindow->pasteRuleAboveAction->setEnabled( false );
    m_mainWindow->pasteRuleBelowAction->setEnabled( false );

    m_mainWindow->compileAction->setEnabled( true );
    m_mainWindow->installAction->setEnabled( true );
}

void FWWindow::selectRules()
{
    m_mainWindow->insertRuleAction->setEnabled( true );
    m_mainWindow ->moveRuleAction->setEnabled( true );
    m_mainWindow ->moveRuleUpAction->setEnabled( true );
    m_mainWindow ->moveRuleDownAction->setEnabled( true );
    m_mainWindow ->addRuleAfterCurrentAction->setEnabled( true );
    m_mainWindow ->removeRuleAction->setEnabled( true );
    m_mainWindow ->copyRuleAction->setEnabled( true );
    m_mainWindow ->cutRuleAction->setEnabled( true );
    m_mainWindow ->pasteRuleAboveAction->setEnabled( true );
    m_mainWindow ->pasteRuleBelowAction->setEnabled( true );

    m_mainWindow ->compileAction->setEnabled( true );
    m_mainWindow ->installAction->setEnabled( true );

    if (activeProject())
        activeProject()->selectRules();
}

void FWWindow::unselectRules()
{
    if (activeProject())
        activeProject()->unselectRules();
}

void FWWindow::disableActions(bool havePolicies)
{
    m_mainWindow ->insertRuleAction->setEnabled( havePolicies ); // enabled if there are policies
    m_mainWindow ->moveRuleAction->setEnabled( false );
    m_mainWindow ->moveRuleUpAction->setEnabled( false );
    m_mainWindow ->moveRuleDownAction->setEnabled( false );
    m_mainWindow ->addRuleAfterCurrentAction->setEnabled( false );
    m_mainWindow ->removeRuleAction->setEnabled( false );
    m_mainWindow ->copyRuleAction->setEnabled( false );
    m_mainWindow ->cutRuleAction->setEnabled( false );
    m_mainWindow ->pasteRuleAboveAction->setEnabled( false );
    m_mainWindow ->pasteRuleBelowAction->setEnabled( false );

    m_mainWindow ->compileAction->setEnabled( havePolicies );
    m_mainWindow ->installAction->setEnabled( havePolicies );
}

void FWWindow::editCopy()
{
    if (activeProject())
        activeProject()->editCopy();
}

void FWWindow::editCut()
{
    if (activeProject())
        activeProject()->editCut();
}

void FWWindow::editDelete()
{
    if (activeProject())
        activeProject()->editDelete();
}

void FWWindow::editPaste()
{
    if (activeProject())
        activeProject()->editPaste();
}

void FWWindow::compile()
{
    std::set<libfwbuilder::Firewall*> emp;

    instd = new instDialog(NULL,BATCH_COMPILE,emp);
    instd->show();

//    id->exec();
//    delete id;
}

void FWWindow::compile(set<Firewall*> vf)
{
    if (fwbdebug)
        qDebug("FWWindow::compile preselected %d firewalls", int(vf.size()));


    instDialog *id = new instDialog(NULL,BATCH_COMPILE,vf);

    instd = id;
    instd->show();

//    id->exec();
//    delete id;
}

void FWWindow::install(set<Firewall*> vf)
{
    instDialog *id=new instDialog(NULL,BATCH_INSTALL, vf);

    instd = id;
    instd->show();

//    id->exec();
//    delete id;
}

void FWWindow::install()
{
    std::set<libfwbuilder::Firewall*> emp;
    instd = new instDialog(NULL, BATCH_INSTALL, emp);

    instd->show();

//    id->exec();
//    delete id;
}

void FWWindow::changeInfoStyle()
{
    if (activeProject())
        activeProject()->changeInfoStyle();
}

void FWWindow::insertRule()
{
    if (activeProject())
        activeProject()->insertRule();
}

void FWWindow::addRuleAfterCurrent()
{
    if (activeProject())
        activeProject()->addRuleAfterCurrent();
}

void FWWindow::removeRule()
{
    if (activeProject())
        activeProject()->removeRule();
}

void FWWindow::moveRule()
{
    if (activeProject())
        activeProject()->moveRule();
}

void FWWindow::moveRuleUp()
{
    if (activeProject())
        activeProject()->moveRuleUp();
}

void FWWindow::moveRuleDown()
{
    if (activeProject())
        activeProject()->moveRuleDown();
}

void FWWindow::copyRule()
{
    if (activeProject())
        activeProject()->copyRule();
}

void FWWindow::cutRule()
{
    if (activeProject())
        activeProject()->cutRule();
}

void FWWindow::pasteRuleAbove()
{
    if (activeProject())
        activeProject()->pasteRuleAbove();
}

void FWWindow::pasteRuleBelow()
{
    if (activeProject())
        activeProject()->pasteRuleBelow();
}

void FWWindow::search()
{
    if (activeProject())
        activeProject()->search();
}

void FWWindow::findWhereUsed(FWObject * obj)
{
    if (activeProject())
        activeProject()->findWhereUsed(obj);
}

void FWWindow::showEvent( QShowEvent *ev)
{
    st->restoreGeometry(this, QRect(100,100,750,600) );
    QMainWindow::showEvent(ev);
}

void FWWindow::hideEvent( QHideEvent *ev)
{
    st->saveGeometry(this);
    QMainWindow::hideEvent(ev);
}

void FWWindow::back()
{
    if (activeProject())
        activeProject()->back();
}

void FWWindow::newObject()
{
    if (activeProject())
        activeProject()->newObject();
}

void FWWindow::lockObject()
{
    if (activeProject())
        activeProject()->lockObject();
}

void FWWindow::unlockObject()
{
    if (activeProject())
        activeProject()->unlockObject();
}

void FWWindow::prepareObjectMenu()
{
    if (!activeProject())
        return;
    ObjectTreeView* otv = activeProject()->getCurrentObjectTree();
    m_mainWindow->ObjectUnlockAction->setEnabled(otv->isUnlockable());
    m_mainWindow->ObjectLockAction->setEnabled(otv->isLockable());
}


void FWWindow::setupAutoSave()
{
    if (activeProject())
        activeProject()->setupAutoSave();
}

QString FWWindow::getCurrentFileName()
{
    if (activeProject())
        return activeProject()->getCurrentFileName();
    return "";
}

RCS * FWWindow::getRCS()
{
    if (activeProject())
        return activeProject()->getRCS();
    return 0;
}

void FWWindow::findObject(FWObject *o)
{
    if (activeProject())
        activeProject()->findObject(o);
}

void FWWindow::closeAuxiliaryPanel()
{
    if (activeProject())
        activeProject()->closeAuxiliaryPanel();
}

void FWWindow::closeEditorPanel()
{
    if (activeProject())
        activeProject()->closeEditorPanel();
}

void FWWindow::openEditorPanel()
{
    if (activeProject())
        activeProject()->openEditorPanel();
}

void FWWindow::editPrefs()
{
    PrefsDialog pd(this);
    pd.exec();
}


/*
 * reset tab via callback because calling setCurrentPage from
 * ruleSetTabChanged causes recursive call to ruleSetTabChanged
 */
void FWWindow::restoreRuleSetTab()
{
    if (activeProject())
        activeProject()->restoreRuleSetTab();
}

/*
 *  w       - widget that requests editor ownership (ruleset view or tree)
 *  obj     - object to be opened in the editor
 *  otype   - editor type in case obj is a rule
 * validate - validate and save editor contents
 *
 *  if w==NULL, then request is done by the same widget that owns editor.
 *  just need to run validateAndSave and return result
 *
 *  if obj==NULL, then no new object is to be opened in the editor
 *
 */
bool FWWindow::requestEditorOwnership(QWidget *w,
                                      FWObject *obj,
                                      ObjectEditor::OptType otype,
                                      bool validate)
{
    if (activeProject())
        return activeProject()->requestEditorOwnership(w, obj, otype, validate);
    return false;
}

void FWWindow::editFind()
{
}

void FWWindow::editRedo()
{
}

void FWWindow::editUndo()
{
}

void FWWindow::helpContents()
{
}

void FWWindow::helpContentsAction()
{
}

void FWWindow::helpIndex()
{
}

//wrapers for some ObjectManipulator functions
libfwbuilder::FWObject* FWWindow::getOpened()
{
    if (activeProject())
        return activeProject()->getOpened();
    return 0;
}

libfwbuilder::FWObject* FWWindow::getCurrentLib()
{
    if (activeProject())
        return activeProject()->getCurrentLib();
    return 0;
}


void FWWindow::loadDataFromFw(libfwbuilder::Firewall *fw)
{
    if (activeProject())
        activeProject()->loadDataFromFw(fw);
}

libfwbuilder::FWObject* FWWindow::createObject(const QString &objType,
                                          const QString &objName,
                                          libfwbuilder::FWObject *copyFrom)
{
    if (activeProject())
        return activeProject()->createObject(objType, objName, copyFrom);
    return 0;
}

libfwbuilder::FWObject* FWWindow::createObject(libfwbuilder::FWObject *parent,
                                      const QString &objType,
                                      const QString &objName,
                                      libfwbuilder::FWObject *copyFrom)
{
    if (activeProject())
        return activeProject()->createObject(parent, objType, objName, copyFrom);
    return 0;
}

void FWWindow::moveObject(libfwbuilder::FWObject *target,
                    libfwbuilder::FWObject *obj)
{
    if (activeProject())
        activeProject()->moveObject(target, obj);
}

void FWWindow::moveObject(const QString &targetLibName,
                    libfwbuilder::FWObject *obj)
{
    if (activeProject())
        activeProject()->moveObject(targetLibName, obj);
}

void FWWindow::autorename(libfwbuilder::FWObject *obj,
                    const std::string &objtype,
                    const std::string &namesuffix)
{
    if (activeProject())
        activeProject()->autorename(obj, objtype, namesuffix);
}


void FWWindow::updateLibColor(libfwbuilder::FWObject *lib)
{
    if (activeProject())
        activeProject()->updateLibColor(lib);
}

void FWWindow::updateLibName(libfwbuilder::FWObject *lib)
{
    if (activeProject())
        activeProject()->updateLibName(lib);
}

void FWWindow::updateObjName(libfwbuilder::FWObject *obj,
                       const QString &oldName,
                       bool  askForAutorename)
{
    if (activeProject())
        activeProject()->updateObjName(obj, oldName, askForAutorename);
}

void FWWindow::updateObjName(libfwbuilder::FWObject *obj,
                       const QString &oldName,
                       const QString &oldLabel,
                       bool  askForAutorename)
{
    if (activeProject())
        activeProject()->updateObjName(obj, oldName, oldLabel, askForAutorename);
}


void FWWindow::updateLastModifiedTimestampForOneFirewall(libfwbuilder::FWObject *o)
{
    if (activeProject())
        activeProject()->updateLastModifiedTimestampForOneFirewall(o);
}

void FWWindow::updateLastModifiedTimestampForAllFirewalls(libfwbuilder::FWObject *o)
{
    if (activeProject())
        activeProject()->updateLastModifiedTimestampForAllFirewalls(o);
}

void FWWindow::updateLastInstalledTimestamp(libfwbuilder::FWObject *o)
{
    if (activeProject())
        activeProject()->updateLastInstalledTimestamp(o);
}

void FWWindow::updateLastCompiledTimestamp(libfwbuilder::FWObject *o)
{
    if (activeProject())
        activeProject()->updateLastCompiledTimestamp(o);
}


libfwbuilder::FWObject* FWWindow::pasteTo(libfwbuilder::FWObject *target,
                                    libfwbuilder::FWObject *obj,
                                    bool openobj,
                                    bool validateOnly)
{
    if (activeProject())
        return activeProject()->pasteTo(target, obj, openobj, validateOnly);
    return 0;
}

void FWWindow::delObj(libfwbuilder::FWObject *obj,bool openobj)
{
    if (activeProject())
        activeProject()->delObj(obj, openobj);
}

ObjectTreeView* FWWindow::getCurrentObjectTree()
{
    if (activeProject())
        return activeProject()->getCurrentObjectTree();
    return 0;
}

void FWWindow::openObject(QTreeWidgetItem *otvi)
{
    if (activeProject())
        activeProject()->openObject(otvi);
}

void FWWindow::openObject(libfwbuilder::FWObject *obj)
{
    if (activeProject())
        activeProject()->openObject(obj);
}

bool FWWindow::editObject(libfwbuilder::FWObject *obj)
{
    if (activeProject())
        return activeProject()->editObject(obj);
    return false;
}

void FWWindow::findAllFirewalls (std::list<libfwbuilder::Firewall *> &fws)
{
    if (activeProject())
        activeProject()->findAllFirewalls (fws);
}

libfwbuilder::FWObject* FWWindow::duplicateObject(libfwbuilder::FWObject *target,
                                            libfwbuilder::FWObject *obj,
                                            const QString &name,
                                            bool  askForAutorename)
{
    if (activeProject())
        return activeProject()->duplicateObject(target, obj, name, askForAutorename);
    return 0;
}

void FWWindow::showDeletedObjects(bool f)
{
    if (activeProject())
        activeProject()->showDeletedObjects(f);
}

void FWWindow::select()
{
    if (activeProject())
        activeProject()->select();
}

void FWWindow::unselect()
{
    if (activeProject())
        activeProject()->unselect();
}

void FWWindow::info()
{
    if (activeProject())
        info(activeProject()->getSelectedObject(), true);
}

void FWWindow::setManipulatorFocus()
{
    if (activeProject())
        activeProject()->setManipulatorFocus();
}

void FWWindow::clearManipulatorFocus()
{
    if (activeProject())
        activeProject()->clearManipulatorFocus();
}

//wrapers for some Object Editor functions
bool FWWindow::isEditorVisible()
{
    if (activeProject())
        return activeProject()->isEditorVisible();
    return false;
}

bool FWWindow::isEditorModified()
{
    if (activeProject())
        return activeProject()->isEditorModified();
    return false;
}

void FWWindow::showEditor()
{    
    if (activeProject())
        activeProject()->showEditor();
}

void FWWindow::hideEditor()
{
    if (activeProject())
        activeProject()->hideEditor();
}

void FWWindow::closeEditor()
{
    if (activeProject())
        activeProject()->closeEditor();
}

void FWWindow::openEditor(libfwbuilder::FWObject *o)
{
    if (activeProject())
        activeProject()->openEditor(o);
}

void FWWindow::openOptEditor(libfwbuilder::FWObject *o, ObjectEditor::OptType t)
{
    if (activeProject())
        activeProject()->openOptEditor(o, t);
}

void FWWindow::blankEditor()
{
    if (activeProject())
        activeProject()->blankEditor();
}


libfwbuilder::FWObject* FWWindow::getOpenedEditor()
{
    if (activeProject())
        return activeProject()->getOpenedEditor();
    return 0;
}

ObjectEditor::OptType FWWindow::getOpenedOptEditor()
{
    if (activeProject())
        return activeProject()->getOpenedOptEditor();
    return ObjectEditor::optNone;
}

void FWWindow::selectObjectInEditor(libfwbuilder::FWObject *o)
{
    if (activeProject())
        activeProject()->selectObjectInEditor(o);
}

void FWWindow::actionChangedEditor(libfwbuilder::FWObject *o)
{
    if (activeProject())
        activeProject()->actionChangedEditor(o);
}

bool FWWindow::validateAndSaveEditor()
{
    if (activeProject())
        return activeProject()->validateAndSaveEditor();
    return false;
}

void FWWindow::setFDObject(libfwbuilder::FWObject *o)
{
    if (activeProject())
        activeProject()->setFDObject(o);
}

QPrinter* FWWindow::getPrinter()
{
    return printer;
}

libfwbuilder::FWObjectDatabase* FWWindow::db() 
{ 
    if (activeProject())
        return activeProject()->db(); 
    return 0;
}

QString FWWindow::printHeader()
{
    if (activeProject())
        return activeProject()->printHeader();
    return "";
}

listOfLibraries *FWWindow::getAddOnLibs()
{
    if (activeProject())
        return activeProject()->getAddOnLibs();
    return 0;
}


bool FWWindow::isSystem(libfwbuilder::FWObject *obj)
{
    if (activeProject())
        return activeProject()->isSystem(obj);
    return false;
}


void FWWindow::closeEvent( QCloseEvent*)
{   
    st->setInt("Window/maximized", activeProject()->isMaximized());

    QList<QMdiSubWindow *> subWindowList = m_space->subWindowList();
    for (int i = 0 ; i < subWindowList.size();i++)
    {
        ProjectPanel * pp = dynamic_cast<ProjectPanel *>(subWindowList[i]->widget());
        if (pp!=NULL)
        {
            pp->saveState();
        }
    }
}

void FWWindow::selectActiveSubWindow (/*const QString & text*/)
{
    QObject * sender_ = sender ();
    QAction * act = (QAction*) sender_ ;
    QString text = act->text();
    if (text=="[Noname]")
        text="";
    for (int i = 0 ; i < windowsTitles.size();i++)
    {
        if (windowsTitles[i]==text)
        {
            getMdiArea()->setActiveSubWindow (windowsPainters[i]);
        }
    }
}

void FWWindow::minimize()
{
    if (fwbdebug) qDebug("FWWindow::minimize");
    m_space->activeSubWindow ()->showMinimized ();
    st->setInt("Window/maximized", 0);

    QList<QMdiSubWindow *> subWindowList = m_space->subWindowList();
    for (int i = 0 ; i < subWindowList.size();i++)
    {
        ProjectPanel * pp = dynamic_cast<ProjectPanel *>(subWindowList[i]->widget());
        if (pp!=NULL)
        {
            pp->loadState();
        }
    }
}

void FWWindow::maximize ()
{
    if (fwbdebug) qDebug("FWWindow::maximize");
    m_space->activeSubWindow()->showMaximized ();
    st->setInt("Window/maximized", 1);
}

void FWWindow::recreateWindowsMenu ()
{
    windowsPainters.clear();
    windowsTitles.clear();
    m_mainWindow->menuWindow->clear();
    QAction * close = m_mainWindow->menuWindow->addAction ("Close");
    QAction * closeAll = m_mainWindow->menuWindow->addAction ("Close All");
    QAction * title = m_mainWindow->menuWindow->addAction ("Title");
    QAction * cascade = m_mainWindow->menuWindow->addAction ("Cascade");
    QAction * next = m_mainWindow->menuWindow->addAction ("Next");
    QAction * previous = m_mainWindow->menuWindow->addAction ("Previous");

    QAction * minimize = m_mainWindow->menuWindow->addAction ("Minimize");
    QAction * maximize = m_mainWindow->menuWindow->addAction ("Maximize");
    m_mainWindow->menuWindow->addSeparator ();

    connect(minimize, SIGNAL(triggered()), this, SLOT(minimize()));
    connect(maximize, SIGNAL(triggered()), this, SLOT(maximize()));
    connect(close, SIGNAL(triggered()),m_space, SLOT(closeActiveSubWindow()));
    connect(closeAll, SIGNAL(triggered()),m_space, SLOT(closeAllSubWindows()));
    connect(title, SIGNAL(triggered()), m_space, SLOT(tileSubWindows()));
    connect(cascade, SIGNAL(triggered()), m_space, SLOT(cascadeSubWindows()));
    connect(next, SIGNAL(triggered()),m_space, SLOT(activateNextSubWindow()));
    connect(previous, SIGNAL(triggered()),m_space, SLOT(activatePreviousSubWindow()));
    QList<QMdiSubWindow *> subWindowList = getMdiArea()->subWindowList();
    QActionGroup * ag = new QActionGroup ( this );
    ag->setExclusive ( true );
    for (int i = 0 ; i < subWindowList.size();i++)
    {
        windowsPainters.push_back (subWindowList[i]);
        ProjectPanel * pp = dynamic_cast <ProjectPanel *>(subWindowList[i]->widget());
        if (pp!=NULL)
        {
            if (pp->isClosing())
                continue ;
            QString text = pp->getFileName ();
            windowsTitles.push_back(text);
            if (text=="")
                text = "[Noname]";
            QAction * act = m_mainWindow->menuWindow->addAction (text);
            ag->addAction(act);
            act->setCheckable ( true );
            if (subWindowList[i]==m_space->activeSubWindow ())
                act->setChecked(true);
            connect(act, SIGNAL(triggered()), this, SLOT(selectActiveSubWindow()));
        }
    }

}

void FWWindow::changeActiveSubwindow (  )
{
    recreateWindowsMenu();
}

void FWWindow::updateTreeFont ()
{
    QFont font = st->getTreeFont();
   QList<QMdiSubWindow *> subWindowList = getMdiArea()->subWindowList();
    for (int i = 0 ; i < subWindowList.size();i++)
    {
        ProjectPanel * pp = dynamic_cast <ProjectPanel *>(subWindowList[i]->widget());
        if (pp!=NULL)
        {
            std::vector<QTreeWidget*> trees = pp->m_panel->om->getTreeWidgets();
            for (unsigned int o = 0 ; o < trees.size(); o++)
            {
                trees[o]->setFont(font);
            }
        }
    }
}

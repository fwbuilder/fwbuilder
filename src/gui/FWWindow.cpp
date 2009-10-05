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

#include "../../config.h"
#include "global.h"
#include "check_update_url.h"
#include "../../VERSION.h"

#include "utils.h"
#include "utils_no_qt.h"

#include <ui_FWBMainWindow_q.h>
#include <ui_pagesetupdialog_q.h>

#include "FWWindow.h"
#include "ProjectPanel.h"
#include "ObjectTreeView.h"
#include "ObjectManipulator.h"
#include "FWObjectClipboard.h"
#include "FWBTree.h"
#include "FWBSettings.h"
#include "FWObjectPropertiesFactory.h"
#include "upgradePredicate.h"
#include "ObjConflictResolutionDialog.h"
#include "RuleSetView.h"
#include "ObjectEditor.h"
#include "PrefsDialog.h"
#include "LibExportDialog.h"
#include "findDialog.h"
#include "DiscoveryDruid.h"
#include "FindObjectWidget.h"
#include "FindWhereUsedWidget.h"
#include "longTextDialog.h"
#include "Help.h"

#include "FWBAboutDialog.h"
#include "debugDialog.h"
#include "filePropDialog.h"

#include "instConf.h"
#include "instDialog.h"
#include "HttpGet.h"
#include "StartTipDialog.h"

#include "transferDialog.h"

#include "events.h"

#include "fwbuilder/FWReference.h"
#include "fwbuilder/Policy.h"
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
#include <memory>
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
#include <QUrl>
#include <QDockWidget>


using namespace libfwbuilder;
using namespace std;
using namespace Ui;

FWWindow::FWWindow() : QMainWindow(),   // QMainWindow(NULL, Qt::Desktop),
                       m_space(0),
                       previous_subwindow(0),
                       instd(0),
                       instDialogOnScreenTimer(
                           new QTimer(static_cast<QObject*>(this))), 
                       editorOwner(0),
                       printer(0), searchObject(0), replaceObject(0),
                       auto_load_from_rcs_head_revision(0),
                       oe(0), findObjectWidget(0), findWhereUsedWidget(0)
{
    setUnifiedTitleAndToolBarOnMac(true);

    m_mainWindow = new Ui::FWBMainWindow_q();
    m_mainWindow->setupUi(dynamic_cast<QMainWindow*>(this));

    //setCentralWidget(m_space);

    psd = NULL;

    prepareFileOpenRecentMenu();

    // ProjectPanel *proj = newProjectPanel();
    // showSub(proj);
    
#ifdef Q_OS_MACX
    m_mainWindow->m_space->setViewMode(QMdiArea::TabbedView);
#endif    

    findObjectWidget = new FindObjectWidget(
        m_mainWindow->auxiliaryPanel, NULL, "findObjectWidget");
    findObjectWidget->setFocusPolicy( Qt::NoFocus );
    m_mainWindow->auxiliaryPanel->layout()->addWidget( findObjectWidget );
    findObjectWidget->show();

    findWhereUsedWidget = new FindWhereUsedWidget(
        m_mainWindow->auxiliaryPanel, NULL, "findWhereUsedWidget");
    findWhereUsedWidget->setFocusPolicy( Qt::NoFocus );
    m_mainWindow->auxiliaryPanel->layout()->addWidget( findWhereUsedWidget );
    findWhereUsedWidget->hide();

    oe  = new ObjectEditor((QWidget*)m_mainWindow->objectEditorStack);
    //oe->setCloseButton(m_panel->closeObjectEditorButton);
    oe->setApplyButton(m_mainWindow->applyObjectEditorButton);
    oe->setHelpButton(m_mainWindow->helpObjectEditorButton);

    m_mainWindow->editorDockWidget->setupEditor(oe);
    m_mainWindow->editorDockWidget->hide();

    printer = new QPrinter(QPrinter::HighResolution);

    current_version_http_getter = new HttpGet();
    connect(current_version_http_getter, SIGNAL(done(const QString&)),
            this, SLOT(checkForUpgrade(const QString&)));

    connect(instDialogOnScreenTimer, SIGNAL(timeout()),
            this, SLOT(killInstDialog()));

    instDialogOnScreenTimer->start(1000);


    connect( m_mainWindow->newObjectAction, SIGNAL( triggered() ),
             this, SLOT(newObject() ) );

    connect( m_mainWindow->backAction, SIGNAL( triggered() ),
             this, SLOT(back() ) );

    connect( m_mainWindow->findAction, SIGNAL( triggered() ),
             this, SLOT(search()) );

    connect( m_mainWindow->editMenu, SIGNAL (aboutToShow() ),
            this,  SLOT( prepareEditMenu()  ));

    connect( m_mainWindow->viewMenu, SIGNAL (aboutToShow() ),
            this,  SLOT( prepareViewMenu()  ));

    connect( m_mainWindow->ObjectMenu, SIGNAL (aboutToShow() ),
            this,  SLOT( prepareObjectMenu()  ));

    connect( m_mainWindow->fileMenu, SIGNAL (aboutToShow() ),
            this,  SLOT( prepareFileMenu()  ));

    connect( m_mainWindow->toolsMenu, SIGNAL (aboutToShow() ),
            this,  SLOT( prepareToolsMenu()  ));

    connect( m_mainWindow->menuWindow, SIGNAL (aboutToShow() ),
            this,  SLOT( prepareWindowsMenu()  ));

    connect( m_mainWindow->m_space, SIGNAL(subWindowActivated(QMdiSubWindow*)),
             this, SLOT(subWindowActivated(QMdiSubWindow*))); 

    disableActions(false);

    ProjectPanel *proj = newProjectPanel();
    showSub(proj);

    setSafeMode(false);

//    findObject->setMinimumSize( QSize( 0, 0 ) );
}

FWWindow::~FWWindow()
{
    delete m_mainWindow;
}

void FWWindow::prepareFileOpenRecentMenu()
{
    for (int i = 0; i < MAXRECENTFILES; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));

        m_mainWindow->menuOpen_Recent->addAction(recentFileActs[i]);
    }
    openRecentSeparatorAct = m_mainWindow->menuOpen_Recent->addSeparator();
    m_mainWindow->menuOpen_Recent->addAction(
        m_mainWindow->actionClearRecentFiles);
    updateRecentFileActions();
}

void FWWindow::clearRecentFilesMenu()
{
    QStringList empty_list;
    st->setRecentFiles(empty_list);
    updateRecentFileActions();
}

void FWWindow::updateRecentFileActions()
{
    QStringList files = st->getRecentFiles();
    QMap<QString, int> file_name_counters;

    int numRecentFiles = qMin(files.size(), (int)MAXRECENTFILES);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString file_name = QFileInfo(files[i]).fileName();
        int c = file_name_counters[file_name]; // default constructed value is 0
        file_name_counters[file_name] = c + 1;
    }

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString file_name = QFileInfo(files[i]).fileName();
        int c = file_name_counters[file_name];
        // if c > 1, we have two files with the same name but different path
        QString text = (c > 1) ? files[i] : file_name;
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MAXRECENTFILES; ++j)
        recentFileActs[j]->setVisible(false);

    openRecentSeparatorAct->setVisible(numRecentFiles > 0);
}

/*
 * Add file name to the "File/Open Recent" menu.
 */
void FWWindow::updateOpenRecentMenu(const QString &fileName)
{
    QStringList files = st->getRecentFiles();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MAXRECENTFILES)
        files.removeLast();

    st->setRecentFiles(files);

    updateRecentFileActions();
}

void FWWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString(), false);
}

void FWWindow::registerAutoOpenDocFile(const QString &filename,
                                       bool load_from_rcs_head)
{
    openDocFiles.append(filename);
    auto_load_from_rcs_head_revision = load_from_rcs_head;
}

ProjectPanel *FWWindow::newProjectPanel()
{
    ProjectPanel *projectW = new ProjectPanel(m_mainWindow->m_space);
    projectW->initMain(this);
    return projectW;
}

void FWWindow::showSub(ProjectPanel *pp)
{
    QMdiSubWindow *sub = new QMdiSubWindow;
    pp->mdiWindow = sub;
    sub->setWidget(pp);
    sub->setAttribute(Qt::WA_DeleteOnClose);
    m_mainWindow->m_space->addSubWindow(sub);

    if (st->getInt("Window/maximized"))
        pp->setWindowState(Qt::WindowMaximized);

    sub->show();
    /*
     * for reasons I do not understand, QMdiArea does not send signal
     * subWindowActivated when the very first subwindow comes up. I
     * think it should, but at least QT 4.4.1 on Mac does not do
     * it. Calling the slot manually so that editor panel can be
     * attached to the current project panel.
     */
    attachEditorToProjectPanel(pp);
}

ProjectPanel* FWWindow::activeProject()
{
    QList<QMdiSubWindow*> subwindows = m_mainWindow->m_space->subWindowList(
        QMdiArea::StackingOrder);
    if (subwindows.size() == 0) return NULL;
    QMdiSubWindow *w = subwindows.last(); // last item is the topmost window

    // QMdiSubWindow *w = m_mainWindow->m_space->currentSubWindow();
    // if (w) return dynamic_cast<ProjectPanel*>(w->widget());
    // if (fwbdebug)
    //     qDebug() << "FWWindow::activeProject(): currentSubWindow() returns NULL, trying activeSubWindow()";
    // w = m_mainWindow->m_space->activeSubWindow();

    if (w) return dynamic_cast<ProjectPanel*>(w->widget());
    return NULL;
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

void FWWindow::updateWindowTitle()
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
    if (st->getCheckUpdates())
    {
        // start http query to get latest version from the web site
        QString url = QString(CHECK_UPDATE_URL).arg(VERSION).arg(st->getAppGUID());
        current_version_http_getter->get(QUrl(url));
    }

    if (fwbdebug)
        qDebug() << "FWWindow::startupLoad 1 "
                 << " activeProject()=" << activeProject();

    if (activeProject())
    {
        activeProject()->loadStandardObjects();
        activeProject()->readyStatus(true);
        activeProject()->loadState(true);
    }

    if (fwbdebug)
        qDebug() << "FWWindow::startupLoad 2 "
                 << " activeProject()=" << activeProject();

    // foreach is QT macro
    foreach (QString file, openDocFiles)
    {
        loadFile(file, auto_load_from_rcs_head_revision);
        updateOpenRecentMenu(file);
    }

//    m_mainWindow->m_space->setActiveSubWindow(
//        m_mainWindow->m_space->currentSubWindow());

    if (fwbdebug)
        qDebug() << "FWWindow::startupLoad 3 "
                 << " activeProject()=" << activeProject();

    QString release_notes_flag = QString("UI/%1/ReleaseNotesShown").arg(VERSION);
//    if (!st->getBool(release_notes_flag))
//    {
    showReleaseNotes();
    st->setBool(release_notes_flag, true);
//    }

    if (! st->getBool("UI/NoStartTip"))
    {
        StartTipDialog *stdlg = new StartTipDialog();
        stdlg->run();

        //stdlg->show();
        //stdlg->raise();
    }

    if (fwbdebug)
        qDebug() << "FWWindow::startupLoad 4 "
                 << " activeProject()=" << activeProject();

    prepareFileMenu();

    if (fwbdebug)
        qDebug() << "FWWindow::startupLoad 5 "
                 << " activeProject()=" << activeProject();
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

void FWWindow::fileNew()
{
    // if the only project panel window that we have shows
    // default object tree (i.e. its filename is empty), then load file
    // into. Otherwise create new project window.
    if (activeProject() && activeProject()->getFileName().isEmpty())
    {
        activeProject()->fileNew();
    } else
    {
        std::auto_ptr<ProjectPanel> proj(newProjectPanel());
        if (proj->fileNew())
        {
            showSub(proj.get());
            prepareFileMenu();
            proj.release();
        }
    }
}

void FWWindow::fileOpen()
{
    QString dir;
    QMdiSubWindow *last_active_window = m_mainWindow->m_space->activeSubWindow();

/*
 * Pick default directory where to look for the file to open.
 * 1) if "work directory" is configured in preferences, always use it
 * 2) if it is blank, use the same directory where currently opened file is
 * 3) if this is the first file to be opened, get directory where the user opened
 *    during last session from settings using st->getOpenFileDir
 */

    dir = st->getWDir();
    if (fwbdebug) qDebug("Choosing directory for file open 1: %s",
                         dir.toStdString().c_str());

    if (dir.isEmpty() && !mw->getCurrentFileName().isEmpty())
        dir = getFileDir(mw->getCurrentFileName());
    if (fwbdebug) qDebug("Choosing directory for file open 2: %s",
                         dir.toStdString().c_str());

    if (dir.isEmpty()) dir = st->getOpenFileDir();
    if (fwbdebug) qDebug("Choosing directory for file open 3: %s",
                         dir.toStdString().c_str());

    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        dir,
        "FWB files (*.fwb *.fwl *.xml);;All Files (*)");
    
    if (fileName.isEmpty())
    {
        m_mainWindow->m_space->setActiveSubWindow(last_active_window);
        return ;
    }

    if (loadFile(fileName, false))
    {
        updateOpenRecentMenu(fileName);
        // reset actions, including Save() which should now
        // be inactive
        prepareFileMenu();
    } else
        m_mainWindow->m_space->setActiveSubWindow(last_active_window);
}

bool FWWindow::loadFile(const QString &file_name, bool load_rcs_head)
{
    ProjectPanel *proj;
    // if the only project panel window that we have shows
    // default object tree (i.e. its filename is empty), then load file
    // into. Otherwise create new project window.
    if (activeProject() && activeProject()->getFileName().isEmpty())
    {
        proj = activeProject();
        if (!proj->loadFile(file_name, load_rcs_head)) return false;
    } else
    {
        proj = newProjectPanel();
        if (proj->loadFile(file_name, load_rcs_head))
        {
            showSub(proj);
        } else
        {
            delete proj;
            return false;
        }
    }
    proj->readyStatus(true);
    proj->loadState(true);
    return true;
}

void FWWindow::fileClose()
{
    if (fwbdebug) qDebug("FWWindow::fileClose()");

    if (activeProject())
    {
        ProjectPanel * project = activeProject();
        if (!project->saveIfModified()) return;  // abort operation
        project->saveState();
        project->fileClose();
    }

    if (fwbdebug) qDebug("subWindowList().size()=%d",
                         m_mainWindow->m_space->subWindowList().size());

}

void FWWindow::fileExit()
{
    if (activeProject())
    {
        QList<QMdiSubWindow *> subWindowList = m_mainWindow->m_space->subWindowList();
        for (int i = 0 ; i < subWindowList.size(); i++)
        {
            ProjectPanel * project =
                dynamic_cast<ProjectPanel*>(subWindowList[i]->widget ());
            if (project!=NULL)
            {
                if (!project->saveIfModified()) return;  // abort operation
                project->saveState();
                project->fileClose();
            }
        }
    }

    QCoreApplication::exit(0);
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

    if (activeProject()) activeProject()->selectRules();
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

    m_mainWindow ->compileAction->setEnabled( true );
    m_mainWindow ->installAction->setEnabled( true );
}

void FWWindow::compile()
{
    std::set<Firewall*> emp;

    instd = new instDialog(NULL, BATCH_COMPILE, emp);
    instd->show();

//    id->exec();
//    delete id;
}

void FWWindow::compile(set<Firewall*> vf)
{
    if (fwbdebug)
        qDebug("FWWindow::compile preselected %d firewalls", int(vf.size()));


    instDialog *id = new instDialog(NULL, BATCH_COMPILE, vf);

    instd = id;
    instd->show();

//    id->exec();
//    delete id;
}

void FWWindow::install(set<Firewall*> vf)
{
    instDialog *id = new instDialog(NULL, BATCH_INSTALL, vf);

    instd = id;
    instd->show();

//    id->exec();
//    delete id;
}

void FWWindow::install()
{
    std::set<Firewall*> emp;
    instd = new instDialog(NULL, BATCH_INSTALL, emp);

    instd->show();

//    id->exec();
//    delete id;
}

void FWWindow::transferfw(set<Firewall*> vf)
{
    transferDialog *ed = new transferDialog(NULL, vf);
    ed->show();
}

void FWWindow::transferfw()
{
    std::set<Firewall*> emp;
    transferDialog *ed = new transferDialog(NULL, emp);
    ed->show();
}

void FWWindow::showEvent(QShowEvent *ev)
{
    st->restoreGeometry(this, QRect(100,100,750,600) );
    QMainWindow::showEvent(ev);
}

void FWWindow::hideEvent(QHideEvent *ev)
{
    st->saveGeometry(this);
    QMainWindow::hideEvent(ev);
}

void FWWindow::prepareEditMenu()
{
    if (!activeProject())
    {
        m_mainWindow->editCopyAction->setEnabled(false);
        m_mainWindow->editDeleteAction->setEnabled(false);
        m_mainWindow->editCutAction->setEnabled(false);
        m_mainWindow->editPasteAction->setEnabled(false);
        return;
    }

    bool dupMenuItem=true;
    bool moveMenuItem=true;
    bool copyMenuItem=true;
    bool pasteMenuItem=true;
    bool delMenuItem=true;
    bool newMenuItem=true;
    bool inDeletedObjects = false;

    activeProject()->m_panel->om->getMenuState(
        false,
        dupMenuItem, moveMenuItem, copyMenuItem, pasteMenuItem,
        delMenuItem, newMenuItem, inDeletedObjects
    );

    m_mainWindow->editCopyAction->setEnabled(copyMenuItem);
    m_mainWindow->editDeleteAction->setEnabled(delMenuItem);
    m_mainWindow->editCutAction->setEnabled(delMenuItem);
    m_mainWindow->editPasteAction->setEnabled(pasteMenuItem);
}

void FWWindow::prepareViewMenu()
{
    if (!activeProject())
    {
        m_mainWindow->actionObject_Tree->setEnabled(false);
        m_mainWindow->actionEditor_panel->setEnabled(false);
        return;
    }

    m_mainWindow->actionObject_Tree->setEnabled(true);
    m_mainWindow->actionEditor_panel->setEnabled(true);

    m_mainWindow->actionObject_Tree->setChecked(
        activeProject()->m_panel->treeDockWidget->isVisible());
    m_mainWindow->actionEditor_panel->setChecked(
        m_mainWindow->editorDockWidget->isVisible());
}

void FWWindow::prepareObjectMenu()
{
    if (!activeProject())
    {
        m_mainWindow->newObjectAction->setEnabled(false);
        m_mainWindow->findAction->setEnabled(false);
        m_mainWindow->ObjectLockAction->setEnabled(false);
        m_mainWindow->ObjectUnlockAction->setEnabled(false);
        return;
    }

    ObjectTreeView* otv = activeProject()->getCurrentObjectTree();
    m_mainWindow->ObjectUnlockAction->setEnabled(otv->isUnlockable());
    m_mainWindow->ObjectLockAction->setEnabled(otv->isLockable());
}

void FWWindow::prepareFileMenu()
{
    if (!activeProject())
    {
        m_mainWindow->fileCloseAction->setEnabled(false);
        m_mainWindow->fileSaveAction->setEnabled(false);
        m_mainWindow->fileSaveAsAction->setEnabled(false);
        m_mainWindow->addToRCSAction->setEnabled(false);
        m_mainWindow->fileCommitAction->setEnabled(false);
        m_mainWindow->fileDiscardAction->setEnabled(false);
        m_mainWindow->filePropAction->setEnabled(false);
        m_mainWindow->libImportAction->setEnabled(false);
        m_mainWindow->policyImportAction->setEnabled(false);
        return;
    }

    bool real_file_opened = (activeProject()->getFileName() != "");
    bool in_rcs = (activeProject()->getRCS() != NULL &&
                   activeProject()->getRCS()->isCheckedOut());
    bool needs_saving = (db() && db()->isDirty());

    // if (fwbdebug)
    //     qDebug("FWWindow::prepareFileMenu(): activeProject()=%p"
    //            "  activeProject()->getFileName()='%s'"
    //            "  real_file_opened=%d  needs_saving=%d",
    //            activeProject(),
    //            activeProject()->getFileName().toAscii().constData(),
    //            real_file_opened, needs_saving);
    
    m_mainWindow->fileSaveAction->setEnabled(real_file_opened && needs_saving);
    m_mainWindow->fileCloseAction->setEnabled(real_file_opened);
    m_mainWindow->filePropAction->setEnabled(real_file_opened);
    m_mainWindow->filePrintAction->setEnabled(real_file_opened);
    m_mainWindow->libExportAction->setEnabled(real_file_opened);

    m_mainWindow->addToRCSAction->setEnabled(real_file_opened && !in_rcs);
    m_mainWindow->fileCommitAction->setEnabled(real_file_opened && in_rcs);
    m_mainWindow->fileDiscardAction->setEnabled(real_file_opened && in_rcs);

    m_mainWindow->fileNewAction->setEnabled(true);
    m_mainWindow->fileOpenAction->setEnabled(true);
    m_mainWindow->fileSaveAsAction->setEnabled(true);
}

void FWWindow::prepareToolsMenu()
{
    m_mainWindow->DiscoveryDruidAction->setEnabled(db()!=NULL);
}

void FWWindow::prepareWindowsMenu()
{
    windowsPainters.clear();
    windowsTitles.clear();
    m_mainWindow->menuWindow->clear();
    QAction *close = m_mainWindow->menuWindow->addAction("Close");
    QAction *closeAll = m_mainWindow->menuWindow->addAction("Close All");
    QAction *tile = m_mainWindow->menuWindow->addAction("Tile");
    QAction *cascade = m_mainWindow->menuWindow->addAction("Cascade");
    QAction *next = m_mainWindow->menuWindow->addAction("Next");
    QAction *previous = m_mainWindow->menuWindow->addAction("Previous");

    QAction *minimize = m_mainWindow->menuWindow->addAction("Minimize");
    QAction *maximize = m_mainWindow->menuWindow->addAction("Maximize");
    m_mainWindow->menuWindow->addSeparator();

    connect(minimize, SIGNAL(triggered()), this, SLOT(minimize()));
    connect(maximize, SIGNAL(triggered()), this, SLOT(maximize()));
    connect(close, SIGNAL(triggered()), m_mainWindow->m_space, SLOT(closeActiveSubWindow()));
    connect(closeAll, SIGNAL(triggered()), m_mainWindow->m_space, SLOT(closeAllSubWindows()));
    connect(tile, SIGNAL(triggered()), m_mainWindow->m_space, SLOT(tileSubWindows()));
    connect(cascade, SIGNAL(triggered()), m_mainWindow->m_space, SLOT(cascadeSubWindows()));
    connect(next, SIGNAL(triggered()), m_mainWindow->m_space, SLOT(activateNextSubWindow()));
    connect(previous, SIGNAL(triggered()), m_mainWindow->m_space, SLOT(activatePreviousSubWindow()));

    QList<QMdiSubWindow *> subWindowList = m_mainWindow->m_space->subWindowList();

    minimize->setEnabled(subWindowList.size() > 0);
    maximize->setEnabled(subWindowList.size() > 0);
    close->setEnabled(subWindowList.size() > 0);
    closeAll->setEnabled(subWindowList.size() > 0);
    tile->setEnabled(subWindowList.size() > 0);
    cascade->setEnabled(subWindowList.size() > 0);
    next->setEnabled(subWindowList.size() > 0);
    previous->setEnabled(subWindowList.size() > 0);

    QActionGroup * ag = new QActionGroup(this);
    ag->setExclusive (true);
    for (int i = 0 ; i < subWindowList.size(); i++)
    {
        windowsPainters.push_back (subWindowList[i]);
        ProjectPanel * pp = dynamic_cast<ProjectPanel *>(
            subWindowList[i]->widget());
        if (pp!=NULL)
        {
            if (fwbdebug) qDebug("FWWindow::prepareWindowsMenu() pp=%p", pp);

            if (pp->isClosing()) continue ;

//            QString text = pp->windowTitle();
            QString text = pp->getPageTitle();
            windowsTitles.push_back(text);

//            if (text=="") text = "[Noname]";
            QAction * act = m_mainWindow->menuWindow->addAction(text);
            ag->addAction(act);
            act->setCheckable ( true );
            if (subWindowList[i] == m_mainWindow->m_space->activeSubWindow()) act->setChecked(true);
            connect(act, SIGNAL(triggered()), this, SLOT(selectActiveSubWindow()));
        }
    }
}

void FWWindow::activatePreviousSubWindow()
{
    m_mainWindow->m_space->setActiveSubWindow(previous_subwindow);
    //previous_subwindow->raise();
}
 
/**
 * QMdiArea emits this signal after window has been activated. When
 * window is 0, QMdiArea has just deactivated its last active window,
 * and there are no active windows on the workspace. 
 *
 * During the call to this method @subwindow is already current (equal
 * to the pointer returned by m_mainWindow->m_space->currentSubWindow())
 */
void FWWindow::subWindowActivated(QMdiSubWindow *subwindow)
{
    if (subwindow==NULL) return;

    if (fwbdebug)
        qDebug() << "FWWindow::subWindowActivated subwindow="
                 << subwindow
                 << " " 
                 << subwindow->windowTitle();

    if (previous_subwindow == subwindow) return;

    if (isEditorVisible() && !oe->validateAndSave())
    {
        // editor has unsaved data and user clicked "Continue editing"
        // Roll back switch of subwindows

        if (fwbdebug)
            qDebug() << "Activating previous subwindow " 
                     << previous_subwindow
                     << " "
                     << previous_subwindow->windowTitle();
        QTimer::singleShot(0, this, SLOT(activatePreviousSubWindow()));
        return;
    }

    previous_subwindow = subwindow;

    ProjectPanel *pp = dynamic_cast<ProjectPanel*>(subwindow->widget());
    if (pp)
    {
        prepareFileMenu();
        attachEditorToProjectPanel(pp);
    }
}

void FWWindow::attachEditorToProjectPanel(ProjectPanel *pp)
{
    findObjectWidget->attachToProjectWindow(pp);
    findWhereUsedWidget->attachToProjectWindow(pp);
    oe->attachToProjectWindow(pp);
    if (isEditorVisible()) oe->open(pp->getSelectedObject());
}

void FWWindow::editPrefs()
{
    PrefsDialog pd(this);
    pd.exec();
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

QPrinter* FWWindow::getPrinter()
{
    return printer;
}

void FWWindow::closeEvent(QCloseEvent* ev)
{
    if (fwbdebug) qDebug("FWWindow::closeEvent");

    if (activeProject())
        st->setInt("Window/maximized", activeProject()->isMaximized());

    QList<QMdiSubWindow *> subWindowList = m_mainWindow->m_space->subWindowList();
    for (int i = 0 ; i < subWindowList.size();i++)
    {
        ProjectPanel * pp = dynamic_cast<ProjectPanel *>(
            subWindowList[i]->widget());

        if (pp!=NULL)
        {
            if (!pp->saveIfModified())
            {
                ev->ignore();
                return;
            }
            pp->saveState();
            pp->fileClose();
        }
    }
}

bool FWWindow::event(QEvent *event)
{
    if (event->type() >= QEvent::User)
    {
        // dispatch event to all projectpanel windows
        QList<QMdiSubWindow*> subWindowList = m_mainWindow->m_space->subWindowList();
        for (int i = 0 ; i < subWindowList.size(); i++)
            QCoreApplication::sendEvent(subWindowList[i]->widget(), event);

        event->accept();
        return true;
    }
    return QMainWindow::event(event);
}

void FWWindow::selectActiveSubWindow(/*const QString & text*/)
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
            m_mainWindow->m_space->setActiveSubWindow(windowsPainters[i]);
        }
    }
}

void FWWindow::minimize()
{
    if (fwbdebug) qDebug("FWWindow::minimize");
    if (m_mainWindow->m_space->activeSubWindow())
    {
        m_mainWindow->m_space->activeSubWindow()->showMinimized ();
        st->setInt("Window/maximized", 0);

        QList<QMdiSubWindow *> subWindowList = m_mainWindow->m_space->subWindowList();
        for (int i = 0 ; i < subWindowList.size();i++)
        {
            ProjectPanel * pp = dynamic_cast<ProjectPanel *>(
                subWindowList[i]->widget());

            if (pp!=NULL)
            {
                pp->loadState(false);
            }
        }
    }
}

void FWWindow::maximize()
{
    if (fwbdebug) qDebug("FWWindow::maximize");
    if (m_mainWindow->m_space->activeSubWindow())
    {
        m_mainWindow->m_space->activeSubWindow()->showMaximized ();
        st->setInt("Window/maximized", 1);
    }
}

void FWWindow::updateTreeFont ()
{
    QFont font = st->getTreeFont();
    QList<QMdiSubWindow *> subWindowList = m_mainWindow->m_space->subWindowList();
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

void FWWindow::checkForUpgrade(const QString& server_response)
{
    disconnect(current_version_http_getter, SIGNAL(done(const QString&)),
               this, SLOT(checkForUpgrade(const QString&)));

    /*
     * getStatus() returns error status if server esponded with 302 or
     * 301 redirect. Only "200" is considered success.
     */
    if (current_version_http_getter->getStatus())
    {
        /*
         * server response may be some html or other data in case
         * connection goes via proxy, esp. with captive portals. We
         * should not interpret that as "new version is available"
         */
        if (server_response.trimmed() == "update = 1")
        {
            QMessageBox::warning(
                this,"Firewall Builder",
                tr("A new version of Firewall Builder is available at"
                   " http://www.fwbuilder.org"));
        }
    } else
    {
        if (fwbdebug)
            qDebug("Update check error:  %s", 
                   current_version_http_getter->getLastError().
                   toLatin1().constData());
    }
}

/*
 * This slot is called after one of the mdi windows is closed.  This
 * is where the decision is made as to wether we should terminate the
 * program when the last MDI window is closed. Bug #2144114 "fwbuilder
 * exits if the last object file is closed" requests for the program
 * to continue after the last window is closed.
 */
void FWWindow::projectWindowClosed()
{
//    if (m_space->subWindowList().size() == 0) QCoreApplication::exit(0);
}

void FWWindow::help()
{
    Help *h = new Help(this, "Firewall Builder");
    h->setSource(QUrl("main.html"));
    h->show();
}

void FWWindow::showReleaseNotes()
{
    QString file_name = QString("release_notes_%1.html").arg(VERSION);
    // Show "release notes" dialog only if corresponding file
    // exists.
    QString contents;
    Help *h = new Help(this, "Important Information About This Release");
    if (h->findHelpFile(file_name).isEmpty())
    {
        // the file does not exist
        delete h;
    } else
    {
        // I do not know why, but url "file://file_name" does not seem to work.
        // But "file:file_name" works.
        h->setSource(QUrl("file:" + file_name));
        h->setModal(false);
        h->show();
        h->exec();
        // Class Help uses attribute Qt::WA_DeleteOnClose which
        // means the system will delete the object on close.  No
        // need to delete it explicitly if it was shown.
    }
}

void FWWindow::enableBackAction()
{
    m_mainWindow->backAction->setEnabled(true);
}

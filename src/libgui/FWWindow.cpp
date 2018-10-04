/*

                          Firewall Builder

                 Copyright (C) 2003, 2006 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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

#include "global.h"
#include "check_update_url.h"

#include "utils.h"
#include "utils_no_qt.h"

#include <ui_FWBMainWindow_q.h>
#include <ui_pagesetupdialog_q.h>

#include "FWBApplication.h"
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
#include "ObjectTreeViewItem.h"
#include "RuleSetView.h"
#include "ObjectEditor.h"
#include "PrefsDialog.h"
#include "LibExportDialog.h"
#include "findDialog.h"
#include "FindObjectWidget.h"
#include "FindWhereUsedWidget.h"
#include "CompilerOutputPanel.h"
#include "longTextDialog.h"
#include "Help.h"
#include "TutorialDialog.h"
#include "MDIEventFilter.h"

#include "FWBAboutDialog.h"
#include "debugDialog.h"
#include "filePropDialog.h"

#include "instConf.h"
#include "instDialog.h"
#include "StartTipDialog.h"

#include "events.h"

#include "importAddressListWizard/ImportAddressListWizard.h"
#include "snmpNetworkDiscoveryWizard/SNMPNetworkDiscoveryWizard.h"
#include "importFirewallConfigurationWizard/ImportFirewallConfigurationWizard.h"

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

#include "BackgroundCompileInfoWidget.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <memory.h>
#include <memory>
#include <algorithm>
#include <stdlib.h>

#ifndef _WIN32
#  include <unistd.h>     // for access(2)
#else
#  undef index
#endif

#include <QDesktopServices>
#include <QCloseEvent>
#include <QDateTime>
#include <QDockWidget>
#include <QHideEvent>
#include <QList>
#include <QMap>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QShowEvent>
#include <QSignalMapper>
#include <QUndoGroup>
#include <QUndoStack>
#include <QUrl>
#include <qaction.h>
#include <qapplication.h>
#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qeventloop.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qheaderview.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistwidget.h>
#include <qlistwidget.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qprinter.h>
#include <qradiobutton.h>
#include <qsplitter.h>
#include <qstackedwidget.h>
#include <qstatusbar.h>
#include <qstringlist.h>
#include <qtabwidget.h>
#include <qtextedit.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <QTemporaryDir>
 
extern bool regCheck();

using namespace libfwbuilder;
using namespace std;
using namespace Ui;


FWWindow::FWWindow() : QMainWindow(),   // QMainWindow(nullptr, Qt::Desktop),
                       m_space(0),
                       previous_subwindow(0),
                       instd(0),
                       editorOwner(0),
                       printer(0), searchObject(0), replaceObject(0),
                       auto_load_from_rcs_head_revision(0),
                       oe(0), findObjectWidget(0), findWhereUsedWidget(0),
                       undoGroup(0)
{
    instd = new instDialog(this);


    m_mainWindow = new Ui::FWBMainWindow_q();
    m_mainWindow->setupUi(dynamic_cast<QMainWindow*>(this));
    this->setupGlobalToolbar();

    //setCentralWidget(m_space);

    psd = nullptr;

    prepareFileOpenRecentMenu();
    setCompileAndInstallActionsEnabled(false);

    // ProjectPanel *proj = newProjectPanel();
    // showSub(proj);

#ifdef Q_OS_MACX
    m_mainWindow->m_space->setViewMode(QMdiArea::TabbedView);
#endif

    findObjectWidget = new FindObjectWidget(
        m_mainWindow->find_panel, nullptr, "findObjectWidget");
    findObjectWidget->setFocusPolicy( Qt::NoFocus );
    m_mainWindow->find_panel->layout()->addWidget( findObjectWidget );
    findObjectWidget->show();

    findWhereUsedWidget = new FindWhereUsedWidget(
        m_mainWindow->find_panel, nullptr, "findWhereUsedWidget");
    findWhereUsedWidget->setFocusPolicy( Qt::NoFocus );
    m_mainWindow->find_panel->layout()->addWidget( findWhereUsedWidget );
    findWhereUsedWidget->hide();

    // compiler_output = new CompilerOutputPanel(m_mainWindow->output_panel);
    // m_mainWindow->output_panel->layout()->addWidget(compiler_output);
    // compiler_output->show();

    // Designer adds editorDockWidget to the child widget of the main
    // window and I can't seem to be able to get rid of this
    // intermediatery child widget (named "widget"). Reparent editor
    // dock panel.
    m_mainWindow->editorDockWidget->setParent(this);
    addDockWidget(Qt::BottomDockWidgetArea, m_mainWindow->editorDockWidget);
    m_mainWindow->editorDockWidget->hide();

    oe  = new ObjectEditor((QWidget*)m_mainWindow->objectEditorStack);

    m_mainWindow->editorDockWidget->setupEditor(oe);
    m_mainWindow->editorDockWidget->hide();

    undoGroup = new QUndoGroup(this);
    undoAction = undoGroup->createUndoAction(this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    m_mainWindow->undoView->setGroup(undoGroup);
    if (st->getShowUndoPanel())
        m_mainWindow->undoDockWidget->show();
    else
        m_mainWindow->undoDockWidget->hide();

    connect(m_mainWindow->undoDockWidget, SIGNAL(visibilityChanged(bool)),
            this, SLOT(undoViewVisibilityChanged(bool)));

    redoAction = undoGroup->createRedoAction(this);
    QList<QKeySequence> redoShortcuts;
    redoShortcuts << tr("Ctrl+Y") << tr("Shift+Ctrl+Z");
    redoAction->setShortcuts(redoShortcuts);

    m_mainWindow->editMenu->insertAction(m_mainWindow->editMenu->actions().at(0), undoAction);
    m_mainWindow->editMenu->insertAction(undoAction, redoAction);

    printer = new QPrinter(QPrinter::HighResolution);

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

    connect( m_mainWindow->RulesMenu, SIGNAL (aboutToShow()),
             this, SLOT(prepareRulesMenu()));

    connect( m_mainWindow->m_space, SIGNAL(subWindowActivated(QMdiSubWindow*)),
             this, SLOT(subWindowActivated(QMdiSubWindow*)));

    ruleStaticActions = m_mainWindow->RulesMenu->actions();
    m_mainWindow->RulesMenu->actions().clear();

    disableActions(false);

    ProjectPanel *proj = newProjectPanel();
    showSub(proj);
    proj->setActive();

    setSafeMode(false);

//    findObject->setMinimumSize( QSize( 0, 0 ) );
    QWidget *tabbar= m_mainWindow->m_space->findChild<QTabBar*>();
    if (tabbar)
        tabbar->installEventFilter(new MDIEventFilter());

    m_temporaryDir = new QTemporaryDir(QDir::tempPath().append("/fwbuilder-tempdir-"));

}

FWWindow::~FWWindow()
{
    delete m_temporaryDir;

    QList<QMdiSubWindow*> subwindows = m_mainWindow->m_space->subWindowList(
        QMdiArea::StackingOrder);
    foreach (QMdiSubWindow* sw, subwindows)
    {
        //ProjectPanel *pp = dynamic_cast<ProjectPanel*>(sw->widget());
        sw->close();
        delete sw;
    }

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
    m_mainWindow->menuOpen_Recent->addAction(m_mainWindow->actionClearRecentFiles);
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
    {
        QString file_path = action->data().toString();
        if (fwbdebug) qDebug() << "Open recently opened file " << file_path;
        QMdiSubWindow* sw = alreadyOpened(file_path);
        if (sw != nullptr)
        {
            // activate window with this file
            m_mainWindow->m_space->setActiveSubWindow(sw);
            return;
        }
        loadFile(file_path, false);
        QCoreApplication::postEvent(this, new updateSubWindowTitlesEvent());
    }
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
    QList<QMdiSubWindow*> subwindows = m_mainWindow->m_space->subWindowList(
        QMdiArea::StackingOrder);
    bool windows_maximized =
        (subwindows.size()>0) ? subwindows[0]->isMaximized() : st->getInt("Window/maximized");

    if (fwbdebug)
        qDebug() << "FWWindow::showSub"
                 << "subwindows=" << subwindows
                 << "current window maximized: "
                 << int((subwindows.size()>0) ? subwindows[0]->isMaximized() : 0)
                 << "settings: " << st->getInt("Window/maximized");

    QMdiSubWindow *sub = new QMdiSubWindow;
    pp->mdiWindow = sub;
    sub->setWidget(pp);
    sub->setAttribute(Qt::WA_DeleteOnClose);
    m_mainWindow->m_space->addSubWindow(sub);

    connect( sub, SIGNAL(aboutToActivate()), pp, SLOT(aboutToActivate()));

    if (fwbdebug)
        qDebug() << "Show subwindow maximized: " << windows_maximized;

    if (windows_maximized)
        pp->setWindowState(Qt::WindowMaximized);
    else
        pp->setWindowState(Qt::WindowNoState);
    
    sub->show();
    /*
     * for reasons I do not understand, QMdiArea does not send signal
     * subWindowActivated when the very first subwindow comes up. I
     * think it should, but at least QT 4.4.1 on Mac does not do
     * it. Calling the slot manually so that editor panel can be
     * attached to the current project panel.
     */
    attachEditorToProjectPanel(pp);
    if (isEditorVisible()) oe->open(pp->getSelectedObject());
}

ProjectPanel* FWWindow::activeProject()
{
    QList<QMdiSubWindow*> subwindows = m_mainWindow->m_space->subWindowList(
        QMdiArea::StackingOrder);
    if (subwindows.size() == 0) return nullptr;
    QMdiSubWindow *w = subwindows.last(); // last item is the topmost window

    // QMdiSubWindow *w = m_mainWindow->m_space->currentSubWindow();
    // if (w) return dynamic_cast<ProjectPanel*>(w->widget());
    // if (fwbdebug)
    //     qDebug() << "FWWindow::activeProject(): currentSubWindow() returns nullptr, trying activeSubWindow()";
    // w = m_mainWindow->m_space->activeSubWindow();

    if (w) return dynamic_cast<ProjectPanel*>(w->widget());
    return nullptr;
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
    if (activeProject())
    {
        activeProject()->loadStandardObjects();
        activeProject()->readyStatus(true);
        activeProject()->loadState(true);
    }

    foreach (QString file, openDocFiles)
    {
        loadFile(file, auto_load_from_rcs_head_revision);
        updateOpenRecentMenu(file);
    }

    QCoreApplication::postEvent(this, new updateSubWindowTitlesEvent());

    showIntroDialog();

    QCoreApplication::postEvent(mw, new updateGUIStateEvent());
}

void FWWindow::showIntroDialog()
{
    if (st->isIntroDialogEnabled())
    {
        // Show dialog inviting user to look at the "Quick start"
        // guide on the web site.
        QMessageBox msg_box;
        msg_box.setText("<html>"
                        "<h1>Welcome to Firewall Builder</h1>"
                        "<h3>Quick Start Guide</h3>"
                        "<p>"
                        "There is a short online guide that provides basic information "
                        "to help new users save time when first learning "
                        "to use Firewall Builder."
                        "</p>"
                        "<p>"
                        "In this guide you will learn:"
                        "</p>"
                        "<p>"
                        "<ul>"
                        "<li>Layout of the application windows</li>"
                        "<li>Location of frequently used command buttons</li>"
                        "<li>How to create and edit objects</li>"
                        "<li>Where to find predefined system objects</li>"
                        "</ul>"
                        "</p>"
                        "</html>"
        );


        QPixmap pm;
        pm.load(":/Images/fwbuilder3-128x128.png");

        msg_box.setWindowModality(Qt::ApplicationModal);

        msg_box.setWindowFlags(
            Qt::Window |
            Qt::WindowTitleHint |
            Qt::CustomizeWindowHint |
            Qt::WindowCloseButtonHint |
            Qt::WindowSystemMenuHint);

        msg_box.setWindowTitle(tr("Welcome to Firewall Builder"));
        msg_box.setIconPixmap(pm);
        msg_box.setInformativeText(tr("The guide will open in the web browser"));
        QCheckBox cb(tr("Do not show this again"), &msg_box);
        msg_box.addButton(&cb, QMessageBox::ResetRole);  // is this role right ?
        QPushButton *watch_button = 
            msg_box.addButton(tr("Watch the guide"), QMessageBox::AcceptRole);
        msg_box.addButton(QMessageBox::Close);

        /* Hack alert!  Disconnect signals from the checkbox so that
           QMessageBox doesn't know when it gets clicked.  We check it
           directly ourselves to see what state it's in when the user
           clicks "close" or "watch". */
        cb.disconnect();

        msg_box.setDefaultButton(watch_button);
        msg_box.exec();

        if (cb.isChecked())
        {
            st->setIntroDialogEnabled(false);
        }

        if (msg_box.clickedButton() == watch_button)
        {
            int ab_group = st->getABTestingGroup();
            QString url("http://www.fwbuilder.org/4.0/quick_start_guide_%1.html");
            QDesktopServices::openUrl(QUrl(url.arg(ab_group), QUrl::StrictMode));
        }

        return;
    }

    if (!st->getBool("UI/NoStartTip"))
    {
        StartTipDialog *stdlg = new StartTipDialog(this);
        stdlg->run();
    }
}

void FWWindow::helpAbout()
{
    FWBAboutDialog ad(this);
    ad.exec();
}

void FWWindow::debug()
{
    debugDialog dd(this);
    dd.exec();
}

void FWWindow::fileNew()
{
    // if the only project panel window that we have shows default
    // object tree (i.e. its filename is empty) and has no unsaved
    // changes, then load file into. Otherwise create new project
    // window.

    ProjectPanel *proj = activeProject();
    if (proj && proj->getFileName().isEmpty() && !proj->db()->isDirty())
    {
        activeProject()->fileNew();
    } else
    {
        std::unique_ptr<ProjectPanel> proj(newProjectPanel());
        if (proj->fileNew())
        {
            showSub(proj.get());
            QCoreApplication::postEvent(mw, new updateGUIStateEvent());

            //prepareFileMenu();
            //updateGlobalToolbar();
            //prepareRulesMenu();
            proj.release();
        }
    }
}

void FWWindow::fileOpen()
{
    QString dir;
    QMdiSubWindow *last_active_window = m_mainWindow->m_space->activeSubWindow();

    QString file_name = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        st->getOpenFileDir(mw->getCurrentFileName()),
        "FWB files (*.fwb *.fwl *.xml);;All Files (*)");

    if (file_name.isEmpty())
    {
        m_mainWindow->m_space->setActiveSubWindow(last_active_window);
        return;
    }

    st->setOpenFileDir(file_name);

    // Using absoluteFilePath(), see #1334
    QFileInfo fi(file_name);
    QString file_path = fi.absoluteFilePath();

    if (fwbdebug) qDebug() << "FWWindow::fileOpen():"
                           << "File name: " << file_name
                           << "Absolute file path: " << file_path;

    QMdiSubWindow* sw = alreadyOpened(file_name);
    if (sw != nullptr)
    {
        if (fwbdebug) qDebug() << "This file is already opened";
        // activate window with this file
        m_mainWindow->m_space->setActiveSubWindow(sw);
        return;
    }

    QFileInfo file_path_info(file_path);
    if (!file_path_info.exists() || !file_path_info.isReadable())
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("File '%1' does not exist or is not readable").arg(file_path));
        return;
    }

    if (loadFile(file_path, false))
    {
        updateOpenRecentMenu(file_name);
        // reset actions, including Save() which should now
        // be inactive
        QCoreApplication::postEvent(mw, new updateGUIStateEvent());

        //prepareFileMenu();
        //prepareRulesMenu();
        //updateGlobalToolbar();

        QCoreApplication::postEvent(this, new updateSubWindowTitlesEvent());
    } else
        m_mainWindow->m_space->setActiveSubWindow(last_active_window);
}

QMdiSubWindow* FWWindow::alreadyOpened(const QString &file_name)
{
    QFileInfo fi(file_name);
    QString file_path = fi.absoluteFilePath();

    if (fwbdebug) qDebug() << "FWWindow::alreadyOpened():"
                           << "File name: " << file_name
                           << "Absolute file path: " << file_path;

    foreach(QMdiSubWindow* sw, m_mainWindow->m_space->subWindowList())
    {
        ProjectPanel * pp = dynamic_cast<ProjectPanel *>(sw->widget());
        if (pp!=nullptr)
        {
            if (fwbdebug) qDebug() << "Opened file" << pp->getFileName();
            if (pp->getFileName() == file_path) return sw;
        }
    }
    return nullptr;
}

bool FWWindow::loadFile(const QString &file_name, bool load_rcs_head)
{
    /* We always create new project panel, deleting the old one if
       it's the default (standard objects) one with no changes.
       Otherwise we leave behind traces of the old project panel. */

    ProjectPanel *oldProj = activeProject();
    ProjectPanel *proj = newProjectPanel();
    if (proj->loadFile(file_name, load_rcs_head)) {
        if (oldProj != 0 && oldProj->getFileName().isEmpty() &&
            (oldProj->db() == nullptr || !oldProj->db()->isDirty())) {
            oldProj->fileClose();
        }
        showSub(proj);
    } else {
        delete proj;
        return false;
    }

    proj->readyStatus(true);
    proj->loadState(true);
    if (st->getBool("/Diff/AutoCompile"))
        autoCompile();
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

        // reset actions, including Save() which should now
        // be inactive
        QCoreApplication::postEvent(mw, new updateGUIStateEvent());

        //prepareFileMenu();
        //prepareRulesMenu();
        //updateGlobalToolbar();
    }

    if (fwbdebug) qDebug("subWindowList().size()=%d",
                         m_mainWindow->m_space->subWindowList().size());
}

void FWWindow::fileExit()
{
    if (fwbdebug) qDebug() << "FWWindow::fileExit()";

    bool window_maximized_state = false;
    if (activeProject())
    {
        QList<QMdiSubWindow *> subWindowList = m_mainWindow->m_space->subWindowList();
        for (int i = 0 ; i < subWindowList.size(); i++)
        {
            window_maximized_state = subWindowList[i]->isMaximized();

            ProjectPanel * project =
                dynamic_cast<ProjectPanel*>(subWindowList[i]->widget());
            if (project!=nullptr)
            {
                if (!project->saveIfModified() ||
                    !project->checkin(true))  return; // aborted
                //if (!project->saveIfModified()) return;  // abort operation
                project->saveState();
                project->fileClose();
            }
        }
    }

    st->setInt("Window/maximized", window_maximized_state);

    app->quit();
}

void FWWindow::toolsImportAddressesFromFile()
{
    if (activeProject())
    {
        ImportAddressListWizard wiz(this);
        wiz.exec();
    }
}

void FWWindow::toolsSNMPDiscovery()
{
    if (activeProject())
    {
        SNMPNetworkDiscoveryWizard wiz(this);
        wiz.exec();
    }
}

void FWWindow::importPolicy()
{
    if (activeProject())
    {
        if (!activeProject()->m_panel->om->isObjectAllowed(Firewall::TYPENAME))
            return;
        
        ImportFirewallConfigurationWizard wiz(this, db());
        wiz.exec();
    }
}

void FWWindow::showEvent(QShowEvent *ev)
{
    st->restoreGeometry(this, QRect(100,100,1000,600) );
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
        activeProject()->m_panel->treePanelFrame->isVisible());
    m_mainWindow->actionEditor_panel->setChecked(
        m_mainWindow->editorDockWidget->isVisible());
    m_mainWindow->actionUndo_view->setChecked(
        m_mainWindow->undoDockWidget->isVisible());
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

    m_mainWindow->ObjectUnlockAction->setEnabled(
        activeProject()->m_panel->om->isCurrentObjectUnlockable());
    m_mainWindow->ObjectLockAction->setEnabled(
        activeProject()->m_panel->om->isCurrentObjectLockable());
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
        m_mainWindow->libExportAction->setEnabled(false);
        m_mainWindow->libImportAction->setEnabled(false);
        m_mainWindow->policyImportAction->setEnabled(false);
        return;
    }

    bool real_file_opened = (activeProject()->getFileName() != "");
    bool in_rcs = (activeProject()->getRCS() != nullptr &&
                   activeProject()->getRCS()->isCheckedOut());
    bool needs_saving = (db() && db()->isDirty());

    m_mainWindow->fileSaveAction->setEnabled(real_file_opened && needs_saving);
    m_mainWindow->fileCloseAction->setEnabled(real_file_opened);
    m_mainWindow->filePropAction->setEnabled(real_file_opened);
    m_mainWindow->filePrintAction->setEnabled(real_file_opened);
    m_mainWindow->libExportAction->setEnabled(real_file_opened);

    FWObject *lib = activeProject()->getCurrentLib();
    bool f = (
        lib == nullptr ||
        lib->getId()==FWObjectDatabase::TEMPLATE_LIB_ID ||
        lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
        lib->isReadOnly()
    );
    bool new_object_op_possible = !f;

    m_mainWindow->libImportAction->setEnabled(new_object_op_possible);
    m_mainWindow->policyImportAction->setEnabled(new_object_op_possible);

    m_mainWindow->addToRCSAction->setEnabled(real_file_opened && !in_rcs);
    m_mainWindow->fileCommitAction->setEnabled(
        real_file_opened && in_rcs && needs_saving);
    m_mainWindow->fileDiscardAction->setEnabled(
        real_file_opened && in_rcs && needs_saving);

    m_mainWindow->fileNewAction->setEnabled(true);
    m_mainWindow->fileOpenAction->setEnabled(true);
    m_mainWindow->fileSaveAsAction->setEnabled(true);
}

void FWWindow::prepareToolsMenu()
{
#ifdef HAVE_LIBSNMP
    m_mainWindow->SNMPDiscoveryAction->setEnabled(true);
#else
    m_mainWindow->SNMPDiscoveryAction->setEnabled(false);
#endif
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
        if (pp!=nullptr)
        {
            if (fwbdebug) qDebug("FWWindow::prepareWindowsMenu() pp=%p", pp);

            //if (pp->isClosing()) continue ;

            QString text = subWindowList[i]->windowTitle();
            windowsTitles.push_back(text);

            QAction * act = m_mainWindow->menuWindow->addAction(text);
            ag->addAction(act);
            act->setCheckable ( true );
            if (subWindowList[i] == m_mainWindow->m_space->activeSubWindow()) act->setChecked(true);
            connect(act, SIGNAL(triggered()), this, SLOT(selectActiveSubWindow()));
        }
    }
}

/*
 * returns list of file names (full canonical path) of the data files
 * currently opened in the program
 */
QStringList FWWindow::getListOfOpenedFiles()
{
    QStringList res;
    QList<QMdiSubWindow *> subWindowList = m_mainWindow->m_space->subWindowList();
    for (int i = 0 ; i < subWindowList.size(); i++)
    {
        ProjectPanel * pp = dynamic_cast<ProjectPanel *>(subWindowList[i]->widget());
        if (pp!=nullptr)
        {
            res.push_back(pp->getFileName()); // full path
        }
    }
    return res;
}

void FWWindow::activatePreviousSubWindow()
{
    if (fwbdebug) qDebug() << "FWWindow::activatePreviousSubWindow()";
    m_mainWindow->m_space->setActiveSubWindow(previous_subwindow);
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
    if (subwindow==nullptr) return;

    if (fwbdebug)
        qDebug() << "FWWindow::subWindowActivated"
                 << "subwindow=" << subwindow
                 << "(" << subwindow->windowTitle() << ")"
                 << "previous_subwindow=" << previous_subwindow
                 << "("
                 << QString((previous_subwindow) ? previous_subwindow->windowTitle() : "")
                 << ")"
                 << "isMaximized()=" << subwindow->isMaximized();

    if (previous_subwindow == subwindow) return;

    previous_subwindow = subwindow;

    ProjectPanel *pp = dynamic_cast<ProjectPanel*>(subwindow->widget());
    if (pp)
    {
        QCoreApplication::postEvent(mw, new updateGUIStateEvent());
        pp->setActive();
        if (isEditorVisible()) openEditor(pp->getSelectedObject());
    }
}

void FWWindow::attachEditorToProjectPanel(ProjectPanel *pp)
{
    findObjectWidget->attachToProjectWindow(pp);
    findWhereUsedWidget->attachToProjectWindow(pp);
    oe->attachToProjectWindow(pp);
}

void FWWindow::editPrefs()
{
    PrefsDialog pd(this);
    pd.exec();
}


void FWWindow::editFind()
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
        st->setInt("Window/maximized", activeProject()->mdiWindow->isMaximized());

    QList<QMdiSubWindow *> subWindowList = m_mainWindow->m_space->subWindowList();
    for (int i = 0 ; i < subWindowList.size();i++)
    {
        ProjectPanel * pp = dynamic_cast<ProjectPanel *>(
            subWindowList[i]->widget());

        if (pp!=nullptr)
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
        fwbUpdateEvent *ev = dynamic_cast<fwbUpdateEvent*>(event);
        int obj_id = ev->getObjectId();

        /*
         * TODO:
         *
         * db() returns pointer to the FWObjectDatabase object that
         * belongs to the current active project panel. If the event
         * was sent for an object that does not belong to the active
         * panel, the object @obj won't be found in this
         * database. Event has project file name as another parameter,
         * need to use that to localte right project panel instead of
         * just calling activeProject().
         *
         * This happens when two data files are open and object tree
         * panels are detached. User can try to open an object from
         * file A by double clicking in the tree, while active panel
         * shows file B. See ticket #1804 "With 2 files open and
         * object trees undocked you cannot open objects from both
         * object trees"
         */

        FWObject *obj = db()->findInIndex(obj_id);
        ProjectPanel *pp = activeProject();

        if (fwbdebug)
            qDebug() << this
                     << "event:"
                     << ev->getEventName()
                     << "object:"
                     << ((obj!=nullptr) ? QString::fromUtf8(obj->getName().c_str()) : "<nullptr>");

        switch (event->type() - QEvent::User)
        {
        case UPDATE_GUI_STATE_EVENT:
            prepareFileMenu();
            prepareEditMenu();
            updateGlobalToolbar();
            // do not return, let ProjectPanel process the same event as well
            break;

        case OPEN_OBJECT_IN_EDITOR_EVENT:
        {
            if (pp && obj)
            {
                openEditor(obj);
                // pp->editObject(obj);
                pp->mdiWindow->update();
            }
            ev->accept();
            return true;
        }

        case OPEN_OPT_OBJECT_IN_EDITOR_EVENT:
        {
            if (pp && obj)
            {
                openOptEditor(
                    obj,
                    dynamic_cast<openOptObjectInEditorEvent*>(event)->opt_code);
                // pp->editObject(obj);
                pp->mdiWindow->update();
            }
            ev->accept();
            return true;
        }

        case UPDATE_SUBWINDOW_TITLES_EVENT:
        {
            QMap<QString, int> short_name_counters;
            QMap<QMdiSubWindow*, QString> short_titles;
            QMap<QMdiSubWindow*, QString> long_titles;

            foreach(QMdiSubWindow* sw, m_mainWindow->m_space->subWindowList())
            {
                ProjectPanel * pp = dynamic_cast<ProjectPanel *>(sw->widget());
                if (pp!=nullptr)
                {
                    // string returned by getPageTitle() may also
                    // include RCS revision number. Compare only
                    // file name, without the path and rev number
                    // to make sure we show long paths for two
                    // subwindows where file names are identical,
                    // regardless of the RCS revision number.
                    QString file_name = pp->getFileName(); // full path
                    QFileInfo fi(file_name);
                    QString short_name = fi.fileName();
                    int c = short_name_counters[short_name];
                    short_name_counters[short_name] = c + 1;

                    short_titles[sw] = pp->getPageTitle(false);
                    long_titles[sw] = pp->getPageTitle(true);

                    if (fwbdebug)
                        qDebug() << "Subwindow " << sw
                                 << "file_name " << file_name
                                 << "short_name " << short_name
                                 << "short_name_counter " << c
                                 << "short_title " << short_titles[sw]
                                 << "long_title " << long_titles[sw];

                }
            }

            foreach(QMdiSubWindow* sw, m_mainWindow->m_space->subWindowList())
            {
                QString short_name = short_titles[sw];
                if (short_name_counters[short_name] > 1)
                    sw->setWindowTitle(long_titles[sw]);
                else
                    sw->setWindowTitle(short_titles[sw]);
            }
            ev->accept();
            return true;
        }

        case CLOSE_EDITOR_PANEL_EVENT:
        {
            hideEditor();
            ev->accept();
            return true;
        }

        case CLEAR_EDITOR_PANEL_EVENT:
        {
            clearEditorAndSearchPanels();
            ev->accept();
            return true;
        }

        }

        // dispatch event to all projectpanel windows
        foreach(QMdiSubWindow* sw, m_mainWindow->m_space->subWindowList())
            QCoreApplication::sendEvent(sw->widget(), event);

        event->accept();
        return true;
    }

    //if (fwbdebug) qDebug() << this << "event:" << event;

    return QMainWindow::event(event);
}

void FWWindow::selectActiveSubWindow(/*const QString & text*/)
{
    QObject * sender_ = sender ();
    QAction * act = (QAction*) sender_ ;
    QString text = act->text();
    if (text=="[Noname]") text="";

    if (fwbdebug) qDebug() << "FWWindow::selectActiveSubWindow()"
                           << "text=" << text;

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

            if (pp!=nullptr)
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
        if (pp!=nullptr)
        {
            std::vector<QTreeWidget*> trees = pp->m_panel->om->getTreeWidgets();
            for (unsigned int o = 0 ; o < trees.size(); o++)
            {
                trees[o]->setFont(font);
            }
        }
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
    Help *h = Help::getHelpWindow(this);
    h->setSource(QUrl("main.html"));
    h->raise();
    h->show();
}

void FWWindow::showReleaseNotes()
{
    QStringList version_components = QString(VERSION).split(".");
    assert(version_components.size() >= 3);

    QString version_no_build = QString("%1.%2.%3")
        .arg(version_components[0])
        .arg(version_components[1])
        .arg(version_components[2]);

    QString file_name = QString("release_notes_%1.html").arg(version_no_build);

    // Show "release notes" dialog only if corresponding file
    // exists.
    QString contents;
    Help *h = Help::getHelpWindow(this);
    h->setName("Firewall Builder Release Notes");
    if (h->findHelpFile(file_name).isEmpty())
    {
        // the file does not exist - find the latest release note
        // use release_notes_5.3.0 to find the release notes directory
        h->showAllReleaseNotes(h->findHelpFile(QString("release_notes_5.3.0.html")));
        h->raise();
        h->show();

        // use the generated list of release notes and select the first one
        h->showReleaseNotesSelected();
    } else
    {
        h->showAllReleaseNotes(h->findHelpFile(file_name));
        // I do not know why, but url "file://file_name" does not seem to work.
        // But "file:file_name" works.
        h->setSource(QUrl("file:" + file_name));
        h->raise();
        h->show();
        //h->exec();
        // Class Help uses attribute Qt::WA_DeleteOnClose which
        // means the system will delete the object on close.  No
        // need to delete it explicitly if it was shown.
    }
}

void FWWindow::enableBackAction()
{
    m_mainWindow->backAction->setEnabled(true);
}

void FWWindow::activateRule(ProjectPanel* project, QString fwname, QString setname, int rule)
{
    // Find firewall object tree item
    FWObject* firewall = nullptr;
    foreach(QTreeWidgetItem* item,
            project->getCurrentObjectTree()->findItems(fwname,
                                     Qt::MatchExactly | Qt::MatchRecursive, 0))
    {
        if (Firewall::cast(dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject())!=nullptr)
        {
            firewall = dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject();
            break;
        }
    }
    if (firewall == nullptr) return;

    FWObject::const_iterator i =
        find_if(firewall->begin(), firewall->end(),
                FWObjectNameEQPredicate(string(setname.toUtf8().constData())));
    if (i==firewall->end()) return;
    RuleSet *set = RuleSet::cast(*i);
    if (set == nullptr) return;

    QCoreApplication::postEvent(
        mw, new openRulesetImmediatelyEvent(project->getFileName(),
                                            set->getId()));

    FWObject *ruleObject = set->getRuleByNum(rule);
    if (ruleObject == nullptr) return;

    QCoreApplication::postEvent(mw, new selectRuleElementEvent(project->getFileName(),
                                 ruleObject->getId(),
                                 ColDesc::Action));
}

void FWWindow::undoViewVisibilityChanged(bool visible)
{
   if(mw->isVisible())
       st->setShowUndoPanel(visible);
}

void FWWindow::updateGlobalToolbar()
{
    ProjectPanel* pp = activeProject();
    if (pp)
    {
        list<Firewall *> fws;
	if (pp->db() != nullptr) pp->findAllFirewalls(fws);
        setCompileAndInstallActionsEnabled(fws.size() != 0);
    } else
        setCompileAndInstallActionsEnabled(false);
}

void FWWindow::setupGlobalToolbar()
{
    setUnifiedTitleAndToolBarOnMac(false);
    if (st->getBool("/UI/IconWithText"))
        m_mainWindow->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    else
        m_mainWindow->toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    setUnifiedTitleAndToolBarOnMac(true);
}


/*
 * This method constructs main menu "Rules" and enables or disables items
 * as appropriate.
 */
void FWWindow::prepareRulesMenu()
{
    if (fwbdebug) qDebug() << "FWWindow::prepareRulesMenu()";

    cleanRulesMenu();

    ProjectPanel* pp = activeProject();
    if (pp)
    {
        RuleSetView* rsv = activeProject()->getCurrentRuleSetView();

        if (fwbdebug) qDebug() << "FWWindow::prepareRulesMenu() rsv=" << rsv;

        if(rsv)
        {
            if (rsv->selectedRulesCount() == 0)
                rsv->addGenericMenuItemsToContextMenu(m_mainWindow->RulesMenu);
            else
                rsv->addRowMenuItemsToMenu(m_mainWindow->RulesMenu);
        }
        m_mainWindow->RulesMenu->addSeparator();
        m_mainWindow->RulesMenu->addActions(ruleStaticActions);

        list<Firewall *> fws;
        pp->findAllFirewalls(fws);
        setCompileAndInstallActionsEnabled(fws.size() != 0);
    }
}

void FWWindow::cleanRulesMenu()
{
    if (fwbdebug) qDebug() << "FWWindow::cleanRulesMenu()";

    m_mainWindow->RulesMenu->actions().clear();
    m_mainWindow->RulesMenu->clear();
}


void FWWindow::showStatusBarMessage(const QString &txt)
{
    statusBar()->showMessage(txt);
    // Keep status bar message little longer so user can read it. See #272
    QTimer::singleShot( 1000, statusBar(), SLOT(clearMessage()));
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
}

void FWWindow::setCompileAndInstallActionsEnabled(bool en)
{
    if (fwbdebug) qDebug() << "FWWindow::setCompileAndInstallActionsEnabled en="
                           << en;
    m_mainWindow->compileAction->setEnabled(en );
    m_mainWindow->installAction->setEnabled(en );
    m_mainWindow->inspectAction->setEnabled(en );
}

void FWWindow::setEnabledAfterRF()
{
    if (fwbdebug) qDebug() << "FWWindow::setEnabledAfterRF()";
    m_mainWindow->compileAction->setEnabled( true );
    m_mainWindow->installAction->setEnabled( true );
    m_mainWindow->inspectAction->setEnabled( true );
}

void FWWindow::selectRules()
{
    if (fwbdebug) qDebug() << "FWWindow::selectRules()";

    m_mainWindow ->compileAction->setEnabled( true );
    m_mainWindow ->installAction->setEnabled( true );
    m_mainWindow ->inspectAction->setEnabled( true );

    if (activeProject()) activeProject()->selectRules();
}

void FWWindow::disableActions(bool havePolicies)
{
    if (fwbdebug) qDebug() << "FWWindow::disableActions()";

    m_mainWindow ->compileAction->setEnabled(havePolicies);
    m_mainWindow ->installAction->setEnabled(havePolicies);
    m_mainWindow ->inspectAction->setEnabled(havePolicies);
}

void FWWindow::compile()
{
    if (activeProject())
    {
        activeProject()->save();
        // if there is no file name associated with the project yet,
        // user is offered a chance to choose the file. If they hit
        // Cancel in the dialog where they choose the file name,
        // operation should be cancelled. We do not get direct
        // information whether they hit Cancel so the only way to
        // check is to verify that the file has been saved at this
        // point.
        if (activeProject()->db()->isDirty()) return;
        std::set<Firewall*> emp;
        instd->show(this->activeProject(), false, false, emp);
    }
}

void FWWindow::install()
{
    if (activeProject())
    {
        activeProject()->save();
        // see comment in FWWindow::compile()
        if (activeProject()->db()->isDirty()) return;
        std::set<Firewall*> emp;
        instd->show(this->activeProject(), true, false, emp);
    }
}

void FWWindow::compile(set<Firewall*> vf)
{
    if (fwbdebug)
        qDebug("FWWindow::compile preselected %d firewalls", int(vf.size()));
    if (activeProject())
    {
        activeProject()->save();
        // see comment in FWWindow::compile()
        if (activeProject()->db()->isDirty()) return;
        instd->show(this->activeProject(), false, true, vf);
    }
}

void FWWindow::install(set<Firewall*> vf)
{
    if (activeProject())
    {
        activeProject()->save();
        // see comment in FWWindow::compile()
        if (activeProject()->db()->isDirty()) return;
        instd->show(this->activeProject(), true, true, vf);
    }
}

void FWWindow::inspect()
{
    if (activeProject())
    {
        activeProject()->save();
        // see comment in FWWindow::compile()
        if (activeProject()->db()->isDirty()) return;
        this->activeProject()->inspectAll();
    }
}

void FWWindow::autoCompile()
{
    if (activeProject() && !activeProject()->db()->isDirty())
    {
        instDialog *idlg = new instDialog(this);
        new BackgroundCompileInfoWidget(this, idlg, this);
        idlg->autoCompile(this->activeProject());
    }
}

void FWWindow::addNewObjectMenu(QMenu *m)
{
    QMenu *old_menu = m_mainWindow->newObjectAction->menu();
    if (old_menu) delete old_menu;
    m_mainWindow->newObjectAction->setMenu( m );
}

void FWWindow::showNewObjectMenu()
{
    m_mainWindow->newObjectAction->menu()->popup(QCursor::pos());
}

QString FWWindow::getTemporaryDirPath() const
{
    if (m_temporaryDir->isValid())
        return m_temporaryDir->path();
    return QString();
}


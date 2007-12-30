/* 

                          Firewall Builder

                 Copyright (C) 2003, 2006 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: FWWindow.cpp,v 1.220 2007/07/07 05:39:33 vkurland Exp $

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
#include "RCSFileDialog.h"
#include "RCSFilePreview.h"
#include "ObjectEditor.h"
#include "execDialog.h"
#include "PrefsDialog.h"
#include "LibExportDialog.h"
#include "findDialog.h"
#include "DiscoveryDruid.h"
#include "FindObjectWidget.h"
#include "FindWhereUsedWidget.h"
#include "longTextDialog.h"

#include <ui_rcsfilesavedialog_q.h>
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

#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"

#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

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

using namespace libfwbuilder;
using namespace std;
using namespace Ui;

FWWindow::FWWindow()
{
    if (fwbdebug)
        qDebug("FWWindow constructor");

    setUnifiedTitleAndToolBarOnMac(true);

    m_mainWindow = new Ui::FWBMainWindow_q();
    m_mainWindow->setupUi(dynamic_cast<QMainWindow*>(this)); 

    rcs             = NULL;
    systemFile      = true;
    visibleFirewall = NULL;
    shownInInfo     = NULL;
    ruleSetTabIndex = 0;
    lastFirewallIdx = -2;
    delete m_mainWindow->treeFrame;
    
    instd = NULL; 

    changingTabs = false;
    ruleSetRedrawPending = false;

    setSafeMode(false);
    setStartupFileName("");

    printer = new QPrinter(QPrinter::HighResolution);

    noFirewalls = tr("No firewalls defined");

    autosaveTimer = new QTimer(static_cast<QObject*>(this));
    instDialogOnScreenTimer = new QTimer(static_cast<QObject*>(this));

    connect(instDialogOnScreenTimer, SIGNAL(timeout()), this, SLOT(killInstDialog()));
    instDialogOnScreenTimer->start(1000);
    
    // om is a global var
    
    om=new ObjectManipulator( m_mainWindow->objInfoSplitter );
    
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(0);
    policy.setHeightForWidth(om->sizePolicy().hasHeightForWidth());
    
    om->setSizePolicy( policy );
    
    m_mainWindow->objInfoSplitter->insertWidget( 0, om );
    //m_mainWindow->frame16->setMinimumSize(QSize(100,0));
    m_mainWindow->rightFrame->setMinimumSize(QSize(0,0));

    connect( m_mainWindow->newObjectAction, SIGNAL( triggered() ),
             om,              SLOT( newObject() ) );

    connect( m_mainWindow->backAction, SIGNAL( triggered() ),
             om,         SLOT( back() ) );

    connect( m_mainWindow->findAction, SIGNAL( triggered() ),
             this,       SLOT(   search()    ) );

    connect( m_mainWindow->ObjectMenu, SIGNAL (aboutToShow() ),
            this,     SLOT( prepareObjectMenu()  )); 

    delete m_mainWindow->infoFrame;

    oi=new QTextEdit( m_mainWindow->objInfoSplitter );
    oi->setReadOnly(true);

    m_mainWindow->objInfoSplitter->setStretchFactor(m_mainWindow->objInfoSplitter->indexOf(oi), 0);

    policy.setHeightForWidth(oi->sizePolicy().hasHeightForWidth());
    oi->setSizePolicy( policy );
    
    m_mainWindow->objInfoSplitter->addWidget( oi );

    
    oi->setGeometry( oi->geometry().x(), oi->geometry().y(),
                     oi->geometry().width(), st->getInfoWindowHeight() );

    if (st->getInfoStyle()!=0) oi->show();
    else oi->hide(); 

    findObjectWidget = new FindObjectWidget( m_mainWindow->auxiliaryPanel, "findObjectWidget" );
    findObjectWidget->setFocusPolicy( Qt::NoFocus );
    m_mainWindow->auxiliaryPanel->layout()->addWidget( findObjectWidget );
    connect( findObjectWidget, SIGNAL( close() ), this, SLOT( closeAuxiliaryPanel() ) );
    
    findWhereUsedWidget = new FindWhereUsedWidget(m_mainWindow->auxiliaryPanel, "findWhereUsedWidget");
    findWhereUsedWidget->setFocusPolicy( Qt::NoFocus );
    m_mainWindow->auxiliaryPanel->layout()->addWidget( findWhereUsedWidget );
    findWhereUsedWidget->hide();
    connect( findWhereUsedWidget, SIGNAL( close() ), this, SLOT( closeAuxiliaryPanel() ) );
    m_mainWindow->auxiliaryPanel->hide();
    om->show();
    
    if (fwbdebug)
        qDebug("/FWWindow constructor");


//    findObject->setMinimumSize( QSize( 0, 0 ) );
}

FWWindow::~FWWindow()
{
    if (rcs!=NULL) delete rcs;
    delete m_mainWindow;
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

void FWWindow::startupLoad()
{
    if (fwbdebug) qDebug("startup: load everything ...");

    int sa = st->getStartupAction();

    if (safeMode)  mw->load(NULL);
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
                load(NULL,rcs);
            } catch (FWException &ex)
            {
                qDebug("Exception: %s",ex.toString().c_str());
                load(NULL);
            }
        } else
        {
            load(NULL); // load standard objects
        }
    }

    QString id = st->getStr("UI/visibleFirewall");
    FWObject *show_fw=NULL;
    if ( !id.isEmpty() ) show_fw = mw->db()->getById(id.toLatin1().constData(),true);

    id = st->getStr("UI/visibleObject");
    FWObject *show_obj=NULL;
    if ( !id.isEmpty() ) show_obj = mw->db()->getById(id.toLatin1().constData(),true);

    showFirewalls( show_fw==NULL );

    if ( sa==1 && !safeMode )
    {
        if (show_fw)
        {
            if (fwbdebug)
                qDebug("open firewall %s",show_fw->getName().c_str());
            showFirewall( show_fw );
        }

        if (show_obj)
        {
            if (fwbdebug)
                qDebug("open object %s",show_obj->getName().c_str());
            om->openObject( show_obj );
        }
    }

}

void FWWindow::clearFirewallTabs()
{
    if (fwbdebug) qDebug("FWWindow::clearFirewallTabs");

    m_mainWindow->ruleSets->hide();

    while (m_mainWindow->ruleSets->count()!=0)
    {
        QWidget *p = m_mainWindow->ruleSets->widget(0);
        m_mainWindow->ruleSets->removeTab(m_mainWindow->ruleSets->indexOf(p));
        delete p;
    }
    m_mainWindow->ruleSets->show();
    return;
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
    
    if (st->getInfoStyle()!=0 && (shownInInfo!=obj || forced))
    {
        oi->clear();

        QString s="";
        if (st->getInfoStyle()==2)
        {
            s=FWObjectPropertiesFactory::getObjectPropertiesDetailed(obj)
                + QString("<hr height=\"0\">");
            oi->setText(s);
        }

        oi->setFontWeight(QFont::Normal);
        oi->setFontItalic(false);
        oi->setFontUnderline(false);
        oi->setTextColor(Qt::black);
        s=QString::fromUtf8(obj->getComment().c_str());
        oi->append(s);
        oi->moveCursor(QTextCursor::Start);

        shownInInfo = obj;
    }

//    mw->unselectRules(); 
}

bool FWWindow::saveIfModified()
{
    if (db()->isDirty())
    {
        switch (QMessageBox::information(this, "Firewall Builder",
            tr("Some objects have been modified but not saved.\n"
               "Do you want to save changes now ?"),
            tr("&Save"), tr("&Discard"), tr("&Cancel"),
            0,       // Enter = button 0
            2 ) ) {   // Escape == button 2

        case 0:
            save();
            break;
        case 1:  // discard
            db()->setDirty(false);
            break;
        case 2:  // cancel
            return(false);
        }
    }
    return true;
}

QString FWWindow::getDestDir(const QString &fname)
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



QString FWWindow::chooseNewFileName(const QString &fname,
                                    bool checkPresence,const QString &title)
{
    QString destdir = getDestDir(fname);

    QString fn = QFileDialog::getSaveFileName( this, title, destdir,
       tr( "FWB Files (*.fwb);;All Files (*)" ) );
    if ( fn.isEmpty() ) return "";

    QFileInfo finfo(fn);

    //if (finfo.extension(false)!="fwb") fn=fn+".fwb";
    if (finfo.suffix()!="fwb") fn=fn+".fwb";

    finfo.setFile(fn);

    if ( ! checkPresence || ! finfo.exists() ||
             QMessageBox::warning(
                 this,"Firewall Builder", 
                 tr("The file %1 already exists.\nDo you want to overwrite it ?")
                 .arg(fn.toLatin1().constData()),
                 tr("&Yes"), tr("&No"), QString::null,
                 0, 1 )==0 )
    {
        return fn;
    }

    return "";
}

void FWWindow::setFileName(const QString &fname)
{
    systemFile=false;
    rcs->setFileName(fname);
    db()->setFileName(fname.toLatin1().constData());

    QString caption = rcs->getFileName().section("/",-1,-1); 
    if (rcs->isInRCS()) caption = caption + ", rev " + rcs->getSelectedRev(); 

    setWindowTitle( QString("Firewall Builder: ")+caption ); 
}

void FWWindow::fileProp()
{
    if (rcs!=NULL)
    {
        filePropDialog fpd(this,rcs);
	fpd.setPrinter(printer);
        fpd.exec();
    } 
}

void FWWindow::fileNew()
{    
    QString nfn=chooseNewFileName(st->getWDir(),true,
                                  tr("Choose name and location for the new file"));
    if ( !nfn.isEmpty() )
    {
        if (!saveIfModified() || !checkin(true)) return;
        if (!systemFile && rcs!=NULL) fileClose();       // fileClose calls load(this)
        else  load(this);

        visibleFirewall = NULL;
        showFirewalls( false );


        setFileName(nfn);

        save();

        m_mainWindow->addToRCSAction->setEnabled( !rcs->isInRCS() && !rcs->isRO() && !rcs->isTemp());
        m_mainWindow->fileDiscardAction->setEnabled( rcs->isInRCS() && !rcs->isRO() && !rcs->isTemp());
        m_mainWindow->fileCommitAction->setEnabled( rcs->isInRCS() && !rcs->isRO() && !rcs->isTemp());
        m_mainWindow->fileSaveAction->setEnabled( !rcs->isRO() && !rcs->isTemp() ); 

        setupAutoSave();
    } 
}

void FWWindow::fileOpen()
{
    if (fwbdebug) qDebug("FWWindow::fileOpen(): start");

    RCSFileDialog   fd(this, 0, true);
    RCSFilePreview  fp(this);

    if ( fd.exec() != QDialog::Accepted )
        return;

    bool hasRCS = fp.showFileRLog( fd.selectedFiles()[0] );

    if ( (!hasRCS) || (fp.exec() == QDialog::Accepted) )
    {
        if (!saveIfModified() || !checkin(true)) return;
        if (!systemFile && rcs!=NULL) fileClose();

        //try to get simple rcs instance from RCS preview
        RCS *rcs = fp.getSelectedRev();

        //if (by some matter) preview cannot give RCS,
        //get a new RCS from file dialog
        if (rcs==NULL) 
            rcs = fd.getSelectedRev();

        //if RCS isn't still formed, it's an error
        if (rcs==NULL)
            return;

/***********************************************************************
 * TODO : add an option "RCS support"
 *
 * if opening read-only, do not checkout 
 * checkout may throw exception, need to catch it
 */
        try
        {
            rcs->co();

        } catch (FWException &ex)
        {
/* if there was an exception, abort operation. E.g. RCS::co may throw
 * exception */
            return;
        }
/***********************************************************************/

        load(this, rcs );
        showFirewalls( true );

        if (rcs->isTemp()) unlink(rcs->getFileName().toLatin1().constData());
    }
}

void FWWindow::fileClose()
{
    if (fwbdebug) qDebug("FWWindow::fileClose(): start");

    findObjectWidget->init();
    if (oe->isVisible()) oe->hide(); 

    if (!saveIfModified() || !checkin(true))  return;

    if (rcs) delete rcs;
    rcs=NULL;

    if (fwbdebug) qDebug("FWWindow::fileClose(): clearing widgets");

    firewalls.clear();
    m_mainWindow->fwList->clear();
    visibleFirewall = NULL;
    clearFirewallTabs();
    ruleSetViews.clear(); 
    om->clearObjects();
    FWObjectClipboard::obj_clipboard->clear();

    if (fwbdebug) qDebug("FWWindow::fileClose(): loading standard objects");

    load(this);

    if (fwbdebug) qDebug("FWWindow::fileClose(): show firewalls");

    showFirewalls( false );

    if (fwbdebug) qDebug("FWWindow::fileClose(): all done");

    setupAutoSave();
}

void FWWindow::fileSave()
{
    QStatusBar *sb = statusBar();
    sb->showMessage( tr("Saving data to file...") );
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
    save();
    sb->clearMessage();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
}

void FWWindow::fileSaveAs()
{
    if (oe->isVisible()) oe->hide();

/* we need to save data into the current file before we save it into a
 * new file, provided we do have current file
    if (!systemFile && rcs &&
        !rcs->isRO() && !rcs->isTemp() && !rcs->getFileName().isEmpty() &&
        (!saveIfModified() || !checkin(true))
    ) return;

 */


/* need to close the file without asking and saving, then reopen it again */
        
    db()->setDirty(false);  // so it wont ask if user wants to save
    rcs->abandon();

    QString oldFileName = rcs->getFileName();
    if (rcs!=NULL) delete rcs;

    rcs = new RCS("");

    QString nfn=chooseNewFileName(oldFileName,true,
                                  tr("Choose name and location for the file"));

    if (!nfn.isEmpty())
    {
        setFileName(nfn);

        save();

        m_mainWindow->addToRCSAction->setEnabled( !rcs->isInRCS() && !rcs->isRO() && !rcs->isTemp());
        m_mainWindow->fileDiscardAction->setEnabled( rcs->isInRCS() && !rcs->isRO() && !rcs->isTemp());
        m_mainWindow->fileCommitAction->setEnabled( rcs->isInRCS() && !rcs->isRO() && !rcs->isTemp());
        m_mainWindow->fileSaveAction->setEnabled( !rcs->isRO() && !rcs->isTemp() );
}
}

void FWWindow::fileExit()
{    
    if (saveIfModified() && checkin(true))
    {
        if (rcs) delete rcs;
        qApp->quit();
    }
}

void FWWindow::fileCommit()
{
    // Steps:
    // 1.save the file
    // 2.checkin (checkin() returns false when user hits Cancel)
    // 3. close file
    // 4. reopen it
    
    QString fname = rcs->getFileName();
    save();
    if (!checkin(true))  return;
    fileClose();
    try
    {
        RCS *rcs = new RCS(fname);
        if (rcs==NULL) return;
        rcs->co();
        load(this,rcs);
    } catch (FWException &ex)
    {
        load(this);
        return;
    }
    showFirewalls( true );
}

/*
 * discard changes done to the file and check out clean copy of the
 * head revision from RCS
 */
void FWWindow::fileDiscard()
{
    if (QMessageBox::warning(this, "Firewall Builder",
      tr("This operation discards all changes that have been saved\n"
         "into the file so far, closes it and replaces it with a clean\n"
         "copy of its head revision from RCS.\n"
         "\n"
         "All changes will be lost if you do this.\n\n"),
      tr("&Discard changes"),
      tr("&Cancel"), QString::null,
      1 )==0 )
    {
/* need to close the file without asking and saving, then reopen it again */
        
        QString fname = rcs->getFileName();

        db()->setDirty(false);  // so it wont ask if user wants to save
        rcs->abandon();
        fileClose();

        try
        {
            RCS *rcs = new RCS(fname);
            if (rcs==NULL) return;
            rcs->co();
            load(this, rcs );
        } catch (FWException &ex)
        {
/* if there was an exception, abort operation. E.g. RCS::co may throw
 * exception */
            load(this);
            return;
        }
/***********************************************************************/

        showFirewalls( true );
    }
}

void FWWindow::fileAddToRCS()
{
    if (!saveIfModified()) return;
    if (rcs && rcs->isCheckedOut()) return;

    try
    {
        if (!rcs->isInRCS() && !rcs->isRO())
        {
            rcs->add();
            rcs->co();
            QMessageBox::information(
                this,"Firewall Builder", 
                tr("File %1 has been added to RCS.").arg(rcs->getFileName()),
                tr("&Continue"), QString::null,QString::null,
                0, 1 );
        }
    }
    catch (FWException &ex)
    {
        QMessageBox::critical(
            this,"Firewall Builder", 
            tr("Error adding file to RCS:\n%1").arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
    }

    QString caption = rcs->getFileName().section("/",-1,-1);
    if (rcs->isInRCS()) caption = caption + ", rev " + rcs->getSelectedRev();
    if (rcs->isRO()) caption = caption + " " + tr("(read-only)");
    
    setWindowTitle( QString("Firewall Builder: ")+caption );

    m_mainWindow->addToRCSAction->setEnabled( !rcs->isInRCS() && !rcs->isRO());
    m_mainWindow->fileDiscardAction->setEnabled( rcs->isInRCS() && !rcs->isRO() && !rcs->isTemp());
    m_mainWindow->fileCommitAction->setEnabled( rcs->isInRCS() && !rcs->isRO() && !rcs->isTemp());
}

bool FWWindow::editingLibrary()
{
    return (rcs!=NULL && 
        ( rcs->getFileName().endsWith(".fwl")) );
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
    if (fwbdebug) qDebug("FWWindow::load(): start");
    QStatusBar *sb = statusBar();

    editingStandardLib = false;
    editingTemplateLib = false;

    QWidget *dlgp=NULL;
    if (dialogs_parent==NULL)
    {       
        if (isVisible()) dlgp=this;
    } else
    {
        dlgp=dialogs_parent;
    }

    MessageBoxUpgradePredicate upgrade_predicate(dlgp);

    fd->reset(); 

    if (fwbdebug) qDebug("FWWindow::load(): start 2");

    try
    {
// need to drop read-only flag on the database before I load new objects

        objdb = new FWObjectDatabase();
        objdb->setReadOnly( false );

        if (fwbdebug) qDebug("FWWindow::load(): loading objects");

        sb->showMessage( tr("Loading system objects...") );
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);

// always loading system objects
        if (fwbdebug) qDebug("FWWindow::load(): sysfname = %s",sysfname.c_str());
        objdb->load( sysfname, &upgrade_predicate, librespath);
        objdb->setFileName("");

        if (fwbdebug) qDebug("FWWindow::load(): create User library");

        FWObject *userLib=FWBTree::createNewLibrary( objdb );
        userLib->setName("User");
        userLib->setStr("color","#d2ffd0");

        if (fwbdebug) qDebug("FWWindow::load(): loading libraries");

        for (list<libData>::iterator i=addOnLibs->begin(); i!=addOnLibs->end(); ++i)
        {
            string libfname = i->path.toLatin1().constData();
            if (libfname!=sysfname && i->load)
            {
                if (fwbdebug) qDebug("FWWindow::load(): libfname = %s",libfname.c_str());
                FWObjectDatabase *ndb = new FWObjectDatabase();
                ndb->load(libfname, &upgrade_predicate,librespath);
                FWObject *dobj =
                    ndb->findInIndex( FWObjectDatabase::getDeletedObjectsId());
                if (dobj) ndb->remove(dobj, false);

                MergeConflictRes mcr(dlgp);
                objdb->merge(ndb, &mcr); 

                delete ndb;
            }
        }
        
        if (fwbdebug) qDebug("FWWindow::load(): done loading");

        objdb->setDirty(false);
        objdb->setFileName("");

        if (fwbdebug) qDebug("FWWindow::load(): create RCS");

        rcs = new RCS("");
        systemFile=true;
/*
 * TODO: we should create new FWObjectDatabase object and assign db
 * instead of using singleton
 */
//        objdb = FWObjectDatabase::db;

        setWindowTitle( "Firewall Builder" );

        m_mainWindow->fileSaveAction->setEnabled( false );
        m_mainWindow->addToRCSAction->setEnabled( false );
        m_mainWindow->fileDiscardAction->setEnabled( false );
        m_mainWindow->fileCommitAction->setEnabled( false );
    
        if (fwbdebug) qDebug("FWWindow::load(): done");

    } catch(FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder", 
            tr("Error loading file:\n%1").arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
    }

    if (fwbdebug) qDebug("FWWindow::load(): load objects in ObjectManager");

    om->loadObjects();

    if (fwbdebug) qDebug("FWWindow::load(): all done");

    setupAutoSave();
}

void FWWindow::load(QWidget *dialogs_parent,RCS *_rcs)
{
    QStatusBar *sb = statusBar();

    fd->reset(); 

    editingStandardLib = false;
    editingTemplateLib = false;

    bool forceSave=false; // use this flag to force 'save' operation if file should be renamed

    QWidget *dlgp=NULL;
    if (dialogs_parent==NULL)
    {       
        if (isVisible()) dlgp=this;
    } else
    {
        dlgp=dialogs_parent;
    }

    MessageBoxUpgradePredicate upgrade_predicate(dlgp);

    assert(_rcs!=NULL);

    rcs = _rcs;

    try
    {
        /* load the data file */
        systemFile=false;

        objdb = new FWObjectDatabase();

// need to drop read-only flag on the database before I load new objects
        objdb->setReadOnly( false );

// always loading system objects
        sb->showMessage( tr("Loading system objects...") );
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        objdb->load( sysfname, &upgrade_predicate, librespath);
        objdb->setFileName("");

// objects from a data file are in database ndb

        sb->showMessage( tr("Reading and parsing data file...") );
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        //QApplication::eventLoop()->processEvents(QEventLoop::ExcludeUserInput,100);

        FWObjectDatabase *ndb = new FWObjectDatabase();
        ndb->load(rcs->getFileName().toLatin1().constData(), &upgrade_predicate,librespath);
        time_t   oldtimestamp = ndb->getTimeLastModified();

        sb->clearMessage();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

/* loadingLib is true if user wants to open a library or master library file */
        bool loadingLib         = editingLibrary();

        if (fwbdebug)
        {
            list<FWObject*> ll = ndb->getByType(Library::TYPENAME);
            for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
            {
                qDebug("* Found library %s %s in the data file",
                       (*i)->getId().c_str(),(*i)->getName().c_str() );
            }
        }

/* if user opens library file, clear read-only flag so they can edit it */
        if (loadingLib)
        {
            list<FWObject*> ll = ndb->getByType(Library::TYPENAME);
            for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
            {
                if ((*i)->getId()==STANDARD_LIB) editingStandardLib=true;
                if ((*i)->getId()==TEMPLATE_LIB) editingTemplateLib=true;
                (*i)->setReadOnly( false );
            }
        } else
        {
/* preload libraries only if we do not edit a library file */
            for (list<libData>::iterator i=addOnLibs->begin();
                 i!=addOnLibs->end(); ++i)
            {
                string libfname = i->path.toLatin1().constData();
                if (libfname!=sysfname && i->load)
                {
                    if (fwbdebug)
                        qDebug("* Adding library %s",i->name.toLatin1().constData());

                    FWObjectDatabase *ndb1 = new FWObjectDatabase();
                    ndb1->load(libfname, &upgrade_predicate,librespath);
                    FWObject  *nlib1 = ndb1->getFirstByType(Library::TYPENAME);
                    if(nlib1==NULL)
                    {
                        qDebug("Error preloading library from file %s",
                               libfname.c_str());
                        assert(nlib1!=NULL);
                    }
                    string nlib1ID = nlib1->getId();
                    FWObject *dobj =
                        ndb1->findInIndex(FWObjectDatabase::getDeletedObjectsId());
                    if (dobj) ndb1->remove(dobj, false);

                    MergeConflictRes mcr(dlgp);
                    objdb->merge(ndb1, &mcr);

/* preloaded libraries are always read-only */
                    objdb->findInIndex(nlib1ID)->setReadOnly(true);

                    delete ndb1;
                }
            }
        }

        sb->showMessage( tr("Merging with system objects...") );
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
        //QApplication::eventLoop()->processEvents(QEventLoop::ExcludeUserInput,100);

        MergeConflictRes mcr(dlgp);
        objdb->merge(ndb, &mcr); 

        delete ndb;

        objdb->setFileName(rcs->getFileName().toLatin1().constData());
        objdb->resetTimeLastModified(oldtimestamp);
        objdb->setDirty(false);

        sb->clearMessage();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
        //QApplication::eventLoop()->processEvents(QEventLoop::ExcludeUserInput,100);

/*
 * TODO: we should create new FWObjectDatabase object and assign db
 * instead of using singleton
 */
//        objdb = FWObjectDatabase::db;

        if (fwbdebug)
        {
            qDebug("* Merge is done");
            list<FWObject*> ll = db()->getByType(Library::TYPENAME);
            for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
            {
                qDebug("* Library %s %s in the data file",
                       (*i)->getId().c_str(),(*i)->getName().c_str() );
            }
        }


/* this is a hack: 'Standard' library should be read-only. I have too
 * many files I already converted to the new API/DTD and I am too lazy
 * to convert them again, so I patch it up here.
 *
 * However, if I am editing standard library, it should not be read-only.
 */
        FWObject *slib = objdb->findInIndex("syslib000");
        if (fwbdebug)
            qDebug("standard library read-only status: %d, editingStandardLib: %d",
                   slib->isReadOnly(), editingStandardLib);

        if (slib!=NULL ) slib->setReadOnly(! editingStandardLib);

/* if the file name has an old extension .xml, change it to .fwb and
 * warn the user
 */
        QString   fn = rcs->getFileName();
        QFileInfo ofinfo(fn);

        if ( ofinfo.suffix()=="xml")
        {
            if (fwbdebug)
            {
                qDebug("Need to rename file:  %s",fn.toAscii().constData());
                qDebug("             dirPath: %s",ofinfo.dir().absolutePath().toAscii().constData());
                qDebug("            filePath: %s",ofinfo.absoluteFilePath().toAscii().constData());
            }
            QString nfn=ofinfo.dir().absolutePath() + "/" + ofinfo.completeBaseName() + ".fwb";

            bool needToRename = true;

/* need these dances with symlinks to fix bug #1008956: "Existing .fwb
 * file gets overwritten if has wrong extension"
 */
            QFileInfo nfinfo(nfn);
            if (nfinfo.exists() && ofinfo.isSymLink() && ofinfo.readLink()==nfn)
            {
// .xml file is a symlink pointing at .fwb file
// no need to rename
                needToRename = false;
            }

            if (needToRename)
            {
                if (nfinfo.exists())
                {
/* .fwb file exists but .xml is not a symlink
 * .fwb is a separate file with the same name.
 *
 * tell the user we need to rename old file but the new file exists,
 * then ask them to choose a new name. If the user chooses the same
 * name and agrees to overwrite the file, just use this name. If the
 * user hits cancel, tell them they need to choose a new name and open
 * "file save" dialog again.
 *
 * Show the first dialog only once. If user hits Cancel, they see
 * shorted version of the dialog and will be presented with "save
 * file" dialog again.
 */
                    QMessageBox::warning(
                        this,"Firewall Builder", 
                        tr("Firewall Builder 2 uses file extension '.fwb' and \nneeds to rename old data file '%1' to '%2',\nbut file '%3' already exists.\nChoose a different name for the new file.")
                        .arg(fn).arg(nfn).arg(nfn),
                        tr("&Continue"), QString::null,QString::null,
                        0, 1 );

                    nfn=chooseNewFileName(fn,true,
                                          tr("Choose name and location for the new file"));
                    if (nfn.isEmpty())
                    {
                        QString oldFileName = ofinfo.absoluteFilePath() + ".bak";
                        rename(oldFileName.toLatin1().constData(), fn.toLatin1().constData());

                        QMessageBox::warning(
                            this,"Firewall Builder", 
                            tr("Load operation cancelled and data file reverted to original version."),
                            tr("&Continue"), QString::null,QString::null,
                            0, 1 );

                        load(this);
                        return;
                    }
                    nfinfo.setFile(nfn);
                }

                rename(fn.toLatin1().constData(), nfn.toLatin1().constData());

                
                QMessageBox::warning(
                this,"Firewall Builder", 
                tr("Firewall Builder 2 uses file extension '.fwb'. Your data file '%1' \nhas been renamed '%2'")
                .arg(fn).arg(nfn),
                tr("&Continue"), QString::null,QString::null,
                0, 1 );
                
            }

            fn = nfn;
        }

        rcs->setFileName(fn);
        db()->setFileName(fn.toLatin1().constData());

        QString caption = rcs->getFileName().section("/",-1,-1);
        if (rcs->isInRCS()) caption = caption + ", rev " + rcs->getSelectedRev();
        if (rcs->isRO()) caption = caption + " " + tr("(read-only)");

        setWindowTitle( QString("Firewall Builder: ")+caption );

        m_mainWindow->fileSaveAction->setEnabled( !rcs->isRO() && !rcs->isTemp());
        m_mainWindow->addToRCSAction->setEnabled( !rcs->isInRCS() && !rcs->isRO());
        m_mainWindow->fileDiscardAction->setEnabled( rcs->isInRCS() && !rcs->isRO());
        m_mainWindow->fileCommitAction->setEnabled( rcs->isInRCS() && !rcs->isRO()); 

    } catch(FWException &ex)
    {
        string trans = ex.getProperties()["failed_transformation"];
        string elem  = ex.getProperties()["failed_element"];

        if(!trans.empty() || !elem.empty())
        {
            QString msg = tr("Exception: %1").arg(ex.toString().c_str());
            if (!trans.empty())
		msg+="\n"+tr("Failed transformation : %1").arg(trans.c_str());
            if (!elem.empty())
		msg+="\n"+tr("XML element : %1").arg(elem.c_str());

             QMessageBox::warning(
                 this,"Firewall Builder", 
                 tr("Error loading file:\n%1").arg(msg),
                 tr("&Continue"), QString::null,QString::null,
                 0, 1 );
        } else
             QMessageBox::warning(
                 this,"Firewall Builder", 
                 tr("Error loading file:\n%1").arg(ex.toString().c_str()),
                 tr("&Continue"), QString::null,QString::null,
                 0, 1 );

        load(this);
        return;
    }

    db()->setReadOnly( rcs->isRO() || rcs->isTemp() );

// clear dirty flag for all objects, recursively
    if (!forceSave)  db()->setDirty(false);

    sb->showMessage( tr("Building object tree...") );
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);

    om->loadObjects();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);

    sb->showMessage( tr("Indexing...") );
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
    db()->reIndex();

    sb->clearMessage();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);

    setupAutoSave();  
}

bool FWWindow::checkin(bool unlock)
{
/* doing checkin only if we did checkout so rcs!=NULL */
    QString rlog="";

    if (systemFile || rcs==NULL || !rcs->isCheckedOut() || rcs->isTemp())
        return true;

    if (rcs->isDiff()) // if the file hasn't changed, do not need to ask for the comment
    {
        if ( ! st->getRCSLogState())
        {
            RCSFileSaveDialog_q fsd;
            QDialog *fsd_dialog = new QDialog(this);
            fsd.setupUi(fsd_dialog);
            fsd.checkinDialogTitle->setText(
                QString("<b>")+tr("Checking file %1 in RCS").arg(rcs->getFileName())+QString("</b>")
            );
            if ( fsd_dialog->exec()== QDialog::Rejected ) 
            {
                delete fsd_dialog;
                return false;
            }

            bool    empty_rcslog = fsd.nolog->isChecked();
            if (empty_rcslog)
            {
                rlog = "";
                st->setRCSLogState(true);
            } else
                rlog = fsd.rcslog->toPlainText(); 
                
            delete fsd_dialog;
        }
    }


/***********************************************************************/
    try
    {
        if (fwbdebug)  qDebug("about to check the file in");
        rcs->ci(rlog,unlock);
        if (fwbdebug)  qDebug("done");
    }
    catch (FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder", 
            tr("Error checking in file %1:\n%2")
            .arg(rcs->getFileName()).arg(ex.toString().c_str()),
            tr("&Continue"), QString::null, QString::null,
            0, 1 );
    }
/***********************************************************************/
    return true;
}

void FWWindow::save()
{
    if (fwbdebug)
        qDebug("FWWindow::save:  rcs=%p  rcs->isRO=%d  rcs->isTemp=%d rcs->getFileName=%s",
               rcs, rcs->isRO(), rcs->isTemp(), rcs->getFileName().toLatin1().constData());

    if (!rcs->isRO() && !rcs->isTemp())
    {
        try
        {
            if (rcs->getFileName().isEmpty())
                fileSaveAs();  // eventually calls this method again
            else
            {
/* editingLibfile is true if user edits a library or master library file */
                bool editingLibfile=editingLibrary();

                if (st->getDontSaveStdLib())  // this is now default
                {
                    list<FWObject*> userLibs;
                    list<FWObject*> ll = mw->db()->getByType(Library::TYPENAME);
                    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
                    {
                        if (fwbdebug) qDebug("FWWindow::save()  lib %s",
                                             (*i)->getName().c_str() );
/* if we are not editing a library file, skip preloaded libraries */
                        if (!editingLibfile &&
                            addOnLibs->isLoaded((*i)->getName().c_str()))
                        {
                            if (fwbdebug) qDebug("                   skip");
                            continue;
                        }
/* skip standard and template libraries unless we edit them */
                        QString s=(*i)->getId().c_str();
                        if (s==STANDARD_LIB && !editingStandardLib) continue;
                        if (s==TEMPLATE_LIB && !editingTemplateLib) continue;

                        if (fwbdebug) qDebug("                   add");
                        userLibs.push_back( *i );
                    }

                    QApplication::setOverrideCursor(QCursor( Qt::WaitCursor));

                    FWObjectDatabase *ndb = mw->db()->exportSubtree(userLibs);

                    if (editingLibfile)
                    {
/* exported libraries are always read-only */
                        list<FWObject*> ll = ndb->getByType(Library::TYPENAME);
                        for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
                            if ((*i)->getId()!=STANDARD_LIB &&
                                (*i)->getId()!=DELETED_LIB) (*i)->setReadOnly( true );
                    }

                    ndb->resetTimeLastModified( db()->getTimeLastModified() );
                    ndb->saveFile( rcs->getFileName().toLatin1().constData() );

                    delete ndb;

                    QApplication::restoreOverrideCursor();

                } else
                {
                    QApplication::setOverrideCursor(QCursor( Qt::WaitCursor));
                    db()->saveFile( rcs->getFileName().toLatin1().constData() );
                    QApplication::restoreOverrideCursor();
                }
            }
            db()->setDirty(false);
        }
        catch (FWException &ex)
        {
            QApplication::restoreOverrideCursor();

/* error saving the file. Since XMLTools does not return any useful
 * error message in the exception, let's check for obvious problems here
 */
            QString err;
            if (access( rcs->getFileName().toLatin1().constData(), W_OK)!=0 && errno==EACCES)
                err=tr("File is read-only");
            else
                err=ex.toString().c_str();

            QMessageBox::warning(
                this,"Firewall Builder", 
                tr("Error saving file %1: %2")
                .arg(rcs->getFileName()).arg(err),
                tr("&Continue"), QString::null, QString::null,
                0, 1 );
        }
    }
}

void FWWindow::loadLibrary(const string &libfpath)
{
    MessageBoxUpgradePredicate upgrade_predicate;

    try
    {
        FWObjectDatabase *ndb = new FWObjectDatabase();
        ndb->load(libfpath,  &upgrade_predicate,  librespath);

        FWObject *dobj = ndb->findInIndex(FWObjectDatabase::getDeletedObjectsId());
        if (dobj) ndb->remove(dobj, false);

#if 0
        list<FWObject*> newLibs;
        newLibs= ndb->getByType(Library::TYPENAME);

        list<FWObject*> currentLibs;
        currentLibs= db()->getByType(Library::TYPENAME);

        list<FWObject*> duplicateLibs;

        for (list<FWObject*>::iterator i=newLibs.begin(); i!=newLibs.end(); i++)
        {
            string newLibID = (*i)->getId();
            if (newLibID==STANDARD_LIB)
            {
                duplicateLibs.push_back(*i);
                continue;
            }
            QString name = (*i)->getName().c_str();
            if (std::find_if(currentLibs.begin(),currentLibs.end(),
                             findFWObjectIDPredicate(newLibID))!=currentLibs.end() )
            {
                QMessageBox::warning(
                    NULL,"Firewall Builder", 
                    QObject::tr("Duplicate library '%1'").arg(QString::fromUtf8(name)),
                    QObject::tr("&Continue"), QString::null,QString::null,
                    0, 1 );
                duplicateLibs.push_back(*i);
            }
        }
    
        if (!duplicateLibs.empty())
        {
            for (list<FWObject*>::iterator i=duplicateLibs.begin(); i!=duplicateLibs.end(); i++)
                ndb->remove(*i,false);
        }
#endif
        MergeConflictRes mcr(this);
        db()->merge(ndb, &mcr); 

        delete ndb;

    } catch(FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder", 
            tr("Error loading file %1:\n%2").
                 arg(libfpath.c_str()).arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
    } 
}

void FWWindow::fileImport()
{
    fd->reset();

    QString fname = QFileDialog::getOpenFileName( this,
            tr("Choose a file to import"),
            st->getWDir(),
            "Firewall Builder 4 (2) files (*.fwl);;FWB Files (*.fwb);;All Files (*)");

    if (fname.isEmpty()) return;   // Cancel  - keep working with old file

    loadLibrary( fname.toLatin1().constData() );

    om->loadObjects();
    showFirewalls( true ); 

//    addOnLibs->add( fname.toLatin1().constData() );
}


void FWWindow::fileCompare()
{
    fd->reset();   // fd : find dialog

    QMessageBox initial_question( QMessageBox::Information, "Firewall Builder",
                    tr("This operation inspects two data files (either .fwb or .fwl) and finds conflicting objects. Conflicting objects have the same internal ID but different attributes. Two data files can not be merged, or one imported into another, if they contain such objects. This operation also helps identify changes made to objects in two copies of the same data file.<br><br>This operation does not find objects present in one file but not in the other, such objects present no problem for merge or import operations.<br><br>This operation works with two external files, neither of which needs to be opened in the program. Currently opened data file is not affected by this operation and objects in the tree do not change.<br><br>Do you want to proceed ?"),
                    QMessageBox::Yes | QMessageBox::No);
    
    initial_question.setTextFormat( Qt::RichText );
    if (initial_question.exec() != QMessageBox::Yes) return;


    QString fname1 = QFileDialog::getOpenFileName( this,
            tr("Choose the first file"),
            st->getWDir(),
            "Firewall Builder 4 (2) files (*.fwb);;FWB Library Files (*.fwl);;All Files (*)");

    if (fname1.isEmpty()) return;   // Cancel

    QString fname2 = QFileDialog::getOpenFileName( this,
            tr("Choose the second file"),
            st->getWDir(), 
            "Firewall Builder 4 (2) files (*.fwb);;FWB Library Files (*.fwl);;All Files (*)");

    if (fname2.isEmpty()) return;   // Cancel

    MessageBoxUpgradePredicate upgrade_predicate;

    FWObjectDatabase *db1;
    FWObjectDatabase *db2;
    FWObject *dobj;

    try
    {
        db1 = new FWObjectDatabase();
        db1->load(fname1.toLatin1().constData(),  &upgrade_predicate,  librespath);

        dobj = db1->findInIndex(FWObjectDatabase::getDeletedObjectsId());
        if (dobj) db1->remove(dobj, false);
    } catch(FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder", 
            tr("Error loading file %1:\n%2").
                 arg(fname1).arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
        return;
    }

    try
    {
        db2 = new FWObjectDatabase();
        db2->load(fname2.toLatin1().constData(),  &upgrade_predicate,  librespath);

        dobj = db2->findInIndex(FWObjectDatabase::getDeletedObjectsId());
        if (dobj) db2->remove(dobj, false);
    } catch(FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder", 
            tr("Error loading file %1:\n%2").
                 arg(fname2).arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
        return;
    }

    try
    {
        // CompareObjectsDialog is just like ObjConflictResolutionDialog
        // except it always returns 'accepted' and keeps record
        // of all object differences so we can print report in the end

        CompareObjectsDialog cod(this);
        db1->merge(db2, &cod);
        list<QString> report = cod.getReport();

        delete db1;
        delete db2;

        ostringstream str;
        str << cod.getNumberOfConflicts();

        QMessageBox mb( QMessageBox::Information, "Firewall Builder",
                        tr("Total number of conflicting objects: %1.\nDo you want to generate report?").arg(str.str().c_str()),
                        QMessageBox::Yes | QMessageBox::No);

        if (mb.exec() == QMessageBox::Yes)
        {
            // save report to a file

            QString destdir = getDestDir(fname1);

            QString fn = QFileDialog::getSaveFileName( this,
                           tr("Choose name and location for the report file"),
                           destdir,
                           tr( "TXT Files (*.txt);;All Files (*)" ));

            if (fwbdebug)
                qDebug( QString("Saving report to %1").arg(fn).toAscii().constData() );

            if (fn.isEmpty() ) return ;   // Cancel

            if (!fn.endsWith(".txt"))
            {
                fn+=".txt";
            }

            QFile report_file(fn);
            if (report_file.open(QIODevice::WriteOnly))
            {
                QTextStream report_stream(&report_file);
                for (list<QString>::iterator i=report.begin(); i!=report.end(); ++i)
                {
                    report_stream << *i;
                }
                report_file.close();
            } else 
            {
                QMessageBox::critical(
                    this,"Firewall Builder", 
                    tr("Can not open report file for writing. File '%1'").arg(fn),
                    tr("&Continue"), QString::null,QString::null,
                    0, 1 );
            }
            
        }

    } catch(FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder", 
            tr("Unexpected error comparing files %1 and %2:\n%3").
                 arg(fname1).arg(fname2).arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
    } 

}

void FWWindow::findExternalRefs(FWObject *lib,
                                       FWObject *root,
                                       list<FWReference*> &extRefs)
{
    FWReference *ref=FWReference::cast(root);
    if (ref!=NULL)
    {
        FWObject *plib = ref->getPointer()->getLibrary();
        if ( plib->getId()!=STANDARD_LIB &&
             plib->getId()!=DELETED_LIB  &&
             plib!=lib )
            extRefs.push_back(ref);
        return;
    } else
    {
        for (FWObject::iterator i=root->begin(); i!=root->end(); i++)
            findExternalRefs(lib, *i, extRefs);

    }
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
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

    list<FWReference*> externalRefs;
    for (list<FWObject*>::iterator i=selectedLibs.begin(); i!=selectedLibs.end(); ++i)
        findExternalRefs( *i, *i, externalRefs);

    QApplication::restoreOverrideCursor();

    if (fwbdebug) qDebug("LibExportDialog::accept  externalRefs.size()=%d",
                         externalRefs.size() );

/*
 * if externalRefs.size()!=0, then there were some references pointing
 * outside of the libraries we export. Some of these references may
 * point at other libraries we export, lets find these.
 */
    list<FWReference*> externalRefs2;
    for (list<FWReference*>::iterator i=externalRefs.begin(); i!=externalRefs.end(); ++i)
    {
        FWObject *tgt    = (*i)->getPointer();
        FWObject *tgtlib = tgt->getLibrary();

        if (std::find(selectedLibs.begin(),selectedLibs.end(),tgtlib)!=selectedLibs.end()) continue;
        externalRefs2.push_back(*i);
    }

    if (externalRefs2.size()!=0)
    {
        QString objlist = "";
        QString s       = "";

        for (list<FWReference*>::iterator i=externalRefs2.begin(); i!=externalRefs2.end(); ++i)
        {
            FWReference *robj   = *i;
            FWObject *selLib = robj->getLibrary();
            FWObject *pp     = robj->getParent();
            FWObject *tgt    = robj->getPointer();
            FWObject *tgtlib = tgt->getLibrary();

            if (fwbdebug)
            {
                qDebug("LibExportDialog::accept  tgt: %s pp_type: %s lib: %s",
                       tgt->getName().c_str(),
                       pp->getTypeName().c_str(),
                       tgtlib->getName().c_str());
            }

            if (std::find(selectedLibs.begin(),selectedLibs.end(),tgtlib)!=selectedLibs.end()) continue;

            if (RuleElement::cast(pp)!=NULL)
            {
                FWObject *fw       = pp;
                FWObject *rule     = pp;
                FWObject *ruleset  = pp;
                FWObject *iface    = pp;

                while (rule!=NULL && Rule::cast(rule)==NULL)
                    rule=rule->getParent();
                while (ruleset!=NULL && RuleSet::cast(ruleset)==NULL) 
                    ruleset=ruleset->getParent();
                while (iface!=NULL && Interface::cast(iface)==NULL)
                    iface=iface->getParent();
                while (fw!=NULL && Firewall::cast(fw)==NULL)
                    fw=fw->getParent();

                QString rsname;
                if (Policy::cast(ruleset)!=NULL)
                {
                    s = 
   QObject::tr("Library %1: Firewall '%2' (global policy rule #%3) uses object '%4' from library '%5'")
                        .arg(selLib->getName().c_str())
                        .arg(fw->getName().c_str())
                        .arg(Rule::cast(rule)->getPosition())
                        .arg(tgt->getName().c_str())
                        .arg(tgtlib->getName().c_str());
                }
                if (InterfacePolicy::cast(ruleset)!=NULL)
                {
   QObject::tr("Library %1: Firewall '%2' (interface %3 policy rule #%4) uses object '%5' from library '%6'")
                        .arg(selLib->getName().c_str())
                        .arg(fw->getName().c_str())
                        .arg(iface->getName().c_str())
                        .arg(Rule::cast(rule)->getPosition())
                        .arg(tgt->getName().c_str())
                        .arg(tgtlib->getName().c_str());
                }
                if (NAT::cast(ruleset)!=NULL)
                {
                    s = 
   QObject::tr("Library %1: Firewall '%2' (NAT rule #%3) uses object '%4' from library '%5'")
                        .arg(selLib->getName().c_str())
                        .arg(fw->getName().c_str())
                        .arg(Rule::cast(rule)->getPosition())
                        .arg(tgt->getName().c_str())
                        .arg(tgtlib->getName().c_str());
                }
            } else
            {
                    s = 
   QObject::tr("Library %1: Group '%2' uses object '%3' from library '%4'")
                        .arg(selLib->getName().c_str())
                        .arg(pp->getName().c_str())
                        .arg(tgt->getName().c_str())
                        .arg(tgtlib->getName().c_str());
            }
            s = s + "\n";

            if (fwbdebug) qDebug(s.toAscii().constData());

            objlist = objlist + s;
        }

        longTextDialog ltd( this,

            tr("A library that you are trying to export contains references\n"
               "to objects in the other libraries and can not be exported.\n"
               "The following objects need to be moved outside of it or\n"
               "objects that they refer to moved in it:"),
                            objlist );

        ltd.exec();
        return false;
    }
    return true;
}

void FWWindow::exportLibraryTo(QString fname,list<FWObject*> &selectedLibs, bool rof)
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

    FWObjectDatabase *ndb = mw->db()->exportSubtree( selectedLibs );

    QApplication::restoreOverrideCursor();

    if (rof)
    {
        for (list<FWObject*>::iterator i=selectedLibs.begin(); i!=selectedLibs.end(); ++i)
        {
            FWObject *nlib= ndb->findInIndex( (*i)->getId() );
            if (nlib && nlib->getId()!=DELETED_LIB)
                nlib->setReadOnly( true );
        }
    }

    try
    {
        ndb->saveFile( fname.toLatin1().constData() );
    }
    catch (FWException &ex)
    {
/* error saving the file. Since XMLTools does not return any useful
 * error message in the exception, let's check for obvious problems here
 */
        QString err;
        if (access( fname.toLatin1().constData(), W_OK)!=0 && errno==EACCES)
            err=QObject::tr("File is read-only");

        QMessageBox::warning(
            this,"Firewall Builder", 
            QObject::tr("Error saving file %1: %2")
            .arg(fname).arg(err),
            "&Continue", QString::null, QString::null,
            0, 1 );
    }
}

void FWWindow::fileExport()
{
    LibExportDialog ed;
    list<FWObject*>  selectedLibs;
    map<int,FWObject*>::iterator i;
    QString path="";
    int lib_idx = -1;
    do
    {
        if (ed.exec()!=QDialog::Accepted) return;
        
        QList<QListWidgetItem*> selitems = ed.m_dialog->libs->selectedItems();
        
        for (i=ed.mapOfLibs.begin(); i!=ed.mapOfLibs.end(); i++)
            if (selitems.contains(ed.m_dialog->libs->item(i->first))) 
                selectedLibs.push_back(i->second);
        
        lib_idx=ed.m_dialog->libs->currentRow ();
        
        if (lib_idx<0 || selectedLibs.size()==0)
        {
            QMessageBox::critical(
                this,"Firewall Builder", 
                tr("Please select a library you want to export."),
                "&Continue", QString::null,QString::null,
                0, 1 );

            return;
        }
    } while (!exportLibraryTest(selectedLibs));
    
    FWObject *selLib = ed.mapOfLibs[ lib_idx ];
    path=st->getWDir()+QString::fromUtf8(selLib->getName().c_str())+".fwl";
    
    fd->reset();
    QString fname;
        fname = QFileDialog::getSaveFileName(
            this,
            "Choose a filename to save under",
            path,
            "Firewall Builder 2 files (*.fwl)");
        
    if (fname.isEmpty()) return;
    if (QFile::exists(fname) &&
         QMessageBox::warning(
             this,"Firewall Builder", 
             tr("The file %1 already exists.\nDo you want to overwrite it ?")
             .arg(fname),
             tr("&Yes"), tr("&No"), QString::null,
             0, 1 )==1 ) return;
    exportLibraryTo(fname,selectedLibs,ed.m_dialog->exportRO->isChecked());
}

void FWWindow::showFirewalls(bool open_first_firewall)
{
    if (fwbdebug)  qDebug("FWWindow::showFirewalls");

    list<FWObject*> fl;
    findFirewalls(db(), fl);
    fl.sort(FWObjectNameCmpPredicate());

    firewalls.clear();
    m_mainWindow->fwList->clear();
    clearFirewallTabs();
    ruleSetViews.clear();
    m_mainWindow->firewallName->setText("");

    m_mainWindow->insertRuleAction->setEnabled( fl.size()!=0 );
    m_mainWindow->moveRuleAction->setEnabled( fl.size()!=0 );
    m_mainWindow->moveRuleUpAction->setEnabled( fl.size()!=0 );
    m_mainWindow->moveRuleDownAction->setEnabled( fl.size()!=0 );
    m_mainWindow->addRuleAfterCurrentAction->setEnabled( fl.size()!=0 );
    m_mainWindow->removeRuleAction->setEnabled( fl.size()!=0 );
    m_mainWindow->copyRuleAction->setEnabled( fl.size()!=0 );
    m_mainWindow->cutRuleAction->setEnabled( fl.size()!=0 );
    m_mainWindow->pasteRuleAboveAction->setEnabled( fl.size()!=0 );
    m_mainWindow->pasteRuleBelowAction->setEnabled( fl.size()!=0 );

    m_mainWindow->compileAction->setEnabled( fl.size()!=0 );
    m_mainWindow->installAction->setEnabled( fl.size()!=0 );

    if (fl.size()==0)
    {
        m_mainWindow->fwList->addItem( noFirewalls );
        return;
    }

    for (list<FWObject*>::iterator m=fl.begin(); m!=fl.end(); m++)
        addFirewallToList( *m );

    if (open_first_firewall)
    {
        m_mainWindow->fwList->setCurrentIndex( 0 );
        openFirewall( 0 );
    }
    if (fwbdebug)  qDebug("end of FWWindow::showFirewalls");
}

int  FWWindow::findFirewallInList(FWObject *f)
{
    vector<FWObject*>::iterator i;
    int n=0;
    for (i=firewalls.begin(); i!=firewalls.end(); i++,n++)
    {
        if ( (*i)->getId()==f->getId() ) return n;
    }
    return -1; 
}

void FWWindow::addFirewallToList(FWObject *o)
{
    QString icn_filename =
        ( ":/Icons/"+o->getTypeName()+"icon-tree" ).c_str();

    int n=m_mainWindow->fwList->count();

    if (fwbdebug) qDebug("FWWindow::addFirewallToList %d %p %s",
                         n, o, o->getName().c_str() );

    if (m_mainWindow->fwList->currentText() == noFirewalls )
    {
        m_mainWindow->fwList->removeItem(0);
    }

    QPixmap pm;
    if ( ! QPixmapCache::find( icn_filename, pm) )
    {
        pm.load( icn_filename );
        QPixmapCache::insert( icn_filename, pm);
    }
    m_mainWindow->fwList->addItem( pm, QString::fromUtf8(o->getName().c_str()) );

    firewalls.push_back(o);

    m_mainWindow->fwList->setCurrentIndex( n ); 
//    openFirewall( n );
}

void FWWindow::removeFirewallFromList(FWObject *o)
{
    if (fwbdebug) qDebug("FWWindow::removeFirewallFromList %p %s",
                         o, o->getName().c_str() );

    vector<FWObject*>::iterator i;
    int  n=0;
    for (i=firewalls.begin(); i!=firewalls.end(); i++,n++)
    {
        if ( (*i)->getId()==o->getId() )
        {
            m_mainWindow->fwList->removeItem(n);
            firewalls.erase( i );
            break;
        }
    }
}

void FWWindow::ensureObjectVisibleInRules(FWReference *obj)
{
    FWObject *p=obj;
    while (p && Firewall::cast(p)==NULL ) p=p->getParent();
    if (p==NULL) return;  // something is broken

    if (p!=getVisibleFirewall()) showFirewall(p);

    p=obj;

    while (p && RuleSet::cast(p)==NULL ) p=p->getParent();
    if (p==NULL) return;  // something is broken

    RuleSetView *rsv = ruleSetViews[p];

    if (rsv==NULL)
    {
        if (fwbdebug)
            qDebug("FWWindow::ensureObjectVisible : orphan rule set found");
        return;
    }

    m_mainWindow->ruleSets->setCurrentIndex(
            m_mainWindow->ruleSets->indexOf(rsv));
    rsv->selectRE( obj );
}

void FWWindow::updateRuleSetViewSelection()
{
    RuleSetView* rv=dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget());
    if (rv!=NULL)
        rv->repaintSelection();
}

void FWWindow::updateTreeViewItemOrder()
{
    //this is for case when tree becomes to be resorted
    //if we do not reopen parent item, some of child
    //items mix incorrectly (maybe bug of QT?)
    om->reopenCurrentItemParent();
}

void FWWindow::updateRuleSetView()
{
//    ruleSets->repaint();
    RuleSetView* rv=dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget());
    if (rv!=NULL) rv->updateAll();
}

void FWWindow::updateRuleOptions()
{
    RuleSetView* rv=dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget());
    if (rv!=NULL) rv->updateCurrentCell();
}

void FWWindow::updateFirewallName(FWObject *obj,const QString &)
{
    if (fwbdebug) qDebug("FWWindow::updateFirewallName ");

    QString icn_filename =
        Resources::global_res->getObjResourceStr(obj, "icon-tree").c_str();

    vector<FWObject*>::iterator i;
    int n = 0;
    for (i=firewalls.begin(); i!=firewalls.end(); i++,n++)
    {
        if ( (*i)->getId()==obj->getId())
        {
            QPixmap pm;
            if ( ! QPixmapCache::find( icn_filename, pm) )
            {
                pm.load( icn_filename );
                QPixmapCache::insert( icn_filename, pm);
            }
            m_mainWindow->fwList->setItemIcon( n, QIcon(pm) );
            m_mainWindow->fwList->setItemText( n,
                QString::fromUtf8(obj->getName().c_str()));
            if (n==m_mainWindow->fwList->currentIndex ())
                m_mainWindow->firewallName->setText(QString::fromUtf8(obj->getName().c_str()));
            return;
        }
    }
}

void FWWindow::deleteFirewall(FWObject *fw)
{
    if (fwbdebug) qDebug("FWWindow::deleteFirewall   - fw %s %s",
                         fw->getName().c_str(), fw->getId().c_str());

    removeFirewallFromList(fw);
    if (visibleFirewall==fw)  visibleFirewall=NULL;
}

void FWWindow::setPolicyBranchTabName(RuleSet *subset)
{
    assert(subset!=NULL);
    PolicyRule *rule = PolicyRule::cast(subset->getParent());
    RuleSetView *rsv = ruleSetViews[subset];
    assert(rsv);
    FWOptions *ropt = rule->getOptionsObject();
    QString branchName = ropt->getStr("branch_name").c_str();
    m_mainWindow->ruleSets->setTabText(m_mainWindow->ruleSets->indexOf(rsv), 
                                       tr("%1").arg(branchName) );
}

void FWWindow::addPolicyBranchTab(RuleSet *subset)
{
    assert(subset!=NULL);

    PolicyRule *rule = PolicyRule::cast(subset->getParent());
    FWOptions *ropt = rule->getOptionsObject();
    QString branchName = ropt->getStr("branch_name").c_str();

    QStatusBar *sb = statusBar();
    sb->showMessage( tr("Building branch policy view '%1'...").arg(branchName) );
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,1000);
    if (fwbdebug)  qDebug("FWWindow::reopenFirewall()   adding branch tab");

//    if (subset==NULL)
//    {
//        subset = new Policy();
//        rule->add(subset);
//    }
    if (ruleSetViews.count(subset)==0)
    {
        RuleSetView *rsv = new PolicyView(Policy::cast(subset),NULL);
        m_mainWindow->ruleSets->addTab(rsv,"Branch"); // temporary name
        ruleSetViews[subset]=rsv;
    }
    setPolicyBranchTabName(subset);

    for (FWObject::iterator i=subset->begin(); i!=subset->end(); i++)
    {
        PolicyRule *srule = PolicyRule::cast(*i);
        if (srule->getAction() == PolicyRule::Branch)
            addPolicyBranchTab(srule->getBranch());
    }

}

void FWWindow::removePolicyBranchTab(RuleSet *subset)
{
    if (subset==NULL) return;
    RuleSetView *rsv = ruleSetViews[subset];
    assert(rsv);
    m_mainWindow->ruleSets->removeTab(m_mainWindow->ruleSets->indexOf(rsv));
    ruleSetViews.erase(subset);
}

void FWWindow::scheduleRuleSetRedraw()
{
    if (!ruleSetRedrawPending)
    {
        ruleSetRedrawPending = true;
        QTimer::singleShot( 0, this, SLOT(redrawRuleSets()) );
    }
}

void FWWindow::redrawRuleSets()
{
    ruleSetRedrawPending = false;
    reopenFirewall();
}

void FWWindow::reopenFirewall()
{
    if (fwbdebug)  qDebug("FWWindow::reopenFirewall()");

    if (ruleSetRedrawPending) return;

    int currentPage = m_mainWindow->ruleSets->currentIndex();

    changingTabs=true;

    clearFirewallTabs();
    ruleSetViews.clear();

    if (firewalls.size()==0 || visibleFirewall==NULL)
    {
        changingTabs=false;
        return;
    }

    QStatusBar *sb = statusBar();
    sb->showMessage( tr("Building policy view...") );
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
    if (fwbdebug)  qDebug("FWWindow::reopenFirewall()   adding Policy tab");

    RuleSetView *rsv;
    Policy *pol=Policy::cast(visibleFirewall->getFirstByType(Policy::TYPENAME));
    m_mainWindow->ruleSets->addTab( rsv=new PolicyView(pol,NULL) , tr("Policy") );
    ruleSetViews[pol]=rsv;

// let the GUI process events to display new tab
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);

// as of 2.1.5 we have rule branches :-)
// so far branches are only supported in policy rules because only there
// we have action which we use to define branching rules

    for (FWObject::iterator i=pol->begin(); i!=pol->end(); i++)
    {
        PolicyRule *rule = PolicyRule::cast(*i);
        if (rule->getAction() == PolicyRule::Branch)
            addPolicyBranchTab(rule->getBranch());
    }

// let the GUI process events to display new tab(s)
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);

    if (Resources::getTargetCapabilityBool(visibleFirewall->getStr("platform"),
                                           "supports_nat"))
    {
        sb->showMessage( tr("Building NAT view...") );
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
        if (fwbdebug)  qDebug("FWWindow::reopenFirewall()   adding NAT tab");

        NAT *nat  = NAT::cast(visibleFirewall->getFirstByType(NAT::TYPENAME));
        m_mainWindow->ruleSets->addTab( rsv=new NATView(nat,NULL) , tr("NAT") );
        ruleSetViews[nat]=rsv;
    }

// let the GUI process events to display new tab
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);

    if (Resources::getTargetCapabilityBool(visibleFirewall->getStr("host_OS"),
                                           "supports_routing"))
    {
        sb->showMessage( tr("Building routing view...") );
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
        if (fwbdebug)  qDebug("FWWindow::reopenFirewall()   adding Routing tab");

        Routing *r = Routing::cast(visibleFirewall->getFirstByType(Routing::TYPENAME));
        m_mainWindow->ruleSets->addTab( rsv=new RoutingView(r,NULL) , tr("Routing") );
        ruleSetViews[r]=rsv;
    }

    sb->clearMessage();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
    if (fwbdebug)  qDebug("FWWindow::reopenFirewall()   all tabs are done");

    m_mainWindow->ruleSets->setCurrentIndex( currentPage );

    changingTabs=false;

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

    m_mainWindow->ruleSets->show();
}

void FWWindow::showFirewall(FWObject *obj)
{
    if (firewalls.size()>0)
    {
        vector<FWObject*>::iterator i;
        int n=0;
        for (i=firewalls.begin(); i!=firewalls.end(); i++,n++)
        {
            if ( (*i)->getId()==obj->getId() )
            {
                m_mainWindow->fwList->setCurrentIndex( n );
                openFirewall( n );
                return;
            }
        }
    }
}

void FWWindow::openFirewall( int idx )
{
    if (fwbdebug)
        qDebug("FWWindow::openFirewall");

    if (firewalls.size()>0)
    {
        if (!oe->isVisible() ||
            requestEditorOwnership(NULL,NULL,ObjectEditor::optNone,true))
        {
            oe->blank(); 
            FWObject *fw = firewalls[idx];
            showFirewallRuleSets(fw);
            visibleFirewall = fw;
            om->openObject(fw);
            lastFirewallIdx=idx;
        } else
            m_mainWindow->fwList->setCurrentIndex( lastFirewallIdx ); 
    } else
        visibleFirewall = NULL;
}

void FWWindow::showFirewallRuleSets( FWObject *fw )
{
    if (fwbdebug)
        qDebug("FWWindow::showFirewallRuleSets");

    if (fw==NULL) return;

    visibleFirewall = fw;
    findObjectWidget->firewallOpened(Firewall::cast(fw)); 
    m_mainWindow->firewallName->setText(QString::fromUtf8(fw->getName().c_str()));
    scheduleRuleSetRedraw();
    //reopenFirewall();
}

void FWWindow::selectRules()
{
//    om->unselect();

    m_mainWindow->insertRuleAction->setEnabled( true );
    m_mainWindow->moveRuleAction->setEnabled( true );
    m_mainWindow->moveRuleUpAction->setEnabled( true );
    m_mainWindow->moveRuleDownAction->setEnabled( true );
    m_mainWindow->addRuleAfterCurrentAction->setEnabled( true );
    m_mainWindow->removeRuleAction->setEnabled( true );
    m_mainWindow->copyRuleAction->setEnabled( true );
    m_mainWindow->cutRuleAction->setEnabled( true );
    m_mainWindow->pasteRuleAboveAction->setEnabled( true );
    m_mainWindow->pasteRuleBelowAction->setEnabled( true );

    m_mainWindow->compileAction->setEnabled( true );
    m_mainWindow->installAction->setEnabled( true );

    RuleSetView* rv=dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget());
    rv->setFocus();
}

void FWWindow::unselectRules()
{
    bool havePolicies = (m_mainWindow->ruleSets->count()!=0);

/* commented this out so that when I hit "Edit" in the object's pop-down
 * menu in a rule, ruleset wont lose focus when object editor is opened.
 * If rule set loses focus, the object's background turns from "selected" color
 * to white and user loses context (which object is shown in the object editor)
 */
    if (havePolicies)
    {
        RuleSetView* rv=dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget());
        
        if (rv && rv->getSelectedObject()!=om->getSelectedObject()) 
        {
            rv->unselect();
            rv->clearFocus();
        }
    }

    m_mainWindow->insertRuleAction->setEnabled( havePolicies ); // enabled if there are policies
    m_mainWindow->moveRuleAction->setEnabled( false );
    m_mainWindow->moveRuleUpAction->setEnabled( false );
    m_mainWindow->moveRuleDownAction->setEnabled( false );
    m_mainWindow->addRuleAfterCurrentAction->setEnabled( false );
    m_mainWindow->removeRuleAction->setEnabled( false );
    m_mainWindow->copyRuleAction->setEnabled( false );
    m_mainWindow->cutRuleAction->setEnabled( false );
    m_mainWindow->pasteRuleAboveAction->setEnabled( false );
    m_mainWindow->pasteRuleBelowAction->setEnabled( false );

    m_mainWindow->compileAction->setEnabled( havePolicies );
    m_mainWindow->installAction->setEnabled( havePolicies );

}

void FWWindow::editCopy()
{
    if (om->isSelected()) om->copyObj();
    else
        if (m_mainWindow->ruleSets->count()!=0)
            dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->copySelectedObject();
}

void FWWindow::editCut()
{
    if (om->isSelected()) om->cutObj();
    else
        if (m_mainWindow->ruleSets->count()!=0)
            dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->cutSelectedObject();
}

void FWWindow::editDelete()
{
    if (om->isSelected()) om->deleteObj();
    //else
    //    if (ruleSets->count()!=0)
    //        dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->deleteSelectedObject();
}

void FWWindow::editPaste()
{
    if (om->isSelected()) om->pasteObj();
    else
        if (m_mainWindow->ruleSets->count()!=0)
            dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->pasteObject();
}

void FWWindow::editPrefs()
{
    PrefsDialog pd(this);
    pd.exec();
}

void FWWindow::closeEvent( QCloseEvent * ev)
{
    if (saveIfModified() && checkin(true))
    {
        if (rcs) delete rcs;
    } else
    {
        ev->ignore();
        return;
    }
    QMainWindow::closeEvent(ev);
//    emit closed();
}

void FWWindow::compile()
{
    if (fwbdebug) qDebug("FWWindow::compile");

    if (oe->isVisible() &&
        !requestEditorOwnership(NULL,NULL,ObjectEditor::optNone,true))
        return;

    fileSave();

    std::set<libfwbuilder::Firewall*> emp;

    instd = new instDialog(NULL,BATCH_COMPILE,emp);
    instd->show();

//    id->exec();
//    delete id;
}

void FWWindow::compile(set<Firewall*> vf)
{
    if (fwbdebug)
        qDebug("FWWindow::compile preselected %d firewalls",vf.size());

    if (oe->isVisible() &&
        !requestEditorOwnership(NULL,NULL,ObjectEditor::optNone,true))
        return;

    fileSave();

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

/*
 * info styles go like this:
 * 0 - collapsed
 * 1 - opened
 * 2 - opened, more information
 *  etc.
 *
 */
void FWWindow::changeInfoStyle()
{
    shownInInfo = NULL;
    switch (st->getInfoStyle())
    {
    case 0:
        st->setInfoStyle(1);
        oi->show();
        break;
    case 1:
        st->setInfoStyle(2);
        oi->show();
        break;
    case 2:
        st->setInfoStyle(0);
        oi->hide();
        break;
    }

/* TODO: now need to send signal to the dialog telling it to change
 * according to the style
 */

    om->info();
}

void FWWindow::insertRule()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->insertRule();
}

void FWWindow::addRuleAfterCurrent()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->addRuleAfterCurrent();
}

void FWWindow::removeRule()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->removeRule();
}

void FWWindow::moveRule()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->moveRule();
}

void FWWindow::moveRuleUp()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->moveRuleUp();
}

void FWWindow::moveRuleDown()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->moveRuleDown();
}

void FWWindow::copyRule()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->copyRule();
}

void FWWindow::cutRule()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->cutRule();
}

void FWWindow::pasteRuleAbove()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->pasteRuleAbove();
}

void FWWindow::pasteRuleBelow()
{
    if (visibleFirewall==NULL || m_mainWindow->ruleSets->count()==0) return;
    dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget())->pasteRuleBelow();
}

void FWWindow::search()
{
    findWhereUsedWidget->hide();
    m_mainWindow->auxiliaryPanel->show();
    findObjectWidget->show();
}

void FWWindow::findWhereUsed(FWObject * obj)
{
    findObjectWidget->hide();
    m_mainWindow->auxiliaryPanel->show();
    findWhereUsedWidget->find(obj);
}

void FWWindow::showEvent( QShowEvent *ev)
{
    QString val = st->getStr("Layout/MainWindowSplitter");
    if (!val.isEmpty())
    {
        int  w1 = val.section(',',0,0).toInt();
        int  w2 = val.section(',',1,1).toInt();

        QList<int> sl;
        sl.push_back(w1);
        sl.push_back(w2);
        m_mainWindow->mainSplitter->setSizes( sl );
    }

    val = st->getStr("Layout/ObjInfoSplitter");
    if (!val.isEmpty())
    {
        int  w1 = val.section(',',0,0).toInt();
        int  w2 = val.section(',',1,1).toInt();

        QList<int> sl;
        sl.push_back(w1);
        sl.push_back(w2);
        m_mainWindow->objInfoSplitter->setSizes( sl );
    }

    st->restoreGeometry(this, QRect(100,100,750,600) );
    QMainWindow::showEvent(ev);
}

void FWWindow::hideEvent( QHideEvent *ev)
{
    st->saveGeometry(this);
    QList<int> sl = m_mainWindow->mainSplitter->sizes();
    QString arg = QString("%1,%2").arg(sl[0]).arg(sl[1]);
    st->setStr("Layout/MainWindowSplitter", arg );

    sl = m_mainWindow->objInfoSplitter->sizes();
    arg = QString("%1,%2").arg(sl[0]).arg(sl[1]);
    st->setStr("Layout/ObjInfoSplitter", arg );

    QMainWindow::hideEvent(ev);
}
    
void FWWindow::newObject()
{
    om->newObject();
}

void FWWindow::lockObject()
{
    om->lockObject();
}

void FWWindow::unlockObject()
{
    om->unlockObject();
}

void FWWindow::prepareObjectMenu()
{
    ObjectTreeView* otv =om->getCurrentObjectTree();
    m_mainWindow->ObjectUnlockAction->setEnabled(otv->isUnlockable());
    m_mainWindow->ObjectLockAction->setEnabled(otv->isLockable());
}


void FWWindow::setupAutoSave()
{
    if ( st->getBool("Environment/autoSaveFile") &&
         rcs!=NULL && rcs->getFileName()!="")
    {
        int p = st->getInt("Environment/autoSaveFilePeriod");
        autosaveTimer->start( p*1000*60 );
        connect( autosaveTimer, SIGNAL(timeout()), this, SLOT(fileSave()) );
    } else
        autosaveTimer->stop();
}

QString FWWindow::getCurrentFileName()
{
    if (rcs!=NULL)  return rcs->getFileName();
    return "";
}

RCS * FWWindow::getRCS()
{
    return rcs;
}

void FWWindow::findObject(FWObject *o)
{
    findWhereUsedWidget->hide();
    if (fwbdebug) qDebug("FWWindow::findObject");
    findObjectWidget->findObject(o);
    m_mainWindow->auxiliaryPanel->show();
    
}

void FWWindow::closeAuxiliaryPanel()
{
    m_mainWindow->auxiliaryPanel->hide();
}

void FWWindow::closeEditorPanel()
{
    m_mainWindow->objectEditorFrame->hide();
}

void FWWindow::openEditorPanel()
{
    m_mainWindow->objectEditorFrame->show();
}

void FWWindow::ruleSetTabChanged(int tab)
{
    QWidget *w = m_mainWindow->ruleSets->widget(tab);

    if (fwbdebug)
        qDebug("FWWindow::ruleSetTabChanged:  w=%p ruleSetTabIndex=%d changingTabs=%d",
               w,ruleSetTabIndex,changingTabs);

    if (changingTabs) return;

    if (!oe->isVisible())
    {
        ruleSetTabIndex = tab;
        return;
    } 

    RuleSetView* rv=dynamic_cast<RuleSetView*>(m_mainWindow->ruleSets->currentWidget());

    if ((ruleSetTabIndex != m_mainWindow->ruleSets->indexOf(w)) && 
        !requestEditorOwnership(rv,NULL,ObjectEditor::optNone,true))
    {
        // this causes recursive call to ruleSetTabChanged
        changingTabs = true;
        m_mainWindow->ruleSets->setCurrentIndex(ruleSetTabIndex);
        changingTabs = false;
        return;
    } 
    ruleSetTabIndex = tab;
    rv->editSelected(); 
//    rollBackSelectionDifferentWidget();  // make widget reopen the same object
}

/*
 * reset tab via callback because calling setCurrentPage from
 * ruleSetTabChanged causes recursive call to ruleSetTabChanged
 */
void FWWindow::restoreRuleSetTab()
{
    if (fwbdebug) qDebug("FWWindow::restoreRuleSetTab()");
    m_mainWindow->ruleSets->setCurrentIndex(ruleSetTabIndex);
}

void FWWindow::releaseEditor()
{
    disconnect( SIGNAL(restoreSelection_sign(bool)) );
}

void FWWindow::connectEditor(QWidget *w)
{
    connect(this,
            SIGNAL(restoreSelection_sign(bool)),
            w,
            SLOT(restoreSelection(bool)));
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
    if (!oe->isVisible()) return false;

    if(obj==oe->getOpened() && 
       otype==oe->getOpenedOpt() &&
       w == editorOwner )
    {
        releaseEditor();
        editorOwner = w;
        connectEditor(editorOwner);
        return true;
    }

    if (validate && !oe->validateAndSave()) 
    {
        /*
         * roll back selection in the widget that currently
         * owns the editor. Signal restoreSelection_sign 
         * is still connected to the previous owner
         */
        if (w == editorOwner )
            QTimer::singleShot( 0, this, SLOT(rollBackSelectionSameWidget()) );
        else
            QTimer::singleShot( 0, this, SLOT(rollBackSelectionDifferentWidget()) );
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


void FWWindow::rollBackSelectionSameWidget()
{
    editorOwner->setFocus();
    emit restoreSelection_sign(true); 
}

void FWWindow::rollBackSelectionDifferentWidget()
{
    editorOwner->setFocus();
    emit restoreSelection_sign(false); 
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

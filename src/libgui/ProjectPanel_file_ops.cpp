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

#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "events.h"

#include <fwbuilder/RuleSet.h>
#include <fwbuilder/Policy.h>
#include <fwbuilder/NAT.h>
#include <fwbuilder/Routing.h>
#include <fwbuilder/Rule.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/Library.h>
#include "fwbuilder/Constants.h"

#include "ProjectPanel.h"

#include <ui_rcsfilesavedialog_q.h>

#include "FWWindow.h"
#include "RCS.h"
#include "filePropDialog.h"
#include "FWBSettings.h"
#include "RCSFilePreview.h"
#include "FindObjectWidget.h"
#include "FWObjectClipboard.h"
#include "upgradePredicate.h"
#include "ObjConflictResolutionDialog.h"
#include "LibExportDialog.h"
#include "longTextDialog.h"
#include "FWBTree.h"
#include "ObjectTreeView.h"

#include <QtDebug>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QStatusBar>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QUndoStack>

#include <list>
#include <algorithm>

#include <errno.h>

#include <libxml/tree.h>

#define LONG_ERROR_CUTOFF 1024

using namespace Ui;
using namespace libfwbuilder;
using namespace std;

bool ProjectPanel::saveIfModified(bool include_discard_button)
{
    if (fwbdebug) qDebug() << "ProjectPanel::saveIfModified()";

    if (db() && db()->isDirty())
//    if (db() && db()->isDirty() && rcs && !rcs->getFileName().isEmpty())
    {
        QString message = "Some objects have been modified but not saved.\n";
        message += "Do you want to save ";
        message += rcs->getFileName();
        message += " changes now ?"; 

        if (include_discard_button)
        {
            switch (QMessageBox::information(this, "Firewall Builder",
                                             message,
                                             tr("&Save"), tr("&Discard"), tr("&Cancel"),
                                             0,      // Enter = button 0
                                             2 ) )   // Escape == button 2
            {
            case 0:
                save();
                break;
            case 1:  // discard
                db()->setDirty(false);
                break;
            case 2:  // cancel
                return false;
            }
        } else
        {
            switch (QMessageBox::information(this, "Firewall Builder",
                                             message,
                                             tr("&Save"),tr("&Cancel"),
                                             nullptr,      // Enter = button 0
                                             1 ) )   // Escape == button 1
            {
            case 0:
                save();
                break;
            case 1:  // cancel
                return false;
            }
        }

    }
    return true;
}

#ifdef _WIN32
#  include <io.h>       // for access
#  define W_OK 2        // for access
#endif


QString ProjectPanel::chooseNewFileName(const QString &fname,
                                        const QString &title)
{
    // when file open dialog is created using static function
    // QFileDialog::getSaveFileName, its behavior is different on
    // Linux and Mac (did not check on windows)

    // Native dialog usd by Qt on Mac automatically adds .fwb suffix
    // to the file name user enters if it does not have any suffix. It
    // checks for the conflicts with exitsing files _after_ the suffix
    // has been added. On Linux dialog created by the static function
    // QFileDialog::getSaveFileName does not add suffix and checks for
    // conflicts using the name without one. Since I used to add
    // suffix here but did not check for the conflict again after
    // that, it was possible for the user to enter name with no suffix
    // and that way overwrite old file without warning. Will avoid
    // static fucntion and instead build dialog manually and use
    // setDefaultSuffix() to enforce suffix.

    QFileDialog fd(this);
    fd.setFileMode(QFileDialog::AnyFile);
    fd.setDefaultSuffix("fwb");
    fd.setNameFilter(tr( "FWB Files (*.fwb);;All Files (*)" ) );
    fd.setWindowTitle(title);
    fd.setDirectory(st->getOpenFileDir(fname));
    fd.setAcceptMode(QFileDialog::AcceptSave);

    QString fn;
    if (fd.exec())
    {
        QStringList fileNames = fd.selectedFiles();
        fn = fileNames.front();
        QFileInfo finfo(fn);
        if (finfo.suffix().isEmpty()) fn += ".fwb";
        st->setOpenFileDir(fn);
    }

    return fn;
}


void ProjectPanel::fileProp()
{
    if (rcs!=nullptr)
    {
        filePropDialog fpd(this,rcs);
        fpd.setPrinter(mainW->getPrinter());
        fpd.exec();
    }
}

bool ProjectPanel::fileNew()
{
    if (fwbdebug) qDebug("ProjectPanel::fileNew()");

    QString nfn = chooseNewFileName(
        st->getWDir(), tr("Choose name and location for the new file"));

    if ( !nfn.isEmpty() )
    {
        //if (!saveIfModified() || !checkin(true)) return;
        if (!systemFile && rcs!=nullptr) 
            fileClose();       // fileClose calls load(this)
        else  
            loadStandardObjects();

        visibleFirewall = nullptr;
        setFileName(nfn);
        save();
        setupAutoSave();

        loadFirstNonStandardLib();

        QCoreApplication::postEvent(mw, new updateSubWindowTitlesEvent());
    }

    if (fwbdebug)
        qDebug("ProjectPanel::fileNew()  rcs=%p  rcs->getFileName()='%s'",
               rcs, rcs == nullptr ? "<null>" :
               rcs->getFileName().toLatin1().constData());

    return (rcs!=nullptr);
}

bool ProjectPanel::loadFile(const QString &fileName, bool load_rcs_head)
{
    if (fwbdebug)
        qDebug() << "ProjectPanel::loadFile  fileName="
                 << fileName.toLocal8Bit()
                 << "load_rcs_head=" << load_rcs_head;

    RCSFilePreview  fp(this);
    bool hasRCS = fp.showFileRLog(fileName);

    // class RCS automatically selected head revision when it opens
    // file and reads RCS log. User can choose different revision
    // using RCSFilePreview. If they do, RCSFilePreview::getSelectedRev()
    // returns selected revision. However if they do not choose, or
    // dialog is never executed, getSelectedRev() returns head.

    // if load_rcs_head == true, we do not run RCSFilePreview dialog
    // which meand it will return head revision automatically.

    int dlg_res = QDialog::Accepted;
    if (hasRCS && !load_rcs_head) dlg_res = fp.exec();
    if (dlg_res!=QDialog::Accepted) return false;

    if (!saveIfModified() || !checkin(true)) return false;

    if (!systemFile && rcs!=nullptr)
    {
        if (mw->isEditorVisible()) mw->hideEditor();
        reset();
    }

    //try to get simple rcs instance from RCS preview
    RCS *new_rcs = fp.getSelectedRev();

    //if preview cannot give RCS,
    //get a new RCS from file dialog
    if (new_rcs==nullptr) new_rcs = new RCS(fileName);
    if (new_rcs==nullptr) return false;

    try
    {
        new_rcs->co();
        if (loadFromRCS(new_rcs)) 
        {
            if (new_rcs->isTemp())
                unlink(new_rcs->getFileName().toLocal8Bit().constData());

            st->setOpenFileDir(getFileDir(fileName));

            // For Diff Viewer
            if (origObjdb)
                delete origObjdb;
            origObjdb = new FWObjectDatabase(*objdb);
            origObjdb->reIndex();

            return true;
        }

    } catch (FWException &ex)
    {
        return false;
    }

    return false;
}

void ProjectPanel::fileClose()
{
    if (fwbdebug) qDebug("ProjectPanel::fileClose(): start");
    if (mw->isEditorVisible()) mw->hideEditor();
    if (!saveIfModified() || !checkin(true))  return;
    reset();
    mdiWindow->close();
    mw->setCompileAndInstallActionsEnabled(false);
    if (fwbdebug) qDebug("ProjectPanel::fileClose(): done");
}

/*
 * slot that is called by a timer if user turned on auto-save feature
 * using controls in the Preferences dialog. Need to save only if data
 * was modified (flag "dirty" is set).
 */
void ProjectPanel::autoSave()
{
    if (db() && db()->isDirty() && rcs && !rcs->getFileName().isEmpty())
        fileSave();
}

void ProjectPanel::fileSave()
{
    save();
}

void ProjectPanel::fileSaveAs()
{
    if (mw->isEditorVisible()) mw->hideEditor();

/* we need to save data into the current file before we save it into a
 * new file, provided we do have current file
    if (!systemFile && rcs &&
        !rcs->isRO() && !rcs->isTemp() && !rcs->getFileName().isEmpty() &&
        (!saveIfModified() || !checkin(true))
    ) return;

 */


/* need to close the file without asking and saving, then reopen it again */

    QString oldFileName = rcs->getFileName();

    QString newFileName = chooseNewFileName(
        oldFileName, tr("Choose name and location for the file"));

    if (!newFileName.isEmpty())
    {
        db()->setDirty(false);  // so it wont ask if user wants to save
        rcs->abandon();
        if (rcs!=nullptr) delete rcs;
        rcs = new RCS("");
        setFileName(newFileName);
        save();

        mw->updateOpenRecentMenu(newFileName);
    }
}

void ProjectPanel::fileCommit()
{
    save();
    if (!checkin(true))  return;
    rcs->co();
}

/*
 * discard changes done to the file and check out clean copy of the
 * head revision from RCS
 */
void ProjectPanel::fileDiscard()
{
    if (QMessageBox::warning(this, "Firewall Builder",
      tr("This operation discards all changes that have been saved "
         "into the file so far, closes it and replaces it with a clean "
         "copy of its head revision from RCS."
         "\n"
         "All changes will be lost if you do this.\n"),
      tr("&Discard changes"),
      tr("&Cancel"), QString::null,
      1 )==0 )
    {
        /* need to close the file without asking and saving, then
         * reopen it again
         */

        QString fname = rcs->getFileName();

        db()->setDirty(false);  // so it wont ask if user wants to save
        rcs->abandon();

        /* do everything fileClose() does except do not close mdiWindow
         * because we'll need it again to reopen the file into
         */
        if (mw->isEditorVisible()) mw->hideEditor();

        if (rcs) delete rcs;
        rcs=nullptr;

        FWObjectClipboard::obj_clipboard->clear();

        firewalls.clear();
        visibleFirewall = nullptr;
        visibleRuleSet = nullptr;
        clearFirewallTabs();
        clearObjects();

        /* loadFile calls fileClose, but only if file is currently
         * open, which it isn't because we reset rcs to nullptr
         */
        loadFile(fname, false);
    }
}

void ProjectPanel::fileAddToRCS()
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

    QCoreApplication::postEvent(mw, new updateSubWindowTitlesEvent());
}

void ProjectPanel::fileImport()
{
    resetFD();

    QString fname = QFileDialog::getOpenFileName(
        mainW,
        tr("Choose a file to import"),
        st->getWDir(),
        "FWB library files (*.fwl);;FWB Files (*.fwb);;All Files (*)");

    if (fname.isEmpty()) return;   // Cancel  - keep working with old file

    FWObject *new_lib = loadLibrary( fname.toLocal8Bit().constData() );

    loadObjects();

    m_panel->om->openLib(new_lib);
}


void ProjectPanel::fileCompare()
{
    resetFD();

    QMessageBox initial_question( QMessageBox::Information, "Firewall Builder",
                    tr("This operation inspects two data files (either .fwb or .fwl) and finds conflicting objects. Conflicting objects have the same internal ID but different attributes. Two data files can not be merged, or one imported into another, if they contain such objects. This operation also helps identify changes made to objects in two copies of the same data file.<br><br>This operation does not find objects present in one file but not in the other, such objects present no problem for merge or import operations.<br><br>This operation works with two external files, neither of which needs to be opened in the program. Currently opened data file is not affected by this operation and objects in the tree do not change.<br><br>Do you want to proceed ?"),
                    QMessageBox::Yes | QMessageBox::No);

    initial_question.setTextFormat( Qt::RichText );
    if (initial_question.exec() != QMessageBox::Yes) return;


    QString fname1 = QFileDialog::getOpenFileName(
        mainW,
        tr("Choose the first file"),
        st->getOpenFileDir(),
        "FWB files (*.fwb);;FWB Library Files (*.fwl);;All Files (*)");

    if (fname1.isEmpty()) return;   // Cancel
    st->setOpenFileDir(fname1);

    QString fname2 = QFileDialog::getOpenFileName(
        mainW,
        tr("Choose the second file"),
        st->getOpenFileDir(),
        "FWB files (*.fwb);;FWB Library Files (*.fwl);;All Files (*)");

    if (fname2.isEmpty()) return;   // Cancel
    st->setOpenFileDir(fname2);

    MessageBoxUpgradePredicate upgrade_predicate(mainW);

    FWObjectDatabase *db1;
    FWObjectDatabase *db2;
    FWObject *dobj;

    try
    {
        db1 = new FWObjectDatabase();
        db1->load(fname1.toLocal8Bit().constData(),
                  &upgrade_predicate,  Constants::getDTDDirectory());

        dobj = db1->findInIndex(FWObjectDatabase::DELETED_OBJECTS_ID);
        if (dobj) db1->remove(dobj, false);
    } catch(FWException &ex)
    {
        QMessageBox::critical(
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
        db2->load(fname2.toLocal8Bit().constData(),
                  &upgrade_predicate,  Constants::getDTDDirectory());

        dobj = db2->findInIndex(FWObjectDatabase::DELETED_OBJECTS_ID);
        if (dobj) db2->remove(dobj, false);
    } catch(FWException &ex)
    {
        QMessageBox::critical(
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

            QString fn = QFileDialog::getSaveFileName( this,
                           tr("Choose name and location for the report file"),
                           st->getOpenFileDir(fname1),
                           tr( "TXT Files (*.txt);;All Files (*)" ));

            if (fn.isEmpty()) return;   // Cancel
            st->setOpenFileDir(fn);

            if (!fn.endsWith(".txt")) fn += ".txt";
            if (fwbdebug)
                qDebug() << QString("Saving report to %1").arg(fn);

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
        QMessageBox::critical(
            this,"Firewall Builder",
            tr("Unexpected error comparing files %1 and %2:\n%3").
                 arg(fname1).arg(fname2).arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
    }

}

void ProjectPanel::fileExport()
{
    LibExportDialog ed;
    list<FWObject*>  selectedLibs;
    map<int,FWObject*>::iterator i;
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
    QString path = st->getOpenFileDir() + QString::fromUtf8(selLib->getName().c_str()) + ".fwl";

    resetFD();

    QString fname = QFileDialog::getSaveFileName(
        this,
        "Choose a filename to save under",
        path,
        "Firewall Builder library files (*.fwl)");

    if (fname.isEmpty()) return;

    if (QFile::exists(fname) &&
         QMessageBox::warning(
             this,"Firewall Builder",
             tr("The file %1 already exists.\nDo you want to overwrite it ?")
             .arg(fname),
             tr("&Yes"), tr("&No"), QString::null,
             0, 1 )==1 ) return;

    st->setOpenFileDir(path);
    exportLibraryTo(fname,selectedLibs,ed.m_dialog->exportRO->isChecked());
}

bool ProjectPanel::exportLibraryTest(list<FWObject*> &selectedLibs)
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
                         int(externalRefs.size()) );

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

        for (list<FWReference*>::iterator i=externalRefs2.begin();
             i!=externalRefs2.end(); ++i)
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

            if (RuleElement::cast(pp)!=nullptr)
            {
                FWObject *fw       = pp;
                FWObject *rule     = pp;
                FWObject *ruleset  = pp;
                FWObject *iface    = pp;

                while (rule!=nullptr && Rule::cast(rule)==nullptr)
                    rule=rule->getParent();
                while (ruleset!=nullptr && RuleSet::cast(ruleset)==nullptr)
                    ruleset=ruleset->getParent();
                while (iface!=nullptr && Interface::cast(iface)==nullptr)
                    iface=iface->getParent();
                while (fw!=nullptr && Firewall::cast(fw)==nullptr)
                    fw=fw->getParent();

                s = QObject::tr("Library %1: Firewall '%2' (%3 rule #%4) uses "
                                "object '%5' from library '%6'")
                    .arg(selLib->getName().c_str())
                    .arg(fw->getName().c_str())
                    .arg(ruleset->getTypeName().c_str())
                    .arg(Rule::cast(rule)->getPosition())
                    .arg(tgt->getName().c_str())
                    .arg(tgtlib->getName().c_str());

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

            if (fwbdebug) qDebug() << s;

            objlist = objlist + s;
        }

        longTextDialog ltd(
            this,
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

void ProjectPanel::exportLibraryTo(QString fname,list<FWObject*> &selectedLibs, bool rof)
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

    FWObjectDatabase *ndb = db()->exportSubtree( selectedLibs );

    QApplication::restoreOverrideCursor();

    if (rof)
    {
        for (list<FWObject*>::iterator i=selectedLibs.begin(); i!=selectedLibs.end(); ++i)
        {
            FWObject *nlib= ndb->findInIndex( (*i)->getId() );
            if (nlib && nlib->getId()!=FWObjectDatabase::DELETED_OBJECTS_ID)
                nlib->setReadOnly( true );
        }
    }

    try
    {
        xmlSetCompressMode(st->getCompression() ? 9 : 0);
        ndb->saveFile( fname.toLocal8Bit().constData() );
    }
    catch (FWException &ex)
    {
/* error saving the file. Since XMLTools does not return any useful
 * error message in the exception, let's check for obvious problems here
 */
        QString err;
        if (access( fname.toLocal8Bit().constData(), W_OK)!=0 && errno==EACCES)
            err=QObject::tr("File is read-only");

        QMessageBox::critical(
            this,"Firewall Builder",
            QObject::tr("Error saving file %1: %2")
            .arg(fname).arg(err),
            "&Continue", QString::null, QString::null,
            0, 1 );
    }
}

void ProjectPanel::setupAutoSave()
{
    if ( st->getBool("Environment/autoSaveFile") &&
         rcs!=nullptr && rcs->getFileName()!="")
    {
        int p = st->getInt("Environment/autoSaveFilePeriod");
        autosaveTimer->start( p*1000*60 );
        connect( autosaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()) );
    } else
        autosaveTimer->stop();
}

void ProjectPanel::findExternalRefs(FWObject *lib,
                                       FWObject *root,
                                       list<FWReference*> &extRefs)
{
    FWReference *ref=FWReference::cast(root);
    if (ref!=nullptr)
    {
        FWObject *plib = ref->getPointer()->getLibrary();
        if ( plib->getId()!=FWObjectDatabase::STANDARD_LIB_ID &&
             plib->getId()!=FWObjectDatabase::DELETED_OBJECTS_ID  &&
             plib!=lib )
            extRefs.push_back(ref);
        return;
    } else
    {
        for (FWObject::iterator i=root->begin(); i!=root->end(); i++)
            findExternalRefs(lib, *i, extRefs);

    }
}

/**
 * Load library or several libraries from an external file. Return
 * pointer to the last new imported library.
 */
FWObject* ProjectPanel::loadLibrary(const string &libfpath)
{
    MessageBoxUpgradePredicate upgrade_predicate(mainW);
    FWObject *last_new_lib = nullptr;

    try
    {
        FWObjectDatabase *ndb = new FWObjectDatabase();
        ndb->load(libfpath,  &upgrade_predicate,  Constants::getDTDDirectory());

        FWObject *dobj = ndb->findInIndex(FWObjectDatabase::DELETED_OBJECTS_ID);
        if (dobj) ndb->remove(dobj, false);

        set<int> duplicate_ids;
        db()->findDuplicateIds(ndb, duplicate_ids);

        map<int, int> id_mapping;
        for (set<int>::iterator it=duplicate_ids.begin(); it!=duplicate_ids.end();
             ++it)
        {
            FWObject *obj = ndb->findInIndex(*it);
            assert(obj!=nullptr);
            int new_id = FWObjectDatabase::generateUniqueId();
            obj->setId(new_id);
            id_mapping[*it] = new_id;

            // cerr << "Duplicate ID: " << *it 
            //      << " " << FWObjectDatabase::getStringId(*it)
            //      << obj->getPath()
            //      << endl;
        }
        ndb->fixReferences(ndb, id_mapping);

        int new_lib_id = -1;

        // check for duplicate library names
        FWObjectTypedChildIterator it2 = ndb->findByType(Library::TYPENAME);
        for (; it2!=it2.end(); ++it2)
        {
            QString new_name = m_panel->om->makeNameUnique(
                db(), QString::fromUtf8((*it2)->getName().c_str()), Library::TYPENAME);
            (*it2)->setName(string(new_name.toUtf8()));
            if ((*it2)->getId() != FWObjectDatabase::STANDARD_LIB_ID)
                new_lib_id = (*it2)->getId();
        }

        MergeConflictRes mcr(this);
        db()->merge(ndb, &mcr);
        delete ndb;

        last_new_lib = db()->findInIndex(new_lib_id);
    } catch(FWException &ex)
    {
        QString error_txt = ex.toString().c_str();
        if (error_txt.length() > LONG_ERROR_CUTOFF) 
        {
            error_txt.truncate(LONG_ERROR_CUTOFF);
            error_txt += "\n\n" + tr("(Long error message was truncated)");
        }
        QMessageBox::critical(
            this,"Firewall Builder",
            tr("The program encountered error trying to load file %1.\n"
               "The file has not been loaded. Error:\n%2").
                 arg(libfpath.c_str()).arg(error_txt),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
    }
    return last_new_lib;
}

/*
 * Load standard library objects
 */
void ProjectPanel::loadStandardObjects()
{
    if (fwbdebug) qDebug("ProjectPanel::load(): start");

    editingStandardLib = false;
    editingTemplateLib = false;

    MessageBoxUpgradePredicate upgrade_predicate(mainW);

    resetFD();

    try
    {
// need to drop read-only flag on the database before I load new objects

        if (objdb)
        {
            objdb->destroyChildren();
            delete objdb;
        }

        objdb = new FWObjectDatabase();
        objdb->setReadOnly( false );

        mw->showStatusBarMessage(tr("Loading system objects..."));

// always load system objects
        if (fwbdebug)
            qDebug("ProjectPanel::load(): sysfname = %s",
                   Constants::getStandardObjectsFilePath().c_str());

        objdb->load( Constants::getStandardObjectsFilePath(),
                     &upgrade_predicate, Constants::getDTDDirectory());
        objdb->setFileName("");

        if (fwbdebug) qDebug("ProjectPanel::load(): create User library");

        FWObject *userLib = FWBTree().createNewLibrary(objdb);

        userLib->setName("User");
        userLib->setStr("color","#d2ffd0");

        objdb->setDirty(false);
        objdb->setFileName("");

        createRCS("");

        //setWindowTitle(getPageTitle());
        QCoreApplication::postEvent(mw, new updateSubWindowTitlesEvent());

        loadObjects();
        setupAutoSave();

        time_t last_modified = objdb->getTimeLastModified();

        if (fwbdebug)
            qDebug("ProjectPanel::load(): done  last_modified=%s dirty=%d",
                   ctime(&last_modified), objdb->isDirty());

        // For Diff Viewer
        if (origObjdb)
            delete origObjdb;
        origObjdb = new FWObjectDatabase(*objdb);
        origObjdb->reIndex();

    } catch(FWException &ex)
    {
        QMessageBox::critical(
            this,"Firewall Builder",
            tr("Error loading file:\n%1").arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
    }
}

bool ProjectPanel::loadFromRCS(RCS *_rcs)
{
    resetFD();

    editingStandardLib = false;
    editingTemplateLib = false;

    bool forceSave=false; // use this flag to force 'save' operation if file should be renamed

    MessageBoxUpgradePredicate upgrade_predicate(mainW);

    assert(_rcs!=nullptr);

    rcs = _rcs;
    try
    {
        /* load the data file */
        systemFile = false;

        clearObjects();

        if (objdb)
        {
            objdb->destroyChildren();
            delete objdb;
        }

        objdb = new FWObjectDatabase();

// need to drop read-only flag on the database before I load new objects
        objdb->setReadOnly( false );

// always loading system objects
        mw->showStatusBarMessage(tr("Loading system objects...") );

        objdb->load( Constants::getStandardObjectsFilePath(),
                     &upgrade_predicate, Constants::getDTDDirectory());
        objdb->setFileName("");

// objects from a data file are in database ndb

        mw->showStatusBarMessage(tr("Reading and parsing data file..."));

        FWObjectDatabase *ndb = new FWObjectDatabase();
        ndb->load(rcs->getFileName().toLocal8Bit().constData(),
                  &upgrade_predicate,Constants::getDTDDirectory());
        time_t   oldtimestamp = ndb->getTimeLastModified();

/* loadingLib is true if user wants to open a library or master library file */
        bool loadingLib = editingLibrary();

        if (fwbdebug)
        {
            list<FWObject*> ll = ndb->getByType(Library::TYPENAME);
            for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
            {
                qDebug("* Found library %s %s in the data file",
                       FWObjectDatabase::getStringId((*i)->getId()).c_str(),
                       (*i)->getName().c_str() );
            }
        }

/* if user opens library file, clear read-only flag so they can edit it */
        if (loadingLib)
        {
            list<FWObject*> ll = ndb->getByType(Library::TYPENAME);
            for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
            {
                if ((*i)->getId()==FWObjectDatabase::STANDARD_LIB_ID)
                    editingStandardLib=true;
                if ((*i)->getId()==FWObjectDatabase::TEMPLATE_LIB_ID)
                    editingTemplateLib=true;
                (*i)->setReadOnly( false );
            }
        }

        mw->showStatusBarMessage(tr("Merging with system objects...") );

        MergeConflictRes mcr(mainW);
        objdb->merge(ndb, &mcr);

        ndb->destroyChildren();
        delete ndb;

        objdb->setFileName(rcs->getFileName().toLocal8Bit().constData());
        objdb->resetTimeLastModified(oldtimestamp);
        objdb->setDirty(false);

        if (fwbdebug)
        {
            qDebug("* Merge is done");
            list<FWObject*> ll = db()->getByType(Library::TYPENAME);
            for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
            {
                qDebug("* Library %s %s in the data file",
                       FWObjectDatabase::getStringId((*i)->getId()).c_str(),
                       (*i)->getName().c_str() );
            }
        }


        /* this is a hack: 'Standard' library should be read-only. I
         * have too many files I already converted to the new API/DTD
         * and I am too lazy to convert them again, so I patch it up
         * here.
         *
         * However, if I am editing standard library, it should not be
         * read-only.
         */
        FWObject *slib = objdb->findInIndex(FWObjectDatabase::STANDARD_LIB_ID);
        if (slib!=nullptr )
        {
            if (fwbdebug)
                qDebug("standard library read-only status: %d, "
                       "editingStandardLib: %d",
                       slib->isReadOnly(), editingStandardLib);

            slib->setReadOnly(! editingStandardLib);
        }

        /* if the file name has an old extension .xml, change it to .fwb and
         * warn the user
         */
        QString fn = rcs->getFileName();
        QFileInfo ofinfo(fn);

        if ( ofinfo.suffix()=="xml")
        {
            if (fwbdebug)
            {
                qDebug("Need to rename file:  %s",
                       fn.toLatin1().constData());
                qDebug("             dirPath: %s",
                       ofinfo.dir().absolutePath().toLatin1().constData());
                qDebug("            filePath: %s",
                       ofinfo.absoluteFilePath().toLatin1().constData());
            }
            QString newFileName = ofinfo.dir().absolutePath()
                + "/" + ofinfo.completeBaseName() + ".fwb";
            
            bool needToRename = true;

            /* need these dances with symlinks to fix bug #1008956:
             * "Existing .fwb file gets overwritten if has wrong
             * extension"
             */
            QFileInfo nfinfo(newFileName);
            if (nfinfo.exists() && ofinfo.isSymLink() && ofinfo.readLink()==newFileName)
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
                     * tell the user we need to rename old file but
                     * the new file exists, then ask them to choose a
                     * new name. If the user chooses the same name and
                     * agrees to overwrite the file, just use this
                     * name. If the user hits cancel, tell them they
                     * need to choose a new name and open "file save"
                     * dialog again.
                     *
                     * Show the first dialog only once. If user hits
                     * Cancel, they see shorted version of the dialog
                     * and will be presented with "save file" dialog
                     * again.
                     */
                    QMessageBox::warning(
                        this,"Firewall Builder",
                        tr("Firewall Builder uses file extension '.fwb' and\n"
                           "needs to rename old data file '%1' to '%2',\n"
                           "but file '%3' already exists.\n"
                           "Choose a different name for the new file.")
                        .arg(fn).arg(newFileName).arg(newFileName),
                        tr("&Continue"), QString::null,QString::null,
                        0, 1 );

                    newFileName = chooseNewFileName(
                        fn, tr("Choose name and location for the new file"));
                    if (newFileName.isEmpty())
                    {
                        QString oldFileName = ofinfo.absoluteFilePath()
                            + ".bak";
                        rename(oldFileName.toLocal8Bit().constData(),
                               fn.toLocal8Bit().constData());

                        QMessageBox::warning(
                            this,"Firewall Builder",
                            tr("Load operation cancelled and data file reverted"
                               "to original version."),
                            tr("&Continue"), QString::null,QString::null,
                            0, 1 );

                        loadStandardObjects();
                        return false;
                    }
                    nfinfo.setFile(newFileName);
                }

                rename(fn.toLocal8Bit().constData(),
                       newFileName.toLocal8Bit().constData());

                QMessageBox::warning(
                this,"Firewall Builder",
                tr("Firewall Builder uses file extension '.fwb'. Your data"
                   "file '%1' \nhas been renamed '%2'")
                .arg(fn).arg(newFileName),
                tr("&Continue"), QString::null,QString::null,
                0, 1 );
            }

            fn = newFileName;
        }

        rcs->setFileName(fn);
        db()->setFileName(fn.toLocal8Bit().constData());

        //setWindowTitle(getPageTitle());
        //QCoreApplication::postEvent(mw, new updateSubWindowTitlesEvent());

        mainW->disableActions(m_panel->ruleSets->count()!=0);

        time_t last_modified = db()->getTimeLastModified();
        if (fwbdebug)
            qDebug("ProjectPanel::load(): load complete dirty=%d "
                   "last_modified=%s",
                   db()->isDirty(), ctime(&last_modified));
        
    } catch(FWException &ex)
    {
        QString trans = ex.getProperties()["failed_transformation"].c_str();
        QString elem  = ex.getProperties()["failed_element"].c_str();

        if(!trans.isEmpty() || !elem.isEmpty())
        {
            QString msg = tr("Exception: %1").arg(ex.toString().c_str());
            if (!trans.isEmpty())
            {
                trans.truncate(LONG_ERROR_CUTOFF);
                msg+="\n"+tr("Failed transformation : %1").arg(trans);
            }
            if (!elem.isEmpty())
            {
                elem.truncate(LONG_ERROR_CUTOFF);
                msg+="\n"+tr("XML element : %1").arg(elem);
            }
            QMessageBox::critical(
                this,"Firewall Builder",
                tr("The program encountered error trying to load data file.\n"
                   "The file has not been loaded. Error:\n%1").arg(msg),
                tr("&Continue"), QString::null,QString::null,
                0, 1 );
        } else
        {
            // this was not XML error, perhaps permissions or other
            // filesystem problem

            QString error_txt = QString::fromUtf8(ex.toString().c_str());
            if (error_txt.length() > LONG_ERROR_CUTOFF) 
            {
                error_txt.truncate(LONG_ERROR_CUTOFF);
                error_txt += "\n\n" + tr("(Long error message was truncated)");
            }

            QMessageBox::critical(
                this,"Firewall Builder",
                tr("The program encountered error trying to load data file.\n"
                   "The file has not been loaded. Error:\n%1").arg(
                       error_txt),
                tr("&Continue"), QString::null,QString::null,
                0, 1 );
        }
        // load standard objects so the window does not remain empty
        loadStandardObjects();
        return false;
    }

    db()->setReadOnly( rcs->isRO() || rcs->isTemp() );

// clear dirty flag for all objects, recursively
    if (!forceSave)  db()->setDirty(false);

    mw->showStatusBarMessage(tr("Building object tree..."));
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);

    loadObjects();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);

    mw->showStatusBarMessage(tr("Indexing...") );
    db()->reIndex();

    setupAutoSave();

    time_t last_modified = db()->getTimeLastModified();
    if (fwbdebug)
        qDebug("ProjectPanel::load(): all done: "
               "dirty=%d last_modified=%s",
               db()->isDirty(), ctime(&last_modified));

    return true;
}

bool ProjectPanel::checkin(bool unlock)
{
/* doing checkin only if we did checkout so rcs!=nullptr */
    QString rlog="";

    if (systemFile || rcs==nullptr || !rcs->isCheckedOut() || rcs->isTemp())
        return true;

    if (rcs->isDiff())
    {
        // if the file hasn't changed, do not need to ask for the comment
        if ( ! st->getRCSLogState())
        {
            RCSFileSaveDialog_q fsd;
            QDialog *fsd_dialog = new QDialog(this);
            fsd.setupUi(fsd_dialog);
            fsd.checkinDialogTitle->setText(
                QString("<b>") +
                tr("Checking file %1 in RCS").arg(rcs->getFileName()) +
                QString("</b>")
            );

            if ( fsd_dialog->exec()== QDialog::Rejected )
            {
                if (fwbdebug)
                    qDebug() << "RCSFileSaveDialog_q  user hit Cancel";

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
        QMessageBox::critical(
            this,"Firewall Builder",
            tr("Error checking in file %1:\n%2")
            .arg(rcs->getFileName()).arg(ex.toString().c_str()),
            tr("&Continue"), QString::null, QString::null,
            0, 1 );
    }
/***********************************************************************/
    return true;
}

void ProjectPanel::save()
{
    if (fwbdebug)
        qDebug("ProjectPanel::save:  rcs=%p  rcs->isRO=%d  "
               "rcs->isTemp=%d rcs->getFileName=%s",
               rcs,
               rcs->isRO(),
               rcs->isTemp(),
               rcs->getFileName().toLocal8Bit().constData());

    //undoStack->clear();

    if (!rcs->isRO() && !rcs->isTemp())
    {
        try
        {
            if (rcs->getFileName().isEmpty())
                fileSaveAs();  // eventually calls this method again
            else
            {
/* editingLibfile is true if user edits a library or master library file */

                mw->showStatusBarMessage(
                    tr("Saving data to file %1").arg(rcs->getFileName()));

                bool editingLibfile = editingLibrary();

/* ****************************************************************
 *
 *      REMOVE THIS
 *
 * or may be not. The savings of not storing standard objects in each file
 * are minimal but this code seems to be leaking too
 *
 ******************************************************************
 */
                //if (st->getDontSaveStdLib())  // this is now default
                if (false)
                {
                    list<FWObject*> userLibs;
                    list<FWObject*> ll = db()->getByType(Library::TYPENAME);
                    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
                    {
                        if (fwbdebug) qDebug("ProjectPanel::save()  lib %s",
                                             (*i)->getName().c_str() );

/* skip standard and template libraries unless we edit them */
                        int id = (*i)->getId();
                        if (id==FWObjectDatabase::STANDARD_LIB_ID && 
                            !editingStandardLib) continue;
                        if (id==FWObjectDatabase::TEMPLATE_LIB_ID &&
                            !editingTemplateLib) continue;

                        if (fwbdebug) qDebug("                   add");
                        userLibs.push_back( *i );
                    }

                    QApplication::setOverrideCursor(QCursor( Qt::WaitCursor));

                    FWObjectDatabase *ndb = db()->exportSubtree(userLibs);

                    if (editingLibfile)
                    {
/* exported libraries are always read-only */
                        list<FWObject*> ll = ndb->getByType(Library::TYPENAME);
                        for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
                        {
                            if ((*i)->getId()!=FWObjectDatabase::STANDARD_LIB_ID
                                &&
                                (*i)->getId()!=FWObjectDatabase::DELETED_OBJECTS_ID)
                                (*i)->setReadOnly( true );
                        }
                    }

                    ndb->resetTimeLastModified( db()->getTimeLastModified() );
                    xmlSetCompressMode(st->getCompression() ? 9 : 0);

                    ndb->saveFile(
                        rcs->getFileName().toLocal8Bit().constData());

                    delete ndb;

                    QApplication::restoreOverrideCursor();
                    // reset "dirty" flag only after we actually save the data
                    // fixes #389
                    db()->setDirty(false);
                    // and reset actions, including Save() which should now
                    // be inactive
                    QCoreApplication::postEvent(mw, new updateGUIStateEvent());

                    //mw->prepareFileMenu();
                } else
                {
                    QApplication::setOverrideCursor(QCursor( Qt::WaitCursor));
                    xmlSetCompressMode(st->getCompression() ? 9 : 0);
                    db()->saveFile(
                        rcs->getFileName().toLocal8Bit().constData());
                    QApplication::restoreOverrideCursor();
                }
            }
        }
        catch (FWException &ex)
        {
            QApplication::restoreOverrideCursor();

/* error saving the file. Since XMLTools does not return any useful
 * error message in the exception, let's check for obvious problems here
 */
            QString err;
            if (access(
                    rcs->getFileName().toLocal8Bit().constData(), W_OK)!=0 &&
                errno==EACCES)  err=tr("File is read-only");
            else                err=ex.toString().c_str();

            QMessageBox::critical(
                this,"Firewall Builder",
                tr("Error saving file %1: %2")
                .arg(rcs->getFileName()).arg(err),
                tr("&Continue"), QString::null, QString::null,
                0, 1 );
        }
    }
}


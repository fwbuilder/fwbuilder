/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: StartWizard.cpp,v 1.12 2005/01/03 01:43:50 vkurland Exp $

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

#include "../../config.h"
#include "global.h"

#include "StartWizard.h"
#include "RCSFileDialog.h"
#include "FWWindow.h"
#include "FWBSettings.h"
#include "VERSION.h"

#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qfile.h>

#include <errno.h>
#include <iostream>

using namespace std;
using namespace libfwbuilder;

StartWizard::~StartWizard()
{
    delete m_dialog;
}

StartWizard::StartWizard()
{
    m_dialog = new Ui::startWizard_q;
    m_dialog->setupUi(this);

    setControlWidgets(this, m_dialog->stackedWidget,
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);

    /*connect( m_dialog->nextButton, SIGNAL( clicked() ),
             this, SLOT( nextClicked() ));
    connect( m_dialog->backButton, SIGNAL( clicked() ),
             this, SLOT( backClicked() ));
    connect( m_dialog->finishButton, SIGNAL( clicked() ),
             this, SLOT( finishClicked() ));
    connect( m_dialog->cancelButton, SIGNAL( clicked() ),
             this, SLOT( cancelClicked() ));*/

    wantRCS=false;
    oldfile=false;
    newfile=false;

    cancelButton->hide();

    m_dialog->programName->setText( QString("<b>Firewall Builder %1</b>").arg(VERSION) );

    setNextEnabled(   0, false );
}

void StartWizard::openFile()
{
    RCSFileDialog   fd(this, 0, true);

    if ( fd.exec()== QDialog::Accepted )
    {
        RCS *rcs = fd.getSelectedRev();

        if (rcs==NULL) return;

        try
        {
            rcs->co();

        } catch (FWException &ex)
        {
/* if there was an exception, abort operation. */
            return;
        }
/***********************************************************************/

        mw->load( this, rcs );
        mw->showFirewalls();

        if (rcs->isTemp()) QFile(rcs->getFileName()).remove();

        setFinishEnabled( 0, true );
        oldfile=true;

        finishClicked();
    }
}

void StartWizard::newFile()
{
    fname=mw->chooseNewFileName(st->getWDir(),true,
                                tr("Choose name and location for the new file"));
    if (fname.isEmpty()) return;

    if (QFileInfo(fname).exists() && ! QFileInfo(fname).isWritable() )
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("File %1 is read-only, you can not save changes to it.")
            .arg(fname),
            "&Continue", QString::null, QString::null,
            0, 1 );
        return;
    }

    mw->load(this);

    mw->setFileName(fname);

// save blank data into the new file ("initialize" it)
    mw->save();
    mw->fileClose();

    setNextEnabled( 0, true );
    newfile=true;
}

void StartWizard::selected(const QString &title)
{
    int p = currentPage();

    if (p==1 && newfile && !fname.isEmpty())
    {
        m_dialog->fileLbl->setText( m_dialog->fileLbl->text().arg(fname) );
        setFinishEnabled( 1, true );
    }
}

void StartWizard::finishClicked()
{
    if (newfile && !fname.isEmpty())
    {
        if (m_dialog->autoopenBtn->isChecked())
        {
            st->setStartupAction(1);
            st->setLastEdited(fname);
        }

        RCS *rcs=new RCS(fname);
        if (m_dialog->rcsBtn->isChecked())
        {
            try
            {
                rcs->add();
            }
            catch (FWException &ex)
            {
                QMessageBox::warning(
                    this,"Firewall Builder",
                    tr("Error adding file to RCS:\n%1").arg(ex.toString().c_str()),
                    "&Continue", QString::null,QString::null,
                    0, 1 );
            }
        }
        try
        {
            rcs->co();
            mw->load( this, rcs );
        }
        catch (FWException &ex)
        {
            QMessageBox::warning(
                this,"Firewall Builder",
                tr("Error opening file:\n%1").arg(ex.toString().c_str()),
                "&Continue", QString::null,QString::null,
                0, 1 );
        }
    }
    QDialog::accept();
}

void StartWizard::cancelClicked()
{
    QDialog::reject();
}


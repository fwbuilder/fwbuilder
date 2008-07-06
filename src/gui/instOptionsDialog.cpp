/*

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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
#include "platforms.h"
#include "definitions.h"

#include "instOptionsDialog.h"
#include "instConf.h"

#include "fwbuilder/Firewall.h"

#include <qstring.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qframe.h>

#include <stdlib.h>

using namespace std;
using namespace libfwbuilder;

instOptionsDialog::instOptionsDialog(QWidget *parent, instConf *_cnf) :
    QDialog(parent)
{
    m_dialog = new Ui::instOptionsDialog_q;
    m_dialog->setupUi(this);
    cnf = _cnf;

    QString platform = cnf->fwobj->getStr("platform").c_str();

    m_dialog->pwd->setEchoMode( QLineEdit::Password );
    m_dialog->epwd->setEchoMode( QLineEdit::Password );

    QString fwname = QString::fromUtf8(cnf->fwobj->getName().c_str());
    if (!cnf->batchInstall)
        m_dialog->dialogTitleLine->setText(
            QString("<p align=\"center\"><b><font size=\"+2\">")+
            tr("Install options for firewall '%1'").arg(fwname)+
            QString("</font></b></p>")
        );

    m_dialog->uname->setFocus();
    m_dialog->uname->setText( cnf->user );
    m_dialog->incr->setChecked( cnf->incremental );
    m_dialog->test->setChecked( cnf->dry_run );
    m_dialog->backupConfigFile->setText( cnf->backup_file );
    m_dialog->saveDiff->setChecked( cnf->save_diff );
    m_dialog->saveStandby->setChecked( cnf->saveStandby );
    m_dialog->altAddress->setText( cnf->maddr );
    m_dialog->quiet->setChecked( cnf->quiet );
    m_dialog->verbose->setChecked( cnf->verbose );
    m_dialog->stripComments->setChecked( cnf->stripComments );
    m_dialog->compressScript->setChecked( cnf->compressScript );
    m_dialog->copyFWB->setChecked( cnf->copyFWB );
    m_dialog->testRun->setChecked( cnf->testRun );
    m_dialog->rollback->setChecked( cnf->rollback );
    m_dialog->rollbackTime->setValue( cnf->rollbackTime );
    m_dialog->cancelRollbackIfSuccess->setChecked( cnf->cancelRollbackIfSuccess );

    if (platform=="pix" || platform=="fwsm" || platform=="iosacl")
    {
        m_dialog->copyFWB->hide();

        // Hide elements of installOptions dialog for which we do not have commands
        QString cmd = cnf->getCmdFromResource("schedule_rollback");
        // option "schedule_rollback" is currently used to control rollback
        // behavior only for pix, fwsm and ios
        if (cmd.isEmpty())
        {
            m_dialog->rollback->hide();
            m_dialog->rollbackTime->hide();
            m_dialog->rollbackTimeUnit->hide();
            m_dialog->cancelRollbackIfSuccess->hide();
        }

        if (platform=="iosacl")
        {
            m_dialog->PIXgroupBox->hide();
/*
            incr->hide();
            test->hide();
            saveStandby->hide();
            backupConfigFile->hide();
            backupConfigFileLbl->hide();
*/
        }

        if (cnf->batchInstall)
        {
            m_dialog->backupConfigFile->hide();
            m_dialog->backupConfigFileLbl->hide();
        }

    } else
    {
        m_dialog->epwd->hide();
        m_dialog->PIXgroupBox->hide();
        // cancelling rollback at the end of activation is currently
        // only supported on pix,fwsm and ios
        m_dialog->cancelRollbackIfSuccess->hide();
    }



/* hide anyway, diff does not work for pix 6.3(3) */
    //dlg->hideOption( dlg->saveDiff );
    m_dialog->saveDiff->hide();

    //progressBar->hide();
    //dlg->hideOption( dlg->stripComments );
    m_dialog->stripComments->hide();
    //dlg->compressScript->hide();

    //if (platform=="pix" || platform=="fwsm")
    //{
    //    progressBar->show();
    //    stripComments->show();
    //}

    if (cnf->fwobj->getStr("host_OS")!="linksys")
    {
        m_dialog->compressScript->hide();
        //dlg->hideOption( dlg->compressScript );
    }

    m_dialog->PIXgroupBox->adjustSize();
    m_dialog->generalOptionsBox->adjustSize();
    m_dialog->testOptionsBox->adjustSize();
    m_dialog->mainBox->adjustSize();

    adjustSize();

    if (fwbdebug)
    {
        QSize sz = sizeHint();
        qDebug(QString("instOptionsDialog:  sizeHint: %1x%2").arg(sz.width()).arg(sz.height()).toAscii().constData());
        sz = minimumSizeHint();
        qDebug(QString("instOptionsDialog:  minimumSizeHint: %1x%2").
                arg(sz.width()).arg(sz.height()).toAscii().constData());

        QRect bfr;

        bfr = m_dialog->titleFrame->geometry();
        qDebug(QString("instOptionsDialog:  titleFrame: top=%1 bottom=%2").
                arg(bfr.top()).arg(bfr.bottom()).toAscii().constData());
        bfr = m_dialog->buttonsFrame->geometry();
        qDebug(QString("instOptionsDialog:  buttonsFrame: top=%1 bottom=%2").
                arg(bfr.top()).arg(bfr.bottom()).toAscii().constData());
    }

    //resize( minimumSizeHint() );

    //adjustSize();

    //dlg->setFixedHeight( dlg->minimumSizeHint().height() );
}

instOptionsDialog::~instOptionsDialog()
{
    delete m_dialog;
}


QString instOptionsDialog::getUName() { return m_dialog->uname->text(); }
QString instOptionsDialog::getPWD()   { return m_dialog->pwd->text();   }
QString instOptionsDialog::getEPWD()  { return m_dialog->epwd->text();  }


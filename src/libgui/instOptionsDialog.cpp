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

#include "definitions.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"

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
#include <QtDebug>

#include <stdlib.h>

#include "FWBSettings.h"
#include "FWWindow.h"

using namespace std;
using namespace libfwbuilder;

instOptionsDialog::instOptionsDialog(QWidget *parent, instConf *_cnf, bool installing_many_firewalls) :
    QDialog(parent)
{
    m_dialog = new Ui::instOptionsDialog_q;
    m_dialog->setupUi(this);
    cnf = _cnf;

    this->m_dialog->cancelAllButton->setVisible(installing_many_firewalls);

    int fw_id = -1;
    if (cnf->fwobj)
    {
        // Note cnf->fwobj == nullptr during batch install
        fw_id = cnf->fwobj->getId();
    }

    QString username = cnf->user;
    bool savePassEnabled = st->getBool("Environment/RememberSshPassEnabled");
    if (savePassEnabled && !username.isEmpty())
    {
        m_dialog->rememberPass->setEnabled(true);
        m_dialog->rememberPass->setChecked(true);
        QPair<QString, QString> passwds = mw->passwords[qMakePair(fw_id, username)];
        m_dialog->pwd->setText(passwds.first);
        m_dialog->epwd->setText(passwds.second);
    }
    else
    {
        m_dialog->rememberPass->setEnabled(false);
        m_dialog->rememberPass->setChecked(false);
    }

    m_dialog->pwd->setEchoMode(QLineEdit::Password);
    m_dialog->epwd->setEchoMode(QLineEdit::Password);

    m_dialog->uname->setText( cnf->user );
    m_dialog->test->setChecked( cnf->dry_run );
    m_dialog->backupConfigFile->setText( cnf->backup_file );
    m_dialog->saveDiff->setChecked( cnf->save_diff );
    m_dialog->saveStandby->setChecked( cnf->saveStandby );

    if (!cnf->putty_session.isEmpty())
        m_dialog->altAddress->setText( cnf->putty_session );
    else
        m_dialog->altAddress->setText( cnf->maddr );

    m_dialog->quiet->setChecked( cnf->quiet );
    m_dialog->verbose->setChecked( cnf->verbose );
    m_dialog->stripComments->setChecked( cnf->stripComments );
    m_dialog->compressScript->setChecked( cnf->compressScript );
    m_dialog->copyFWB->setChecked( cnf->copyFWB );

    // If we have user name, bring focus to the password input field
    // if we do not have user name, focus goes to the user name field
    if (cnf->user.isEmpty()) m_dialog->uname->setFocus();
    else m_dialog->pwd->setFocus();

    // "batch install" checkbox moved from instDialog to instOptionsDialog
    m_dialog->batchInstallText->setEnabled(installing_many_firewalls);
    m_dialog->batchInstall->setEnabled(installing_many_firewalls);
    m_dialog->batchInstall->setChecked(false);

    QString fwname = QString::fromUtf8(cnf->fwobj->getName().c_str());
    m_dialog->dialogTitleLine->setText(
        QString("<p align=\"center\"><b><font size=\"+2\">")+
        tr("Install options for firewall '%1'").arg(fwname)+
        QString("</font></b></p>")
    );

    QString platform = cnf->fwobj->getStr("platform").c_str();
    string version = cnf->fwobj->getStr("version");

    if (platform=="pix" || platform=="fwsm" ||
        platform=="iosacl" || platform=="nxosacl" ||
        platform=="procurve_acl" )
    {
        m_dialog->copyFWB->hide();
        m_dialog->PIXgroupBox->hide();

    } else if (platform=="junosacl") {
        m_dialog->copyFWB->hide();
        m_dialog->PIXgroupBox->hide();
        m_dialog->epwd->hide();
        m_dialog->epwdLbl->hide();
    } else
    {
        m_dialog->epwd->hide();
        m_dialog->epwdLbl->hide();
        m_dialog->PIXgroupBox->hide();
        m_dialog->test->hide();  // dry run option
    }

/* hide anyway, diff does not work for pix 6.3(3) */
    m_dialog->saveDiff->hide();

    m_dialog->stripComments->hide();

    m_dialog->compressScript->hide();

    m_dialog->PIXgroupBox->adjustSize();
    //m_dialog->generalOptionsBox->adjustSize();
    m_dialog->mainBox->adjustSize();

    adjustSize();

    if (fwbdebug)
    {
        QSize sz = sizeHint();
        qDebug() << QString("instOptionsDialog:  sizeHint: %1x%2")
            .arg(sz.width()).arg(sz.height());
        sz = minimumSizeHint();
        qDebug() << QString("instOptionsDialog:  minimumSizeHint: %1x%2")
            .arg(sz.width()).arg(sz.height());

        QRect bfr;

        bfr = m_dialog->titleFrame->geometry();
        qDebug() << QString("instOptionsDialog:  titleFrame: top=%1 bottom=%2")
            .arg(bfr.top()).arg(bfr.bottom());
        bfr = m_dialog->buttonsFrame->geometry();
        qDebug() << QString("instOptionsDialog:  buttonsFrame: top=%1 bottom=%2")
            .arg(bfr.top()).arg(bfr.bottom());
    }

    //resize( minimumSizeHint() );

    //adjustSize();

    //dlg->setFixedHeight( dlg->minimumSizeHint().height() );
}

void instOptionsDialog::savePassword()
{
    int fw_id = -1;
    if (cnf->fwobj)
    {
        // Note cnf->fwobj == nullptr during batch install
        fw_id = cnf->fwobj->getId();
    }

    if ( m_dialog->rememberPass->isChecked() )
        mw->passwords[qMakePair(fw_id, m_dialog->uname->text())] =
                 qMakePair(m_dialog->pwd->text(), m_dialog->epwd->text());
    else
        mw->passwords.remove(qMakePair(fw_id, m_dialog->uname->text()));

    st->setBool("Environment/RememberSshPass", m_dialog->rememberPass->isChecked());
}

instOptionsDialog::~instOptionsDialog()
{
    delete m_dialog;
}

QString instOptionsDialog::getUName() { return m_dialog->uname->text(); }
QString instOptionsDialog::getPWD()   { return m_dialog->pwd->text();   }
QString instOptionsDialog::getEPWD()  { return m_dialog->epwd->text();  }

void instOptionsDialog::cancelAll()
{
    this->done(-1);
}

void instOptionsDialog::batchInstallStateChange()
{
    if (m_dialog->batchInstall->isChecked())
    {
        m_dialog->altAddress->setText("");
        m_dialog->altAddressLabel->setEnabled(false);
        m_dialog->altAddress->setEnabled(false);
    } else
    {
        m_dialog->altAddressLabel->setEnabled(true);
        m_dialog->altAddress->setEnabled(true);
    }
}


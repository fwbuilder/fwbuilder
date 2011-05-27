/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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
#include "FWBSettings.h"

#include "IC_FirewallNamePage.h"
#include "ImportFirewallConfigurationWizard.h"

#include <QString>
#include <QFile>
#include <QRegExp>
#include <QTextStream>
#include <QtDebug>


IC_FirewallNamePage::IC_FirewallNamePage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::IC_FirewallNamePage_q;
    m_dialog->setupUi(this);

    m_dialog->deduplicateOnImport->setChecked(true);

    registerField("firewallName*", m_dialog->firewallName);
    registerField("deduplicate", m_dialog->deduplicateOnImport);
}

void IC_FirewallNamePage::initializePage()
{
    QString platform = 
        dynamic_cast<ImportFirewallConfigurationWizard*>(wizard())-> platform;

    QStringList *buf = 
        &(dynamic_cast<ImportFirewallConfigurationWizard*>(wizard())->buffer);

    qDebug() << "platform=" << platform;

    if (platform == "pix" || platform == "fwsm" || platform == "iosacl")
    {
        QRegExp cisco_re("^hostname\\s+(\\S+)");

        foreach(QString line, *buf)
        {
            if (cisco_re.indexIn(line) > -1)
            {
                QString name = cisco_re.cap(1).replace("\"", "").replace("'", "");
                m_dialog->firewallName->setText(name);
                break;
            }
        }
    }
    setCommitPage(true);
    emit completeChanged();
}


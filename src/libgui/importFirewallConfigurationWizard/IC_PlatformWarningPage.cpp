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

#include "IC_PlatformWarningPage.h"
#include "ImportFirewallConfigurationWizard.h"

#include "PreImport.h"

#include "platforms.h"

#include <QString>
#include <QFile>
#include <QRegExp>
#include <QTextStream>
#include <QDesktopServices>

#include <list>

using namespace std;


IC_PlatformWarningPage::IC_PlatformWarningPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::IC_PlatformWarningPage_q;
    m_dialog->setupUi(this);

    setField("platform", "");

    // user-chosen host os and version, so far we only show these for PF
    registerField("hostOS*", m_dialog->hostOS);
    registerField("version*", m_dialog->version);
    registerField("addStandardComments", m_dialog->addStandardComments);

    m_dialog->importOptionsFrame->hide();

    platformOk = false;
}

bool IC_PlatformWarningPage::isComplete() const
{
    if (!platformOk) return false;

    if (dynamic_cast<ImportFirewallConfigurationWizard*>(wizard())->
        platform == "pf")
    {
        QString host_os = m_dialog->hostOS->currentText();
        QString version = m_dialog->version->currentText();
        return (! host_os.isEmpty() && ! version.isEmpty());
    } else
        return true;
}

void IC_PlatformWarningPage::initializePage()
{
    QString fileName = field("fileName").toString();

    ImportFirewallConfigurationWizard* wz = dynamic_cast<ImportFirewallConfigurationWizard*>(wizard());

    QFile cf(fileName);
    if (cf.open(QIODevice::ReadOnly ))
    {
        m_dialog->configFileBrowser->clear();
        m_dialog->platform->setText(tr("Unknown"));

        QStringList *buf = &(wz->buffer);
        buf->clear();

        QTextStream stream(&cf);
        while (true)
        {
            QString line = stream.readLine().trimmed();
            if (line.isNull()) break;
            m_dialog->configFileBrowser->append(line);
            *buf << line;
        }

        QTextCursor cursor = m_dialog->configFileBrowser->textCursor();
        cursor.setPosition(0, QTextCursor::MoveAnchor);
        m_dialog->configFileBrowser->setTextCursor(cursor);
        m_dialog->configFileBrowser->ensureCursorVisible();


        bool iptables_c = false;
        Q_UNUSED(iptables_c);
        platformOk = false;

        PreImport pi(buf);
        pi.scan();

        switch (pi.getPlatform())
        {
        case PreImport::UNKNOWN:
            m_dialog->platform->setText(tr("Unknown"));
            m_dialog->platformSpecificWarning->setText(
                tr("Unrecognized configuration file format, can not import."));
            platformOk = false;
            break;

        case PreImport::PIX:
        case PreImport::FWSM:
            m_dialog->platform->setText(tr("Cisco PIX / CIsco ASA / Cisco FWSM"));
            m_dialog->platformSpecificWarning->setText(
                tr("Not all Cisco ASA and PIX configuration commands are "
                   "supported by Firewall Builder. "
                   "The following configuration components will be imported "
                   "and can be managed using "
                   "Firewall Builder:"
                   "<ul>"
                   "<li>Interface configurations (IP address, security level, "
                   "name, etc.)</li>"
                   "<li>Access lists</li>"
                   "<li>NAT configuration</li>"
                   "</ul>"
                   "The following configuration components will not be imported:"
                   "<ul>"
                   "<li>VPN</li>"
                   "<li>Static routes</li>"
                   "<li>Dynamic routing protocols</li>"
                   "<li>QoS</li>"
                   "</ul>"
                ));
            platformOk = true;
            break;


        case PreImport::IOSACL:
            m_dialog->platform->setText(tr("Cisco Router IOS"));
            m_dialog->platformSpecificWarning->setText(
                tr("Not all Cisco IOS configuration commands are "
                   "supported by Firewall Builder. The following "
                   "configuration components will be imported and "
                   "can be managed using Firewall Builder:"
                   "<ul>"
                   "<li>Interface configurations (IP address)</li>"
                   "<li>Extended access lists</li>"
                   "</ul>"
                   "The following configuration components will not be imported:"
                   "<ul>"
                   "<li>VPN</li>"
                   "<li>Static routes</li>"
                   "<li>Dynamic routing protocols (OSPF, RIP, etc.)</li>"
                   "<li>QoS</li>"
                   "</ul>"
                ));
            platformOk = true;
            break;

        case PreImport::IPTABLES:
            m_dialog->platform->setText(tr("iptables"));
            m_dialog->platformSpecificWarning->setText(
                tr("<html><p>Firewall Builder will import all the rules defined "
                   "in the iptables configuration. Discovered IP networks "
                   "and IP addresses used in the iptables rules will "
                   "automatically have objects created in the object tree. "
                   "Each user defined chain will be created as its own Policy "
                   "object in Firewall Builder."
                   "</p>"
                   "<p>"
                   "The import process will also attempt to detect interface "
                   "names and IP addresses based on -i and -o parameters in "
                   "the configuration, but you may have to update the firewall "
                   "object with additional interface information like IP addresses."
                   "</p></html>"
                ));
            platformOk = true;
            break;

        case PreImport::IPTABLES_WITH_COUNTERS:
            m_dialog->platform->setText(tr("iptables"));
            m_dialog->platformSpecificWarning->setText(
                tr("This appears to be iptables configuration saved using "
                   "command \"iptables-save -c\""
                   "and it includes packet counters. Please save configuration "
                   "using command \"iptables-save\" without option \"-c\" and "
                   "try to import it again."));
            platformOk = false;
            break;

        case PreImport::PF:
        {
            m_dialog->platform->setText(tr("pf"));
            m_dialog->platformSpecificWarning->setText(
                tr("<html><p>Firewall Builder supports import PF "
                   "configuration from a pf.conf file. Tables will be imported "
                   "as object groups and their names will be preserved. "
                   "Macros are expanded in place and not imported as "
                   "objects. Import of anchors is not supported at this time."
                   "</p>"
                   "<p>PF version in Firewall Builder corresponds to its "
                   "versions in OpenBSD. If you run FreeBSD 8.2 or earlier, "
                   "choose \"3.9\"."
                   "</p></html>"
                   ));

            platformOk = true;

            m_dialog->version->clear();
            m_dialog->hostOS->clear();
            wz->host_os_list.clear();
            wz->version_list.clear();

            // populate host OS items using standard function from platforms.cpp
            // but add an empty item on top and make it current
            setHostOS(m_dialog->hostOS, "pf", "");
            m_dialog->hostOS->insertItem(0, "");
            m_dialog->hostOS->setCurrentIndex(0);

            for (int i=0; i<m_dialog->hostOS->count(); ++i)
            {
                wz->host_os_list.append(m_dialog->hostOS->itemText(i));
            }

            // populate versions using standard function from platforms.cpp
            // and add empty item on top
            list<QStringPair> vl;
            getVersionsForPlatform("pf", vl);
            vl.push_front(QStringPair("", QObject::tr("")));

            for (list<QStringPair>::iterator i1=vl.begin(); i1!=vl.end(); i1++)
            {
                m_dialog->version->addItem( i1->second );
                wz->version_list.append(i1->first);
            }

            m_dialog->importOptionsFrame->show();
            break;
        }

        case PreImport::PF_REVERSE:
            m_dialog->platform->setText(tr("pf"));
            m_dialog->platformSpecificWarning->setText(
                tr(
                    "<html><p>This appears to be PF configuration designed "
                    "without use of the <b>\"quick\"</b> keyword, where "
                    "the packet is evaluated by all filtering rules in "
                    "sequential order and the last matching rule decides "
                    "what action is to be taken. Firewall Builder uses "
                    "different rule model, where the first matching rule "
                    "is always final and makes the decision on the action. "
                    "This means Firewall Builder can only import PF "
                    "configuration written using <b>\"quick\"</b> "
                    "keywords.</p></html>"
                   ));
            platformOk = false;
            break;
        }

        wz->platform = pi.getPlatformAsString();
    }

    emit completeChanged();
}


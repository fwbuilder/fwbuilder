/*
 * secuwallosAdvancedDialog.cpp - secuwall advanced host OS dialog implementation
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#include "global.h"
#include "platforms.h"

#include <memory>

#include "secuwallosAdvancedDialog.h"
#include "FWCmdChange.h"

#include "fwbuilder/Firewall.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qstackedwidget.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qdesktopservices.h>
#include <qurl.h>

#include "FWWindow.h"
#include "Help.h"

using namespace std;
using namespace libfwbuilder;

secuwallosAdvancedDialog::~secuwallosAdvancedDialog()
{
    delete m_dialog;
}

secuwallosAdvancedDialog::secuwallosAdvancedDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::secuwallosAdvancedDialog_q;
    m_dialog->setupUi(this);
    setWindowModality(Qt::WindowModal);
    obj = o;

    FWOptions *fwopt = (Firewall::cast(obj))->getOptionsObject();
    assert(fwopt != nullptr);

    // mappings from value to QComboBox index
    QStringList threeStateMapping;
    QStringList resoStateMapping;

    threeStateMapping.push_back(QObject::tr("No change"));
    threeStateMapping.push_back("");

    threeStateMapping.push_back(QObject::tr("On"));
    threeStateMapping.push_back("1");

    threeStateMapping.push_back(QObject::tr("Off"));
    threeStateMapping.push_back("0");

    resoStateMapping.push_back("");
    resoStateMapping.push_back("none");

    resoStateMapping.push_back("Hosts");
    resoStateMapping.push_back("files");

    resoStateMapping.push_back("DNS");
    resoStateMapping.push_back("dns");

    resoStateMapping.push_back("NIS");
    resoStateMapping.push_back("nis");

    resoStateMapping.push_back("NIS+");
    resoStateMapping.push_back("nisplus");

    resoStateMapping.push_back("DB");
    resoStateMapping.push_back("db");

    // management settings
    data.registerOption(m_dialog->secuwall_mgmt_mgmtaddr,
                        fwopt,
                        "secuwall_mgmt_mgmtaddr");
    data.registerOption(m_dialog->secuwall_mgmt_loggingaddr,
                        fwopt,
                        "secuwall_mgmt_loggingaddr");
    data.registerOption(m_dialog->secuwall_mgmt_snmpaddr,
                        fwopt,
                        "secuwall_mgmt_snmpaddr");
    data.registerOption(m_dialog->secuwall_mgmt_rosnmp,
                        fwopt,
                        "secuwall_mgmt_rosnmp");
    data.registerOption(m_dialog->secuwall_mgmt_ntpaddr,
                        fwopt,
                        "secuwall_mgmt_ntpaddr");
    data.registerOption(m_dialog->secuwall_mgmt_nagiosaddr,
                        fwopt,
                        "secuwall_mgmt_nagiosaddr");
    data.registerOption(m_dialog->secuwall_mgmt_varpart,
                        fwopt,
                        "secuwall_mgmt_varpart");
    data.registerOption(m_dialog->secuwall_mgmt_confpart,
                        fwopt,
                        "secuwall_mgmt_confpart");
    data.registerOption(m_dialog->secuwall_mgmt_rules_disable,
                        fwopt,
                        "secuwall_mgmt_rules_disable");

    // dns settings
    data.registerOption(m_dialog->secuwall_dns_srv1,
                        fwopt,
                        "secuwall_dns_srv1");
    data.registerOption(m_dialog->secuwall_dns_srv2,
                        fwopt,
                        "secuwall_dns_srv2");
    data.registerOption(m_dialog->secuwall_dns_srv3,
                        fwopt,
                        "secuwall_dns_srv3");
    data.registerOption(m_dialog->secuwall_dns_domains,
                        fwopt,
                        "secuwall_dns_domains");
    data.registerOption(m_dialog->secuwall_dns_reso1,
                        fwopt,
                        "secuwall_dns_reso1", resoStateMapping);
    data.registerOption(m_dialog->secuwall_dns_reso2,
                        fwopt,
                        "secuwall_dns_reso2", resoStateMapping);
    data.registerOption(m_dialog->secuwall_dns_reso3,
                        fwopt,
                        "secuwall_dns_reso3", resoStateMapping);
    data.registerOption(m_dialog->secuwall_dns_reso4,
                        fwopt,
                        "secuwall_dns_reso4", resoStateMapping);
    data.registerOption(m_dialog->secuwall_dns_reso5,
                        fwopt,
                        "secuwall_dns_reso5", resoStateMapping);

    // hosts settings
    data.registerOption(m_dialog->secuwall_dns_hosts,
                        fwopt,
                        "secuwall_dns_hosts", resoStateMapping);

    // iptables / routing and TCP
    data.registerOption(m_dialog->linux24_log_martians,
                        fwopt,
                        "linux24_log_martians", threeStateMapping);
    data.registerOption(m_dialog->linux24_accept_redirects,
                        fwopt,
                        "linux24_accept_redirects", threeStateMapping);
    data.registerOption(m_dialog->linux24_icmp_echo_ignore_all,
                        fwopt,
                        "linux24_icmp_echo_ignore_all", threeStateMapping);
    data.registerOption(m_dialog->linux24_icmp_echo_ignore_broadcasts,
                        fwopt,
                        "linux24_icmp_echo_ignore_broadcasts",
                        threeStateMapping);
    data.registerOption(m_dialog->linux24_icmp_ignore_bogus_error_responses,
                        fwopt,
                        "linux24_icmp_ignore_bogus_error_responses",
                        threeStateMapping);
    data.registerOption(m_dialog->linux24_ip_dynaddr,
                        fwopt,
                        "linux24_ip_dynaddr", threeStateMapping);
    data.registerOption(m_dialog->linux24_rp_filter,
                        fwopt,
                        "linux24_rp_filter", threeStateMapping);
    data.registerOption(m_dialog->linux24_accept_source_route,
                        fwopt,
                        "linux24_accept_source_route", threeStateMapping);
    data.registerOption(m_dialog->linux24_ip_forward,
                        fwopt,
                        "linux24_ip_forward", threeStateMapping);
    data.registerOption(m_dialog->linux24_ipv6_forward,
                        fwopt,
                        "linux24_ipv6_forward", threeStateMapping);
    data.registerOption(m_dialog->linux24_tcp_fin_timeout,
                        fwopt,
                        "linux24_tcp_fin_timeout");
    data.registerOption(m_dialog->linux24_tcp_keepalive_interval,
                        fwopt,
                        "linux24_tcp_keepalive_interval");
    data.registerOption(m_dialog->linux24_tcp_window_scaling,
                        fwopt,
                        "linux24_tcp_window_scaling", threeStateMapping);
    data.registerOption(m_dialog->linux24_tcp_sack,
                        fwopt,
                        "linux24_tcp_sack", threeStateMapping);
    data.registerOption(m_dialog->linux24_tcp_fack,
                        fwopt,
                        "linux24_tcp_fack", threeStateMapping);
    data.registerOption(m_dialog->linux24_tcp_ecn,
                        fwopt,
                        "linux24_tcp_ecn", threeStateMapping);
    data.registerOption(m_dialog->linux24_tcp_syncookies,
                        fwopt,
                        "linux24_tcp_syncookies", threeStateMapping);
    data.registerOption(m_dialog->linux24_tcp_timestamps,
                        fwopt,
                        "linux24_tcp_timestamps", threeStateMapping);

    // additional files
    data.registerOption(m_dialog->additional_files_enabled,
                        fwopt,
                        "secuwall_add_files");
    data.registerOption(m_dialog->additional_files_dir,
                        fwopt,
                        "secuwall_add_files_dir");
    data.loadAll();

    m_dialog->tabWidget->setCurrentIndex(0);
}

/*
 * store all data in the object
 */
void secuwallosAdvancedDialog::accept()
{
    // validate user input before saving
    if (!validate()) return;

    ProjectPanel *project = mw->activeProject();
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(project, obj));

    // new_state  is a copy of the fw object
    FWObject* new_state = cmd->getNewState();
    FWOptions* fwoptions = Firewall::cast(new_state)->getOptionsObject();
    assert(fwoptions!=nullptr);

    data.saveAll(fwoptions);

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());

    QDialog::accept();
}

void secuwallosAdvancedDialog::reject()
{
    QDialog::reject();
}

void secuwallosAdvancedDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
                            m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("Host secunet wall");
    h->setSource(QUrl("secuwallosAdvancedDialog.html#" + anchor));
    h->raise();
    h->show();
}

void secuwallosAdvancedDialog::additionalChanged(int state)
{
    if (state == Qt::Checked)
    {
        m_dialog->templdir_label->setEnabled(true);
        m_dialog->additional_files_dir->setEnabled(true);
        m_dialog->buttonBrowse->setEnabled(true);
        m_dialog->buttonOpenURL->setEnabled(true);
    }
    else
    {
        m_dialog->templdir_label->setEnabled(false);
        m_dialog->additional_files_dir->setEnabled(false);
        m_dialog->buttonBrowse->setEnabled(false);
        m_dialog->buttonOpenURL->setEnabled(false);
    }
}

void secuwallosAdvancedDialog::buttonBrowseClicked()
{
    QString templ_dir = QFileDialog::getExistingDirectory
        (this,
         tr("Select templates directory"),
         m_dialog->additional_files_dir->text(),
         QFileDialog::DontResolveSymlinks);

    if (templ_dir == "")
    {
        return ;
    }
    m_dialog->additional_files_dir->setText(templ_dir);
}

void secuwallosAdvancedDialog::buttonOpenURLClicked()
{
    bool ok = true;
    QString message;
    QString path = m_dialog->additional_files_dir->text();
    QUrl url("file://" + path);

    if (!url.isValid())
    {
        ok = false;
        message = tr("URL is not valid: %1").arg(path);
    }
    else
    {
        // note: "Warning: unknown mime-type" errors can not be detected here
        if (!QDesktopServices::openUrl(url))
        {
            ok = false;
            message = tr("Could not open URL: %1").arg(url.toLocalFile());
        }
    }

    if (!ok)
    {
        QMessageBox::warning
            (this, "Firewall Builder",
             message, "&Continue", QString::null, QString::null, 0, 1);
    }
}

bool secuwallosAdvancedDialog::validate()
{
    bool valid = true;
    QWidget *focus = nullptr;
    QString message;

    // widgets to verify
    struct _tocheck {
        QLineEdit* widget;
        bool (*chkfn)(const QString &);
    } widgets[5] =
    {
      {
          m_dialog->secuwall_mgmt_mgmtaddr,
          &secuwallosAdvancedDialog::validateNetworkOrAddress
      },
      {
          m_dialog->secuwall_mgmt_loggingaddr,
          &secuwallosAdvancedDialog::validateAddress
      },
      {
          m_dialog->secuwall_mgmt_snmpaddr,
          &secuwallosAdvancedDialog::validateNetworkOrAddress
      },
      {
          m_dialog->secuwall_mgmt_ntpaddr,
          &secuwallosAdvancedDialog::validateAddress
      },
      {
          m_dialog->secuwall_mgmt_nagiosaddr,
          &secuwallosAdvancedDialog::validateNetworkOrAddress
      },
    };

    int size = sizeof(widgets) / sizeof(struct _tocheck);

    // reset widget colors first
    for (int i = 0; i < size; i++)
    {
        if (widgets[i].widget->palette() != QApplication::palette())
        {
            widgets[i].widget->setPalette(QApplication::palette());
        }
    }

    // validate each widget one by one
    for (int i = 0; i < size && valid; i++)
    {
        // get text to verify
        QString to_verify = widgets[i].widget->text();
        // focus current widget
        focus = widgets[i].widget;
        // if empty, continue
        if (to_verify.isEmpty())
        {
            continue;
        }

        // check comma sep. list of addresses
        QStringList addrlist = to_verify.split(",");
        int pos = 1;
        foreach(QString addr, addrlist)
        {
            if (addr.isEmpty())
            {
                valid = false;
                message = tr("Empty address found (position %1)").arg(pos);
                break;
            }
            addr = addr.simplified();
            if (!widgets[i].chkfn(addr))
            {
                valid = false;
                message = tr("Illegal address '%1' (position %2)").
                          arg(addr).arg(pos);
                break;
            }
            pos++;
        }
    }

    if (!valid)
    {
        // highlight error: focus and set different background color
        focus->setFocus();
        m_dialog->tabWidget->setCurrentIndex(0);
        QPalette palette;
        palette.setColor(focus->backgroundRole(), QColor(255, 0, 0, 100));
        focus->setPalette(palette);
        // display errror message
        QMessageBox::warning(this, "Firewall Builder",
                             tr("Input not valid: %1").arg(message), "&Continue",
                             QString::null, QString::null, 0, 1);
    }
    return valid;
}

bool secuwallosAdvancedDialog::validateAddress(const QString &addr)
{
    if (addr.indexOf("/") != -1)
    {
        return false;
    }

    try
    {
        InetAddr(addr.toLatin1().constData());
    } catch (FWException &ex)
    {
        return false;
    }
    return true;
}

bool secuwallosAdvancedDialog::validateNetwork(const QString &addr)
{
    if (addr.indexOf("/") == -1)
    {
        return false;
    }

    // validate IP/netmask address pairs
    QStringList addrpair = addr.split("/");

    try
    {
        InetAddr(addrpair.at(0).toLatin1().constData());
    } catch (FWException &ex)
    {
        return false;
    }

    try
    {
        InetAddr(addrpair.at(1).toLatin1().constData());
    } catch (FWException &ex)
    {
        // not in dotted notation?
        bool ok = false;
        int ilen = addrpair[1].toInt(&ok);
        if (ok)
        {
            if (ilen < 0 || ilen > 32)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool secuwallosAdvancedDialog::validateNetworkOrAddress(const QString &addr)
{
    return (validateNetwork(addr) || validateAddress(addr));
}

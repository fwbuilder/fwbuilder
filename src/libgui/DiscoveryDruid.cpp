/*

                          Firewall Builder

                 Copyright (C) 2005, 2006 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org
           Illiya Yalovoy    <yalovoy@gmail.com>

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
#include "events.h"

#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qlabel.h>
#include <qtextbrowser.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qprogressdialog.h>
#include <qlistview.h>
#include <qtextstream.h>
#include <qiodevice.h>
#include <qhostaddress.h>
#include <qhostinfo.h>
#include <qgroupbox.h>
#include <qmessagebox.h>
#include <QtDebug>

#include "DiscoveryDruid.h"
#include "ProjectPanel.h"

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <memory>

#include "fwbuilder/HostsFile.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/InetAddrMask.h"
#include "fwbuilder/Inet6AddrMask.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Policy.h"

#include "fwbuilder/dns.h"
#include "fwbuilder/snmp.h"
#include "fwbuilder/Logger.h"

#include "FWBSettings.h"
#include "ObjectManipulator.h"
#include "FWWindow.h"
#include "networkZoneManager.h"

#include "IOSImporter.h"
#include "IPTImporter.h"
#include "PIXImporter.h"


using namespace std;
using namespace libfwbuilder;

DiscoveryDruid::DiscoveryDruid(QWidget *parent, bool start_with_import) :
    QDialog(parent)
{
    init = true;

    discovered_fw = nullptr;

    m_dialog = new Ui::DiscoveryDruid_q;
    m_dialog->setupUi(this);

    setControlWidgets(this, m_dialog->stackedWidget,
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);

    QTextCursor cursor(m_dialog->discoverylog->textCursor());
    normal_format = cursor.charFormat();

    error_format = normal_format;
    error_format.setForeground(QBrush(Qt::red));
    error_format.setAnchorHref("http://somewhere.com");
    error_format.setAnchor(true);
    // weight must be between 0 and 99. Qt 4.4.1 does not seem to mind if
    // it is >99 (just caps it) but older versions assert
    error_format.setProperty(QTextFormat::FontWeight, 99);

    warning_format = normal_format;
    warning_format.setForeground(QBrush(Qt::blue));
    warning_format.setProperty(QTextFormat::FontWeight, 99);
    warning_format.setAnchor(true);
    warning_format.setAnchorHref("http://somewhere.com");

    dm_method = new QButtonGroup;
    dm_method->addButton(m_dialog->dm_fromfile,0);
    dm_method->addButton(m_dialog->dm_importdns,1);
    dm_method->addButton(m_dialog->dm_usesnmp,2);
    dm_method->addButton(m_dialog->dm_import_config,3);

    connect(dm_method, SIGNAL( buttonClicked(int) ),
            this, SLOT( changedDiscoveryMethod(int) ) );
    connect(m_dialog->dnsfromlist, SIGNAL( clicked(bool) ),
            this, SLOT( changedNameServer() ) );
    connect(m_dialog->dnscustom, SIGNAL( clicked(bool) ),
            this, SLOT( changedNameServer() ) );
    connect(m_dialog->nameserverlist, SIGNAL( editTextChanged(QString) ),
            this, SLOT( changedNameServer() ) );
    connect(m_dialog->nameserverline, SIGNAL( textChanged(QString) ),
            this, SLOT( changedNameServer() ) );

    thread = nullptr;

    timer = new QTimer(this);
    prg_timer = new QTimer(this);
    unBar = nullptr;
    unProg = 0;

    connect(prg_timer,SIGNAL(timeout()),this,SLOT(updatePrg()));

    setDiscoveryMethod_file();

    flt_obj     = new Filter();
    flt_obj_d   = new FilterDialog(this);
    flt_obj_d->setFilter(flt_obj);

    flt_last    = new Filter();
    flt_last_d  = new FilterDialog(this);
    flt_last_d->setFilter(flt_last);

    flt_net     = new Filter();
    flt_net_d   = new FilterDialog(this);
    flt_net_d->setFilter(flt_net);

    assert(mw->activeProject()->db());

    fillLibraries(m_dialog->libs, mw->activeProject()->db());
    m_dialog->libs->setEditable(true);
    m_dialog->libs->lineEdit()->setText(mw->getCurrentLib()->getName().c_str());

    m_dialog->DNSprogress->hide();
    m_dialog->DNSprogress_2->hide();

    m_dialog->dm_importdns->hide();
    m_dialog->snmpdnsparameters->hide();

#ifndef HAVE_LIBSNMP
    m_dialog->dm_usesnmp->setEnabled(false);
#endif

    restore();

    importPlatformChanged(m_dialog->import_platform->currentIndex());

    showPage(CHOOSE_METHOD_PAGE);
    setNextEnabled(CHOOSE_METHOD_PAGE, true);

    if (start_with_import)
    {
        m_dialog->dm_import_config->setDown(true);
        setDiscoveryMethod_Import();
        setAppropriate( CHOOSE_METHOD_PAGE, false );
        // show the first page of the "import policy" track of the wizard
        showPage( IMPORT_CONFIG_PAGE );
        setNextEnabled(IMPORT_CONFIG_PAGE, false);
        cancelButton->show();
    }

    prg_timer->start(100);

    init = false;
}

void DiscoveryDruid::nextClicked()
{
    if (nextRelevant( currentPage() ) > -1)
        showPage(nextRelevant( currentPage() ));
}

void DiscoveryDruid::backClicked()
{
    if (previousRelevant( currentPage() ) > -1)
        showPage(previousRelevant( currentPage() ));
}

void DiscoveryDruid::finishClicked()
{
    if (current_task == BT_IMPORT && discovered_fw != nullptr)
    {
        if (selectedPlatform() == "pix" && currentPage() == NETWORK_ZONES_PAGE)
        {
            // read and configure network zones
            list<FWObject*> all_interfaces =
                discovered_fw->getByTypeDeep(Interface::TYPENAME);
            list<FWObject*>::iterator it;
            for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
            {
                Interface *iface = Interface::cast(*it);

                string  network_zone_str_id = "";

                QList<QTableWidgetItem*> ltwi =
                    m_dialog->iface_nz_list->findItems( iface->getName().c_str(),
                                                        Qt::MatchExactly );
                if ( ! ltwi.empty())
                {
                    QTableWidgetItem *itm2 = ltwi[0];
                    assert(itm2!=nullptr);
                    int row = itm2->row();
                    QComboBox *cb = dynamic_cast<QComboBox*>(
                        m_dialog->iface_nz_list->cellWidget(row, 3));
                    assert(cb!=nullptr);
                    int network_zone_int_id =
                        cb->itemData(cb->currentIndex(), Qt::UserRole).toInt();
                    if (network_zone_int_id != 0)
                        network_zone_str_id = FWObjectDatabase::getStringId(
                            network_zone_int_id);
                    else
                        network_zone_str_id = "";
                }

                // only set network zone if it is supported and is not
                // empty. See #2014
                if (!network_zone_str_id.empty())
                    iface->setStr("network_zone", network_zone_str_id);

            }
        }

        QCoreApplication::postEvent(
            mw->activeProject(), new expandObjectInTreeEvent(
                mw->activeProject()->getFileName(), discovered_fw->getId()));

        QCoreApplication::postEvent(
            mw->activeProject(), new showObjectInTreeEvent(
                mw->activeProject()->getFileName(), discovered_fw->getId()));

        QCoreApplication::postEvent(
            mw, new openObjectInEditorEvent(
                mw->activeProject()->getFileName(), discovered_fw->getId()));

    }

    QDialog::accept();
}

void DiscoveryDruid::cancelClicked()
{
    QDialog::reject();
}

DiscoveryDruid::~DiscoveryDruid()
{
    save();

    delete flt_obj;
    delete flt_last;
    delete flt_net;
    delete flt_obj_d;
    delete flt_last_d;
    delete flt_net_d;

    delete m_dialog;
    delete dm_method;
}

const char * DISCOVERY_DRUID_PREFIX="DiscoveryDruid/";

const char * DISCOVERY_DRUID_DISCOVERYMETHOD="DiscoveryMethod";
const char * DISCOVERY_DRUID_FILENAME       ="Filename";
const char * DISCOVERY_DRUID_DOMAINNAME     ="Domainname";
const char * DISCOVERY_DRUID_USELONGNAME    ="UseLongName";
const char * DISCOVERY_DRUID_NAMESERVER     ="NameServer";
const char * DISCOVERY_DRUID_DNSTIMEOUT     ="DNSTimeout";
const char * DISCOVERY_DRUID_DNSRETRIES     ="DNSRetries";
const char * DISCOVERY_DRUID_SEEDHOST       ="SeedHost";
const char * DISCOVERY_DRUID_SNMPINADDR     ="SNMPInAddr";
const char * DISCOVERY_DRUID_SNMPINMASK     ="SNMPInMask";
const char * DISCOVERY_DRUID_SNMPRECURSIVE  ="SNMPRecursive";
const char * DISCOVERY_DRUID_SNMPFOLLOWP2P  ="SNMPFollowP2P";
const char * DISCOVERY_DRUID_SNMPINCLUDEUNNUMBERED="SnmpIncludeUnnumbered";
const char * DISCOVERY_DRUID_SNMPDODNS      ="SNMPDoDNS";
const char * DISCOVERY_DRUID_SNMPCOMMUNITY  ="SNMPCommunity";
const char * DISCOVERY_DRUID_SNMPRETRIES    ="SNMPRetries";
const char * DISCOVERY_DRUID_SNMPTIMEOUT    ="SNMPTimeout";
const char * DISCOVERY_DRUID_SNMPDNSRETRIES ="DNSRetries";
const char * DISCOVERY_DRUID_SNMPDNSTIMEOUT ="DNSTimeout";
const char * DISCOVERY_DRUID_SNMPDNSTHREADS ="SNMPDnsThreads";
const char * DISCOVERY_DRUID_IMPORRT_CONFIG_PLATFORM = "ImportPlatform";


void DiscoveryDruid::restore()
{
    int i;
    QString s;
    //Restore from settings
    dm_method->button(st->getInt(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_DISCOVERYMETHOD))->setChecked(true);
    changedDiscoveryMethod(dm_method->checkedId());

    //m_dialog->filename->setText(st->getStr(
    //            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_FILENAME));
    //m_dialog->domainname->setText(st->getStr(
    //            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_DOMAINNAME));
    m_dialog->uselongname->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_USELONGNAME));
    //m_dialog->nameserverline->setText(st->getStr(
    //            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_NAMESERVER));
    i=st->getInt(QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_DNSTIMEOUT);
    m_dialog->dnstimeout->setValue((i)?i:2);
    i=st->getInt(QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_DNSRETRIES);
    m_dialog->dnsretries->setValue((i)?i:1);

    m_dialog->seedhostname->setText(st->getStr(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SEEDHOST));
    m_dialog->snmpinaddr->setText(st->getStr(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINADDR));
    m_dialog->snmpinmask->setText(st->getStr(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINMASK));
    m_dialog->snmprecursive->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPRECURSIVE));
    m_dialog->snmpfollowp2p->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPFOLLOWP2P));
    m_dialog->snmpincludeunnumbered->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINCLUDEUNNUMBERED));
    m_dialog->snmpdodns->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDODNS));
    s=st->getStr(QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPCOMMUNITY);
    m_dialog->snmpcommunity->setText((s.isEmpty())?"public":s);
    i=st->getInt(QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPRETRIES);
    m_dialog->snmpretries->setValue((i)?i:1);
    i=st->getInt(QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPTIMEOUT);
    m_dialog->snmptimeout->setValue((i)?i:2);
    i=st->getInt(QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDNSRETRIES);
    m_dialog->snmpdnsretries->setValue((i)?i:1);
    i=st->getInt(QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDNSTIMEOUT);
    m_dialog->snmpdnstimeout->setValue((i)?i:2);
    i=st->getInt(QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDNSTHREADS);
    m_dialog->snmpdnsthreads->setValue((i)?i:5);

    m_dialog->import_platform->setCurrentIndex(st->getInt(
           QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_IMPORRT_CONFIG_PLATFORM));
}

void DiscoveryDruid::save()
{
    // Save to settings
    st->setInt(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_DISCOVERYMETHOD,
            dm_method->checkedId());
    st->setBool(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_USELONGNAME,
            m_dialog->uselongname->isChecked());
    if (current_task==BT_DNS)
    {
        st->setInt(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_DNSTIMEOUT,
                m_dialog->dnstimeout->value());
        st->setInt(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_DNSRETRIES,
                m_dialog->dnsretries->value());
    }
    else
    {
        st->setInt(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDNSRETRIES,
                m_dialog->snmpdnsretries->value());
        st->setInt(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDNSTIMEOUT,
                m_dialog->snmpdnstimeout->value());
    }
    st->setStr(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SEEDHOST,
            m_dialog->seedhostname->text());
    st->setStr(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINADDR,
            m_dialog->snmpinaddr->text());
    st->setStr(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINMASK,
            m_dialog->snmpinmask->text());
    st->setBool(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPRECURSIVE,
            m_dialog->snmprecursive->isChecked());
    st->setBool(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPFOLLOWP2P,
            m_dialog->snmpfollowp2p->isChecked());
    st->setBool(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINCLUDEUNNUMBERED,
            m_dialog->snmpincludeunnumbered->isChecked());
    st->setBool(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDODNS,
            m_dialog->snmpdodns->isChecked());
    st->setStr(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPCOMMUNITY,
            m_dialog->snmpcommunity->text());
    st->setInt(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPRETRIES,
            m_dialog->snmpretries->value());
    st->setInt(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPTIMEOUT,
            m_dialog->snmptimeout->value());
    st->setInt(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDNSTHREADS,
            m_dialog->snmpdnsthreads->value());
    st->setInt(
        QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_IMPORRT_CONFIG_PLATFORM,
        m_dialog->import_platform->currentIndex());

}

void DiscoveryDruid::dnsFinish(QHostInfo host)
{
    QList<QHostAddress> list = host.addresses();

    unBar->hide();

    if (userIsTyping)
    {
        //abandon the test result
        if (current_task==BT_DNS)
        {
            changedNameServer();
        }
        else
        {
            changedSeedHost();
        }
    }
    else
    {
        //get the test result
        if (list.isEmpty())
        {
            QPalette palette = errMessage->palette();
            palette.setColor(errMessage->foregroundRole(), Qt::darkRed);
            errMessage->setPalette(palette);

            errMessage->setText( "host name not found");
            isSeedHostOK=false;
        }
        else
        {
            QPalette palette = errMessage->palette();
            palette.setColor(errMessage->foregroundRole(), Qt::darkGreen);
            errMessage->setPalette(palette);

            errMessage->setText( "host name verified");
            isSeedHostOK=true;

        }
        nextButton->setEnabled(isSNMPInclNetOK && isSeedHostOK);
    }

}

void DiscoveryDruid::changedSelected( const int &page )
{
    if (init) return;

    switch (page)
    {

    case READ_HOSTS_FILE_PAGE: // Reading file in hosts format
    {
        setNextEnabled(page, false);
        changedHostsFileName();
        m_dialog->filename->setFocus();
        break;
    }

    case IMPORT_CONFIG_PAGE: // import config
    {
        m_dialog->obj_name->setFocus();
        setBackEnabled(page, true);
        setFinishEnabled(page, false);
        break;
    }

    case IMPORT_DNS_ZONE_PAGE: // Import DNS zone
    {
        changedDomainName();
        m_dialog->domainname->setFocus();
        //setNextEnabled(page,false);
        break;
    }

    case NAME_SERVER_PAGE: // Name server
    {
        if (page>FromPage)
            getNameServers();
        disconnect(timer,SIGNAL(timeout()),0,0);
        connect(timer,SIGNAL(timeout()),this,SLOT(checkHostName()));
        changedNameServer();
        m_dialog->nameserverline->setFocus();

        //setNextEnabled(page,false);
        break;
    }

    case SNMP_DISCOVERY_PAGE: // Network discovery using SNMP
    {
        disconnect(timer,SIGNAL(timeout()),0,0);
        connect(timer,SIGNAL(timeout()),this,SLOT(checkHostName()));

        isSeedHostOK=false;
        isSNMPInclNetOK=false;

        changedSeedHost();
        changedInclNet();
        m_dialog->seedhostname->setFocus();
        break;
    }

    case NETWORK_SCAN_OPTIONS_PAGE: // Network scan options
    {
        m_dialog->snmprecursive->setFocus();
        //setNextEnabled(page,false);
        break;
    }

    case SNMP_PARAMETERS_PAGE: // SNMP and DNS reverse lookup queries parameters
    {
        checkSNMPCommunity();
        m_dialog->snmpcommunity->setFocus();
        break;
    }

    case BACKGROUND_PROCESS_PAGE: // Background process (import from hosts and from config file)
    {
        m_dialog->discoveryprogress->setValue(-1);
        m_dialog->discoverylog->clear();
        m_dialog->discoveryStopButton->setEnabled(true);
        m_dialog->logSaveButton->setEnabled(false);

        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);

        setNextEnabled(page, false);
        cancelButton->hide();
        setBackEnabled(page, false);
        disconnect(timer, SIGNAL(timeout()), 0, 0);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateLog()));
        timer->setSingleShot(false);
        timer->start(1000);

        startBackgroundProcess();
        break;
    }

    case CHOOSE_NETWORKS_PAGE: // Networks
    {
        fillListOfNetworks();
        fillNetworks();
        backButton->setEnabled(false);
        nextButton->setEnabled(m_dialog->networklist->count ()>0 || Objects.size()>0);
        break;
    }

    case CHOOSE_OBJECTS_PAGE: // Objects
    {
        if (Networks.size()==0)
            setBackEnabled(page,false);

        fillListOfObjects();
        fillObjects();
        nextButton->setEnabled(m_dialog->objectlist->count ()>0 || m_dialog->networklist->count()>0);
        break;
    }

    case ADJUST_OBJECT_TYPES_PAGE: // Adjust Object type
    {
        setBackEnabled(page,true);
        fillTypeChangingList();
        break;
    }

    case TARGET_LIB_PAGE: // Target library
    {
        break;
    }

    case CREATE_OBJECTS_PAGE: // Objects creation ...
    {
        setBackEnabled(page,false);
        cancelButton->hide();
        createRealObjects();
        setNextEnabled(page, false);
        setFinishEnabled(page, true);
        finishButton->setFocus();
        break;
    }

    case NETWORK_ZONES_PAGE: // Network zones for PIX
    {
        setBackEnabled(page, false);
        cancelButton->hide();
        setNextEnabled(page, false);
        setFinishEnabled(page, true);
        finishButton->setFocus();
        fillNetworkZones();
        break;
    }

    default : {}

    }
    FromPage = page;
}

void DiscoveryDruid::startBackgroundProcess()
{
    switch (current_task)
    {
        case BT_HOSTS:
        case BT_IMPORT:
        {
            m_dialog->discoveryprogress->setMaximum(100);
            m_dialog->discoveryprogress->setValue(0);
            m_dialog->discoveryprogress->setEnabled(false);
            m_dialog->discoveryStopButton->setEnabled(false);
            break;
        }
        case BT_DNS:
        case BT_SNMP:
        {
            m_dialog->discoveryprogress->setMaximum(0);
            m_dialog->discoveryprogress->setValue(-1);
            break;
        }
        default:
        {}
    }

    switch (current_task)
    {
        case BT_HOSTS:    startHostsScan();     break;
        case BT_DNS:      startDNSScan();       break;
        case BT_SNMP:     startSNMPScan();      break;
        case BT_IMPORT:   startConfigImport();  break;
        default:
        {}
    }

}

void DiscoveryDruid::browseHostsFile()
{
    QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    st->getOpenFileDir(),
                    "All files (*)");

    if (s.isEmpty()) return;
    st->setOpenFileDir(s);

    m_dialog->filename->setText(s);

}

void DiscoveryDruid::browseForImport()
{
    QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    st->getOpenFileDir(),
                    "All files (*)");

    if (s.isEmpty()) return;
    st->setOpenFileDir(s);

    m_dialog->import_filename->setText(s);
}

void DiscoveryDruid::updatePrg()
{
    if (unBar!=nullptr)
    {
        unBar->setValue(unProg++);
    }
}

void DiscoveryDruid::getNameServers()
{
    // this is not supported anymore since all resolver functions
    // have been removed from class DNS
    m_dialog->nameserverlist->setEnabled(false);
    m_dialog->dnsfromlist->setEnabled(false);
    m_dialog->dnscustom->setChecked(true);
}

void DiscoveryDruid::setDiscoveryMethod_file()
{
    m_dialog->processname->setText(tr("Hosts file parsing ..."));
    current_task = BT_HOSTS;
    for (int i=0; i<WIZARD_PAGES; i++)
    {
        setAppropriate( i, WIZARD_FILE_PAGES[i]);
    }
}

void DiscoveryDruid::setDiscoveryMethod_DNS()
{
    m_dialog->processname->setText(tr("DNS zone transfer ..."));
    current_task = BT_DNS;
    for (int i=0; i<WIZARD_PAGES; i++)
    {
        setAppropriate( i, WIZARD_DNS_PAGES[i]);
    }
}

void DiscoveryDruid::setDiscoveryMethod_SNMP()
{
    m_dialog->processname->setText(tr("Network discovery using SNMP ..."));
    current_task = BT_SNMP;
    for (int i=0; i<WIZARD_PAGES; i++)
    {
        setAppropriate( i, WIZARD_SNMP_PAGES[i]);
    }
}

void DiscoveryDruid::setDiscoveryMethod_Import()
{
    m_dialog->processname->setText(tr("Import configuration from file ..."));
    current_task = BT_IMPORT;
    for (int i=0; i<WIZARD_PAGES; i++)
    {
        setAppropriate( i, WIZARD_IMPORT_PAGES[i]);
    }
}


void DiscoveryDruid::changedDiscoveryMethod(int c)
{
    if (fwbdebug)
        qDebug("DiscoveryDruid::changedDiscoveryMethod c=%d",c);

    switch (c)
    {
    case 0:
    {
        setDiscoveryMethod_file();
        break;
    }
    case 1:
    {
        setDiscoveryMethod_DNS();
        break;
    }
    case 2:
    {
        setDiscoveryMethod_SNMP();
        break;
    }
    case 3:
    {
        setDiscoveryMethod_Import();
        break;
    }

       default : {}
   }
}

void DiscoveryDruid::startHostsScan()
{
    if (thread!=nullptr)
    {
        delete thread;
    }

    thread = new HostsFileImport(m_dialog->filename->text());
    thread->setTargetWidget(this);
    thread->start();
}

void DiscoveryDruid::startConfigImport()
{
    if (thread!=nullptr)
    {
        delete thread;
    }

    QFile cf( m_dialog->import_filename->text()  );
    if (cf.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&cf);
        QString s = stream.readAll();
        cf.close();
        std::string *buffer = new std::string( s.toLatin1().constData() );
        //if (fwbdebug) qDebug(buffer->c_str());

        // count lines, gather some general stats on the config file.

        std::string::size_type pos, n;
        pos = 0;
        int line_count = 0;
        while ( (n=buffer->find('\n', pos))!=std::string::npos)
        {
            line_count++;
            pos = n+1;
        }
        m_dialog->discoveryprogress->setMaximum(line_count);

        // need to pick right platform string based on
        // m_dialog->import_platform->currentItem()
        string platform = selectedPlatform();

        //
        // ConfigImport "owns" buffer - it is deleted
        // in destructor of ConfigImport
        //
        thread = new ConfigImport(buffer, platform, m_dialog->obj_name->text().toStdString());
        thread->setTargetWidget(this);
        thread->start();

    } else
    {
        QMessageBox::critical(this, tr("Discovery error"),
          tr("Could not open file %1").arg(m_dialog->import_filename->text()));
        setBackEnabled(currentPage(),true);
    }
}

string DiscoveryDruid::selectedPlatform()
{
    string platform = "";
    switch (m_dialog->import_platform->currentIndex())
    {
    case IMPORT_IOS: platform = "iosacl"; break;
    case IMPORT_IPT: platform = "iptables"; break;
    case IMPORT_PIX: platform = "pix"; break;
    }
    return platform;
}

InetAddr DiscoveryDruid::getNS()
{
    string ns;
    if (m_dialog->dnscustom->isChecked())
    {
        ns=m_dialog->nameserverline->text().toLatin1().constData();

        try
        {
            return InetAddr(ns);
        } catch (FWException &ex)
        {
        /* perhaps not address but host name */
            list<InetAddr> addr;
            try
            {
                addr=DNS::getHostByName(ns);
            } catch (FWException &ex)
            {
                return InetAddr();
            }

            return addr.front();
        }
    }

    return NameServers[m_dialog->nameserverlist->currentText()];
}

void DiscoveryDruid::startDNSScan()
{
    // this is not supported since all resolver functions have been
    // removed from class DNS
}

InetAddr DiscoveryDruid::getSeedHostAddress()
{
    if (fwbdebug)
        qDebug() <<
            QString("DiscoveryDruid::getSeedHostAddress(): Seed host name %1").
            arg(m_dialog->seedhostname->text());

    libfwbuilder::InetAddr   seed_host_addr;
    if (!m_dialog->seedhostname->text().isEmpty())
    {
        try
        {
            QString a = getAddrByName( m_dialog->seedhostname->text(), AF_INET);
            if (fwbdebug)
                qDebug() <<
                    QString("DiscoveryDruid::getSeedHostAddress() address: %1").
                    arg(a);

            return InetAddr( a.toLatin1().constData() );
        } catch(const FWException &ex)
        {
        }

        try
        {
            seed_host_addr = InetAddr(
                m_dialog->seedhostname->text().toLatin1().constData());
            return seed_host_addr;
        } catch(const FWException &ex)
        {
        }

    }
    return seed_host_addr;
}

void DiscoveryDruid::changedDomainName()
{
    if (m_dialog->domainname->text().isEmpty())
    {
        nextButton->setEnabled(false);
    }
    else
    {
        nextButton->setEnabled(true);
    }
}

void DiscoveryDruid::changedNameServer()
{
    userIsTyping=true;
    isSNMPInclNetOK=true;

    if(m_dialog->dnscustom->isChecked())
    {
        nextButton->setEnabled(false);
        QString s=m_dialog->nameserverline->text();
        HostName=s;

        if (s.isEmpty())
        {
            timer->stop();
            m_dialog->DNSprogress_2->hide();

            QPalette palette = m_dialog->nameserver_error->palette();
            palette.setColor(m_dialog->nameserver_error->foregroundRole(), Qt::darkRed);
            m_dialog->nameserver_error->setPalette(palette);

            m_dialog->nameserver_error->setText("Enter valid host name or address.");
            nextButton->setEnabled(false);
            return;
        }

        if(isInetAddr(s))
        {
            timer->stop();
            m_dialog->DNSprogress_2->hide();

            QString rs=testInetAddr(s);
            if (rs.isEmpty())
            {
                m_dialog->nameserver_error->setText(" ");
                nextButton->setEnabled(true);
            }
            else
            {
                QPalette palette = m_dialog->nameserver_error->palette();
                palette.setColor(m_dialog->nameserver_error->foregroundRole(), Qt::darkRed);
                m_dialog->nameserver_error->setPalette(palette);

                m_dialog->nameserver_error->setText(rs);
                nextButton->setEnabled(false);
            }
        }
        else
        {
            unBar=m_dialog->DNSprogress_2;

            unBar->show();
            timer->setSingleShot(true);
            timer->start(1000);
            errMessage=m_dialog->nameserver_error;
            userIsTyping=false;

            QPalette palette = errMessage->palette();
            palette.setColor(errMessage->foregroundRole(), Qt::black);
            errMessage->setPalette(palette);

            errMessage->setText("DNS resolution in progress...");

            unProg = 0;
        }
    }
    else
    {
        timer->stop();
        m_dialog->DNSprogress_2->hide();
        m_dialog->nameserver_error->setText(" ");
        nextButton->setEnabled(true);
    }
}

void DiscoveryDruid::typedCustomNS()
{
    if(!m_dialog->dnscustom->isChecked())
    {
        m_dialog->dnscustom->setChecked(true);
    }
}

bool DiscoveryDruid::isInetAddr(const QString s)
{
    QRegExp r=QRegExp("^(\\d|\\.)+$",Qt::CaseInsensitive); //non wildcard
    return r.exactMatch(s);
}

QString DiscoveryDruid::testInetAddr(const QString s)
{
    QString res;
    QRegExp r=QRegExp("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$",Qt::CaseInsensitive); //non wildcard
    if (r.exactMatch(s))
    {
        try
        {
            InetAddr(s.toLatin1().constData());
        } catch(const FWException &ex)
        {
            res=ex.toString().c_str();
        }
    }
    else
    {
        res="Wrong IPv4 format";
    }
    return res;
}

void DiscoveryDruid::changedHostsFileName()
{
    QFile f;
    f.setFileName(m_dialog->filename->text());
    if (f.exists())
    {
        setNextEnabled(currentPage(),true);
    }
    else
    {
        setNextEnabled(currentPage(),false);
    }
}

void DiscoveryDruid::changedSNMPOptions()
{

}

void DiscoveryDruid::stopBackgroundProcess()
{
    if (fwbdebug)
        qDebug("stopBackgroundProcess bop=%p  isRunning=%d",
               bop,(bop!=nullptr)?bop->isRunning():-1);

    if (bop!=nullptr && bop->isRunning())
    {
        addToLog("Terminating task. Please wait...");

        bop->stop_operation();
        m_dialog->discoveryStopButton->setEnabled(false);
    }
}

void DiscoveryDruid::addNetwork()
{


    int count = m_dialog->networkresultlist->count();
    int upd_max=(count > 10)?count/10:1;
    int updc=upd_max;
    int t=0;
    QProgressDialog pd(tr("Adding objects ..."), tr("Cancel"), 0, count,this);

    QListWidgetItem* item=(QListWidgetItem*)m_dialog->networkresultlist->item(0);
    int i = 0;

    while (item)
    {

        if (item->isSelected())
        {
            QString k=item->text();
            if (!Networks[k].isSelected)
            {
                Networks[k].isSelected=true;
                m_dialog->networklist->addItem(item->text());
            }
        }

        i++;
        item=(QListWidgetItem*)m_dialog->networkresultlist->item(i);

        if (updc--<=0)
        {
            pd.setValue(t);
            qApp->processEvents();

            if (pd.wasCanceled())
            {
                break;
            }
            updc=upd_max;
        }
        t++;
    }
    nextButton->setEnabled(m_dialog->networklist->count ()>0 || Objects.size()>0);

}

void DiscoveryDruid::removeNetwork()
{
    QListWidgetItem* item1=m_dialog->networklist->item(0);
    QListWidgetItem* item2;

    while (item1!=0)
    {
        item2=m_dialog->networklist->item(
                m_dialog->networklist->row(item1)+1);
        if (item1->isSelected())
        {
            Networks[item1->text()].isSelected=false;
            delete item1;
        }
        item1=item2;
    }
    nextButton->setEnabled(m_dialog->networklist->count ()>0 || Objects.size()>0);
}

void DiscoveryDruid::setNetworkFilter()
{
    flt_net_d->exec();
    fillListOfNetworks();
}

void DiscoveryDruid::removeNetworkFilter()
{
    flt_net->clear();
    fillListOfNetworks();
}

void DiscoveryDruid::addObject()
{
    int count = m_dialog->objectresultlist->count();
    int upd_max=(count > 10)?count/10:1;
    int updc=upd_max;
    int t=0;
    QProgressDialog pd(tr("Adding objects ..."),
                       tr("Cancel"), 0, count,this);

    QListWidgetItem* item=(QListWidgetItem*)m_dialog->objectresultlist->item(0);
    int i = 0;

    while (item)
    {
        if (item->isSelected())
        {
            QString k=item->text();
            if (!Objects[k].isSelected)
            {
                Objects[k].isSelected=true;
                m_dialog->objectlist->addItem(item->text());
            }
        }

        i++;
        item=(QListWidgetItem*)m_dialog->objectresultlist->item(i);

        if (updc--<=0)
        {
            pd.setValue(t);
            qApp->processEvents();

            if (pd.wasCanceled())
            {
                break;
            }
            updc=upd_max;
        }
        t++;
    }
    nextButton->setEnabled(m_dialog->objectlist->count ()>0 || m_dialog->networklist->count()>0);
}

void DiscoveryDruid::removeObject()
{
    QListWidgetItem* item1=m_dialog->objectlist->item(0);
    QListWidgetItem* item2;

    while (item1!=0)
    {
        item2=m_dialog->objectlist->item(
                m_dialog->objectlist->row(item1)+1);
        if (item1->isSelected())
        {
            Objects[item1->text()].isSelected=false;
            delete item1;
        }
        item1=item2;
    }
    nextButton->setEnabled(m_dialog->objectlist->count ()>0 || m_dialog->networklist->count()>0);
}

void DiscoveryDruid::setLastFilter()
{
    flt_last_d->exec();
    fillTypeChangingList();
}

void DiscoveryDruid::setObjectFilter()
{
    flt_obj_d->exec();
    fillListOfObjects();
}

void DiscoveryDruid::removeLastFilter()
{
    flt_last->clear();
    fillTypeChangingList();
}

void DiscoveryDruid::removeObjectFilter()
{
    flt_obj->clear();
    fillListOfObjects();
}

void DiscoveryDruid::selectAllResNets()
{
    m_dialog->networkresultlist->selectAll();
}

void DiscoveryDruid::selectAllNets()
{
    m_dialog->networklist->selectAll();
}

void DiscoveryDruid::selectAllResObjs()
{
    m_dialog->objectresultlist->selectAll();
}

void DiscoveryDruid::selectAllObjs()
{
    m_dialog->objectlist->selectAll();
}

void DiscoveryDruid::fillNetworkZones()
{
    m_dialog->iface_nz_list->clear();

    QStringList labels;
    labels << QObject::tr("Name") << QObject::tr("Label")
           << QObject::tr("Address") << QObject::tr("Network Zone");
    m_dialog->iface_nz_list->setHorizontalHeaderLabels(labels);

    NetworkZoneManager netzone_manager;
    netzone_manager.load(mw->activeProject()->db());

    list<FWObject*> all_interfaces = discovered_fw->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*>::iterator it;
    int row = 0;
    for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
    {
        Interface *iface = Interface::cast(*it);

        m_dialog->iface_nz_list->insertRow(row);

        QTableWidgetItem* itm;

        itm = new QTableWidgetItem(iface->getName().c_str());
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_nz_list->setItem(row, 0, itm);

        itm = new QTableWidgetItem(iface->getLabel().c_str());
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_nz_list->setItem(row, 1, itm);

        QString addr_str;
        const InetAddr* addr = iface->getAddressPtr();
        if (addr) addr_str = addr->toString().c_str();

        itm = new QTableWidgetItem(addr_str);
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_nz_list->setItem(row, 2, itm);

        QComboBox *widget = new QComboBox();
        netzone_manager.packComboBox(widget, -1);
        m_dialog->iface_nz_list->setCellWidget(row, 3, widget);

        row++;
    }

    m_dialog->iface_nz_list->resizeColumnToContents(3);
}

void DiscoveryDruid::fillNetworks()
{
    ObjectDescriptor buf;

    m_dialog->networklist->clear();
    bool f=false;
    QMap<QString,ObjectDescriptor >::iterator i;
    for(i=Networks.begin(); i!=Networks.end(); ++i)
    {
        buf=i.value();
        if (buf.isSelected)
        {
            m_dialog->networklist->addItem(new QListWidgetItem(i.key()));
            f=true;
        }
    }
    nextButton->setEnabled(f);
}

void DiscoveryDruid::fillObjects()
{
    ObjectDescriptor buf;

    m_dialog->objectlist->clear();
    bool f=false;
    QMap<QString,ObjectDescriptor >::iterator i;
    for(i=Objects.begin(); i!=Objects.end(); ++i)
    {
        buf=i.value();
        if (buf.isSelected)
        {
            m_dialog->objectlist->addItem(new QListWidgetItem(i.key()));
            f=true;
        }
    }
    nextButton->setEnabled(f);
}

void DiscoveryDruid::fillTypeChangingList()
{

    ObjectDescriptor buf;

    m_dialog->typeChangingList->clear();

    QMap<QString,ObjectDescriptor >::iterator i;
    for(i=Objects.begin(); i!=Objects.end(); ++i)
    {
        buf=i.value();
        if (buf.isSelected)
        {
            QString ins;
            if ( flt_last->test(buf) )
            {
                ins=(buf.interfaces.size())?
                    QString("%1").arg(buf.interfaces.size()):"";
                QStringList sl;
                sl << buf.toString().c_str() << ins << buf.type.c_str();
                new QTreeWidgetItem( m_dialog->typeChangingList, sl );
            }
        }
    }

    m_dialog->typeChangingList->resizeColumnToContents(0);
    m_dialog->typeChangingList->resizeColumnToContents(1);
}

void DiscoveryDruid::loadDataFromDNS()
{
    // this is not supported since all resolver functions have been
    // removed from class DNS
}

void DiscoveryDruid::loadDataFromFile()
{
    m_dialog->objectresultlist->clear();
    int t=0;
    HostsFileImport *himport = dynamic_cast<HostsFileImport*>(thread);
    assert(himport!=nullptr);
    int count = himport->hosts.size();
    if (count > 0)
    {
        int upd_max=(count > 10)?count/10:1;

        int updc=upd_max;

        QProgressDialog pd(tr("Prepare objects ..."), tr("Cancel"), 0, count,this);

        vector<ObjectDescriptor>::iterator i;
        for(i = himport->hosts.begin(); i != himport->hosts.end(); ++i)
        {
            if (i->type.empty())
            {
                i->type=IPv4::TYPENAME;
            }
            i->isSelected=false;

            Objects[i->toString().c_str()] = *i;
            if (updc--<=0)
            {
                pd.setValue(t);
                qApp->processEvents();

                if (pd.wasCanceled())
                {
                    break;
                }
                updc=upd_max;
            }
            t++;
        }
    }
}

void DiscoveryDruid::loadDataFromImporter()
{
    ConfigImport *confimp = dynamic_cast<ConfigImport*>(thread);
    assert(confimp!=nullptr);
    Importer *imp = confimp->getImporterObject();
    if (imp!=nullptr)
    {
        Firewall *fw = imp->finalize();
        qApp->processEvents(); // to flush the log

        if (fw) // fw can be nullptr if import was uncussessful
        {
            discovered_fw = fw;

            ProjectPanel *pp = mw->activeProject();
            QString filename = pp->getFileName();
            //pp->m_panel->om->reload();
            //pp->m_panel->om->autoRenameChildren(fw, "");

            QCoreApplication::postEvent(mw, new reloadObjectTreeEvent(filename));
            if (mw->isEditorVisible())
                QCoreApplication::postEvent(
                    mw, new openObjectInEditorEvent(filename, fw->getId()));
            QCoreApplication::postEvent(
                mw, new showObjectInTreeEvent(filename, fw->getId()));

            // Open first created Policy ruleset object
            FWObject *first_policy = fw->getFirstByType(Policy::TYPENAME);
            if (first_policy)
                QCoreApplication::postEvent(
                    mw, new openRulesetEvent(filename, first_policy->getId()));

        }
    }
}

void DiscoveryDruid::saveScanLog()
{
    QString s = QFileDialog::getSaveFileName(
                    this,
                    "Choose a file",
                    st->getOpenFileDir(),
                    "Text file (*.txt)");

    if (s.isEmpty()) return;
    st->setOpenFileDir(s);

    if (s.endsWith(".txt")) s += ".txt";

    QFile f(s);
    if (f.open(QIODevice::WriteOnly))
    {
        if (fwbdebug)
        {
            qDebug("Saving crawler log to file: %d chars",
                   m_dialog->discoverylog->toPlainText().length());
            qDebug("--------------------------------");
        }

        QTextStream strm(&f);
        QString txt = m_dialog->discoverylog->toPlainText();
        strm << txt << endl;
        if (fwbdebug) qDebug("%s",txt.toLatin1().constData());
        if (fwbdebug)
            qDebug("--------------------------------");
        f.close();
    }
}

void DiscoveryDruid::startSNMPScan()
{
#ifdef HAVE_LIBSNMP


    bool use_incl=!m_dialog->snmpinaddr->text().isEmpty() && !m_dialog->snmpinmask->text().isEmpty();
    if (use_incl)
    {
        try
        {
            InetAddrMask in(
                 InetAddr(m_dialog->snmpinaddr->text().toLatin1().constData()),
                 InetAddr(m_dialog->snmpinmask->text().toLatin1().constData())
                 );
            include_networks.push_back(in);
        }
        catch (const FWException &ex)
        {
            //TODO: do something usefull
        }
    }
    libfwbuilder::SNMPCrawler *q = new SNMPCrawler();
    q->init(getSeedHostAddress(),
            m_dialog->snmpcommunity->text().toLatin1().constData(),
            m_dialog->snmprecursive->isChecked(),
            false,
            m_dialog->snmpfollowp2p->isChecked(),
            0,
            m_dialog->snmpretries->value(),
            1000000L*m_dialog->snmptimeout->value(),
            0,
            0,
            (use_incl) ? &include_networks : nullptr);

    m_dialog->discoveryprogress->setMaximum(0);
    unBar = m_dialog->discoveryprogress;

    bop=q;
    try
    {
        logger = bop->start_operation();
        if (fwbdebug) logger->copyToStderr();
        addToLog("Collecting data ...");

        disconnect(timer, SIGNAL(timeout()), 0, 0);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateLog()));
        timer->setSingleShot(false);
        timer->start(100);

    } catch(const FWException &ex)
    {
        delete q;
        q=nullptr;
    }


#endif
}

void DiscoveryDruid::loadDataFromCrawler()
{
#ifdef HAVE_LIBSNMP
    SNMPCrawler *q=(SNMPCrawler*)bop;
    Objects.clear();
    Networks.clear();

    set<InetAddrMask*>::iterator m;
    set<InetAddrMask*> discovered_networks = q->getNetworks();

    if (fwbdebug)
        qDebug() << QString("got %1 networks").arg(discovered_networks.size());

    for (m=discovered_networks.begin(); m!=discovered_networks.end(); ++m)
    {
        ObjectDescriptor od;
        InetAddrMask *net = *m;

        if (fwbdebug)
            qDebug() << QString("network %1").arg(net->toString().c_str());

        // if address in *m is ipv6, recreate it as Inet6AddrMask and
        // use type NetworkIPv6
        if (net->getAddressPtr()->isV6())
        {
            Inet6AddrMask in6am(*(net->getAddressPtr()),
                                *(net->getNetmaskPtr()));
            od.sysname = in6am.toString(); // different from ipv6
            od.type = NetworkIPv6::TYPENAME;
        } else
        {
            od.sysname = net->toString();
            od.type = Network::TYPENAME;
        }
        od.addr = *(net->getAddressPtr());
        od.netmask = *(net->getNetmaskPtr());
        od.isSelected = false;
        Networks[od.sysname.c_str()]= od;
    }

    map<InetAddr, CrawlerFind>  t = q->getAllIPs();

    if (fwbdebug)
        qDebug() << QString("got %1 addresses").arg(t.size());

    m_dialog->discoveryprogress->setMaximum( t.size() );
    m_dialog->discoveryprogress->setValue(0);

    int cntr = 0;
    map<InetAddr, CrawlerFind>::iterator j;
    for(j = t.begin(); j!=t.end(); ++j,++cntr)
    {
        m_dialog->discoveryprogress->setValue( cntr );

        ObjectDescriptor od( (*j).second );
        od.addr     = (*j).first;
        od.type=(od.interfaces.size()>1)?
            (Host::TYPENAME):(IPv4::TYPENAME);

        od.isSelected=false;

        if (od.sysname.empty())
        {
            od.sysname = string("h-") + od.addr.toString();
            if (m_dialog->snmpdodns->isChecked())
            {
                QString hostName = getNameByAddr( od.addr.toString().c_str() );
                if (!hostName.isEmpty())
                    od.sysname = hostName.toLatin1().constData();
            }

            addToLog(
                QString(od.addr.toString().c_str()) + " : " + od.sysname.c_str());

        }

        Objects[od.toString().c_str()]=od;

        set<string>::iterator si;
        for(si=od.dns_info.aliases.begin();
                si!=od.dns_info.aliases.end();
                ++si)
        {
            od.sysname=(*si);
            Objects[od.toString().c_str()]=od;
        }
    }
#endif
}



void DiscoveryDruid::fillListOfNetworks()
{
    m_dialog->networkresultlist->clear();
    int t=0;
    int count = Networks.size();
    if (count > 0)
    {
        int upd_max=(count > 10)?count/10:1;

        int updc=upd_max;

        QProgressDialog pd(tr("Copying results ..."), tr("Cancel"), 0, count,this);

        QMap<QString, ObjectDescriptor>::iterator i;
        for (i=Networks.begin(); i!=Networks.end(); ++i)
        {

            if ( flt_net->test(i.value()) )
            {

                m_dialog->networkresultlist->addItem(new QListWidgetItem(i.key()));
                if (updc--<=0)
                {
                    pd.setValue(t);
                    qApp->processEvents();

                    if (pd.wasCanceled())
                    {
                        break;
                    }
                    updc=upd_max;
                }
            }
            t++;
        }
    }
}

void DiscoveryDruid::fillListOfObjects()
{

    m_dialog->objectresultlist->clear();
    int t=0;
    int count = Objects.size();
    if (count > 0)
    {
        int upd_max=(count > 10)?count/10:1;

        int updc=upd_max;

        QProgressDialog pd(tr("Copying results ..."),
                           tr("Cancel"), 0,count,this);

        QMap<QString,ObjectDescriptor >::iterator i;
        for(i=Objects.begin(); i!=Objects.end(); ++i)
        {
            if ( flt_obj->test(i.value()) )
            {

                m_dialog->objectresultlist->addItem(new QListWidgetItem(i.key()));
                if (updc--<=0)
                {
                    pd.setValue(t);
                    qApp->processEvents();

                    if (pd.wasCanceled())
                    {
                        break;
                    }
                    updc=upd_max;
                }
            }
            t++;
        }
    }
}

void DiscoveryDruid::customEvent(QEvent *event)
{
    int evtype=(int)event->type();
    if (evtype == ProgressEv)
    {
        ProgressEvent *e = (ProgressEvent*)event;
        m_dialog->discoveryprogress->setValue(e->value);
    } else if (evtype == DoneEv)
    {
        cancelButton->show();

        timer->stop();
        disconnect(timer,SIGNAL(timeout()),0,0);

        updateLog();
        m_dialog->logSaveButton->setEnabled(true);

        // actually create objects
        switch (current_task)
        {
        case BT_HOSTS:
            loadDataFromFile();
            break;

        case BT_IMPORT:
            loadDataFromImporter();
            break;

        default:
            break;
        }

        thread->wait();
        QString er = thread->getError();
        delete thread;
        thread=nullptr;

        switch (current_task)
        {
        case BT_HOSTS:
            if (Objects.size()>0)
            {
                nextButton->setDefault(true);
                nextButton->setFocus();
                nextButton->setEnabled(true);
                backButton->setEnabled(false);
            }
            else
            {
                backButton->setEnabled(true);
                nextButton->setEnabled(false);
            }
            break;

        case BT_IMPORT:
        {
            // if imported PIX, need to show one more page to select network zones
            if (selectedPlatform() == "pix")
            {
                setNextEnabled(currentPage(), true);
                setFinishEnabled(currentPage(), false);
            } else
            {
                setNextEnabled(currentPage(), false);
                setFinishEnabled(currentPage(), true);
                finishButton->setFocus();
            }
            break;
        }
        default:
            break;
        }

    }
}

void DiscoveryDruid::updateLog()
{
    if (fwbdebug) qDebug("DiscoveryDruid::updateLog");

    if (current_task==BT_HOSTS || current_task==BT_IMPORT)
    {
        QString buf;
        if (thread!=nullptr)
        {
            while(thread->Log->ready())
            {
                buf = thread->Log->getLine().c_str();
                addToLog(buf);
            }
        }
    }
    else if (current_task==BT_SNMP)
    {
       if (monitorOperation() > 0)
       {

           //m_dialog->discoveryprogress->setValue(prg++);
       }
       else
       {
            timer->stop();
            disconnect(timer,SIGNAL(timeout()),0,0);

            if (fwbdebug) qDebug("Crawler finished");

            loadDataFromCrawler();

            cancelButton->show();

            FWException * ex=bop->get_latest_error();
            if (ex!=nullptr)
            {
                QMessageBox::critical(this,
                                      tr("Discovery error"),
                                      ex->toString().c_str());
            }
            if (Objects.size()>0 || Networks.size()>0)
            {
                if (Networks.size()==0) setAppropriate( 8,0);
                nextButton->setEnabled(true);
                nextButton->setDefault(true);
                nextButton->setFocus();
                backButton->setEnabled(false);
            }
            else
            {
                nextButton->setEnabled(false);
                backButton->setEnabled(true);
            }

            m_dialog->logSaveButton->setEnabled(true);

            delete bop;
            bop=nullptr;
            unBar=nullptr;
            m_dialog->discoveryprogress->setMaximum(100);
            m_dialog->discoveryprogress->setValue(100);
            m_dialog->discoveryStopButton->setEnabled(false);
       }
    }
    else if (current_task==BT_DNS)
    {
       if (monitorOperation() > 0)
       {
           //m_dialog->discoveryprogress->setMaximum(0);
           //m_dialog->discoveryprogress->setValue(
           //        m_dialog->discoveryprogress->progress()+1);
       }
       else
       {
            timer->stop();
            disconnect(timer,SIGNAL(timeout()),0,0);

            loadDataFromDNS();

            cancelButton->show();
            FWException * ex=bop->get_latest_error();
            if (ex!=nullptr)
            {
                QMessageBox::critical(this, tr("Discovery error"),
                                      ex->toString().c_str());
            }
            if (Objects.size()>0)
            {
                nextButton->setEnabled(true);
                nextButton->setDefault(true);
                nextButton->setFocus();
                backButton->setEnabled(false);
            }
            else
            {
                nextButton->setEnabled(false);
                backButton->setEnabled(true);
            }
            m_dialog->logSaveButton->setEnabled(true);
            delete bop;
            bop=nullptr;
            unBar=nullptr;
            m_dialog->discoveryprogress->setMaximum(100);
            m_dialog->discoveryprogress->setValue(100);
            m_dialog->discoveryStopButton->setEnabled(false);
       }
    }
}

void DiscoveryDruid::addToLog(const QString &buf)
{
    if (buf.isEmpty()) return;

    foreach(QString line, buf.trimmed().split("\n"))
    {
        QTextCharFormat format = normal_format;

        if (line.contains("Parser error"))
            format = error_format;

        if (line.contains("Parser warning"))
            format = warning_format;

        if (line.contains("SNMP error, status 2 Timeout"))
            format = warning_format;

        QString txt = line;
        while (!txt.isEmpty() && (txt.endsWith("\n") || txt.endsWith("\r")))
            txt.chop(1);

        if (format == error_format || format == warning_format)
            format.setAnchorHref(txt);

        QTextCursor cursor = m_dialog->discoverylog->textCursor();
        cursor.insertBlock();
        cursor.insertText(txt, format);
    }

    m_dialog->discoverylog->ensureCursorVisible();
}

void DiscoveryDruid::changedSeedHost()
{
    m_dialog->seedhosterror_message->setText(" ");
    userIsTyping = true;
    errMessage = m_dialog->seedhosterror_message;
    HostName = m_dialog->seedhostname->text();

    if (HostName.isEmpty())
    {
        timer->stop();
        m_dialog->DNSprogress->hide();
        QPalette palette = m_dialog->seedhosterror_message->palette();
        palette.setColor(
            m_dialog->seedhosterror_message->foregroundRole(), Qt::darkRed);
        m_dialog->seedhosterror_message->setPalette(palette);

        m_dialog->seedhosterror_message->setText(
            "Enter a valid host name or address.");
        isSeedHostOK=false;
    }
    else
    {
        if(isInetAddr(HostName))
        { // seems to be an IP Address
            m_dialog->DNSprogress->hide();
            timer->stop();
            QRegExp r = QRegExp(
                "^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$",
                Qt::CaseInsensitive); //non wildcard
            if (r.exactMatch(HostName))
            {
                try
                {
                    InetAddr(HostName.toLatin1().constData());

                    QPalette palette =
                        m_dialog->seedhosterror_message->palette();
                    palette.setColor(
                        m_dialog->seedhosterror_message->foregroundRole(),
                        Qt::darkGreen);
                    m_dialog->seedhosterror_message->setPalette(palette);

                    m_dialog->seedhosterror_message->setText(
                        "Address verified");
                    isSeedHostOK=true;
                } catch(const FWException &ex)
                {
                    QPalette palette =
                        m_dialog->seedhosterror_message->palette();
                    palette.setColor(
                        m_dialog->seedhosterror_message->foregroundRole(),
                        Qt::darkRed);
                    m_dialog->seedhosterror_message->setPalette(palette);

                    m_dialog->seedhosterror_message->setText(
                        ex.toString().c_str());
                    // need to return focus to the input field in case of error
                    //m_dialog->seedhostname->setFocus();
                    isSeedHostOK=false;
                }
            }
            else
            {
                QPalette palette = m_dialog->seedhosterror_message->palette();
                palette.setColor(
                    m_dialog->seedhosterror_message->foregroundRole(),
                    Qt::darkRed);
                m_dialog->seedhosterror_message->setPalette(palette);

                m_dialog->seedhosterror_message->setText("Wrong IPv4 format");
                isSeedHostOK=false;

            }
        }
        else
        {// it looks like a DNS name
            isSeedHostOK = false;

            QPalette palette = m_dialog->seedhosterror_message->palette();
            palette.setColor(
                m_dialog->seedhosterror_message->foregroundRole(), Qt::black);
            m_dialog->seedhosterror_message->setPalette(palette);

            m_dialog->seedhosterror_message->setText(
                "DNS resolution in progress...");
            unProg = 0;
            unBar=m_dialog->DNSprogress;

            errMessage=m_dialog->seedhosterror_message;
            m_dialog->DNSprogress->show();
            timer->setSingleShot(true);
            timer->start(1000);
        }
    }
    nextButton->setEnabled(isSNMPInclNetOK && isSeedHostOK);
}

void DiscoveryDruid::changedInclNet()
{
    setNextEnabled(currentPage(),false);
    m_dialog->confineerror_message->setText(" ");
    bool use_incl=!m_dialog->snmpinaddr->text().isEmpty() && !m_dialog->snmpinmask->text().isEmpty();
    if (use_incl)
    {
        try
        {

            InetAddr a(m_dialog->snmpinaddr->text().toLatin1().constData());
            InetAddr n(m_dialog->snmpinmask->text().toLatin1().constData());
            InetAddrMask(a,n);

            m_dialog->confineerror_message->setText(" ");
            isSNMPInclNetOK=true;
        } catch (const FWException &ex)
        {
            isSNMPInclNetOK=false;
            m_dialog->confineerror_message->setText(ex.toString().c_str());
        }

    }
    else
    {
        if (!m_dialog->snmpinaddr->text().isEmpty() || !m_dialog->snmpinmask->text().isEmpty())
        {
            isSNMPInclNetOK=false;
            m_dialog->confineerror_message->setText(tr("Incomlete network specification."));
        }
        else
        {
            m_dialog->confineerror_message->setText(" ");
            isSNMPInclNetOK=true;
        }
    }
    nextButton->setEnabled(isSNMPInclNetOK && isSeedHostOK);
}

int DiscoveryDruid::monitorOperation()
{
    QString buf;
    bool fl;

    if (fwbdebug) qDebug("monitorOperation  bop=%p  isRunning=%d",
                         bop,(bop!=nullptr)?bop->isRunning():-1);


    fl=false;
    while( logger->ready() )
    {
        buf= logger->getLine().c_str();
        if (buf.endsWith('\n'))
            buf = buf.left(buf.length() - 1);

        addToLog(buf);

        /*if (fwbdebug) qDebug("monitorOperation  appending the following buf: (1)");
        if (fwbdebug) qDebug(buf.toLatin1().constData());
        if (fwbdebug) qDebug("----------------------------------------------");*/

        fl=true;
    }
    if (fl)
    {
        return 1;
    }
    if (bop==nullptr)
    {

        return 0; // BackgroundOp has been disconnected
    }

    if (bop->isRunning())
    {
        return 1;
    }
    // send signal "completed", argument is 0 if ok and -1 if error


    FWException *ex=bop->get_latest_error();
    if (ex)
    {
        buf= ex->toString().c_str();
        if (buf.endsWith('\n'))
            buf = buf.left(buf.length() - 1);

        addToLog(buf);

        /*if (fwbdebug) qDebug("monitorOperation  appending the following buf: (2)");
        if (fwbdebug) qDebug(buf.toLatin1().constData());
        if (fwbdebug) qDebug("----------------------------------------------");*/

      //  completed(-1);   // this sends signal to another widget
    } else
    {
      //  completed(0);   // this sends signal to another widget
    }
    return 0;

}

void DiscoveryDruid::checkHostName()
{
    if (!HostName.isEmpty())
    {
        userIsTyping=false;
        QHostInfo::lookupHost(HostName,
                              this, SLOT(dnsFinish(QHostInfo)));
    }
}

void DiscoveryDruid::checkSNMPCommunity()
{
    if (m_dialog->snmpcommunity->text().isEmpty())
    {
        m_dialog->snmpcommunity_message->setText(tr("Empty community string"));
        setNextEnabled(currentPage(),false);
    }
    else
    {
        m_dialog->snmpcommunity_message->setText("");
        setNextEnabled(currentPage(),true);
    }
}

void DiscoveryDruid::changeTargetObject(const QString &buf)
{

    QTreeWidgetItem* item=m_dialog->typeChangingList->topLevelItem(0);

    while (item!=0)
    {
        if (item->isSelected())
        {
            Objects[item->text(0)].type=buf.toLatin1().constData();
            item->setText(2,buf);
        }
        item=m_dialog->typeChangingList->topLevelItem(
                m_dialog->typeChangingList->indexOfTopLevelItem(item)+1);
    }
}

void DiscoveryDruid::selectAllLast()
{
    m_dialog->typeChangingList->selectAll();
}

void DiscoveryDruid::unselectAllLast()
{
    m_dialog->typeChangingList->selectAll();
}

void DiscoveryDruid::typeAddress()
{
    changeTargetObject(IPv4::TYPENAME);
}

void DiscoveryDruid::typeHost()
{
    changeTargetObject(Host::TYPENAME);
}

void DiscoveryDruid::typeFirewall()
{
    changeTargetObject(Firewall::TYPENAME);
}

/*
 * Guess OS from the sysDescr string returned by the host. Returned OS
 * name is always lower case one word, such as "linux", "ios"
 *
 * Examples of sysDescr strings:
 *
 * IOS (tm) 3600 Software (C3620-IK9O3S-M), Version 12.2(13), RELEASE SOFTWARE (fc1)
 * Linux guardian 2.4.20 #2 Wed Nov 17 11:49:43 CET 2004 mips
 * Linux crash 2.6.24-22-server #1 SMP Mon Nov 24 20:06:28 UTC 2008 x86_64
 * Apple AirPort - Apple Computer, 2006.  All rights Reserved
 * Cisco Secure FWSM Firewall Version 2.3(4)
 * Cisco PIX Firewall Version 6.2(1)
 * Cisco Adaptive Security Appliance Version 8.2(0)227
 */
QString DiscoveryDruid::guessOS(const string &sysDescr)
{
    QStringList elements = QString(sysDescr.c_str()).split(" ");
    QString first = elements[0].toLower();
    if (first == "cisco")
    {
        if (elements[1].toLower() == "adaptive" &&
            elements[2].toLower() == "security" &&
            elements[3].toLower() == "appliance") return "pix";
        if (elements[1].toLower() == "pix") return "pix";
        if (elements[1].toLower() == "secure" &&
            elements[2].toLower() == "fwsm") return "pix";
    }
    if (first == "darwin") return "macosx";
    if (first == "apple") return "macosx";

    return first;
}


FWObject* DiscoveryDruid::addInterface(FWObject *parent, InterfaceData *in,
                                       bool skip_ip_address_check)
{
    ObjectManipulator *om = mw->activeProject()->m_panel->om;

    if (!m_dialog->snmpincludeunnumbered->isChecked() && !skip_ip_address_check)
    {
        if (in->addr_mask.size()==0) return nullptr;
        if (in->addr_mask.front()->getAddressPtr()->isAny())
            return nullptr;
    }

    QString obj_name = in->name.c_str();
    Interface *itf = nullptr;
    itf = Interface::cast(
        mw->createObject(parent,
                         QString(Interface::TYPENAME), obj_name));
    
    QString iname = om->getStandardName(itf, physAddress::TYPENAME, "mac");
    iname = om->makeNameUnique(itf, iname, physAddress::TYPENAME);

    physAddress *paddr = physAddress::cast(
        mw->createObject(itf, physAddress::TYPENAME, iname)
    );
    paddr->setPhysAddress(in->mac_addr);

    itf->setLabel(in->label);
    itf->setSecurityLevel(in->securityLevel);

    if (fwbdebug)
        qDebug() << "Interface=" << obj_name
                 << "type=" << in->interface_type.c_str();

    if (!in->interface_type.empty())
    {
        itf->getOptionsObject()->setStr("type", in->interface_type);
        if (in->interface_type == "8021q")
            itf->getOptionsObject()->setInt("vlan_id", in->vlan_id);
    } else
    {
        std::unique_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(parent));
        if (int_prop->looksLikeVlanInterface(obj_name))
        {
            QString base_name;
            int vlan_id;
            int_prop->parseVlan(obj_name, &base_name, &vlan_id);

            itf->getOptionsObject()->setStr("type", "8021q");
            itf->getOptionsObject()->setInt("vlan_id", vlan_id);
        }
    }

    if (in->addr_mask.size()==0 ||
        in->addr_mask.front()->getAddressPtr()->isAny())
    {
        itf->setUnnumbered(true);
    } else
    {
        list<InetAddrMask*>::iterator n;
        for (n=in->addr_mask.begin(); n!=in->addr_mask.end(); ++n)
        {
            const InetAddr *addr = (*n)->getAddressPtr();
            const InetAddr *netm = (*n)->getNetmaskPtr();

            if (addr->isV4())
            {
                try
                {
                    QString iname = om->getStandardName(itf, IPv4::TYPENAME, "ip");
                    iname = om->makeNameUnique(itf, iname, IPv4::TYPENAME);

                    IPv4 *ipv4= IPv4::cast(
                        om->createObject(itf, IPv4::TYPENAME, iname)
                    );
                    ipv4->setAddress(*addr);
                    ipv4->setNetmask(*netm);
                } catch (FWException &ex)
                {
                    cerr << "FWException: " << ex.toString() << endl;
                }
            }

            if (addr->isV6())
            {
                try
                {
                    QString iname = om->getStandardName(itf, IPv6::TYPENAME, "ip");
                    iname = om->makeNameUnique(itf, iname, IPv6::TYPENAME);

                    IPv6 *ipv6 = IPv6::cast(
                        om->createObject(itf, IPv6::TYPENAME, iname)
                    );
                    ipv6->setAddress(*addr);
                    ipv6->setNetmask(*netm);
                } catch (FWException &ex)
                {
                    cerr << "FWException: " << ex.toString() << endl;
                }
            }
        }
    }
    return itf;
}

void DiscoveryDruid::createRealObjects()
{
    ObjectDescriptor od;
    string type, name, a;

    int t=0;
    m_dialog->lastprogress->setValue(0);
    m_dialog->lastprogress->setMaximum( Objects.size());

    QMap<QString,ObjectDescriptor >::iterator i;
    for (i=Networks.begin(); i!=Networks.end(); ++i)
    {
        od=i.value();
        if (od.isSelected)
        {
            type = od.type; // Network or NetworkIPv6
            name = od.sysname;
            a = od.addr.toString().c_str();

            Address *net = Address::cast(
                mw->createObject(type.c_str(), name.c_str()));
            assert(net!=nullptr);
            net->setName(name);
            net->setAddress(od.addr);
            net->setNetmask(od.netmask);

            mw->moveObject(m_dialog->libs->currentText(), net);
        }
    }

    for (i=Objects.begin(); i!=Objects.end(); ++i)
    {
        od = i.value();
        type = od.type;

        name = od.sysname;

        QString os = guessOS(od.descr);

        a = od.addr.toString();

        if (od.isSelected)
        {
            if (type==Host::TYPENAME || type==Firewall::TYPENAME)
            {
                FWObject *o=nullptr;

                o = mw->createObject(type.c_str(), name.c_str());
                o->setName(name);

                if (type==Firewall::TYPENAME)
                {
                    if (os == "linux")
                    {
                        o->setStr("platform", "iptables");
                        o->setStr("host_OS", "linux24");
                    }
                    if (os == "freebsd")
                    {
                        o->setStr("platform", "pf");
                        o->setStr("host_OS", "freebsd");
                    }
                    if (os == "openbsd")
                    {
                        o->setStr("platform", "pf");
                        o->setStr("host_OS", "openbsd");
                    }
                    if (os == "ios")
                    {
                        o->setStr("platform", "iosacl");
                        o->setStr("host_OS", "ios");
                    }
                    if (os == "pix" || os == "fwsm")
                    {
                        o->setStr("platform", "pix");
                        o->setStr("host_OS", "pix_os");
                    }
                    if (os == "apple")
                    {
                        o->setStr("platform", "ipfw");
                        o->setStr("host_OS", "macosx");
                    }
                    if (os == "solaris")
                    {
                        o->setStr("platform", "ipf");
                        o->setStr("host_OS", "solaris");
                    }

                    Resources::setDefaultTargetOptions( o->getStr("platform"),
                                                        Firewall::cast(o) );
                    Resources::setDefaultTargetOptions( o->getStr("host_OS"),
                                                        Firewall::cast(o) );
                }

                if (od.interfaces.size()==0)
                {
                    Interface *itf= Interface::cast(
                        mw->createObject(o,Interface::TYPENAME,"nic1")
                    );

                    if (od.addr.isV4())
                    {
                        IPv4 *ipv4= IPv4::cast(
                            mw->createObject(itf, IPv4::TYPENAME, a.c_str())
                        );
                        ipv4->setAddress(od.addr);
                        ipv4->setNetmask(InetAddr());
                    }

                    if (od.addr.isV6())
                    {
                        IPv6 *ipv6 = IPv6::cast(
                            mw->createObject(itf, IPv6::TYPENAME, a.c_str())
                        );
                        ipv6->setAddress(od.addr);
                        ipv6->setNetmask(InetAddr());
                    }

                } else
                {
                    if (fwbdebug)
                    {
                        map<int,InterfaceData>::iterator i;
                        for (i=od.interfaces.begin(); i!=od.interfaces.end(); ++i)
                        {
                            InterfaceData *intf = &(i->second);
                            QString str("Discovered interface %1: %2");
                            qDebug() <<
                                str.arg(intf->name.c_str()).arg(intf->mac_addr.c_str());

                        }
                    }

                    list<InterfaceData*> interface_tree;
                    std::unique_ptr<interfaceProperties> int_prop(
                        interfacePropertiesObjectFactory::getInterfacePropertiesObject(o));
                    int_prop->rearrangeInterfaces(od.interfaces, interface_tree);

                    if (interface_tree.size() != od.interfaces.size())
                    {
                        // Some interfaces have been converted to subinterfaces
                        // Show warning

                        QMessageBox::warning(
                            this, "Firewall Builder",
                            tr(
"Some discovered interfaces have been rearranged in "
"fwbuilder objects and recreated as subinterfaces to "
"reflect VLANs, bonding and bridging configurations. "
"The algorithm used to guess correct relationship "
"between interfaces and subinterfaces is imperfect "
"because of the limited information provided by SNMP "
"daemon. Pelase review created objects to make sure "
"generated configuration is accurate. "
"\n"
"\n"
"The program expects MAC addresses of bonding, bridge "
"and vlan interfaces to be the same. It is especially "
"important to review and fix generated objects if you "
"use MAC address spoofing."
),
                            tr("&Continue"), 0, 0,
                            0 );


                    }

                    list<InterfaceData*>::iterator it;
                    for (it=interface_tree.begin(); it!=interface_tree.end(); ++it)
                    {
                        InterfaceData *in = *it;
                        // if this interface has subinterfaces, add even if it
                        // has no ip address (last arg)

                        FWObject *intf = addInterface(
                            o, in, in->subinterfaces.size()!=0);
                        if (intf == nullptr) continue;

                        list<InterfaceData*>::iterator sit;
                        for (sit=in->subinterfaces.begin();
                             sit!=in->subinterfaces.end(); ++sit)
                        {
                            InterfaceData *subint = *sit;
                            addInterface(intf, subint, true);
                        }
                    }
                }

                if (!od.descr.empty())
                {
                    FWOptions* opt=(dynamic_cast<Host*>(o))->getOptionsObject();
                    opt->setStr("snmp_description",od.descr);
                    opt->setStr("snmp_location",   od.location);
                    opt->setStr("snmp_contact",    od.contact);
                }

                mw->moveObject(m_dialog->libs->currentText(), o);

            } else if (type==Network::TYPENAME)
            {
                Network *net=dynamic_cast<Network*>(
                    mw->createObject(type.c_str(),name.c_str())
                );
                assert(net!=nullptr);
                net->setName(name);
                net->setAddress(InetAddr(a));
                net->setNetmask(InetAddr(InetAddr(a)));
                mw->moveObject(m_dialog->libs->currentText(), net);
            } else if (type==IPv4::TYPENAME)
            {
                IPv4 *obj=dynamic_cast<IPv4*>(
                    mw->createObject(type.c_str(),name.c_str())
                );
                assert(obj!=nullptr);
                obj->setName(name);
                obj->setAddress(InetAddr(a));
                obj->setNetmask(InetAddr(InetAddr::getAllOnes()));
                mw->moveObject(m_dialog->libs->currentText(), obj);
            }
        }
        m_dialog->lastprogress->setValue(t++);
        qApp->processEvents();
    }
    m_dialog->lastprogress->setValue(Objects.size());

    ProjectPanel *pp = mw->activeProject();
    QString filename = pp->getFileName();

    QCoreApplication::postEvent(mw, new reloadObjectTreeEvent(filename));

}

void DiscoveryDruid::importPlatformChanged(int cp)
{
    if (fwbdebug)
        qDebug("DiscoveryDruid::importPlatformChanged(): %d",cp);

    switch (cp)
    {
    case IMPORT_IOS:
        m_dialog->import_text->setText(
            QObject::tr("Firewall Builder can import Cisco IOS access lists "
                        "from the router configuration saved using 'show run' "
                        "or any other command that saves running config. The name "
                        "of the created firewall object, all of its interfaces "
                        "and their addresses will be configured automatically if "
                        "this information can be found in the configuration file."
            )
        );
        break;

    case IMPORT_PIX:
        m_dialog->import_text->setText(
            QObject::tr("Firewall Builder can import Cisco PIX and ASA "
                        "configuration saved with 'show run' command.  "
                        "The name of the created firewall object, all of "
                        "its interfaces and their addresses will be "
                        "configured automatically if this information can "
                        "be found in the configuration file."
            )
        );
        break;

    case IMPORT_IPT:
        m_dialog->import_text->setText(
            QObject::tr("Firewall Builder can import iptables rules "
                        "from a file in iptables-save format. Firewall "
                        "name and addresses of its interfaces need "
                        "to be configured manually because iptables-save "
                        "file does not have this information. "
            )
        );
        break;
            
    }
    
}

//----------------------------------------------------------------------
ObjectDescriptor::ObjectDescriptor() {}

ObjectDescriptor::ObjectDescriptor(const ObjectDescriptor& od) {
    have_snmpd       = od.have_snmpd;
    descr            = od.descr;
    contact          = od.contact;
    location         = od.location;
    sysname          = od.sysname;
    interfaces       = od.interfaces;
    MAC_addr         = od.MAC_addr;
    dns_info.name    = od.dns_info.name;
    dns_info.aliases = od.dns_info.aliases;
    addr             = od.addr;
    type             = od.type;
    isSelected       = od.isSelected;
    netmask          = od.netmask;

}

#ifdef HAVE_LIBSNMP
ObjectDescriptor::ObjectDescriptor(const libfwbuilder::CrawlerFind& cf) {
    have_snmpd       = cf.have_snmpd;
    descr            = cf.descr;
    contact          = cf.contact;
    location         = cf.location;
    sysname          = cf.sysname;
    interfaces       = cf.interfaces;
    MAC_addr         = cf.found_phys_addr;
    dns_info.name    = cf.name;
    dns_info.aliases = cf.aliases;
}
#endif

ObjectDescriptor::~ObjectDescriptor()   {};

ObjectDescriptor& ObjectDescriptor::operator=(const ObjectDescriptor& od) {
    have_snmpd       = od.have_snmpd;
    descr            = od.descr;
    contact          = od.contact;
    location         = od.location;
    sysname          = od.sysname;
    interfaces       = od.interfaces;
    MAC_addr         = od.MAC_addr;
    dns_info.name    = od.dns_info.name;
    dns_info.aliases = od.dns_info.aliases;
    addr             = od.addr;
    type             = od.type;
    isSelected       = od.isSelected;
    netmask          = od.netmask;

    return *this;
}

// ================================================================

WorkerThread::WorkerThread() : QThread()
{
    Log = new QueueLogger();
}

WorkerThread::~WorkerThread()
{
    delete Log;
}

void WorkerThread::setProgress(int p)
{
   ProgressEvent *event = new ProgressEvent();
   event->value=p;

   QApplication::postEvent(Widget,event);
}

void WorkerThread::done()
{
   DoneEvent *event=new DoneEvent();

   QApplication::postEvent(Widget,event);
}

QString WorkerThread::getError()
{
    return last_error;
}

void WorkerThread::run()
{
    done();
}

// ================================================================

HostsFileImport::HostsFileImport(const QString &f) :
    WorkerThread()
{
    file_name = f;
}

void HostsFileImport::run()
{
    *Log << "Discovery method: "
         << "Read file in hosts format. \n";

    map<InetAddr, vector<string> > reverse_hosts;
    HostsFile *hf;
/*
 *    read hosts file here
 */
    hf=new HostsFile();
    last_error="";
    setProgress(10);

    *Log << "Parsing file: " << file_name.toLatin1().constData() << "\n";
    if (!file_name.isEmpty())
    {
        try
        {
            hf->parse( file_name.toLatin1().constData() );
        } catch ( FWException &ex )
        {
            last_error = ex.toString().c_str();
            *Log << "Exception: " << last_error.toLatin1().constData() << "\n";
            delete hf;
            done();
            return;
        }
        reverse_hosts=hf->getAll();
        delete hf;

        setProgress(50);
        *Log << "Loading the list ...\n";
    /*
    *    convert map format
    */
        hosts.clear();

        map<InetAddr,vector<string> >::iterator i;
        int count=reverse_hosts.size();
        int t=0;
        for (i=reverse_hosts.begin(); i!=reverse_hosts.end(); ++i)
        {

            ObjectDescriptor od;
            od.addr    = (*i).first;
            od.sysname = ((*i).second).front();

            hosts.push_back( od );

            setProgress(50+(t++)*50/count);
        }
    }
    *Log << "done.\n";
    setProgress(100);

    done();
}

// ================================================================

ConfigImport::ConfigImport(string *b, const std::string &p, const std::string &fwname) : WorkerThread()
{
    buffer = b;
    platform = p;
    this->fwname = fwname;
}

ConfigImport::~ConfigImport()
{
    if (imp)  delete imp;
    if (buffer) delete buffer;
}

void ConfigImport::run()
{
    *Log << "Discovery method: Import firewall configuration.\n";

    std::istringstream instream(*buffer);

    imp = nullptr;
    if (platform == "iosacl") imp = new IOSImporter(mw->getCurrentLib(),
                                                    instream,
                                                    Log, fwname);
    if (platform == "iptables") imp = new IPTImporter(mw->getCurrentLib(),
                                                      instream,
                                                      Log, fwname);
    if (platform == "pix") imp = new PIXImporter(mw->getCurrentLib(),
                                                 instream,
                                                 Log, fwname);

    // add other platforms here when available

    if (imp)
    {
        try
        {
            imp->run();
        } catch(ImporterException &e)
        {
            last_error = e.toString().c_str();
            *Log << e.toString() << "\n";
        }

    } else
    {
        *Log << "Can not import configuration for choosen platform\n";
    }

    done();
}

void DiscoveryDruid::objNameChanged(QString)
{
    m_dialog->nextButton->setEnabled(!(m_dialog->obj_name->text().isEmpty()
                                       || m_dialog->import_filename->text().isEmpty()));
    m_dialog->nextButton->setDefault(true);
}

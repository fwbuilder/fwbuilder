/* 

                          Firewall Builder

                 Copyright (C) 2005, 2006 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org
           Illiya Yalovoy    <yalovoy@gmail.com>

  $Id: DiscoveryDruid.cpp,v 1.37 2007/06/13 02:58:48 vkurland Exp $

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

#include "DiscoveryDruid.h"

#include <iostream>
#include <map>
#include <vector>
#include <set>

#include "fwbuilder/HostsFile.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/InetAddrMask.h"
#include "fwbuilder/Firewall.h"

#include "fwbuilder/dns.h"
#include "fwbuilder/snmp.h"
#include "fwbuilder/Logger.h"

#include "FWBSettings.h"
#include "ObjectManipulator.h"
#include "FWWindow.h"

#include "IOSImporter.h"
#include "IPTImporter.h"

using namespace std;
using namespace libfwbuilder;

DiscoveryDruid::DiscoveryDruid(QWidget *parent, bool start_with_import) :
    QDialog(parent)
{
    m_dialog = new Ui::DiscoveryDruid_q;
    m_dialog->setupUi(this);
    
    setControlWidgets(this, m_dialog->stackedWidget, 
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);
    
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
    
    thread = NULL;
    
    timer = new QTimer(this);
    prg_timer = new QTimer(this);
    unBar = NULL;
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

    fillLibraries(m_dialog->libs,mw->db());
    m_dialog->libs->setEditable(true);
    m_dialog->libs->lineEdit()->setText(mw->getCurrentLib()->getName().c_str());

    m_dialog->DNSprogress->hide();
    m_dialog->DNSprogress_2->hide();

#ifndef HAVE_GOODLIBRESOLV
    m_dialog->dm_importdns->hide();
    m_dialog->snmpdnsparameters->hide();
#endif
    
#ifndef HAVE_LIBSNMP
    m_dialog->dm_usesnmp->setEnabled(false);
#endif

#ifndef HAVE_ANTLR_RUNTIME
    m_dialog->dm_import_config->setEnabled(false);
#endif

    restore();

    importPlatformChanged(m_dialog->import_platform->currentIndex());

#ifdef HAVE_ANTLR_RUNTIME
    if (start_with_import)
    {
        m_dialog->dm_import_config->setDown(true);
        setDiscoveryMethod_Import();
        setAppropriate( 0, false );
        // show the first page of the "import policy" track of the wizard
        showPage( 2 );
        cancelButton->show();
    }
#endif

    showPage(0);
    setNextEnabled(0, true);
    prg_timer->start(100);
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
const char * DISCOVERY_DRUID_SNMPINCLUDEVIRT="SNMPIncludeVirt";
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
    m_dialog->snmpincludevirt->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINCLUDEVIRT));
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
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINCLUDEVIRT,
            m_dialog->snmpincludevirt->isChecked());
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
    switch (page)
    {
    
    case 1: // Reading file in hosts format 
    {
        setNextEnabled(page,false);
        changedHostsFileName();
        m_dialog->filename->setFocus();
        break;
    }

    case 2: // import config
    {
        m_dialog->import_filename->setFocus();
        setBackEnabled(page,true);
        setFinishEnabled(page,false);
        break;
    }

    case 3: // Import DNS zone
    {
        changedDomainName();
        m_dialog->domainname->setFocus();
        //setNextEnabled(page,false);
        break;
    }

    case 4: // Name server
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

    case 5: // Network discovery using SNMP
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

    case 6: // Network scan options
    {
        m_dialog->snmprecursive->setFocus();
        //setNextEnabled(page,false);
        break;
    }

    case 7: // SNMP and DNS reverse lookup queries parameters
    {
        checkSNMPCommunity();
        m_dialog->snmpcommunity->setFocus();
        break;
    }

    case 8: // Background process (import from hosts and from config file)
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

    case 9: // Networks
    {
        fillListOfNetworks();
        fillNetworks();
        backButton->setEnabled(false);
        nextButton->setEnabled(m_dialog->networklist->count ()>0 || Objects.size()>0);
        break;
    }

    case 10: // Objects
    {
        if (Networks.size()==0)
            setBackEnabled(page,false);
            
        fillListOfObjects();
        fillObjects();
        nextButton->setEnabled(m_dialog->objectlist->count ()>0 || m_dialog->networklist->count()>0);
        break;
    }

    case 11: // Adjust Object type
    {
        setBackEnabled(page,true);
        fillTypeChangingList();
        break;
    }

    case 12: // Target library
    {
        break;
    }

    case 13: // Objects creation ...
    {
        setBackEnabled(page,false);
        cancelButton->hide();
        createRealObjects();
        setFinishEnabled(page,true);
        finishButton->setFocus();
        break;
    }
        
    default : {}
    
    }
    FromPage=page;
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
    QString dir;
    dir=st->getWDir();
    if (dir.isEmpty())  dir=st->getOpenFileDir();
    if (dir.isEmpty())  dir="~";
    
    QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    dir,
                    "All files (*.*)");
    
    if (!s.isEmpty())
    {
        m_dialog->filename->setText(s);
    }
    
}

void DiscoveryDruid::browseForImport()
{
    QString dir;
    dir=st->getWDir();
    if (dir.isEmpty())  dir=st->getOpenFileDir();
    if (dir.isEmpty())  dir="~";
    
    QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    dir,
                    "All files (*.*)");
    
    if (!s.isEmpty())
    {
        m_dialog->import_filename->setText(s);
    }
    
}

void DiscoveryDruid::updatePrg()
{
    if (unBar!=NULL)
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
    current_task=BT_HOSTS;
    m_dialog->processname->setText(tr("Hosts file parsing ..."));
    for (int i=0;i<WIZARD_PAGES;i++)
    {
        setAppropriate( i, WIZARD_FILE_PAGES[i]);
    }
}

void DiscoveryDruid::setDiscoveryMethod_DNS()
{
    m_dialog->processname->setText(tr("DNS zone transfer ..."));
    current_task=BT_DNS;
    for (int i=0;i<WIZARD_PAGES;i++)
    {
        setAppropriate( i, WIZARD_DNS_PAGES[i]);
    }
}

void DiscoveryDruid::setDiscoveryMethod_SNMP()
{
    m_dialog->processname->setText(tr("Network discovery using SNMP ..."));
    current_task=BT_SNMP;
    for (int i=0;i<WIZARD_PAGES;i++)
    {
        setAppropriate( i, WIZARD_SNMP_PAGES[i]);
    }    
}

void DiscoveryDruid::setDiscoveryMethod_Import()
{
    m_dialog->processname->setText(tr("Import configuration from file ..."));
    current_task=BT_IMPORT;
    for (int i=0;i<WIZARD_PAGES;i++)
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
    if (thread!=NULL)
    {
        delete thread;
    }
    
    thread = new HostsFileImport(m_dialog->filename->text());
    thread->setTargetWidget(this);
    thread->start();
}

void DiscoveryDruid::startConfigImport()
{
    if (thread!=NULL)
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
        string platform = "";
        switch (m_dialog->import_platform->currentIndex())
        {
        case 0: platform = "iosacl"; break;
        case 1: platform = "iptables"; break;
        }

        //
        // ConfigImport "owns" buffer - it is deleted
        // in destructor of ConfigImport
        //
        thread = new ConfigImport(buffer, platform);
        thread->setTargetWidget(this);
        thread->start();
    } else
    {
        QMessageBox::critical(this, tr("Discovery error"),
          tr("Could not open file %1").arg(m_dialog->import_filename->text()));
        setBackEnabled(currentPage(),true);
    }
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
        qDebug(
            QString("DiscoveryDruid::getSeedHostAddress(): Seed host name %1").
            arg(m_dialog->seedhostname->text()).toLatin1().constData());

    libfwbuilder::InetAddr   seed_host_addr;
    if (!m_dialog->seedhostname->text().isEmpty())
    {
        try 
        {
            QString a = getAddrByName( m_dialog->seedhostname->text() );
            if (fwbdebug)
                qDebug(
                    QString("DiscoveryDruid::getSeedHostAddress() address: %1").
                       arg(a).toLatin1().constData());

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
               bop,(bop!=NULL)?bop->isRunning():-1);

    if (bop!=NULL && bop->isRunning())
    {
        m_dialog->discoverylog->append("Terminating task. Please wait...");
        
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

void DiscoveryDruid::fillNetworks()
{
    ObjectDescriptor buf;

    m_dialog->networklist->clear();
    bool f=false;
    QMap<QString,ObjectDescriptor >::iterator i;
    for(i=Networks.begin();
        i!=Networks.end();
        ++i)
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
    assert(himport!=NULL);
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
    assert(confimp!=NULL);
    Importer *imp = confimp->getImporterObject();
    if (imp!=NULL)
    {
        Firewall *fw = imp->finalize();

        mw->loadDataFromFw(fw);
    }
}

void DiscoveryDruid::saveScanLog()
{
    QString dir;
    dir=st->getWDir();
    if (dir.isEmpty())  dir=st->getOpenFileDir();
    if (dir.isEmpty())  dir="~";
    
    QString s = QFileDialog::getSaveFileName(
                    this,
                    "Choose a file",
                    dir,
                    "Text file (*.txt)");
    
    
    if (!s.isEmpty())
    {
        if (s.endsWith(".txt"))
        {
            s+=".txt";
        }
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
            if (fwbdebug) qDebug("%s",txt.toAscii().constData());
            if (fwbdebug)
                qDebug("--------------------------------");
            f.close();
        }
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
            //TODO: to do something usefull
        }
    }
    libfwbuilder::SNMPCrawler *q = new SNMPCrawler();
    q->init(getSeedHostAddress(),
            m_dialog->snmpcommunity->text().toLatin1().constData(),
            m_dialog->snmprecursive->isChecked(),
            ! m_dialog->snmpincludevirt->isChecked(),
            false,
            m_dialog->snmpfollowp2p->isChecked(),
            0,
            m_dialog->snmpretries->value(),
            1000000L*m_dialog->snmptimeout->value(),
            0,
            0,
            (use_incl) ? &include_networks : NULL);
    
    m_dialog->discoveryprogress->setMaximum(0);
    unBar = m_dialog->discoveryprogress;
    
    bop=q;
    try
    {
        logger = bop->start_operation();
        if (fwbdebug) logger->copyToStderr();
        m_dialog->discoverylog->append("Collecting data ...");

        disconnect(timer, SIGNAL(timeout()), 0, 0);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateLog()));
        timer->setSingleShot(false);
        timer->start(100);
        
    } catch(const FWException &ex)
    {
        delete q;
        q=NULL;
    }
            
    
#endif
}
    
void DiscoveryDruid::loadDataFromCrawler()
{
#ifdef HAVE_LIBSNMP
    SNMPCrawler *q=(SNMPCrawler*)bop;
    Objects.clear();
    Networks.clear();
    
    set<InetAddrMask>::iterator m;
    set<InetAddrMask> s = q->getNetworks();

    if (fwbdebug)
        qDebug(QString("got %1 networks").arg(s.size()).toAscii().constData());

    for (m=s.begin(); m!=s.end(); ++m)
    {
        ObjectDescriptor od;
        
        od.sysname = m->toString();
        od.addr = *(m->getAddressPtr());
        od.netmask = *(m->getNetmaskPtr());
        od.type = Network::TYPENAME;
        od.isSelected = false;
        
        Networks[od.sysname.c_str()]= od ;
    }

    map<InetAddr, CrawlerFind>  t = q->getAllIPs();

    if (fwbdebug)
        qDebug(QString("got %1 addresses").arg(t.size()).toAscii().constData());

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

            QString buf;

            buf = QString(od.addr.toString().c_str()) + " : " + od.sysname.c_str();
            m_dialog->discoverylog->append(buf);

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
/*
    (arg==0) ? 
    _("Network scan completed, click 'Next' to continue") : 
    _("There has been an error running the network scan. You can continue but data gathered by the scanner may be incomplete") 
*/
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
        for(i=Networks.begin();
            i!=Networks.end();
            ++i)
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
        thread=NULL;
        
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
            setFinishEnabled(currentPage(),true);
            finishButton->setFocus();
            break;
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
        if (thread!=NULL)
        {
            while(thread->Log->ready())
            {
                buf = thread->Log->getLine().c_str();
                m_dialog->discoverylog->insertPlainText(buf);
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
            if (ex!=NULL)
            {
                QMessageBox::critical(this,tr("Discovery error"), ex->toString().c_str());
                //m_dialog->discoverylog->append(QString("\nLast exception: ")+ex->toString().c_str()+"\n");    
            }
            if (Objects.size()>0 || Networks.size()>0)
            {
                if (Networks.size()==0) 
                    setAppropriate( 8,0);
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
            bop=NULL;
            unBar=NULL;
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
            if (ex!=NULL)
            {    
                QMessageBox::critical(this,tr("Discovery error"), ex->toString().c_str());
                //m_dialog->discoverylog->append(QString("\nLast exception: ")+ex->toString().c_str()+"\n");    
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
            bop=NULL;
            unBar=NULL;
            m_dialog->discoveryprogress->setMaximum(100);
            m_dialog->discoveryprogress->setValue(100);
            m_dialog->discoveryStopButton->setEnabled(false);
       }
    }
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
                         bop,(bop!=NULL)?bop->isRunning():-1);

    
    fl=false;
    while( logger->ready() ) 
    {
        buf= logger->getLine().c_str();
        if (buf.endsWith('\n'))
            buf = buf.left(buf.length() - 1);

        m_dialog->discoverylog->append(buf);

        /*if (fwbdebug) qDebug("monitorOperation  appending the following buf: (1)");
        if (fwbdebug) qDebug(buf.toAscii().constData());
        if (fwbdebug) qDebug("----------------------------------------------");*/

        fl=true;
    }
    if (fl)
    {
        return 1;
    }
    if (bop==NULL)
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

        m_dialog->discoverylog->append(buf);

        /*if (fwbdebug) qDebug("monitorOperation  appending the following buf: (2)");
        if (fwbdebug) qDebug(buf.toAscii().constData());
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

void DiscoveryDruid::createRealObjects()
{
    
    ObjectDescriptor od;
    string type,name,a;
    
    int t=0;
    m_dialog->lastprogress->setValue(0);
    m_dialog->lastprogress->setMaximum( Objects.size());
    
    QMap<QString,ObjectDescriptor >::iterator i;
    for(i=Networks.begin(); i!=Networks.end(); ++i)
    {
        od=i.value();
        if (od.isSelected)
        {
            type = od.type;
            name=od.sysname;
            a = od.addr.toString().c_str();
            
            Network *net=dynamic_cast<Network*>(
                mw->createObject(type.c_str(),name.c_str())
            );
            assert(net!=NULL);
            net->setName(name);
            net->setAddress(InetAddr(a));
            net->setNetmask(InetAddr(InetAddr(a)));
            mw->moveObject(m_dialog->libs->currentText(), net);
        }
    }
    
    for(i=Objects.begin(); i!=Objects.end(); ++i)
    {
        od = i.value();
        type = od.type;
        
        name = od.sysname;
        a = od.addr.toString();

        if(od.isSelected)
        {
            if (type==Host::TYPENAME || type==Firewall::TYPENAME) 
            {
                FWObject *o=NULL;

                o = mw->createObject(type.c_str(), name.c_str());
                o->setName(name);

                if (od.interfaces.size()==0) 
                {
                    Interface *itf= Interface::cast(
                        mw->createObject(o,Interface::TYPENAME,"nic1")
                    );
                    IPv4 *ipv4= IPv4::cast(
                        mw->createObject(itf,IPv4::TYPENAME,a.c_str())
                    );
                    
                    
                    ipv4->setAddress(InetAddr(a));
                    ipv4->setNetmask(InetAddr());
                } else
                {
                    map<int,InterfaceData>::const_iterator i;
                    for (i=od.interfaces.begin(); i!=od.interfaces.end(); ++i)
                    {
                        InterfaceData in = i->second;
                        if (in.addr_mask.getAddressPtr()->isAny()) continue;

                        Interface *itf = Interface::cast(
                            mw->createObject(o,
                                             QString(Interface::TYPENAME),
                                             QString(i->second.name.c_str())));

                        itf->setPhysicalAddress(in.mac_addr);
                        itf->setLabel(in.label);
                        itf->setExt(in.ext);
                        itf->setSecurityLevel(in.securityLevel);

                        const InetAddr *addr = in.addr_mask.getAddressPtr();
                        IPv4 *ipv4= IPv4::cast(
                            mw->createObject(itf, IPv4::TYPENAME,
                                             addr->toString().c_str())
                        );
                        ipv4->setAddress(*addr);
                        ipv4->setNetmask(*(in.addr_mask.getNetmaskPtr()));

                        mw->autorename(itf, IPv4::TYPENAME, "ip");
                        mw->autorename(itf, physAddress::TYPENAME, "mac");
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
                if (type==Firewall::TYPENAME)
                {
                    map<string,string> platforms = Resources::getPlatforms();
                    map<string,string>::iterator i;
                    for (i=platforms.begin(); i!=platforms.end(); i++)
                        Resources::setDefaultTargetOptions( i->first,
                                                            Firewall::cast(o) );

                    map<string,string> OSs = Resources::getOS();
                    for (i=OSs.begin(); i!=OSs.end(); i++)
                        Resources::setDefaultTargetOptions( i->first,
                                                            Firewall::cast(o) );

                }
            }else if (type==Network::TYPENAME)
            {
                Network *net=dynamic_cast<Network*>(
                    mw->createObject(type.c_str(),name.c_str())
                );
                assert(net!=NULL);
                net->setName(name);
                net->setAddress(InetAddr(a));
                net->setNetmask(InetAddr(InetAddr(a)));
                mw->moveObject(m_dialog->libs->currentText(), net);
            }else if (type==IPv4::TYPENAME)
            {
                IPv4 *obj=dynamic_cast<IPv4*>(
                    mw->createObject(type.c_str(),name.c_str())
                );
                assert(obj!=NULL);
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
}

void DiscoveryDruid::autorename(FWObject *obj,
                                const string &objtype,
                                const string &namesuffix)
{
    FWObject      *hst = obj->getParent();
    list<FWObject*> ol = obj->getByType(objtype);
    int           sfxn = 1;

    for (list<FWObject*>::iterator j=ol.begin(); j!=ol.end(); ++j,++sfxn)
    {
        QString sfx;
        if (ol.size()==1) sfx="";
        else              sfx.setNum(sfxn);
        QString nn = QString("%1:%2:%3%4")
            .arg(QString::fromUtf8(hst->getName().c_str()))
            .arg(QString::fromUtf8(obj->getName().c_str()))
            .arg(namesuffix.c_str())
            .arg(sfx);

        (*j)->setName(string(nn.toUtf8().constData()));
    }
    ol.clear();
}

void DiscoveryDruid::importPlatformChanged(int cp)
{
    if (fwbdebug)
        qDebug("DiscoveryDruid::importPlatformChanged(): %d",cp);

    switch (cp)
    {
    case 0:
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
    case 1:
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
    *Log << "Discovery method:"
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
            hf->parse( file_name.toAscii().constData() );
        } catch ( FWException &ex ) 
        {
            last_error = ex.toString().c_str();
            *Log << "Exception: " << last_error.toAscii().constData() << "\n";
            
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

ConfigImport::ConfigImport(string *b, const std::string &p) : WorkerThread()
{
    buffer = b;
    platform = p;
}

ConfigImport::~ConfigImport()
{
    if (imp)  delete imp;
    if (buffer) delete buffer;
}

void ConfigImport::run()
{
    *Log << "Discovery method:"
         << "Import firewall configuration. \n";

    std::istringstream instream(*buffer);
    imp = NULL;
    if (platform == "iosacl") imp = new IOSImporter(mw->getCurrentLib(),
                                                    instream,
                                                    Log);
    if (platform == "iptables") imp = new IPTImporter(mw->getCurrentLib(),
                                                      instream,
                                                      Log);

    // add other platforms here when available

    if (imp)
    {
        try
        {
            imp->run();
        } catch(ImporterException &e)
        {
            last_error = e.toString().c_str();
            *Log << "Parser error:\n";
            *Log << e.toString() << "\n";
        }

    } else
    {
        *Log << "Can not import configuration for choosen platform\n";
    }
    
    done();
}

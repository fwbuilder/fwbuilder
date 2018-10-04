/*

                          Firewall Builder

                 Copyright (C) 2003-2011 NetCitadel, LLC

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
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include "newFirewallDialog.h"
#include "ObjConflictResolutionDialog.h"
#include "upgradePredicate.h"
#include "FWBSettings.h"
#include "FWBTree.h"
#include "events.h"
#include "FWBApplication.h"
#include "QDesktopWidget"
#include "networkZoneManager.h"
#include "ObjConflictResolutionDialog.h"

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/BackgroundOp.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Constants.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTextBrowser>
#include <QMessageBox>
#include <QTimer>
#include <QListWidget>
#include <QApplication>
#include <QCursor>
#include <QPixmapCache>
#include <QFileDialog>
#include <QDir>
#include <QtDebug>

#include <algorithm>
#include <iostream>

// must be the last for win
#include "fwbuilder/snmp.h"

using namespace libfwbuilder;
using namespace std;


#define NAME_AND_PLATFORM_PAGE 0
#define INTERFACES_MANUAL_OR_SNMP 1
#define CONFIGURE_INTERFACES_MANUALLY 2
#define CONFIGURE_SECURITY_LEVELS 3
#define CONFIGURE_NETWORK_ZONES 4
#define CHOOSE_FW_TEMPLATE 5
#define CONFIGURE_TEMPLATE_INTERFACES_MANUALLY 6


newFirewallDialog::newFirewallDialog(QWidget *parentw, FWObject *_p) :
    QDialog(parentw)
{

    db_orig = _p->getRoot();
    db_copy = new FWObjectDatabase();
    db_copy->duplicate(db_orig, false);

    parent = db_copy->getById(_p->getId(), true);

    m_dialog = new Ui::newFirewallDialog_q;
    m_dialog->setupUi(this);

    possible_inside_interface_labels.push_back("inside");
    possible_inside_interface_labels.push_back("GREEN");
    possible_inside_interface_labels.push_back("green");

    possible_outside_interface_labels.push_back("outside");
    possible_outside_interface_labels.push_back("RED");
    possible_outside_interface_labels.push_back("red");

    possible_dmz_interface_labels.push_back("dmz");
    possible_dmz_interface_labels.push_back("ORANGE");
    possible_dmz_interface_labels.push_back("orange");


    setControlWidgets(this, m_dialog->stackedWidget,
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);

    nfw = nullptr;
    tmpldb = nullptr;
    snmpPollCompleted = false;
    q = nullptr;
    unloadTemplatesLib = false;
    getInterfacesBusy = false;

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(monitor()));
    connect( m_dialog->selectCustomTemplateLib, SIGNAL(pressed()),
             this, SLOT(browseTemplate()));
    connect( m_dialog->useStandard, SIGNAL(toggled(bool)),
             this, SLOT(updateTemplatePanel()));
    connect( m_dialog->useTemplate, SIGNAL(released()),
             this, SLOT(updateTemplatePanel()));

    m_dialog->useStandard->setChecked(true);
    m_dialog->templateFilePath->setText(
        Constants::getTemplatesObjectsFilePath().c_str());
    updateTemplatePanel();

    /* fill in platform. Since iptables is the most popular, start with
     * it.
     */
    QString new_fw_platform = st->getNewFirewallPlatform();

    /* if new_fw_platform is empty, the drop-down list will have empty
     * item which will be current. This is so only on the first run of
     * the program because it remembers chosen platform and uses it on
     * subsequent runs.
     */
    setPlatform(m_dialog->platform, new_fw_platform);

    /* fill in host OS  */
    setHostOS(m_dialog->hostOS, readPlatform(m_dialog->platform), "");

    setNextEnabled( NAME_AND_PLATFORM_PAGE, false );

    //m_dialog->iface_sl_list->setAllColumnsShowFocus( true );
    QTimer::singleShot(0, m_dialog->obj_name, SLOT(setFocus()));

    currentTemplate = nullptr;
    this->m_dialog->interfaceEditor1->clear();
    this->m_dialog->interfaceEditor2->clear();
    this->m_dialog->interfaceEditor1->closeTab();
    this->m_dialog->interfaceEditor2->closeTab();//->removeTab(0);

    this->m_dialog->interfaceEditor1->setExplanation(
        tr("Interfaces with the type set to 'Dynamic IP address' get "
           "IP address by means of DHCP or PPP protocol and do not "
           "require an address here. Interfaces with the type set to "
           "'Static IP address' have statically configured IP address "
           "which should be entered on this page. Interface can have "
           "several IPv4 and IPv6 addresses.")
    );

    this->m_dialog->interfaceEditor2->setExplanation(
        tr("Here you can change IP address of the template interface "
           "to match addresses used on your network. "
           "Interface can have several IPv4 and "
           "IPv6 addresses.")
    );

    this->resize(this->width(), this->minimumHeight());
    int maxheight = (int)(app->desktop()->height()*0.9);
    if (this->height() > maxheight)
        this->resize(this->width(), maxheight);

    showPage(NAME_AND_PLATFORM_PAGE);
}

void newFirewallDialog::browseTemplate()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("FWBuilder template files"), st->getOpenFileDir(),
        tr("FWBuilder template files (*.xml *.fwb *.fwl)"));

    if (fileName.isEmpty()) return;
    st->setOpenFileDir(fileName);

    m_dialog->templateFilePath->setText(fileName);
    updateTemplatePanel();
}

void newFirewallDialog::useStandardTemplate()
{
    m_dialog->templateFilePath->setText(
        Constants::getTemplatesObjectsFilePath().c_str());
    updateTemplatePanel();
}

void newFirewallDialog::updateTemplatePanel()
{
    if (st->customTemplatesEnabled() &&
        m_dialog->useTemplate->checkState()==Qt::Checked)
    {
        QString fileName = m_dialog->templateFilePath->text();
        bool using_std = m_dialog->useStandard->isChecked();

        m_dialog->templateGroupBox->setVisible(true);
        m_dialog->templateFilePathLabel->setVisible(true);
        m_dialog->templateFilePath->setVisible(true);
        m_dialog->templateLibExplanation->setVisible(true);

        m_dialog->templateFilePath->setEnabled(!using_std);
        m_dialog->selectCustomTemplateLib->setEnabled(!using_std);

        if (using_std)
        {
            m_dialog->templateFilePath->setText(
                Constants::getTemplatesObjectsFilePath().c_str());
        }
    }
    else
    {
        m_dialog->templateGroupBox->setVisible(false);
    }
}

newFirewallDialog::~newFirewallDialog()
{
    delete m_dialog;
    if (timer!=nullptr) delete timer;
#ifdef HAVE_LIBSNMP
    if (q!=nullptr) delete q;
#endif
    delete db_copy;
}

void newFirewallDialog::changed()
{
    int p = currentPage();

    if (fwbdebug)
        qDebug() << "newFirewallDialog::changed() page=" << p
                 << "use_manual=" << m_dialog->use_manual->isChecked()
                 << "use_snmp=" << m_dialog->use_snmp->isChecked();

    if (p==NAME_AND_PLATFORM_PAGE)
    {
        setNextEnabled(p,
                       !m_dialog->obj_name->text().isEmpty() &&
                       !readPlatform(m_dialog->platform).isEmpty()
        );
        setHostOS(m_dialog->hostOS, readPlatform(m_dialog->platform), "");
        QString host_os = readHostOS(m_dialog->hostOS);
        m_dialog->interfaceEditor1->setHostOS(host_os);
        m_dialog->interfaceEditor2->setHostOS(host_os);
    }

    if (p==INTERFACES_MANUAL_OR_SNMP)
    {

        bool use_snmp = false;

#ifdef HAVE_LIBSNMP
        use_snmp = m_dialog->use_snmp->isChecked();
#else
        use_snmp = false;
        m_dialog->use_snmp->setEnabled( use_snmp );
#endif
        m_dialog->snmpIP->setEnabled( use_snmp );
        m_dialog->snmp_community->setEnabled( use_snmp );
        m_dialog->snmpQuery->setEnabled( use_snmp );
        m_dialog->snmpProgress->setEnabled( use_snmp );
        if ( use_snmp ) m_dialog->snmp_community->setFocus();

        if (use_snmp)
        {
            getIPAddressOfFirewallByName();
        }

        use_snmp = m_dialog->use_manual->isChecked() || snmpPollCompleted;
        setNextEnabled( 1, use_snmp );
    }

    if (fwbdebug)
        qDebug() << "newFirewallDialog::changed() done";
}

void newFirewallDialog::getIPAddressOfFirewallByName()
{
    getInterfacesBusy = true;

    m_dialog->snmpIP->setText("");

    QString name = m_dialog->obj_name->text().toLatin1().constData();
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
    QString addr = getAddrByName(name, AF_INET);
    QApplication::restoreOverrideCursor();

    if (!addr.isEmpty())
        m_dialog->snmpIP->setText(addr);
    else
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Address of %1 could not be obtained via DNS")
            .arg(m_dialog->obj_name->text()),
            "&Continue", QString::null, QString::null, 0, 1 );
    }

    getInterfacesBusy = false;
}

void newFirewallDialog::monitor()
{
    if (logger==nullptr || q==nullptr) return;

#ifdef HAVE_LIBSNMP

    if( logger->ready() )
    {
        QString str = logger->getLine().c_str();
        m_dialog->snmpProgress->moveCursor( QTextCursor::End );
        m_dialog->snmpProgress->insertPlainText( str );
        return;
    }

    if (q->isRunning()) return;

    timer->stop();

    QString platform = readPlatform(m_dialog->platform);

    guessOSAndPlatformFromSysDescr(q->getDescr().c_str(),
                                   discovered_platform,
                                   discovered_host_os,
                                   discovered_version);

    if (fwbdebug)
        qDebug() << "Guessed version as " << discovered_version;


    map<int, InterfaceData>* intf = q->getInterfaces();
    map<int, InterfaceData>::iterator i;
    this->m_dialog->interfaceEditor1->clear();
    this->m_dialog->interfaceEditor1->removeTab(0);
    for (i=intf->begin(); i!=intf->end(); ++i)
    {
        InterfaceData* idata = &(i->second);

        if (fwbdebug)
        {
            qDebug() << "------------------------------------------------";
            qDebug() << "id=" << idata->id.c_str();
            qDebug() << "name=" << idata->name.c_str();
            qDebug() << "snmp_type=" << idata->snmp_type;
            qDebug() << "ostatus=" << idata->ostatus;
            qDebug() << "mac_addr=" << idata->mac_addr.c_str();
            qDebug() << "interface_type=" << idata->interface_type.c_str();
            qDebug() << "";
        }

        /*
         * some special treatment of discovered interfaces for Cisco ASA devices:
         * if mac address is reported as 00:00:00:00:00:00 or
         * 00:00:00:anything, this is usually some kind of internal special
         * interface and we can skip it. Examples: "_internal_loopback",
         * "Internal-Data0/1"
         *
         * This is different from how Linux reports mac address of a
         * loopback because Linux snmpd returns empty string for the
         * loopback mac address.
         *
         * The name of the interface reported by ASA is like this:
         * "Adaptive Security Appliance 'Ethernet0/0' interface"
         *
         * Need to strip all thie verbose description
         */ 

        if (idata->ostatus)
        {
            guessInterfaceLabel(idata);

            if (platform == "pix" || platform == "fwsm")
            {
                if ( ! idata->mac_addr.empty() && idata->snmp_type == 1 &&
                     idata->mac_addr.find("00:00:00")==0) continue;

                QString name = idata->name.c_str();
                name.replace("Adaptive Security Appliance '", "");
                name.replace("Cisco PIX Security Appliance '", "");
                name.replace("PIX Firewall '", "");
                name.replace("' interface", "");
                idata->name = name.toStdString();
            }

            this->m_dialog->interfaceEditor1->addInterfaceFromData(idata);
        }
    }
    if ( this->m_dialog->interfaceEditor1->count() == 0 )
        this->m_dialog->interfaceEditor1->addNewInterface();

    delete q;
    q=nullptr;

#endif

    snmpPollCompleted=true;
    setNextEnabled( INTERFACES_MANUAL_OR_SNMP, true );
}

void newFirewallDialog::getInterfacesViaSNMP()
{
#ifdef HAVE_LIBSNMP

// need to protect from reentry because getAddrByName processes events
    if (q!=nullptr || getInterfacesBusy) return;

    snmpPollCompleted=false;
    m_dialog->interfaceEditor1->clear();

    string rcomm=m_dialog->snmp_community->text().toLatin1().constData();

    if ( rcomm.empty() )
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Missing SNMP community string."),
            "&Continue", QString::null, QString::null, 0, 1 );
        return ;
    }

    getInterfacesBusy = true;

    InetAddr addr;
    try
    {
        addr = InetAddr(m_dialog->snmpIP->text().toStdString());
    }
    catch (FWException &ex)
    {
        try
        {
            QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
            QString a = getAddrByName(m_dialog->snmpIP->text(), AF_INET);
            QApplication::restoreOverrideCursor();
            addr = InetAddr(a.toLatin1().constData());
            getInterfacesBusy = false;
        }
        catch (FWException &ex)
        {
            QMessageBox::warning(
                this,"Firewall Builder",
                tr("Address of %1 could not be obtained via DNS")
                .arg(m_dialog->snmpIP->text()),
                "&Continue", QString::null, QString::null, 0, 1 );
            getInterfacesBusy = false;
            return ;
        }
    }

    logger = nullptr;
    m_dialog->snmpProgress->clear();

    if (q!=nullptr) delete q;

    q = new SNMP_interface_query();
    q->init(addr.toString(), rcomm, SNMP_DEFAULT_RETRIES, SNMP_DEFAULT_TIMEOUT);

    timer->setSingleShot(false);
    timer->start(0);

    try
    {
        logger = q->start_operation();

    } catch(const FWException &ex)
    {
        //do nothing
    }

    getInterfacesBusy = false;

#endif
}

bool interfaceCompare(libfwbuilder::Interface *first,
                      libfwbuilder::Interface *second)
{
    return first->getName() < second->getName();
}

bool newFirewallDialog::appropriate(const int page) const
{
    switch (page)
    {
    case NAME_AND_PLATFORM_PAGE:
    case CHOOSE_FW_TEMPLATE:
        return true;

    case INTERFACES_MANUAL_OR_SNMP:
    case CONFIGURE_INTERFACES_MANUALLY:
    case CONFIGURE_SECURITY_LEVELS:
    case CONFIGURE_NETWORK_ZONES:
        return (!m_dialog->useTemplate->isChecked());
    }
    return true;
}

void newFirewallDialog::nextClicked()
{
    if ( currentPage() == CHOOSE_FW_TEMPLATE )
    {
        if (m_dialog->templateList->currentItem() == nullptr)
        {
            QMessageBox::warning(
                    this,"Firewall Builder",
                    tr("Please select template"),
                    tr("&Continue"), QString::null,QString::null,
                    0, 1 );
            showPage(CHOOSE_FW_TEMPLATE);
            return;
        }
    }

    if ( currentPage() == CONFIGURE_INTERFACES_MANUALLY )
        if ( !this->m_dialog->interfaceEditor1->isValid() )
            return;

    // FakeWizard::nextRelevant() finds next (in the numerical order)
    // page that is permitted by function appropriate() and returns
    // its number
    if (nextRelevant( currentPage() ) > -1)
        showPage(nextRelevant( currentPage() ));
}

void newFirewallDialog::backClicked()
{
    if (previousRelevant( currentPage() ) > -1)
        showPage(previousRelevant( currentPage() ));
}

void newFirewallDialog::showPage(const int page)
{
    FakeWizard::showPage(page);

    int p = page;

    if (fwbdebug)
        qDebug() << "newFirewallDialog::showPage  page=" << page;

// p is a page number _after_ it changed
    switch (p)
    {
    case NAME_AND_PLATFORM_PAGE:
        // we get here if user hits "Back" on page 4 (where they
        // choose template object)
        if (tmpldb!=nullptr)
        {
            m_dialog->templateList->clear();
            delete tmpldb;
            tmpldb = nullptr;
        }
        m_dialog->nextButton->setDefault(true);
        m_dialog->obj_name->setFocus();
        break;

    case INTERFACES_MANUAL_OR_SNMP:
    {
// page 1 is where we choose to configure interfaces manually or via snmp
        m_dialog->snmpIP->setText("");
        changed();  // to properly enable/disable widgets
        m_dialog->nextButton->setDefault(true);
        break;
    }

    case CONFIGURE_INTERFACES_MANUALLY:
    {
        if (!Resources::getTargetCapabilityBool(
                readPlatform(m_dialog->platform).toLatin1().constData(),
                "security_levels") )
        {

/* if chosen fw platform does not support security levels,
 * this is the last page
 */
            setNextEnabled( CONFIGURE_INTERFACES_MANUALLY, false );
            setFinishEnabled( CONFIGURE_INTERFACES_MANUALLY, true );
            m_dialog->finishButton->setDefault(true);
        }
        break;
    }

    case CONFIGURE_SECURITY_LEVELS:
    {
        if (m_dialog->useTemplate->isChecked())
        {
            showPage( NAME_AND_PLATFORM_PAGE );
            return;
        }

        // Edit security levels
        fillInterfaceSLList();

        setNextEnabled( CONFIGURE_SECURITY_LEVELS, true );
        setFinishEnabled( CONFIGURE_SECURITY_LEVELS, false );
        m_dialog->nextButton->setDefault(true);
        break;
    }

    case CONFIGURE_NETWORK_ZONES:
    {
        if (m_dialog->useTemplate->isChecked())
        {
            showPage( NAME_AND_PLATFORM_PAGE );
            return;
        }

        // Edit network zones
        fillInterfaceNZList();

        setNextEnabled(CONFIGURE_NETWORK_ZONES, false );
        setFinishEnabled(CONFIGURE_NETWORK_ZONES, true );
        m_dialog->finishButton->setDefault(true);
        break;
    }

    case CHOOSE_FW_TEMPLATE:
    {
        // Show firewall templates
        setFinishEnabled( CHOOSE_FW_TEMPLATE, false );
        setNextEnabled( CHOOSE_FW_TEMPLATE, true );

        // load templates if not loaded
        if (tmpldb==nullptr)
        {

            MessageBoxUpgradePredicate upgrade_predicate(this);
            tmpldb = new FWObjectDatabase();
            tmpldb->setReadOnly( false );
            try
            {
                tmpldb->load(
                    m_dialog->templateFilePath->text().toLatin1().data(),
                    &upgrade_predicate, Constants::getDTDDirectory());
            }
            catch (FWException &ex)
            {
                QMessageBox::critical(
                    this,"Firewall Builder",
                    tr("Error loading template library:\n%1")
                    .arg(ex.toString().c_str()),
                    tr("&Continue"), QString::null,QString::null,
                    0, 1 );
            }
        }

        // nfw != nullptr if user clicked Back on one of the subsequent
        // pages because we create firewall object when they click
        // Next on the page where they choose template ( see case
        // CONFIGURE_TEMPLATE_INTERFACES_MANUALLY below)
        if (nfw)
        {
            parent->remove(nfw, false);
            delete nfw;
            nfw = nullptr;
        }

        list<FWObject*> fl;

        FWObjectTypedChildIterator libiter = tmpldb->findByType(Library::TYPENAME);
        for ( ; libiter!=libiter.end(); ++libiter)
            findFirewalls(*libiter, fl, false);

        QString icn = ":/Icons/Firewall/icon-tree";

        m_dialog->templateList->clear();

        int n = 0;
        QListWidgetItem *first_template = nullptr;
        for (list<FWObject*>::iterator m=fl.begin(); m!=fl.end(); m++,n++)
        {
            FWObject *o = *m;

/*
  Ticket #1492 requested the change to only show the user templates
  that match platform and host OS they choose on the first page of the
  wizard.

  Unfortunately this does not really work because most templates have
  platform and host OS set to "unknown". Either we have to maintain
  many almost identical templates to provide enough choices for all
  possible platforms, or we should not filter by platform.

            string platform = readPlatform(m_dialog->platform).toLatin1().constData();
            string host_os = readHostOS(m_dialog->hostOS).toLatin1().constData();
            if (o->getStr("platform") != platform || o->getStr("host_OS") != host_os)
                continue;
*/

            QPixmap pm;
            if ( ! QPixmapCache::find( icn, pm) )
            {
                pm.load( icn );
                QPixmapCache::insert( icn, pm);
            }

            QListWidgetItem *twi = new QListWidgetItem;
            twi->setIcon( QIcon(pm) );
            twi->setText( QString(o->getName().c_str()) );

            m_dialog->templateList->addItem( twi );
            templates[ m_dialog->templateList->item(
                    m_dialog->templateList->count()-1 ) ] = o;

            if (first_template == nullptr) first_template = twi;
        }

        m_dialog->templateList->setFocus();
        m_dialog->templateList->setCurrentItem(first_template,
                                               QItemSelectionModel::SelectCurrent);
        m_dialog->finishButton->setDefault(false);
        m_dialog->nextButton->setDefault(true);
        break;
    }

    case CONFIGURE_TEMPLATE_INTERFACES_MANUALLY:
    {
        // Edit interfaces of the template object
        createFirewallFromTemplate();
        setFinishEnabled( CONFIGURE_TEMPLATE_INTERFACES_MANUALLY, true );

        this->m_dialog->interfaceEditor2->clear();
        this->m_dialog->interfaceEditor2->closeTab();//->removeTab(0);
        this->m_dialog->interfaceEditor2->setCornerWidgetsVisible(false);
        QList<Interface*> interfaces;
        FWObjectTypedChildIterator intiter = nfw->findByType(Interface::TYPENAME);
        for ( ; intiter != intiter.end(); ++intiter )
            interfaces.append(Interface::cast(*intiter));
        sort(interfaces.begin(), interfaces.end(), interfaceCompare);
        foreach(Interface* intr, interfaces)
            m_dialog->interfaceEditor2->addInterface(intr);
        m_dialog->finishButton->setDefault(true);
    }
    }
}

void newFirewallDialog::getInterfaceDataFromInterfaceEditor(
    EditedInterfaceData &edata, InterfaceData &idata)
{
    idata.name  = edata.name.toStdString();
    idata.label = edata.label.toStdString();
    AddressInfo address;
    bool gotIPv4 = false;
    foreach(AddressInfo addr, edata.addresses.values())
    {
        if (addr.ipv4)
        {
            address = addr;
            gotIPv4 = true;
            break;
        }
    }

    InetAddrMask *iam;// = new InetAddrMask();
    if (edata.type == 0 && edata.addresses.size() != 0)
    {
        if (!gotIPv4) address = edata.addresses.values().first();
        if ( address.ipv4 )
            iam = new InetAddrMask(
                InetAddr(address.address.toStdString()),
                InetAddr(address.netmask.toStdString()));
        else
        {
            iam = new InetAddrMask(
                InetAddr(AF_INET6, address.address.toStdString()),
                InetAddr(AF_INET6, address.netmask.toStdString()));
        }
        idata.addr_mask.push_back(iam);
    }

    if (gotIPv4)
    {
        try
        {
            guessSecurityLevel(
                readPlatform(m_dialog->platform).toStdString(), &idata);
        }
        catch (FWException &ex)
        {
            QMessageBox::warning(
                this,"Firewall Builder", ex.toString().c_str(),
                "&Continue", QString::null, QString::null, 0, 1 );
            showPage( CONFIGURE_INTERFACES_MANUALLY );
            return;
        }
    }
    else idata.securityLevel = 0;
}

void newFirewallDialog::fillInterfaceSLList()
{
    m_dialog->iface_sl_list->clear();

    QStringList labels;
    labels << QObject::tr("Name") << QObject::tr("Label")
           << QObject::tr("Address") << QObject::tr("Security Level");
    m_dialog->iface_sl_list->setHorizontalHeaderLabels(labels);

    int row = 0;
    foreach(EditedInterfaceData iface,
            this->m_dialog->interfaceEditor1->getData().values() +
            this->m_dialog->interfaceEditor1->getNewData())
    {
        InterfaceData idata;

        getInterfaceDataFromInterfaceEditor(iface, idata);

        m_dialog->iface_sl_list->insertRow(row);

        QTableWidgetItem* itm;

        itm = new QTableWidgetItem(iface.name);
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_sl_list->setItem(row, 0, itm);

        itm = new QTableWidgetItem(iface.label);
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_sl_list->setItem(row, 1, itm);

        QString addr_str;
        if (iface.addresses.size() > 0)
        {
            AddressInfo addr = *(iface.addresses.begin());
            addr_str = addr.address;
        }

        itm = new QTableWidgetItem(addr_str);
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_sl_list->setItem(row, 2, itm);

        //itm = new QTableWidgetItem(QString::number(idata.securityLevel));
        QSpinBox *widget = new QSpinBox();
        widget->setMaximum(100);
        widget->setMinimum(0);
        widget->setValue(idata.securityLevel);
        m_dialog->iface_sl_list->setCellWidget(row, 3, widget);

        row++;
    }
}

void newFirewallDialog::fillInterfaceNZList()
{
    m_dialog->iface_nz_list->clear();

    QStringList labels;
    labels << QObject::tr("Name") << QObject::tr("Label")
           << QObject::tr("Address") << QObject::tr("Network Zone");
    m_dialog->iface_nz_list->setHorizontalHeaderLabels(labels);

    NetworkZoneManager netzone_manager;
    netzone_manager.load(db_copy);

    int row = 0;
    foreach(EditedInterfaceData iface,
            this->m_dialog->interfaceEditor1->getData().values() +
            this->m_dialog->interfaceEditor1->getNewData())
    {
        InterfaceData idata;

        getInterfaceDataFromInterfaceEditor(iface, idata);

        m_dialog->iface_nz_list->insertRow(row);

        QTableWidgetItem* itm;

        itm = new QTableWidgetItem(iface.name);
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_nz_list->setItem(row, 0, itm);

        itm = new QTableWidgetItem(iface.label);
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_nz_list->setItem(row, 1, itm);

        QString addr_str;
        if (iface.addresses.size() > 0)
        {
            AddressInfo addr = *(iface.addresses.begin());
            addr_str = addr.address;
        }

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

/*
 * this slot is connected to currentItemChanged signal of templateList
 * As a side effect, this slot is called when we clear templateList.
 */
void newFirewallDialog::templateSelected(QListWidgetItem *itm)
{
    if (templates.size()==0) return;
    FWObject *o = templates[itm];
    if (o==nullptr) return;
    this->m_dialog->interfaceEditor2->setTemplate(o);
    currentTemplate = o;

    Firewall *fw = Firewall::cast(o);

    m_dialog->templateComment->clear();
    QString s = QString("<a name=\"top\">\n") + fw->getComment().c_str();
    m_dialog->templateComment->append( s );
    m_dialog->templateComment->scrollToAnchor("top");

    bool haveOutside = false;
    bool haveInside  = false;
    bool haveDMZ     = false;
    list<FWObject*> ll = fw->getByType(Interface::TYPENAME);
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        Interface *intf = Interface::cast( *i );
        if (std::find(possible_outside_interface_labels.begin(),
                      possible_outside_interface_labels.end(),
                      intf->getLabel()) != possible_outside_interface_labels.end())
        {
            haveOutside=true;
            m_dialog->intfOutsideLine->show();
            m_dialog->intfOutsideText->show();
            fillInterfaceData(intf,m_dialog->intfOutsideText);
        }

        if (std::find(possible_inside_interface_labels.begin(),
                      possible_inside_interface_labels.end(),
                      intf->getLabel()) != possible_inside_interface_labels.end())
        {
            haveInside=true;
            m_dialog->intfInsideLine->show();
            m_dialog->intfInsideText->show();
            fillInterfaceData(intf,m_dialog->intfInsideText);
        }

        if (std::find(possible_dmz_interface_labels.begin(),
                      possible_dmz_interface_labels.end(),
                      intf->getLabel()) != possible_dmz_interface_labels.end())
        {
            haveDMZ=true;
            m_dialog->intfDMZLine->show();
            m_dialog->intfDMZText->show();
            fillInterfaceData(intf,m_dialog->intfDMZText);
        }
    }

    if (!haveOutside) { m_dialog->intfOutsideLine->hide(); m_dialog->intfOutsideText->hide(); }
    if (!haveInside)  { m_dialog->intfInsideLine->hide();  m_dialog->intfInsideText->hide();  }
    if (!haveDMZ)     { m_dialog->intfDMZLine->hide();     m_dialog->intfDMZText->hide();     }
}

void newFirewallDialog::fillInterfaceData(Interface *intf, QTextBrowser *qte)
{
    qte->clear();
    QString s;

    s += "<table border='0' cellspacing='0' cellpadding='0'>";

    s += "<tr>";
    s += "<td>";
    s +=  tr("Interface: %1 (%2)")
        .arg(intf->getName().c_str())
        .arg(intf->getLabel().c_str());
    s += "</td>";
    s += "</tr>";

    s += "<tr>";
    s += "<td>";
    if (intf->isDyn()) s +=  tr("Dynamic address");
    else
        if (intf->isUnnumbered()) s +=  tr("Unnumbered interface");
        else
        {
            const InetAddr *addr = intf->getAddressPtr();
            QString addr_str = (addr) ? addr->toString().c_str() : "";
            const InetAddr *netm = intf->getNetmaskPtr();
            QString netm_str = (netm) ? netm->toString().c_str() : "";
            s += QString("%1/%2").arg(addr_str).arg(netm_str);
        }
    s += "</td>";
    s += "</tr>";
    s += "</table>";
    qte->setText(s);
}

bool newFirewallDialog::validateAddressAndMask(const QString &addr,
                                           const QString &netm)
{
    try
    {
        InetAddr(addr.toLatin1().constData());
    }
    catch (FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Invalid address '%1/%2'").arg(addr).arg(netm),
            "&Continue", QString::null, QString::null, 0, 1 );
        return false;
    }
    try
    {
        bool ok = false ;
        int ilen = netm.toInt (&ok);
        if (ok)
        {
            if (ilen < 0 || ilen > 32)
            {
                QMessageBox::warning(
                    this,"Firewall Builder",
                    tr("Invalid address '%1/%2'").arg(addr).arg(netm),
                    "&Continue", QString::null, QString::null, 0, 1 );
                return false;
            }
        }
        else
        {
            InetAddr(netm.toLatin1().constData());
        }

    }
    catch (FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Invalid address '%1/%2'").arg(addr).arg(netm),
            "&Continue", QString::null, QString::null, 0, 1 );
        return false;
    }
    return true;
}

void newFirewallDialog::cleanup()
{
    if (nfw)
    {
        parent->remove(nfw, false);
        delete nfw;
        nfw = nullptr;
    }

    if (tmpldb)
    {
        delete tmpldb;
        tmpldb = nullptr;
    }
}

void newFirewallDialog::cancelClicked()
{
    cleanup();
    reject();
}

void newFirewallDialog::finishClicked()
{
    // getting focus to close table cell editor
    // see #1594
    m_dialog->finishButton->setFocus(Qt::OtherFocusReason);

    if (fwbdebug)
        qDebug() << "newFirewallDialog::finishClicked()"
                 << "currentPage()=" << currentPage();

    if ((!this->m_dialog->useTemplate->isChecked()) &&
        currentPage() == CONFIGURE_INTERFACES_MANUALLY)
    {
        if ( !this->m_dialog->interfaceEditor1->isValid() )
            return;
    }

    if ( currentPage() == CONFIGURE_TEMPLATE_INTERFACES_MANUALLY )
        if ( !this->m_dialog->interfaceEditor2->isValid() )
            return;

    string platform = readPlatform(m_dialog->platform).toLatin1().constData();
    string host_os = readHostOS(m_dialog->hostOS).toLatin1().constData();

    st->setNewFirewallPlatform(platform.c_str());

    if (currentPage()==CONFIGURE_INTERFACES_MANUALLY)  fillInterfaceSLList();

    if (currentPage()==CHOOSE_FW_TEMPLATE ||
        currentPage()==CONFIGURE_TEMPLATE_INTERFACES_MANUALLY)
    {
        // Creating from a template
        if (nfw==nullptr) createFirewallFromTemplate();
        else
            changedAddressesInNewFirewall();

    } else
    {
        // Create from interface list (obtained either manually or via snmp)
        if ( !this->m_dialog->interfaceEditor1->isValid() )
            return;

        FWObject *o;
        o = db_copy->create(Firewall::TYPENAME);

        if (o==nullptr)
        {
            QDialog::accept();
            return;
        }

        o->setName( string( m_dialog->obj_name->text().toUtf8().constData() ) );//.toStdString());
        parent->add(o);

        nfw = Firewall::cast(o);

        o->setStr("platform", platform);
        Resources::setDefaultTargetOptions(platform , nfw);

        o->setStr("host_OS", host_os);
        Resources::setDefaultTargetOptions(host_os , nfw);

        if ( ! discovered_version.isEmpty())
            o->setStr("version", discovered_version.toStdString());

/* create interfaces */

        foreach(EditedInterfaceData iface,
                this->m_dialog->interfaceEditor1->getNewData())
        {
            QString name     =  iface.name;
            QString label    =  iface.label;
            bool    dyn      =  iface.type == 1;
            bool    unnum    =  iface.type == 2;
            QString physaddr =  iface.mac;
            int     sec_level = 0;
            string  network_zone_str_id = "";

            QList<QTableWidgetItem*> ltwi =
                m_dialog->iface_sl_list->findItems( name , Qt::MatchExactly );

            if ( ! ltwi.empty())
            {
                QTableWidgetItem *itm2 = ltwi[0];
                assert(itm2!=nullptr);
                int row = itm2->row();
                QSpinBox *sb = dynamic_cast<QSpinBox*>(
                    m_dialog->iface_sl_list->cellWidget(row, 3));
                assert(sb!=nullptr);
                sec_level = sb->value();
            }

            ltwi = m_dialog->iface_nz_list->findItems( name , Qt::MatchExactly );
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

            Interface *oi = Interface::cast(db_copy->create(Interface::TYPENAME));
            assert(oi!=nullptr);

            nfw->add(oi);

            oi->setName( string(name.toUtf8().constData()) );
            oi->setLabel( string(label.toUtf8().constData()) );
            oi->setComment( string(iface.comment.toUtf8().constData()) );

            oi->setDyn(dyn);
            oi->setUnnumbered(unnum);
            oi->setSecurityLevel(sec_level);

            // only set network zone if it is supported and is not empty. See #2014
            if (!network_zone_str_id.empty())
                oi->setStr("network_zone", network_zone_str_id);

            std::unique_ptr<interfaceProperties> int_prop(
                interfacePropertiesObjectFactory::getInterfacePropertiesObject(nfw));
            if (int_prop->looksLikeVlanInterface(name))
            {
                QString base_name;
                int vlan_id;
                int_prop->parseVlan(name, &base_name, &vlan_id);

                oi->getOptionsObject()->setStr("type", "8021q");
                oi->getOptionsObject()->setInt("vlan_id", vlan_id);
            }


            if (iface.type == 0)
            {
                foreach(AddressInfo address, iface.addresses)
                {
                    if (address.address == "0.0.0.0") continue;
                    if (address.ipv4)
                    {
                        string addrname = string( QString("%1:%2:ip").arg(QString(m_dialog->obj_name->text())).arg(name).toUtf8().constData() );
                        IPv4 *oa = IPv4::cast(db_copy->create(IPv4::TYPENAME));
                        oi->add(oa);
                        oa->setName(addrname);
                        oa->setAddress( InetAddr(address.address.toLatin1().constData()) );
                        bool ok = false ;
                        int inetmask = address.netmask.toInt(&ok);
                        if (ok)
                        {
                            oa->setNetmask( InetAddr(inetmask) );
                        }
                        else
                        {
                            oa->setNetmask( InetAddr(address.netmask.toLatin1().constData()) );
                        }
                    }
                    else
                    {
                        string addrname = string ( QString("%1:%2:ip6").arg(QString(m_dialog->obj_name->text())).arg(name).toUtf8().constData() );
                        IPv6 *oa = IPv6::cast(db_copy->create(IPv6::TYPENAME));
                        oi->add(oa);
                        oa->setName(addrname);
                        oa->setAddress(InetAddr(AF_INET6, address.address.toLatin1().constData()) );
                        bool ok = false ;
                        int inetmask = address.netmask.toInt(&ok);
                        if (ok)
                        {
                            oa->setNetmask( InetAddr(AF_INET6, inetmask) );
                        }
                        else
                        {
                            oa->setNetmask(InetAddr(AF_INET6, address.netmask.toLatin1().constData()));
                        }
                    }
                }
            }
        }
    }

    // merge dbcopy into db

    CompareObjectsDialog cod(this);
    db_orig->merge(db_copy, &cod);
    db_orig->fixTree();

    nfw = Firewall::cast(db_orig->findInIndex(nfw->getId()));

    if (tmpldb!=nullptr)
    {
        delete tmpldb;
        tmpldb = nullptr;
    }

    QDialog::accept();
}



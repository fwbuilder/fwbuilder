/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: newFirewallDialog.cpp,v 1.32 2007/05/23 03:05:51 vkurland Exp $

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

#include "config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include "newFirewallDialog.h"
#include "InterfaceData.h"
#include "FWWindow.h"
#include "ObjConflictResolutionDialog.h"
#include "upgradePredicate.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/InterfacePolicy.h"
#include "fwbuilder/BackgroundOp.h"

#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qtreewidget.h>
#include <qtextbrowser.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qlistwidget.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qpixmapcache.h>

#include <iostream>

// must be the last for win
#include "fwbuilder/snmp.h"

using namespace libfwbuilder;
using namespace std;



newFirewallDialog::newFirewallDialog() : QDialog()
{
    m_dialog = new Ui::newFirewallDialog_q;
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

    nfw = NULL;
    tmpldb = NULL;
    snmpPollCompleted = false;
    q = NULL;
    unloadTemplatesLib = false;
    getInterfacesBusy = false;

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(monitor()) );

/* fill in platform */
    setPlatform(m_dialog->platform, "" );

/* fill in host OS  */
    setHostOS(m_dialog->hostOS, "" );

    setNextEnabled( 0, false );
    /*for (int i=0; i<pageCount(); ++i)
        setHelpEnabled( i, false );*/

    //m_dialog->iface_list->setItemMargin( 1 );
    m_dialog->iface_list->setAllColumnsShowFocus( true );

    //m_dialog->iface_sl_list->setItemMargin( 1 );
    m_dialog->iface_sl_list->setAllColumnsShowFocus( true );
    m_dialog->iface_dyn->setToolTip(wordWrap(tr("Check option 'dynamic address' for the interface that gets its IP address dynamically via DHCP or PPP protocol.") ,80 ));
    m_dialog->iface_unnum->setToolTip(wordWrap(tr("Check option 'Unnumbered interface' for the interface that does not have an IP address. Examples of interfaces of this kind are those used to terminate PPPoE or VPN tunnels.") ,80 ));
    m_dialog->obj_name->setFocus();

    showPage(0);
}

newFirewallDialog::~newFirewallDialog()
{
    delete m_dialog;
    if (timer!=NULL) delete timer;
#ifdef HAVE_LIBSNMP
    if (q!=NULL) delete q;
#endif
}

void newFirewallDialog::changed()
{
    int p = currentPage();
    if (p==0)
    {
        setNextEnabled( p, !m_dialog->obj_name->text().isEmpty() );
    }

    if (p==1)
    {

        bool f;

#ifdef HAVE_LIBSNMP
        f = m_dialog->use_snmp->isChecked();
#else
        f = false;
        m_dialog->use_snmp->setEnabled( f );
#endif

        m_dialog->snmp_community->setEnabled( f );
        m_dialog->snmpQuery->setEnabled( f );
        m_dialog->snmpProgress->setEnabled( f );
        if (f) m_dialog->snmp_community->setFocus();

        f = m_dialog->use_manual->isChecked() || snmpPollCompleted;
        setNextEnabled( 1, f );
    }

    if (p==2)
    {
        if (m_dialog->iface_dyn->isChecked() ||
            m_dialog->iface_unnum->isChecked() ||
            m_dialog->iface_bridgeport->isChecked())
        {
            m_dialog->iface_addr->clear();
            m_dialog->iface_addr->setEnabled(false);
            m_dialog->iface_netmask->clear();
            m_dialog->iface_netmask->setEnabled(false);
        } else
        {
            m_dialog->iface_addr->setEnabled(true);
            m_dialog->iface_netmask->setEnabled(true);
        }
    }
}

void  newFirewallDialog::monitor()
{
    if (logger==NULL || q==NULL) return;

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

    const map<int, Interface> &intf = q->getInterfaces();
    for(map<int, Interface>::const_iterator i=intf.begin();i!=intf.end(); ++i)
    {
        if ( i->second.isUp() )
        {
            InterfaceData idata( i->second );

            idata.guessLabel( readPlatform(m_dialog->platform).toLatin1().constData() );

            QString dn;
            if (idata.isDyn)        dn+="dyn";
            if (idata.isUnnumbered) dn+="unn";
            if (idata.isBridgePort) dn+="bridge";

            QStringList qsl;
            qsl << idata.name.c_str()
                << idata.label.c_str()
                << idata.address.c_str()
                << idata.netmask.c_str()
                << dn
                << idata.physicalAddress.c_str();
            new QTreeWidgetItem(m_dialog->iface_list, qsl);

//            cerr << "Added interface " << idata.name << endl;

        }
    }

    delete q;
    q=NULL;

#endif

    snmpPollCompleted=true;
    setNextEnabled( 1, true );
}

void newFirewallDialog::getInterfacesViaSNMP()
{
#ifdef HAVE_LIBSNMP

// need to protect from reentry because getAddrByName processes events
    if (q!=NULL || getInterfacesBusy) return;

    snmpPollCompleted=false;
    m_dialog->iface_list->clear();

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
    QString name=m_dialog->obj_name->text().toLatin1().constData();
    try
    {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
        QString a = getAddrByName(name);
        QApplication::restoreOverrideCursor();
        addr = InetAddr(a.toAscii().constData());
    } catch (FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Address of %1 could not be obtained via DNS")
            .arg(m_dialog->obj_name->text()),
            "&Continue", QString::null, QString::null, 0, 1 );
        getInterfacesBusy = false;
        return ;
    }

    logger=NULL;
    m_dialog->snmpProgress->clear();

    if (q!=NULL) delete q;
    q=new SNMP_interface_query();
    q->init(addr.toString(),rcomm,SNMP_DEFAULT_RETRIES,SNMP_DEFAULT_TIMEOUT);

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

bool newFirewallDialog::appropriate(const int page) const
{
    int p  = page;

    if (fwbdebug)
    {
        qDebug("newFirewallDialog::appropriate  p=%d",p);
    }

    switch (p)
    {
    case 0:
    case 4:
        return true;

    case 1:
    case 2:
    case 3:
        return (!m_dialog->useTemplate->isChecked());
    }
    return true;
}

void newFirewallDialog::nextClicked()
{
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

// p is a page number _after_ it changed
    switch (p)
    {
    case 1:
    {
        changed();  // to properly enable/disable widgets
        break;
    }

    case 2:
    {
        m_dialog->iface_name->setFocus();

        if (!Resources::getTargetCapabilityBool(readPlatform(m_dialog->platform).toLatin1().constData(),
                                                "security_levels") )
        {
/* if chosen fw platform does not support security levels,
 * this is the last page
 */
            setNextEnabled( 2, false );
            setFinishEnabled( 2, true );
        }
        break;
    }

    case 3:
    {
        if (m_dialog->useTemplate->isChecked())
        {
            showPage( 0 );
            return;
        }

        fillInterfaceSLList();

        setFinishEnabled( 3, true );
        break;
    }

    case 4:
    {
        setFinishEnabled( 4, true );
/* load templates if not loaded */
        if (tmpldb==NULL)
        {

            MessageBoxUpgradePredicate upgrade_predicate(this);

            tmpldb = new FWObjectDatabase();
            tmpldb->setReadOnly( false );
            tmpldb->load( tempfname, &upgrade_predicate, librespath);
        }
        FWObject *tlib = tmpldb->getById(TEMPLATE_LIB);

#if 0
        FWObject *tlib = mw->db()->getById(TEMPLATE_LIB);
        if (tlib==NULL)
        {
            FWObject *cl = mw->getCurrentLib();
            mw->loadLibrary(tempfname);
            unloadTemplatesLib = true;
            mw->loadObjects();
            tlib = mw->db()->getById(TEMPLATE_LIB);
/* restore library that was opened prior loading templates */
            mw->openLib(cl);
        }
#endif

        list<FWObject*> fl;
        findFirewalls(tlib, fl, false);

        QString icn = QString( Resources::global_res->getObjResourceStr(fl.front(), "icon-tree").c_str() );

        m_dialog->templateList->clear();

        int n=0;
        for (list<FWObject*>::iterator m=fl.begin(); m!=fl.end(); m++,n++)
        {
            FWObject *o=*m;

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
            templates[ m_dialog->templateList->item( m_dialog->templateList->count()-1 ) ]=o;
        }
        m_dialog->templateList->setCurrentItem(0);
        m_dialog->templateList->setFocus();
        break;
    }
    }
}

void newFirewallDialog::fillInterfaceSLList()
{

    QTreeWidgetItem *itm = m_dialog->iface_list->topLevelItem(0);// firstChild();

    int itm_index = 0;

    m_dialog->iface_sl_list->clear();

    while (itm!=NULL)
    {
        InterfaceData idata;

        idata.name         =  itm->text(0).toLatin1().constData();
        idata.label        =  itm->text(1).toLatin1().constData();
        idata.isDyn        =  itm->text(4).indexOf("Dyn")!=-1;
        idata.isUnnumbered =  itm->text(4).indexOf("Unn")!=-1;
        idata.isBridgePort =  itm->text(4).indexOf("Bridge")!=-1;

        if (!idata.isDyn && !idata.isUnnumbered && !idata.isBridgePort)
            idata.address  =  itm->text(2).toLatin1().constData();
        else
            idata.address  =  QObject::tr("dynamic").toLatin1().constData();

        try
        {
            idata.guessSecurityLevel( readPlatform(m_dialog->platform).toLatin1().constData() );
        }
        catch (FWException &ex)
        {
            QMessageBox::warning(
                this,"Firewall Builder", ex.toString().c_str(),
                "&Continue", QString::null, QString::null, 0, 1 );

            showPage( 2 );
            return;
        }

        QStringList qsl;
        qsl << idata.name.c_str()
            << idata.label.c_str()
            << idata.address.c_str()
            << QString::number(idata.securityLevel);
        new QTreeWidgetItem(m_dialog->iface_sl_list, qsl);

        itm_index++;
        itm=m_dialog->iface_list->topLevelItem(itm_index);
    }
}

void newFirewallDialog::templateSelected(QListWidgetItem *itm)
{
    FWObject *o=templates[itm];
    assert (o!=NULL);

    Firewall *fw = Firewall::cast(o);

    m_dialog->templateComment->clear();
    QString s=QString("<a name=\"top\">\n") + fw->getComment().c_str();
    m_dialog->templateComment->append( s );
    m_dialog->templateComment->scrollToAnchor("top");

    bool haveOutside = false;
    bool haveInside  = false;
    bool haveDMZ     = false;
    list<FWObject*> ll = fw->getByType(Interface::TYPENAME);
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        Interface *intf = Interface::cast( *i );
        if (intf->getLabel()=="outside")
        {
            haveOutside=true;
            m_dialog->intfOutsideLine->show();
            m_dialog->intfOutsideText->show();
            fillInterfaceData(intf,m_dialog->intfOutsideText);
        }
        if (intf->getLabel()=="inside")
        {
            haveInside=true;
            m_dialog->intfInsideLine->show();
            m_dialog->intfInsideText->show();
            fillInterfaceData(intf,m_dialog->intfInsideText);
        }
        if (intf->getLabel()=="dmz")
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
            if (intf->isBridgePort()) s +=  tr("Bridge port");
            else
                s += QString("%1/%2")
                    .arg(intf->getAddress().toString().c_str())
                    .arg( intf->getNetmask().toString().c_str());
    s += "</td>";
    s += "</tr>";
    s += "</table>";
    qte->setText(s);
}

void newFirewallDialog::addInterface()
{
    QString dn = "";
    if (m_dialog->iface_dyn->isChecked())        dn+="Dyn";
    if (m_dialog->iface_unnum->isChecked())      dn+="Unn";
    if (m_dialog->iface_bridgeport->isChecked()) dn+="Bridge";

    QString addr;
    QString netm;

    if (!m_dialog->iface_dyn->isChecked() &&
        !m_dialog->iface_unnum->isChecked() &&
        !m_dialog->iface_bridgeport->isChecked())
    {
        addr = m_dialog->iface_addr->text();
        netm = m_dialog->iface_netmask->text();

        if (addr.isEmpty())
            addr = QString(InetAddr::getAny().toString().c_str());
        if (netm.isEmpty())
            netm = QString(InetAddr::getAny().toString().c_str());

        try
        {
            InetAddr(addr.toLatin1().constData());
            InetNetmask(netm.toLatin1().constData());
        }
        catch (FWException &ex)
        {
            QMessageBox::warning(
                this,"Firewall Builder",
                tr("Illegal address '%1/%2'").arg(addr).arg(netm),
                "&Continue", QString::null, QString::null, 0, 1 );
            return;
        }
    }
    QStringList qsl;
    qsl << m_dialog->iface_name->text()
        << m_dialog->iface_label->text()
        << addr << netm << dn
        << m_dialog->iface_physaddr->text();

    new QTreeWidgetItem(m_dialog->iface_list, qsl);
}

void newFirewallDialog::selectedInterface(QTreeWidgetItem*cur,QTreeWidgetItem*)
{
    QTreeWidgetItem *itm = cur; //current item

    m_dialog->iface_name->setText( itm->text(0) );
    m_dialog->iface_label->setText( itm->text(1) );
    m_dialog->iface_addr->setText( itm->text(2) );
    m_dialog->iface_netmask->setText( itm->text(3) );
    m_dialog->iface_reg->setChecked( itm->text(4).isEmpty() );
    m_dialog->iface_dyn->setChecked( itm->text(4).indexOf("Dyn")!=-1 );
    m_dialog->iface_unnum->setChecked( itm->text(4).indexOf("Unn")!=-1 );
    m_dialog->iface_bridgeport->setChecked( itm->text(4).indexOf("Bridge")!=-1 );
    m_dialog->iface_physaddr->setText( itm->text(5) );
}

void newFirewallDialog::updateInterface()
{
    QString dn = "";
    if (m_dialog->iface_dyn->isChecked())   dn+="Dyn";
    if (m_dialog->iface_unnum->isChecked()) dn+="Unn";
    if (m_dialog->iface_bridgeport->isChecked()) dn+="Bridge";

    QTreeWidgetItem *itm = m_dialog->iface_list->currentItem();
    if (itm==NULL) return;

    itm->setText( 0 , m_dialog->iface_name->text() );
    itm->setText( 1 , m_dialog->iface_label->text() );
    itm->setText( 2 , m_dialog->iface_addr->text() );
    itm->setText( 3 , m_dialog->iface_netmask->text() );
    itm->setText( 4 , dn );
    itm->setText( 5 , m_dialog->iface_physaddr->text() );
}

void newFirewallDialog::deleteInterface()
{
    QTreeWidgetItem *itm = m_dialog->iface_list->currentItem();
    if (itm==NULL) return;
    m_dialog->iface_list->takeTopLevelItem(
            m_dialog->iface_list->indexOfTopLevelItem(itm) );
}

void newFirewallDialog::adjustSL(QTreeWidgetItem *itm1)
{
// interface 1 is above 2. Adjust their security levels accordingly
    int sl1 = itm1->text(3).toInt();

    int index = itm1->treeWidget()->indexOfTopLevelItem(itm1);

    QTreeWidgetItem *itm2 = itm1->treeWidget()->topLevelItem(index+1);
    QTreeWidgetItem *itm3 = itm1->treeWidget()->topLevelItem(index-1);

    if (itm2==NULL) sl1=100;
    else
    {
        if (itm3==NULL) sl1=0;
        else
        {
            int sl2 = itm2->text(3).toInt();
            int sl3 = itm3->text(3).toInt();
            sl1 = (sl2+sl3)/2;
        }
    }
    itm1->setText( 3 , QString("%1").arg(sl1) );
}

void newFirewallDialog::upInterface()
{
    QTreeWidgetItem *itm1 = m_dialog->iface_sl_list->currentItem();
    if (itm1==NULL) return;
    int index = m_dialog->iface_sl_list->indexOfTopLevelItem(itm1);

    QTreeWidgetItem *itm2 = m_dialog->iface_sl_list->topLevelItem(index-1);
    if (itm2==NULL) return;
    m_dialog->iface_sl_list->takeTopLevelItem(index);
    m_dialog->iface_sl_list->insertTopLevelItem(index-1, itm1);
    adjustSL(itm1);
}

void newFirewallDialog::downInterface()
{

    QTreeWidgetItem *itm1 = m_dialog->iface_sl_list->currentItem();
    if (itm1==NULL) return;
    int index = m_dialog->iface_sl_list->indexOfTopLevelItem(itm1);

    QTreeWidgetItem *itm2 = m_dialog->iface_sl_list->topLevelItem(index+1);
    if (itm2==NULL) return;
    m_dialog->iface_sl_list->takeTopLevelItem(index);
    m_dialog->iface_sl_list->insertTopLevelItem(index+1, itm1);
    adjustSL(itm1);
}

void newFirewallDialog::cancelClicked()
{
    reject();
}

void newFirewallDialog::finishClicked()
{
    int p = currentPage();

    if (p==2)  fillInterfaceSLList();

    if (p==4)
    {
        QListWidgetItem *itm = m_dialog->templateList->currentItem();
        FWObject *template_fw=templates[itm];
        assert (template_fw!=NULL);

        FWObject *no = mw->duplicateObject(mw->getCurrentLib(),
                                           template_fw,
                                           m_dialog->obj_name->text(),
                                           false );  // do not ask to autorename
        if (no==NULL)
        {
            QDialog::accept();
            return;
        }

        map<string,string> platforms = Resources::getPlatforms();
        map<string,string>::iterator i;
        for (i=platforms.begin(); i!=platforms.end(); i++)
            Resources::setDefaultTargetOptions( i->first , Firewall::cast(no) );

        map<string,string> OSs = Resources::getOS();
        for (i=OSs.begin(); i!=OSs.end(); i++)
            Resources::setDefaultTargetOptions( i->first  , Firewall::cast(no) );

        no->setStr("platform", readPlatform(m_dialog->platform).toLatin1().constData() );
        no->setStr("host_OS",  readHostOS(m_dialog->hostOS).toLatin1().constData() );

        nfw=Firewall::cast(no);
    } else
    {
        FWObject *o;
        o=mw->createObject(Firewall::TYPENAME, m_dialog->obj_name->text() );

        if (o==NULL)
        {
            QDialog::accept();
            return;
        }

        map<string,string> platforms = Resources::getPlatforms();
        map<string,string>::iterator i;
        for (i=platforms.begin(); i!=platforms.end(); i++)
            Resources::setDefaultTargetOptions( i->first , Firewall::cast(o) );

        map<string,string> OSs = Resources::getOS();
        for (i=OSs.begin(); i!=OSs.end(); i++)
            Resources::setDefaultTargetOptions( i->first  , Firewall::cast(o) );

        o->setStr("platform", readPlatform(m_dialog->platform).toLatin1().constData() );
        o->setStr("host_OS",  readHostOS(m_dialog->hostOS).toLatin1().constData() );

        nfw=Firewall::cast(o);

/* create interfaces */

        int itm_index = 0;
        QTreeWidgetItem *itm = m_dialog->iface_list->topLevelItem(itm_index);

        while (itm!=NULL)
        {
            QString name    =  itm->text(0);
            QString label   =  itm->text(1);
            QString addr    =  itm->text(2);
            QString netmask =  itm->text(3);
            bool    dyn     =  itm->text(4).indexOf("Dyn")!=-1;
            bool    unnum   =  itm->text(4).indexOf("Unn")!=-1;
            bool    bridgeport =  itm->text(4).indexOf("Bridge")!=-1;
            QString physaddr=  itm->text(5);

            QList<QTreeWidgetItem*> ltwi = m_dialog->iface_sl_list->findItems( name , Qt::MatchExactly );
            assert(!ltwi.empty());
            QTreeWidgetItem *itm2 = ltwi[0];
            assert(itm2!=NULL);

            int     sl = itm2->text(3).toInt();

            Interface *oi = Interface::cast(mw->createObject(nfw,Interface::TYPENAME,
                                                             name));
#ifdef USE_INTERFACE_POLICY
            oi->add(new InterfacePolicy());
#endif
            oi->setLabel( label.toLatin1().constData() );

            oi->setDyn(dyn);
            oi->setUnnumbered(unnum);
            oi->setBridgePort(bridgeport);
            oi->setSecurityLevel(sl);

            if (!dyn && !unnum && !bridgeport)
            {
                QString addrname=QString("%1:%2:ip").arg(m_dialog->obj_name->text()).arg(name);
                IPv4 *oa = IPv4::cast(mw->createObject(oi, IPv4::TYPENAME,addrname));
                oa->setAddress( InetAddr(addr.toLatin1().constData()) );
                oa->setNetmask( InetNetmask(netmask.toLatin1().constData()) );
            }
            // updateObjName has a side effect: it causes redraw of the ruleset
            // views in the main window
            mw->updateObjName(oi,"","",false);

            itm_index++;
            itm=m_dialog->iface_list->topLevelItem(itm_index);
        }
    }
    if (unloadTemplatesLib)
    {
#if 0
        FWObject *tlib = mw->db()->getById(TEMPLATE_LIB);
        assert (tlib!=NULL);
        string tlibID = tlib->getId();
        if (fwbdebug)
            qDebug("newFirewallDialog::accept  Delete template library");
        mw->delObj(tlib,false);

/*
 * deleting an object places it in the "Deleted objects" library, so
 * we need to remove "templates" library from there.
 *
 * TODO: need to add flags to the API to be able to delete objects
 * without placing them in "Deleted objects" automatically
 */
        FWObject *delObjLib = mw->db()->getById( DELETED_LIB );
        if (delObjLib!=NULL && delObjLib->getById(tlibID)!=NULL)
        {
            if (fwbdebug) qDebug("newFirewallDialog::accept  Delete library of templates from 'Deleted objects'");
            mw->delObj(tlib,false);  // this time from deleted objects lib
        }
#endif

        delete tmpldb;
        tmpldb = NULL;
        unloadTemplatesLib=false;
    }
    QDialog::accept();
}



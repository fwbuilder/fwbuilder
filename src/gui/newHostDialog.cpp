/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: newHostDialog.cpp,v 1.13 2007/05/23 03:05:51 vkurland Exp $

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

#include "newHostDialog.h"
#include "InterfaceData.h"
#include "FWWindow.h"
#include "ObjConflictResolutionDialog.h"
#include "upgradePredicate.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Host.h"
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
#include <qlistview.h>
#include <qtextbrowser.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qpixmapcache.h>

#include <iostream>

// must be the last for win
#include "fwbuilder/snmp.h"

using namespace libfwbuilder;
using namespace std;

#define OBJECT_NAME_PAGE 0
#define SNMP_PAGE        1
#define MANUAL_PAGE      2
#define TEMPLATES_PAGE   3

newHostDialog::newHostDialog() : QDialog()
{
    m_dialog = new Ui::newHostDialog_q;
    m_dialog->setupUi(this);

    setControlWidgets(this, m_dialog->stackedWidget,
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);

    nhst=NULL;
    tmpldb = NULL;
    snmpPollCompleted=false;
    q=NULL;
    unloadTemplatesLib = false;
    getInterfacesBusy = false;

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(monitor()) );

    setNextEnabled( OBJECT_NAME_PAGE, false );

    m_dialog->iface_list->setAllColumnsShowFocus( true );

    m_dialog->obj_name->setFocus();

    showPage(0);
}

newHostDialog::~newHostDialog()
{
    delete m_dialog;
    if (timer!=NULL) delete timer;
#ifdef HAVE_LIBSNMP
    if (q!=NULL) delete q;
#endif
}

void newHostDialog::nextClicked()
{
    if (nextRelevant( currentPage() ) > -1)
        showPage(nextRelevant( currentPage() ));
}

void newHostDialog::backClicked()
{
    if (previousRelevant( currentPage() ) > -1)
        showPage(previousRelevant( currentPage() ));
}


void newHostDialog::changed()
{
    int p = currentPage();
    if (p==OBJECT_NAME_PAGE)
    {
        setNextEnabled( p, !m_dialog->obj_name->text().isEmpty() );
    }

    if (p==SNMP_PAGE)
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
        setNextEnabled( SNMP_PAGE, f );
    }

    if (p==MANUAL_PAGE)
    {
        if (m_dialog->iface_dyn->isChecked() || m_dialog->iface_unnum->isChecked())
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

void  newHostDialog::monitor()
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

            idata.guessLabel("");

            QString dn;
            if (idata.isDyn)        dn+="dyn";
            if (idata.isUnnumbered) dn+="unn";

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
    setNextEnabled( SNMP_PAGE, true );
}

void newHostDialog::getInterfacesViaSNMP()
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

    IPAddress addr;
    QString name=m_dialog->obj_name->text().toLatin1().constData();
    try
    {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
        QString a = getAddrByName(name);
        QApplication::restoreOverrideCursor();
        addr = a.toAscii().constData();
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

bool newHostDialog::appropriate(const int page) const
{
    int p  = page;

    if (fwbdebug)
    {
        qDebug("newHostDialog::appropriate  p=%d",p);
    }

    switch (p)
    {
    case OBJECT_NAME_PAGE:
    case TEMPLATES_PAGE:
        return true;

    case SNMP_PAGE:
    case MANUAL_PAGE:
        return (!m_dialog->useTemplate->isChecked());
    }
    return true;
}


void newHostDialog::showPage(const int page)
{
    FakeWizard::showPage(page);

    int p = currentPage();

    if (fwbdebug) qDebug("newHostDialog::selected  p=%d",p);

// p is a page number _after_ it changed

    switch (p)
    {
    case SNMP_PAGE:
        changed();  // to properly enable/disable widgets
        break;

    case MANUAL_PAGE:
    {
        m_dialog->iface_name->setFocus();

        setNextEnabled( MANUAL_PAGE, false );
        setFinishEnabled( MANUAL_PAGE, true );
        break;
    }

    case TEMPLATES_PAGE:
    {
        setFinishEnabled( TEMPLATES_PAGE, true );
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
        findHosts(tlib, fl, false);

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

            QListWidgetItem *item = new QListWidgetItem(
                    QIcon(pm), QString(o->getName().c_str()));
            m_dialog->templateList->addItem(item);

            templates[ m_dialog->templateList->item( m_dialog->templateList->count()-1 ) ]=o;
        }
        m_dialog->templateList->setCurrentItem(0);
        m_dialog->templateList->setFocus();
        break;
    }
    }
}
void newHostDialog::templateSelected(QListWidgetItem *itm)
//void newHostDialog::templateSelected(QListWidgetItem *cur)
{
    //QListWidgetItem *itm = cur;
    if (fwbdebug) qDebug("newHostDialog::templateSelected ");

    FWObject *o=templates[itm];
    assert (o!=NULL);

    Host *fw = Host::cast(o);

    m_dialog->templateComment->clear();
    m_dialog->templateComment->append( fw->getComment().c_str() );
    m_dialog->templateComment->moveCursor(QTextCursor::Start);

    bool haveOutside = false;
    bool haveInside  = false;
    bool haveDMZ     = false;
    list<FWObject*> ll = fw->getByType(Interface::TYPENAME);
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        Interface *intf = Interface::cast( *i );
        QString     nam = intf->getName().c_str();
        QString     lbl = intf->getLabel().c_str();

        if (lbl=="outside"                ||
            nam.indexOf(QRegExp(".*0$"))!=-1 ||
            nam.indexOf(QRegExp(".*0/0$"))!=-1 )
        {
            haveOutside=true;
            m_dialog->intfOutsideLine->show();
            m_dialog->intfOutsideText->show();
            fillInterfaceData(intf,m_dialog->intfOutsideText);
        }
        if (lbl=="inside"                 ||
            nam.indexOf(QRegExp(".*1$"))!=-1 ||
            nam.indexOf(QRegExp(".*0/1$"))!=-1 )
        {
            haveInside=true;
            m_dialog->intfInsideLine->show();
            m_dialog->intfInsideText->show();
            fillInterfaceData(intf,m_dialog->intfInsideText);
        }
    }

    if (!haveOutside) { m_dialog->intfOutsideLine->hide(); m_dialog->intfOutsideText->hide(); }
    if (!haveInside)  { m_dialog->intfInsideLine->hide();  m_dialog->intfInsideText->hide();  }
    if (!haveDMZ)     { m_dialog->intfDMZLine->hide();     m_dialog->intfDMZText->hide();     }
}

void newHostDialog::fillInterfaceData(Interface *intf, QTextBrowser *qte)
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
            s += QString("%1/%2")
                .arg(intf->getAddress().toString().c_str())
                .arg( intf->getNetmask().toString().c_str());
    s += "</td>";
    s += "</tr>";
    s += "</table>";
    qte->setText(s);
}

void newHostDialog::addInterface()
{
    QString dn = "";
    if (m_dialog->iface_dyn->isChecked())   dn += "Dyn";
    if (m_dialog->iface_unnum->isChecked()) dn += "Unn";

    QString addr;
    QString netm;

    if (!m_dialog->iface_dyn->isChecked() && !m_dialog->iface_unnum->isChecked())
    {
        addr = m_dialog->iface_addr->text();
        netm = m_dialog->iface_netmask->text();

        if (addr.isEmpty()) addr="0.0.0.0";
        if (netm.isEmpty()) netm="0.0.0.0";

        try
        {
            IPAddress(addr.toLatin1().constData());
            Netmask(netm.toLatin1().constData());
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
    QStringList sl;
    sl << m_dialog->iface_name->text()
       << m_dialog->iface_label->text()
       << addr
       << netm
       << dn
       << m_dialog->iface_physaddr->text();

    new QTreeWidgetItem(m_dialog->iface_list, sl);
}
void newHostDialog::selectedInterface(QTreeWidgetItem*cur,QTreeWidgetItem*)
//void newHostDialog::selectedInterface(QTreeWidgetItem *cur)
{
    QTreeWidgetItem *itm = cur;
    m_dialog->iface_name->setText( itm->text(0) );
    m_dialog->iface_label->setText( itm->text(1) );
    m_dialog->iface_addr->setText( itm->text(2) );
    m_dialog->iface_netmask->setText( itm->text(3) );
    m_dialog->iface_dyn->setChecked( itm->text(4).indexOf("Dyn")!=-1 );
    m_dialog->iface_unnum->setChecked( itm->text(4).indexOf("Unn")!=-1 );
    m_dialog->iface_physaddr->setText( itm->text(5) );
}

void newHostDialog::updateInterface()
{
    QString dn = "";
    if (m_dialog->iface_dyn->isChecked())   dn+="Dyn";
    if (m_dialog->iface_unnum->isChecked()) dn+="Unn";

    QTreeWidgetItem *itm = m_dialog->iface_list->currentItem();
    if (itm==NULL) return;

    itm->setText( 0 , m_dialog->iface_name->text() );
    itm->setText( 1 , m_dialog->iface_label->text() );
    itm->setText( 2 , m_dialog->iface_addr->text() );
    itm->setText( 3 , m_dialog->iface_netmask->text() );
    itm->setText( 4 , dn );
    itm->setText( 5 , m_dialog->iface_physaddr->text() );
}

void newHostDialog::deleteInterface()
{
    QTreeWidgetItem *itm = m_dialog->iface_list->currentItem();
    if (itm==NULL) return;
    m_dialog->iface_list->takeTopLevelItem(
            m_dialog->iface_list->indexOfTopLevelItem(itm) );
}

void newHostDialog::cancelClicked()
{
    QDialog::reject();
}

void newHostDialog::finishClicked()
{
    int p = currentPage();

    if (p==TEMPLATES_PAGE)
    {
        QListWidgetItem *itm = m_dialog->templateList->currentItem();
        FWObject *o=templates[itm];
        assert (o!=NULL);

        FWObject *no = mw->duplicateObject(mw->getCurrentLib(),
                                           o,
                                           m_dialog->obj_name->text(),
                                           false );  // do not ask to autorename
        if (no==NULL)
        {
          QDialog::accept();
          return;
        }
        nhst=Host::cast(no);
    } else
    {
        FWObject *o;
        o=mw->createObject(Host::TYPENAME, m_dialog->obj_name->text() );
        if (o==NULL)
        {
          QDialog::accept();
          return;
        }

        nhst=Host::cast(o);

/* create interfaces */

        for (int i = 0; i < m_dialog->iface_list->topLevelItemCount(); i++)
        {
            QTreeWidgetItem *itm = m_dialog->iface_list->topLevelItem(i);
            QString name    =  itm->text(0);
            QString label   =  itm->text(1);
            QString addr    =  itm->text(2);
            QString netmask =  itm->text(3);
            bool    dyn     =  itm->text(4).indexOf("Dyn")!=-1;
            bool    unnum   =  itm->text(4).indexOf("Unn")!=-1;
            QString physaddr=  itm->text(5);

            Interface *oi = Interface::cast(
                mw->createObject(nhst,Interface::TYPENAME, name)
            );
#ifdef USE_INTERFACE_POLICY
            oi->add(new InterfacePolicy());
#endif
            oi->setLabel( label.toLatin1().constData() );

            if (dyn)   oi->setDyn(true);
            if (unnum) oi->setUnnumbered(true);
            oi->setSecurityLevel(0);

            if (!dyn && !unnum)
            {
                QString addrname=QString("%1:%2:ip")
                    .arg(m_dialog->obj_name->text()).arg(name);
                IPv4 *oa = IPv4::cast(
                    mw->createObject(oi, IPv4::TYPENAME,addrname)
                );
                oa->setAddress( addr.toLatin1().constData()    );
                oa->setNetmask( netmask.toLatin1().constData() );
            }

            mw->updateObjName(oi,"","",false);
        }
    }
    if (unloadTemplatesLib)
    {
#if 0
        FWObject *tlib = mw->db()->getById(TEMPLATE_LIB);
        assert (tlib!=NULL);
        string tlibID = tlib->getId();
        if (fwbdebug) qDebug("  Delete library of templates");
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
            if (fwbdebug) qDebug("  Delete library of templates from 'Deleted objects'");
            mw->delObj(tlib,false);  // this time from deleted objects lib
        }
#endif

        delete tmpldb;
        tmpldb = NULL;

        unloadTemplatesLib=false;
    }
    QDialog::accept();
}




/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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
#include "utils_no_qt.h"
#include "platforms.h"

#include "newHostDialog.h"
#include "FWWindow.h"
#include "ObjConflictResolutionDialog.h"
#include "upgradePredicate.h"
#include "events.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/BackgroundOp.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Constants.h"

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
#include <qfiledialog.h>
#include <qdir.h>

#include <iostream>

// must be the last for win
#include "fwbuilder/snmp.h"

using namespace libfwbuilder;
using namespace std;

#define OBJECT_NAME_PAGE 0
#define SNMP_PAGE        1
#define MANUAL_PAGE      2
#define TEMPLATES_PAGE   3

newHostDialog::newHostDialog(QWidget *parentw, FWObject *_p) : QDialog(parentw)
{
    parent = _p;
    db = parent->getRoot();

    m_dialog = new Ui::newHostDialog_q;
    m_dialog->setupUi(this);

    setControlWidgets(this, m_dialog->stackedWidget,
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);

    nhst=nullptr;
    tmpldb = nullptr;
    snmpPollCompleted=false;
    q=nullptr;
    unloadTemplatesLib = false;
    getInterfacesBusy = false;

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(monitor()) );
    connect( m_dialog->templateUseCustom, SIGNAL(pressed()),
             this,SLOT(browseTemplate()));
    connect( m_dialog->templateUseStandard, SIGNAL(pressed()),
             this,SLOT(useStandardTemplate()));
    connect( m_dialog->useTemplate, SIGNAL(released()),
             this,SLOT(updateTemplatePanel()));
    m_dialog->templateFilePath->setText(
        Constants::getTemplatesObjectsFilePath().c_str());
    updateTemplatePanel();

    setNextEnabled( OBJECT_NAME_PAGE, false );

    m_dialog->obj_name->setFocus();

    m_dialog->interfaceEditor->clear();

    showPage(0);
}

void newHostDialog::browseTemplate()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("FWBuilder template files"), st->getOpenFileDir(),
                tr("FWBuilder template files (*.xml *.fwb)"));

    if (fileName.isEmpty()) return;
    st->setOpenFileDir(fileName);

    m_dialog->templateFilePath->setText(fileName);
    updateTemplatePanel();
}

void newHostDialog::useStandardTemplate()
{
    m_dialog->templateFilePath->setText(
        Constants::getTemplatesObjectsFilePath().c_str());
    updateTemplatePanel();
}

void newHostDialog::updateTemplatePanel()
{
    if (m_dialog->useTemplate->checkState()==Qt::Checked)
    {
        QString fileName = m_dialog->templateFilePath->text();
        bool using_std = fileName == Constants::getTemplatesObjectsFilePath().c_str();

        m_dialog->templateFrame->setVisible(true);
        m_dialog->templateFilePathLabel->setVisible(!using_std);
        m_dialog->templateFilePath->setVisible(!using_std);
        m_dialog->templateUseCustom->setVisible(using_std);
        m_dialog->templateUseStandard->setVisible(!using_std);
    }
    else
    {
        m_dialog->templateFrame->setVisible(false);
    }
}


newHostDialog::~newHostDialog()
{
    delete m_dialog;
    if (timer!=nullptr) delete timer;
#ifdef HAVE_LIBSNMP
    if (q!=nullptr) delete q;
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
        /*
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
        }*/
    }
}

void  newHostDialog::monitor()
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

    map<int, InterfaceData>* intf = q->getInterfaces();
    map<int, InterfaceData>::iterator i;
    for(i=intf->begin(); i!=intf->end(); ++i)
    {
        InterfaceData* idata = &(i->second);
        if ( idata->ostatus )
        {
            guessInterfaceLabel(idata);
            this->m_dialog->interfaceEditor->addInterfaceFromData(idata);
        }
    }

    delete q;
    q=nullptr;

#endif

    snmpPollCompleted=true;
    setNextEnabled( SNMP_PAGE, true );
}

void newHostDialog::getInterfacesViaSNMP()
{
#ifdef HAVE_LIBSNMP

// need to protect from reentry because getAddrByName processes events
    if (q!=nullptr || getInterfacesBusy) return;

    snmpPollCompleted=false;

    string rcomm=m_dialog->snmp_community->text().toLatin1().constData();

    if ( rcomm.empty() )
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Missing SNMP community string."),
            "&Continue", QString::null, QString::null, 0, 1 );
        return ;
    }

    m_dialog->interfaceEditor->clear();
    m_dialog->interfaceEditor->removeTab(0);
    getInterfacesBusy = true;

    InetAddr addr;
    QString name=m_dialog->obj_name->text().toLatin1().constData();
    try
    {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
        QString a = getAddrByName(name, AF_INET);
        QApplication::restoreOverrideCursor();
        addr = InetAddr(a.toLatin1().constData());
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

    logger=nullptr;
    m_dialog->snmpProgress->clear();

    if (q!=nullptr) delete q;
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
        m_dialog->nextButton->setDefault(true);
        break;

    case MANUAL_PAGE:
    {
        setNextEnabled( MANUAL_PAGE, false );
        setFinishEnabled( MANUAL_PAGE, true );
        m_dialog->finishButton->setDefault(true);
        break;
    }

    case TEMPLATES_PAGE:
    {
        m_dialog->finishButton->setDefault(true);
        setFinishEnabled( TEMPLATES_PAGE, true );
/* load templates if not loaded */

        if (tmpldb==nullptr)
        {

            MessageBoxUpgradePredicate upgrade_predicate(this);

            tmpldb = new FWObjectDatabase();
            tmpldb->setReadOnly( false );
            tmpldb->load( m_dialog->templateFilePath->text().toLatin1().data(),
                          &upgrade_predicate, Constants::getDTDDirectory());
        }
        list<FWObject*> fl;

        FWObjectTypedChildIterator libiter = tmpldb->findByType(Library::TYPENAME);
        for ( ; libiter!=libiter.end(); ++libiter)
            findHosts(*libiter, fl, false);

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
    assert (o!=nullptr);

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

bool newHostDialog::validateAddressAndMask(const QString &addr,
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
            tr("Illegal address '%1/%2'").arg(addr).arg(netm),
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
                    tr("Illegal address '%1/%2'").arg(addr).arg(netm),
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
            tr("Illegal address '%1/%2'").arg(addr).arg(netm),
            "&Continue", QString::null, QString::null, 0, 1 );
        return false;
    }
    return true;
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
        assert (o!=nullptr);

        FWObject *no = db->create(Host::TYPENAME);
        no->duplicate(o, true);
        no->setName(m_dialog->obj_name->text().toUtf8().constData());

        mw->activeProject()->m_panel->om->autorename(no);

        nhst = Host::cast(no);
    } else
    {
        if ( !this->m_dialog->interfaceEditor->isValid() )
            return;

        FWObject *o;
        o = db->create(Host::TYPENAME);
        o->setName(m_dialog->obj_name->text().toUtf8().constData());
        if (o==nullptr)
        {
          QDialog::accept();
          return;
        }

        nhst = Host::cast(o);

/* create interfaces */

        foreach(EditedInterfaceData iface,
                this->m_dialog->interfaceEditor->getNewData())
        {
            QString name     =  iface.name;
            QString label    =  iface.label;
            bool    dyn      =  iface.type == 1;
            bool    unnum    =  iface.type == 2;
            QString physaddr =  iface.mac;

            Interface *oi = Interface::cast(db->create(Interface::TYPENAME));
            oi->setName( string(name.toUtf8().constData()) );
            oi->setLabel( string(label.toUtf8().constData()) );
            oi->setComment( string(iface.comment.toUtf8().constData()) );
            nhst->add(oi);

            if (dyn)   oi->setDyn(true);
            if (unnum) oi->setUnnumbered(true);
            oi->setSecurityLevel(0);
            if (physaddr != "")
            {
                QString addrname=QString("%1:%2:mac")
                    .arg(m_dialog->obj_name->text()).arg(name);
                physAddress* pa = physAddress::cast(
                    db->create(physAddress::TYPENAME));
                pa->setName(addrname.toUtf8().constData());
                oi->add(pa);
                pa->setPhysAddress(physaddr.toLatin1().constData());
            }
            if (iface.type == 0)
            {
                foreach(AddressInfo address, iface.addresses)
                {
                    if (address.address == "0.0.0.0") continue;
                    if (address.ipv4)
                    {
                        string addrname = string(
                            QString("%1:%2:ip")
                            .arg(m_dialog->obj_name->text()).arg(name).toUtf8());
                        IPv4 *oa = IPv4::cast(db->create(IPv4::TYPENAME));
                        oi->add(oa);
                        oa->setName(addrname);
                        oa->setAddress(
                            InetAddr(address.address.toLatin1().constData()) );
                        bool ok = false ;
                        int inetmask = address.netmask.toInt(&ok);
                        if (ok)
                        {
                            oa->setNetmask( InetAddr(inetmask) );
                        }
                        else
                        {
                            oa->setNetmask(
                                InetAddr(address.netmask.toLatin1().constData()) );
                        }
                    }
                    else
                    {
                        string addrname = string(
                            QString("%1:%2:ip6")
                            .arg(m_dialog->obj_name->text()).arg(name).toUtf8() );
                        IPv6 *oa = IPv6::cast(db->create(IPv6::TYPENAME));
                        oi->add(oa);
                        oa->setName(addrname);
                        oa->setAddress(
                            InetAddr(AF_INET6,
                                     address.address.toLatin1().constData()) );
                        bool ok = false ;
                        int inetmask = address.netmask.toInt(&ok);
                        if (ok)
                        {
                            oa->setNetmask( InetAddr(AF_INET6, inetmask) );
                        }
                        else
                        {
                            oa->setNetmask(
                                InetAddr(AF_INET6,
                                         address.netmask.toLatin1().constData()));
                        }
                    }
                }
            }
        }
        
    }
    if (unloadTemplatesLib)
    {
        delete tmpldb;
        tmpldb = nullptr;

        unloadTemplatesLib=false;
    }
    QDialog::accept();
}




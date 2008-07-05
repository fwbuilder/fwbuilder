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


#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"

#include "FWWindow.h"
#include "RuleSetView.h"
#include "FWBSettings.h"
#include "PrintingProgressDialog.h"
#include "FWObjectPropertiesFactory.h"
#include <ui_pagesetupdialog_q.h>
#include "printerStream.h"

#include <qglobal.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qprinter.h>
#include <qstatusbar.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qmessagebox.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qpixmapcache.h>
#include <qtablewidget.h>
#include <qabstractprintdialog.h>
#include <qprintdialog.h>

#include "fwbuilder/Policy.h"
#include "fwbuilder/InterfacePolicy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Interface.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/RuleSet.h"

#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/RuleElement.h"

#include <algorithm>
#include "ProjectPanel.h"
#include <QMdiArea>
#include <QMdiSubWindow>

using namespace libfwbuilder;
using namespace std;

QString legendList[] = {
    Firewall::TYPENAME, QObject::tr("Firewall"),
    Host::TYPENAME, QObject::tr("Host"),
    IPv4::TYPENAME, QObject::tr("Address"),
    IPv6::TYPENAME, QObject::tr("Address"),
    AddressRange::TYPENAME, QObject::tr("Addres Range"),
    Interface::TYPENAME, QObject::tr("Interface"),
    Network::TYPENAME, QObject::tr("Network"),
    ObjectGroup::TYPENAME, QObject::tr("Group of objects"),
    CustomService::TYPENAME, QObject::tr("Custom Service"),
    IPService::TYPENAME, QObject::tr("IP Service"),
    ICMPService::TYPENAME, QObject::tr("ICMP Service"),
    ICMP6Service::TYPENAME, QObject::tr("ICMP Service"),
    TCPService::TYPENAME, QObject::tr("TCP Service"),
    UDPService::TYPENAME, QObject::tr("UDP Service"),
    ServiceGroup::TYPENAME, QObject::tr("Group of services"),
    Interval::TYPENAME, QObject::tr("Time Interval"),
    "", ""
};


class pixmapOrText {
    public:

    QString text;
    QPixmap pixmap;

    pixmapOrText(const QString &t,const QPixmap &w);
};



pixmapOrText::pixmapOrText(const QString &t,const QPixmap &w) : text(t), pixmap(w)
{ }


list<FWObject*> findAllUsedByType(list<FWObject*> &result,FWObject *obj,const string &typeName)
{
    if (RuleElement::cast(obj)!=NULL)
    {
        for (list<FWObject*>::iterator m=obj->begin(); m!=obj->end(); m++)
        {
            FWObject *o=*m;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            if (o->getTypeName()==typeName)
                result.push_back(o);
        }
    }

    if (RuleSet::cast(obj)!=NULL)
    {
        for (list<FWObject*>::iterator m=obj->begin(); m!=obj->end(); m++)
        {
            if (Rule::cast(*m)!=NULL)
            {
                for (list<FWObject*>::iterator n=(*m)->begin(); n!=(*m)->end(); n++)
                {
                    if (RuleElement::cast(*n)!=NULL)
                    {
                        findAllUsedByType(result,*n,typeName);
                    }
                }
            }
        }
    }

    if (Firewall::isA(obj))
    {
        FWObject       *ruleSet;

        ruleSet = obj->getFirstByType(Policy::TYPENAME);
        findAllUsedByType(result,ruleSet,typeName);

        FWObjectTypedChildIterator j=obj->findByType(Interface::TYPENAME);
        for ( ; j!=j.end(); ++j )
        {
          if ((ruleSet = (*j)->getFirstByType(InterfacePolicy::TYPENAME))!=NULL)
            findAllUsedByType(result,ruleSet,typeName);
        }

        ruleSet = obj->getFirstByType(NAT::TYPENAME);
        findAllUsedByType(result,ruleSet,typeName);

        ruleSet = obj->getFirstByType(Routing::TYPENAME);
        findAllUsedByType(result,ruleSet,typeName);

        result.sort();
        result.unique();
    }

    return result;
}

int addObjectsToTable(list<FWObject*> &objects,
                      QTableWidget *tbl,
                      int &row,
                      int &col)
{
    int added =0;
    string icon_path="/FWBuilderResources/Type/";

    QPixmap bfr(32,32);
    QPainter bfrp(&bfr);

    for (list<FWObject*>::iterator i=objects.begin(); i!=objects.end(); ++i)
    {
        if (Address::cast(*i)!=NULL && Address::cast(*i)->isAny()) continue;
        if (Service::cast(*i)!=NULL && Service::cast(*i)->isAny()) continue;
        if (Interval::cast(*i)!=NULL && Interval::cast(*i)->isAny()) continue;

        if (col>=tbl->columnCount())
        {
            col = 0;
            row++;
            tbl->insertRow(row);
        }

        string typeName = (*i)->getTypeName();

        QString icn = (":/Icons/"+typeName+"/icon").c_str();
        QPixmap pm;
        if ( ! QPixmapCache::find( icn, pm) )
        {
            pm.load( icn );
            QPixmapCache::insert( icn, pm);
        }

        bfrp.fillRect(0,0,32,32,QColor("white"));
        bfrp.drawPixmap(4,4,pm);

        tbl->setItem(row,col, new QTableWidgetItem(QIcon(bfr),
                           QString::fromUtf8((*i)->getName().c_str())));

        QString descr = FWObjectPropertiesFactory::getObjectProperties(*i);
        QString comment =  QString::fromUtf8((*i)->getComment().c_str());

        // collapse paragraphs
        //comment.replace("\n\n", "&#10;");
        //comment.replace("\n", "");
        //comment.replace("&#10;", "\n");


        tbl->setItem(row,col+1, new QTableWidgetItem(descr));
        tbl->setItem(row,col+2, new QTableWidgetItem(comment));

        //tbl->item(row,col+2)->setWordWrap(true);

        added++;

        if (fwbdebug)
            qDebug("objTbl: row=%d col=%d '%s'",
                   row, col, (*i)->getName().c_str());

        col = col+3;
    }
    return added;
}

void findAllGroups(list<FWObject*> &objects,list<FWObject*> &groups)
{
    if (fwbdebug) qDebug("findAllGroups: arg1 size %d", int(objects.size()));
    for (FWObject::iterator obj=objects.begin(); obj!=objects.end(); ++obj)
    {
        if (fwbdebug) qDebug("   %s",(*obj)->getName().c_str());
        FWObject *o = *obj;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        if (Group::cast(o)!=NULL &&
            std::find(groups.begin(),groups.end(),o)==groups.end())
        {
            groups.push_back(o);
            if (fwbdebug) qDebug("Add group %s to groups",o->getName().c_str());
            findAllGroups(*o,groups);
        }
    }
}

void printFirewall(FWObject *fw,
                   printerStream &pr,
                   PrintingProgressDialog*,
                   bool newPageForSection,
                   ProjectPanel *project)
{
    if (Firewall::cast(fw)==NULL)
        return ;
    list<pixmapOrText> listPT;

    QString txt;

    QString platform = fw->getStr("platform").c_str();
    QString version  = fw->getStr("version").c_str();
    QString readableVersion = getVersionString(platform,version);
    QString hostOS = fw->getStr("host_OS").c_str();

    pr.beginPage();   // resets yPos

    pr.printText(QObject::tr("Firewall name: %1").arg(QString::fromUtf8(fw->getName().c_str())));
    pr.printText(QObject::tr("Platform: ") + platform);
    pr.printText(QObject::tr("Version: ")  + readableVersion);
    pr.printText(QObject::tr("Host OS: ")  + hostOS);
    pr.printText(" ");
    RuleSetView *ruleView =NULL;
//            ppd->genericProgressIndicator(ppdCounter++,QObject::tr("Processing global policy"));
    if (fwbdebug)  qDebug("******** Global policy");


    FWObjectTypedChildIterator j =fw->findByType(Policy::TYPENAME);
    for ( ; j!=j.end(); ++j )
    {
    Policy * pol = Policy::cast(*j) ;
    QString name = QObject::tr("Policy: ");
    name += pol->getName().c_str();
    pr.printText(name);

    ruleView=new PolicyView(project,
        pol,NULL);
    ruleView->setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                          (QSizePolicy::Policy)7) );
    ruleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ruleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ruleView->setMaximumHeight(pr.getPageHeight());

    /*if (fwbdebug) qDebug("Contents: %dx%d",
                         ruleView->contentsWidth(),ruleView->contentsHeight());
    if (fwbdebug) qDebug("Visible: %dx%d",
                         ruleView->visibleWidth(),ruleView->visibleHeight());*/
    if (fwbdebug) qDebug("Viewport: %dx%d",
                         ruleView->viewport()->width(),ruleView->viewport()->height());
    /*if (fwbdebug) qDebug("Clipper: %dx%d",
                         ruleView->clipper()->width(),ruleView->clipper()->height());*/

    if (fwbdebug) qDebug("Size: %dx%d",ruleView->width(),ruleView->height());

//    pr.printPixmap(QPixmap::grabWidget(ruleView,0,0));
    pr.printQTable(ruleView);

    delete ruleView;
    }

    j=fw->findByType(Interface::TYPENAME);
    for ( ; j!=j.end(); ++j )
    {
        Interface       *intf = Interface::cast(*j);
        InterfacePolicy *ip   = InterfacePolicy::cast((*j)->getFirstByType(InterfacePolicy::TYPENAME));
        if (ip)
        {
          QString tabName;
          if ( !intf->getLabel().empty() )
            tabName=QString::fromUtf8(intf->getLabel().c_str());
          else
            tabName=QString::fromUtf8(intf->getName().c_str());

//                ppd->genericProgressIndicator(ppdCounter++,QObject::tr("Processing policy for interface %1").arg(tabName));
          if (fwbdebug)
              qDebug("******** Interface policy for %s",tabName.toLatin1().constData());


          if (newPageForSection)
            {
              pr.flushPage();
              pr.beginPage();   // resets yPos
            } else
            pr.printText(" ");

          pr.printText(QObject::tr("Interface %1").arg(tabName));

          ruleView=new InterfacePolicyView(project, ip,NULL);
          ruleView->setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                                (QSizePolicy::Policy)7) );
          ruleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
          ruleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

          if (fwbdebug) qDebug("%dx%d",ruleView->width(),ruleView->height());

          pr.printQTable(ruleView);
          //        pr.printPixmap(QPixmap::grabWidget(ruleView,0,0));
          delete ruleView;
        }
    }

//            ppd->genericProgressIndicator(ppdCounter++,QObject::tr("Processing NAT rules"));
    if (fwbdebug)  qDebug("******** NAT");
    j=fw->findByType(NAT::TYPENAME);
    for ( ; j!=j.end(); ++j )
    {

    NAT *nat  = NAT::cast(*j);
    if (nat && nat->size()!=0)
    {
        if (newPageForSection)
        {
            pr.flushPage();
            pr.beginPage();   // resets yPos
        } else
            pr.printText(" ");
        QString name = QObject::tr("NAT: ");
        name += nat->getName ().c_str(); 
        pr.printText(name);

        ruleView=new NATView(project, nat,NULL);

        ruleView->setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                              (QSizePolicy::Policy)7) );
        ruleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ruleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        if (fwbdebug) qDebug("%dx%d",ruleView->width(),ruleView->height());

        pr.printQTable(ruleView);
//    pr.printPixmap(QPixmap::grabWidget(ruleView,0,0));
        delete ruleView;
    }
    }
    if (fwbdebug)  qDebug("******** Routing");
    Routing *routing  = Routing::cast(fw->getFirstByType(Routing::TYPENAME));
    if (routing && routing->size()!=0)
    {
        if (newPageForSection)
        {
            pr.flushPage();
            pr.beginPage();   // resets yPos
        } else
            pr.printText(" ");

        pr.printText(QObject::tr("Routing"));

        ruleView=new RoutingView(project, routing,NULL);

        ruleView->setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                              (QSizePolicy::Policy)7) );
        ruleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ruleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        if (fwbdebug) qDebug("%dx%d",ruleView->width(),ruleView->height());

        pr.printQTable(ruleView);
//    pr.printPixmap(QPixmap::grabWidget(ruleView,0,0));
        delete ruleView;
    }
}

void FWWindow::filePrint()
{
    if (!activeProject())
    {
        if (fwbdebug) 
            qDebug("There isn't any selected subwindow");
        return;
    }
    int pageWidth = 0;
    int pageHeight = 0;
    bool  fullPage = false;

    float margin = 0;
    int   resolution = 150;
    bool  printHeader = true;
    bool  printLegend = true;
    bool  printObjects = true;
    bool  newPageForSection = false;
    int   tableResolution = 2;   // 50%, 75%, 100%, 150%, 200%, default 100%

    if (!st->getStr("PrintSetup/newPageForSection").isEmpty())
        newPageForSection = st->getBool("PrintSetup/newPageForSection");

    if (!st->getStr("PrintSetup/printHeader").isEmpty())
        printHeader = st->getBool("PrintSetup/printHeader");

    if (!st->getStr("PrintSetup/printLegend").isEmpty())
        printLegend = st->getBool("PrintSetup/printLegend");

    if (!st->getStr("PrintSetup/printObjects").isEmpty())
        printObjects = st->getBool("PrintSetup/printObjects");

    if (!st->getStr("PrintSetup/tableResolution").isEmpty())
        tableResolution = st->getInt("PrintSetup/tableResolution");

    Ui::pageSetupDialog_q psd;
    QDialog dlg;

    psd.setupUi(&dlg);

    psd.newPageForSection->setChecked(newPageForSection);
    psd.printHeader->setChecked(printHeader);
    psd.printLegend->setChecked(printLegend);
    psd.printObjects->setChecked(printObjects);
    psd.tableResolution->setCurrentIndex(tableResolution);

    if ( dlg.exec() == QDialog::Accepted )
    {
        newPageForSection = psd.newPageForSection->isChecked();
        printHeader       = psd.printHeader->isChecked();
        printLegend       = psd.printLegend->isChecked();
        printObjects      = psd.printObjects->isChecked();
        tableResolution   = psd.tableResolution->currentIndex();

        st->setBool("PrintSetup/newPageForSection",newPageForSection);
        st->setBool("PrintSetup/printHeader",      printHeader      );
        st->setBool("PrintSetup/printLegend",      printLegend      );
        st->setBool("PrintSetup/printObjects",     printObjects     );
        st->setInt("PrintSetup/tableResolution",   tableResolution  );

        switch (tableResolution)
        {
        case 0: resolution = 300; break;
        case 1: resolution = 225; break;
        case 2: resolution = 150; break;
        case 3: resolution = 100; break;
        case 4: resolution = 75;  break;
        }

        st->getPrinterOptions(printer,pageWidth,pageHeight);

        printer->setResolution(resolution);
        printer->setFullPage(fullPage);

        QPrintDialog pdialog(printer, this);

        pdialog.addEnabledOption(QAbstractPrintDialog::PrintPageRange);
        pdialog.setMinMax(1,9999);
        pdialog.setPrintRange(QAbstractPrintDialog::AllPages);

        if (pdialog.exec())
        {
            int fromPage = printer->fromPage();
            int toPage = printer->toPage();
            if (fromPage==0) fromPage=1;
            if (toPage==0) toPage=9999;

            statusBar()->showMessage( "Printing..." );

            PrintingProgressDialog *ppd = new PrintingProgressDialog(this,printer,0,false);

            QString headerText = mw->printHeader();

#if defined(Q_OS_MACX)
            printerStream pr(printer,margin,printHeader,headerText,NULL);
#else
            printerStream pr(printer,margin,printHeader,headerText,ppd);
            ppd->show();
#endif
            pr.setFromTo(fromPage,toPage);

            if ( !pr.begin())
            {
                ppd->hide();
                delete ppd;
                return;
            }

            int leftMargin = printer->paperRect().left() - printer->pageRect().left();
            int topMargin = printer->paperRect().top() - printer->pageRect().top();

            if (fwbdebug)
            {
                qDebug("Margins: %d,%d",leftMargin,topMargin);
            }

//            int ppdCounter = 1;
            printFirewall(activeProject()->getCurrentRuleSet()->getParent(),pr,ppd,
                  newPageForSection, activeProject());

            if (printLegend)
            {
                if (fwbdebug) qDebug("******** Legend");

                if (newPageForSection)
                {
                    pr.flushPage();
                    pr.beginPage();   // resets yPos
                } else
                    pr.printText("\n");

                pr.printText(tr("Legend"));
                pr.printText(" ");

                QTableWidget legendTbl(1,2);
                legendTbl.resize(pr.getWorkspaceWidth(),
                                 pr.getWorkspaceHeight());
                legendTbl.setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                                      (QSizePolicy::Policy)7) );
                legendTbl.setShowGrid(false);
                legendTbl.setFrameStyle(QFrame::NoFrame | QFrame::Plain);

                legendTbl.horizontalHeader()->hide();
                legendTbl.verticalHeader()->hide();
                //legendTbl.setTopMargin(0);
                //legendTbl.setLeftMargin(0);

                legendTbl.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                legendTbl.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


                string icon_path="/FWBuilderResources/Type/";
                int row=0;
                int col=0;

                QPixmap pm;

                QPixmap bfr(32,32);
                QPainter bfrp(&bfr);

                for (int i=0; !legendList[i].isEmpty(); ++i,++i)
                {
                  if (row>=legendTbl.rowCount()) legendTbl.insertRow(row);

                    QString typeName=legendList[i];
                    QString objName=legendList[i+1];

                    if (typeName==CustomService::TYPENAME)
                    {
                        col++;
                        row=0;
                    }

                    if (fwbdebug)
                        qDebug("Legend table: row=%d col=%d %s %s",
                               row,col,typeName.toAscii().constData(),objName.toAscii().constData());

//                    pm.load( Resources::global_res->getResourceStr(
//                        icon_path+string(typeName.toLatin1().constData())+"/icon").c_str() );

                    QString icn = ":/Icons/"+typeName+"/icon";
                    QPixmap pm;
                    if ( ! QPixmapCache::find( icn, pm) )
                    {
                        pm.load( icn );
                        QPixmapCache::insert( icn, pm);
                    }

                    bfrp.fillRect(0,0,32,32,QColor(Qt::white));
                    bfrp.drawPixmap(4,4,pm);

                    QTableWidgetItem *itm = new QTableWidgetItem;
                    itm->setIcon(QIcon(bfr));
                    itm->setText(objName);
                    legendTbl.setItem(row, col, itm);

                    row++;
                }

                legendTbl.resizeColumnToContents(0);
                legendTbl.resizeColumnToContents(1);

                for (int i=0; i<legendTbl.rowCount(); ++i)
                    legendTbl.resizeRowToContents(i);

                QSize sh = legendTbl.sizeHint();
                legendTbl.resize(sh.width(),sh.height());
                if (fwbdebug) qDebug("legendTbl size: %dx%d",
                                     legendTbl.width(),legendTbl.height());

                pr.printQTable(&legendTbl, false, false);
            }

/* *********** Objects ************** */

            if (printObjects)
            {
                if (fwbdebug)
                    qDebug("******** Objects");

                if (newPageForSection)
                {
                    pr.flushPage();
                    pr.beginPage();   // resets yPos
                } else
                    pr.printText("\n");

                pr.printText(tr("Objects"));
                pr.printText(" ");

                bool haveObjGroups = false;
                bool haveSrvGroups = false;

                QTableWidget fwObjTbl(1,3);
                //fwObjTbl.setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                //                                 (QSizePolicy::Policy)7) );
                //fwObjTbl.setColumnStretchable(2, true);
                fwObjTbl.setFrameStyle(QFrame::NoFrame | QFrame::Plain);

                fwObjTbl.horizontalHeader()->hide();
                fwObjTbl.verticalHeader()->hide();
                fwObjTbl.setContentsMargins(0,0,0,0);

                fwObjTbl.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                fwObjTbl.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                fwObjTbl.resize(pr.getWorkspaceWidth(),
                                pr.getWorkspaceHeight());

                list<FWObject*> objects;
                QString descr;

                int row = 0;
                int col = 0;
                int added = 0;

                objects.clear();

                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  Firewall::TYPENAME);
                added = addObjectsToTable(objects, &fwObjTbl, row, col);
                if (fwbdebug) qDebug("Objects table: added %d firewalls",added);

                for (int i=0; i<fwObjTbl.columnCount(); ++i)
                    fwObjTbl.resizeColumnToContents(i);
                for (int i=0; i<fwObjTbl.rowCount(); ++i)
                    fwObjTbl.resizeRowToContents(i);

                pr.printQTable(&fwObjTbl, false, false);
                pr.printText(" ");

                QTableWidget objTbl(1,6);
                objTbl.resize(pr.getWorkspaceWidth(),
                              pr.getWorkspaceHeight());
                objTbl.setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                                   (QSizePolicy::Policy)7) );
                objTbl.setFrameStyle(QFrame::NoFrame | QFrame::Plain);

                objTbl.horizontalHeader()->hide();
                objTbl.verticalHeader()->hide();
                setContentsMargins ( 0,0,0,0 );

                objTbl.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                objTbl.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

                row = 0;
                col = 0;
                added = 0;

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  Host::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug) qDebug("Objects table: added %d hosts",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  Network::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d networks",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  IPv4::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d addresses",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  IPv6::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d ipv6 addresses",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  AddressRange::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d address ranges",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  ObjectGroup::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d obj groups",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                    haveObjGroups = true;
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  IPService::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d ip services",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  ICMPService::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d icmp services",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  ICMP6Service::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d icmp6 services",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  TCPService::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d tcp services",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  UDPService::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d udp services",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  CustomService::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d custom services",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  ServiceGroup::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d srv groups",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                    haveSrvGroups = true;
                }

                objects.clear();
                findAllUsedByType(objects,activeProject()->getVisibleFirewall(),
                                  Interval::TYPENAME);
                added=addObjectsToTable(objects,&objTbl,row,col);
                if (fwbdebug)
                    qDebug("Objects table: added %d time intervals",added);
                if (added)
                {
                    if (col!=0)
                    {
                        row++; col=0;
                        objTbl.insertRow(row);
                    }
                }

                for (int i=0; i<objTbl.columnCount(); ++i) objTbl.resizeColumnToContents(i);
                for (int i=0; i<objTbl.rowCount(); ++i) objTbl.resizeRowToContents(i);

                pr.printQTable(&objTbl, false, false);
//                pr.printPixmap(QPixmap::grabWidget(&objTbl,0,0));

                if (haveObjGroups || haveSrvGroups)
                {
                    if (fwbdebug) qDebug("******** Groups");

                    pr.printText("\n");
                    pr.printText(tr("Groups"));
                    pr.printText(" ");

                    list<FWObject*> groups;

                    objects.clear();
                    findAllUsedByType(objects,activeProject()->getVisibleFirewall(),ObjectGroup::TYPENAME);
                    findAllGroups(objects,groups);

                    objects.clear();
                    findAllUsedByType(objects,activeProject()->getVisibleFirewall(),ServiceGroup::TYPENAME);
                    findAllGroups(objects,groups);

                    for (FWObject::iterator obj=groups.begin(); obj!=groups.end(); ++obj)
                    {
                        QTableWidget objTbl(1,6);
                        objTbl.setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                                           (QSizePolicy::Policy)7) );
                        objTbl.setFrameStyle(QFrame::NoFrame | QFrame::Plain);

                        objTbl.setContentsMargins(0,0,0,0);
                        objTbl.horizontalHeader()->hide();
                        objTbl.verticalHeader()->hide();

                        objTbl.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        objTbl.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

                        row = 0;
                        col = 0;
                        list<FWObject*> groupMembers;

                        for (FWObject::iterator j=(*obj)->begin();
                             j!=(*obj)->end(); ++j)
                        {
                            FWObject *o = *j;
                            if (FWReference::cast(o)!=NULL)
                                o=FWReference::cast(o)->getPointer();
                            groupMembers.push_back(o);
                        }

                        added=addObjectsToTable(groupMembers,&objTbl,row,col);
                        if (fwbdebug) qDebug("Group %s: added %d group members",
                                             (*obj)->getName().c_str(),added);

                        if (added == 0)
                        {
                            objTbl.setItem(row,col, new QTableWidgetItem(tr("EMPTY")) );
                        }

                        for (int i=0; i<objTbl.columnCount(); ++i)
                            objTbl.resizeColumnToContents(i);
                        for (int i=0; i<objTbl.rowCount(); ++i)
                            objTbl.resizeRowToContents(i);

                        pr.printText((*obj)->getName().c_str());
                        pr.printQTable(&objTbl, false, false);
                        pr.printText("\n");
                    }
                }
            }

            ppd->hide();
            delete ppd;

            pr.end();

            if (printer->printerState() == QPrinter::Aborted)
            {
                statusBar()->showMessage( tr("Printing aborted"), 2000 );
                QMessageBox::information(
                    this,"Firewall Builder",
                    tr("Printing aborted"),
                    tr("&Continue"), QString::null,QString::null,
                    0, 1 );
            } else
                statusBar()->showMessage( tr("Printing completed"), 2000 );

        } else
        {
            statusBar()->showMessage( tr("Printing <ed"), 2000 );
        }

        st->setPrinterOptions(printer,pageWidth,pageHeight);
    }

}


void FWWindow::printFirewallFromFile (QString fileName,
                                      QString firewallName,
                                      QString outputFileName)
{
    if (outputFileName=="")
    {
        outputFileName = "print.pdf";
    }
    if (firewallName=="")
    {
        return ;
    }
    if (fileName=="")
    {
        return;
    }
    FWObjectDatabase * objdb = new FWObjectDatabase();
    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    objdb->load(fileName.toLatin1().constData(), NULL,librespath);
    FWObject* obj = objdb->findObjectByName(Firewall::TYPENAME,firewallName.toAscii().data());
    if (obj!=NULL)
    {
        int pageWidth = 0;
        int pageHeight = 0;
        bool  fullPage = false;
    
        float margin = 0;
        int   resolution = 150;
        bool  printHeader = true;
        bool  printLegend = true;
        bool  printObjects = true;
        bool  newPageForSection = false;
        int   tableResolution = 2;   // 50%, 75%, 100%, 150%, 200%, default 100%
    
        if (!st->getStr("PrintSetup/newPageForSection").isEmpty())
            newPageForSection = st->getBool("PrintSetup/newPageForSection");
    
        if (!st->getStr("PrintSetup/printHeader").isEmpty())
            printHeader = st->getBool("PrintSetup/printHeader");
    
        if (!st->getStr("PrintSetup/printLegend").isEmpty())
            printLegend = st->getBool("PrintSetup/printLegend");
    
        if (!st->getStr("PrintSetup/printObjects").isEmpty())
            printObjects = st->getBool("PrintSetup/printObjects");
    
        if (!st->getStr("PrintSetup/tableResolution").isEmpty())
            tableResolution = st->getInt("PrintSetup/tableResolution");
    
        switch (tableResolution)
        {
        case 0: resolution = 300; break;
        case 1: resolution = 225; break;
        case 2: resolution = 150; break;
        case 3: resolution = 100; break;
        case 4: resolution = 75;  break;
        }

        st->getPrinterOptions(printer,pageWidth,pageHeight);

        printer->setResolution(resolution);
        printer->setFullPage(fullPage);
        printer->setOutputFileName (outputFileName);
            int fromPage = 1;
            int toPage = 9999;


            QString headerText = fileName; //mw->printHeader();
            printerStream pr(printer,margin,printHeader,headerText,NULL);

            pr.setFromTo(fromPage,toPage);

            if ( !pr.begin())
            {
                return;
            }

            int leftMargin = printer->paperRect().left() - printer->pageRect().left();
            int topMargin = printer->paperRect().top() - printer->pageRect().top();

            if (fwbdebug)
            {
                qDebug("Margins: %d,%d",leftMargin,topMargin);
            }

//            int ppdCounter = 1;
            printFirewall(obj,pr,NULL,newPageForSection, NULL);

            if (printLegend)
            {
                if (fwbdebug) qDebug("******** Legend");

                if (newPageForSection)
                {
                    pr.flushPage();
                    pr.beginPage();   // resets yPos
                } else
                    pr.printText("\n");

                pr.printText(tr("Legend"));
                pr.printText(" ");

                QTableWidget legendTbl(1,2);
                legendTbl.resize(pr.getWorkspaceWidth(),
                                 pr.getWorkspaceHeight());
                legendTbl.setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                                      (QSizePolicy::Policy)7) );
                legendTbl.setShowGrid(false);
                legendTbl.setFrameStyle(QFrame::NoFrame | QFrame::Plain);

                legendTbl.horizontalHeader()->hide();
                legendTbl.verticalHeader()->hide();
                //legendTbl.setTopMargin(0);
                //legendTbl.setLeftMargin(0);

                legendTbl.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                legendTbl.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


                string icon_path="/FWBuilderResources/Type/";
                int row=0;
                int col=0;

                QPixmap pm;

                QPixmap bfr(32,32);
                QPainter bfrp(&bfr);

                for (int i=0; !legendList[i].isEmpty(); ++i,++i)
                {
                  if (row>=legendTbl.rowCount()) legendTbl.insertRow(row);

                    QString typeName=legendList[i];
                    QString objName=legendList[i+1];

                    if (typeName==CustomService::TYPENAME)
                    {
                        col++;
                        row=0;
                    }

                    if (fwbdebug)
                        qDebug("Legend table: row=%d col=%d %s %s",
                               row,col,typeName.toAscii().constData(),objName.toAscii().constData());

//                    pm.load( Resources::global_res->getResourceStr(
//                        icon_path+string(typeName.toLatin1().constData())+"/icon").c_str() );

                    QString icn = ":/Icons/"+typeName+"/icon";
                    QPixmap pm;
                    if ( ! QPixmapCache::find( icn, pm) )
                    {
                        pm.load( icn );
                        QPixmapCache::insert( icn, pm);
                    }

                    bfrp.fillRect(0,0,32,32,QColor(Qt::white));
                    bfrp.drawPixmap(4,4,pm);

                    QTableWidgetItem *itm = new QTableWidgetItem;
                    itm->setIcon(QIcon(bfr));
                    itm->setText(objName);
                    legendTbl.setItem(row, col, itm);

                    row++;
                }

                legendTbl.resizeColumnToContents(0);
                legendTbl.resizeColumnToContents(1);

                for (int i=0; i<legendTbl.rowCount(); ++i)
                    legendTbl.resizeRowToContents(i);

                QSize sh = legendTbl.sizeHint();
                legendTbl.resize(sh.width(),sh.height());
                if (fwbdebug) qDebug("legendTbl size: %dx%d",
                                     legendTbl.width(),legendTbl.height());

                pr.printQTable(&legendTbl, false, false);
            }
    }
    else
    {   
        QString errmes = "Error: can't find firewall ";
        errmes += firewallName ;
        qDebug (errmes.toAscii().data());
    }
}

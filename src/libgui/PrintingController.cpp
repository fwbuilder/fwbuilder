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
#include "platforms.h"

#include "RuleSetView.h"
#include "FWBSettings.h"
#include "FWObjectPropertiesFactory.h"
#include "ProjectPanel.h"

#include "PrintingController.h"

#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Interface.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
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

#include <qstring.h>
#include <QPixmapCache>
#include <QHeaderView>

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



pixmapOrText::pixmapOrText(const QString &t,const QPixmap &w) :
    text(t), pixmap(w)
{ }


list<FWObject*> PrintingController::findAllUsedByType(list<FWObject*> &result,
                                                      FWObject *obj,
                                                      const string &type_name)
{
    if (RuleElement::cast(obj)!=nullptr)
    {
        for (list<FWObject*>::iterator m=obj->begin(); m!=obj->end(); m++)
        {
            FWObject *o=*m;
            if (FWReference::cast(o)!=nullptr)
                o=FWReference::cast(o)->getPointer();
            if (o->getTypeName()==type_name)
                result.push_back(o);
        }
    }

    if (RuleSet::cast(obj)!=nullptr)
    {
        for (list<FWObject*>::iterator m=obj->begin(); m!=obj->end(); m++)
        {
            if (Rule::cast(*m)!=nullptr)
            {
                for (list<FWObject*>::iterator n=(*m)->begin();
                     n!=(*m)->end(); n++)
                {
                    if (RuleElement::cast(*n)!=nullptr)
                    {
                        findAllUsedByType(result,*n,type_name);
                    }
                }
            }
        }
    }

    if (Firewall::cast(obj))  // cast matches Firewall and Cluster
    {
        FWObject *ruleSet;
        const char* const child_types[] =
            {Policy::TYPENAME, NAT::TYPENAME, Routing::TYPENAME, nullptr};
        for (const char* const *cptr = child_types; *cptr; ++cptr)
        {
            for (FWObjectTypedChildIterator it = obj->findByType(*cptr);
                 it != it.end(); ++it)
            {
                ruleSet = *it;
                findAllUsedByType(result, ruleSet, type_name);
            }
        }

        result.sort();
        result.unique();
    }

    return result;
}

int PrintingController::addObjectsToTable(list<FWObject*> &objects,
                                          QTableWidget *tbl,
                                          int &row, int &col)
{
    int added =0;
    string icon_path="/FWBuilderResources/Type/";

    QPixmap bfr(32,32);
    QPainter bfrp(&bfr);

    for (list<FWObject*>::iterator i=objects.begin(); i!=objects.end(); ++i)
    {
        if (Address::cast(*i)!=nullptr && Address::cast(*i)->isAny()) continue;
        if (Service::cast(*i)!=nullptr && Service::cast(*i)->isAny()) continue;
        if (Interval::cast(*i)!=nullptr && Interval::cast(*i)->isAny()) continue;

        if (col>=tbl->columnCount())
        {
            col = 0;
            row++;
            tbl->insertRow(row);
        }

        string type_name = (*i)->getTypeName();

        QString icn = (":/Icons/"+type_name+"/icon").c_str();
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

bool PrintingController::addObjectsByTypeToTable(FWObject *parent,
                                                 const string &type_name,
                                                 QTableWidget *tbl,
                                                 int &row, int &col)
{
    list<FWObject*> objects;
    findAllUsedByType(objects, parent, type_name);
    int added = addObjectsToTable(objects, tbl, row, col);
    if (fwbdebug) qDebug("Objects table: type %s, added %d",
                         type_name.c_str(), added);
    if (added)
    {
        if (col!=0)
        {
            row++; col=0;
            tbl->insertRow(row);
        }
        return true;
    }
    return false;
}

void PrintingController::findAllGroups(list<FWObject*> &objects,
                                       list<FWObject*> &groups)
{
    if (fwbdebug) qDebug("findAllGroups: arg1 size %d", int(objects.size()));
    for (FWObject::iterator obj=objects.begin(); obj!=objects.end(); ++obj)
    {
        if (fwbdebug) qDebug("   %s",(*obj)->getName().c_str());
        FWObject *o = *obj;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        if (Group::cast(o)!=nullptr &&
            std::find(groups.begin(),groups.end(),o)==groups.end())
        {
            groups.push_back(o);
            if (fwbdebug) qDebug("Add group %s to groups",o->getName().c_str());
            findAllGroups(*o,groups);
        }
    }
}

void PrintingController::printRuleSet(FWObject *fw,
                                      const string &ruleset_type_name,
                                      ProjectPanel *project)
{
    FWObjectTypedChildIterator j =fw->findByType(ruleset_type_name);
    for ( ; j!=j.end(); ++j )
    {
        RuleSet * ruleset = RuleSet::cast(*j) ;
        QString name = QObject::tr("Policy: ");
        name += ruleset->getName().c_str();

        pr->printText("\n");
        pr->printText(name);

        RuleSetView *ruleView = RuleSetView::getRuleSetViewByType(
            project, ruleset, nullptr);
        ruleView->setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                              (QSizePolicy::Policy)7) );
        ruleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ruleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        ruleView->setMaximumHeight(pr->getPageHeight());

        ruleView->restoreCollapsedGroups();

        if (fwbdebug)
            qDebug("Viewport: %dx%d",
                   ruleView->viewport()->width(),ruleView->viewport()->height());

        if (fwbdebug) qDebug("Size: %dx%d",ruleView->width(),ruleView->height());

        pr->printRuleSetView(ruleView);

        delete ruleView;
    }
}

void PrintingController::printFirewall(FWObject *fw, ProjectPanel *project)
{
    if (Firewall::cast(fw)==nullptr) return ;

    string platform = fw->getStr("platform");
    string version  = fw->getStr("version");
    string hostOS = fw->getStr("host_OS");

    QString readableVersion = getVersionString(QString(platform.c_str()),
                                               QString(version.c_str()));
    pr->beginPage();   // resets yPos

    pr->printText(QObject::tr("Firewall name: %1").arg(
                     QString::fromUtf8(fw->getName().c_str())));
    pr->printText(QObject::tr("Platform: ") + platform.c_str());
    pr->printText(QObject::tr("Version: ")  + readableVersion);
    pr->printText(QObject::tr("Host OS: ")  + hostOS.c_str());

    const char* const child_types[] =
        {Policy::TYPENAME, NAT::TYPENAME, Routing::TYPENAME, nullptr};

    for (const char* const *cptr = child_types; *cptr; ++cptr)
    {
        if (fwbdebug)  qDebug("******** %s", *cptr);
        printRuleSet(fw, *cptr, project);
    }
}

void PrintingController::printLegend(bool newPageForSection)
{
    if (fwbdebug) qDebug("******** Legend");

    if (newPageForSection)
    {
        pr->flushPage();
        pr->beginPage();   // resets yPos
    } else
        pr->printText("\n");

    pr->printText(QObject::tr("Legend"));
    pr->printText(" ");

    QTableWidget *legendTbl = new QTableWidget(1,2);
    configureQTableForPrint(legendTbl);

    string icon_path="/FWBuilderResources/Type/";
    int row = 0;
    int col = 0;

    QPixmap pm;

    QPixmap bfr(32,32);
    QPainter bfrp(&bfr);

    for (int i=0; !legendList[i].isEmpty(); ++i,++i)
    {
        if (row >= legendTbl->rowCount()) legendTbl->insertRow(row);

        QString type_name = legendList[i];
        QString objName = legendList[i+1];

        if (type_name==CustomService::TYPENAME)
        {
            col++;
            row=0;
        }

        if (fwbdebug)
            qDebug("Legend table: row=%d col=%d %s %s",
                   row, col, type_name.toLatin1().constData(),
                   objName.toLatin1().constData());

        QString icn = ":/Icons/"+type_name+"/icon";
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
        legendTbl->setItem(row, col, itm);

        row++;
    }

    legendTbl->resizeColumnToContents(0);
    legendTbl->resizeColumnToContents(1);

    for (int i=0; i<legendTbl->rowCount(); ++i)
        legendTbl->resizeRowToContents(i);

    QSize sh = legendTbl->sizeHint();
    legendTbl->resize(sh.width(),sh.height());
    if (fwbdebug) qDebug("legendTbl size: %dx%d",
                         legendTbl->width(),legendTbl->height());

    pr->printQTable(legendTbl, false, false);
}

void PrintingController::printObjects(FWObject *firewall_to_print,
                                      bool newPageForSection)
{
    if (fwbdebug) qDebug("******** Objects");

    if (newPageForSection)
    {
        pr->flushPage();
        pr->beginPage();   // resets yPos
    } else
        pr->printText("\n");

    pr->printText(QObject::tr("Objects"));
    pr->printText(" ");

    bool haveObjGroups = false;
    bool haveSrvGroups = false;

    QTableWidget *fwObjTbl = new QTableWidget(1,3);
    configureQTableForPrint(fwObjTbl);

    QString descr;

    int row = 0;
    int col = 0;

    addObjectsByTypeToTable(firewall_to_print, Firewall::TYPENAME,
                            fwObjTbl, row, col);

    for (int i=0; i<fwObjTbl->columnCount(); ++i)
        fwObjTbl->resizeColumnToContents(i);
    for (int i=0; i<fwObjTbl->rowCount(); ++i)
        fwObjTbl->resizeRowToContents(i);

    QSize sh = fwObjTbl->sizeHint();
    fwObjTbl->resize(sh.width(), sh.height());
    pr->printQTable(fwObjTbl, false, false);
    pr->printText(" ");

    QTableWidget *objTbl = new QTableWidget(1,6);
    configureQTableForPrint(objTbl);

    row = 0;
    col = 0;

    addObjectsByTypeToTable(firewall_to_print, Host::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print, Network::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print, NetworkIPv6::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print, IPv4::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print, IPv6::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print,
                            AddressRange::TYPENAME,
                            objTbl, row, col);

    haveObjGroups = addObjectsByTypeToTable(
        firewall_to_print, ObjectGroup::TYPENAME,
        objTbl, row, col);

    addObjectsByTypeToTable(firewall_to_print, IPService::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print,
                            ICMPService::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print,
                            ICMP6Service::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print, TCPService::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print, UDPService::TYPENAME,
                            objTbl, row, col);
    addObjectsByTypeToTable(firewall_to_print,
                            CustomService::TYPENAME,
                            objTbl, row, col);

    haveSrvGroups = addObjectsByTypeToTable(
        firewall_to_print, ServiceGroup::TYPENAME,
        objTbl, row, col);

    addObjectsByTypeToTable(firewall_to_print, Interval::TYPENAME,
                            objTbl, row, col);

    for (int i=0; i<objTbl->columnCount(); ++i)
        objTbl->resizeColumnToContents(i);
    for (int i=0; i<objTbl->rowCount(); ++i)
        objTbl->resizeRowToContents(i);

    sh = objTbl->sizeHint();
    objTbl->resize(sh.width(), sh.height());
    pr->printQTable(objTbl, false, false);

    if (haveObjGroups || haveSrvGroups)
    {
        if (fwbdebug) qDebug("******** Groups");

        pr->printText("\n");
        pr->printText(QObject::tr("Groups"));
        pr->printText(" ");

        list<FWObject*> groups;
        list<FWObject*> objects;

        findAllUsedByType(objects, firewall_to_print,
                          ObjectGroup::TYPENAME);
        findAllGroups(objects,groups);

        objects.clear();
        findAllUsedByType(objects, firewall_to_print,
                          ServiceGroup::TYPENAME);
        findAllGroups(objects,groups);

        for (FWObject::iterator obj=groups.begin();
             obj!=groups.end(); ++obj)
        {
            QTableWidget *objTbl = new QTableWidget(1,6);
            configureQTableForPrint(objTbl);

            row = 0;
            col = 0;
            list<FWObject*> groupMembers;

            for (FWObject::iterator j=(*obj)->begin();
                 j!=(*obj)->end(); ++j)
            {
                FWObject *o = *j;
                if (FWReference::cast(o)!=nullptr)
                    o=FWReference::cast(o)->getPointer();
                groupMembers.push_back(o);
            }

            int added = addObjectsToTable(
                groupMembers, objTbl, row, col);
            if (fwbdebug) qDebug("Group %s: added %d group members",
                                 (*obj)->getName().c_str(),added);

            if (added == 0)
            {
                objTbl->setItem(row, col,
                                new QTableWidgetItem(QObject::tr("EMPTY")) );
            }

            for (int i=0; i<objTbl->columnCount(); ++i)
                objTbl->resizeColumnToContents(i);
            for (int i=0; i<objTbl->rowCount(); ++i)
                objTbl->resizeRowToContents(i);

            pr->printText((*obj)->getName().c_str());
            pr->printQTable(objTbl, false, false);
            pr->printText("\n");
        }
    }

}

void PrintingController::configureQTableForPrint(QTableWidget *tbl)
{
    tbl->resize(pr->getWorkspaceWidth(), pr->getWorkspaceHeight());
    tbl->setSizePolicy( QSizePolicy( (QSizePolicy::Policy)7,
                                    (QSizePolicy::Policy)7) );
    tbl->setShowGrid(false);
    tbl->setFrameStyle(QFrame::NoFrame | QFrame::Plain);

    tbl->horizontalHeader()->hide();
    tbl->verticalHeader()->hide();

    tbl->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tbl->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


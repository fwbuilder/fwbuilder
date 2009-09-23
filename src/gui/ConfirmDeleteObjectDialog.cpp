/*

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"
#include "../../definitions.h"

#include "ConfirmDeleteObjectDialog.h"
#include "FindWhereUsedWidget.h"
#include "ObjectManipulator.h"
#include "FWWindow.h"
#include "FWBTree.h"


#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Library.h"


//#include <qhbox.h>
#include <qlayout.h>
#include <qlistwidget.h>
#include <qpixmapcache.h>

#include <vector>

using namespace std;
using namespace libfwbuilder;

ConfirmDeleteObjectDialog::ConfirmDeleteObjectDialog(QWidget*p) : QDialog(p)
{
    m_dialog = new Ui::ConfirmDeleteObjectDialog_q;
    m_dialog->setupUi(this);
}

ConfirmDeleteObjectDialog::~ConfirmDeleteObjectDialog()
{
    delete m_dialog;
}

void ConfirmDeleteObjectDialog::load(vector<FWObject *> objs)
{
    if (objs.size()==0) return;

    vector<FWObject*>::iterator i;
    for( i=objs.begin(); i!=objs.end(); ++i)
    {
        findForObject(*i);
    }
}

void ConfirmDeleteObjectDialog::findForObject(FWObject *obj)
{
    set<FWObject*> resset;
    //objectsView->clear();
    //mapping.clear();
    //resset.clear();

    QPixmap pm0;
    QString icn_file = (":/Icons/" + obj->getTypeName() + "/icon-tree").c_str();

    if ( ! QPixmapCache::find( icn_file, pm0) )
    {
        pm0.load( icn_file );
        QPixmapCache::insert( icn_file, pm0);
    }

    FWObjectDatabase *db = obj->getRoot();
    assert(db);

    db->findWhereObjectIsUsed(obj, db, resset);
    FindWhereUsedWidget::humanizeSearchResults(resset);

    if (fwbdebug)
    {
        qDebug(QString("ConfirmDeleteObjectDialog::findForObject   deleting obj=%1").
               arg(obj->getName().c_str()).toAscii().constData());
        qDebug(QString("resset.size()==%1").arg(resset.size()).toAscii().constData());
    }

    set<FWObject*>::iterator i=resset.begin();

    int itemCounter = 0;

    for (;i!=resset.end();++i)
    {
        FWObject* o = *i;

        // one of the objects returned by findWhereObjectIsUsed is a group
        // object <o> belongs to. We need to skip this group.
        //if (findRef(obj,o)==NULL) continue;
        QTreeWidgetItem *item = FindWhereUsedWidget::createQTWidgetItem(obj, o);
        if (item==NULL) continue;
        m_dialog->objectsView->addTopLevelItem(item);
        itemCounter++;
    }

    if (itemCounter==0)
    {
        QStringList qsl;
        qsl << QString::fromUtf8( obj->getName().c_str()) << ""
            << tr("Not used anywhere");
        QTreeWidgetItem *item = new QTreeWidgetItem(m_dialog->objectsView, qsl);
        item->setIcon(0,QIcon(pm0));
    }

    m_dialog->objectsView->resizeColumnToContents(0);
    m_dialog->objectsView->resizeColumnToContents(1);
}

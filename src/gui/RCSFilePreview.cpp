/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland vadim@fwbuilder.org

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

#include "RCS.h"
#include "RCSFilePreview.h"

#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/FWException.h"

#include <QTreeWidget>
#include <qregexp.h>
#include <qpushbutton.h>
// #include <qtextbrowser.h>

#include <string>
#include <iostream>

using namespace std;
using namespace libfwbuilder;

int RCSViewItem::compare(QTreeWidgetItem *itm, int col, bool ) const
{
    QString rev1 = text(col);
    QString rev2 = itm->text(col);

    for(int i=1; ; i++)
    {
        QString v1 = rev1.section(".",i,i);
        QString v2 = rev2.section(".",i,i);
        if (v1=="" && v2=="") return 0;
        if (v1==v2) continue;
        if (v1=="" && v2!="") return -1;
        if (v1!="" && v2=="") return 1;
        if (v1.toInt()>v2.toInt()) return 1;
        if (v1.toInt()<v2.toInt()) return -1;
        i++;
    }
    return 0;
}

RCSFilePreview::RCSFilePreview(QWidget *parent): QDialog(parent)
{
    if (fwbdebug) qDebug("RCSFilePreview: constructor");

    m_widget = new Ui::RCSFilePreview_q;
    m_widget->setupUi(this);

    connect( m_widget->cancelButton, SIGNAL( clicked() ),
             this, SLOT( reject() ) );
    connect( m_widget->RCSTreeView,
             SIGNAL( itemActivated( QTreeWidgetItem*, int ) ),
             this, SLOT( accept() ) );

    m_widget->RCSTreeView->setAllColumnsShowFocus( true );
    m_widget->RCSTreeView->setSelectionMode(
        QAbstractItemView::SingleSelection );
    m_widget->RCSTreeView->setRootIsDecorated( FALSE );
//    m_widget->RCSTreeView->sortByColumn( 0, Qt::AscendingOrder );

    if (fwbdebug) qDebug("RCSFilePreview: constructor done");

    rcs = NULL;
    RO = false;
}

RCSFilePreview::~RCSFilePreview()
{
    if (fwbdebug) qDebug("~RCSFilePreview()  rcs=%p", rcs);
//    if (rcs!=NULL) delete rcs;
}

void RCSFilePreview::openReadOnly()
{
    if (rcs!=NULL) rcs->setRO(true);
    RO = true;
    accept();
}

void RCSFilePreview::openFile()
{
    accept();
}

void RCSFilePreview::selectedRevision(QTreeWidgetItem *itm)
{
    if (itm == m_widget->RCSTreeView->topLevelItem(0)) return;

    QString rev = itm->text(0);
    assert(rcs!=NULL);
    rcs->setSelectedRev(rev);
    m_widget->comment->setText( rcsComments[rev] );
    if (fwbdebug)
        qDebug("RCSFilePreview::selectedRevision : %s",
               rev.toAscii().constData());
}

bool RCSFilePreview::showFileRLog( const QString &filename )
{
    if (fwbdebug) qDebug("RCSFilePreview::showFileRLog filename=%s rcs=%p",
                         filename.toLocal8Bit().constData(),rcs);

    m_widget->RCSTreeView->clear();

    if (rcs!=NULL) delete rcs;
    rcs = new RCS(filename);

    if (rcs->revisions.size()==0)
    {
        QTreeWidgetItem *itm = new QTreeWidgetItem(m_widget->RCSTreeView);
        itm->setText( 0, tr("File is not in RCS") );
//        addToRCS->setEnabled(true);
        m_widget->comment->setText("");

        return false;
    }
//    addToRCS->setEnabled(false);

    QTreeWidgetItem *rootItm = new QTreeWidgetItem( m_widget->RCSTreeView );
    rootItm->setText(
        0, filename.right( filename.length()-filename.lastIndexOf("/")-1 ) );
    rootItm->setExpanded(true);

    rcsComments.clear();

    QList<Revision>::iterator i;
    QList<RCSViewItem*> itemList;
    QList<RCSViewItem*>::iterator ili;
    RCSViewItem* lastItem = NULL;

    for (i=rcs->revisions.begin(); i!=rcs->revisions.end(); ++i)
    {
        rcsComments[(*i).rev]=(*i).log;

        if ((*i).rev.indexOf(QRegExp("^[0-9]+\\.[0-9]+$"))!=-1)
        {
            RCSViewItem *itm = new RCSViewItem( rootItm );
            itm->setText( 0, (*i).rev       );
            itm->setText( 1, (*i).date      );
            itm->setText( 2, (*i).author    );
            itm->setText( 3, QString("    ")+(*i).locked_by );

            itemList.push_back(itm);
            lastItem = itm;
        }

        if ((*i).rev.indexOf(QRegExp("^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+"))!=-1)
        {
            QString branch_root = (*i).rev.section(".",0,1);
            for (ili=itemList.begin(); ili!=itemList.end(); ++ili)
                if ((*ili)->text(0) == branch_root)
                {
                    QTreeWidgetItem   *br = *ili;
                    if (br!=NULL)
                    {
                        RCSViewItem *itm=new RCSViewItem(br);
                        itm->setText( 0, (*i).rev       );
                        itm->setText( 1, (*i).date      );
                        itm->setText( 2, (*i).author    );
                        itm->setText( 3, QString("    ")+(*i).locked_by );
                    }
                }
        }
    }

    m_widget->RCSTreeView->scrollToItem( lastItem );

    m_widget->RCSTreeView->expandAll();
//    m_widget->RCSTreeView->sortByColumn(0, Qt::AscendingOrder);
    m_widget->RCSTreeView->resizeColumnToContents ( 0 );
    m_widget->RCSTreeView->resizeColumnToContents ( 1 );

    lastItem->setSelected( true );
    m_widget->RCSTreeView->setCurrentItem( lastItem );

    return true;
}

RCS* RCSFilePreview::getSelectedRev()
{

    if (fwbdebug) qDebug("RCSFilePreview::getSelectedRev rcs=%p",rcs);

    return rcs;

}





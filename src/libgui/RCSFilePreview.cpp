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

#include "global.h"

#include "RCS.h"
#include "RCSFilePreview.h"
#include "FWBSettings.h"

#include "fwbuilder/FWException.h"
#include "fwbuilder/XMLTools.h"

#include <QTreeWidget>
#include <qregexp.h>
#include <qpushbutton.h>
// #include <qtextbrowser.h>

#include <string>
#include <iostream>

using namespace std;
using namespace libfwbuilder;

bool RCSViewItem::operator<(const QTreeWidgetItem &other) const
{
    int col = treeWidget()->sortColumn();

    QString col_txt_1 = text(col);
    QString col_txt_2 = other.text(col);

    if (col==0)  // column 0 is revision number
        return (XMLTools::version_compare(
                    col_txt_1.toStdString(), col_txt_2.toStdString()) < 0);

    return (col_txt_1 < col_txt_2);
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

    if (st->getRCSFilePreviewStyle()==1)
        m_widget->list_view->setChecked(true);
    else
        m_widget->tree_view->setChecked(true);

    m_widget->RCSTreeView->setAllColumnsShowFocus( true );
    m_widget->RCSTreeView->setSelectionMode(QAbstractItemView::SingleSelection );
    m_widget->RCSTreeView->setRootIsDecorated( false );

//    m_widget->RCSTreeView->sortByColumn( 0, Qt::AscendingOrder );

    if (fwbdebug) qDebug("RCSFilePreview: constructor done");

    rcs = nullptr;
    RO = false;
}

RCSFilePreview::~RCSFilePreview()
{
    if (fwbdebug) qDebug("~RCSFilePreview()  rcs=%p", rcs);
//    if (rcs!=nullptr) delete rcs;
    st->setRCSFilePreviewSortColumn(m_widget->RCSTreeView->sortColumn());
}

void RCSFilePreview::openReadOnly()
{
    if (rcs!=nullptr) rcs->setRO(true);
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
    assert(rcs!=nullptr);
    rcs->setSelectedRev(rev);
    m_widget->comment->setText( rcsComments[rev] );
    if (fwbdebug)
        qDebug("RCSFilePreview::selectedRevision : %s",
               rev.toLatin1().constData());
}

bool RCSFilePreview::showFileRLog( const QString &filename )
{
    if (fwbdebug) qDebug("RCSFilePreview::showFileRLog filename=%s rcs=%p",
                         filename.toLocal8Bit().constData(),rcs);

    current_file = filename;

    m_widget->RCSTreeView->disconnect(
        SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    m_widget->RCSTreeView->clear();

    if (rcs!=nullptr) delete rcs;
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
    RCSViewItem* latest_revision_item = nullptr;
    RCSViewItem* latest_date_item = nullptr;
    string latest_revision = "1.0";
    QString latest_date = "";

    for (i=rcs->revisions.begin(); i!=rcs->revisions.end(); ++i)
    {
        rcsComments[(*i).rev] = (*i).log;

        RCSViewItem *itm = nullptr;
        if (st->getRCSFilePreviewStyle()==1)
        {
            // List style
            itm = addRevision(*i, rootItm);
            itemList.push_back(itm);
        } else
        {
            // tree style
            if ((*i).rev.indexOf(QRegExp("^[0-9]+\\.[0-9]+$"))!=-1)
            {
                itm = addRevision(*i, rootItm);
                itemList.push_back(itm);
            }

            if ((*i).rev.indexOf(QRegExp("^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+"))!=-1)
            {
                QString branch_root = (*i).rev.section(".",0,1);
                for (ili=itemList.begin(); ili!=itemList.end(); ++ili)
                {
                    if ((*ili)->text(0) == branch_root)
                    {
                        QTreeWidgetItem *br = *ili;
                        if (br!=nullptr)  itm = addRevision((*i), br);
                    }
                }
            }
        }

        string itm_revision = (*i).rev.toStdString();

        if (XMLTools::version_compare(itm_revision, latest_revision) > 0)
        {
            latest_revision = itm_revision;
            latest_revision_item = itm;
        }

        // This relies on the date string in the rcslog output
        // being in sortable format. This is so for the "C" or "en_US"
        // locale, but I am not sure about other locales.
        if ((*i).date > latest_date)
        {
            latest_date = (*i).date;
            latest_date_item = itm;
        }
    }

    m_widget->RCSTreeView->expandAll();

    m_widget->RCSTreeView->sortByColumn(st->getRCSFilePreviewSortColumn(),
                                        Qt::AscendingOrder);

    // connect signal before setting current item so that
    // selectedRevision gets control and updates rcs log panel
    connect( m_widget->RCSTreeView,
             SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
             this, SLOT(selectedRevision(QTreeWidgetItem*)));

    RCSViewItem* show_item = nullptr;

    if (m_widget->RCSTreeView->sortColumn()==0 && latest_revision_item)
        show_item = latest_revision_item;

    if (m_widget->RCSTreeView->sortColumn()==1 && latest_date_item)
        show_item = latest_date_item;

    if (show_item)
    {
        show_item->setSelected(true);
        m_widget->RCSTreeView->expandItem(show_item->parent());
        m_widget->RCSTreeView->setCurrentItem(show_item);
        m_widget->RCSTreeView->scrollToItem(show_item);
    }

    // resize after parent of the current item was expanded
    m_widget->RCSTreeView->resizeColumnToContents ( 0 );
    m_widget->RCSTreeView->resizeColumnToContents ( 1 );


    return true;
}

RCSViewItem* RCSFilePreview::addRevision(Revision &rev,
                                         QTreeWidgetItem *parent_item)
{
    RCSViewItem *itm = new RCSViewItem(parent_item);
    itm->setText(0, rev.rev);
    itm->setText(1, rev.date);
    itm->setText(2, rev.author);
    itm->setText(3, QString("    ") + rev.locked_by);
    return itm;
}

void RCSFilePreview::switchToTreeView()
{
    st->setRCSFilePreviewStyle(0);  // 0 for backward compatibility
    if (!current_file.isEmpty()) showFileRLog(current_file);
}

void RCSFilePreview::switchToListView()
{
    st->setRCSFilePreviewStyle(1);
    if (!current_file.isEmpty()) showFileRLog(current_file);
}

RCS* RCSFilePreview::getSelectedRev()
{

    if (fwbdebug) qDebug("RCSFilePreview::getSelectedRev rcs=%p",rcs);

    return rcs;

}

void RCSFilePreview::closeEvent(QCloseEvent *event)
{
    if (fwbdebug)
        qDebug("RCSFilePreview::closeEvent");
    st->setRCSFilePreviewSortColumn(m_widget->RCSTreeView->sortColumn());
    QDialog::closeEvent(event);
}

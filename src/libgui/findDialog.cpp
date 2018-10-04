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

#include "findDialog.h"
#include "ProjectPanel.h"
#include "FWBTree.h"
#include "FWBSettings.h"
#include "FWWindow.h"
#include "events.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

#include <qcombobox.h>
#include <qcheckbox.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qregexp.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qtimer.h>

#include <iostream>

using namespace libfwbuilder;
using namespace std;

#define MAX_SEARCH_ITEMS_COUNT 10

findDialog::findDialog(QWidget *p, ProjectPanel *project) : QDialog(p), treeSeeker(), m_project(project)
{
    m_dialog = new Ui::findDialog_q;
    m_dialog->setupUi(this);

    lastFound=nullptr;
    lastTextSearch="";
    lastAttrSearch="";

    m_dialog->findText->setFocus();
}

void findDialog::setObject(FWObject *o)
{
    reset();
    m_dialog->findText->lineEdit()->setText( QString::fromUtf8(o->getName().c_str()) );
}

void findDialog::reset()
{
    lastFound=nullptr;
    lastTextSearch="";
    treeSeeker=m_project->db()->tree_begin();
}

void findDialog::findTextChanged(const QString &ns)
{
    if (ns!=lastTextSearch)  reset();
    lastTextSearch=ns;
}

void findDialog::findAttrChanged(const QString &ns)
{
    if (ns!=lastAttrSearch)  reset();
    lastAttrSearch=ns;
}

void findDialog::find()
{
    if (m_dialog->findText->currentText().isEmpty() && m_dialog->findAttr->currentText().isEmpty()) return;

    if (m_dialog->findText->currentText() != m_dialog->findText->itemText(0))
    {
        if (m_dialog->findText->count()>=MAX_SEARCH_ITEMS_COUNT)
            m_dialog->findText->removeItem(MAX_SEARCH_ITEMS_COUNT-1);

        m_dialog->findText->insertItem( 0, m_dialog->findText->currentText() );

        if (fwbdebug)
        {
            qDebug("findDialog::find() : findText->text(0)=%s",
                   m_dialog->findText->itemText(0).toLatin1().constData());
        }
    }

    if (m_dialog->findAttr->currentText() != m_dialog->findAttr->itemText(0))
    {
        if (m_dialog->findAttr->count()>=MAX_SEARCH_ITEMS_COUNT)
            m_dialog->findAttr->removeItem(MAX_SEARCH_ITEMS_COUNT-1);

        m_dialog->findAttr->insertItem( 0, m_dialog->findAttr->currentText() );

        if (fwbdebug)
            qDebug("findDialog::find() : findAttr->text(0)=%s",
                   m_dialog->findAttr->itemText(0).toLatin1().constData());
    }

    findNext();
}

bool findDialog::matchName(const QString &name)
{
    QString s=m_dialog->findText->currentText();
    if (s.isEmpty()) return true;

    bool res=false;

    if (m_dialog->useRegexp->isChecked()) res= ( name.indexOf( QRegExp(s) )!=-1 );
    else                        res= ( name == s );

    return res;
}

bool findDialog::matchAttr(libfwbuilder::FWObject *obj)
{
    QString s=m_dialog->findAttr->currentText();
    if (s.isEmpty()) return true;

    bool res=false;
    int  attrN = m_dialog->attribute->currentIndex();

    switch (attrN) {
    case 0:   // Address
    {
        Address *a = Address::cast(obj);
        if (a!=nullptr)
        {
            QString addr = a->getAddressPtr()->toString().c_str();
            if (m_dialog->useRegexp->isChecked()) res= ( addr.indexOf( QRegExp(s) )!=-1 );
            else                        res= ( addr == s );
        }
        break;
    }
    case 1:   // port
        if (TCPService::cast(obj)!=nullptr || UDPService::cast(obj)!=nullptr)
        {
            if (m_dialog->useRegexp->isChecked())
            {
                QString port;
                port.setNum(TCPUDPService::cast(obj)->getSrcRangeStart());
                res |= ( port.indexOf( QRegExp(s) )!=-1 );
                port.setNum(TCPUDPService::cast(obj)->getSrcRangeEnd());
                res |= ( port.indexOf( QRegExp(s) )!=-1 );
                port.setNum(TCPUDPService::cast(obj)->getDstRangeStart());
                res |= ( port.indexOf( QRegExp(s) )!=-1 );
                port.setNum(TCPUDPService::cast(obj)->getDstRangeEnd());
                res |= ( port.indexOf( QRegExp(s) )!=-1 );
            } else
            {
                int port = s.toInt();
                res |= (port == TCPUDPService::cast(obj)->getSrcRangeStart());
                res |= (port == TCPUDPService::cast(obj)->getSrcRangeEnd());
                res |= (port == TCPUDPService::cast(obj)->getDstRangeStart());
                res |= (port == TCPUDPService::cast(obj)->getDstRangeEnd());
            }
            break;
        }
    /* FALLTHRU */
    case 2:   // protocol num.
        if (IPService::cast(obj)!=nullptr)
        {
            if (m_dialog->useRegexp->isChecked())
            {
                QString proto;
                proto.setNum(obj->getInt("protocol_num"));
                res |= ( proto.indexOf( QRegExp(s) )!=-1 );
            } else
            {
                int proto = s.toInt();
                res |= (proto == obj->getInt("protocol_num"));
            }
            break;
        }
    /* FALLTHRU */
    case 3:   // icmp type
        if (ICMPService::cast(obj)!=nullptr)
        {
            if (m_dialog->useRegexp->isChecked())
            {
                QString icmptype;
                icmptype.setNum(obj->getInt("type"));
                res |= ( icmptype.indexOf( QRegExp(s) )!=-1 );
            } else
            {
                int icmptype = s.toInt();
                res |= (icmptype == obj->getInt("type"));
            }
            break;
        }
    }

    return res;
}

void findDialog::findNext()
{
    if (m_dialog->findText->currentText().isEmpty() &&
        m_dialog->findAttr->currentText().isEmpty()) return;

    if (m_dialog->findText->count()>10)  m_dialog->findText->removeItem(0);
    if (m_dialog->findAttr->count()>10)  m_dialog->findAttr->removeItem(0);

    FWObject *o=nullptr;

loop:
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

    for (; treeSeeker!=m_project->db()->tree_end(); ++treeSeeker)
    {
        o = *treeSeeker;

        if( RuleElement::cast(o->getParent())!=nullptr)
        {
            if (! m_dialog->searchInRules->isChecked()) continue;
        } else
        {
/* if not in rules, then in the tree. */
            if (! m_dialog->searchInTree->isChecked()) continue;
        }

        if (FWReference::cast(o)!=nullptr)
        {
            FWReference *r=FWReference::cast(o);
            if ( matchName( QString::fromUtf8(r->getPointer()->getName().c_str()) ) &&
                 matchAttr( r->getPointer() )) break;
        } else
        {
            if (matchName( QString::fromUtf8(o->getName().c_str())) &&
                matchAttr( o )) break;
        }
    }

    QApplication::restoreOverrideCursor();

    if (treeSeeker==m_project->db()->tree_end())
    {
        reset();

        if ( QMessageBox::warning(
              this,"Firewall Builder",
              tr("Search hit the end of the object tree."),
              tr("&Continue at top"), tr("&Stop"), QString::null, 0, 1 )==0 ) goto loop;

        return;
    }
    assert(o!=nullptr);

/* found object. Shift iterator so it does not return the same object
 * when user hits 'find next'
 */

    ++treeSeeker;

    if (FWReference::cast(o)!=nullptr && RuleElement::cast(o->getParent())!=nullptr)
    {
        m_project->ensureObjectVisibleInRules( FWReference::cast(o) );
        QTimer::singleShot(200, this, SLOT(makeActive()) );
        return;
    }

    if (Group::cast(o->getParent())!=nullptr &&
        !FWBTree().isSystem(o->getParent()))
    {
        QCoreApplication::postEvent(
            mw, new showObjectInTreeEvent(m_project->getFileName(),
                                          o->getParent()->getId()));
        QCoreApplication::postEvent(
            mw, new openObjectInEditorEvent(m_project->getFileName(),
                                            o->getParent()->getId()));
        QTimer::singleShot(200, this, SLOT(makeActive()) );
        return;
    }

    if (fwbdebug)
    {
        qDebug("Found object: o=%p  id=%s  name=%s  type=%s",
               o,
               FWObjectDatabase::getStringId(o->getId()).c_str(),
               o->getName().c_str(),
               o->getTypeName().c_str());
    }

    QCoreApplication::postEvent(
        mw, new showObjectInTreeEvent(m_project->getFileName(),
                                      o->getId()));
    QCoreApplication::postEvent(
        mw, new openObjectInEditorEvent(m_project->getFileName(),
                                        o->getId()));
    QTimer::singleShot(200, this, SLOT(makeActive()) );
}

void findDialog::makeActive()
{
    activateWindow();
}

void findDialog::showEvent( QShowEvent *ev)
{
    st->restoreGeometry(this, QRect(200,100,330,140) );
    QDialog::showEvent(ev);

    m_dialog->useRegexp->setChecked( st->getBool("Search/useRegexp") );
    m_dialog->searchInTree->setChecked(  st->getBool("Search/findInTree" ) );
    m_dialog->searchInRules->setChecked( st->getBool("Search/findInRules") );

    m_dialog->findText->setFocus();
}

void findDialog::hideEvent( QHideEvent *ev)
{
    st->saveGeometry(this);
    QDialog::hideEvent(ev);

    st->setBool("Search/useRegexp",    m_dialog->useRegexp->isChecked() );
    st->setBool("Search/findInTree",  m_dialog->searchInTree->isChecked() );
    st->setBool("Search/findInRules", m_dialog->searchInRules->isChecked() );
}


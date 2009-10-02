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
#include "../../definitions.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"

#include <QLineEdit>
#include "FindObjectWidget.h"
#include "FWWindow.h"
#include "FWObjectDropArea.h"
#include "FWBTree.h"
#include "FWBSettings.h"
#include "ObjectTreeView.h"
#include "RuleSetView.h"
#include "ObjectEditor.h"
#include "ProjectPanel.h"
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

#include <QStackedWidget>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qregexp.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace libfwbuilder;

#define MAX_SEARCH_ITEMS_COUNT 10

FindObjectWidget::FindObjectWidget(QWidget*p, ProjectPanel *pp,
                                   const char * n, Qt::WindowFlags f) : QWidget(p,f)
{
    project_panel = pp;
    m_widget = new Ui::findObjectWidget_q;
    m_widget->setupUi(this);

    setObjectName(n);

    replaceDisable();
    m_widget->srScope->setCurrentIndex(
        st->getInt(SETTINGS_PATH_PREFIX"/Search/Scope"));
}

void FindObjectWidget::findObject(FWObject *o)
{
    if (fwbdebug) qDebug("FindObjectWidget::findObject");
    show();
    m_widget->findDropArea->insertObject(o);

}


void FindObjectWidget::enableAll()
{
    m_widget->useRegexp->setEnabled (true);
    m_widget->findAttr->setEnabled  (true);
    m_widget->attribute->setEnabled (true);
}

void FindObjectWidget::disableAll()
{
    m_widget->useRegexp->setEnabled (false);
    m_widget->findAttr->setEnabled  (false);
    m_widget->attribute->setEnabled (false);
}

void FindObjectWidget::objectInserted()
{
    FWObject *o=m_widget->findDropArea->getObject();
    if (o==NULL) return;
    disableAll();

    QString n=QString::fromUtf8(o->getName().c_str());

    if (m_widget->findAttr->count()>=MAX_SEARCH_ITEMS_COUNT)
        m_widget->findAttr->removeItem(MAX_SEARCH_ITEMS_COUNT-1);

    m_widget->findAttr->lineEdit()->setText (n);

     reset();
}

void FindObjectWidget::reset()
{
    lastFound=NULL;
    lastAttrSearch="";
    treeSeeker = project_panel->db()->tree_begin();
}


void FindObjectWidget::findAttrChanged(const QString &ns)
{
    if (ns!=lastAttrSearch)  reset();
    lastAttrSearch=ns;
}

void FindObjectWidget::find()
{
    if (m_widget->findAttr->currentText().isEmpty() &&
        m_widget->findDropArea->isEmpty()) return;

    if (m_widget->findAttr->currentText() != m_widget->findAttr->itemText(0))
    {
        if (m_widget->findAttr->count()>=MAX_SEARCH_ITEMS_COUNT)
            m_widget->findAttr->removeItem(MAX_SEARCH_ITEMS_COUNT-1);

        m_widget->findAttr->insertItem(0, m_widget->findAttr->lineEdit()->text());

        if (fwbdebug)
            qDebug("FindObjectWidget::find() : m_widget->findAttr->text(0)=%s",
                   m_widget->findAttr->itemText(0).toLatin1().constData());
    }
    findNext();
}

bool FindObjectWidget::matchID(int id)
{
    if (m_widget->findDropArea->isEmpty()) return true;
    int s_id = m_widget->findDropArea->getObject()->getId();
    return s_id==id;
}

bool FindObjectWidget::matchAttr(libfwbuilder::FWObject *obj)
{
    if (!m_widget->findDropArea->isEmpty()) return true;
    QString s=m_widget->findAttr->currentText();
    if (s.isEmpty()) return true;

    bool res=false;
    int  attrN = m_widget->attribute->currentIndex();

    switch (attrN) {
    case 0:   // Name
    {
        QString name=QString::fromUtf8( obj->getName().c_str() );

        if (m_widget->useRegexp->isChecked()) res= ( name.indexOf( QRegExp(s) )!=-1 );
        else                        res= ( name == s );

        //res= ( name == s );
        break;

    }

    case 1:   // Address
    {
        Address *a = Address::cast(obj);
        if (a!=NULL)
        {
            const InetAddr *inet_addr = a->getAddressPtr();
            if (inet_addr)
            {
                QString addr = inet_addr->toString().c_str();

                if (m_widget->useRegexp->isChecked())
                    res = ( addr.indexOf( QRegExp(s) )!=-1 );
                else
                    res= ( addr == s );
            }
            //res= ( addr == s );
        }
        break;
    }

    case 2:   // port
        if (TCPService::cast(obj)!=NULL || UDPService::cast(obj)!=NULL)
        {
            if (m_widget->useRegexp->isChecked())
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
                bool conversion_status = false;
                int port = s.toInt(&conversion_status);
                res |= (conversion_status &&
                        (port == TCPUDPService::cast(obj)->getSrcRangeStart()));
                res |= (conversion_status &&
                        (port == TCPUDPService::cast(obj)->getSrcRangeEnd()));
                res |= (conversion_status &&
                        (port == TCPUDPService::cast(obj)->getDstRangeStart()));
                res |= (conversion_status &&
                        (port == TCPUDPService::cast(obj)->getDstRangeEnd()));
            }
            break;
        }
        break;

    case 3:   // protocol num.
        if (IPService::cast(obj)!=NULL)
        {
            if (m_widget->useRegexp->isChecked())
            {
                QString proto;
                proto.setNum(obj->getInt("protocol_num"));
                res |= ( proto.indexOf( QRegExp(s) )!=-1 );
            } else
            {
                bool conversion_status = false;
                int proto = s.toInt(&conversion_status);
                res |= (conversion_status && (proto == obj->getInt("protocol_num")));
            }
            break;
        }
        break;

    case 4:   // icmp type
        if (ICMPService::cast(obj)!=NULL)
        {
            if (m_widget->useRegexp->isChecked())
            {
                QString icmptype;
                icmptype.setNum(obj->getInt("type"));
                res |= ( icmptype.indexOf( QRegExp(s) )!=-1 );
            } else
            {
                bool conversion_status = false;
                int icmptype = s.toInt(&conversion_status);
                res |= (conversion_status && (icmptype == obj->getInt("type")));
            }
            break;
        }
        break;
    }

    return res;
}

void FindObjectWidget::findNext()
{
    if (fwbdebug) qDebug("FindObjectWidget::findNext");
    if (
        m_widget->findAttr->currentText().isEmpty() &&
        m_widget->findDropArea->isEmpty()) return;

    if (m_widget->findAttr->count()>MAX_SEARCH_ITEMS_COUNT)
        m_widget->findAttr->removeItem(0);

    FWObject *o = NULL;

    // if scope is "policies of opened firewall" then we need to get 
    // pointer to the currently opened firewall object
    RuleSet* current_rule_set = project_panel->getCurrentRuleSet();
    if (current_rule_set)
        selectedFirewall = Firewall::cast(current_rule_set->getParent());
    else
        selectedFirewall = NULL;

    if (fwbdebug)
        qDebug("selectedFirewall: %p", selectedFirewall);

loop:

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

    for (; treeSeeker != project_panel->db()->tree_end(); ++treeSeeker)
    {
        o = *treeSeeker;

//        if (fwbdebug)
//            qDebug("Found object %s (%s)", o->getName().c_str(), o->getTypeName().c_str());

        if( RuleElement::cast(o->getParent())!=NULL)
        {
            if (m_widget->srScope->currentIndex()==3) // scope == selected firewalls
            {
                if ( !inSelectedFirewall(RuleElement::cast(o->getParent())) )
                {
                    continue;

                }
            } else
                if (m_widget->srScope->currentIndex()==0) continue ; // scope == tree only
        } else
        {
/* if not in rules, then in the tree. */
            if (m_widget->srScope->currentIndex()>1) continue; // scope in (firewalls only , selected firewalls)
        }

        if (FWReference::cast(o)!=NULL)
        {
            FWReference *r=FWReference::cast(o);
            if (matchAttr( r->getPointer() ) &&
                matchID( r->getPointer()->getId() )) break;
        } else
        {
            if (matchAttr( o ) &&
                matchID( o->getId() )) break;
        }
    }

    QApplication::restoreOverrideCursor();

    if (treeSeeker == project_panel->db()->tree_end())
    {
        reset();
        if (m_widget->srScope->currentIndex()==3) // scope ==selected firewalls
        {
            if ( QMessageBox::warning(
                     this,"Firewall Builder",
                     tr("Search hit the end of the policy rules."),
                     tr("&Continue at top"), tr("&Stop"), QString::null, 0, 1 )==0 ) goto loop;
        }
        else
        {
            if (fwbdebug) qDebug("widget that has focus: %p",mw->focusWidget());
            bool r= ( QMessageBox::warning(
                     this,"Firewall Builder",
                     tr("Search hit the end of the object tree."),
                     tr("&Continue at top"), tr("&Stop"), QString::null, 0, 1 )==0 );
            if (fwbdebug) qDebug("widget that has focus: %p",mw->focusWidget());
            if (r)  goto loop;
        }
        return;
    }
    assert(o!=NULL);
    lastFound=o;
/* found object. Shift iterator so it does not return the same object
 * when user hits 'find next'
 */

    ++treeSeeker;

    if (fwbdebug)
    {
        qDebug("Found object: o=%p  id=%s  name=%s  type=%s",
               o,
               FWObjectDatabase::getStringId(o->getId()).c_str(),
               o->getName().c_str(),o->getTypeName().c_str());
    }
    showObject(o);
}

bool FindObjectWidget::validateReplaceObject()
{
    if (m_widget->findDropArea->isEmpty() || m_widget->replaceDropArea->isEmpty())
    {
        QMessageBox::warning(
              this,"Firewall Builder",
              tr("Search or Replace object ind't specified."));
        return false;
    }
    FWObject *findObj, *replObj;
    findObj=m_widget->findDropArea->getObject();
    replObj=m_widget->replaceDropArea->getObject();
    if (findObj==replObj || findObj->getId() == replObj->getId())
    {
        QMessageBox::warning(
              this,"Firewall Builder",
              tr("Cannot replace object by itself."));
        return false;
    }
    if (!((Address::cast(findObj)!=NULL && Address::cast(replObj)) ||
            (Service::cast(findObj)!=NULL && Service::cast(replObj))))
    {
        QMessageBox::warning(
              this,"Firewall Builder",
              tr("Search and Replace objects are incompatible."));

        return false;
    }
    return true;
}

void FindObjectWidget::replace()
{
    if(!validateReplaceObject())  return;


    if (lastFound==NULL)
    {
        find();
        return;
    }

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
    FWObject *res = _replaceCurrent();

    if (res)
    {
        showObject(res);
    }
    else
    {
        // object isn't inserted
        qDebug("object isn't inserted");
    }

    QApplication::restoreOverrideCursor();
}

void FindObjectWidget::replaceAll()
{
    if(!validateReplaceObject()) return;
    reset();
    FWObject *o=NULL;
    int count=0;
    bool f=true;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
    while (f)
    {
        for (; treeSeeker != project_panel->db()->tree_end(); ++treeSeeker)
        {
            o = *treeSeeker;
            if( RuleElement::cast(o->getParent())!=NULL)
            {
                if (m_widget->srScope->currentIndex()==3) // scope == selected firewalls
                {
                    if ( !inSelectedFirewall(RuleElement::cast(o->getParent())) )
                    {
                        continue;

                    }
                } else if (m_widget->srScope->currentIndex()==0) continue ; // scope == tree only
            } else
            {
                /* if not in rules, then in the tree. */
                if (m_widget->srScope->currentIndex()>1) continue; // scope in (firewalls only , selected firewalls)
            }

            if (FWReference::cast(o)!=NULL)
            {
                FWReference *r=FWReference::cast(o);
                if (matchAttr( r->getPointer() ) &&
                    matchID( r->getPointer()->getId() )) break;
            } else
            {
                if (matchAttr( o ) &&
                    matchID( o->getId() )) break;
            }
        }
        if (treeSeeker == project_panel->db()->tree_end())
        {
            f=false;

        } else
        {
            lastFound=o;
            ++treeSeeker;
            count++;
            _replaceCurrent();
        }
    }

    QApplication::restoreOverrideCursor();
    QMessageBox::information(
              this, "Firewall Builder",
              tr("Replaced %1 objects.").arg(count));

}

FWObject* FindObjectWidget::_replaceCurrent()
{
    FWObject *o=lastFound;
    FWObject *p=lastFound->getParent();

    if (p==NULL || o==NULL) return NULL;
    if (FWReference::cast(o)==NULL) return NULL;

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(project_panel->getFileName(), p->getId()));

    p->removeRef(FWReference::cast(o)->getPointer());

    // check for duplicates --------

    FWObject *ro=m_widget->replaceDropArea->getObject();
    if (RuleElement::cast(p)==NULL || !RuleElement::cast(p)->isAny())
    {
        // avoid duplicates
        int cp_id = ro->getId();
        FWObject *oo;
        FWReference *ref;

        list<FWObject*>::iterator j;
        for(j=p->begin(); j!=p->end(); ++j)
        {
            oo=*j;
            if(cp_id==oo->getId()) return NULL;

            if( (ref=FWReference::cast(oo))!=NULL &&
                cp_id==ref->getPointerId()) return NULL;
        }
    }

    p->addRef(ro);
    FWObject *to;
    FWReference *ref;
    list<FWObject*>::iterator i;
    int id = m_widget->replaceDropArea->getObject()->getId();
    for (i=p->begin();i!=p->end();++i)
    {
        to=*i;
        ref=FWReference::cast(to);
        if(ref && ref->getPointerId()==id)
        {
            return to;
        }

    }
    return NULL;

}
bool FindObjectWidget::inSelectedFirewall( RuleElement* r)
{
    FWObject *f=r;
    while (f!=NULL && Firewall::cast(f)==NULL) f=f->getParent();
    if (f==NULL) return false;

    return selectedFirewall==(Firewall::cast(f));
}

void FindObjectWidget::replaceEnable()
{
    m_widget->replaceButton->setEnabled   (true);
    m_widget->repNextButton->setEnabled   (true);
    m_widget->replaceAllButton->setEnabled(true);

}

void FindObjectWidget::replaceDisable()
{
    m_widget->replaceButton->setEnabled   (false);
    m_widget->repNextButton->setEnabled   (false);
    m_widget->replaceAllButton->setEnabled(false);

}

void FindObjectWidget::showObject(FWObject* o)
{
    if (fwbdebug)
        qDebug("FindObjectWidget::showObject  o: %s parent: %s",
               o->getName().c_str(), o->getParent()->getName().c_str());

    FWReference* ref=FWReference::cast(o);
    if (ref!=NULL && RuleElement::cast(o->getParent())!=NULL)
    {

        // found object in rules
        project_panel->closeEditor();
        project_panel->clearManipulatorFocus();
        project_panel->ensureObjectVisibleInRules( ref );
        return;
    }

//    project_panel->unselectRules();

    if (fwbdebug)
        qDebug("FindObjectWidget::showObject  checkpoint #1");

    if (!FWBTree().isStandardFolder(o) &&
        Group::cast(o->getParent())!=NULL &&
        !FWBTree().isStandardFolder(o->getParent()))
    {
        project_panel->openObject( o->getParent() );
        project_panel->editObject( o->getParent() );
        project_panel->selectObjectInEditor( (ref) ? ref->getPointer() : o );
        return;
    }

    if (fwbdebug)
        qDebug("FindObjectWidget::showObject  checkpoint #2");

    project_panel->closeEditor();
    project_panel->openObject( o );
    project_panel->select();  // selects an item in the tree and assigns kbd focus to it
}

void FindObjectWidget::init()
{
    m_widget->findDropArea->deleteObject();
    m_widget->replaceDropArea->deleteObject();
}

void FindObjectWidget::firewallOpened(Firewall *f)
{
    if (f==NULL) return;
    selectedFirewall = f;
    m_widget->srScope->setItemText(
        3, tr("Policy of firewall '")+f->getName().c_str()+"'" );
}

void FindObjectWidget::findPrev()
{

}

void FindObjectWidget::replaceNext()
{
    replace();
    findNext();
}
void FindObjectWidget::scopeChanged()
{
    st->setValue(SETTINGS_PATH_PREFIX"/Search/Scope",
                 m_widget->srScope->currentIndex ());
}

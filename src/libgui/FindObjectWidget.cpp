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

#include "definitions.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"

#include "FindObjectWidget.h"
#include "FWWindow.h"
#include "FWObjectDropArea.h"
#include "FWBTree.h"
#include "FWBSettings.h"
#include "ObjectTreeView.h"
#include "RuleSetView.h"
#include "ObjectEditor.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"
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
#include "fwbuilder/MultiAddress.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/AddressRange.h"

#include <QLineEdit>
#include <QStackedWidget>
#include <QtDebug>
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
#include <memory>


using namespace std;
using namespace libfwbuilder;

#define MAX_SEARCH_ITEMS_COUNT 10

class FindAndReplaceError : public FWException
{
    public:
    FindAndReplaceError(const string &err) : FWException(err) {}
};


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

    m_widget->findDropArea->setHelperText(
        tr("Drop object you want to find here"));
    m_widget->replaceDropArea->setHelperText(
        tr("Drop object to replace with here"));

    connect(m_widget->findDropArea, SIGNAL(objectDeleted()),
            this, SLOT(objectDeleted()));
}

void FindObjectWidget::keyPressEvent( QKeyEvent* ev )
{
    if (ev->key()==Qt::Key_Enter || ev->key()==Qt::Key_Return)
    {
        if (fwbdebug)
            qDebug() << "FindObjectWidget::keyPressEvent Qt::Key_Enter";
        ev->accept();
        find();
        return;
    }
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
    FWObject *o = m_widget->findDropArea->getObject();
    if (o == nullptr) return;

//    disableAll();  // see #1757

    QString n = QString::fromUtf8(o->getName().c_str());

    m_widget->findAttr->blockSignals(true);

    if (m_widget->findAttr->count()>=MAX_SEARCH_ITEMS_COUNT)
        m_widget->findAttr->removeItem(MAX_SEARCH_ITEMS_COUNT-1);

    m_widget->findAttr->lineEdit()->setText(n);

    m_widget->findAttr->blockSignals(false);

    reset();
}

void FindObjectWidget::reset()
{
    lastFound = nullptr;
    lastAttrSearch = "";
    found_objects.clear();
}

void FindObjectWidget::clear()
{
    m_widget->findDropArea->deleteObject();
    m_widget->replaceDropArea->deleteObject();
    lastFound = nullptr;
    lastAttrSearch = "";
    found_objects.clear();
}

/**
 * This slot is called when user deletes an object from the "find"
 * drop area. See #1785
 */
void FindObjectWidget::objectDeleted()
{
    m_widget->findAttr->blockSignals(true);
    m_widget->findAttr->lineEdit()->setText("");
    m_widget->findAttr->blockSignals(false);
}

/**
 * This slot is called when @attribute or @findAttr fields change
 * (that is, whenever user changes attribute they want to match or type
 * in the input field to change the value)
 */
void FindObjectWidget::findAttrChanged(const QString &ns)
{
    if (ns!=lastAttrSearch)  reset();
    lastAttrSearch = ns;
    m_widget->findDropArea->blockSignals(true);
    m_widget->findDropArea->deleteObject(); //  for #1757
    m_widget->findDropArea->blockSignals(false);
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

bool FindObjectWidget::matchAttr(FWObject *obj)
{
    if (!m_widget->findDropArea->isEmpty()) return true;
    QString s = m_widget->findAttr->currentText();
    if (s.isEmpty()) return true;

    bool res = false;
    int  attrN = m_widget->attribute->currentIndex();

    switch (attrN) {
    case 0:   // Name or label
    {
        QString name = QString::fromUtf8( obj->getName().c_str() );
        QString label;
        if (Interface::isA(obj))
            label = QString::fromUtf8( Interface::cast(obj)->getLabel().c_str() );

        if (m_widget->useRegexp->isChecked())
        {
            res = (name.indexOf( QRegExp(s) )!=-1 ||
                   (!label.isEmpty() && label.indexOf( QRegExp(s) )!=-1));
        } else
        {
            res = (name == s || (!label.isEmpty() && label == s));
        }
        //res= ( name == s );
        break;

    }

    case 1:   // Address
    {
        if ( ! m_widget->useRegexp->isChecked())
        {
            AddressRange *ar = AddressRange::cast(obj);
            if (ar)
            {
                const InetAddr &inet_addr_start = ar->getRangeStart();
                const InetAddr &inet_addr_end = ar->getRangeEnd();

                // if address entered by the user has /NN perfix length or
                // /255.255.255.0 netmask, do not match it to address ranges
                if ( ! s.contains("/"))
                {
                    int af = AF_INET;
                    if (s.contains(':')) af = AF_INET6;

                    InetAddr addr = InetAddr(af, s.toStdString());

                    res = (inet_addr_start == addr || inet_addr_end == addr ||
                           (inet_addr_start < addr && addr < inet_addr_end));
                    break;
                }

            }
        }

        Address *a = Address::cast(obj);
        if (a!=nullptr)
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
        if (TCPService::cast(obj)!=nullptr || UDPService::cast(obj)!=nullptr)
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
        if (IPService::cast(obj)!=nullptr)
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
        if (ICMPService::cast(obj)!=nullptr)
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

/*
 * Walks the whole tree and fills found_objects list
 */
void FindObjectWidget::_findAll()
{
    treeSeeker = project_panel->db()->tree_begin();
    for (; treeSeeker != project_panel->db()->tree_end(); ++treeSeeker)
    {
        FWObject *o = *treeSeeker;

        if( RuleElement::cast(o->getParent())!=nullptr)
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

        FWObject *obj = o;
        if (FWReference::cast(o)!=nullptr)
        {
            FWReference *r = FWReference::cast(o);
            obj = r->getPointer();
        }

        if (matchAttr(obj) && matchID(obj->getId()))
        {
            // If object we have found belongs to the deleted objects lib
            // but this library is hidden, don't show it.
            if (obj->getLibrary()->getId() == FWObjectDatabase::DELETED_OBJECTS_ID &&
                !st->getBool("UI/ShowDeletedObjects")) continue;
            // Note that if o is a reference, we should add reference
            // to found_objects
            found_objects.push_back(o);
        }
    }
}

/*
 * Group::duplicateForUndo(), which is used in FWCmdChange::redo() and
 * undo() breaks FWObject::tree_iterator because it adds or removes
 * objects to groups. To work around this problem, we fill list
 * found_objects on the first call to findNext() and then just iterate
 * over items in this list later.
 */
void FindObjectWidget::findNext()
{
    if (fwbdebug) qDebug("FindObjectWidget::findNext");

    if (
        m_widget->findAttr->currentText().isEmpty() &&
        m_widget->findDropArea->isEmpty()) return;

    if (m_widget->findAttr->count()>MAX_SEARCH_ITEMS_COUNT)
        m_widget->findAttr->removeItem(0);

    FWObject *o = nullptr;

    // if scope is "policies of opened firewall" then we need to get 
    // pointer to the currently opened firewall object
    RuleSet* current_rule_set = project_panel->getCurrentRuleSet();
    if (current_rule_set)
        selectedFirewall = Firewall::cast(current_rule_set->getParent());
    else
        selectedFirewall = nullptr;

    if (fwbdebug) qDebug() << "selectedFirewall: " << selectedFirewall;

loop:

    if (found_objects.size() == 0)
    {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
        _findAll();
        QApplication::restoreOverrideCursor();

        found_objects_iter = found_objects.begin();
    }

    if (found_objects_iter == found_objects.end())
    {
        reset();
        if (m_widget->srScope->currentIndex()==3) // scope ==selected firewalls
        {
            if ( QMessageBox::warning(
                     this,"Firewall Builder",
                     tr("Search hit the end of the policy rules."),
                     tr("&Continue at top"), tr("&Stop"), QString::null, 0, 1 )==0 )
                goto loop;
        }
        else
        {
            if (fwbdebug) qDebug("widget that has focus: %p",mw->focusWidget());
            bool r= ( QMessageBox::warning(
                     this,"Firewall Builder",
                     tr("Search hit the end of the object tree."),
                     tr("&Continue at top"), tr("&Stop"), QString::null, 0, 1 )==0);
            if (fwbdebug) qDebug("widget that has focus: %p",mw->focusWidget());
            if (r)  goto loop;
        }
        return;
    }

    o = *found_objects_iter;
    ++found_objects_iter;

    assert(o!=nullptr);
    lastFound = o;

    if (fwbdebug)
    {
        qDebug() << "Found object: o=" << o
                 << "id=" << FWObjectDatabase::getStringId(o->getId()).c_str()
                 << "name=" << o->getName().c_str()
                 << "type=" << o->getTypeName().c_str();
    }
    showObject(o);
}

bool FindObjectWidget::validateReplaceObject()
{
    if (m_widget->findDropArea->isEmpty())
    {
        QMessageBox::warning(
              this, "Firewall Builder", tr("Search object is missing."));
        return false;
    }

    if (m_widget->replaceDropArea->isEmpty())
    {
        QMessageBox::warning(
              this, "Firewall Builder", tr("Replace object is missing."));
        return false;
    }

    FWObject* findObj = m_widget->findDropArea->getObject();
    FWObject* replObj = m_widget->replaceDropArea->getObject();

    if (findObj==replObj || findObj->getId() == replObj->getId())
    {
        QMessageBox::warning(
              this,"Firewall Builder",
              tr("Cannot replace object with itself."));
        return false;
    }

    bool obj_1_address = 
        Address::cast(findObj)!=nullptr ||
        MultiAddress::cast(findObj)!=nullptr ||
        ObjectGroup::cast(findObj)!=nullptr;

    bool obj_2_address =
        Address::cast(replObj)!=nullptr ||
        MultiAddress::cast(replObj)!=nullptr ||
        ObjectGroup::cast(replObj)!=nullptr;

    bool obj_1_service =
        Service::cast(findObj)!=nullptr || ServiceGroup::cast(findObj);
    bool obj_2_service =
        Service::cast(replObj)!=nullptr || ServiceGroup::cast(replObj);

    if ((obj_1_address && obj_2_address) || (obj_1_service && obj_2_service))
        return true;

    QMessageBox::warning(
        this,"Firewall Builder",
        tr("Search and Replace objects are incompatible."));
    
    return false;
}

void FindObjectWidget::replace()
{
    if (!validateReplaceObject()) return;

    if (lastFound==nullptr)
    {
        find();
        return;
    }

    if (lastFound->isReadOnly())
    {
        QMessageBox::critical(this,
                              "Firewall Builder",
                              tr("Can not modify read-only object %1")
                              .arg(lastFound->getPath().c_str()));
        return;
    }

    _replaceCurrent();
}

void FindObjectWidget::replaceAll()
{
    if (!validateReplaceObject()) return;
    reset();

    /*
     * replaceAll() may potentially make many replacements. Check read-only
     * condition early if possible to avoid popping many error dialogs.
     */
    if (m_widget->srScope->currentIndex()==3) // scope == selected firewalls
    {
        RuleSet* current_rule_set = project_panel->getCurrentRuleSet();
        if (current_rule_set)
            selectedFirewall = Firewall::cast(current_rule_set->getParent());
        else
            selectedFirewall = nullptr;

        if (selectedFirewall == nullptr)
        {
            QMessageBox::critical(this,
                                  "Firewall Builder",
                                  tr("Please select a firewall object"));
            return;
        }

        if (selectedFirewall->isReadOnly())
        {
            QMessageBox::critical(this,
                                  "Firewall Builder",
                                  tr("Can not modify read-only object %1")
                                  .arg(selectedFirewall->getPath().c_str()));
            return;
        }
    }

    findNext();  // fill found_objects and position to the first found one

    int count = 0;
    try
    {
        for (found_objects_iter=found_objects.begin();
             found_objects_iter!=found_objects.end(); ++found_objects_iter)
        {
            lastFound = *found_objects_iter;
            _replaceCurrent();
            count++;
        }
    } catch (FindAndReplaceError &ex)
    {
    }

    QMessageBox::information(
              this, "Firewall Builder",
              tr("Replaced %1 objects.").arg(count));
}

void FindObjectWidget::_replaceCurrent()
{
    FWObject *o = lastFound;
    FWObject *p = lastFound->getParent();

    if (p==nullptr || o==nullptr) return;
    if (FWReference::cast(o)==nullptr) return;

    if (p->isReadOnly())
    {
        QMessageBox msg_box;

        msg_box.setWindowModality(Qt::ApplicationModal);

        msg_box.setWindowTitle("Find and Replace Error");
        msg_box.setText(tr("Can not modify read-only object %1")
                        .arg(p->getPath().c_str()));
        QPushButton *btn_continue = 
            msg_box.addButton(
                QObject::tr("Continue"), QMessageBox::AcceptRole);
        Q_UNUSED(btn_continue);
        QPushButton *btn_stop = 
            msg_box.addButton(
                QObject::tr("Stop"), QMessageBox::RejectRole);

        msg_box.setDefaultButton(btn_stop);

        msg_box.show();
        msg_box.raise();

        msg_box.exec();

        if (msg_box.clickedButton() == btn_stop)
        {
            // interrupt "replace all" operation
            throw FindAndReplaceError("Attempt to modify read-only object");
        }

        return;
    }

    FWObject *replace_object = m_widget->replaceDropArea->getObject();

    std::unique_ptr<FWCmdChange> cmd(
        new FWCmdChange(project_panel, p, QObject::tr("Replace object")));
    FWObject *new_state = cmd->getNewState();

    new_state->removeRef(FWReference::cast(o)->getPointer());

    // check for duplicates --------

    if (RuleElement::cast(new_state)==nullptr ||
        !RuleElement::cast(new_state)->isAny())
    {
        // avoid duplicates
        int cp_id = replace_object->getId();
        FWObject *oo;
        FWReference *ref;

        list<FWObject*>::iterator j;
        for(j=new_state->begin(); j!=new_state->end(); ++j)
        {
            oo = *j;
            if (cp_id==oo->getId() ||
                ((ref=FWReference::cast(oo))!=nullptr &&
                 cp_id==ref->getPointerId()))
            {
                // replacement object is already a member of this
                // group or rule element. Do not insert it again to
                // avoid duplicates. Also check if new_state is
                // different from old_state. It can be the same if
                // original object was not a member of the group so
                // the call to removeRef() above did nothing. This
                // happens when user hits Replace, then hits it
                // again without finding next.
                if (!cmd->getOldState()->cmp(new_state, true))
                    project_panel->undoStack->push(cmd.release());
                return;
            }
        }
    }

    new_state->addRef(replace_object);
    if (!cmd->getOldState()->cmp(new_state, true))
        project_panel->undoStack->push(cmd.release());
}

bool FindObjectWidget::inSelectedFirewall( RuleElement* r)
{
    FWObject *f=r;
    while (f!=nullptr && Firewall::cast(f)==nullptr) f=f->getParent();
    if (f==nullptr) return false;

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

/**
 * Show object that we have found in the tree or in rules but retain focus
 * so that user can find next one by just hitting Return on the keyboard.
 */
void FindObjectWidget::showObject(FWObject* o)
{
    if (fwbdebug)
        qDebug("FindObjectWidget::showObject  o: %s parent: %s",
               o->getName().c_str(), o->getParent()->getName().c_str());

    FWReference* ref = FWReference::cast(o);
    if (ref!=nullptr && RuleElement::cast(o->getParent())!=nullptr)
    {
        // found object in rules
        QCoreApplication::sendEvent(
            project_panel, new showObjectInRulesetEvent(
                project_panel->getFileName(), ref->getId()));
        return;
    }

    if (!FWBTree().isStandardFolder(o) &&
        Group::cast(o->getParent())!=nullptr &&
        !FWBTree().isStandardFolder(o->getParent()))
    {
        QCoreApplication::sendEvent(
            mw, new showObjectInTreeEvent(project_panel->getFileName(),
                                          o->getParent()->getId()));
        return;
    }

    QCoreApplication::sendEvent(
        mw, new showObjectInTreeEvent(project_panel->getFileName(), o->getId()));
}

void FindObjectWidget::init()
{
    m_widget->findDropArea->deleteObject();
    m_widget->replaceDropArea->deleteObject();
}

void FindObjectWidget::firewallOpened(Firewall *f)
{
    if (f==nullptr) return;
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

/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: CustomServiceDialog.cpp,v 1.21 2007/04/14 00:18:43 vkurland Exp $

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


#include "config.h"
#include "global.h"
#include "utils.h"

#include "FWBTree.h"
#include "CustomServiceDialog.h"
#include "ObjectManipulator.h"
#include "FWBSettings.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Resources.h"

#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include <iostream>

using namespace libfwbuilder;
using namespace std;

CustomServiceDialog::CustomServiceDialog(QWidget *parent) : QWidget(parent) 
{ 
    m_dialog = new Ui::CustomServiceDialog_q;
    m_dialog->setupUi(this);
    obj=NULL; 
}

CustomServiceDialog::~CustomServiceDialog()
{
    delete m_dialog;
}

void CustomServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    CustomService *s = dynamic_cast<CustomService*>(obj);
    assert(s!=NULL);

    init=true;

    fillLibraries(m_dialog->libs,obj);

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

/* fill in m_dialog->platform */
    m_dialog->platform->clear();

    int cp=0;
    showPlatform=st->value("/FirewallBuilder2/CustomService/Platform").toString();
    QMap<QString,QString> platforms = getAllPlatforms();
    QMap<QString,QString>::iterator i;
    for (i=platforms.begin(); i!=platforms.end(); i++,cp++)
    {
//        cerr << "m_dialog->platform: key=" << i.key() << "  data=" << i.data() << endl;

/* here i.key is m_dialog->platform m_dialog->code ( "ipf", "ipfw", "iptables", "pf")
 * while i.data is human readable name ("ipfilter", "PF" )
 */
        platformReverseMap[i.value()]=i.key();

        m_dialog->platform->addItem( i.value() );
        if (showPlatform=="") showPlatform = i.key();
        if (showPlatform==i.key()) m_dialog->platform->setCurrentIndex( cp );
        allCodes[ i.key() ]=s->getCodeForPlatform( i.key().toLatin1().constData() ).c_str();
    }

    m_dialog->code->setText( allCodes[showPlatform] ); //fromUtf8

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->libs->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->libs);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);

//    m_dialog->platform->setEnabled(!o->isReadOnly());

    m_dialog->code->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->code);


    init=false;
}
    
void CustomServiceDialog::changed()
{
    if (!init)
    {
        QString  pl   = platformReverseMap[m_dialog->platform->currentText()];
        allCodes[pl] = m_dialog->code->text().toUtf8().constData();

        //apply->setEnabled( true );
        emit changed_sign();

    }
}

void CustomServiceDialog::validate(bool *res)
{
    *res=true;
    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}

void CustomServiceDialog::isChanged(bool *res)
{
    //*res=(!init && apply->isEnabled());
}

void CustomServiceDialog::libChanged()
{
    changed();
}

void CustomServiceDialog::platformChanged()
{
    init=true;
    QString npl = platformReverseMap[m_dialog->platform->currentText()];
    m_dialog->code->setText( allCodes[ npl ] ); //fromUtf8
    showPlatform = npl;
    st->setValue("/FirewallBuilder2/CustomService/Platform",showPlatform);
    init=false;
//    changed();
}

void CustomServiceDialog::applyChanges()
{
    CustomService *s = dynamic_cast<CustomService*>(obj);
    assert(s!=NULL);

    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    string commText = string(m_dialog->comment->toPlainText().toUtf8().constData());
    obj->setComment( commText );

    QMap<QString,QString>::iterator i;
    for (i=allCodes.begin(); i!=allCodes.end(); ++i)
    {
        string code = string(i.value().toUtf8().constData());
        if (fwbdebug)
            qDebug("Storing custom service m_dialog->code %s :: %s",
                   i.key().toLatin1().constData(),code.c_str());
        s->setCodeForPlatform( i.key().toLatin1().constData(), code );
    }
    om->updateObjName(obj,QString::fromUtf8(oldname.c_str()));
 
    init=true;

/* move to another lib if we have to */
    if (! FWBTree::isSystem(obj) && m_dialog->libs->currentText() != QString(obj->getLibrary()->getName().c_str()))
        om->moveObject(m_dialog->libs->currentText(), obj);

    init=false;

    //apply->setEnabled( false );
    om->updateLastModifiedTimestampForAllFirewalls(obj);
}

void CustomServiceDialog::discardChanges()
{
    loadFWObject(obj);
}

/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void CustomServiceDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}


/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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

#include "utils.h"
#include "global.h"

#include "DialogData.h"

#include "fwbuilder/FWObject.h"

#include <qwidget.h>
#include <qcombobox.h>
#include <qlistwidget.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <QtDebug>

#include <iostream>

using namespace std;
using namespace libfwbuilder;

DialogOption::DialogOption(QWidget *_w, FWObject *_o, const char* _a)
{
    w       = _w;
    obj     = _o;
    attr    = _a;

    dtype   = Unknown;

    if (dynamic_cast<QComboBox*>(w)!=nullptr)    dtype=String;
    if (dynamic_cast<QCheckBox*>(w)!=nullptr)    dtype=Bool;
    if (dynamic_cast<QLineEdit*>(w)!=nullptr)    dtype=String;
    if (dynamic_cast<QTextEdit*>(w)!=nullptr)    dtype=String;
    if (dynamic_cast<QRadioButton*>(w)!=nullptr) dtype=Bool;
    if (dynamic_cast<QSpinBox*>(w)!=nullptr)     dtype=Int;
}

DialogOption::DialogOption(QWidget *_w, FWObject *_o, const char* _a,QStringList _m)
{
    w       = _w;
    obj     = _o;
    attr    = _a;
    mapping = _m;

    dtype   = Unknown;

    if (dynamic_cast<QComboBox*>(w)!=nullptr)    dtype=String;
    if (dynamic_cast<QCheckBox*>(w)!=nullptr)    dtype=Bool;
    if (dynamic_cast<QLineEdit*>(w)!=nullptr)    dtype=String;
    if (dynamic_cast<QTextEdit*>(w)!=nullptr)    dtype=String;
    if (dynamic_cast<QRadioButton*>(w)!=nullptr) dtype=Bool;
    if (dynamic_cast<QSpinBox*>(w)!=nullptr)     dtype=Int;
}


DialogData::DialogData() {}
DialogData::~DialogData()
{
    options.clear();
}

void DialogData::clear()
{
    options.clear();
}

void DialogData::registerOption(QWidget *widget, libfwbuilder::FWObject *obj, const char* attr)
{
    options.push_back( DialogOption(widget,obj,attr) );
}

void DialogData::registerOption(QWidget *widget, libfwbuilder::FWObject *obj, const char* attr,QStringList mapping)
{
    options.push_back( DialogOption(widget,obj,attr,mapping) );
}

void DialogData::loadToWidget( DialogOption &dopt , bool override)
{
    if (dynamic_cast<QComboBox*>(dopt.w)!=nullptr)
    {
        QComboBox *cbx = dynamic_cast<QComboBox*>(dopt.w);
        QString      s = (override) ? dopt.override_str_val : QString(dopt.obj->getStr(dopt.attr.toLatin1().constData()).c_str());

        int current_item = 0;
        if (!dopt.mapping.empty())
        {
/*
 * REMINDER:
 * Mapping is defined by an array of strings in the following format:
 *
 *     "Linux 2.4" , "linux24",
 *     "IPFilter"  , "ipf",
 *     "Cisco PIX" , "pix",
 *     nullptr,         nullptr
 *
 *  Odd strings correspond to the data in the widget, while even
 *  strings define what is stored in the object (counting strings in
 *  the array from 1).
 */
            int idx = 0;
            QStringList::iterator i1 = dopt.mapping.begin();
            QStringList::iterator i2 = dopt.mapping.begin();
            ++i2;

            if (fwbdebug)
            {
                qDebug("loadToWidget -- QComboBox  dopt.mapping.count()=%d",dopt.mapping.count());
                qDebug("loadToWidget -- QComboBox  s=%s",s.toLatin1().constData());
            }

            while ( idx < dopt.mapping.count()/2 )
            {
                if (fwbdebug)
                {
                    qDebug("loadToWidget -- QComboBox  (*i1)=%s",(*i1).toLatin1().constData());
                    qDebug("loadToWidget -- QComboBox  (*i2)=%s",(*i2).toLatin1().constData());
                }

                if (s== (*i2)) { current_item = idx; }
                i1++; i1++;
                i2++; i2++;
                idx++;
            }
        } else {
// no mapping, just scan items and find current
            current_item= cbx->findText(s,Qt::MatchExactly);
        }
        cbx->setCurrentIndex( current_item );
    }
    if (dynamic_cast<QCheckBox*>(dopt.w)!=nullptr)
    {
        QCheckBox *cbx=dynamic_cast<QCheckBox*>(dopt.w);
        cbx->setChecked( (override)?dopt.override_int_val:dopt.obj->getBool(dopt.attr.toLatin1().constData()) );
    }
    if (dynamic_cast<QLineEdit*>(dopt.w)!=nullptr)
    {
        QLineEdit *edit=dynamic_cast<QLineEdit*>(dopt.w);
        edit->setText( (override) ? dopt.override_str_val : QString(dopt.obj->getStr(dopt.attr.toLatin1().constData()).c_str()) );
    }
    if (dynamic_cast<QTextEdit*>(dopt.w)!=nullptr)
    {
        QTextEdit *edit=dynamic_cast<QTextEdit*>(dopt.w);
        edit->setText( (override) ? dopt.override_str_val : QString(dopt.obj->getStr(dopt.attr.toLatin1().constData()).c_str()) );
    }
    if (dynamic_cast<QRadioButton*>(dopt.w)!=nullptr)
    {
        QRadioButton *rbtn=dynamic_cast<QRadioButton*>(dopt.w);
        rbtn->setChecked( (override)?dopt.override_int_val:dopt.obj->getBool(dopt.attr.toLatin1().constData()) );
    }
    if (dynamic_cast<QSpinBox*>(dopt.w)!=nullptr)
    {
        QSpinBox *sbx = dynamic_cast<QSpinBox*>(dopt.w);
        sbx->setValue( (override)?dopt.override_int_val:dopt.obj->getInt(dopt.attr.toLatin1().constData()) );
    }
}


void DialogData::loadAll()
{
    for (list<DialogOption>::iterator i=options.begin(); i!=options.end(); ++i)
        loadToWidget( *i );
}

void DialogData::saveAll(FWObject *new_obj)
{
    for (list<DialogOption>::iterator i=options.begin(); i!=options.end(); ++i)
    {
        FWObject *use_obj = (new_obj!=nullptr) ? new_obj : i->obj;

        if (dynamic_cast<QComboBox*>(i->w)!=nullptr)
        {
            QComboBox *cbx = dynamic_cast<QComboBox*>(i->w);
            QString s = cbx->currentText();
            if (fwbdebug)
                qDebug() << "DialogData::saveAll() QComboBox"
                         << i->w->objectName()
                         << "s=" << s.toUtf8();

            if ( !i->mapping.empty() && !s.isNull() )
            {
                if (fwbdebug) qDebug("Remapping...");
/*
 * REMINDER:
 * Mapping is defined by an array of strings in the following format:
 *
 * char *mapping[] = {
 *     "Linux 2.4" , "linux24",
 *     "IPFilter"  , "ipf",
 *     "Cisco PIX" , "pix",
 *     nullptr,         nullptr
 *  };
 *
 *  Odd strings correspond to the data in the widget, while even
 *  strings define what is stored in the object (counting strings in
 *  the array from 1).
 */
                QStringList::iterator i1 = i->mapping.begin();
                QStringList::iterator i2 = i->mapping.begin();
                ++i2;
                while (i1!=i->mapping.end())
                {
                    if (fwbdebug)
                        qDebug() << "   (*i1)=" << *i1;

                    if (s== (*i1)) { s= *i2; break; }
                    i1++; i1++;
                    i2++; i2++;
                }

            }
            if (s.isEmpty()) s="";
            use_obj->setStr(i->attr.toLatin1().constData(), s.toLatin1().constData());
        }
        if (dynamic_cast<QCheckBox*>(i->w)!=nullptr)
        {
            QCheckBox *cbx=dynamic_cast<QCheckBox*>(i->w);
            use_obj->setBool(i->attr.toLatin1().constData(), cbx->isChecked() );
        }
        if (dynamic_cast<QLineEdit*>(i->w)!=nullptr)
        {
            QLineEdit *edit=dynamic_cast<QLineEdit*>(i->w);
            use_obj->setStr(i->attr.toLatin1().constData(),
                            edit->text().toLatin1().constData() );
        }
        if (dynamic_cast<QTextEdit*>(i->w)!=nullptr)
        {
            QTextEdit *edit=dynamic_cast<QTextEdit*>(i->w);
            use_obj->setStr(i->attr.toLatin1().constData(),
                            edit->toPlainText().toLatin1().constData() );
        }
        if (dynamic_cast<QRadioButton*>(i->w)!=nullptr)
        {
            QRadioButton *rbtn=dynamic_cast<QRadioButton*>(i->w);
            use_obj->setBool(i->attr.toLatin1().constData(), rbtn->isChecked() );
        }
        if (dynamic_cast<QSpinBox*>(i->w)!=nullptr)
        {
            QSpinBox *sbx = dynamic_cast<QSpinBox*>(i->w);
            use_obj->setInt( i->attr.toLatin1().constData(), sbx->value() );
        }

    }
}

void DialogData::setWidgetValue(const char *attr,const QString &val)
{
    for (list<DialogOption>::iterator i=options.begin(); i!=options.end(); ++i)
    {
        if (i->attr == attr)
        {
            i->overrideValue(val);
            loadToWidget( *i , true );
            break;
        }
    }
}


void DialogData::setWidgetValue(const char *attr,int val)
{
    for (list<DialogOption>::iterator i=options.begin(); i!=options.end(); ++i)
    {
        if (i->attr == attr)
        {
            i->overrideValue(val);
            loadToWidget( *i , true );
            break;
        }
    }
}



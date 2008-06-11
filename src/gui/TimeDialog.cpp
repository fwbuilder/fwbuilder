/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: TimeDialog.cpp,v 1.16 2007/03/14 05:08:11 vkurland Exp $

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

#include "FWBTree.h"
#include "TimeDialog.h"
#include "ProjectPanel.h"

#include "FWWindow.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Interval.h"

#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>

#include <iostream>
#include "FWBSettings.h"

using namespace libfwbuilder;
using namespace std;

TimeDialog::TimeDialog(ProjectPanel *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    m_dialog = new Ui::TimeDialog_q;
    m_dialog->setupUi(this);
    setFont(st->getUiFont());

    obj=NULL;
}

TimeDialog::~TimeDialog()
{
    delete m_dialog;
}

void TimeDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Interval *s = dynamic_cast<Interval*>(obj);
    assert(s!=NULL);

    init=true;

    m_dialog->cbStart1_2->setCheckState(Qt::Unchecked);
    m_dialog->cbStart2_2->setCheckState(Qt::Unchecked);
    m_dialog->cbStart3_2->setCheckState(Qt::Unchecked);
    m_dialog->cbStart4_2->setCheckState(Qt::Unchecked);
    m_dialog->cbStart5_2->setCheckState(Qt::Unchecked);
    m_dialog->cbStart6_2->setCheckState(Qt::Unchecked);
    m_dialog->cbStart7_2->setCheckState(Qt::Unchecked);

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->comment->setPlainText( QString::fromUtf8(s->getComment().c_str()) );

    /*switch (m_dialog->startDate->order())
    {
    case QDateEdit::MDY:
        m_dialog->startDateLabel->setText( tr("(M/D/Y)") );
        m_dialog->endDateLabel->setText(   tr("(M/D/Y)") );
        break;

    case QDateEdit::DMY:
        m_dialog->startDateLabel->setText( tr("(D/M/Y)") );
        m_dialog->endDateLabel->setText(   tr("(D/M/Y)") );
        break;

    case QDateEdit::YMD:
        m_dialog->startDateLabel->setText( tr("(Y/M/D)") );
        m_dialog->endDateLabel->setText(   tr("(Y/M/D)") );
        break;

    case QDateEdit::YDM:
        m_dialog->startDateLabel->setText( tr("(Y/D/M)") );
        m_dialog->endDateLabel->setText(   tr("(Y/D/M)") );
        break;

    }*/
    int fromH = obj->getInt("from_hour");
    int fromM = obj->getInt("from_minute");
    if (fromH<0) fromH=0;
    if (fromM<0) fromM=0;
    m_dialog->startTime->setTime( QTime( fromH, fromM ) );

    int y=obj->getInt("from_year");
    int m=obj->getInt("from_month");
    int d=obj->getInt("from_day");
    bool using_start_date = (y>0 && m>0 && d>0);
    m_dialog->startDate->setDate( (using_start_date)?QDate( y, m, d ):QDate() );
    m_dialog->useStartDate->setChecked(using_start_date);

    // from_weekday is -1 for "All days"
//    m_dialog->startDOW->setCurrentIndex( obj->getInt("from_weekday") + 1 );
    QString sFromWeekday = s->getDaysOfWeek().c_str();

    if (sFromWeekday=="-1")
        sFromWeekday="0,1,2,3,4,5,6";
    // Sunday is cbStart7
    if (sFromWeekday.contains('0'))
        m_dialog->cbStart7_2->setCheckState(Qt::Checked);
    if (sFromWeekday.contains('1'))
        m_dialog->cbStart1_2->setCheckState(Qt::Checked);
    if (sFromWeekday.contains('2'))
        m_dialog->cbStart2_2->setCheckState(Qt::Checked);
    if (sFromWeekday.contains('3'))
        m_dialog->cbStart3_2->setCheckState(Qt::Checked);
    if (sFromWeekday.contains('4'))
        m_dialog->cbStart4_2->setCheckState(Qt::Checked);
    if (sFromWeekday.contains('5'))
        m_dialog->cbStart5_2->setCheckState(Qt::Checked);
    if (sFromWeekday.contains('6'))
        m_dialog->cbStart6_2->setCheckState(Qt::Checked);

    int toH = obj->getInt("to_hour");
    int toM = obj->getInt("to_minute");
    if (toH<0) toH=0;
    if (toM<0) toM=0;
    m_dialog->endTime->setTime( QTime( toH, toM ) );

    y=obj->getInt("to_year");
    m=obj->getInt("to_month");
    d=obj->getInt("to_day");
    bool using_end_date = (y>0 && m>0 && d>0);
    m_dialog->endDate->setDate( (using_end_date)?QDate( y, m, d ):QDate() );
    m_dialog->useEndDate->setChecked(using_end_date);

    setDisabledPalette(m_dialog->obj_name);
    setDisabledPalette(m_dialog->comment);
    setDisabledPalette(m_dialog->startTime);
    setDisabledPalette(m_dialog->useStartDate);
    setDisabledPalette(m_dialog->startDate);
    //setDisabledPalette(startDOW);
    setDisabledPalette(m_dialog->endTime);
    setDisabledPalette(m_dialog->useEndDate);
    setDisabledPalette(m_dialog->endDate);
    //setDisabledPalette(endDOW);

    enableAllWidgets();

    //apply->setEnabled( false );


    init=false;
}

void TimeDialog::enableAllWidgets()
{
    m_dialog->obj_name->setEnabled(!obj->isReadOnly());
    m_dialog->comment->setReadOnly(obj->isReadOnly());

    m_dialog->startTime->setEnabled(!obj->isReadOnly());
    m_dialog->useStartDate->setEnabled(!obj->isReadOnly());
    m_dialog->startDate->setEnabled(!obj->isReadOnly() && m_dialog->useStartDate->isChecked());
//    m_dialog->startDOW->setEnabled(!obj->isReadOnly() && !m_dialog->useStartDate->isChecked());
/*    m_dialog->cbStart1_2->setEnabled(!obj->isReadOnly() && !m_dialog->useStartDate->isChecked());
    m_dialog->cbStart2_2->setEnabled(!obj->isReadOnly() && !m_dialog->useStartDate->isChecked());
    m_dialog->cbStart3_2->setEnabled(!obj->isReadOnly() && !m_dialog->useStartDate->isChecked());
    m_dialog->cbStart4_2->setEnabled(!obj->isReadOnly() && !m_dialog->useStartDate->isChecked());
    m_dialog->cbStart5_2->setEnabled(!obj->isReadOnly() && !m_dialog->useStartDate->isChecked());
    m_dialog->cbStart6_2->setEnabled(!obj->isReadOnly() && !m_dialog->useStartDate->isChecked());
    m_dialog->cbStart7_2->setEnabled(!obj->isReadOnly() && !m_dialog->useStartDate->isChecked());*/

    m_dialog->endTime->setEnabled(!obj->isReadOnly());
    m_dialog->useEndDate->setEnabled(!obj->isReadOnly());
    m_dialog->endDate->setEnabled(!obj->isReadOnly() && m_dialog->useEndDate->isChecked());

//    m_dialog->endDOW->setEnabled(!obj->isReadOnly() && !m_dialog->useEndDate->isChecked());
}

void TimeDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void TimeDialog::useStartOrEndDate()
{
    enableAllWidgets();
    changed();
}

void TimeDialog::validate(bool *res)
{
    *res=true;
}

void TimeDialog::isChanged(bool*)
{
    //*res=(!init && apply->isEnabled());
}

void TimeDialog::libChanged()
{
    changed();
}

void TimeDialog::applyChanges()
{
    if (!isTreeReadWrite(this,obj)) return;
    Interval *s = dynamic_cast<Interval*>(obj);
    assert(s!=NULL);

    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    if (m_dialog->useStartDate->isChecked())
    {
        obj->setInt( "from_day"   ,      m_dialog->startDate->date().day()   );
        obj->setInt( "from_month" ,      m_dialog->startDate->date().month() );
        obj->setInt( "from_year"  ,      m_dialog->startDate->date().year()  );
    } else
    {
        obj->setInt( "from_day"   ,      -1 );
        obj->setInt( "from_month" ,      -1 );
        obj->setInt( "from_year"  ,      -1 );
    }
    obj->setInt( "from_minute"   ,   m_dialog->startTime->time().minute());
    obj->setInt( "from_hour"  ,      m_dialog->startTime->time().hour()  );

    if (m_dialog->useEndDate->isChecked())
    {
        obj->setInt( "to_day"   ,        m_dialog->endDate->date().day()     );
        obj->setInt( "to_month" ,        m_dialog->endDate->date().month()   );
        obj->setInt( "to_year"  ,        m_dialog->endDate->date().year()    );
    } else
    {
        obj->setInt( "to_day"   ,      -1 );
        obj->setInt( "to_month" ,      -1 );
        obj->setInt( "to_year"  ,      -1 );
    }
    obj->setInt( "to_minute"   ,     m_dialog->endTime->time().minute()  );
    obj->setInt( "to_hour"  ,        m_dialog->endTime->time().hour()    );

    QStringList weekDays ;
    if (m_dialog->cbStart7_2->checkState ()==Qt::Checked)
        weekDays.append("0");
    if (m_dialog->cbStart1_2->checkState ()==Qt::Checked)
        weekDays.append("1");
    if (m_dialog->cbStart2_2->checkState ()==Qt::Checked)
        weekDays.append("2");
    if (m_dialog->cbStart3_2->checkState ()==Qt::Checked)
        weekDays.append("3");
    if (m_dialog->cbStart4_2->checkState ()==Qt::Checked)
        weekDays.append("4");
    if (m_dialog->cbStart5_2->checkState ()==Qt::Checked)
        weekDays.append("5");
    if (m_dialog->cbStart6_2->checkState ()==Qt::Checked)
        weekDays.append("6");

    s->setDaysOfWeek(weekDays.join(",").toAscii().data());

    mw->updateObjName(obj, QString::fromUtf8(oldname.c_str()));

    //apply->setEnabled( false );
    mw->updateLastModifiedTimestampForAllFirewalls(obj);
}

void TimeDialog::discardChanges()
{
    loadFWObject(obj);
}

/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void TimeDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}


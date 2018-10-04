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

#include "FWBTree.h"
#include "TimeDialog.h"
#include "ProjectPanel.h"

#include "FWWindow.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Interval.h"

#include <memory>

#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <QUndoStack>
#include <QDebug>

#include <iostream>
#include "FWBSettings.h"

using namespace libfwbuilder;
using namespace std;

TimeDialog::TimeDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::TimeDialog_q;
    m_dialog->setupUi(this);

    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

TimeDialog::~TimeDialog()
{
    delete m_dialog;
}

void TimeDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Interval *s = dynamic_cast<Interval*>(obj);
    assert(s!=nullptr);

    init = true;

    // See #893 No need to show object attributes if the object is "Any"
    if (obj->getId() == FWObjectDatabase::ANY_INTERVAL_ID)
    {
        m_dialog->object_attributes_1->hide();
        m_dialog->object_attributes_2->hide();

        m_dialog->commentKeywords->setReadOnlyComment(
            QObject::tr(
                "When used in the Time Interval field of a rule, "
                "the Any object will match any time of the day or day "
                "of the week. To update your rule to "
                "match only specific "
                "service, drag-and-drop an object from "
                "the Object tree into the field in the rule."));
    } else
    {
        m_dialog->cbStart1_2->setCheckState(Qt::Unchecked);
        m_dialog->cbStart2_2->setCheckState(Qt::Unchecked);
        m_dialog->cbStart3_2->setCheckState(Qt::Unchecked);
        m_dialog->cbStart4_2->setCheckState(Qt::Unchecked);
        m_dialog->cbStart5_2->setCheckState(Qt::Unchecked);
        m_dialog->cbStart6_2->setCheckState(Qt::Unchecked);
        m_dialog->cbStart7_2->setCheckState(Qt::Unchecked);

        m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
        m_dialog->commentKeywords->loadFWObject(o);

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

        m_dialog->object_attributes_1->show();
        m_dialog->object_attributes_2->show();

        setDisabledPalette(m_dialog->obj_name);
        //setDisabledPalette(m_dialog->comment);
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
    }

    init = false;
}

void TimeDialog::enableAllWidgets()
{
    m_dialog->obj_name->setEnabled(!obj->isReadOnly());

    m_dialog->startTime->setEnabled(!obj->isReadOnly());
    m_dialog->useStartDate->setEnabled(!obj->isReadOnly());
    m_dialog->startDate->setEnabled(!obj->isReadOnly() &&
                                    m_dialog->useStartDate->isChecked());

    m_dialog->endTime->setEnabled(!obj->isReadOnly());
    m_dialog->useEndDate->setEnabled(!obj->isReadOnly());
    m_dialog->endDate->setEnabled(!obj->isReadOnly() && m_dialog->useEndDate->isChecked());

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

void TimeDialog::applyChanges()
{

    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    Interval *interval = dynamic_cast<Interval*>(new_state);
    assert(interval!=nullptr);

    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    if (m_dialog->useStartDate->isChecked())
    {
        new_state->setInt( "from_day"   ,      m_dialog->startDate->date().day()   );
        new_state->setInt( "from_month" ,      m_dialog->startDate->date().month() );
        new_state->setInt( "from_year"  ,      m_dialog->startDate->date().year()  );
    } else
    {
        new_state->setInt( "from_day"   ,      -1 );
        new_state->setInt( "from_month" ,      -1 );
        new_state->setInt( "from_year"  ,      -1 );
    }
    new_state->setInt( "from_minute"   ,   m_dialog->startTime->time().minute());
    new_state->setInt( "from_hour"  ,      m_dialog->startTime->time().hour()  );

    if (m_dialog->useEndDate->isChecked())
    {
        new_state->setInt( "to_day"   ,        m_dialog->endDate->date().day()     );
        new_state->setInt( "to_month" ,        m_dialog->endDate->date().month()   );
        new_state->setInt( "to_year"  ,        m_dialog->endDate->date().year()    );
    } else
    {
        new_state->setInt( "to_day"   ,      -1 );
        new_state->setInt( "to_month" ,      -1 );
        new_state->setInt( "to_year"  ,      -1 );
    }
    new_state->setInt( "to_minute"   ,     m_dialog->endTime->time().minute()  );
    new_state->setInt( "to_hour"  ,        m_dialog->endTime->time().hour()    );

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

    interval->setDaysOfWeek(weekDays.join(",").toLatin1().data());

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
}


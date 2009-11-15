/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "FWObjectState.h"

void FWObjectStateTime::save(libfwbuilder::FWObject* object)
{
    FWObjectState::save(object);

    int y = object->getInt("from_year");
    int m = object->getInt("from_month");
    int d = object->getInt("from_day");

    int mm = object->getInt("from_minute");
    int hh = object->getInt("from_hour");

    if (QDate::isValid(y, m, d)) startDate = QDate(y, m, d);
    if (QTime::isValid(hh, mm, 0)) startTime = QTime(hh, mm, 0);

    y = object->getInt("to_year");
    m = object->getInt("to_month");
    d = object->getInt("to_day");

    mm = object->getInt("to_minute");
    hh = object->getInt("to_hour");

    if (QDate::isValid(y, m, d)) endDate = QDate(y, m, d);
    if (QTime::isValid(hh, mm, 0)) endTime = QTime(hh, mm, 0);

    days_of_week = object->getStr("days_of_week").c_str();
}

void FWObjectStateTime::restore(libfwbuilder::FWObject* object)
{
    FWObjectState::restore(object);

    if (startTime.isValid())
    {
        object->setInt("from_minute", startTime.minute());
        object->setInt("from_hour", startTime.hour());
    } else
    {
        object->setInt("from_minute", -1);
        object->setInt("from_hour", -1);
    }

    if (startDate.isValid())
    {
        object->setInt("from_day", startDate.day());
        object->setInt("from_month", startDate.month());
        object->setInt("from_year", startDate.year());
    } else
    {
        object->setInt("from_day", -1);
        object->setInt("from_month", -1);
        object->setInt("from_year", -1);
    }


    if (endTime.isValid()) {
        object->setInt("to_minute", endTime.minute());
        object->setInt("to_hour", endTime.hour());
    } else
    {
        object->setInt("to_minute", -1);
        object->setInt("to_hour", -1);
    }

    if (endDate.isValid())
    {
        object->setInt("to_day", endDate.day());
        object->setInt("to_month", endDate.month());
        object->setInt("to_year", endDate.year());
    } else
    {
        object->setInt("to_day", -1);
        object->setInt("to_month", -1);
        object->setInt("to_year", -1);
    }

    object->setStr("days_of_week", days_of_week.toUtf8().data());
}



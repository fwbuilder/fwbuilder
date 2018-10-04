/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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



#include "fwbuilder/Interval.h"
#include "fwbuilder/FWIntervalReference.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWObjectDatabase.h"

#include <sstream>

using namespace libfwbuilder;

const char *Interval::TYPENAME={"Interval"};

Interval::Interval()
{
    setInt("from_minute", -1);
    setInt("from_hour", -1);
    setInt("from_day", -1);
    setInt("from_month", -1);
    setInt("from_year", -1);
    setInt("from_weekday", -1);

    setInt("to_minute", -1);
    setInt("to_hour", -1);
    setInt("to_day", -1);
    setInt("to_month", -1);
    setInt("to_year", -1);
    setInt("to_weekday", -1);

    setStr("days_of_week", "");
}

std::string Interval::constructDaysOfWeek(int sdayofweek, int edayofweek)
{
    std::ostringstream  ostr;

    if (sdayofweek<0) sdayofweek=0;
    if (sdayofweek>6) sdayofweek=6;
    
    // if both start and end day are -1, need to
    // generate "sun,mon,tue,wed,thu,fri,sat"
    if (edayofweek<0) edayofweek=6;
    if (edayofweek>6) edayofweek=6;

    bool first=true;
    bool inside_interval = false;
    int  day=0;
    while (1)
    {
        if (!inside_interval && day==sdayofweek) inside_interval=true;
        if (inside_interval)
        {
            if (!first) ostr << ",";
            first=false;
            ostr << day;
            // if sdayofweek==edayofweek print one day
            if (day==edayofweek)  break;
        }
        if (++day>6) day=0;
    }
    return ostr.str();
}


void Interval::setStartTime(int min,int hour,int day,int month,int year,int dayofweek)
{
    setInt("from_minute", min);
    setInt("from_hour", hour);
    setInt("from_day", day);
    setInt("from_month", month);
    setInt("from_year", year);
    setInt("from_weekday", dayofweek);

    setStr("days_of_week", constructDaysOfWeek(dayofweek,getInt("to_weekday")));
}

void Interval::setEndTime(int min,int hour,int day,int month,int year,int dayofweek)
{
    setInt("to_minute", min);
    setInt("to_hour", hour);
    setInt("to_day", day);
    setInt("to_month", month);
    setInt("to_year", year);
    setInt("to_weekday", dayofweek);

    setStr("days_of_week", constructDaysOfWeek(getInt("from_weekday"),dayofweek));
}



void Interval::setStartTime(int min,int hour,int day,int month,int year)
{
    setInt("from_minute", min);
    setInt("from_hour", hour);
    setInt("from_day", day);
    setInt("from_month", month);
    setInt("from_year", year);
}

void Interval::setEndTime(int min,int hour,int day,int month,int year)
{
    setInt("to_minute", min);
    setInt("to_hour", hour);
    setInt("to_day", day);
    setInt("to_month", month);
    setInt("to_year", year);
}

void Interval::setDaysOfWeek(const std::string &days_of_week)
{
    setStr("days_of_week", days_of_week);
}


void Interval::getStartTime(int *min,int *hour,int *day,int *month,int *year,int *dayofweek) const
{
    *min=getInt("from_minute");
    *hour=getInt("from_hour");
    *day=getInt("from_day");
    *month=getInt("from_month");
    *year=getInt("from_year");
    *dayofweek=getInt("from_weekday");
}

void Interval::getEndTime(int *min,int *hour,int *day,int *month,int *year,int *dayofweek) const
{
    *min=getInt("to_minute");
    *hour=getInt("to_hour");
    *day=getInt("to_day");
    *month=getInt("to_month");
    *year=getInt("to_year");
    *dayofweek=getInt("to_weekday");
}

/**
 * Returns a string, a comma separated list of day numbers. Sunday is
 * day "0"
 */
std::string Interval::getDaysOfWeek()
{
    std::string days_of_week = getStr("days_of_week");
    int sdayofweek = getInt("from_weekday");
    int edayofweek = getInt("to_weekday");
    if (!days_of_week.empty())
        return getStr("days_of_week");

    if (sdayofweek!=-1 || edayofweek!=-1)
        // Old school representation of the days of week
        return constructDaysOfWeek(sdayofweek, edayofweek);

    return "";
}

void Interval::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    const char *n;

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("from_minute")));
    if (n!=nullptr)
    {
        setStr("from_minute", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("from_hour")));
    if (n!=nullptr)
    {
        setStr("from_hour", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("from_day")));
    if (n!=nullptr)
    {
        setStr("from_day", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("from_month")));
    if (n!=nullptr)
    {
        setStr("from_month", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("from_year")));
    if (n!=nullptr)
    {
        setStr("from_year", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("from_weekday")));
    if (n!=nullptr)
    {
        setStr("from_weekday", n);
        XMLTools::FreeXmlBuff(n);
    }


    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("to_minute")));
    if (n!=nullptr)
    {
        setStr("to_minute", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("to_hour")));
    if (n!=nullptr)
    {
        setStr("to_hour", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("to_day")));
    if (n!=nullptr)
    {
        setStr("to_day", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("to_month")));
    if (n!=nullptr)
    {
        setStr("to_month", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("to_year")));
    if (n!=nullptr)
    {
        setStr("to_year", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("to_weekday")));
    if (n!=nullptr)
    {
        setStr("to_weekday", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("days_of_week")));
    if (n!=nullptr)
    {
        setStr("days_of_week", n);
        XMLTools::FreeXmlBuff(n);
    } else
    {
        setStr("days_of_week", 
               constructDaysOfWeek(getInt("from_weekday"), getInt("to_weekday")));
    }
}

xmlNodePtr Interval::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    return me;
}

FWReference* Interval::createRef()
{
//    FWIntervalReference *ref=new FWIntervalReference();
    FWIntervalReference *ref = getRoot()->createFWIntervalReference();
    ref->setPointer(this);
    return ref;
}

bool Interval::isAny() const
{ 
    return getId()==FWObjectDatabase::ANY_INTERVAL_ID;
}


/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: Interval.cpp 975 2006-09-10 22:40:37Z vkurland $


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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Interval.h>
#include <fwbuilder/FWIntervalReference.h>
#include <fwbuilder/XMLTools.h>
#include <fwbuilder/FWObjectDatabase.h>

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

}

Interval::Interval(const FWObject *root,bool prepopulate) : FWObject(root,prepopulate)
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

}

void Interval::setStartTime(int min,int hour,int day,int month,int year,int dayofweek)
{
    setInt("from_minute", min);
    setInt("from_hour", hour);
    setInt("from_day", day);
    setInt("from_month", month);
    setInt("from_year", year);
    setInt("from_weekday", dayofweek);
}

void Interval::setEndTime(int min,int hour,int day,int month,int year,int dayofweek)
{
    setInt("to_minute", min);
    setInt("to_hour", hour);
    setInt("to_day", day);
    setInt("to_month", month);
    setInt("to_year", year);
    setInt("to_weekday", dayofweek);
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

void Interval::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char *n;

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("from_minute")));
    if (n!=NULL)
    {
        setStr("from_minute", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("from_hour")));
    if (n!=NULL)
    {
        setStr("from_hour", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("from_day")));
    if (n!=NULL)
    {
        setStr("from_day", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("from_month")));
    if (n!=NULL)
    {
        setStr("from_month", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("from_year")));
    if (n!=NULL)
    {
        setStr("from_year", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("from_weekday")));
    if (n!=NULL)
    {
        setStr("from_weekday", n);
        FREEXMLBUFF(n);
    }


    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("to_minute")));
    if (n!=NULL)
    {
        setStr("to_minute", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("to_hour")));
    if (n!=NULL)
    {
        setStr("to_hour", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("to_day")));
    if (n!=NULL)
    {
        setStr("to_day", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("to_month")));
    if (n!=NULL)
    {
        setStr("to_month", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("to_year")));
    if (n!=NULL)
    {
        setStr("to_year", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("to_weekday")));
    if (n!=NULL)
    {
        setStr("to_weekday", n);
        FREEXMLBUFF(n);
    }

}

FWReference* Interval::createRef()
{
//    FWIntervalReference *ref=new FWIntervalReference();
    FWIntervalReference *ref=FWIntervalReference::cast(getRoot()->create(FWIntervalReference::TYPENAME));
    ref->setPointer(this);
    return ref;
}

bool Interval::isAny()
{ 
    return getId()==FWObjectDatabase::getAnyIntervalId();
}


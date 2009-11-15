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

#ifndef FWOBJECTSTATE_H
#define FWOBJECTSTATE_H

#include "fwbuilder/FWObject.h"
#include <QString>
#include <QDate>
#include <QTime>

/********************************************************
 * FWObjectState
 ********************************************************/

class FWObjectState
{
public:
    QString name;
    QString comment;

    FWObjectState() {}

    virtual void save(libfwbuilder::FWObject* object)
    {
        name = QString::fromUtf8(object->getName().c_str());
        comment = QString::fromUtf8(object->getComment().c_str());
    }

    virtual void restore(libfwbuilder::FWObject* object)
    {
        object->setName(std::string(name.toUtf8().constData()));
        object->setComment(std::string(comment.toUtf8().constData()));
    }
};

class FWObjectStateTime : public FWObjectState
{
public:

    QDate startDate;
    QTime startTime;

    QDate endDate;
    QTime endTime;

    QString days_of_week;

    FWObjectStateTime() {}

    virtual void save(libfwbuilder::FWObject* object);
    virtual void restore(libfwbuilder::FWObject* object);

};

#endif // FWOBJECTSTATE_H

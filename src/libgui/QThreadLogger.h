/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef _QTHREAD_LOGGER_H_
#define _QTHREAD_LOGGER_H_

#include "fwbuilder/Logger.h"

#include <QWidget>


class QThreadLogger : public QObject, public libfwbuilder::Logger
{
    Q_OBJECT;
    
public:

    QThreadLogger();
        
    virtual Logger& operator<< (char c)            ;
    virtual Logger& operator<< (char  *str)        ;
    virtual Logger& operator<< (const char  *str)  ;
    virtual Logger& operator<< (const std::string &str) ;
    virtual Logger& operator<< (int    i  )        ;
    virtual Logger& operator<< (long   l  )        ;
    virtual Logger& operator<< (std::ostringstream &sstr);

signals:
    void lineReady(const QString &txt);
};


#endif

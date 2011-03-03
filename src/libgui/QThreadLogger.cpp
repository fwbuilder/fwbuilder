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

#include "QThreadLogger.h"

#include <QString>

#include <string>
#include <sstream>

using namespace std;
using namespace libfwbuilder;


QThreadLogger::QThreadLogger() : Logger() {}

Logger& QThreadLogger::operator<< (char c)
{
    emit lineReady(QString(c));
    return *this;
}

Logger& QThreadLogger::operator<< (char *str)
{
    emit lineReady(QString(str));
    return *this;
}

Logger& QThreadLogger::operator<< (const char *str)
{
    emit lineReady(QString(str));
    return *this;
}

Logger& QThreadLogger::operator<< (const std::string &str)
{
    emit lineReady(QString(str.c_str()));
    return *this;
}

Logger& QThreadLogger::operator<< (int i )
{
    QString s; s.setNum(i);
    emit lineReady(s);
    return *this;
}

Logger& QThreadLogger::operator<< (long l )
{
    QString s; s.setNum(l);
    emit lineReady(s);
    return *this;
}

Logger& QThreadLogger::operator<< (std::ostringstream &sstr)
{
    emit lineReady(QString(sstr.str().c_str()));
    return *this;
}


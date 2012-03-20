/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org
           Vadim Zaliva      lord@crocodile.org

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

#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"


#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <sstream>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#include "fwbuilder/BackgroundOp.h"

using namespace std;
using namespace libfwbuilder;

/*
Logger &libfwbuilder::start(Logger &l)
{
    l.line_lock.lock();
    return l;
}

Logger &libfwbuilder::end(Logger &l)
{
    l.line_lock.unlock();
    return l;
}
*/

void    Logger::blackhole()
{
    line_lock.lock();
    blackhole_mode = true;
    copy_to_stderr = false;
    line_lock.unlock();
}


Logger& NullLogger::operator<< (char )         { return *this;}
Logger& NullLogger::operator<< (char*)         { return *this;}
Logger& NullLogger::operator<< (const char*)   { return *this;}
Logger& NullLogger::operator<< (const string&) { return *this;}
Logger& NullLogger::operator<< (int)           { return *this;}
Logger& NullLogger::operator<< (long)          { return *this;}
Logger& NullLogger::operator<< (std::ostringstream&) { return *this;}



QueueLogger::QueueLogger() {}

Logger& QueueLogger::operator<< (char c)            
{ 
    if (blackhole_mode) return *this;
    if (copy_to_stderr) cerr << c;

    std::ostringstream str;
    str << c;
    *this << str;
    return *this;
}

Logger& QueueLogger::operator<< (char  *str)        
{ 
    if (blackhole_mode) return *this;
    if (copy_to_stderr) cerr << str;

    line_lock.lock();
    linequeue.push(str);
    line_lock.unlock();
    return *this;
}

Logger& QueueLogger::operator<< (const char  *str)        
{ 
    if (blackhole_mode) return *this;
    if (copy_to_stderr) cerr << str;

    line_lock.lock();
    linequeue.push(str);
    line_lock.unlock();
    return *this;
}

Logger& QueueLogger::operator<< (const string &str) 
{ 
    if (blackhole_mode) return *this;
    if (copy_to_stderr) cerr << str;

    line_lock.lock();
    linequeue.push(str);
    line_lock.unlock();
    return *this;
}

Logger& QueueLogger::operator<< (int    i  )        
{ 
    if (blackhole_mode) return *this;
    if (copy_to_stderr) cerr << i;

    std::ostringstream str;
    str << i;
    *this << str;
    return *this;
}

Logger& QueueLogger::operator<< (long   l  )        
{ 
    if (blackhole_mode) return *this;
    if (copy_to_stderr) cerr << l;

    std::ostringstream str;
    str << l;
    *this << str;
    return *this;
}

Logger& QueueLogger::operator<< (std::ostringstream &sstr)
{
    if (blackhole_mode) return *this;
    if (copy_to_stderr) cerr << sstr.str();

    line_lock.lock();
    linequeue.push(sstr.str());
    line_lock.unlock();
    sstr.str("");    // purge stream contents

    return *this;
}

bool   QueueLogger::ready()
{
    if (blackhole_mode) return false;

    bool res=false;
    line_lock.lock();
    res=(!linequeue.empty());
    line_lock.unlock();
    return res;
}

string QueueLogger::getLine()
{
    if (blackhole_mode) return "";

    string str;
    line_lock.lock();
    if(!linequeue.empty()) {
	str=linequeue.front();
	linequeue.pop();
    }
    line_lock.unlock();
    return str;
}


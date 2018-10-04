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


#ifndef __LOGGER_HH_FLAG__
#define __LOGGER_HH_FLAG__

#include <sstream>
#include <queue>
#include "fwbuilder/ThreadTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Tools.h"

namespace libfwbuilder
{

class Logger
{
    protected:

    Mutex line_lock;
    bool  blackhole_mode;
    bool  copy_to_stderr;

    public:

    Logger() { blackhole_mode=false; copy_to_stderr=false; }
    virtual ~Logger() {};

    virtual Logger& operator<< (char c)            = 0;
    virtual Logger& operator<< (char  *str)        = 0;
    virtual Logger& operator<< (const char  *str)  = 0;
    virtual Logger& operator<< (const std::string &str) = 0;
    virtual Logger& operator<< (int    i  )        = 0;
    virtual Logger& operator<< (long   l  )        = 0;
    virtual Logger& operator<< (std::ostringstream &sstr)        = 0;

    // Manipulator
    virtual Logger& operator<< (Logger&(*f)(Logger &l)) 
    {
        return f(*this);
    }

    virtual bool   ready()        { return true; }
    virtual std::string getLine() { return std::string(""); }
    void blackhole();
    void copyToStderr() { copy_to_stderr=true; }
};

class NullLogger:public Logger
{
    public:

    virtual Logger& operator<< (char c)            ;
    virtual Logger& operator<< (char  *str)        ;
    virtual Logger& operator<< (const char  *str)  ;
    virtual Logger& operator<< (const std::string &str) ;
    virtual Logger& operator<< (int    i  )        ;
    virtual Logger& operator<< (long   l  )        ;
    virtual Logger& operator<< (std::ostringstream &sstr);
};

class QueueLogger:public Logger
{
    private:

    std::queue<std::string> linequeue;

    public:
    
    QueueLogger();
    
    virtual Logger& operator<< (char c)            ;
    virtual Logger& operator<< (char  *str)        ;
    virtual Logger& operator<< (const char  *str)  ;
    virtual Logger& operator<< (const std::string &str) ;
    virtual Logger& operator<< (int    i  )        ;
    virtual Logger& operator<< (long   l  )        ;
    virtual Logger& operator<< (std::ostringstream &sstr);

    virtual bool        ready();
    virtual std::string getLine();
};

}
#endif

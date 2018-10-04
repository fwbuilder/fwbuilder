/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

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



#include "fwbuilder/ThreadTools.h"

#ifdef USE_PTHREADS

#include <time.h>
#include <sys/types.h>
#ifndef _WIN32
#  include <unistd.h>
#  include <sys/poll.h>
#else
#  include <windows.h>
#endif

# ifndef DST_NONE
#  define DST_NONE 0
# endif

using namespace std;
using namespace libfwbuilder;

Mutex::Mutex()
{
    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init( &mutexattr);
//    pthread_mutexattr_settype( &mutexattr, PTHREAD_MUTEX_FAST_NP );
    pthread_mutex_init(&mutex, &mutexattr);
}

Mutex::~Mutex()
{
}

void Mutex::lock() const
{
    pthread_mutex_lock( (pthread_mutex_t*)&mutex );
}

void Mutex::unlock() const
{
    pthread_mutex_unlock( (pthread_mutex_t*)&mutex );
}

Cond::Cond()
{
    pthread_cond_init( &cond, nullptr );
}

Cond::~Cond()
{
}

bool Cond::wait(UniqueLock &lock) const
{
    Mutex m = lock.get_mutex();
    pthread_cond_wait( (pthread_cond_t*)&cond, (pthread_mutex_t*)&m.mutex);
    return true;
}

void Cond::signal() const
{
    pthread_cond_signal( (pthread_cond_t*)&cond );
}

void Cond::broadcast() const
{
    pthread_cond_broadcast( (pthread_cond_t*)&cond );
}

void Cond::notify_one() const
{
    signal();
}

void Cond::notify_all() const
{
    broadcast();
}

SyncFlag::SyncFlag(bool v)
{
    value = v;
}

bool SyncFlag::peek() const
{
    return value;
}

bool SyncFlag::get() const
{
    bool v;
    lock();
    v = value; 
    unlock();
    return v;
}

void SyncFlag::modify(bool v)
{
    value = v;
}

void SyncFlag::set(bool v)
{
    lock();
    value = v;
    unlock();
}
    
SyncFlag::operator bool() const
{
    return get();
}
    
SyncFlag& SyncFlag::operator=(const SyncFlag &o)
{
    set(o.get());
    return *this;
}
    
SyncFlag& SyncFlag::operator=(bool v)
{
    set(v);
    return *this;
}


#ifndef _WIN32

TimeoutCounter::TimeoutCounter(unsigned int _timeout, const string &_name) :
    timeout(_timeout),name(_name)
{
    start();
}

void TimeoutCounter::start()
{
    time_t tres;
    finish  = time(&tres) + timeout ;
}

unsigned int TimeoutCounter::timeLeft() const
{
    time_t tres;
    int res = finish-time(&tres);
    return res<0?0:res;
}

bool TimeoutCounter::isExpired() const
{
    time_t tres;
    return time(&tres) > finish ;
}

void TimeoutCounter::check() const
{
    if(isExpired())
    {
        //cerr << "Expired Timeout Counter." << endl;
        throw FWException(name+" timeout");
    }
}

ssize_t TimeoutCounter::read(int fd, void *buf, size_t n) const
{
    struct pollfd ufds[1];

    ufds[0].fd=fd;
    ufds[0].events=POLLIN|POLLPRI;
    
    int retval=poll(ufds, 1, 1000*timeLeft());

    if(retval==0)
        throw FWException("Timeout "+name);
    else if(retval>0)
        return ::read(fd, buf, n);
    else 
        return -1; //error
}

#endif

#endif // USE_PTHREADS

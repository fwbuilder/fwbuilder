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

#ifndef __THREAD_TOOLS_H_FLAG__
#define __THREAD_TOOLS_H_FLAG__

#include "fwbuilder/libfwbuilder-config.h"

#include <time.h> //for time_t definition
#include <pthread.h>

#include <string>
#include <queue>

#include "fwbuilder/FWException.h"

namespace libfwbuilder
{

/**
 * POSIX Mutex wrapper class.
 */
class Mutex
{
    friend class Cond;

    protected:

    pthread_mutex_t mutex;

    public:

    Mutex();
    virtual ~Mutex();

    void lock() const;
    void unlock() const;

};

/**
 * POSIX Mutex wrapper class.
 */
class Cond
{
    protected:

    pthread_cond_t cond;

    public:

    Cond();
    virtual ~Cond();

    bool wait(const Mutex &mutex) const;
    void signal   () const;
    void broadcast() const;

};

/**
 * This class represents boolean value, access to which
 * is guarded by mutex.
 */
class SyncFlag: public Mutex
{
    private:

    bool value;

    public:

    SyncFlag(bool v=false);

    /**
     * Checks value without locking.
     * Use with lock/unlock()
     */
    bool peek() const;

    /**
     * Changes value without locking.
     * Use with lock/unlock()
     */
    void modify(bool v);

    bool get() const;
    void set(bool v);
    
    operator bool() const;
    SyncFlag& operator=(const SyncFlag &o);
    SyncFlag& operator=(bool v);
};

#ifndef _WIN32
/**
 * Timeout counter. This class needs poll(2) which does not exist on Windows
 */
class TimeoutCounter
{
    private:

    unsigned int timeout ; 
    std::string name     ;
    time_t finish        ;

    public:
    
    /**
     * Creates counter with start time now
     * and end time now+timeout. String is counter
     * name which will be used in thrown exception
     * in check() method.
     * NB: timeout is in whole seconds.
     */
    TimeoutCounter(unsigned int timeout, const std::string &name);

    /** 
     * Starts counter with start time 'now'
     * and end time now+timeout.
     * Could be called several times to restart it.
     */
    void start();
    
    /**
     * retunrs time left before
     * timeout expiration (in seconds)
     */
    unsigned int timeLeft() const;

    /**
     * Returns 'true' if timeout is expired.
     */
    bool isExpired() const;

    /**
     * Throw exception if timeout is expired
     */
    void check() const throw(FWException) ;

    /**
     * Reads from socket/file.
     * Throws FWException if timeout occured.
     */
    ssize_t read(int fd, void *buf, size_t n) const throw(FWException);
    
};
#endif




}

#endif //__THREAD_TOOLS_H_FLAG__



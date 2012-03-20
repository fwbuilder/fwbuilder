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

#ifndef __SYNC_QUEUE_H_FLAG__
#define __SYNC_QUEUE_H_FLAG__

#include "fwbuilder/ThreadTools.h"

namespace libfwbuilder
{

/**
 * This is exception thrown by SyncQueue.
 *
 * @see SyncQueue
 */
class SyncQueueDoneException: public FWException
{
    public:

    SyncQueueDoneException(const std::string &reason):FWException(reason) {}
};

/**
 * SyncQueue - queue with access synchronized by mutex.
 *
 * @see SyncQueueDoneException
 */
template <class _Tp>  class SyncQueue: protected queue<_Tp>
{
    protected:

    Mutex mutex ;
    Cond  cond  ;
    bool  down  ;
    
    public:

    /**
     * Creates empty queue.
     */
    SyncQueue() : queue<_Tp>() { down = false; }
    
    /**
     * Returns true if queue is currently empty
     *
     * @exception SyncQueueDoneException if queue have been already shut down
     */
    bool empty() const throw (SyncQueueDoneException)
    { 
        mutex.lock();
        if(down)
        {
            mutex.ulock();
            throw SyncQueueDoneException("This queue already have been shut down.");
        }
        bool res=queue<_Tp>::empty(); 
        mutex.unlock();
        return res;
    }

    /**
     * Returns current queue size
     *
     * @exception SyncQueueDoneException if queue have been already shut down
     */
    size_type size() const throw (SyncQueueDoneException)
    { 
        mutex.lock();
        if(down)
        {
            mutex.ulock();
            throw SyncQueueDoneException("This queue already have been shut down.");
        }
        size_type res=queue<_Tp>::size(); 
        mutex.unlock();
        return res;
    }
    
    /**
     * Adds element to end of queue
     *
     * @exception SyncQueueDoneException if queue have been already shut down
     */
    void push(const queue<_Tp>::value_type& __x) throw (SyncQueueDoneException)
    { 
        mutex.lock();
        if(down)
        {
            mutex.ulock();
            throw SyncQueueDoneException("This queue already have been shut down.");
        }
        queue<_Tp>::push_back(__x); 
        cond.signal();
        mutex.unlock();
    }

    /**
     * Pops element from front of queue. It queue is empty, blocks
     * according to timeout_ms parameter.
     * 
     * @param timeout_ms timeout in milliseconds. -1 means wait forewer.
     * @exception FWException if timeout occured
     * @exception SyncQueueDoneException if shutdown() was called 
     */
    const queue<_Tp>::value_type pop(long timeout_ms=-1) throw(SyncQueueDoneException, FWException)
    { 
        mutex.lock();
        if(down)
        {
            mutex.ulock();
            throw SyncQueueDoneException("This queue already have been shut down.");
        }

        while(true)
        {
            if(!queue<_Tp>::empty())
                break;

            bool wait_status=cond.wait(mutex, timeout_ms);

            if(down)
            {
                mutex.ulock();
                throw SyncQueueDoneException("The queue have been shut down.");
            }
            
            if(!wait_status)
            {
                //Timeout occured
                mutex.unlock();
                throw FWException("SyncQueue timeout in pop()");
            }

        }
        queue<_Tp>::value_type res = queue<_Tp>::front(); 
        queue<_Tp>::pop();
        mutex.unlock();
    }

    /**
     * Shut down this queue. All waiting threads will be released
     * throwing SyncQueueDoneException exception.
     */
    void shutdown()
    {
        mutex.lock();
        down = true;
        cond.signal();
        mutex.unlock();
    }
};

}

#endif // __SYNC_QUEUE_H_FLAG__


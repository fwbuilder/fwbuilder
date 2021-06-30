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

#ifndef __POOL_HH_FLAG__
#define __POOL_HH_FLAG__

#include <vector>
#include <set>
#include "fwbuilder/ThreadTools.h"

namespace libfwbuilder
{

/**
 * General purpose Pool. 
 * Object can be 'leased' from pool,
 * and 'released' back to pool.
 * All access is synchronized.
 */
template<class T> class Pool
{

    typedef std::vector<T *>   __vtp;
    typedef std::set<T *>      __stp;
    typedef typename __vtp::iterator    __vtp_i;
    typedef typename __stp::iterator    __stp_i;

    private:
    
    size_t max;

    Mutex m_mutex;
    Cond m_cond;
    
    std::vector<T *> freepool;
    std::set   <T *> usedpool;
    
    public:

    /**
     * Creates Pool with initial size and max size.
     * 'initial' objects are created immediately. 
     * Later Pool can automatically grow up to 'max_'.
     */
    Pool(size_t initial, size_t max_)
    {
        max = max_;
        for(size_t i=0;i<initial;i++)
            freepool.push_back(create());
    }
    
    virtual ~Pool()
    {
        for(__vtp_i i=freepool.begin(); i!=freepool.end(); ++i) 
            delete *i;
        for(__stp_i i=usedpool.begin(); i!=usedpool.end(); ++i) 
            delete *i;
//        g_mutex_free(mutex);
//        g_cond_free(cond);
    }
    
    /**
     * Lease object from pool. After you are done with it
     * 'release' it back. If no instance availiable this
     * method will create new one. If pool is reached its
     * maximum capacity this method will block until some 
     * instance is is released.
     */
    virtual T *lease()
    {
        UniqueLock lock(m_mutex);

        while(true)
        {
            if(freepool.size()==0)
            {
                if(usedpool.size() < max)
                {
                    // create new instance 
                    T* res = create();
                    usedpool.insert(res);
                    return res;
                } else
                {
                    // wait for instance to be released
                    m_cond.wait(lock);
                }
            } else
            {
                //get one from free pool
		__vtp_i m=freepool.begin();
                usedpool.insert(*m);
		freepool.erase(m);

                return (*m);
            }
        }
    }
    
    /**
     * Releases instance back to pool.
     */
    virtual void release(T *t)
    {
        LockGuard lock(m_mutex);

        usedpool.erase(t);
        freepool.push_back(t);
        m_cond.notify_one();
    }
    
    protected:

    /**
     * creates new instance. This method is called by pool
     * when it decides to create new instance. It could be overloaded
     * to provide custom object creation code.
     */
    virtual T *create()
    {
        // default implementation using empty constructor
        return new T();
    }
};

}

#endif //  _POOL_HH_FLAG_

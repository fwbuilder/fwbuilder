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


#ifndef __BACKGROUNDOP_HH_FLAG__
#define __BACKGROUNDOP_HH_FLAG__

#include "ThreadTools.h"

#include <vector>
#include <iostream>
#include <atomic>

#include "fwbuilder/FWException.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/Pool.h"
#include "fwbuilder/Logger.h"

// #include <sigc++/signal_system.h>

namespace libfwbuilder
{

    void *background_thread(void *);

/**
 *  Abstract class BackgroundOp represents operatioin executed in background
 */
class BackgroundOp 
{
    friend void *background_thread(void *);

    private: 

    std::atomic<bool> running;
    std::atomic<bool> connected;

#ifdef USE_PTHREADS
    pthread_attr_t tattr;
#endif

    protected:

    std::atomic<bool> *stop_program;
    FWException *error;
    std::atomic<bool> *iamdead;

    /**
     * Implementation of the actual operation. Use logger to send
     * output text to the GUI. The SyncFlag is a mutex flag used to
     * interrupt background operation. We can't keep this flag as a
     * member of the class BackgroundOp because object of this class
     * gets destroyed before actual background operation has finished
     * (especially if it got stuck in a system call). To avoid having
     * to use object of the class BackgroundOp and any of its members
     * or methods, we create this flag as a dynamic variable and pass
     * pointer to run_impl, which should destroy it when it finishes.
     */
    virtual void  run_impl(Logger *, std::atomic<bool> *) = 0;

    /**
     * sets flag "running"
     */
    void     setRunning();

    /**
     * clears flag "running"
     */
    void     clearRunning();

    /**
     * checks "stop" flag and terminates thread if it is set. Used
     * from inside run_impl to check if background operation should be
     * immediately interrupted
     */
    #define CHECK_STOP_AND_RETURN { \
      if ( stop_program->load() ){ return; }\
    }
    
    #define CHECK_STOP_AND_THROW_EXCEPTION { \
      if ( stop_program->load() ){ throw FWException("Interrupted"); }\
    }
    
    public:
    
    BackgroundOp();
    virtual ~BackgroundOp();

    /**
     * checks whether background operation is connected to GUI
     */
    bool isConnected();

    /**
     * disconnects background operation from the GUI
     */
    void disconnect();

    /**
     * Initiates background operation
     */
    virtual Logger* start_operation();

    /**
     * Stops background operation
     */
    virtual void stop_operation();

    /**
     * returns flag "running"
     */
    bool isRunning();

    /**
     * returns last error from the background operation. The meaning of
     * the error is determined by the operation
     */
    FWException *get_latest_error() { return error; }

};

}

#endif // __BACKGROUNDOP_HH_FLAG__

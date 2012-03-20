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

#include <vector>
#include <iostream>
#ifdef __MINGW32__ //win32 pthread ditrib doesn't have config.h
# ifdef HAVE_CONFIG_H
#  undef HAVE_CONFIG_H
#  include <pthread.h>
#  define HAVE_CONFIG_H
# endif
#else
# include <pthread.h>
#endif

#include "fwbuilder/FWException.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/ThreadTools.h"
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

    SyncFlag     running      ;
    SyncFlag     connected    ;

    pthread_attr_t tattr;

    protected:

    SyncFlag    *stop_program ;

    FWException        *error      ;
    SyncFlag           *iamdead    ;

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
    virtual void  run_impl(Logger *,SyncFlag *) throw(FWException) = 0;

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
    #define CHECK_STOP_AND_RETURN { stop_program->lock();\
      if ( stop_program->peek() ){ stop_program->unlock(); return; }\
      stop_program->unlock(); }
    
    #define CHECK_STOP_AND_THROW_EXCEPTION { stop_program->lock();\
      if ( stop_program->peek() ){ stop_program->unlock(); throw FWException("Interrupted"); }\
      stop_program->unlock(); }
    
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
    virtual Logger* start_operation()  throw(FWException);

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

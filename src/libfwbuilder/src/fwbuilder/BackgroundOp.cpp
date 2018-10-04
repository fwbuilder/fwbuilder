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



#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#ifndef USE_PTHREADS
    #include <thread>
#endif

// #include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#include "fwbuilder/BackgroundOp.h"

using namespace std;
using namespace libfwbuilder;


BackgroundOp::BackgroundOp():running(false),connected(true)
{
    error        = nullptr;
    stop_program = new std::atomic<bool>(false);
    iamdead      = new std::atomic<bool>(false);
#ifdef USE_PTHREADS
    pthread_attr_init(&tattr);
#endif
}

/*
 * we destroy logger in the background thread because run_impl may use
 * logger and we should be able to destroy object of this class even
 * while background thread is working. 
 *
 * iamdead should not be deleted either because it is used as a flag
 * so that background thread would know if this object has been deleted
 * while run_impl was running
 */
BackgroundOp::~BackgroundOp()
{
    stop_program->store(true);
    iamdead->store(true);

#ifdef USE_PTHREADS
    pthread_attr_destroy(&tattr);
#endif
}

/* replaced by a macro
void BackgroundOp::check_stop()
{
    stop_program->lock();
    if ( stop_program->peek() )
    {
        stop_program->unlock();
        pthread_exit(nullptr);
    }
    stop_program->unlock();
}
*/

bool BackgroundOp::isRunning()     { return running.load();  }
void BackgroundOp::setRunning()    { running.store(true);    }
void BackgroundOp::clearRunning()  { running.store(false);   }

bool BackgroundOp::isConnected()   { return connected.load(); }
void BackgroundOp::disconnect()    { connected.store(false);  }



Logger* BackgroundOp::start_operation()
{
    /*
     * Suppose calling program wants to stop background operation. It
     * calls stop_oprtation and some times immediately destroys
     * BackgroundOp object. If background op. was stuck in a system
     * call when this happened, it will check stop_program flag only
     * after it returns from the system call. Since BackgroundOp
     * object has been destroyed by then, the program crashes. To
     * avoid this, we create stop_program flag here so that even
     * BackgroundOp object is destoryed, this flag is still available
     * and can be properly checked.
     */
    stop_program->store(false);
    running.store(true);

    Logger *logger  = new QueueLogger();

    void **void_pair = new void*[4];
    void_pair[0] = this;
    void_pair[1] = logger;
    void_pair[2] = iamdead;
    void_pair[3] = stop_program;

#ifdef USE_PTHREADS
        pthread_t tid;
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
        int err = pthread_create(&tid, &tattr, background_thread, void_pair);
        switch (err)
        {
        case EAGAIN:
            throw FWException("Not enough system resources to create new thread");
        case EINVAL:
            throw FWException("The value specified by attr is invalid.");
        }
#else
    try {
        thread opThread(background_thread, void_pair);
        opThread.detach();
    } catch(const std::system_error& e) {
        ostringstream oss;
        oss << "Error creating thread. "
            << "Caught system_error with code " << e.code()
            << " meaning " << e.what() << '\n';
        throw FWException(oss.str());
    }
#endif

    return logger;
}

void BackgroundOp::stop_operation()
{
    error = new FWException("Interrupted by user");
    stop_program->store(true);
}

namespace libfwbuilder
{
void *background_thread(void *args)
{
    void **void_pair = static_cast<void**>(args);

    BackgroundOp *bop          = static_cast<BackgroundOp*>(void_pair[0]);
    Logger       *logger       = static_cast<Logger *>(void_pair[1]);
    std::atomic<bool>     *isdead       = static_cast<std::atomic<bool>*>(void_pair[2]);
    std::atomic<bool>     *stop_program = static_cast<std::atomic<bool>*>(void_pair[3]);

    try 
    {
	bop->run_impl(logger,stop_program);
    } catch (FWException &ex) 
    {
        if (isdead->load())
        { 
            delete logger;
            delete isdead; 
            delete void_pair;
            return(nullptr); 
        }
        *logger << "Exception: " << ex.toString().c_str() << '\n';
        bop->error=new FWException(ex);
    }

    *logger << "Background process has finished\n";

    if (isdead->load())
    {
        delete logger;
        delete isdead; 
        delete void_pair;
        return(nullptr); 
    }

/* operation completed - clear "running" flag */
    bop->clearRunning();

/* wait till the other thread reads all the lines from logger. If
 * widget that was reading lines from this logger has been destroyed,
 * it should have called BackhroundOp::disconnect to release BackgroundOp
 * object
 */

    while (true) {
        if (isdead->load() ||  !bop->isConnected())
        {
            break;
        }

        libfwbuilder::cxx_sleep(1);
    }

    delete logger;
    delete void_pair;

    return(nullptr);
}

}






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
#include <pthread.h>

// #include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#include "fwbuilder/BackgroundOp.h"

using namespace std;
using namespace libfwbuilder;


BackgroundOp::BackgroundOp():running(false),connected(true)
{
    error        = NULL;
    stop_program = new SyncFlag(false);
    iamdead      = new SyncFlag(false);
    pthread_attr_init(&tattr);
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
    stop_program->lock();
    stop_program->modify(true);
    stop_program->unlock();

    iamdead->lock();
    iamdead->modify(true);
    iamdead->unlock();

    pthread_attr_destroy(&tattr);
}

/* replaced by a macro
void BackgroundOp::check_stop()
{
    stop_program->lock();
    if ( stop_program->peek() )
    {
        stop_program->unlock();
        pthread_exit(NULL);
    }
    stop_program->unlock();
}
*/

bool BackgroundOp::isRunning()     { return running;  }
void BackgroundOp::setRunning()    { running=true;    }
void BackgroundOp::clearRunning()  { running=false;   }

bool BackgroundOp::isConnected()   { return connected; }
void BackgroundOp::disconnect()    { connected=false;  }



Logger* BackgroundOp::start_operation() throw(FWException)
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
    stop_program->lock();
    stop_program->modify(false);
    stop_program->unlock();

    running = true;

    Logger *logger  = new QueueLogger();

    void **void_pair = new void*[4];
    void_pair[0] = this;
    void_pair[1] = logger;
    void_pair[2] = iamdead;
    void_pair[3] = stop_program;

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
    return logger;
}

void BackgroundOp::stop_operation()
{
    error = new FWException("Interrupted by user");
    stop_program->lock();
    stop_program->modify(true);
    stop_program->unlock();
}

namespace libfwbuilder
{
void *background_thread(void *args)
{
    void **void_pair=(void**)args;

    BackgroundOp *bop          = (BackgroundOp*)void_pair[0];
    Logger       *logger       = (Logger *)     void_pair[1];
    SyncFlag     *isdead       = (SyncFlag*)    void_pair[2];
    SyncFlag     *stop_program = (SyncFlag*)    void_pair[3];

    try 
    {
	bop->run_impl(logger,stop_program);
    } catch (FWException &ex) 
    {
        isdead->lock();
        if (isdead->peek())
        { 
            isdead->unlock(); 
            delete logger;
            delete isdead; 
            delete void_pair;
            return(NULL); 
        }
        *logger << "Exception: " << ex.toString().c_str() << '\n';
        bop->error=new FWException(ex);
        isdead->unlock();
    }

    *logger << "Background process has finished\n";

    isdead->lock();
    if (isdead->peek())
    { 
        isdead->unlock(); 
        delete logger;
        delete isdead; 
        delete void_pair;
        return(NULL); 
    }

/* operation completed - clear "running" flag */
    bop->clearRunning();

    isdead->unlock();

/* wait till the other thread reads all the lines from logger. If
 * widget that was reading lines from this logger has been destroyed,
 * it should have called BackhroundOp::disconnect to release BackgroundOp
 * object
 */

    while (true) {
        
        isdead->lock();
        if (isdead->peek() ||  !bop->isConnected())
        {
            isdead->unlock();
            break;
        }
        isdead->unlock();

        libfwbuilder::cxx_sleep(1);
    }

    delete logger;
    delete void_pair;

    return(NULL);
}

}






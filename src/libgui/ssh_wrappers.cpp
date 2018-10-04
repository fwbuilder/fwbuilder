/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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



#ifndef _WIN32

#include <QString>
#include <QStringList>
#include <QtDebug>

#include "FWBSettings.h"

#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <sys/select.h>
#include <signal.h>

#if defined(__linux__)
#include <pty.h>
#define HAVE_FORKPTY
#elif defined(__FreeBSD__)
#include <libutil.h>
#define HAVE_FORKPTY
#elif defined(__unix__) || defined(__APPLE__) && defined(__MACH__)
#include <util.h>
#define HAVE_FORKPTY
#endif

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

extern FWBSettings *st;
extern int fwbdebug;

static struct termios save_termios;
static int ttysavefd = -1;
static pid_t pid = 0;


#ifdef _WIN32
static inline void cfmakeraw(struct termios *termios_p)
{
    termios_p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    termios_p->c_oflag &= ~OPOST;
    termios_p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    termios_p->c_cflag &= ~(CSIZE|PARENB);
    termios_p->c_cflag |= CS8;
}
#endif

#ifndef HAVE_FORKPTY

#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
//#include <sys/stream.h>
#include <sys/stropts.h>

/* fork_pty() remplacement for Solaris.
* ignore the last two arguments
* for the moment
*/
int forkpty (int *amaster, char *name, void *unused1, void *unused2)
{
    int master, slave;
    char *slave_name;
    pid_t pid;

    master = open("/dev/ptmx", O_RDWR);
    if (master < 0)
        return -1;

    if (grantpt (master) < 0)
    {
        close (master);
        return -1;
    }

    if (unlockpt (master) < 0)
    {
        close (master);
        return -1;
    }

    slave_name = ptsname (master);
    if (slave_name == nullptr)
    {
        close (master);
        return -1;
    }

    slave = open (slave_name, O_RDWR);
    if (slave < 0)
    {
        close (master);
        return -1;
    }

    if (ioctl (slave, I_PUSH, "ptem") < 0
        || ioctl (slave, I_PUSH, "ldterm") < 0)
    {
        close (slave);
        close (master);
        return -1;
    }

    if (amaster)
        *amaster = master;

    if (name)
        strcpy (name, slave_name);

    pid = fork ();
    switch (pid)
    {
    case -1: /* Error */
        return -1;
    case 0: /* Child */
        close (master);
        dup2 (slave, STDIN_FILENO);
        dup2 (slave, STDOUT_FILENO);
        dup2 (slave, STDERR_FILENO);
        return 0;
    default: /* Parent */
        close (slave);
        return pid;
    }

    return -1;
}

#endif

int tty_raw(int fd)
{
    struct termios buf;

    if (tcgetattr(fd, &save_termios) < 0)
    {
        qDebug("Can not switch terminal to raw mode, tcgetattr error '%s'",strerror(errno));
        exit(1);
    }

    buf = save_termios;

    cfmakeraw(&buf);

// this used to use TCSAFLUSH, but that caused stall which I did not
// completely understand. Apparently there was some data in the output
// buffer at the moment when we try to switch tty to raw mode, but I
// could not figure out where this data comes from and why it could
// not be written to the tty. Anyway, this caused semi-random stalls
// in the installer because whenever it called fwbuilder -X, the child
// process would block in this place and stall installer. I had to
// switch to TCSANOW to fix.

    if (tcsetattr(fd, TCSANOW, &buf) < 0)
    {
        qDebug("Can not switch terminal to raw mode, tcsetattr error '%s'",strerror(errno));
        exit(1);
    }

    ttysavefd = fd;
    return 0;
}

int echo_off(int fd)
{
    struct stat statbuf;
    if (fstat(fd,&statbuf)!=0) return 0;

    struct termios stermios;
    if (tcgetattr(fd, &stermios)<0)
    {
        qDebug("Can not turn terminal echo off, tcgetattr error '%s'",strerror(errno));
        exit(1);
    }

    stermios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
    stermios.c_oflag &= ~(ONLCR);

    if (tcsetattr(fd, TCSANOW, &stermios)<0)
    {
        qDebug("Can not turn terminal echo off, tcsetattr error '%s'",strerror(errno));
        exit(1);
    }

    return 0;
}

ssize_t writen(int fd,const void *vptr, size_t n)
{
    size_t  nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = (const char*)(vptr);
    nleft = n;
    if (fwbdebug) qDebug("need to write %d bytes",int(nleft));
    while (nleft > 0)
    {
        if ( (nwritten = write(fd,ptr,nleft )) <= 0)
            return nwritten;

        if (fwbdebug) qDebug("%d bytes written",int(nwritten));

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

void catch_sign(int sig)
{
    if (fwbdebug) 
    {
        cerr << "Wrapper caight signal " << sig << endl;
        cerr << "Child process pid " << pid << endl;
    }

    if (pid != 0)
    {
        int stat;
        kill(pid, SIGTERM);
        int timeout = 0;
        pid_t cp = 0;
        while ( (cp = waitpid(pid, &stat, WNOHANG)) == 0 && timeout < 10)
        {
            if (fwbdebug) 
                cerr << "Waiting for pid " << pid << " to finish" << endl;
            sleep(1);
            timeout++;
        }
        if (cp == 0)
        {
            if (fwbdebug) 
                cerr << "Timeout, child process is still running. Killing it." << endl;
            kill(pid, SIGKILL);
        }
    }
    exit(1);
}

void ssh_wrapper( int argc, char *argv[] )
{
    bool ssh_wrapper = false;
    bool scp_wrapper = false;
    const char *arg[128];
    int i;

    QStringList new_args;

    for (i = 1 ; i<argc; i++)
    {
        if (strncmp(argv[i], "-X", 2)==0)
        {
            ssh_wrapper=true; continue;
        } else
        {
            if (strncmp(argv[i], "-Y", 2)==0)
            {
                scp_wrapper=true; continue;
            } else
            {
                if (strncmp(argv[i], "-d", 2)==0)
                {
                    fwbdebug++; continue;
                } else
                    new_args.push_back(argv[i]);
            }
        }
    }

    if (ssh_wrapper || scp_wrapper)
    {

        for (i = 0; i < new_args.size() && i < 127; ++i)
            arg[i] = strdup(new_args[i].toLatin1().constData());

        arg[i] = nullptr;

        if (fwbdebug)
        {
            qDebug("cmd: %s", arg[0]);
            qDebug("Arguments:");
            for (const char **cptr = arg; *cptr!=nullptr; cptr++)
            {
                qDebug("    %s", *cptr);
            }
        }

/* forks ssh with a pty and proxies its communication on stdin/stdout
 * to avoid having to deal with pty. This is only needed on Unix.
 */
        int   mfd;
        char  slave_name[64];
//        char  *pgm;

        pid = forkpty(&mfd,slave_name,nullptr,nullptr);
        if (pid<0)
        {
            qDebug("Fork failed: %s", strerror(errno));
            exit(1);
        }
        if (pid==0)
        { // child

// turn echo off on stdin
            echo_off(STDIN_FILENO);

            tty_raw(STDIN_FILENO);

            signal(1,SIG_IGN);

            execvp(arg[0],(char* const*)arg);

// if we've got here there was an error
            qDebug("Exec error: %s %s",strerror(errno),arg[0]);
            exit(1);
        }

        signal(SIGHUP, catch_sign);
        signal(SIGINT, catch_sign);
        signal(SIGTERM, catch_sign);
        signal(SIGQUIT, catch_sign);

        tty_raw(mfd);

        fd_set  rfds;
        struct timeval tv;
        int     retval;

        #define BUFFSIZE 1024

#ifdef DEBUG_INSTALLER
        int debug_file = open("installer.dbg",O_CREAT|O_WRONLY);
#endif
        char    ibuf[BUFFSIZE];
        char    obuf[BUFFSIZE];
        bool    endOfStream = false;

        while (true)
        {
            tv.tv_sec  = 2;
            tv.tv_usec = 0;

            FD_ZERO(&rfds);
            FD_SET(mfd,  &rfds);
            if (!endOfStream)  FD_SET(STDIN_FILENO  , &rfds);

            retval = select( max(STDIN_FILENO,mfd)+1 , &rfds, nullptr, nullptr, &tv);
            if (retval==0)  // timeout
            {
                if (fwbdebug) qDebug("timeout");
                if (endOfStream)
                {
                    if (fwbdebug) qDebug("Closing mfd");
                    close(mfd);
                    break;
                }
            }
            if (retval)
            {
                if (FD_ISSET(STDIN_FILENO, &rfds))
                {
                    int n=read(0,ibuf,sizeof(ibuf));
                    if (fwbdebug) qDebug("Read %d bytes from stdin",n);
                    if (n<0)
                    {
                        if (fwbdebug) qDebug("Error on stdin");
                        break;
                    }
                    if (n==0)
                    {
// eof on stdin
                        if (fwbdebug) qDebug("EOF on stdin");
                        endOfStream = true;
                    } else
                    {
                        int r=writen(mfd,ibuf,n);
#ifdef DEBUG_INSTALLER
                        write(debug_file,ibuf,n);
#endif
                        if (fwbdebug) qDebug("Wrote %d bytes to mfd",r);
                    }
                }
                if (FD_ISSET(mfd, &rfds))
                {
                    int n;
                    obuf[0]='\0';
                    n=read(mfd,obuf,sizeof(obuf));
                    if (fwbdebug) qDebug("Read %d bytes from mfd",n);
                    if (n<=0)
                    {
/* eof on mfd - this means ssh process has died */
                        if (fwbdebug) qDebug("EOF on mfd");
                        break;
                    } else
                    {
                        obuf[n]='\0';
                        int r=writen(1,obuf,n);
                        if (fwbdebug) qDebug("Wrote %d bytes to stdout",r);
                    }
                }
            }
        }

#ifdef DEBUG_INSTALLER
        close(debug_file);
#endif
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) exit(WEXITSTATUS(status));
        exit(0);
    }
}

#else

void ssh_wrapper( int argc, char *argv[] )
{
	(void) argc;
	(void) argv;
}

#endif


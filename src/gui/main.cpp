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


#include "../../config.h"
#include "global.h"
#include "VERSION.h"
#include "../common/commoninit.h"

#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#else
#  ifdef _WIN32
#    include <getopt.h>
#  else
#    include <stdlib.h>
#  endif
#endif

#include <qapplication.h>
#include <qmainwindow.h>
#include <qtextcodec.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <qpixmapcache.h>
#include <QTextCodec>
#include <QLocale>
#include <QDir>
#include <QTranslator>
#include <QLibraryInfo>

/*
#ifdef _WIN32
#  include <windows.h>
#endif
*/

#include "FWBSettings.h"
#include "RCS.h"
#include "FWWindow.h"
#include "FWObjectClipboard.h"
#include "FWBTree.h"
#include "platforms.h"
#include "listOfLibraries.h"
#include "ObjectEditor.h"
#include "findDialog.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/dns.h"
//#include "fwbuilder/crypto.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/FWException.h"

#ifndef _WIN32
#  include <sys/wait.h>
#  include <unistd.h>
#  include <termios.h>
#  include <time.h>
#  include <sys/select.h>
#  include <utmp.h>
#else
#  include <direct.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_PTY_H
#include <pty.h>
#endif

#ifdef HAVE_LIBUTIL_H
#include <libutil.h>
#endif

#ifdef HAVE_UTIL_H
#include <util.h>
#endif

//#ifdef Q_OS_MACX
//#  include <util.h>
//#endif

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../common/init.cpp"

#ifdef ELC
extern bool init2(const std::string &a1,
                  const std::string &moduleName,
                  const std::string &rp,
                  const std::string &rp1,
                  bool f1, bool f2, bool d);
#endif

using namespace libfwbuilder;
using namespace std;

static QString    filename;
static QString    print_output_file_name;

QApplication      *app        = NULL;
FWWindow          *mw         = NULL;
FWBSettings       *st         = NULL;
int                fwbdebug   = 0;
bool               safemode   = false;
bool               registered = false;
bool               gui_experiment1 = false;
bool               cli_print        = false;
QString            cli_print_fwname = "";

#ifndef _WIN32

#ifndef HAVE_CFMAKERAW
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
    if (slave_name == NULL)
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

static struct termios save_termios;
static int            ttysavefd = -1;

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


#ifndef strndup
char* strndup(const char* s,int n)
{
    char *tbuf = (char*)malloc(n);
    if (tbuf) memcpy(tbuf,s,n);
    return tbuf;
}
#endif
#endif

void usage()
{
    cerr << "Usage: fwbuilder [-hv] [-p object_name] [-o file_name] [ [-f] filename]\n";
    cerr << endl;
}


int main( int argc, char *argv[] )
{


    bool        ssh_wrapper=false;
    const char *arg[64];
    int         i, j;

    filename="";
    print_output_file_name="";
    fwbdebug=0;
    safemode=false;

    if(fwbdebug)
        qDebug("main()");
/*
 * I am using njamd a lot, but gtkmm and probably some other libs
 * generate trap in their global static initialization code. Therefore
 * I need to start the program with env. var. NJAMD_PROT set to "none"
 * and then reset it to something useful here.
 */
#ifdef HAVE_SETENV
    setenv("NJAMD_PROT","strict",1);
#else
#  ifdef HAVE_PUTENV
    putenv("NJAMD_PROT=strict");
#  endif
#endif


#ifndef _WIN32

    i=1;
    j=1;

    //for ( ; argv[i]!=NULL; i++)
    for ( ; i<argc; i++)
    {
        if (strncmp(argv[i], "-X", 2)==0) { ssh_wrapper=true; continue; }
        else
            if (strncmp(argv[i], "-d", 2)==0) { fwbdebug++; continue; }
            else
                arg[j] = strdup(argv[i]);
        j++;
    }
    arg[j] = NULL;

    if (ssh_wrapper)
    {

/* need to create and initialize settings to be able to use ssh/scp
 * configuration in the wrapper
 *
 * Note:
 *
 * We need to keep installation data and program settings in registry
 * folders with different names. QSettings always looks into Current
 * User registry first, so if the folders have the same names, then we
 * store evaluation key in Current User, while it is better to put it
 * in the Local Machine branch.
 *
 * So, installation data goes to HKLM Software\NetCitadel\FirewallBuilder
 * and settings to HKCU Software\NetCitadel\FirewallBuilder2
 *
 * fwbuilder-lm determines folder path for the license file by
 * reading key Install_Dir under HKLM Software\NetCitadel\FirewallBuilder
 */
        st = new FWBSettings();

/* initialize preferences */
        st->init();

        QString sshcmd=st->getSSHPath();

        if (sshcmd.isEmpty()) sshcmd="ssh";

        arg[0]=strdup( sshcmd.toLatin1().constData() );

        if (fwbdebug)
        {
            qDebug("cmd: %s",arg[0]);
            qDebug("Arguments:");
            for (const char **cptr = arg; *cptr!=NULL; cptr++)
            {
                qDebug("    %s", *cptr);
                cptr++;
            }
        }

/* forks ssh with a pty and proxies its communication on stdin/stdout
 * to avoid having to deal with pty. This is only needed on Unix.
 */
        pid_t pid;
        int   mfd;
        char  slave_name[64];
//        char  *pgm;

        pid=forkpty(&mfd,slave_name,NULL,NULL);
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

        tty_raw(mfd);

        fd_set  rfds;
        struct timeval tv;
        int     retval;

        #define BUFFSIZE 512

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
            FD_SET(mfd,            &rfds);
            if (!endOfStream)  FD_SET(STDIN_FILENO  , &rfds);

            retval = select( max(STDIN_FILENO,mfd)+1 , &rfds, NULL, NULL, &tv);
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
#endif

    if (fwbdebug)
    {
        qDebug("Command line:  argc=%d", argc);
        for (int i=0; i<argc; ++i)
            qDebug(argv[i]);
    }

    // can not use "-p" for command line printing because
    // Mac OS X supplies switch "-psnXXXXX" when program is
    // started via Finder.

    int c;
    while ((c = getopt (argc , argv , "hvf:o:P:dxsg")) != EOF )
	switch (c) {
	case 'h':
	    usage();
	    exit(0);

	case 'f':
	    filename=optarg;
	    break;

	case 'o':
	    print_output_file_name=optarg;
	    break;

        case 'd':
            fwbdebug++;
            break;

	case 'v':
	    cout << VERSION << endl;
	    exit(0);

        case 's':
            safemode = true;
            break;

        case 'g':
            gui_experiment1 = true;
            break;
        case 'P':
            cli_print = true ;
            cli_print_fwname = optarg;
	}

    if ( (argc-1)==optind)
        filename = strdup( argv[optind++] );

    try
    {
        if (fwbdebug) qDebug("Initializing ...");

        if (fwbdebug) qDebug("Creating app ...");
        //QApplication::setDesktopSettingsAware(desktopaware);
        app = new QApplication( argc, argv );
        app->setOrganizationName(QLatin1String("NetCitadel LLC"));
        app->setApplicationName(QLatin1String("Firewall Builder"));


/* need to initialize in order to be able to use FWBSettings */
        init(argv);
        init_platforms();

#if 0
        if (fwbdebug) qDebug("Creating app ...");
        //QApplication::setDesktopSettingsAware(desktopaware);
        app = new QApplication( argc, argv );
        app->setOrganizationName(QLatin1String("NetCitadel LLC"));
        app->setApplicationName(QLatin1String("Firewall Builder"));
#endif

        Q_INIT_RESOURCE(MainRes);

        if (fwbdebug) qDebug("Reading settings ...");
        st = new FWBSettings();
        st->init();
        if (fwbdebug) qDebug("done");

        QPixmapCache::setCacheLimit(4096);

#ifdef ELC
        registered = init2(argv0,
                           "Firewall Builder",
                           "fwb_gui30",
                           "3.0",
                           true, true, fwbdebug);
#endif

        if (cli_print)
        {
            if (fwbdebug) qDebug("Print from command line");
            FWWindow::printFirewallFromFile(filename,
                                            cli_print_fwname,
                                            print_output_file_name);
            return 0;
        }

        string full_res_path = respath+FS_SEPARATOR+"resources.xml";

        if (fwbdebug)
        {
            qDebug("reading resources from '%s' ...",full_res_path.c_str());
        }

        //respath = RES_DIR;
        new Resources(full_res_path);
        if (fwbdebug) qDebug("done");

#if 0
        QApplication::setDesktopSettingsAware(desktopaware);
        app = new QApplication( argc, argv );
#endif

        vector<std::string> platforms=Resources::getListOfPlatforms();
        if (platforms.empty() || ( platforms.size()==1 && platforms.front()=="unknown" ))
        {
            qDebug("Failed to load list of supported platforms");
            exit(1);
        }

        if (fwbdebug) qDebug("creating widgets ...");

        new FWObjectDatabase();
        new FWObjectClipboard();

        if (fwbdebug) qDebug("loading translation for the current locale ...");

        QString local = QLocale::system().name();//"en_US";//
        QTranslator translator(0);
        translator.load(QLatin1String("fwbuilder_") +
                        QString(local), localepath.c_str());
        app->installTranslator (&translator);

        QString qt_resource_dir =
            QLibraryInfo::location(QLibraryInfo::TranslationsPath);


        QTranslator qt_translator(0);
        qt_translator.load(QLatin1String("qt_") + QLocale::system().name(),
                           qt_resource_dir);
        app->installTranslator (&qt_translator);


/* must build list of available libraries _after_ creation of
 * FWObjectDatabase and settings */

        if (fwbdebug) qDebug("loading libraries ...");

        mw  = new FWWindow();
        mw->setSafeMode(safemode);
        if (filename!="")
        {
            QDir file (filename);
            mw->setStartupFileName(file.canonicalPath ());
        }
        //QToolTip::setWakeUpDelay( st->getTooltipDelay()*1000 );

        mw->show();

        app->connect(app, SIGNAL( lastWindowClosed() ), app, SLOT( quit()));


        if (fwbdebug) qDebug("Checking for new version ...");

// setup single shot timer to call loadEverything()

        QTimer::singleShot( 0, mw, SLOT(startupLoad()) );

        app->exec();

        mw->hide();  // must do this before settings object is destroyed
        if (mw->getAddOnLibs()!=NULL)
            mw->getAddOnLibs()->save();  // ditto

        if ( st->getStartupAction()==1 )
        {
/* save the state of the GUI (opened firewall, opened object tree page, etc */
            FWObject *o=mw->getVisibleFirewalls();

            if (fwbdebug) qDebug("Main: closing. VisibleFirewall = %p",o);

            if (o) st->setStr("UI/visibleFirewall",
                              FWObjectDatabase::getStringId(
                                  o->getId()).c_str());

            o=mw->getOpened();
            if (o) st->setStr("UI/visibleObject",
                              FWObjectDatabase::getStringId(
                                  o->getId()).c_str());
        }

        st->save();
        delete st;
    }
    catch (FWException &ex)
    {
        qDebug("Exception: %s",ex.toString().c_str());
    }
}

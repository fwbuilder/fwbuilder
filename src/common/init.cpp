
#include "../../config.h"

#if defined(Q_OS_MACX) || defined(Q_OS_WIN32)
#  include <qsettings.h>
#  include <QDir>
#  include <QApplication>
#  include <QCoreApplication>
#  include <QString>
//#  include <CoreFoundation/CFURL.h>
//#  include <CoreFoundation/CFBundle.h>
#else
#  include <limits.h>
#  include <unistd.h>
#endif

#include <string>

#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/Resources.h"
#include "commoninit.h"

std::string      appRootDir;
std::string      userDataDir;
std::string      respath; 
std::string      localepath;
std::string      librespath;
std::string      sysfname;
std::string      tempfname;
std::string      argv0;
std::string      ee;

extern int             fwbdebug;

using namespace std;
using namespace libfwbuilder;


/*
 * We do all these different hacks on different OS in order to be able
 * to avoid dependency on QT on Linux and BSD, so people can
 * (theoretically) build and install compilers on the firewall machine
 * where they do not have X11 and QT. It may not be easy but should be
 * possible. (Unless I broke it in 3.0)
 *
 * Note findExecutable returns path to the directory where fwbuilder binary
 * is installed (on Mac it is  <wherever>/fwbuilder3.app/Contents/MacOS , 
 * On Windows it is c:\FWBuilder30, on Linux it is something like /usr/bin
 * or /usr/local/bin and so on.
 */
string findExecutable(const char *argv0)
{
#ifdef OS_LINUX
/*
 * on modern Linux systems full path to the executable is available in
 * /proc/self/exec.
 */
    char buf[PATH_MAX];
    if ( readlink( "/proc/self/exec", buf, sizeof(buf) )<0 )
    {
        // Can do better: use a macro defined in configure via PREFIX 
        return "";
    } else
    {
        // /proc/self/exec points at full path to the executable, including
        // name of the program. Remove the latter and return only path
        // to the directory.
        string exe_path(buf);
        string::size_type n0 = exe_path.find_last_of("/\\");
        if (n0!=string::npos) 
            return exe_path.substr(0,n0) + FS_SEPARATOR;
        else
            return exe_path;
    }
#else
    if (QCoreApplication::instance()==NULL)
    {
        int ac = 0;
        char **av = { NULL };
        new QApplication( ac, av );
    }
    QDir dir(QApplication::applicationDirPath());
    return string(dir.absolutePath().toAscii().constData());
#endif
//#endif
}



void init(char * const *argv)
{
    appRootDir = findExecutable(argv[0]);

    libfwbuilder::init();

#if defined(Q_OS_WIN32) || defined(Q_OS_MACX)
/* On windows and mac we install API resources (DTD etc) in the 
 * dir right above the one where we install resources for the GUI and compilers
 */
    if (respath=="") respath = appRootDir + FS_SEPARATOR + RES_DIR; 
    string::size_type n0 = respath.find_last_of("/\\");
    librespath = respath.substr(0, n0);

#else
/* On Unix RES_DIR and LIBFWBUILDER_TEMPLATE_DIR are absolute paths */

    if (respath=="") respath = RES_DIR;
    librespath = LIBFWBUILDER_TEMPLATE_DIR;

#endif

    sysfname = respath + FS_SEPARATOR  + "objects_init.xml";
    tempfname = respath+ FS_SEPARATOR + "templates.xml";

/* define localepath the same way as we define PKGLOCALEDIR in qmake.inc */
    localepath = respath + "/locale";

/* default directory where the user may want to save files */
#if defined(Q_OS_WIN32)
    userDataDir = string(getenv("USERPROFILE"))+"\\Documents";
#elif defined(Q_OS_MACX)
    userDataDir = string(getenv("HOME"))+"/Documents";
#else
    userDataDir = string(getenv("HOME"));
#endif

}

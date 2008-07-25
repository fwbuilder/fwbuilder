
#include "../../config.h"

#if defined(Q_OS_MACX) || defined(Q_OS_WIN32)
#  include <qsettings.h>
#  include <QDir>
#  include <QApplication>
//#  include <QCoreApplication>
#  include <QString>
#else
#  include <limits.h>
#  include <unistd.h>
#endif

#include <string>

#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/Resources.h"
#include "commoninit.h"

std::string appRootDir;
std::string userDataDir;
std::string respath; 
std::string localepath;
std::string librespath;
std::string sysfname;
std::string tempfname;
std::string argv0;
std::string ee;

extern int fwbdebug;

using namespace std;
using namespace libfwbuilder;



void init(char * const *argv)
{


/*
 * We do it in a different way on different OS in order to be able to
 * avoid dependency on QT on Linux and BSD, so people can
 * (theoretically) build and install compilers on the firewall machine
 * where they do not have X11 and QT. It may not be easy but should be
 * possible. (Unless I broke it in 3.0)
 *
 * Note appRootDir is the path to the directory where fwbuilder binary
 * is installed (on Mac it is <wherever>/fwbuilder3.app/Contents/MacOS
 * , On Windows it is c:\FWBuilder30, on Linux it is something like
 * /usr/bin or /usr/local/bin and so on.
 */
#if defined(Q_OS_WIN32) || defined(Q_OS_MACX)
    if (QCoreApplication::instance()==NULL)
    {
        int ac = 0;
        char **av = { NULL };
        new QApplication( ac, av );
    }
    QDir dir(QApplication::applicationDirPath());
    appRootDir = string(dir.absolutePath().toAscii().constData());

/* On windows and mac we install API resources (DTD etc) in the 
 * dir right above the one where we install resources for the GUI and compilers
 */
    if (respath=="") respath = appRootDir + FS_SEPARATOR + RES_DIR; 
    string::size_type n0 = respath.find_last_of("/\\");
    librespath = respath.substr(0, n0);

#else
    appRootDir = string(PREFIX) + FS_SEPARATOR + "bin";

/* On Unix RES_DIR and LIBFWBUILDER_TEMPLATE_DIR are absolute paths */

    if (respath=="") respath = RES_DIR;
    librespath = LIBFWBUILDER_TEMPLATE_DIR;
#endif


    libfwbuilder::init();


#if defined(Q_OS_WIN32)
    argv0 = appRootDir + FS_SEPARATOR + "fwbuilder.exe";
#else
/* need argv0 for built-in installer on unix and mac */
    argv0 = appRootDir + FS_SEPARATOR + "fwbuilder";
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

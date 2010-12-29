

#include "../../config.h"

#include <string>
#include <assert.h>

#include <qglobal.h>
#include <QString>
#include <QtDebug>

#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/Resources.h"
#include "commoninit.h"


// TODO: switch to QString 
std::string appRootDir;
std::string userDataDir;
std::string respath; 
std::string localepath;
std::string librespath;
std::string sysfname;
std::string tempfname;
std::string argv0;
std::string ee;

QString user_name;


#if defined(Q_OS_WIN32) || defined(Q_OS_MACX)
extern void init_win();
void init(char * const*)
{
    init_win();
}

#else

#include <unistd.h>
#include <pwd.h>

extern int fwbdebug;

using namespace std;
using namespace libfwbuilder;


void init(char * const*)
{
    appRootDir = string(PREFIX) + FS_SEPARATOR + "bin";

/* On Unix RES_DIR and LIBFWBUILDER_TEMPLATE_DIR are absolute paths */

    if (respath=="") respath = RES_DIR;
    librespath = RES_DIR;

    libfwbuilder::init();

/* need argv0 for built-in installer on unix and mac */
    argv0 = appRootDir + FS_SEPARATOR + "fwbuilder";

    sysfname = respath + FS_SEPARATOR  + "objects_init.xml";
    tempfname = respath+ FS_SEPARATOR + "templates.xml";

/* define localepath the same way as we define PKGLOCALEDIR in qmake.inc */
    localepath = respath + "/locale";

/* default directory where the user may want to save files */
    userDataDir = string(getenv("HOME"));

    char *lname = getenv("LOGNAME");
    if (lname!=NULL)
        user_name = QString(lname);
    else
    {
        struct passwd *pwd = getpwuid(getuid());
        assert(pwd);
        user_name = QString(pwd->pw_name);
    }
}

#endif

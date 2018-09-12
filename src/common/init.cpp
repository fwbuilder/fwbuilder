


#include <string>
#include <assert.h>

#include <qglobal.h>
#include <QString>
#include <QtDebug>

#include "fwbuilder/Tools.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Constants.h"

#include "commoninit.h"


// TODO: switch to QString 
std::string appRootDir;
std::string userDataDir;
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

    libfwbuilder::init();

/* need argv0 for built-in installer on unix and mac */
    argv0 = appRootDir + FS_SEPARATOR + "fwbuilder";

/* default directory where the user may want to save files */
    userDataDir = string(getenv("HOME"));

    char *lname = getenv("LOGNAME");
    if (lname!=nullptr)
        user_name = QString(lname);
    else
    {
        struct passwd *pwd = getpwuid(getuid());
        assert(pwd);
        user_name = QString(pwd->pw_name);
    }
}

#endif

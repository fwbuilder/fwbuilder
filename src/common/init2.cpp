

#include <qglobal.h>

#if defined(Q_OS_MACX) || defined(Q_OS_WIN32)
#  include <qsettings.h>
#  include <QDir>
#  include <QCoreApplication>
#  include <assert.h>
#endif

#include <QString>
#include <QtDebug>

#ifdef _WIN32
#  include <direct.h>
#  include <stdlib.h>
#  include <io.h>
#  include <windows.h>
#else
#  include <unistd.h>
#  include <pwd.h>
#endif

#include <string>

#include "fwbuilder/Tools.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Constants.h"
#include "commoninit.h"


extern std::string appRootDir;
extern std::string userDataDir;
extern std::string argv0;
extern std::string ee;
extern QString user_name;

using namespace std;
using namespace libfwbuilder;


void init_win()
{
/*
 * Note appRootDir is the path to the directory where fwbuilder binary
 * is installed (on Mac it is <wherever>/fwbuilder3.app/Contents/MacOS.
 * On Windows it is c:\FWBuilder30, on Linux it is something like
 * /usr/bin or /usr/local/bin and so on.
 */
#if defined(Q_OS_WIN32) || defined(Q_OS_MACX)

//    if (QCoreApplication::instance()==nullptr)
//    {
//        int ac = 0;
//        char **av = { nullptr };
//        new QApplication( ac, av );
//    }
    QDir dir(QCoreApplication::applicationDirPath());
    appRootDir = string(dir.absolutePath().toLatin1().constData());

/* On windows and mac we install API resources (DTD etc) in the 
 * dir right above the one where we install resources for the GUI and compilers
 */
    Constants::init(appRootDir);

//    if (respath == "")
//    {
//        respath = appRootDir + FS_SEPARATOR + Constants::getTemplateDirectory();
//        QFileInfo fi(respath.c_str());
//        respath = fi.canonicalFilePath().toStdString(); // to remove .. and symlinks
//        librespath = fi.canonicalPath().toStdString(); // dir one level up
//    }

    argv0 = QCoreApplication::applicationFilePath().toStdString();

    libfwbuilder::init();

/* default directory where the user may want to save files */
#if defined(Q_OS_WIN32)
    userDataDir = string(getenv("USERPROFILE"))+"\\Documents";
#elif defined(Q_OS_MACX)
    userDataDir = string(getenv("HOME"))+"/Documents";
#endif


#ifdef _WIN32

#define INFO_BUFFER_SIZE 32767

    TCHAR  infoBuf[INFO_BUFFER_SIZE];
    DWORD  bufCharCount = INFO_BUFFER_SIZE;
    
    bufCharCount = INFO_BUFFER_SIZE;
    if( GetUserName( infoBuf, &bufCharCount ) )
    {
#ifdef UNICODE
        user_name = QString::fromUtf16((ushort*)infoBuf);
#else
        user_name = QString::fromLocal8Bit(infoBuf);
#endif
    }

    user_name = user_name.replace(' ','_');

#elif defined(Q_OS_MACX)

    char *lname = getenv("LOGNAME");
    if (lname!=nullptr)
        user_name = QString(lname);
    else
    {
        struct passwd *pwd = getpwuid(getuid());
        assert(pwd);
        user_name = QString(pwd->pw_name);
    }

#endif
#endif
}

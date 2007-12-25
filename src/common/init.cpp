
#include "config.h"

#ifdef _WIN32
#  include <qsettings.h>
#endif

#ifndef _WIN32
#  include <limits.h>
#  include <unistd.h>
#endif

#ifdef Q_OS_MACX
#  include <CoreFoundation/CFURL.h>
#  include <CoreFoundation/CFBundle.h>
#endif

#include <string>
#include <QString>
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


string guessExecPath(const char *argv0)
{

#ifdef Q_OS_MACX
// see http://doc.trolltech.com/3.3/mac-differences.html#7-1
// except that article explains how to get path to bundle, while 
// we need path to executable here. Using CFBundleCopyExecutableURL
// instead of CFBundleCopyBundleURL
//
    CFURLRef bundleURL = CFBundleCopyExecutableURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(bundleURL, 
                                                  kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath, 
                                                CFStringGetSystemEncoding());
    CFRelease(bundleURL);
    CFRelease(macPath);

    return pathPtr;

#else

    QString s = argv0;

    if (s[0]=='/')       return argv0;
    if (s.indexOf('/')!=-1) return s.toLatin1().constData();

// argv0 does not start with '/' and contains no '/' - use PATH
    QString path = getenv("PATH");
    int i1=0;

    while ( !(s=path.section(':',i1,i1)).isEmpty() )
    {
        s=s+"/"+argv0;
        if (access(s.toLatin1().constData(),F_OK|X_OK)==0) return s.toLatin1().constData();
        i1++;
    }
    return argv0;
#endif
}


string findExecutable(const char *argv0)
{
#ifdef _WIN32
/* see explanation about _pgmptr here:
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/getmodulefilename.asp
*/
    string res;

    res = _pgmptr;
    return res;
#else

#ifdef OS_LINUX
/* on modern Linux systems full path to the executable is available in
 * /proc/self/exec.
 */
    char buf[PATH_MAX];
    if ( readlink( "/proc/self/exec", buf, sizeof(buf) )<0 )
    {
        return guessExecPath(argv0);
    } else
        return buf;
#else
    return guessExecPath(argv0);
#endif
#endif
}



void init(char * const *argv)
{
    argv0=findExecutable(argv[0]);

    string::size_type n0=argv0.find_last_of("/\\");
    if (n0!=string::npos)    appRootDir=argv0.substr(0,n0) + FS_SEPARATOR;
    else                     appRootDir="";

    if (fwbdebug)
    {
      qDebug(QString("argv0=%1").arg(argv0.c_str()).toAscii().constData());
      qDebug(QString("appRootDir=%1").arg(appRootDir.c_str()).toAscii().constData());
    }

    libfwbuilder::init();

#if defined(Q_OS_WIN32) || defined(Q_OS_MACX)

/* On windows and mac we install API resources (DTD etc) in the 
 * dir right above the one where we install resources for the GUI and compilers
 */
    if (respath=="") respath    = appRootDir+RES_DIR; 
    n0=respath.find_last_of("/\\");
    librespath = respath.substr(0,n0);

    sysfname = respath+FS_SEPARATOR+"objects_init.xml";
    tempfname = respath+FS_SEPARATOR+"templates.xml";

#else

/* On Unix RES_DIR and LIBFWBUILDER_TEMPLATE_DIR are absolute paths */

    if (respath=="") respath = RES_DIR;
    librespath = LIBFWBUILDER_TEMPLATE_DIR;

    sysfname = respath+ FS_SEPARATOR  + "objects_init.xml";
    tempfname = respath+ FS_SEPARATOR + "templates.xml";

#endif

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

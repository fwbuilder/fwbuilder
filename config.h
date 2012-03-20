/* config.h.  Generated from config.h.in by configure.  */

#include "VERSION.h"

#if defined(_WIN32)
#  include "config.win32.h"
#else

#define BUILD_NUM "3598"

/* #undef PACKAGE_LOCALE_DIR */
/* #undef PACKAGE_DATA_DIR */
/* #undef PACKAGE_SOURCE_DIR */

/* #undef RCS_DIR */
#define RCS_FILE_NAME "/usr/bin/rcs"
#define RCSDIFF_FILE_NAME "/usr/bin/rcsdiff"
#define RLOG_FILE_NAME "/usr/bin/rlog"
#define CI_FILE_NAME "/usr/bin/ci"
#define CO_FILE_NAME "/usr/bin/co"

/* Define if you have the <X11/SM/SMlib.h> header file.  */
/* #undef HAVE_X11_SM_SMLIB_H */

/* Name of package */
#define PACKAGE "fwbuilder"

/* OS */
#define OS "Linux"

/* OS */
/* #undef OS_CYGWIN */
/* #undef OS_MINGW */
/* #undef OS_MACOSX */
/* #undef OS_SOLARIS */
/* #undef OS_FREEBSD */
/* #undef OS_OPENBSD */
#define OS_LINUX "1"
/* #undef OS_UNKNOWN */

#if defined(OS_SOLARIS) || defined(OS_FREEBSD) || defined(OS_OPENBSD) || defined(OS_LINUX) || defined(OS_MACOSX)
#define OS_UNIX 1
#endif

#if defined(_WIN32)
#define OS_WIN32 1
#endif

/* distribution (for Linux) */
#define DISTRO "Debian"

/* prefix dir    */
#define PREFIX "/usr/local"

#define MANIFEST_MARKER "# files: "

/* #undef HAVE_LOCALE_H */
#define HAVE_GETOPT_H 1
/* #undef HAVE_SETLOCALE */
/* #undef HAVE_SETENV */
/* #undef HAVE_PUTENV */
#define HAVE_SIGNAL 1
#define HAVE_SIGNAL_H 1

#define HAVE_PTY_H 1
/* #undef HAVE_LIBUTIL_H */
/* #undef HAVE_UTIL_H */
#define HAVE_UNISTD_H 1

#ifdef HAVE_GETOPT_H
#  define HAVE_DECL_GETOPT  HAVE_GETOPT_H
#endif

#define HAVE_STRUCT_TM_TM_ZONE 1
/* #undef TM_IN_SYS_TIME */

#define HAVE_FORKPTY 1
#define HAVE_CFMAKERAW 1

/*
 *  This is needed for Solaris
 */
/* #undef __PRAGMA_REDEFINE_EXTNAME */



/* #undef HAVE_CATGETS */
/* #undef HAVE_GETTEXT */
/* #undef HAVE_LC_MESSAGES */
/* #undef HAVE_STPCPY */
/* #undef HAVE_LIBSM */
/* #undef HAVE_MEMPCPY */
#define HAVE_STRCHR 1

/* #undef HAVE_ANTLR_RUNTIME */

/* #undef HAVE_QTDBUS */

#define HAVE_CPPUNIT 1

/*
 *  on some platforms (OpenBSD) the second parameter to dlopen is different
 */
/* #undef DLOPEN_MODE */

#if 0
#ifdef __cplusplus
using namespace std;
/*
  #ifndef __STD
  #define __STD std
  #endif
*/
#endif
#endif

#ifndef _WIN32
#  define SNPRINTF  snprintf
#  define VSNPRINTF vsnprintf
#else
#  define SNPRINTF  _snprintf
#  define VSNPRINTF _vsnprintf
#endif

#endif


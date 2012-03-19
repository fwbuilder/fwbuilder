
#include "VERSION.h"

#ifdef _WIN32

#ifndef __GNUC__
/*****************  MSVC ****************/

/* define constants for access that are missing on windows  */

/* Values for the second argument to access.
   These may be OR'd together.  */
#define R_OK    4               /* Test for read permission.  */
#define W_OK    2               /* Test for write permission.  */
#define X_OK    1               /* Test for execute permission.  */
#define F_OK    0               /* Test for existence.  */

/****************************************/
#endif

#define _WIN32_WINNT 0x0501

#undef PACKAGE_LOCALE_DIR
#undef PACKAGE_DATA_DIR
#undef PACKAGE_SOURCE_DIR

#define RCS_FILE_NAME      "rcs.exe"
#define RCSDIFF_FILE_NAME  "rcsdiff.exe"
#define RLOG_FILE_NAME     "rlog.exe"
#define CI_FILE_NAME       "ci.exe"
#define CO_FILE_NAME       "co.exe"

/*
 * Normally this macro defines directory where system-wide QT
 * translations are installed. We do not use it on win32 since
 * we can't assume user has QT installed on their machine.
 */
#define QTTRANSLATIONSDIR "."

/* Define if you have the <X11/SM/SMlib.h> header file.  */
#undef HAVE_X11_SM_SMLIB_H

/* Name of package */
#define PACKAGE fwbuilder2

/* init dir */
#define RES_DIR "resources"

/* OS */
#undef OS

/* OS */
#define OS_WIN32 1

/* distribution (for Linux) */
#define DISTRO "Unknown"

/* prefix dir */
/* #define PREFIX Z: */

#define MANIFEST_MARKER "# files: "

#undef HAVE_LOCALE_H
#undef HAVE_GETOPT_H
#undef HAVE_SETLOCALE
#undef HAVE_SETENV
#undef HAVE_PUTENV

#ifdef HAVE_GETOPT_H
#  define HAVE_DECL_GETOPT  HAVE_GETOPT_H
#endif

#undef HAVE_STRUCT_TM_TM_ZONE
#undef TM_IN_SYS_TIME

/*
 *  This is needed for Solaris
 */
#undef __PRAGMA_REDEFINE_EXTNAME



#undef HAVE_CATGETS
#undef HAVE_GETTEXT
#undef HAVE_LC_MESSAGES
#undef HAVE_STPCPY
#undef HAVE_LIBSM
#undef HAVE_MEMPCPY
#undef HAVE_STRCHR

#define HAVE_LIBSNMP 1
#define HAVE_ANTLR_RUNTIME 1

#define SNPRINTF  _snprintf
#define VSNPRINTF _vsnprintf



#endif     /* End of _WIN32 */

/* **************************************************************** */
#ifdef Q_OS_MACX

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
#define OS "MacOSX"

/* OS */
/* #undef OS_CYGWIN */
/* #undef OS_MINGW */
#define OS_MACOSX "1"
/* #undef OS_SOLARIS */
/* #undef OS_FREEBSD */
/* #undef OS_OPENBSD */
/* #undef OS_LINUX */
/* #undef OS_UNKNOWN */

#if defined(OS_SOLARIS) || defined(OS_FREEBSD) || defined(OS_OPENBSD) || defined(OS_LINUX) || defined(OS_MACOSX)
#define OS_UNIX 1
#endif

/* distribution (for Linux) */
#define DISTRO ""

/* prefix dir    */
/* #undef PREFIX */

/* init dir */
#define RES_DIR "../Resources"

#define MANIFEST_MARKER "# files: "

/* #undef HAVE_LOCALE_H */
#define HAVE_GETOPT_H 1
/* #undef HAVE_SETLOCALE */
/* #undef HAVE_SETENV */
/* #undef HAVE_PUTENV */
#define HAVE_SIGNAL 1
#define HAVE_SIGNAL_H 1

/* #undef HAVE_PTY_H */
/* #undef HAVE_LIBUTIL_H */
#define HAVE_UTIL_H 1

#ifdef HAVE_GETOPT_H
#  define HAVE_DECL_GETOPT  HAVE_GETOPT_H
#endif

#define HAVE_STRUCT_TM_TM_ZONE 1
/* #undef TM_IN_SYS_TIME */

#define HAVE_FORKPTY 1
#define HAVE_CFMAKERAW 1

/* #undef HAVE_CATGETS */
/* #undef HAVE_GETTEXT */
/* #undef HAVE_LC_MESSAGES */
/* #undef HAVE_STPCPY */
/* #undef HAVE_LIBSM */
/* #undef HAVE_MEMPCPY */
#define HAVE_STRCHR 1


/*
 *  on some platforms (OpenBSD) the second parameter to dlopen is different
 */
/* #undef DLOPEN_MODE */

#define SNPRINTF  snprintf
#define VSNPRINTF vsnprintf

#define HAVE_LIBSNMP 1
#define HAVE_ANTLR_RUNTIME 1

#endif

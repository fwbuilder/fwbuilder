dnl 
dnl  $Id: acsite.m4 808 2004-09-08 05:34:53Z vkurland $
dnl 


dnl Test files
define( [AC_TEST_FILES],
[
    ac_file_found=yes
    for f in $1; do
        if test ! -f $2/$f; then
           ac_file_found=no
           break;
        fi
    done

    if test "$ac_file_found" = "yes" ; then
        ifelse([$3], , :,[$3])
    else
        ifelse([$4], , :,[$4])
    fi
])



dnl Search for headers, add path to CPPFLAGS if found 
define( [AC_SEARCH_HEADERS], 
[
    AC_MSG_CHECKING("for $1") 
    ac_hdr_found=no
    for p in $2; do
        AC_TEST_FILES($1, $p, 
            [ 
               ac_hdr_found=yes
               break
            ]
        )
    done 
    if test "$ac_hdr_found" = "yes" ; then
        CPPFLAGS="$CPPFLAGS -I$p"
        AC_MSG_RESULT( [($p) yes] ) 
        ifelse([$3], , :,[$3])
    else
        AC_MSG_RESULT("no") 
        ifelse([$4], , :,[$4])
    fi
])

dnl checks for ucd-snmp or netsnmp library and sets 
dnl vars LIBSNMP_LIBS and HAVE_LIBSNMP
dnl
dnl call like this: 
dnl   AC_CHECK_LIBSNMP ( snmp    )
dnl   AC_CHECK_LIBSNMP ( netsnmp )
dnl 
define( [AC_CHECK_LIBSNMP],
[
  ac_snmplib_name="$1"

  ac_snmplib_name=`echo ${ac_snmplib_name} | tr -d " "`

  AC_CHECK_LIB($ac_snmplib_name, init_snmp,
  [
          LIBSNMP_LIBS="-l$ac_snmplib_name"
	  HAVE_LIBSNMP="1"
          AC_DEFINE(HAVE_LIBSNMP)
  ],[

    if test "${ac_cv_lib_snmp_init_snmp+set}" = "set"; then
        unset ac_cv_lib_snmp_init_snmp
    fi

    save_LIBS="$LIBS"
    LIBS="$LIBS -lcrypto"
    AC_CHECK_LIB($ac_snmplib_name, init_snmp,
    [
          LIBS="$save_LIBS"
          LIBSNMP_LIBS="-lcrypto -l$ac_snmplib_name"
	  HAVE_LIBSNMP="1"
          AC_DEFINE(HAVE_LIBSNMP)
    ],[
	echo

dnl      if test "${ac_cv_lib_snmp_init_snmp+set}" = "set"; then
dnl        unset ac_cv_lib_snmp_init_snmp
dnl      fi
dnl
dnl      save_LIBS="$LIBS"
dnl      LIBS="$LIBS -ldes"
dnl      AC_CHECK_LIB($ac_snmplib_name, init_snmp,
dnl      [
dnl          LIBS="$save_LIBS"
dnl          LIBSNMP_LIBS="-ldes -l$ac_snmplib_name"
dnl	  HAVE_LIBSNMP="1"
dnl          AC_DEFINE(HAVE_LIBSNMP)
dnl      ])
    ])
  ])


dnl  if test "Z$HAVE_LIBSNMP" != "Z"; then
dnl    AC_CHECK_LIB($ac_snmplib_name, snprint_objid, [
dnl        AC_DEFINE(HAVE_SNPRINT_OBJID)
dnl    ])
dnl  fi
])


define( [AC_CHECK_GETHOSTBYNAME_R],
[
  ac_define_this="$1"
  if test -z "$ac_define_this"; then ac_define_this="__FWB_DUMMY__"; fi

AC_MSG_CHECKING(if gethostbyname_r takes 3 arguments)
AC_TRY_COMPILE([
#define $ac_define_this 
#include <netdb.h>
],[

        char *name;
        struct hostent *he;
        struct hostent_data data;
        (void) gethostbyname_r(name, he, &data);

  ],
	AC_MSG_RESULT(yes)
        AC_DEFINE(HAVE_FUNC_GETHOSTBYNAME_R_3)
        ac_cv_func_which_gethostname_r="3", 
  [
dnl			ac_cv_func_which_gethostname_r=no
  AC_MSG_RESULT(no)
  AC_MSG_CHECKING(if gethostbyname_r takes 6 arguments)
  AC_TRY_COMPILE([
#define $ac_define_this 
#include <netdb.h>
  ],[
	char *name;
	struct hostent *he, *res;
	char buffer[2048];
	int buflen = 2048;
	int h_errnop;
	(void) gethostbyname_r(name, he, buffer, buflen, &res, &h_errnop)
    ],
	AC_MSG_RESULT(yes)
        AC_DEFINE(HAVE_FUNC_GETHOSTBYNAME_R_6)
        ac_cv_func_which_gethostname_r="6",
    [
dnl  ac_cv_func_which_gethostname_r=no
    AC_MSG_RESULT(no)
    AC_MSG_CHECKING(if gethostbyname_r takes 5 arguments)
    AC_TRY_COMPILE([
#define $ac_define_this 
#include <netdb.h>
    ],[
			char *name;
			struct hostent *he;
			char buffer[2048];
			int buflen = 2048;
			int h_errnop;
			(void) gethostbyname_r(name, he, buffer, buflen, &h_errnop)
      ],
	AC_MSG_RESULT(yes)
        AC_DEFINE(HAVE_FUNC_GETHOSTBYNAME_R_5)
        ac_cv_func_which_gethostname_r="5",
      [
	AC_MSG_RESULT(no)
        ac_cv_func_which_gethostname_r=no])
    ])
  ] ,ac_cv_func_which_gethostname_r=no)
])






dnl check for number of arguments to gethostbyaddr_r. it might take
dnl 5, 7, or 8 arguments.

define( [AC_CHECK_GETHOSTBYADDR_R],
[
  ac_define_this="$1"
  if test -z "$ac_define_this"; then ac_define_this="__FWB_DUMMY__"; fi


AC_MSG_CHECKING(if gethostbyaddr_r takes 5 arguments)
AC_TRY_COMPILE([
#define $ac_define_this 
#include <sys/types.h>
#include <netdb.h>
],[
char * address;
int length;
int type;
struct hostent h;
struct hostent_data hdata;
int rc;
rc = gethostbyaddr_r(address, length, type, &h, &hdata);
],[
      AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_GETHOSTBYADDR_R_5)
      ac_cv_gethostbyaddr_args=5
],[


      AC_MSG_RESULT(no)
      AC_MSG_CHECKING(if gethostbyaddr_r takes 7 arguments)
      AC_TRY_COMPILE([
#define $ac_define_this 
#include <sys/types.h>
#include <netdb.h>
],[
char * address;
int length;
int type;
struct hostent h;
char buffer[8192];
int h_errnop;
struct hostent * hp;

hp = gethostbyaddr_r(address, length, type, &h,
                     buffer, 8192, &h_errnop);
],[
	  AC_MSG_RESULT(yes)
	  AC_DEFINE(HAVE_GETHOSTBYADDR_R_7)
	  ac_cv_gethostbyaddr_args=7
],[


	  AC_MSG_RESULT(no)
	  AC_MSG_CHECKING(if gethostbyaddr_r takes 8 arguments and first arg is (in_addr*))
	  AC_TRY_COMPILE([
#define $ac_define_this 
#include <sys/types.h>
#include <netdb.h>
],[
struct in_addr *address;
int length;
int type;
struct hostent h;
char buffer[8192];
int h_errnop;
struct hostent * hp;
int rc;

rc = gethostbyaddr_r(address, length, type, &h,
                     buffer, 8192, &hp, &h_errnop);
],[
	    AC_MSG_RESULT(yes)
	    AC_DEFINE(HAVE_GETHOSTBYADDR_R_8)
            AC_DEFINE(GETHOSTBYADDR_FIRST_ARG_VOIDPTR)
            ac_cv_gethostbyaddr_first_arg="voidptr"
	    ac_cv_gethostbyaddr_args=8
],[
	    AC_MSG_RESULT(no)

	    AC_MSG_CHECKING(if gethostbyaddr_r takes 8 arguments and first arg is (char*))
	    AC_TRY_COMPILE([
#define $ac_define_this 
#include <sys/types.h>
#include <netdb.h>
],[
char * address;
int length;
int type;
struct hostent h;
char buffer[8192];
int h_errnop;
struct hostent * hp;
int rc;

rc = gethostbyaddr_r(address, length, type, &h,
                     buffer, 8192, &hp, &h_errnop);
],[
	    AC_MSG_RESULT(yes)
	    AC_DEFINE(HAVE_GETHOSTBYADDR_R_8)
            AC_DEFINE(GETHOSTBYADDR_FIRST_ARG_CHARPTR)
            ac_cv_gethostbyaddr_first_arg="charptr"
	    ac_cv_gethostbyaddr_args=8
],[
	    AC_MSG_RESULT(no)
            have_missing_r_funcs="$have_missing_r_funcs gethostbyaddr_r"
	    ac_cv_gethostbyaddr_args=no
])
])
])
])
])
])



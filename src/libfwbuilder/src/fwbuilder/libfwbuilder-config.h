/* src/libfwbuilder/src/fwbuilder/libfwbuilder-config.h.  Generated from libfwbuilder-config.h.in by configure.  */

#define FWBUILDER_XML_VERSION "22"

#define FREEXMLBUFF(x) xmlFree((void*)(x))


#ifdef _WIN32
/* #  pragma warning(disable:4786) */
#  define _WIN32_WINNT 0x0501

/*
 * Template files directory dir
 * DO NOT USE THIS CONSTANT DIRECTLY - USE Constants class instead!
#  define RES_DIR "/usr/local/share/fwbuilder-5.1.0.3598"
 */

/* #undef UCD_SNMP */
#  define HAVE_LIBSNMP 1
#  define NET_SNMP 1

/*
 * stdint.h must be installed on windows to satisfy uint128.h
 */
#  define HAVE_STDINT_H 1

/* xmlSaveFile */
#  define HAVE_XMLSAVEFORMATFILEENC 1

/* #undef HAVE_LIBXSLT_XSLTCONFIG_H */

/* strtok_s on windows */
#  define HAVE_STRTOK_S  1

/* net-snmp lib checks HAVE_STRTOK_R on windows. This function
 * exists both on windows and mac
 */
#  define HAVE_STRTOK_R 1
#  define WORDS_LITTLEENDIAN 1

/*
 * newer versions of UCD-SNMP use this function. This should be preferred way,
 * but Mandrake 8.2 comes with ucd-snmp v.4.2.3 which does not have it.
 */
#  define HAVE_SNPRINT_OBJID 1

/*
 * always need this for openssl
 */
#  define OPENSSL_NO_KRB5

#  define SNPRINTF  _snprintf
#  define VSNPRINTF _vsnprintf
#  define FS_SEPARATOR "\\"

#else

/* 
 * Template files directory dir
 * DO NOT USE THIS CONSTANT DIRECTLY - USE Constants class instead!
 */
#define RES_DIR "/usr/local/share/fwbuilder-5.1.0.3598"

#define HAVE_LIBSNMP 1
/* #undef UCD_SNMP */
#define NET_SNMP 1

/* #undef HAVE_INET_NET_NTOP */

/* #undef HAVE_GOODLIBRESOLV */

/* #undef HAVE_LWRES */

/* #undef HAVE_LWRES_GETIPNODE */

#define HAVE_GETOPT_H 1

/* gethostbyname_r */
/* #undef HAVE_FUNC_GETHOSTBYNAME_R_6 */
/* #undef HAVE_FUNC_GETHOSTBYNAME_R_5 */
/* #undef HAVE_FUNC_GETHOSTBYNAME_R_3 */

/* gethostbyaddr_r */
/* #undef HAVE_GETHOSTBYADDR_R_5 */
/* #undef HAVE_GETHOSTBYADDR_R_7 */
/* #undef HAVE_GETHOSTBYADDR_R_8 */

/* first argument of gethostbyaddr_r may be char* or void* */
/* #undef GETHOSTBYADDR_FIRST_ARG_CHARPTR */
/* #undef GETHOSTBYADDR_FIRST_ARG_VOIDPTR */

/* xmlSaveFile */
#define HAVE_XMLSAVEFORMATFILEENC 1

#define HAVE_LIBXSLT_XSLTCONFIG_H 1

/* strtok_r */
#define HAVE_STRTOK_R 1

#define WORDS_LITTLEENDIAN 1
/* #undef WORDS_BIGENDIAN */

/*
 * newer versions of UCD-SNMP use this function. This should be preferred way,
 * but Mandrake 8.2 comes with ucd-snmp v.4.2.3 which does not have it.
 */
#define HAVE_SNPRINT_OBJID 1

#  define SNPRINTF  snprintf
#  define VSNPRINTF vsnprintf
#  define FS_SEPARATOR "/"

#endif


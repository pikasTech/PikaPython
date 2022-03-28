/* This file was automatically generated.  Do not edit (except for compile time directive)! */ 
#ifndef _PH7_H_
#define _PH7_H_
/*
 * Symisc PH7: An embeddable bytecode compiler and a virtual machine for the PHP(5) programming language.
 * Copyright (C) 2011-2012, Symisc Systems http://ph7.symisc.net/
 * Version 2.1.4
 * For information on licensing,redistribution of this file,and for a DISCLAIMER OF ALL WARRANTIES
 * please contact Symisc Systems via:
 *       legal@symisc.net
 *       licensing@symisc.net
 *       contact@symisc.net
 * or visit:
 *      http://ph7.symisc.net/
 */
/*
 * Copyright (C) 2011, 2012 Symisc Systems. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Redistributions in any form must be accompanied by information on
 *    how to obtain complete source code for the PH7 engine and any 
 *    accompanying software that uses the PH7 engine software.
 *    The source code must either be included in the distribution
 *    or be available for no more than the cost of distribution plus
 *    a nominal fee, and must be freely redistributable under reasonable
 *    conditions. For an executable file, complete source code means
 *    the source code for all modules it contains.It does not include
 *    source code for modules or files that typically accompany the major
 *    components of the operating system on which the executable file runs.
 *
 * THIS SOFTWARE IS PROVIDED BY SYMISC SYSTEMS ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
 * NON-INFRINGEMENT, ARE DISCLAIMED.  IN NO EVENT SHALL SYMISC SYSTEMS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 /* $SymiscID: ph7.h v2.1 UNIX|WIN32/64 2012-09-15 09:43 stable <chm@symisc.net> $ */
#include <stdarg.h> /* needed for the definition of va_list */
/*
 * Compile time engine version, signature, identification in the symisc source tree
 * and copyright notice.
 * Each macro have an equivalent C interface associated with it that provide the same
 * information but are associated with the library instead of the header file.
 * Refer to [ph7_lib_version()], [ph7_lib_signature()], [ph7_lib_ident()] and
 * [ph7_lib_copyright()] for more information.
 */
/*
 * The PH7_VERSION C preprocessor macroevaluates to a string literal
 * that is the ph7 version in the format "X.Y.Z" where X is the major
 * version number and Y is the minor version number and Z is the release
 * number.
 */
#define PH7_VERSION "2.1.4"
/*
 * The PH7_VERSION_NUMBER C preprocessor macro resolves to an integer
 * with the value (X*1000000 + Y*1000 + Z) where X, Y, and Z are the same
 * numbers used in [PH7_VERSION].
 */
#define PH7_VERSION_NUMBER 2001004
/*
 * The PH7_SIG C preprocessor macro evaluates to a string
 * literal which is the public signature of the ph7 engine.
 * This signature could be included for example in a host-application
 * generated Server MIME header as follows:
 *   Server: YourWebServer/x.x PH7/x.x.x \r\n
 */
#define PH7_SIG "PH7/2.1.4"
/*
 * PH7 identification in the Symisc source tree:
 * Each particular check-in of a particular software released
 * by symisc systems have an unique identifier associated with it.
 * This macro hold the one associated with ph7.
 */
#define PH7_IDENT "ph7:c193f4d8a6b90ee60f9afad11840f1010054fdf9"
/*
 * Copyright notice.
 * If you have any questions about the licensing situation,please
 * visit http://ph7.symisc.net/licensing.html
 * or contact Symisc Systems via:
 *   legal@symisc.net
 *   licensing@symisc.net
 *   contact@symisc.net
 */
#define PH7_COPYRIGHT "Copyright (C) Symisc Systems 2011-2012, http://ph7.symisc.net/"
/* Make sure we can call this stuff from C++ */
#ifdef __cplusplus
extern "C" { 
#endif
/* Forward declaration to public objects */
typedef struct ph7_io_stream ph7_io_stream;
typedef struct ph7_context ph7_context;
typedef struct ph7_value ph7_value;
typedef struct ph7_vfs ph7_vfs;
typedef struct ph7_vm ph7_vm;
typedef struct ph7 ph7;
/*
 * ------------------------------
 * Compile time directives
 * ------------------------------
 * For most purposes, PH7 can be built just fine using the default compilation options.
 * However, if required, the compile-time options documented below can be used to omit
 * PH7 features (resulting in a smaller compiled library size) or to change the default
 * values of some parameters. 
 * Every effort has been made to ensure that the various combinations of compilation 
 * options work harmoniously and produce a working library.
 *
 * PH7_ENABLE_THREADS
 *   This option controls whether or not code is included in PH7 to enable it to operate
 *   safely in a multithreaded environment. The default is not. That is,all mutexing code
 *   is omitted and it is unsafe to use PH7 in a multithreaded program. When compiled
 *   with the PH7_ENABLE_THREADS directive enabled, PH7 can be used in a multithreaded
 *   program and it's safe to share the same virtual machine and engine instance between
 *   two or more threads.
 *   The value of PH7_ENABLE_THREADS can be determined at run-time using the
 *   ph7_lib_is_threadsafe() interface.When PH7 has been compiled with PH7_ENABLE_THREAD
 *   then the threading mode can be altered at run-time using the ph7_lib_config()
 *   interface together with one of these verbs:
 *      PH7_LIB_CONFIG_THREAD_LEVEL_SINGLE
 *      PH7_LIB_CONFIG_THREAD_LEVEL_MULTI
 *   Also note,platforms others than Windows and UNIX systems must install their own
 *   mutex subsystem via  ph7_lib_config() with a configuration verb set to
 *   PH7_LIB_CONFIG_USER_MUTEX. Otherwise the library is not threadsafe.
 *   Note that you must link PH7 with the POSIX threads library under UNIX-like systems
 *   (i.e: -lpthread).Otherwise you will get a link time error.
 * Options To Omit/Enable Features:
 *   The following options can be used to reduce the size of the compiled library
 *   by omitting optional features. This is probably only useful in embedded systems
 *   where space is especially tight, as even with all features included the PH7 library
 *   is relatively small. Don't forget to tell your compiler to optimize for binary
 *   size! (the -Os option if using GCC).
 *   Telling your compiler to optimize for size usually has a much larger impact
 *   on library footprint than employing any of these compile-time options.
 *   PH7_DISABLE_BUILTIN_FUNC
 *     PH7 come with more than 460 built-in functions suitable for most purposes ranging
 *     from string/XML/INI processing to ZIP extracting, Base64 encoding/decoding and so on.
 *     If this directive is enabled, all the built-in functions are omitted from the build.
 *     Note that language construct functions such as is_int(), is_string(), func_get_arg(),
 *     define(), etc. are not omitted from the build and are not affected by this directive.
 *   PH7_ENABLE_MATH_FUNC
 *     If this directive is enabled, built-in math functions such as sqrt(),abs(),
 *     log(), ceil(), etc. are included in the build. Note that you may need to link
 *     PH7 with the math library in same linux/BSD flavor (i.e: -lm).Otherwise you
 *     will get a link time error.
 *  PH7_DISABLE_DISK_IO
 *     If this directive is enabled, built-in Virtual File System functions such as
 *     chdir(), mkdir(), chroot(), unlink(), delete(), etc. are omitted from the build.
 *  PH7_DISABLE_HASH_IO
 *     If this directive is enabled, built-in hash functions such as md5(), sha1(),
 *     md5_file(), crc32(), etc. are omitted from the build.
 *  PH7_OMIT_FLOATING_POINT
 *     This option is used to omit floating-point number support from the PH7 library
 *     if compiling for a processor that lacks floating point support. When specified
 *     the library will substitute 64-bit integer arithmetic for floating-point which
 *     mean that 25.e-3 and 25 are equals and are of type integer.
 */
/* Symisc public definitions */
#if !defined(SYMISC_STANDARD_DEFS)
#define SYMISC_STANDARD_DEFS
#if defined (_WIN32) || defined (WIN32) || defined(__MINGW32__) || defined (_MSC_VER) || defined (_WIN32_WCE)
/* Windows Systems */
#if !defined(__WINNT__)
#define __WINNT__
#endif 
#else
/*
 * By default we will assume that we are compiling on a UNIX systems.
 * Otherwise the OS_OTHER directive must be defined.
 */
#if !defined(OS_OTHER)
#if !defined(__UNIXES__)
#define __UNIXES__
#endif /* __UNIXES__ */
#else
#endif /* OS_OTHER */
#endif /* __WINNT__/__UNIXES__ */
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef signed __int64     sxi64; /* 64 bits(8 bytes) signed int64 */
typedef unsigned __int64   sxu64; /* 64 bits(8 bytes) unsigned int64 */
#else
typedef signed long long int   sxi64; /* 64 bits(8 bytes) signed int64 */
typedef unsigned long long int sxu64; /* 64 bits(8 bytes) unsigned int64 */
#endif /* _MSC_VER */
/* Signature of the consumer routine */
typedef int (*ProcConsumer)(const void *,unsigned int,void *);
/* Forward reference */
typedef struct SyMutexMethods SyMutexMethods;
typedef struct SyMemMethods SyMemMethods;
typedef struct SyString SyString;
typedef struct syiovec syiovec;
typedef struct SyMutex SyMutex;
typedef struct Sytm Sytm;
/* Scatter and gather array. */
struct syiovec
{
#if defined (__WINNT__)
	/* Same fields type and offset as WSABUF structure defined one winsock2 header */
	unsigned long nLen;
	char *pBase;
#else
	void *pBase;
	unsigned long nLen;
#endif
};
struct SyString
{
	const char *zString;  /* Raw string (may not be null terminated) */
	unsigned int nByte;   /* Raw string length */
};
/* Time structure. */
struct Sytm
{
  int tm_sec;     /* seconds (0 - 60) */
  int tm_min;     /* minutes (0 - 59) */
  int tm_hour;    /* hours (0 - 23) */
  int tm_mday;    /* day of month (1 - 31) */
  int tm_mon;     /* month of year (0 - 11) */
  int tm_year;    /* year + 1900 */
  int tm_wday;    /* day of week (Sunday = 0) */
  int tm_yday;    /* day of year (0 - 365) */
  int tm_isdst;   /* is summer time in effect? */
  char *tm_zone;  /* abbreviation of timezone name */
  long tm_gmtoff; /* offset from UTC in seconds */
};
/* Convert a tm structure (struct tm *) found in <time.h> to a Sytm structure */
#define STRUCT_TM_TO_SYTM(pTM,pSYTM) \
	(pSYTM)->tm_hour = (pTM)->tm_hour;\
	(pSYTM)->tm_min	 = (pTM)->tm_min;\
	(pSYTM)->tm_sec	 = (pTM)->tm_sec;\
	(pSYTM)->tm_mon	 = (pTM)->tm_mon;\
	(pSYTM)->tm_mday = (pTM)->tm_mday;\
	(pSYTM)->tm_year = (pTM)->tm_year + 1900;\
	(pSYTM)->tm_yday = (pTM)->tm_yday;\
	(pSYTM)->tm_wday = (pTM)->tm_wday;\
	(pSYTM)->tm_isdst = (pTM)->tm_isdst;\
	(pSYTM)->tm_gmtoff = 0;\
	(pSYTM)->tm_zone = 0;

/* Convert a SYSTEMTIME structure (LPSYSTEMTIME: Windows Systems only ) to a Sytm structure */
#define SYSTEMTIME_TO_SYTM(pSYSTIME,pSYTM) \
	 (pSYTM)->tm_hour = (pSYSTIME)->wHour;\
	 (pSYTM)->tm_min  = (pSYSTIME)->wMinute;\
	 (pSYTM)->tm_sec  = (pSYSTIME)->wSecond;\
	 (pSYTM)->tm_mon  = (pSYSTIME)->wMonth - 1;\
	 (pSYTM)->tm_mday = (pSYSTIME)->wDay;\
	 (pSYTM)->tm_year = (pSYSTIME)->wYear;\
	 (pSYTM)->tm_yday = 0;\
	 (pSYTM)->tm_wday = (pSYSTIME)->wDayOfWeek;\
	 (pSYTM)->tm_gmtoff = 0;\
	 (pSYTM)->tm_isdst = -1;\
	 (pSYTM)->tm_zone = 0;

/* Dynamic memory allocation methods. */
struct SyMemMethods 
{
	void * (*xAlloc)(unsigned int);          /* [Required:] Allocate a memory chunk */
	void * (*xRealloc)(void *,unsigned int); /* [Required:] Re-allocate a memory chunk */
	void   (*xFree)(void *);                 /* [Required:] Release a memory chunk */
	unsigned int  (*xChunkSize)(void *);     /* [Optional:] Return chunk size */
	int    (*xInit)(void *);                 /* [Optional:] Initialization callback */
	void   (*xRelease)(void *);              /* [Optional:] Release callback */
	void  *pUserData;                        /* [Optional:] First argument to xInit() and xRelease() */
};
/* Out of memory callback signature. */
typedef int (*ProcMemError)(void *);
/* Mutex methods. */
struct SyMutexMethods 
{
	int (*xGlobalInit)(void);		/* [Optional:] Global mutex initialization */
	void  (*xGlobalRelease)(void);	/* [Optional:] Global Release callback () */
	SyMutex * (*xNew)(int);	        /* [Required:] Request a new mutex */
	void  (*xRelease)(SyMutex *);	/* [Optional:] Release a mutex  */
	void  (*xEnter)(SyMutex *);	    /* [Required:] Enter mutex */
	int (*xTryEnter)(SyMutex *);    /* [Optional:] Try to enter a mutex */
	void  (*xLeave)(SyMutex *);	    /* [Required:] Leave a locked mutex */
};
#if defined (_MSC_VER) || defined (__MINGW32__) ||  defined (__GNUC__) && defined (__declspec)
#define SX_APIIMPORT	__declspec(dllimport)
#define SX_APIEXPORT	__declspec(dllexport)
#else
#define	SX_APIIMPORT
#define	SX_APIEXPORT
#endif
/* Standard return values from Symisc public interfaces */
#define SXRET_OK       0      /* Not an error */	
#define SXERR_MEM      (-1)   /* Out of memory */
#define SXERR_IO       (-2)   /* IO error */
#define SXERR_EMPTY    (-3)   /* Empty field */
#define SXERR_LOCKED   (-4)   /* Locked operation */
#define SXERR_ORANGE   (-5)   /* Out of range value */
#define SXERR_NOTFOUND (-6)   /* Item not found */
#define SXERR_LIMIT    (-7)   /* Limit reached */
#define SXERR_MORE     (-8)   /* Need more input */
#define SXERR_INVALID  (-9)   /* Invalid parameter */
#define SXERR_ABORT    (-10)  /* User callback request an operation abort */
#define SXERR_EXISTS   (-11)  /* Item exists */
#define SXERR_SYNTAX   (-12)  /* Syntax error */
#define SXERR_UNKNOWN  (-13)  /* Unknown error */
#define SXERR_BUSY     (-14)  /* Busy operation */
#define SXERR_OVERFLOW (-15)  /* Stack or buffer overflow */
#define SXERR_WILLBLOCK (-16) /* Operation will block */
#define SXERR_NOTIMPLEMENTED  (-17) /* Operation not implemented */
#define SXERR_EOF      (-18) /* End of input */
#define SXERR_PERM     (-19) /* Permission error */
#define SXERR_NOOP     (-20) /* No-op */	
#define SXERR_FORMAT   (-21) /* Invalid format */
#define SXERR_NEXT     (-22) /* Not an error */
#define SXERR_OS       (-23) /* System call return an error */
#define SXERR_CORRUPT  (-24) /* Corrupted pointer */
#define SXERR_CONTINUE (-25) /* Not an error: Operation in progress */
#define SXERR_NOMATCH  (-26) /* No match */
#define SXERR_RESET    (-27) /* Operation reset */
#define SXERR_DONE     (-28) /* Not an error */
#define SXERR_SHORT    (-29) /* Buffer too short */
#define SXERR_PATH     (-30) /* Path error */
#define SXERR_TIMEOUT  (-31) /* Timeout */
#define SXERR_BIG      (-32) /* Too big for processing */
#define SXERR_RETRY    (-33) /* Retry your call */
#define SXERR_IGNORE   (-63) /* Ignore */
#endif /* SYMISC_PUBLIC_DEFS */
/* Standard PH7 return values */
#define PH7_OK      SXRET_OK      /* Successful result */
/* beginning-of-error-codes */
#define PH7_NOMEM   SXERR_MEM     /* Out of memory */
#define PH7_ABORT   SXERR_ABORT   /* Foreign Function request operation abort/Another thread have released this instance */
#define PH7_IO_ERR  SXERR_IO      /* IO error */
#define PH7_CORRUPT SXERR_CORRUPT /* Corrupt pointer/Unknown configuration option */
#define PH7_LOOKED  SXERR_LOCKED  /* Forbidden Operation */ 
#define PH7_COMPILE_ERR (-70)     /* Compilation error */
#define PH7_VM_ERR      (-71)     /* Virtual machine error */
/* end-of-error-codes */
/*
 * If compiling for a processor that lacks floating point
 * support, substitute integer for floating-point.
 */
#ifdef PH7_OMIT_FLOATING_POINT
typedef sxi64 ph7_real;
#else
typedef double ph7_real;
#endif
typedef sxi64 ph7_int64;
#define PH7_APIEXPORT SX_APIEXPORT
/*
 * Engine Configuration Commands.
 *
 * The following set of constants are the available configuration verbs that can
 * be used by the host-application to configure the PH7 engine.
 * These constants must be passed as the second argument to the [ph7_config()] 
 * interface.
 * Each options require a variable number of arguments.
 * The [ph7_config()] interface will return PH7_OK on success, any other
 * return value indicates failure.
 * For a full discussion on the configuration verbs and their expected 
 * parameters, please refer to this page:
 *      http://ph7.symisc.net/c_api_func.html#ph7_config
 */
#define PH7_CONFIG_ERR_OUTPUT    1  /* TWO ARGUMENTS: int (*xConsumer)(const void *pOut,unsigned int nLen,void *pUserData),void *pUserData */
#define PH7_CONFIG_ERR_ABORT     2  /* RESERVED FOR FUTURE USE */
#define PH7_CONFIG_ERR_LOG       3  /* TWO ARGUMENTS: const char **pzBuf,int *pLen */
/*
 * Virtual Machine Configuration Commands.
 *
 * The following set of constants are the available configuration verbs that can
 * be used by the host-application to configure the PH7 Virtual machine.
 * These constants must be passed as the second argument to the [ph7_vm_config()] 
 * interface.
 * Each options require a variable number of arguments.
 * The [ph7_vm_config()] interface will return PH7_OK on success, any other return
 * value indicates failure.
 * There are many options but the most importants are: PH7_VM_CONFIG_OUTPUT which install
 * a VM output consumer callback, PH7_VM_CONFIG_HTTP_REQUEST which parse and register
 * a HTTP request and PH7_VM_CONFIG_ARGV_ENTRY which populate the $argv array.
 * For a full discussion on the configuration verbs and their expected parameters, please
 * refer to this page:
 *      http://ph7.symisc.net/c_api_func.html#ph7_vm_config
 */
#define PH7_VM_CONFIG_OUTPUT           1  /* TWO ARGUMENTS: int (*xConsumer)(const void *pOut,unsigned int nLen,void *pUserData),void *pUserData */
#define PH7_VM_CONFIG_IMPORT_PATH      3  /* ONE ARGUMENT: const char *zIncludePath */
#define PH7_VM_CONFIG_ERR_REPORT       4  /* NO ARGUMENTS: Report all run-time errors in the VM output */
#define PH7_VM_CONFIG_RECURSION_DEPTH  5  /* ONE ARGUMENT: int nMaxDepth */
#define PH7_VM_OUTPUT_LENGTH           6  /* ONE ARGUMENT: unsigned int *pLength */
#define PH7_VM_CONFIG_CREATE_SUPER     7  /* TWO ARGUMENTS: const char *zName,ph7_value *pValue */
#define PH7_VM_CONFIG_CREATE_VAR       8  /* TWO ARGUMENTS: const char *zName,ph7_value *pValue */
#define PH7_VM_CONFIG_HTTP_REQUEST     9  /* TWO ARGUMENTS: const char *zRawRequest,int nRequestLength */
#define PH7_VM_CONFIG_SERVER_ATTR     10  /* THREE ARGUMENTS: const char *zKey,const char *zValue,int nLen */
#define PH7_VM_CONFIG_ENV_ATTR        11  /* THREE ARGUMENTS: const char *zKey,const char *zValue,int nLen */
#define PH7_VM_CONFIG_SESSION_ATTR    12  /* THREE ARGUMENTS: const char *zKey,const char *zValue,int nLen */
#define PH7_VM_CONFIG_POST_ATTR       13  /* THREE ARGUMENTS: const char *zKey,const char *zValue,int nLen */
#define PH7_VM_CONFIG_GET_ATTR        14  /* THREE ARGUMENTS: const char *zKey,const char *zValue,int nLen */
#define PH7_VM_CONFIG_COOKIE_ATTR     15  /* THREE ARGUMENTS: const char *zKey,const char *zValue,int nLen */
#define PH7_VM_CONFIG_HEADER_ATTR     16  /* THREE ARGUMENTS: const char *zKey,const char *zValue,int nLen */
#define PH7_VM_CONFIG_EXEC_VALUE      17  /* ONE ARGUMENT: ph7_value **ppValue */
#define PH7_VM_CONFIG_IO_STREAM       18  /* ONE ARGUMENT: const ph7_io_stream *pStream */
#define PH7_VM_CONFIG_ARGV_ENTRY      19  /* ONE ARGUMENT: const char *zValue */
#define PH7_VM_CONFIG_EXTRACT_OUTPUT  20  /* TWO ARGUMENTS: const void **ppOut,unsigned int *pOutputLen */
#define PH7_VM_CONFIG_ERR_LOG_HANDLER 21  /* ONE ARGUMENT: void (*xErrLog)(const char *,int,const char *,const char *) */
/*
 * Global Library Configuration Commands.
 *
 * The following set of constants are the available configuration verbs that can
 * be used by the host-application to configure the whole library.
 * These constants must be passed as the first argument to the [ph7_lib_config()] 
 * interface.
 * Each options require a variable number of arguments.
 * The [ph7_lib_config()] interface will return PH7_OK on success, any other return
 * value indicates failure.
 * Notes:
 * The default configuration is recommended for most applications and so the call to
 * [ph7_lib_config()] is usually not necessary. It is provided to support rare 
 * applications with unusual needs. 
 * The [ph7_lib_config()] interface is not threadsafe. The application must insure that
 * no other [ph7_*()] interfaces are invoked by other threads while [ph7_lib_config()]
 * is running. Furthermore, [ph7_lib_config()] may only be invoked prior to library
 * initialization using [ph7_lib_init()] or [ph7_init()] or after shutdown
 * by [ph7_lib_shutdown()]. If [ph7_lib_config()] is called after [ph7_lib_init()]
 * or [ph7_init()] and before [ph7_lib_shutdown()] then it will return PH7_LOCKED.
 * Refer to the official documentation for more information on the configuration verbs
 * and their expected parameters.
 * For a full discussion on the configuration verbs and their expected parameters,please
 * refer to this page:
 *      http://ph7.symisc.net/c_api_func.html#Global_Library_Management_Interfaces
 */
#define PH7_LIB_CONFIG_USER_MALLOC            1 /* ONE ARGUMENT: const SyMemMethods *pMemMethods */ 
#define PH7_LIB_CONFIG_MEM_ERR_CALLBACK       2 /* TWO ARGUMENTS: int (*xMemError)(void *),void *pUserData */
#define PH7_LIB_CONFIG_USER_MUTEX             3 /* ONE ARGUMENT: const SyMutexMethods *pMutexMethods */ 
#define PH7_LIB_CONFIG_THREAD_LEVEL_SINGLE    4 /* NO ARGUMENTS */ 
#define PH7_LIB_CONFIG_THREAD_LEVEL_MULTI     5 /* NO ARGUMENTS */ 
#define PH7_LIB_CONFIG_VFS                    6 /* ONE ARGUMENT: const ph7_vfs *pVfs */
/*
 * Compile-time flags.
 * The new compile interfaces [ph7_compile_v2()] and [ph7_compile_file()] takes
 * as their last argument zero or more combination of compile time flags.
 * These flags are used to control the behavior of the PH7 compiler while
 * processing the input.
 * Refer to the official documentation for additional information.
 */
#define PH7_PHP_ONLY 0x01 /* If this flag is set then the code to compile is assumed
                           * to be plain PHP only. That is, there is no need to delimit
						   * the PHP code using the standard tags such as <?php ?> or <? ?>.
						   * Everything will pass through the PH7 compiler.
						   */
#define PH7_PHP_EXPR 0x02 /* This flag is reserved for future use. */
/*
 * Call Context Error Message Serverity Level.
 *
 * The following constans are the allowed severity level that can
 * passed as the second argument to the [ph7_context_throw_error()] or
 * [ph7_context_throw_error_format()] interfaces.
 * Refer to the official documentation for additional information.
 */
#define PH7_CTX_ERR      1 /* Call context error such as unexpected number of arguments,invalid types and so on. */
#define PH7_CTX_WARNING  2 /* Call context Warning */
#define PH7_CTX_NOTICE   3 /* Call context Notice */
/* Current VFS structure version*/
#define PH7_VFS_VERSION 2 
/* 
 * PH7 Virtual File System (VFS).
 *
 * An instance of the ph7_vfs object defines the interface between the PH7 core
 * and the underlying operating system. The "vfs" in the name of the object stands
 * for "virtual file system". The vfs is used to implement PHP system functions
 * such as mkdir(), chdir(), stat(), get_user_name() and many more.
 * The value of the iVersion field is initially 2 but may be larger in future versions
 * of PH7.
 * Additional fields may be appended to this object when the iVersion value is increased.
 * Only a single vfs can be registered within the PH7 core. Vfs registration is done
 * using the ph7_lib_config() interface with a configuration verb set to PH7_LIB_CONFIG_VFS.
 * Note that Windows and UNIX (Linux, FreeBSD, Solaris, Mac OS X, etc.) users does not have to
 * worry about registering and installing a vfs since PH7 come with a built-in vfs for these
 * platforms which implement most the methods defined below.
 * Host-application running on exotic systems (ie: Other than Windows and UNIX systems) must
 * register their own vfs in order to be able to use and call PHP system function.
 * Also note that the ph7_compile_file() interface depend on the xMmap() method of the underlying
 * vfs which mean that this method must be available (Always the case using the built-in VFS)
 * in order to use this interface.
 * Developers wishing to implement the vfs methods can contact symisc systems to obtain
 * the PH7 VFS C/C++ Specification manual.
 */
struct ph7_vfs
{
	const char *zName;  /* Underlying VFS name [i.e: FreeBSD/Linux/Windows...] */
	int iVersion;       /* Current VFS structure version [default 2] */
	/* Directory functions */
	int (*xChdir)(const char *);                     /* Change directory */
	int (*xChroot)(const char *);                    /* Change the root directory */
	int (*xGetcwd)(ph7_context *);                   /* Get the current working directory */
	int (*xMkdir)(const char *,int,int);             /* Make directory */
	int (*xRmdir)(const char *);                     /* Remove directory */
	int (*xIsdir)(const char *);                     /* Tells whether the filename is a directory */
	int (*xRename)(const char *,const char *);       /* Renames a file or directory */
	int (*xRealpath)(const char *,ph7_context *);    /* Return canonicalized absolute pathname*/
	/* Systems functions */
	int (*xSleep)(unsigned int);                     /* Delay execution in microseconds */
	int (*xUnlink)(const char *);                    /* Deletes a file */
	int (*xFileExists)(const char *);                /* Checks whether a file or directory exists */
	int (*xChmod)(const char *,int);                 /* Changes file mode */
	int (*xChown)(const char *,const char *);        /* Changes file owner */
	int (*xChgrp)(const char *,const char *);        /* Changes file group */
	ph7_int64 (*xFreeSpace)(const char *);           /* Available space on filesystem or disk partition */
	ph7_int64 (*xTotalSpace)(const char *);          /* Total space on filesystem or disk partition */
	ph7_int64 (*xFileSize)(const char *);            /* Gets file size */
	ph7_int64 (*xFileAtime)(const char *);           /* Gets last access time of file */
	ph7_int64 (*xFileMtime)(const char *);           /* Gets file modification time */
	ph7_int64 (*xFileCtime)(const char *);           /* Gets inode change time of file */
	int (*xStat)(const char *,ph7_value *,ph7_value *);   /* Gives information about a file */
	int (*xlStat)(const char *,ph7_value *,ph7_value *);  /* Gives information about a file */
	int (*xIsfile)(const char *);                    /* Tells whether the filename is a regular file */
	int (*xIslink)(const char *);                    /* Tells whether the filename is a symbolic link */
	int (*xReadable)(const char *);                  /* Tells whether a file exists and is readable */
	int (*xWritable)(const char *);                  /* Tells whether the filename is writable */
	int (*xExecutable)(const char *);                /* Tells whether the filename is executable */
	int (*xFiletype)(const char *,ph7_context *);    /* Gets file type [i.e: fifo,dir,file..] */
	int (*xGetenv)(const char *,ph7_context *);      /* Gets the value of an environment variable */
	int (*xSetenv)(const char *,const char *);       /* Sets the value of an environment variable */
	int (*xTouch)(const char *,ph7_int64,ph7_int64); /* Sets access and modification time of file */
	int (*xMmap)(const char *,void **,ph7_int64 *);  /* Read-only memory map of the whole file */
	void (*xUnmap)(void *,ph7_int64);                /* Unmap a memory view */
	int (*xLink)(const char *,const char *,int);     /* Create hard or symbolic link */
	int (*xUmask)(int);                              /* Change the current umask */
	void (*xTempDir)(ph7_context *);                 /* Get path of the temporary directory */
	unsigned int (*xProcessId)(void);                /* Get running process ID */
	int (*xUid)(void);                               /* user ID of the process */
	int (*xGid)(void);                               /* group ID of the process */
	void (*xUsername)(ph7_context *);                /* Running username */
	int (*xExec)(const char *,ph7_context *);        /* Execute an external program */
};
/* Current PH7 IO stream structure version. */
#define PH7_IO_STREAM_VERSION 1 
/* 
 * Possible open mode flags that can be passed to the xOpen() routine
 * of the underlying IO stream device .
 * Refer to the PH7 IO Stream C/C++ specification manual (http://ph7.symisc.net/io_stream_spec.html)
 * for additional information.
 */
#define PH7_IO_OPEN_RDONLY   0x001  /* Read-only open */
#define PH7_IO_OPEN_WRONLY   0x002  /* Write-only open */
#define PH7_IO_OPEN_RDWR     0x004  /* Read-write open. */
#define PH7_IO_OPEN_CREATE   0x008  /* If the file does not exist it will be created */
#define PH7_IO_OPEN_TRUNC    0x010  /* Truncate the file to zero length */
#define PH7_IO_OPEN_APPEND   0x020  /* Append mode.The file offset is positioned at the end of the file */
#define PH7_IO_OPEN_EXCL     0x040  /* Ensure that this call creates the file,the file must not exist before */
#define PH7_IO_OPEN_BINARY   0x080  /* Simple hint: Data is binary */
#define PH7_IO_OPEN_TEMP     0x100  /* Simple hint: Temporary file */
#define PH7_IO_OPEN_TEXT     0x200  /* Simple hint: Data is textual */
/*
 * PH7 IO Stream Device.
 *
 * An instance of the ph7_io_stream object defines the interface between the PH7 core
 * and the underlying stream device.
 * A stream is a smart mechanism for generalizing file, network, data compression
 * and other IO operations which share a common set of functions using an abstracted
 * unified interface.
 * A stream device is additional code which tells the stream how to handle specific
 * protocols/encodings. For example, the http device knows how to translate a URL
 * into an HTTP/1.1 request for a file on a remote server.
 * PH7 come with two built-in IO streams device:
 * The file:// stream which perform very efficient disk IO and the php:// stream
 * which is a special stream that allow access various I/O streams (See the PHP official
 * documentation for more information on this stream).
 * A stream is referenced as: scheme://target 
 * scheme(string) - The name of the wrapper to be used. Examples include: file,http,https,ftp,
 * ftps, compress.zlib, compress.bz2, and php. If no wrapper is specified,the function default
 * is used (typically file://). 
 * target - Depends on the device used. For filesystem related streams this is typically a path
 * and filename of the desired file.For network related streams this is typically a hostname,often
 * with a path appended. 
 * IO stream devices are registered using a call to ph7_vm_config() with a configuration verb
 * set to PH7_VM_CONFIG_IO_STREAM.
 * Currently the PH7 development team is working on the implementation of the http:// and ftp://
 * IO stream protocols. These devices will be available in the next major release of the PH7 engine.
 * Developers wishing to implement their own IO stream devices must understand and follow
 * The PH7 IO Stream C/C++ specification manual (http://ph7.symisc.net/io_stream_spec.html).
 */
struct ph7_io_stream
{
	const char *zName;                                     /* Underlying stream name [i.e: file/http/zip/php,..] */
	int iVersion;                                          /* IO stream structure version [default 1]*/
	int  (*xOpen)(const char *,int,ph7_value *,void **);   /* Open handle*/
	int  (*xOpenDir)(const char *,ph7_value *,void **);    /* Open directory handle */
	void (*xClose)(void *);                                /* Close file handle */
	void (*xCloseDir)(void *);                             /* Close directory handle */
	ph7_int64 (*xRead)(void *,void *,ph7_int64);           /* Read from the open stream */         
	int (*xReadDir)(void *,ph7_context *);                 /* Read entry from directory handle */
	ph7_int64 (*xWrite)(void *,const void *,ph7_int64);    /* Write to the open stream */
	int (*xSeek)(void *,ph7_int64,int);                    /* Seek on the open stream */
	int (*xLock)(void *,int);                              /* Lock/Unlock the open stream */
	void (*xRewindDir)(void *);                            /* Rewind directory handle */
	ph7_int64 (*xTell)(void *);                            /* Current position of the stream  read/write pointer */
	int (*xTrunc)(void *,ph7_int64);                       /* Truncates the open stream to a given length */
	int (*xSync)(void *);                                  /* Flush open stream data */
	int (*xStat)(void *,ph7_value *,ph7_value *);          /* Stat an open stream handle */
};
/* 
 * C-API-REF: Please refer to the official documentation for interfaces
 * purpose and expected parameters. 
 */ 
/* Engine Handling Interfaces */
PH7_APIEXPORT int ph7_init(ph7 **ppEngine);
PH7_APIEXPORT int ph7_config(ph7 *pEngine,int nConfigOp,...);
PH7_APIEXPORT int ph7_release(ph7 *pEngine);
/* Compile Interfaces */
PH7_APIEXPORT int ph7_compile(ph7 *pEngine,const char *zSource,int nLen,ph7_vm **ppOutVm);
PH7_APIEXPORT int ph7_compile_v2(ph7 *pEngine,const char *zSource,int nLen,ph7_vm **ppOutVm,int iFlags);
PH7_APIEXPORT int ph7_compile_file(ph7 *pEngine,const char *zFilePath,ph7_vm **ppOutVm,int iFlags);
/* Virtual Machine Handling Interfaces */
PH7_APIEXPORT int ph7_vm_config(ph7_vm *pVm,int iConfigOp,...);
PH7_APIEXPORT int ph7_vm_exec(ph7_vm *pVm,int *pExitStatus);
PH7_APIEXPORT int ph7_vm_reset(ph7_vm *pVm);
PH7_APIEXPORT int ph7_vm_release(ph7_vm *pVm);
PH7_APIEXPORT int ph7_vm_dump_v2(ph7_vm *pVm,int (*xConsumer)(const void *,unsigned int,void *),void *pUserData);
/* In-process Extending Interfaces */
PH7_APIEXPORT int ph7_create_function(ph7_vm *pVm,const char *zName,int (*xFunc)(ph7_context *,int,ph7_value **),void *pUserData);
PH7_APIEXPORT int ph7_delete_function(ph7_vm *pVm,const char *zName);
PH7_APIEXPORT int ph7_create_constant(ph7_vm *pVm,const char *zName,void (*xExpand)(ph7_value *,void *),void *pUserData);
PH7_APIEXPORT int ph7_delete_constant(ph7_vm *pVm,const char *zName);
/* Foreign Function Parameter Values */
PH7_APIEXPORT int ph7_value_to_int(ph7_value *pValue);
PH7_APIEXPORT int ph7_value_to_bool(ph7_value *pValue);
PH7_APIEXPORT ph7_int64 ph7_value_to_int64(ph7_value *pValue);
PH7_APIEXPORT double ph7_value_to_double(ph7_value *pValue);
PH7_APIEXPORT const char * ph7_value_to_string(ph7_value *pValue,int *pLen);
PH7_APIEXPORT void * ph7_value_to_resource(ph7_value *pValue);
PH7_APIEXPORT int ph7_value_compare(ph7_value *pLeft,ph7_value *pRight,int bStrict);
/* Setting The Result Of A Foreign Function */
PH7_APIEXPORT int ph7_result_int(ph7_context *pCtx,int iValue);
PH7_APIEXPORT int ph7_result_int64(ph7_context *pCtx,ph7_int64 iValue);
PH7_APIEXPORT int ph7_result_bool(ph7_context *pCtx,int iBool);
PH7_APIEXPORT int ph7_result_double(ph7_context *pCtx,double Value);
PH7_APIEXPORT int ph7_result_null(ph7_context *pCtx);
PH7_APIEXPORT int ph7_result_string(ph7_context *pCtx,const char *zString,int nLen);
PH7_APIEXPORT int ph7_result_string_format(ph7_context *pCtx,const char *zFormat,...);
PH7_APIEXPORT int ph7_result_value(ph7_context *pCtx,ph7_value *pValue);
PH7_APIEXPORT int ph7_result_resource(ph7_context *pCtx,void *pUserData);
/* Call Context Handling Interfaces */
PH7_APIEXPORT int ph7_context_output(ph7_context *pCtx,const char *zString,int nLen);
PH7_APIEXPORT int ph7_context_output_format(ph7_context *pCtx,const char *zFormat,...);
PH7_APIEXPORT int ph7_context_throw_error(ph7_context *pCtx,int iErr,const char *zErr);
PH7_APIEXPORT int ph7_context_throw_error_format(ph7_context *pCtx,int iErr,const char *zFormat,...);
PH7_APIEXPORT unsigned int ph7_context_random_num(ph7_context *pCtx);
PH7_APIEXPORT int ph7_context_random_string(ph7_context *pCtx,char *zBuf,int nBuflen);
PH7_APIEXPORT void * ph7_context_user_data(ph7_context *pCtx);
PH7_APIEXPORT int    ph7_context_push_aux_data(ph7_context *pCtx,void *pUserData);
PH7_APIEXPORT void * ph7_context_peek_aux_data(ph7_context *pCtx);
PH7_APIEXPORT void * ph7_context_pop_aux_data(ph7_context *pCtx);
PH7_APIEXPORT unsigned int ph7_context_result_buf_length(ph7_context *pCtx);
PH7_APIEXPORT const char * ph7_function_name(ph7_context *pCtx);
/* Call Context Memory Management Interfaces */
PH7_APIEXPORT void * ph7_context_alloc_chunk(ph7_context *pCtx,unsigned int nByte,int ZeroChunk,int AutoRelease);
PH7_APIEXPORT void * ph7_context_realloc_chunk(ph7_context *pCtx,void *pChunk,unsigned int nByte);
PH7_APIEXPORT void ph7_context_free_chunk(ph7_context *pCtx,void *pChunk);
/* On Demand Dynamically Typed Value Object allocation interfaces */
PH7_APIEXPORT ph7_value * ph7_new_scalar(ph7_vm *pVm);
PH7_APIEXPORT ph7_value * ph7_new_array(ph7_vm *pVm);
PH7_APIEXPORT int ph7_release_value(ph7_vm *pVm,ph7_value *pValue);
PH7_APIEXPORT ph7_value * ph7_context_new_scalar(ph7_context *pCtx);
PH7_APIEXPORT ph7_value * ph7_context_new_array(ph7_context *pCtx);
PH7_APIEXPORT void ph7_context_release_value(ph7_context *pCtx,ph7_value *pValue);
/* Dynamically Typed Value Object Management Interfaces */
PH7_APIEXPORT int ph7_value_int(ph7_value *pVal,int iValue);
PH7_APIEXPORT int ph7_value_int64(ph7_value *pVal,ph7_int64 iValue);
PH7_APIEXPORT int ph7_value_bool(ph7_value *pVal,int iBool);
PH7_APIEXPORT int ph7_value_null(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_double(ph7_value *pVal,double Value);
PH7_APIEXPORT int ph7_value_string(ph7_value *pVal,const char *zString,int nLen);
PH7_APIEXPORT int ph7_value_string_format(ph7_value *pVal,const char *zFormat,...);
PH7_APIEXPORT int ph7_value_reset_string_cursor(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_resource(ph7_value *pVal,void *pUserData);
PH7_APIEXPORT int ph7_value_release(ph7_value *pVal);
PH7_APIEXPORT ph7_value * ph7_array_fetch(ph7_value *pArray,const char *zKey,int nByte);
PH7_APIEXPORT int ph7_array_walk(ph7_value *pArray,int (*xWalk)(ph7_value *,ph7_value *,void *),void *pUserData);
PH7_APIEXPORT int ph7_array_add_elem(ph7_value *pArray,ph7_value *pKey,ph7_value *pValue);
PH7_APIEXPORT int ph7_array_add_strkey_elem(ph7_value *pArray,const char *zKey,ph7_value *pValue);
PH7_APIEXPORT int ph7_array_add_intkey_elem(ph7_value *pArray,int iKey,ph7_value *pValue);
PH7_APIEXPORT unsigned int ph7_array_count(ph7_value *pArray);
PH7_APIEXPORT int ph7_object_walk(ph7_value *pObject,int (*xWalk)(const char *,ph7_value *,void *),void *pUserData);
PH7_APIEXPORT ph7_value * ph7_object_fetch_attr(ph7_value *pObject,const char *zAttr);
PH7_APIEXPORT const char * ph7_object_get_class_name(ph7_value *pObject,int *pLength);
PH7_APIEXPORT int ph7_value_is_int(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_float(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_bool(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_string(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_null(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_numeric(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_callable(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_scalar(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_array(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_object(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_resource(ph7_value *pVal);
PH7_APIEXPORT int ph7_value_is_empty(ph7_value *pVal);
/* Global Library Management Interfaces */
PH7_APIEXPORT int ph7_lib_init(void);
PH7_APIEXPORT int ph7_lib_config(int nConfigOp,...);
PH7_APIEXPORT int ph7_lib_shutdown(void);
PH7_APIEXPORT int ph7_lib_is_threadsafe(void);
PH7_APIEXPORT const char * ph7_lib_version(void);
PH7_APIEXPORT const char * ph7_lib_signature(void);
PH7_APIEXPORT const char * ph7_lib_ident(void);
PH7_APIEXPORT const char * ph7_lib_copyright(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _PH7_H_ */

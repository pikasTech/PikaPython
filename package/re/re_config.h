

/* Define to 1 if you have the `bcopy' function. */
#ifndef HAVE_BCOPY
#define HAVE_BCOPY 0
#endif

/* Define to 1 if you have the <bits/type_traits.h> header file. */
/* #undef HAVE_BITS_TYPE_TRAITS_H */

/* Define to 1 if you have the <bzlib.h> header file. */
#ifndef HAVE_BZLIB_H
#define HAVE_BZLIB_H 0
#endif

/* Define to 1 if you have the <dirent.h> header file. */
#ifndef HAVE_DIRENT_H
#define HAVE_DIRENT_H 0
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef HAVE_DLFCN_H
#define HAVE_DLFCN_H 0
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef HAVE_INTTYPES_H
#define HAVE_INTTYPES_H 1
#endif

/* Define to 1 if you have the <limits.h> header file. */
#ifndef HAVE_LIMITS_H
#define HAVE_LIMITS_H 1
#endif

/* Define to 1 if the system has the type `long long'. */
#ifndef HAVE_LONG_LONG
#define HAVE_LONG_LONG 1
#endif

/* Define to 1 if you have the `memmove' function. */
#ifndef HAVE_MEMMOVE
#define HAVE_MEMMOVE 1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef HAVE_MEMORY_H
#define HAVE_MEMORY_H 1
#endif

/* Define to 1 if you have the <readline/history.h> header file. */
#ifndef HAVE_READLINE_HISTORY_H
#define HAVE_READLINE_HISTORY_H 0
#endif

/* Define to 1 if you have the <readline/readline.h> header file. */
#ifndef HAVE_READLINE_READLINE_H
#define HAVE_READLINE_READLINE_H 0
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef HAVE_STDINT_H
#define HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the `strerror' function. */
#ifndef HAVE_STRERROR
#define HAVE_STRERROR 1
#endif

/* Define to 1 if you have the <string> header file. */
#ifndef HAVE_STRING
#define HAVE_STRING 0
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef HAVE_STRINGS_H
#define HAVE_STRINGS_H 0
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef HAVE_STRING_H
#define HAVE_STRING_H 1
#endif

/* Define to 1 if you have the `strtoll' function. */
#ifndef HAVE_STRTOLL
#define HAVE_STRTOLL 1
#endif

/* Define to 1 if you have the `strtoq' function. */
#ifndef HAVE_STRTOQ
#define HAVE_STRTOQ 0
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef HAVE_SYS_STAT_H
#define HAVE_SYS_STAT_H 0
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef HAVE_SYS_TYPES_H
#define HAVE_SYS_TYPES_H 0
#endif

/* Define to 1 if you have the <type_traits.h> header file. */
/* #undef HAVE_TYPE_TRAITS_H */

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 0
#endif

/* Define to 1 if the system has the type `unsigned long long'. */
#ifndef HAVE_UNSIGNED_LONG_LONG
#define HAVE_UNSIGNED_LONG_LONG 1
#endif

/* Define to 1 if you have the <windows.h> header file. */
/* #undef HAVE_WINDOWS_H */

/* Define to 1 if you have the <zlib.h> header file. */
#ifndef HAVE_ZLIB_H
#define HAVE_ZLIB_H 0
#endif

/* Define to 1 if you have the `_strtoi64' function. */
/* #undef HAVE__STRTOI64 */

/* The value of LINK_SIZE determines the number of bytes used to store links
   as offsets within the compiled regex. The default is 2, which allows for
   compiled patterns up to 64K long. This covers the vast majority of cases.
   However, PCRE can also be compiled to use 3 or 4 bytes instead. This allows
   for longer patterns in extreme cases. On systems that support it,
   "configure" can be used to override this default. */
#ifndef LINK_SIZE
#define LINK_SIZE 2
#endif

/* The value of MATCH_LIMIT determines the default number of times the
	  internal match() function can be called during a single execution of
	  pcre_exec(). There is a runtime interface for setting a different limit.
	  The limit exists in order to catch runaway regular expressions that take
	  for ever to determine that they do not match. The default is set very large
	  so that it does not accidentally catch legitimate cases. On systems that
	  support it, "configure" can be used to override this default default. */
#ifndef MATCH_LIMIT
#define MATCH_LIMIT 10000000
#endif

/* The above limit applies to all calls of match(), whether or not they
		 increase the recursion depth. In some environments it is desirable to limit
		 the depth of recursive calls of match() more strictly, in order to restrict
		 the maximum amount of stack (or heap, if NO_RECURSE is defined) that is
		 used. The value of MATCH_LIMIT_RECURSION applies only to recursive calls of
		 match(). To have any useful effect, it must be less than the value of
		 MATCH_LIMIT. The default is to use the same value as MATCH_LIMIT. There is
		 a runtime method for setting a different limit. On systems that support it,
		 "configure" can be used to override the default. */
#ifndef MATCH_LIMIT_RECURSION
#define MATCH_LIMIT_RECURSION MATCH_LIMIT
#endif

/* This limit is parameterized just in case anybody ever wants to change it.
			Care must be taken if it is increased, because it guards against integer
			overflow caused by enormously large patterns. */
#ifndef MAX_NAME_COUNT
#define MAX_NAME_COUNT 10000
#endif

/* This limit is parameterized just in case anybody ever wants to change it.
			   Care must be taken if it is increased, because it guards against integer
			   overflow caused by enormously large patterns. */
#ifndef MAX_NAME_SIZE
#define MAX_NAME_SIZE 32
#endif

/* The value of NEWLINE determines the newline character sequence. On systems
				  that support it, "configure" can be used to override the default, which is
				  10. The possible values are 10 (LF), 13 (CR), 3338 (CRLF), -1 (ANY), or -2
				  (ANYCRLF). */
#ifndef NEWLINE
#define NEWLINE 10
#endif

/* When calling PCRE via the POSIX interface, additional working storage is
	  required for holding the pointers to capturing substrings because PCRE
	  requires three integers per substring, whereas the POSIX interface provides
	  only two. If the number of expected substrings is small, the wrapper
	  function uses space on the stack, because this is faster than using
	  malloc() for each call. The threshold above which the stack is no longer
	  used is defined by POSIX_MALLOC_THRESHOLD. On systems that support it,
	  "configure" can be used to override this default. */
#ifndef POSIX_MALLOC_THRESHOLD
#define POSIX_MALLOC_THRESHOLD 10
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef STDC_HEADERS
#define STDC_HEADERS 1
#endif

/* Define to enable support for the UTF-8 Unicode encoding. */
/* #undef SUPPORT_UTF8 */
#define SUPPORT_UTF8


/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

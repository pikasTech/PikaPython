
/* This module contains global variables that are exported by the PCRE library.
PCRE is thread-clean and doesn't use any global variables in the normal sense.
However, it calls memory allocation and freeing functions via the four
indirections below, and it can optionally do callouts, using the fifth
indirection. These values can be changed by the caller, but are shared between
all threads. However, when compiling for Virtual Pascal, things are done
differently, and global variables are not used (see pcre.in). */

#include "re_config.h"
#include "pcre_internal.h"

#ifndef VPCOMPAT
void *(*pcre_malloc)(size_t) = malloc;
void  (*pcre_free)(void *) = free;
void *(*pcre_stack_malloc)(size_t) = malloc;
void  (*pcre_stack_free)(void *) = free;
int   (*pcre_callout)(pcre_callout_block *) = NULL;
#endif

/* End of pcre_globals.c */

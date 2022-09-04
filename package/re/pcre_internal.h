
#ifndef PCRE_INTERNAL_H
#define PCRE_INTERNAL_H


#if 0
#define DEBUG
#endif

#undef DPRINTF
#ifdef DEBUG
#define DPRINTF(p) printf p
#else
#define DPRINTF(p)
#endif


#include <ctype.h>
#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if USHRT_MAX == 65535
  typedef unsigned short pcre_uint16;
#elif UINT_MAX == 65535
  typedef unsigned int pcre_uint16;
#else
  #error Cannot determine a type for 16-bit unsigned integers
#endif

#if UINT_MAX == 4294967295
  typedef unsigned int pcre_uint32;
#elif ULONG_MAX == 4294967295
  typedef unsigned long int pcre_uint32;
#else
  #error Cannot determine a type for 32-bit unsigned integers
#endif


typedef unsigned char uschar;


#define NOTACHAR 0xffffffff


#define NLTYPE_FIXED    0
#define NLTYPE_ANY      1
#define NLTYPE_ANYCRLF  2


#define IS_NEWLINE(p) \
  ((NLBLOCK->nltype != NLTYPE_FIXED)? \
    ((p) < NLBLOCK->PSEND && \
     _pcre_is_newline((p), NLBLOCK->nltype, NLBLOCK->PSEND, &(NLBLOCK->nllen),\
       utf8)) \
    : \
    ((p) <= NLBLOCK->PSEND - NLBLOCK->nllen && \
     (p)[0] == NLBLOCK->nl[0] && \
     (NLBLOCK->nllen == 1 || (p)[1] == NLBLOCK->nl[1]) \
    ) \
  )


#define WAS_NEWLINE(p) \
  ((NLBLOCK->nltype != NLTYPE_FIXED)? \
    ((p) > NLBLOCK->PSSTART && \
     _pcre_was_newline((p), NLBLOCK->nltype, NLBLOCK->PSSTART, \
       &(NLBLOCK->nllen), utf8)) \
    : \
    ((p) >= NLBLOCK->PSSTART + NLBLOCK->nllen && \
     (p)[-NLBLOCK->nllen] == NLBLOCK->nl[0] && \
     (NLBLOCK->nllen == 1 || (p)[-NLBLOCK->nllen+1] == NLBLOCK->nl[1]) \
    ) \
  )


#ifdef CUSTOM_SUBJECT_PTR
#define PCRE_SPTR CUSTOM_SUBJECT_PTR
#define USPTR CUSTOM_SUBJECT_PTR
#else
#define PCRE_SPTR const char *
#define USPTR const unsigned char *
#endif


#include "pcre.h"


#ifdef VPCOMPAT
#define strlen(s)        _strlen(s)
#define strncmp(s1,s2,m) _strncmp(s1,s2,m)
#define memcmp(s,c,n)    _memcmp(s,c,n)
#define memcpy(d,s,n)    _memcpy(d,s,n)
#define memmove(d,s,n)   _memmove(d,s,n)
#define memset(s,c,n)    _memset(s,c,n)
#else


#ifndef HAVE_MEMMOVE
#undef  memmove
#ifdef HAVE_BCOPY
#define memmove(a, b, c) bcopy(b, a, c)
#else
static void *
pcre_memmove(void *d, const void *s, size_t n)
{
size_t i;
unsigned char *dest = (unsigned char *)d;
const unsigned char *src = (const unsigned char *)s;
if (dest > src)
  {
  dest += n;
  src += n;
  for (i = 0; i < n; ++i) *(--dest) = *(--src);
  return (void *)dest;
  }
else
  {
  for (i = 0; i < n; ++i) *dest++ = *src++;
  return (void *)(dest - n);
  }
}
#define memmove(a, b, c) pcre_memmove(a, b, c)
#endif
#endif
#endif


#if LINK_SIZE == 2

#define PUT(a,n,d)   \
  (a[n] = (d) >> 8), \
  (a[(n)+1] = (d) & 255)

#define GET(a,n) \
  (((a)[n] << 8) | (a)[(n)+1])

#define MAX_PATTERN_SIZE (1 << 16)


#elif LINK_SIZE == 3

#define PUT(a,n,d)       \
  (a[n] = (d) >> 16),    \
  (a[(n)+1] = (d) >> 8), \
  (a[(n)+2] = (d) & 255)

#define GET(a,n) \
  (((a)[n] << 16) | ((a)[(n)+1] << 8) | (a)[(n)+2])

#define MAX_PATTERN_SIZE (1 << 24)


#elif LINK_SIZE == 4

#define PUT(a,n,d)        \
  (a[n] = (d) >> 24),     \
  (a[(n)+1] = (d) >> 16), \
  (a[(n)+2] = (d) >> 8),  \
  (a[(n)+3] = (d) & 255)

#define GET(a,n) \
  (((a)[n] << 24) | ((a)[(n)+1] << 16) | ((a)[(n)+2] << 8) | (a)[(n)+3])

#define MAX_PATTERN_SIZE (1 << 30)


#else
#error LINK_SIZE must be either 2, 3, or 4
#endif


#define PUTINC(a,n,d)   PUT(a,n,d), a += LINK_SIZE


#define PUT2(a,n,d)   \
  a[n] = (d) >> 8; \
  a[(n)+1] = (d) & 255

#define GET2(a,n) \
  (((a)[n] << 8) | (a)[(n)+1])

#define PUT2INC(a,n,d)  PUT2(a,n,d), a += 2


#ifndef SUPPORT_UTF8
#define NEXTCHAR(p) p++;
#define GETCHAR(c, eptr) c = *eptr;
#define GETCHARTEST(c, eptr) c = *eptr;
#define GETCHARINC(c, eptr) c = *eptr++;
#define GETCHARINCTEST(c, eptr) c = *eptr++;
#define GETCHARLEN(c, eptr, len) c = *eptr;


#else


#define NEXTCHAR(p) \
  p++; \
  if (utf8) { while((*p & 0xc0) == 0x80) p++; }


#define GETCHAR(c, eptr) \
  c = *eptr; \
  if (c >= 0xc0) \
    { \
    int gcii; \
    int gcaa = _pcre_utf8_table4[c & 0x3f];   \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    for (gcii = 1; gcii <= gcaa; gcii++) \
      { \
      gcss -= 6; \
      c |= (eptr[gcii] & 0x3f) << gcss; \
      } \
    }


#define GETCHARTEST(c, eptr) \
  c = *eptr; \
  if (utf8 && c >= 0xc0) \
    { \
    int gcii; \
    int gcaa = _pcre_utf8_table4[c & 0x3f];   \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    for (gcii = 1; gcii <= gcaa; gcii++) \
      { \
      gcss -= 6; \
      c |= (eptr[gcii] & 0x3f) << gcss; \
      } \
    }


#define GETCHARINC(c, eptr) \
  c = *eptr++; \
  if (c >= 0xc0) \
    { \
    int gcaa = _pcre_utf8_table4[c & 0x3f];   \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    while (gcaa-- > 0) \
      { \
      gcss -= 6; \
      c |= (*eptr++ & 0x3f) << gcss; \
      } \
    }


#define GETCHARINCTEST(c, eptr) \
  c = *eptr++; \
  if (utf8 && c >= 0xc0) \
    { \
    int gcaa = _pcre_utf8_table4[c & 0x3f];   \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    while (gcaa-- > 0) \
      { \
      gcss -= 6; \
      c |= (*eptr++ & 0x3f) << gcss; \
      } \
    }


#define GETCHARLEN(c, eptr, len) \
  c = *eptr; \
  if (c >= 0xc0) \
    { \
    int gcii; \
    int gcaa = _pcre_utf8_table4[c & 0x3f];   \
    int gcss = 6*gcaa; \
    c = (c & _pcre_utf8_table3[gcaa]) << gcss; \
    for (gcii = 1; gcii <= gcaa; gcii++) \
      { \
      gcss -= 6; \
      c |= (eptr[gcii] & 0x3f) << gcss; \
      } \
    len += gcaa; \
    }


#define BACKCHAR(eptr) while((*eptr & 0xc0) == 0x80) eptr--

#endif


#ifndef offsetof
#define offsetof(p_type,field) ((size_t)&(((p_type *)0)->field))
#endif


#define PCRE_IMS (PCRE_CASELESS|PCRE_MULTILINE|PCRE_DOTALL)


#define PCRE_NOPARTIAL     0x0001
#define PCRE_FIRSTSET      0x0002
#define PCRE_REQCHSET      0x0004
#define PCRE_STARTLINE     0x0008
#define PCRE_JCHANGED      0x0010
#define PCRE_HASCRORLF     0x0020


#define PCRE_STUDY_MAPPED   0x01


#define PCRE_NEWLINE_BITS (PCRE_NEWLINE_CR|PCRE_NEWLINE_LF|PCRE_NEWLINE_ANY| \
                           PCRE_NEWLINE_ANYCRLF)

#define PUBLIC_OPTIONS \
  (PCRE_CASELESS|PCRE_EXTENDED|PCRE_ANCHORED|PCRE_MULTILINE| \
   PCRE_DOTALL|PCRE_DOLLAR_ENDONLY|PCRE_EXTRA|PCRE_UNGREEDY|PCRE_UTF8| \
   PCRE_NO_AUTO_CAPTURE|PCRE_NO_UTF8_CHECK|PCRE_AUTO_CALLOUT|PCRE_FIRSTLINE| \
   PCRE_DUPNAMES|PCRE_NEWLINE_BITS|PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE)

#define PUBLIC_EXEC_OPTIONS \
  (PCRE_ANCHORED|PCRE_NOTBOL|PCRE_NOTEOL|PCRE_NOTEMPTY|PCRE_NO_UTF8_CHECK| \
   PCRE_PARTIAL|PCRE_NEWLINE_BITS|PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE)

#define PUBLIC_DFA_EXEC_OPTIONS \
  (PCRE_ANCHORED|PCRE_NOTBOL|PCRE_NOTEOL|PCRE_NOTEMPTY|PCRE_NO_UTF8_CHECK| \
   PCRE_PARTIAL|PCRE_DFA_SHORTEST|PCRE_DFA_RESTART|PCRE_NEWLINE_BITS| \
   PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE)

#define PUBLIC_STUDY_OPTIONS 0


#define MAGIC_NUMBER  0x50435245UL


#define REQ_UNSET (-2)
#define REQ_NONE  (-1)


#define REQ_BYTE_MAX 1000

/* Flags added to firstbyte or reqbyte; a "non-literal" item is either a
variable-length repeat, or a anything other than literal characters. */

#define REQ_CASELESS 0x0100
#define REQ_VARY     0x0200


typedef int BOOL;

#define FALSE   0
#define TRUE    1


#ifndef ESC_e
#define ESC_e 27
#endif

#ifndef ESC_f
#define ESC_f '\f'
#endif

#ifndef ESC_n
#define ESC_n '\n'
#endif

#ifndef ESC_r
#define ESC_r '\r'
#endif


#ifndef ESC_tee
#define ESC_tee '\t'
#endif


#define PT_ANY        0
#define PT_LAMP       1
#define PT_GC         2
#define PT_PC         3
#define PT_SC         4


#define XCL_NOT    0x01
#define XCL_MAP    0x02

#define XCL_END       0
#define XCL_SINGLE    1
#define XCL_RANGE     2
#define XCL_PROP      3
#define XCL_NOTPROP   4

/* These are escaped items that aren't just an encoding of a particular data
value such as \n. They must have non-zero values, as check_escape() returns
their negation. Also, they must appear in the same order as in the opcode
definitions below, up to ESC_z. There's a dummy for OP_ANY because it
corresponds to "." rather than an escape sequence. The final one must be
ESC_REF as subsequent values are used for backreferences (\1, \2, \3, etc).
There are two tests in the code for an escape greater than ESC_b and less than
ESC_Z to detect the types that may be repeated. These are the types that
consume characters. If any new escapes are put in between that don't consume a
character, that code will have to change. */

enum { ESC_A = 1, ESC_G, ESC_K, ESC_B, ESC_b, ESC_D, ESC_d, ESC_S, ESC_s,
       ESC_W, ESC_w, ESC_dum1, ESC_C, ESC_P, ESC_p, ESC_R, ESC_H, ESC_h,
       ESC_V, ESC_v, ESC_X, ESC_Z, ESC_z, ESC_E, ESC_Q, ESC_k, ESC_REF };


/* Opcode table: Starting from 1 (i.e. after OP_END), the values up to
OP_EOD must correspond in order to the list of escapes immediately above.

*** NOTE NOTE NOTE *** Whenever this list is updated, the two macro definitions
that follow must also be updated to match. There is also a table called
"coptable" in pcre_dfa_exec.c that must be updated. */

enum {
  OP_END,



  OP_SOD,
  OP_SOM,
  OP_SET_SOM,
  OP_NOT_WORD_BOUNDARY,
  OP_WORD_BOUNDARY,
  OP_NOT_DIGIT,
  OP_DIGIT,
  OP_NOT_WHITESPACE,
  OP_WHITESPACE,
  OP_NOT_WORDCHAR,
  OP_WORDCHAR,
  OP_ANY,
  OP_ANYBYTE,
  OP_NOTPROP,
  OP_PROP,
  OP_ANYNL,
  OP_NOT_HSPACE,
  OP_HSPACE,
  OP_NOT_VSPACE,
  OP_VSPACE,
  OP_EXTUNI,
  OP_EODN,
  OP_EOD,

  OP_OPT,
  OP_CIRC,
  OP_DOLL,
  OP_CHAR,
  OP_CHARNC,
  OP_NOT,

  OP_STAR,
  OP_MINSTAR,
  OP_PLUS,
  OP_MINPLUS,
  OP_QUERY,
  OP_MINQUERY,

  OP_UPTO,
  OP_MINUPTO,
  OP_EXACT,

  OP_POSSTAR,
  OP_POSPLUS,
  OP_POSQUERY,
  OP_POSUPTO,

  OP_NOTSTAR,
  OP_NOTMINSTAR,
  OP_NOTPLUS,
  OP_NOTMINPLUS,
  OP_NOTQUERY,
  OP_NOTMINQUERY,

  OP_NOTUPTO,
  OP_NOTMINUPTO,
  OP_NOTEXACT,

  OP_NOTPOSSTAR,
  OP_NOTPOSPLUS,
  OP_NOTPOSQUERY,
  OP_NOTPOSUPTO,

  OP_TYPESTAR,
  OP_TYPEMINSTAR,
  OP_TYPEPLUS,
  OP_TYPEMINPLUS,
  OP_TYPEQUERY,
  OP_TYPEMINQUERY,

  OP_TYPEUPTO,
  OP_TYPEMINUPTO,
  OP_TYPEEXACT,

  OP_TYPEPOSSTAR,
  OP_TYPEPOSPLUS,
  OP_TYPEPOSQUERY,
  OP_TYPEPOSUPTO,

  OP_CRSTAR,
  OP_CRMINSTAR,
  OP_CRPLUS,
  OP_CRMINPLUS,
  OP_CRQUERY,
  OP_CRMINQUERY,
  OP_CRRANGE,
  OP_CRMINRANGE,

  OP_CLASS,
  OP_NCLASS,         /* 78 Same, but the bitmap was created from a negative
                           class - the difference is relevant only when a UTF-8
                           character > 255 is encountered. */

  OP_XCLASS,

  OP_REF,
  OP_RECURSE,
  OP_CALLOUT,

  OP_ALT,
  OP_KET,
  OP_KETRMAX,
  OP_KETRMIN,



  OP_ASSERT,
  OP_ASSERT_NOT,
  OP_ASSERTBACK,
  OP_ASSERTBACK_NOT,
  OP_REVERSE,

  /* ONCE, BRA, CBRA, and COND must come after the assertions, with ONCE first,
  as there's a test for >= ONCE for a subpattern that isn't an assertion. */

  OP_ONCE,
  OP_BRA,
  OP_CBRA,
  OP_COND,

  /* These three must follow the previous three, in the same order. There's a
  check for >= SBRA to distinguish the two sets. */

  OP_SBRA,
  OP_SCBRA,
  OP_SCOND,

  OP_CREF,
  OP_RREF,
  OP_DEF,

  OP_BRAZERO,
  OP_BRAMINZERO,



  OP_PRUNE,
  OP_SKIP,
  OP_THEN,
  OP_COMMIT,



  OP_FAIL,
  OP_ACCEPT
};


#define OP_NAME_LIST \
  "End", "\\A", "\\G", "\\K", "\\B", "\\b", "\\D", "\\d",         \
  "\\S", "\\s", "\\W", "\\w", "Any", "Anybyte",                   \
  "notprop", "prop", "\\R", "\\H", "\\h", "\\V", "\\v",           \
  "extuni",  "\\Z", "\\z",                                        \
  "Opt", "^", "$", "char", "charnc", "not",                       \
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",                 \
  "*+","++", "?+", "{",                                           \
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",                 \
  "*+","++", "?+", "{",                                           \
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",                 \
  "*+","++", "?+", "{",                                           \
  "*", "*?", "+", "+?", "?", "??", "{", "{",                      \
  "class", "nclass", "xclass", "Ref", "Recurse", "Callout",       \
  "Alt", "Ket", "KetRmax", "KetRmin", "Assert", "Assert not",     \
  "AssertB", "AssertB not", "Reverse",                            \
  "Once", "Bra", "CBra", "Cond", "SBra", "SCBra", "SCond",        \
  "Cond ref", "Cond rec", "Cond def", "Brazero", "Braminzero",    \
  "*PRUNE", "*SKIP", "*THEN", "*COMMIT", "*FAIL", "*ACCEPT"


#define OP_LENGTHS \
  1,                              \
  1, 1, 1, 1, 1,                  \
  1, 1, 1, 1, 1, 1,               \
  1, 1,                           \
  3, 3, 1,                        \
  1, 1, 1, 1, 1,                  \
  1, 1, 2, 1, 1,                  \
  2,                              \
  2,                              \
  2,                              \
   \
  2, 2, 2, 2, 2, 2,               \
  4, 4, 4,                        \
  2, 2, 2, 4,                     \
   \
  2, 2, 2, 2, 2, 2,               \
  4, 4, 4,                        \
  2, 2, 2, 4,                     \
   \
  2, 2, 2, 2, 2, 2,               \
  4, 4, 4,                        \
  2, 2, 2, 4,                     \
   \
  1, 1, 1, 1, 1, 1,               \
  5, 5,                           \
 33,                              \
 33,                              \
  0,                              \
  3,                              \
  1+LINK_SIZE,                    \
  2+2*LINK_SIZE,                  \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  3+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  3+LINK_SIZE,                    \
  1+LINK_SIZE,                    \
  3,                              \
  3,                              \
  1,                              \
  1, 1,                           \
  1, 1, 1, 1,                     \
  1, 1


#define RREF_ANY  0xffff


enum { ERR0,  ERR1,  ERR2,  ERR3,  ERR4,  ERR5,  ERR6,  ERR7,  ERR8,  ERR9,
       ERR10, ERR11, ERR12, ERR13, ERR14, ERR15, ERR16, ERR17, ERR18, ERR19,
       ERR20, ERR21, ERR22, ERR23, ERR24, ERR25, ERR26, ERR27, ERR28, ERR29,
       ERR30, ERR31, ERR32, ERR33, ERR34, ERR35, ERR36, ERR37, ERR38, ERR39,
       ERR40, ERR41, ERR42, ERR43, ERR44, ERR45, ERR46, ERR47, ERR48, ERR49,
       ERR50, ERR51, ERR52, ERR53, ERR54, ERR55, ERR56, ERR57, ERR58, ERR59,
       ERR60, ERR61, ERR62, ERR63 };

/* The real format of the start of the pcre block; the index of names and the
code vector run on as long as necessary after the end. We store an explicit
offset to the name table so that if a regex is compiled on one host, saved, and
then run on another where the size of pointers is different, all might still
be well. For the case of compiled-on-4 and run-on-8, we include an extra
pointer that is always NULL. For future-proofing, a few dummy fields were
originally included - even though you can never get this planning right - but
there is only one left now.

NOTE NOTE NOTE:
Because people can now save and re-use compiled patterns, any additions to this
structure should be made at the end, and something earlier (e.g. a new
flag in the options or one of the dummy fields) should indicate that the new
fields are present. Currently PCRE always sets the dummy fields to zero.
NOTE NOTE NOTE:
*/

typedef struct real_pcre {
  pcre_uint32 magic_number;
  pcre_uint32 size;
  pcre_uint32 options;
  pcre_uint16 flags;
  pcre_uint16 dummy1;
  pcre_uint16 top_bracket;
  pcre_uint16 top_backref;
  pcre_uint16 first_byte;
  pcre_uint16 req_byte;
  pcre_uint16 name_table_offset;
  pcre_uint16 name_entry_size;
  pcre_uint16 name_count;
  pcre_uint16 ref_count;

  const unsigned char *tables;
  const unsigned char *nullpad;
} real_pcre;


typedef struct pcre_study_data {
  pcre_uint32 size;
  pcre_uint32 options;
  uschar start_bits[32];
} pcre_study_data;


typedef struct compile_data {
  const uschar *lcc;
  const uschar *fcc;
  const uschar *cbits;
  const uschar *ctypes;
  const uschar *start_workspace;
  const uschar *start_code;
  const uschar *start_pattern;
  const uschar *end_pattern;
  uschar *hwm;
  uschar *name_table;
  int  names_found;
  int  name_entry_size;
  int  bracount;
  int  final_bracount;
  int  top_backref;
  unsigned int backref_map;
  int  external_options;
  int  external_flags;
  int  req_varyopt;
  BOOL had_accept;
  int  nltype;
  int  nllen;
  uschar nl[4];
} compile_data;


typedef struct branch_chain {
  struct branch_chain *outer;
  uschar *current;
} branch_chain;


typedef struct recursion_info {
  struct recursion_info *prevrec;
  int group_num;
  const uschar *after_call;
  USPTR save_start;
  int *offset_save;
  int saved_max;
} recursion_info;


typedef struct eptrblock {
  struct eptrblock *epb_prev;
  USPTR epb_saved_eptr;
} eptrblock;


typedef struct match_data {
  unsigned long int match_call_count;
  unsigned long int match_limit;
  unsigned long int match_limit_recursion;
  int   *offset_vector;
  int    offset_end;
  int    offset_max;
  int    nltype;
  int    nllen;
  uschar nl[4];
  const uschar *lcc;
  const uschar *ctypes;
  BOOL   offset_overflow;
  BOOL   notbol;
  BOOL   noteol;
  BOOL   utf8;
  BOOL   endonly;
  BOOL   notempty;
  BOOL   partial;
  BOOL   hitend;
  BOOL   bsr_anycrlf;
  const uschar *start_code;
  USPTR  start_subject;
  USPTR  end_subject;
  USPTR  start_match_ptr;
  USPTR  end_match_ptr;
  int    end_offset_top;
  int    capture_last;
  int    start_offset;
  eptrblock *eptrchain;
  int    eptrn;
  recursion_info *recursive;
  void  *callout_data;
} match_data;


typedef struct dfa_match_data {
  const uschar *start_code;
  const uschar *start_subject;
  const uschar *end_subject;
  const uschar *tables;
  int   moptions;
  int   poptions;
  int    nltype;
  int    nllen;
  uschar nl[4];
  void  *callout_data;
} dfa_match_data;


#define ctype_space   0x01
#define ctype_letter  0x02
#define ctype_digit   0x04
#define ctype_xdigit  0x08
#define ctype_word    0x10
#define ctype_meta    0x80


#define cbit_space     0
#define cbit_xdigit   32
#define cbit_digit    64
#define cbit_upper    96
#define cbit_lower   128
#define cbit_word    160
#define cbit_graph   192
#define cbit_print   224
#define cbit_punct   256
#define cbit_cntrl   288
#define cbit_length  320


#define lcc_offset      0
#define fcc_offset    256
#define cbits_offset  512
#define ctypes_offset (cbits_offset + cbit_length)
#define tables_length (ctypes_offset + 256)


typedef struct {
  pcre_uint16 name_offset;
  pcre_uint16 type;
  pcre_uint16 value;
} ucp_type_table;


extern const int    _pcre_utf8_table1[];
extern const int    _pcre_utf8_table2[];
extern const int    _pcre_utf8_table3[];
extern const uschar _pcre_utf8_table4[];

extern const int    _pcre_utf8_table1_size;

extern const ucp_type_table _pcre_utt[];
extern const int _pcre_utt_size;

extern const uschar _pcre_default_tables[];

extern const uschar _pcre_OP_lengths[];


extern BOOL         _pcre_is_newline(const uschar *, int, const uschar *,
                      int *, BOOL);
extern int          _pcre_ord2utf8(int, uschar *);
extern real_pcre   *_pcre_try_flipped(const real_pcre *, real_pcre *,
                      const pcre_study_data *, pcre_study_data *);
extern int          _pcre_valid_utf8(const uschar *, int);
extern BOOL         _pcre_was_newline(const uschar *, int, const uschar *,
                      int *, BOOL);
extern BOOL         _pcre_xclass(int, const uschar *);

#endif



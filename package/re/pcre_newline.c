#include "re_config.h"
#include "pcre_internal.h"



/*************************************************
*      Check for newline at given position       *
*************************************************/

/* It is guaranteed that the initial value of ptr is less than the end of the
string that is being processed.

Arguments:
  ptr          pointer to possible newline
  type         the newline type
  endptr       pointer to the end of the string
  lenptr       where to return the length
  utf8         TRUE if in utf8 mode

Returns:       TRUE or FALSE
*/

BOOL
_pcre_is_newline(const uschar *ptr, int type, const uschar *endptr,
  int *lenptr, BOOL utf8)
{
int c;
if (utf8) { GETCHAR(c, ptr); } else c = *ptr;

if (type == NLTYPE_ANYCRLF) switch(c)
  {
  case 0x000a: *lenptr = 1; return TRUE;             /* LF */
  case 0x000d: *lenptr = (ptr < endptr - 1 && ptr[1] == 0x0a)? 2 : 1;
               return TRUE;                          /* CR */
  default: return FALSE;
  }

/* NLTYPE_ANY */

else switch(c)
  {
  case 0x000a:                                       /* LF */
  case 0x000b:                                       /* VT */
  case 0x000c: *lenptr = 1; return TRUE;             /* FF */
  case 0x000d: *lenptr = (ptr < endptr - 1 && ptr[1] == 0x0a)? 2 : 1;
               return TRUE;                          /* CR */
  case 0x0085: *lenptr = utf8? 2 : 1; return TRUE;   /* NEL */
  case 0x2028:                                       /* LS */
  case 0x2029: *lenptr = 3; return TRUE;             /* PS */
  default: return FALSE;
  }
}



/*************************************************
*     Check for newline at previous position     *
*************************************************/

/* It is guaranteed that the initial value of ptr is greater than the start of
the string that is being processed.

Arguments:
  ptr          pointer to possible newline
  type         the newline type
  startptr     pointer to the start of the string
  lenptr       where to return the length
  utf8         TRUE if in utf8 mode

Returns:       TRUE or FALSE
*/

BOOL
_pcre_was_newline(const uschar *ptr, int type, const uschar *startptr,
  int *lenptr, BOOL utf8)
{
int c;
ptr--;
#ifdef SUPPORT_UTF8
if (utf8)
  {
  BACKCHAR(ptr);
  GETCHAR(c, ptr);
  }
else c = *ptr;
#else   /* no UTF-8 support */
c = *ptr;
#endif  /* SUPPORT_UTF8 */

if (type == NLTYPE_ANYCRLF) switch(c)
  {
  case 0x000a: *lenptr = (ptr > startptr && ptr[-1] == 0x0d)? 2 : 1;
               return TRUE;                         /* LF */
  case 0x000d: *lenptr = 1; return TRUE;            /* CR */
  default: return FALSE;
  }

else switch(c)
  {
  case 0x000a: *lenptr = (ptr > startptr && ptr[-1] == 0x0d)? 2 : 1;
               return TRUE;                         /* LF */
  case 0x000b:                                      /* VT */
  case 0x000c:                                      /* FF */
  case 0x000d: *lenptr = 1; return TRUE;            /* CR */
  case 0x0085: *lenptr = utf8? 2 : 1; return TRUE;  /* NEL */
  case 0x2028:                                      /* LS */
  case 0x2029: *lenptr = 3; return TRUE;            /* PS */
  default: return FALSE;
  }
}

/* End of pcre_newline.c */

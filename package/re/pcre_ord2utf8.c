#include "re_config.h"
#include "pcre_internal.h"


/*************************************************
*       Convert character value to UTF-8         *
*************************************************/

/* This function takes an integer value in the range 0 - 0x7fffffff
and encodes it as a UTF-8 character in 0 to 6 bytes.

Arguments:
  cvalue     the character value
  buffer     pointer to buffer for result - at least 6 bytes long

Returns:     number of characters placed in the buffer
*/

int
_pcre_ord2utf8(int cvalue, uschar *buffer)
{
#ifdef SUPPORT_UTF8
register int i, j;
for (i = 0; i < _pcre_utf8_table1_size; i++)
  if (cvalue <= _pcre_utf8_table1[i]) break;
buffer += i;
for (j = i; j > 0; j--)
 {
 *buffer-- = 0x80 | (cvalue & 0x3f);
 cvalue >>= 6;
 }
*buffer = _pcre_utf8_table2[i] | cvalue;
return i + 1;
#else
return 0;   /* Keep compiler happy; this function won't ever be */
#endif      /* called when SUPPORT_UTF8 is not defined. */
}

/* End of pcre_ord2utf8.c */

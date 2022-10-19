#include <ctype.h>

#if PIKA_STRING_UTF8_ENABLE

int _valid_utf8(const char *string, int length);
int _utf8_get(const char *string, int length, int at, char *out_buf);
int _utf8_get_offset(const char *string, int length, int at, int *out_char_len);
int _utf8_strlen(const char *string, int length);
int __str_repl(PikaObj *self, char *str, int str_len, int repl_at, int repl_len, char *val, int val_len);

int __utf8_to_utf32_LE_withBOM(const char *utf8, int len, char *out_buf);
int __utf8_to_utf32_LE_noBOM(const char *utf8, int len, char *out_buf);
int __utf8_to_utf32_LE_noBOM_get_size(const char *utf8, int len);

int __utf8_to_utf16_LE_withBOM(const char *utf8, int len, char *out_buf);
int __utf8_to_utf16_LE_noBOM(const char *utf8, int len, char *out_buf);
int __utf8_to_utf16_LE_noBOM_get_size(const char *utf8, int len);
Arg* _str_encode(char*str,  char*encoding);

#endif

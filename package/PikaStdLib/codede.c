#include "PikaStdData_String.h"
#include "PikaStdData_String_Util.h"
#include "dataStrs.h"

#if PIKA_STRING_UTF8_ENABLE

static const uint8_t _pcre_utf8_table4[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

const char mask1 = 0x80;
const char mask2 = 0xc0;
const char mask3 = 0xe0;
const char mask4 = 0xf0;

const char nmask1 = 0x3f;
const char nmask2 = 0x1f;
const char nmask3 = 0x0f;
const char nmask4 = 0x07;

int _valid_utf8(const char *string, int length)
{
    const uint8_t *p;
    if (length < 0)
    {
        length = strlen(string);
    }

    for (p = (const uint8_t *)string; length-- > 0; p++)
    {
        int ab;
        int c = *p;
        if (!(c & 0x80))
            continue;
        if (c < 0xc0)
            return (uintptr_t)p - (uintptr_t)string;
        ab = _pcre_utf8_table4[c & 0x3f];
        if (length < ab || ab > 3)
            return (uintptr_t)p - (uintptr_t)string;
        length -= ab;
        if ((*(++p) & 0xc0) != 0x80)
            return (uintptr_t)p - (uintptr_t)string;
        switch (ab)
        {
        case 1:
            if ((c & 0x3e) == 0)
                return (uintptr_t)p - (uintptr_t)string;
            continue;
        case 2:
            if ((c == 0xe0 && (*p & 0x20) == 0) ||
                (c == 0xed && *p >= 0xa0))
                return (uintptr_t)p - (uintptr_t)string;
            break;
        case 3:
            if ((c == 0xf0 && (*p & 0x30) == 0) ||
                (c > 0xf4) ||
                (c == 0xf4 && *p > 0x8f))
                return (uintptr_t)p - (uintptr_t)string;
            break;
        }
        while (--ab > 0)
        {
            if ((*(++p) & 0xc0) != 0x80)
                return (uintptr_t)p - (uintptr_t)string;
        }
    }
    return -1;
}
int _utf8_get(const char *string, int length, int at, char *out_buf)
{
    const uint8_t *p;
    int ab, c;
    if (length < 0)
    {
        length = strlen(string);
    }
    if (at < 0 || at >= length)
        return -1;

    for (p = (const uint8_t *)string; length > 0 && at; p++, at--)
    {
        c = *p;
        if (!(c & 0x80))
        {
            length--;
            continue;
        }
        ab = _pcre_utf8_table4[c & 0x3f];
        p += ab++;
        length -= ab;
    }
    if (at || length <= 0)
        return -2;
    c = *p;
    if (!(c & 0x80))
    {
        *out_buf = c;
        out_buf[1] = 0;
        return 1;
    };
    ab = _pcre_utf8_table4[c & 0x3f] + 1;
    memcpy(out_buf, p, ab);
    out_buf[ab] = '\0';
    return ab;
}
int _utf8_get_offset(const char *string, int length, int at, int *out_char_len)
{
    const uint8_t *p;
    int ab, c;
    if (length < 0)
    {
        length = strlen(string);
    }
    if (at < 0 || at >= length)
        return -1;

    for (p = (const uint8_t *)string; length > 0 && at; p++, at--)
    {
        c = *p;
        if (!(c & 0x80))
        {
            length--;
            continue;
        }
        ab = _pcre_utf8_table4[c & 0x3f];
        p += ab++;
        length -= ab;
    }
    if (at)
        return -2;
    c = *p;
    if (!(c & 0x80))
    {
        if (out_char_len)
            *out_char_len = 1;
        return (uintptr_t)p - (uintptr_t)string;
    };
    ab = _pcre_utf8_table4[c & 0x3f] + 1;
    if (out_char_len)
        *out_char_len = ab;
    return (uintptr_t)p - (uintptr_t)string;
}
int _utf8_strlen(const char *string, int length)
{
    const uint8_t *p;
    int i, ab, c;
    if (length < 0)
    {
        length = strlen(string);
    }

    for (i = 0, p = (const uint8_t *)string; length > 0; i++, p++)
    {
        c = *p;
        if (!(c & 0x80))
        {
            length--;
            continue;
        }
        ab = _pcre_utf8_table4[c & 0x3f];
        p += ab++;
        length -= ab;
    }
    if (length < 0)
        return -1;
    return i;
}

int __str_repl(PikaObj *self, char *str, int str_len, int repl_at, int repl_len, char *val, int val_len)
{
    if (val_len > repl_len)
    {
        str[repl_at] = 0;
        Arg *s_new = arg_newStr(str);
        if (!s_new)
            return -1;
        s_new = arg_strAppend(s_new, val);
        s_new = arg_strAppend(s_new, str + repl_at + repl_len);
        obj_removeArg(self, "str");
        int rs = obj_setArg(self, "str", s_new);
        arg_deinit(s_new);
        if (rs)
            return -rs;
        return 0;
    }
    char *s = str + repl_at;
    memcpy(s, val, val_len);
    memmove(s + val_len, s + repl_len, str_len - repl_at - repl_len + 1);
    return 0;
}

int __utf8_to_utf32_char_LE(const char *utf8, char *out_buf)
{
    char c = *utf8;
    if (!(c & mask1))
    {
        *out_buf = c;
        out_buf[1] = 0;
        out_buf[2] = 0;
        out_buf[3] = 0;
        return 1;
    }
    int left_length = _pcre_utf8_table4[c & 0x3f];
    char a, b, d;
    switch (left_length)
    {
    case 1:
        a = c & nmask2;
        b = utf8[1] & nmask1;
        out_buf[0] = b | a << 6;
        out_buf[1] = a >> 2;
        out_buf[2] = 0;
        out_buf[3] = 0;
        return 2;
    case 2:
        a = c & nmask3;
        b = utf8[1] & nmask1;
        c = utf8[2] & nmask1;
        out_buf[0] = c | b << 6;
        out_buf[1] = b >> 2 | a << 4;
        out_buf[2] = 0;
        out_buf[3] = 0;
        return 3;
    case 3:
        a = c & nmask4;
        b = utf8[1] & nmask1;
        c = utf8[2] & nmask1;
        d = utf8[3] & nmask1;
        out_buf[0] = d | c << 6;
        out_buf[1] = c >> 2 | b << 4;
        out_buf[2] = b >> 4 | a << 2;
        out_buf[3] = 0;
        return 4;
    default:
        return 0;
    }
}
int __utf8_to_utf32_LE_noBOM_get_size(const char *utf8, int len)
{
    char *p = utf8;
    char buf[4];
    int space_sum = 0;
    while (len > 0)
    {
        int size = __utf8_to_utf32_char_LE(p, buf);
        if (!size)
            return -1;
        p += size;
        len -= size;
        space_sum++;
    }
    return space_sum * 4;
}
int __utf8_to_utf32_LE_noBOM(const char *utf8, int len, char *out_buf)
{
    char *q = out_buf;
    char *p = utf8;
    while (len > 0)
    {
        int size = __utf8_to_utf32_char_LE(p, q);
        if (!size)
            return -1;
        p += size;
        len -= size;
        q += 4;
    }
    return q - out_buf;
}
int __utf8_to_utf32_LE_withBOM(const char *utf8, int len, char *out_buf)
{
    int size = __utf8_to_utf32_LE_noBOM(utf8, len, out_buf + 4);
    if (size < 0)
    {
        return size;
    }
    out_buf[0] = '\xff';
    out_buf[1] = '\xfe';
    out_buf[2] = 0;
    out_buf[3] = 0;
    return size + 4;
}

int32_t __utf8_decode(const char *utf8, int left_length)
{
    int ucode = -1;
    char c = *utf8;
    if (!(c & mask1))
    {
        return c;
    }
    char a, b, d;
    switch (left_length)
    {
    case 1:
        a = c & nmask2;
        b = utf8[1] & nmask1;
        ucode = b | (a & 0x03) << 6;
        ucode |= (a >> 2) << 8;
        break;
    case 2:
        a = c & nmask3;
        b = utf8[1] & nmask1;
        c = utf8[2] & nmask1;
        ucode = c | (b & 0x03) << 6;
        ucode |= (b >> 2 | a << 4) << 8;
        break;
    case 3:
        a = c & nmask4;
        b = utf8[1] & nmask1;
        c = utf8[2] & nmask1;
        d = utf8[3] & nmask1;
        ucode = d | (c & 0x03) << 6;
        ucode |= (c >> 2 | (b & 0x0f) << 4) << 8;
        ucode |= (b >> 4 | a << 2) << 16;
        break;
    default:
        return -1;
    }
    return ucode;
}

int __unicode_to_utf16_char_LE(int32_t u, int8_t *out_buf)
{
    if (!(u & 0xffff0000))
    {
        out_buf[0] = u & 0xff;
        out_buf[1] = (u & 0xff00) >> 8;
        return 2;
    }
    int32_t d = u - 0x10000;
    int32_t L = d & 0x3ff;
    int32_t U = d >> 10;
    L = L | 0xdc00;
    U = U | 0xd800;
    out_buf[0] = U & 0xff;
    out_buf[1] = (U & 0xff00) >> 8;
    out_buf[2] = L & 0xff;
    out_buf[3] = (L & 0xff00) >> 8;
    return 4;
}
int __utf8_to_utf16_LE_noBOM(const char *utf8, int len, char *out_buf)
{
    char *q = out_buf;
    char *p = utf8;
    while (len > 0)
    {
        char c = *p;
        int32_t ucode;
        if (!(c & mask1))
        {
            ucode = c;
            p++;
            len--;
        }
        else
        {
            int left_size = _pcre_utf8_table4[c & 0x3f];
            ucode = __utf8_decode(p, left_size++);
            if (ucode < 0)
                return ucode;
            p += left_size;
            len -= left_size;
        }
        int size = __unicode_to_utf16_char_LE(ucode, q);
        q += size;
    }
    return q - out_buf;
}
int __utf8_to_utf16_LE_noBOM_get_size(const char *utf8, int len)
{
    char out_buf[4];
    char *p = utf8;
    int need_space = 0;
    while (len > 0)
    {
        char c = *p;
        int32_t ucode;
        if (!(c & mask1))
        {
            ucode = c;
            p++;
            len--;
        }
        else
        {
            int left_size = _pcre_utf8_table4[c & 0x3f];
            ucode = __utf8_decode(p, left_size++);
            if (ucode < 0)
                return ucode;
            p += left_size;
            len -= left_size;
        }
        int size = __unicode_to_utf16_char_LE(ucode, out_buf);
        need_space += size;
    }
    return need_space;
}
int __utf8_to_utf16_LE_withBOM(const char *utf8, int len, char *out_buf)
{
    int size = __utf8_to_utf16_LE_noBOM(utf8, len, out_buf + 2);
    if (size < 0)
    {
        return size;
    }
    out_buf[0] = '\xff';
    out_buf[1] = '\xfe';
    return size + 2;
}
Arg *_str_encode(char *str, char *encoding)
{
    if (strEqu(encoding, "utf-8"))
    {
        return arg_newBytes((uint8_t *)str, strGetSize(str));
    }
    int len = strlen(str);

    if (strEqu(encoding, "ascii"))
    {
        int ulen = _utf8_strlen(str, len);
        if (ulen == len)
        {
            return arg_newBytes((uint8_t *)str, strGetSize(str));
        }
        __platform_printf("Warning there is non-ascii characters\r\n");
        char *b = (char *)pikaMalloc(len + 1);
        if (!b)
        {
            return NULL;
        }
        char *p = str;
        char *q = b;
        char c = *p++;
        while (c)
        {
            if (!(c & 0x80))
            {
                *q++ = c;
            }
            c = *p++;
        }
        *q = 0;
        Arg *arg = arg_newBytes((uint8_t *)b, strGetSize(b));
        pikaFree(b, len + 1);
        return arg;
    }

    if (strEqu(encoding, "utf-16"))
    {
        int size_needed = __utf8_to_utf16_LE_noBOM_get_size(str, len);
        if (size_needed <= 0)
        {
            return NULL;
        }
        size_needed += 2;
        char *b = (char *)pikaMalloc(size_needed);
        if (!b)
        {
            return NULL;
        }
        int ok = __utf8_to_utf16_LE_withBOM(str, len, b);
        if (ok < 0)
        {
            pikaFree(b, size_needed);
            return NULL;
        }
        Arg *arg = arg_newBytes((uint8_t *)b, size_needed);
        pikaFree(b, size_needed);
        return arg;
    }

    if (strEqu(encoding, "utf-32"))
    {
        int size_needed = __utf8_to_utf32_LE_noBOM_get_size(str, len);
        if (size_needed <= 0)
        {
            return NULL;
        }
        size_needed += 4;
        char *b = (char *)pikaMalloc(size_needed);
        if (!b)
        {
            return NULL;
        }
        int ok = __utf8_to_utf32_LE_withBOM(str, len, b);
        if (ok < 0)
        {
            pikaFree(b, size_needed);
            return NULL;
        }
        Arg *arg = arg_newBytes((uint8_t *)b, size_needed);
        pikaFree(b, size_needed);
        return arg;
    }
    return NULL;
}
#endif

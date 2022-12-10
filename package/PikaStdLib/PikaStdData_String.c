#include "PikaStdData_String.h"
#include "PikaStdData_List.h"
#include "PikaStdData_String_Util.h"
#include "dataStrs.h"

char* _strlwr(char* str);
static int string_len(char* str);

Arg* PikaStdData_String___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_newRef(self);
}

void PikaStdData_String_set(PikaObj* self, char* s) {
#if PIKA_STRING_UTF8_ENABLE
    int r = _valid_utf8(s, -1);
    if (r >= 0) {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error invaliad character %x\r\n", s[r]);
        return;
    }
#endif
    obj_setStr(self, "str", s);
}

void PikaStdData_String___init__(PikaObj* self, char* s) {
#if PIKA_STRING_UTF8_ENABLE
    int r = _valid_utf8(s, -1);
    if (r >= 0) {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error invaliad character %x\r\n", s[r]);
        return;
    }
#endif
    PikaStdData_String_set(self, s);
}

char* PikaStdData_String_get(PikaObj* self) {
    return obj_getStr(self, "str");
}

Arg* PikaStdData_String___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    char* str = obj_getStr(self, "str");
    uint16_t len = strGetSize(str);
#if PIKA_STRING_UTF8_ENABLE
    char char_buff[5];
    int r = _utf8_get(str, len, __iter_i, char_buff);
    if (r < 0) {
        return arg_newNull();
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return arg_newStr((char*)char_buff);
#else
    Arg* res = NULL;
    char char_buff[] = " ";
    if (__iter_i < len) {
        char_buff[0] = str[__iter_i];
        res = arg_newStr((char*)char_buff);
    } else {
        return arg_newNull();
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
#endif
}

static int _str_get(char* str, int key_i, char* char_buff) {
    uint16_t len = strGetSize(str);
    if (key_i < 0) {
        key_i = string_len(str) + key_i;
    }
#if PIKA_STRING_UTF8_ENABLE
    return _utf8_get(str, len, key_i, char_buff);
#else
    if (key_i < len) {
        char_buff[0] = str[key_i];
        return 0;
    }
    return -1;
#endif
}

char* string_slice(Args* outBuffs, char* str, int start, int end) {
    char* res = args_getBuff(outBuffs, strGetSize(str));
    if (start < 0) {
        start += string_len(str);
    }
    /* magic code, to the end */
    if (end == -99999) {
        end = string_len(str);
    }
    if (end < 0) {
        end += string_len(str);
    }
    for (int i = start; i < end; i++) {
        char char_buff[5] = {0};
        int r = _str_get(str, i, char_buff);
        if (r < 0) {
            return NULL;
        }
        res = strAppend(res, char_buff);
    }
    return res;
}

Arg* PikaStdData_String___getitem__(PikaObj* self, Arg* __key) {
    int key_i = arg_getInt(__key);
    char* str = obj_getStr(self, "str");
    char char_buff[5] = {0};
    int r = _str_get(str, key_i, char_buff);
    if (r < 0) {
        return arg_newNull();
    }
    return arg_newStr((char*)char_buff);
}

void PikaStdData_String___setitem__(PikaObj* self, Arg* __key, Arg* __val) {
    int key_i = arg_getInt(__key);
    char* str = obj_getStr(self, "str");
    char* val = arg_getStr(__val);
    uint16_t len = strGetSize(str);
#if PIKA_STRING_UTF8_ENABLE
    int len2 = strlen(val);
    int is_invalid = _valid_utf8(val, len2);
    if (is_invalid >= 0) {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error String invalid\r\n");
        return;
    }
    int ulen_val = _utf8_strlen(val, len2);
    if (ulen_val != 1) {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error String invalid char\r\n");
        return;
    }
    int char_len;
    int repl_at = _utf8_get_offset(str, len, key_i, &char_len);
    if (repl_at < 0) {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error String Overflow\r\n");
        return;
    }
    int ok = __str_repl(self, str, len, repl_at, char_len, val, len2);
    if (ok < 0) {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error. Internal error(-%d)\r\n", __LINE__);
        return;
    }
#else
    if (key_i >= len) {
        obj_setErrorCode(self, 1);
        __platform_printf("Error String Overflow\r\n");
        return;
    }
    str[key_i] = val[0];
#endif
}

char* PikaStdData_String___str__(PikaObj* self) {
    return obj_getStr(self, "str");
}

int PikaStdData_String_startswith(PikaObj* self, char* prefix) {
    char* str = obj_getStr(self, "str");
    char* p = prefix;
    int i = 0;
    while (*p != '\0') {
        if (*p != str[i])
            return 0;
        p++;
        i++;
    }
    return 1;
}

int PikaStdData_String_endswith(PikaObj* self, char* suffix) {
    char* str = obj_getStr(self, "str");
    int len1 = strlen(str);
    int len2 = strlen(suffix);
    while (len2 >= 1) {
        if (suffix[len2 - 1] != str[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}

int PikaStdData_String_isdigit(PikaObj* self) {
    char* str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0') {
        if (!isdigit((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

int PikaStdData_String_islower(PikaObj* self) {
    char* str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0') {
        if (!islower((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

int PikaStdData_String_isalnum(PikaObj* self) {
    char* str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0') {
        if (!isalnum((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

int PikaStdData_String_isalpha(PikaObj* self) {
    char* str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0') {
        if (!isalpha((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

int PikaStdData_String_isspace(PikaObj* self) {
    char* str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0') {
        if (!isspace((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

PikaObj* PikaStdData_String_split(PikaObj* self, char* s) {
    /* 创建 list 对象 */
    PikaObj* list = newNormalObj(New_PikaStdData_List);
    /* 初始化 list */
    PikaStdData_List___init__(list);

    Args buffs = {0};
    char* str = strsCopy(&buffs, obj_getStr(self, "str"));

    char sign = s[0];
    int token_num = strCountSign(str, sign) + 1;

    for (int i = 0; i < token_num; i++) {
        char* token = strsPopToken(&buffs, &str, sign);
        /* 用 arg_set<type> 的 api 创建 arg */
        Arg* token_arg = arg_newStr(token);
        /* 添加到 list 对象 */
        PikaStdData_List_append(list, token_arg);
        /* 销毁 arg */
        arg_deinit(token_arg);
    }

    strsDeinit(&buffs);
    return list;
}

static int string_len(char* str) {
#if PIKA_STRING_UTF8_ENABLE
    int n = _utf8_strlen(str, -1);
    return n;
#else
    return strGetSize(str);
#endif
}

int PikaStdData_String___len__(PikaObj* self) {
    char* str = obj_getStr(self, "str");
    int n = string_len(str);
    if (n < 0) {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error. Internal error(%d)\r\n", __LINE__);
    }
    return n;
}

char* PikaStdData_String_strip(PikaObj* self, PikaTuple* chrs) {
    Args buffs = {0};
    char to_strip = ' ';
    if (pikaTuple_getSize(chrs) > 1) {
        obj_setErrorCode(self, PIKA_RES_ERR_INVALID_PARAM);
        obj_setSysOut(self, "Error. Invalid param");
    }
    if (pikaTuple_getSize(chrs) == 1) {
        char* ch_str = pikaTuple_getStr(chrs, 0);
        to_strip = ch_str[0];
    }
    char* str = strsCopy(&buffs, obj_getStr(self, "str"));
    /* strip */
    char* str_start = str;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (str[i] != to_strip) {
            str_start = (char*)(str + i);
            break;
        }
    }

    len = strlen(str);
    for (int i = len - 1; i >= 0; i--) {
        if (str[i] != to_strip) {
            str[i + 1] = '\0';
            break;
        }
    }

    obj_setStr(self, "_buf", str_start);
    strsDeinit(&buffs);
    return obj_getStr(self, "_buf");
}

char* PikaStdData_String_replace(PikaObj* self, char* old, char* new) {
    Args buffs = {0};
    char* str = strsCopy(&buffs, obj_getStr(self, "str"));
    str = strsReplace(&buffs, str, old, new);
    obj_setStr(self, "_buf", str);
    strsDeinit(&buffs);
    return obj_getStr(self, "_buf");
}

Arg* PikaStdData_String_encode(PikaObj* self, PikaTuple* encoding) {
    char* str = obj_getStr(self, "str");

#if PIKA_STRING_UTF8_ENABLE
    char* to_code = NULL;
    int argn = pikaTuple_getSize(encoding);
    if (argn < 1) {
        return arg_newBytes((uint8_t*)str, strGetSize(str));
    }
    Arg* arg_i = pikaTuple_getArg(encoding, 0);
    if (arg_getType(arg_i) != ARG_TYPE_STRING) {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error invaliad arguments\r\n");
        return NULL;
    }
    to_code = arg_getStr(arg_i);
    _strlwr(to_code);
    Arg* res = _str_encode(str, to_code);
    if (!res) {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error internal error\r\n");
        return NULL;
    }
    return res;
#else
    return arg_newBytes((uint8_t*)str, strGetSize(str));
#endif
}

#if PIKA_STRING_UTF8_ENABLE

static const uint8_t _pcre_utf8_table4[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

const char mask1 = 0x80;
const char mask2 = 0xc0;
const char mask3 = 0xe0;
const char mask4 = 0xf0;

const char nmask1 = 0x3f;
const char nmask2 = 0x1f;
const char nmask3 = 0x0f;
const char nmask4 = 0x07;

int _valid_utf8(const char* string, int length) {
    const uint8_t* p;
    if (length < 0) {
        length = strlen(string);
    }

    for (p = (const uint8_t*)string; length-- > 0; p++) {
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
        switch (ab) {
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
                if ((c == 0xf0 && (*p & 0x30) == 0) || (c > 0xf4) ||
                    (c == 0xf4 && *p > 0x8f))
                    return (uintptr_t)p - (uintptr_t)string;
                break;
        }
        while (--ab > 0) {
            if ((*(++p) & 0xc0) != 0x80)
                return (uintptr_t)p - (uintptr_t)string;
        }
    }
    return -1;
}
int _utf8_get(const char* string, int length, int at, char* out_buf) {
    const uint8_t* p;
    int ab, c;
    if (length < 0) {
        length = strlen(string);
    }
    if (at < 0 || at >= length)
        return -1;

    for (p = (const uint8_t*)string; length > 0 && at; p++, at--) {
        c = *p;
        if (!(c & 0x80)) {
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
    if (!(c & 0x80)) {
        *out_buf = c;
        out_buf[1] = 0;
        return 1;
    };
    ab = _pcre_utf8_table4[c & 0x3f] + 1;
    __platform_memcpy(out_buf, p, ab);
    out_buf[ab] = '\0';
    return ab;
}
int _utf8_get_offset(const char* string,
                     int length,
                     int at,
                     int* out_char_len) {
    const uint8_t* p;
    int ab, c;
    if (length < 0) {
        length = strlen(string);
    }
    if (at < 0 || at >= length)
        return -1;

    for (p = (const uint8_t*)string; length > 0 && at; p++, at--) {
        c = *p;
        if (!(c & 0x80)) {
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
    if (!(c & 0x80)) {
        if (out_char_len)
            *out_char_len = 1;
        return (uintptr_t)p - (uintptr_t)string;
    };
    ab = _pcre_utf8_table4[c & 0x3f] + 1;
    if (out_char_len)
        *out_char_len = ab;
    return (uintptr_t)p - (uintptr_t)string;
}
int _utf8_strlen(const char* string, int length) {
    const uint8_t* p;
    int i, ab, c;
    if (length < 0) {
        length = strlen(string);
    }

    for (i = 0, p = (const uint8_t*)string; length > 0; i++, p++) {
        c = *p;
        if (!(c & 0x80)) {
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

int __str_repl(PikaObj* self,
               char* str,
               int str_len,
               int repl_at,
               int repl_len,
               char* val,
               int val_len) {
    if (val_len > repl_len) {
        str[repl_at] = 0;
        Arg* s_new = arg_newStr(str);
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
    char* s = str + repl_at;
    __platform_memcpy(s, val, val_len);
    __platform_memmove(s + val_len, s + repl_len,
                       str_len - repl_at - repl_len + 1);
    return 0;
}

int __utf8_to_utf32_char_LE(const char* utf8, char* out_buf) {
    char c = *utf8;
    if (!(c & mask1)) {
        *out_buf = c;
        out_buf[1] = 0;
        out_buf[2] = 0;
        out_buf[3] = 0;
        return 1;
    }
    int left_length = _pcre_utf8_table4[c & 0x3f];
    char a, b, d;
    switch (left_length) {
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
int __utf8_to_utf32_LE_noBOM_get_size(const char* utf8, int len) {
    char* p = (char*)utf8;
    char buf[4];
    int space_sum = 0;
    while (len > 0) {
        int size = __utf8_to_utf32_char_LE(p, buf);
        if (!size)
            return -1;
        p += size;
        len -= size;
        space_sum++;
    }
    return space_sum * 4;
}
int __utf8_to_utf32_LE_noBOM(const char* utf8, int len, char* out_buf) {
    char* q = out_buf;
    char* p = (char*)utf8;
    while (len > 0) {
        int size = __utf8_to_utf32_char_LE(p, q);
        if (!size)
            return -1;
        p += size;
        len -= size;
        q += 4;
    }
    return q - out_buf;
}
int __utf8_to_utf32_LE_withBOM(const char* utf8, int len, char* out_buf) {
    int size = __utf8_to_utf32_LE_noBOM(utf8, len, out_buf + 4);
    if (size < 0) {
        return size;
    }
    out_buf[0] = '\xff';
    out_buf[1] = '\xfe';
    out_buf[2] = 0;
    out_buf[3] = 0;
    return size + 4;
}

int32_t __utf8_decode(const char* utf8, int left_length) {
    int ucode = -1;
    char c = *utf8;
    if (!(c & mask1)) {
        return c;
    }
    char a, b, d;
    switch (left_length) {
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

int __unicode_to_utf16_char_LE(int32_t u, char* out_buf) {
    if (!(u & 0xffff0000)) {
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
int __utf8_to_utf16_LE_noBOM(const char* utf8, int len, char* out_buf) {
    char* q = out_buf;
    char* p = (char*)utf8;
    while (len > 0) {
        char c = *p;
        int32_t ucode;
        if (!(c & mask1)) {
            ucode = c;
            p++;
            len--;
        } else {
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
int __utf8_to_utf16_LE_noBOM_get_size(const char* utf8, int len) {
    char out_buf[4];
    char* p = (char*)utf8;
    int need_space = 0;
    while (len > 0) {
        char c = *p;
        int32_t ucode;
        if (!(c & mask1)) {
            ucode = c;
            p++;
            len--;
        } else {
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
int __utf8_to_utf16_LE_withBOM(const char* utf8, int len, char* out_buf) {
    int size = __utf8_to_utf16_LE_noBOM(utf8, len, out_buf + 2);
    if (size < 0) {
        return size;
    }
    out_buf[0] = '\xff';
    out_buf[1] = '\xfe';
    return size + 2;
}
Arg* _str_encode(char* str, char* encoding) {
    if (strEqu(encoding, "utf-8")) {
        return arg_newBytes((uint8_t*)str, strGetSize(str));
    }
    int len = strlen(str);

    if (strEqu(encoding, "ascii")) {
        int ulen = _utf8_strlen(str, len);
        if (ulen == len) {
            return arg_newBytes((uint8_t*)str, strGetSize(str));
        }
        __platform_printf("Warning there is non-ascii characters\r\n");
        char* b = (char*)pikaMalloc(len + 1);
        if (!b) {
            return NULL;
        }
        char* p = str;
        char* q = b;
        char c = *p++;
        while (c) {
            if (!(c & 0x80)) {
                *q++ = c;
            }
            c = *p++;
        }
        *q = 0;
        Arg* arg = arg_newBytes((uint8_t*)b, strGetSize(b));
        pikaFree(b, len + 1);
        return arg;
    }

    if (strEqu(encoding, "utf-16")) {
        int size_needed = __utf8_to_utf16_LE_noBOM_get_size(str, len);
        if (size_needed <= 0) {
            return NULL;
        }
        size_needed += 2;
        char* b = (char*)pikaMalloc(size_needed);
        if (!b) {
            return NULL;
        }
        int ok = __utf8_to_utf16_LE_withBOM(str, len, b);
        if (ok < 0) {
            pikaFree(b, size_needed);
            return NULL;
        }
        Arg* arg = arg_newBytes((uint8_t*)b, size_needed);
        pikaFree(b, size_needed);
        return arg;
    }

    if (strEqu(encoding, "utf-32")) {
        int size_needed = __utf8_to_utf32_LE_noBOM_get_size(str, len);
        if (size_needed <= 0) {
            return NULL;
        }
        size_needed += 4;
        char* b = (char*)pikaMalloc(size_needed);
        if (!b) {
            return NULL;
        }
        int ok = __utf8_to_utf32_LE_withBOM(str, len, b);
        if (ok < 0) {
            pikaFree(b, size_needed);
            return NULL;
        }
        Arg* arg = arg_newBytes((uint8_t*)b, size_needed);
        pikaFree(b, size_needed);
        return arg;
    }
    return NULL;
}

char* _strlwr(char* str) {
    int i = 0;
    while (str[i] != '\0') {
        str[i] = tolower((int)str[i]);
        i++;
    }
    return str;
}

#endif

char* PikaStdData_String_format(PikaObj* self, PikaTuple* vars) {
    /* 'test{}'.format(123) */
    return NULL;
}

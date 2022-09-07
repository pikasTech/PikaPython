#include "PikaStdData_String.h"
#include "PikaStdData_List.h"
#include "PikaStdData_String_Util.h"
#include "dataStrs.h"

Arg *PikaStdData_String___iter__(PikaObj *self)
{
    obj_setInt(self, "__iter_i", 0);
    return arg_newRef(self);
}

void PikaStdData_String_set(PikaObj *self, char *s)
{
#if PIKA_STRING_UTF8_ENABLE
    int r = _valid_utf8(s, -1);
    if (r >= 0)
    {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error invaliad character %x\r\n", s[r]);
        return;
    }
#endif
    obj_setStr(self, "str", s);
}

void PikaStdData_String___init__(PikaObj *self, char *s)
{
#if PIKA_STRING_UTF8_ENABLE
    int r = _valid_utf8(s, -1);
    if (r >= 0)
    {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error invaliad character %x\r\n", s[r]);
        return;
    }
#endif
    PikaStdData_String_set(self, s);
}

char *PikaStdData_String_get(PikaObj *self)
{
    return obj_getStr(self, "str");
}

Arg *PikaStdData_String___next__(PikaObj *self)
{
    int __iter_i = args_getInt(self->list, "__iter_i");
    char *str = obj_getStr(self, "str");
    uint16_t len = strGetSize(str);
#if PIKA_STRING_UTF8_ENABLE
    char char_buff[5];
    int r = _utf8_get(str, len, __iter_i, char_buff);
    if (r < 0)
    {
        return arg_newNull();
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return arg_newStr((char *)char_buff);
#else
    Arg *res = NULL;
    char char_buff[] = " ";
    if (__iter_i < len)
    {
        char_buff[0] = str[__iter_i];
        res = arg_newStr((char *)char_buff);
    }
    else
    {
        return arg_newNull();
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
#endif
}

Arg *PikaStdData_String___getitem__(PikaObj *self, Arg *__key)
{
    int key_i = arg_getInt(__key);
    char *str = obj_getStr(self, "str");
    uint16_t len = strGetSize(str);
#if PIKA_STRING_UTF8_ENABLE
    char char_buff[5];
    int r = _utf8_get(str, len, key_i, char_buff);
    if (r < 0)
    {
        return arg_newNull();
    }
    return arg_newStr((char *)char_buff);
#else
    char char_buff[] = " ";
    if (key_i < len)
    {
        char_buff[0] = str[key_i];
        return arg_newStr((char *)char_buff);
    }
    else
    {
        return arg_newNull();
    }
#endif
}

void PikaStdData_String___setitem__(PikaObj *self, Arg *__key, Arg *__val)
{
    int key_i = arg_getInt(__key);
    char *str = obj_getStr(self, "str");
    char *val = arg_getStr(__val);
    uint16_t len = strGetSize(str);
#if PIKA_STRING_UTF8_ENABLE
    int len2 = strlen(val);
    int is_invalid = _valid_utf8(val, len2);
    if (is_invalid >= 0)
    {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error String invalid\r\n");
        return;
    }
    int ulen_val = _utf8_strlen(val, len2);
    if (ulen_val != 1)
    {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error String invalid char\r\n");
        return;
    }
    int char_len;
    int repl_at = _utf8_get_offset(str, len, key_i, &char_len);
    if (repl_at < 0)
    {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error String Overflow\r\n");
        return;
    }
    int ok = __str_repl(self, str, len, repl_at, char_len, val, len2);
    if (ok < 0)
    {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error. Internal error(-%d)\r\n", __LINE__);
        return;
    }
#else
    if (key_i >= len)
    {
        obj_setErrorCode(self, 1);
        __platform_printf("Error String Overflow\r\n");
        return;
    }
    str[key_i] = val[0];
#endif
}

char *PikaStdData_String___str__(PikaObj *self)
{
    return obj_getStr(self, "str");
}

int PikaStdData_String_startswith(PikaObj *self, char *prefix)
{
    char *str = obj_getStr(self, "str");
    char *p = prefix;
    int i = 0;
    while (*p != '\0')
    {
        if (*p != str[i])
            return 0;
        p++;
        i++;
    }
    return 1;
}

int PikaStdData_String_endswith(PikaObj *self, char *suffix)
{
    char *str = obj_getStr(self, "str");
    int len1 = strlen(str);
    int len2 = strlen(suffix);
    while (len2 >= 1)
    {
        if (suffix[len2 - 1] != str[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}

int PikaStdData_String_isdigit(PikaObj *self)
{
    char *str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0')
    {
        if (!isdigit((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

int PikaStdData_String_islower(PikaObj *self)
{
    char *str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0')
    {
        if (!islower((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

int PikaStdData_String_isalnum(PikaObj *self)
{
    char *str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0')
    {
        if (!isalnum((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

int PikaStdData_String_isalpha(PikaObj *self)
{
    char *str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0')
    {
        if (!isalpha((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

int PikaStdData_String_isspace(PikaObj *self)
{
    char *str = obj_getStr(self, "str");
    int i = 0;
    while (str[i] != '\0')
    {
        if (!isspace((int)str[i]))
            return 0;
        i++;
    }
    return 1;
}

PikaObj *PikaStdData_String_split(PikaObj *self, char *s)
{
    /* 创建 list 对象 */
    PikaObj *list = newNormalObj(New_PikaStdData_List);
    /* 初始化 list */
    PikaStdData_List___init__(list);

    Args buffs = {0};
    char *str = strsCopy(&buffs, obj_getStr(self, "str"));

    char sign = s[0];
    int token_num = strCountSign(str, sign) + 1;

    for (int i = 0; i < token_num; i++)
    {
        char *token = strsPopToken(&buffs, str, sign);
        /* 用 arg_set<type> 的 api 创建 arg */
        Arg *token_arg = arg_newStr(token);
        /* 添加到 list 对象 */
        PikaStdData_List_append(list, token_arg);
        /* 销毁 arg */
        arg_deinit(token_arg);
    }

    strsDeinit(&buffs);
    return list;
}

int PikaStdData_String___len__(PikaObj *self)
{
    char *str = obj_getStr(self, "str");
#if PIKA_STRING_UTF8_ENABLE
    int n = _utf8_strlen(str, -1);
    if (n < 0)
    {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error. Internal error(%d)\r\n", __LINE__);
        return n;
    }
    return n;
#else
    return strGetSize(str);
#endif
}

char *PikaStdData_String_strip(PikaObj *self)
{
    Args buffs = {0};
    char *str = strsCopy(&buffs, obj_getStr(self, "str"));
    /* strip */
    char *str_start = str;
    for (size_t i = 0; i < strGetSize(str); i++)
    {
        if (str[i] != ' ')
        {
            str_start = (char *)(str + i);
            break;
        }
    }

    for (int i = strGetSize(str) - 1; i >= 0; i--)
    {
        if (str[i] != ' ')
        {
            str[i + 1] = '\0';
            break;
        }
    }

    obj_setStr(self, "_buf", str_start);
    strsDeinit(&buffs);
    return obj_getStr(self, "_buf");
}

char *PikaStdData_String_replace(PikaObj *self, char *old, char *new)
{
    Args buffs = {0};
    char *str = strsCopy(&buffs, obj_getStr(self, "str"));
    str = strsReplace(&buffs, str, old, new);
    obj_setStr(self, "_buf", str);
    strsDeinit(&buffs);
    return obj_getStr(self, "_buf");
}

Arg *PikaStdData_String_encode(PikaObj *self, PikaTuple *encoding)
{
    char *str = obj_getStr(self, "str");

#if PIKA_STRING_UTF8_ENABLE
    char *to_code = NULL;
    int argn = tuple_getSize(encoding);
    if (argn < 1)
    {
        return arg_newBytes((uint8_t *)str, strGetSize(str));
    }
    Arg *arg_i = tuple_getArg(encoding, 0);
    if (arg_getType(arg_i) != ARG_TYPE_STRING)
    {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error invaliad arguments\r\n");
        return NULL;
    }
    to_code = arg_getStr(arg_i);
    strlwr(to_code);
    Arg *res = _str_encode(str, to_code);
    if (!res)
    {
        obj_setErrorCode(self, __LINE__);
        __platform_printf("Error internal error\r\n");
        return NULL;
    }
    return res;
#else
    return arg_newBytes((uint8_t *)str, strGetSize(str));
#endif
}

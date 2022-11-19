#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"
#include "TinyObj.h"
#include "re.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PikaStdData_List.h"
#include "PikaStdData_Tuple.h"
#include "cre.h"

#include "re_Match.h"
#include "re_Pattern.h"

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 10, 5)
#error PikaScript version 1.10.5 or later is required.
#endif

#define raise_error                        \
    {                                      \
        obj_setErrorCode(self, -__LINE__); \
    }

#define tu_getNew(name, obj_name)                       \
    PikaTuple *name = New_pikaTuple();                      \
    Any obj_name = newNormalObj(New_PikaStdData_Tuple); \
    obj_setPtr(obj_name, "list", name);

#define tu_append(tup, val, type)         \
    {                                     \
        Arg *_arg = arg_new##type(val);   \
        pikaList_append(&(tup)->super, _arg); \
        arg_deinit(_arg);                 \
    }
#define li_append(list, val, type)           \
    {                                        \
        Arg *_arg = arg_new##type(val);      \
        PikaStdData_List_append(list, _arg); \
        arg_deinit(_arg);                    \
    }

typedef PikaObj *Any;

void re_Match___init__args(PikaObj *self, char *sub, int *vec, int ven);
int _get_flags(PikaTuple *val);
PikaObj *__split(void *pattern__or__re,
                 char *subject,
                 int max_split,
                 int flags,
                 int mode_re);
PikaObj *__findall(void *pattern__or__re,
                   char *subject,
                   int flags,
                   int mode_re);
PikaObj *__subn(void *pattern__or__re,
                char *repl,
                char *subjet,
                int count,
                int flags,
                int mode_re);
void re___init__(PikaObj *self)
{
    obj_setInt(self, "A", PCRE_ONLY_ASCII);
    obj_setInt(self, "I", PCRE_CASELESS);
    obj_setInt(self, "M", PCRE_MULTILINE);
    obj_setInt(self, "S", PCRE_DOTALL);
    obj_setInt(self, "ASCII", PCRE_ONLY_ASCII);
    obj_setInt(self, "IGNORECASE", PCRE_CASELESS);
    obj_setInt(self, "MULTILINE", PCRE_MULTILINE);
    obj_setInt(self, "DOTALL", PCRE_DOTALL);
}

PikaObj *re_findall(PikaObj *self,
                    char *pattern,
                    char *subject,
                    PikaTuple *val)
{
    int flags = 0;
    flags = _get_flags(val);
    if (flags < 0)
    {
        obj_setErrorCode(self, __LINE__);
        return NULL;
    }
    Any list = __findall(pattern, subject, flags, 0);
    if (!list)
        raise_error;
    return list;
}
PikaObj *re_match(PikaObj *self, char *pattern, char *subject, PikaTuple *val)
{
    int flags = 0;
    flags = _get_flags(val);
    if (flags < 0)
    {
        obj_setErrorCode(self, __LINE__);
        return NULL;
    }
    int ven = -1;
    int *vec = pcre_match(pattern, subject, strlen(subject), &ven, flags);
    if (!vec)
    {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    Any m = newNormalObj(New_re_Match);
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
PikaObj *re_fullmatch(PikaObj *self,
                      char *pattern,
                      char *subject,
                      PikaTuple *val)
{
    int flags = 0;
    flags = _get_flags(val);
    if (flags < 0)
    {
        obj_setErrorCode(self, __LINE__);
        return NULL;
    }
    int ven = -1;

    int *vec = pcre_fullmatch(pattern, subject, strlen(subject), &ven, flags);
    if (!vec)
    {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    Any m = newNormalObj(New_re_Match);
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
PikaObj *re_search(PikaObj *self,
                   char *pattern,
                   char *subject,
                   PikaTuple *val)
{
    int flags = 0;
    flags = _get_flags(val);
    if (flags < 0)
    {
        obj_setErrorCode(self, __LINE__);
        return NULL;
    }
    int ven = -1;
    int *vec = pcre_search(pattern, subject, strlen(subject), &ven, flags);
    if (!vec)
    {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    Any m = newNormalObj(New_re_Match);
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
char *re_sub(PikaObj *self,
             char *pattern,
             char *repl,
             char *subjet,
             PikaTuple *val)
{
    int flags = PCRE_UTF8;
    int count = 0;
    int argn = pikaTuple_getSize(val);
    if (argn >= 1)
    {
        Arg *arg_i = pikaTuple_getArg(val, 0);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        count = arg_getInt(arg_i);
    }
    if (argn >= 2)
    {
        Arg *arg_i = pikaTuple_getArg(val, 1);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags = arg_getInt(arg_i);
        if (flags | PCRE_ONLY_ASCII)
        {
            flags &= ~(PCRE_ONLY_ASCII | PCRE_UTF8);
        }
    }
    int length = strlen(subjet);

    char *s = pcre_subn(pattern, repl, subjet, length, count, flags, NULL);
    if (!s)
    {
        obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    if (s == subjet)
    {
        obj_setStr(self, "_b", subjet);
        return obj_getStr(self, "_b");
    }

    int len = strlen(s);
    char *b = (char *)malloc(len + 1);
    if (!b)
    {
        free(s);
        return NULL;
    }
    memcpy(b, s, len);
    b[len] = 0;
    obj_setStr(self, "_b", b);
    free(b);
    free(s);
    return obj_getStr(self, "_b");
}
PikaObj *re_subn(PikaObj *self,
                 char *pattern,
                 char *repl,
                 char *subjet,
                 PikaTuple *val)
{
    int flags = PCRE_UTF8;
    int count = 0;
    int argn = pikaTuple_getSize(val);
    if (argn >= 1)
    {
        Arg *arg_i = pikaTuple_getArg(val, 0);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        count = arg_getInt(arg_i);
    }
    if (argn >= 2)
    {
        Arg *arg_i = pikaTuple_getArg(val, 1);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags = arg_getInt(arg_i);
        if (flags | PCRE_ONLY_ASCII)
        {
            flags &= ~(PCRE_ONLY_ASCII | PCRE_UTF8);
        }
    }
    Any res = __subn(pattern, repl, subjet, count, flags, 0);
    if (!res)
    {
        raise_error;
    }
    return res;
}
PikaObj *re_compile(PikaObj *self, char *pattern, PikaTuple *val)
{
    const char *error;
    int erroffset;
    int flags = _get_flags(val);
    if (flags < 0)
    {
        raise_error;
        return NULL;
    }
    pcre *re = pcre_compile(pattern, flags, &error, &erroffset, NULL);
    if (!re)
    {
        obj_setErrorCode(self, erroffset);
        return NULL;
    }

    Any m = newNormalObj(New_re_Pattern);
    obj_setPtr(m, "_re", re);
    return m;
}
PikaObj *re_split(PikaObj *self, char *pattern, char *subject, PikaTuple *val)
{
    int flags = PCRE_UTF8;
    int max_split = 0;
    int argn = pikaTuple_getSize(val);
    if (argn >= 1)
    {
        Arg *arg_i = pikaTuple_getArg(val, 0);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        max_split = arg_getInt(arg_i);
    }
    if (argn >= 2)
    {
        Arg *arg_i = pikaTuple_getArg(val, 1);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags = arg_getInt(arg_i);
        if (flags | PCRE_ONLY_ASCII)
        {
            flags &= ~(PCRE_ONLY_ASCII | PCRE_UTF8);
        }
    }
    Any list = __split(pattern, subject, max_split, flags, 0);
    if (!list)
        raise_error;
    return list;
}

char *re_escape(PikaObj *self, char *pattern)
{
    const char *special_chars = "()[]{}?*+-|^$\\.&~# \t\n\r\v\f";
    const int special_chars_len = 25;
    if (!pattern)
        return NULL;
    int n = strlen(pattern);
    int after_size = n;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < special_chars_len; j++)
        {
            if (pattern[i] != special_chars[j])
                continue;
            after_size++;
            break;
        }
    }
    char *new_s = (char *)malloc(after_size + 1);
    if (!new_s)
        return NULL;
    int at = 0;
    while (*pattern)
    {
        char c = *pattern;
        int j = 0;
        for (; j < special_chars_len; j++)
        {
            if (c != special_chars[j])
                continue;
            new_s[at++] = '\\';
            break;
        }
        new_s[at++] = c;
        pattern++;
    }
    new_s[at++] = 0;
    obj_setStr(self, "_b", new_s);
    free(new_s);
    return obj_getStr(self, "_b");
}

void re_Match___del__(PikaObj *self)
{
    void *vec = obj_getPtr(self, "_vec");
    if (!vec)
        return;
    free(vec);
}
void re_Match___init__(PikaObj *self)
{
    if (!obj_isArgExist(self, "_vec"))
    {
        obj_setPtr(self, "_vec", NULL);
        obj_setStr(self, "_b", "");
        obj_setInt(self, "_ven", 0);
        obj_setStr(self, "_s", "");
    }
}
void re_Match___init__args(PikaObj *self, char *sub, int *vec, int ven)
{
    obj_setPtr(self, "_vec", vec);
    obj_setStr(self, "_b", "");
    obj_setInt(self, "_ven", ven);
    obj_setStr(self, "_s", sub);
}
char *re_Match_group(PikaObj *self, PikaTuple *val)
{
    int n = 0;
    int argn = pikaTuple_getSize(val);
    if (argn >= 1)
    {
        Arg *arg_i = pikaTuple_getArg(val, 0);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        n = arg_getInt(arg_i);
    }

    int *vec = obj_getPtr(self, "_vec");
    if (!vec)
        return NULL;
    char *s = obj_getStr(self, "_s");
    if (!s)
        return NULL;
    int ven = obj_getInt(self, "_ven");
    if (n >= ven || n < 0)
    {
        obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    int len = vec[n * 2 + 1] - vec[n * 2];
    if (!len)
        return "";
    char *b = (char *)malloc(len + 1);
    if (!b)
        return NULL;
    memcpy(b, s + vec[n * 2], len);
    b[len] = 0;
    obj_setStr(self, "_b", b);
    free(b);
    return obj_getStr(self, "_b");
}
PikaObj *re_Match_groups(PikaObj *self)
{
    int *vec = obj_getPtr(self, "_vec");
    if (!vec)
        return NULL;
    char *s = obj_getStr(self, "_s");
    if (!s)
        return NULL;
    int ven = obj_getInt(self, "_ven");
    if (!ven)
        return NULL;
    tu_getNew(tup, tup_obj);

    for (int i = 1; i < ven; i++)
    {
        Arg *str_arg1;
        int len = vec[i * 2 + 1] - vec[i * 2];
        if (len)
        {
            char *b = (char *)malloc(len + 1);
            if (!b)
                return NULL;
            memcpy(b, s + vec[i * 2], len);
            b[len] = 0;
            str_arg1 = arg_newStr(b);
            free(b);
        }
        else
        {
            str_arg1 = arg_newStr("");
        }
        pikaList_append(&(tup)->super, str_arg1);
        arg_deinit(str_arg1);
    }
    return tup_obj;
}
PikaObj *re_Match_span(PikaObj *self, PikaTuple *val)
{
    int group_n = 0;
    int argn = pikaTuple_getSize(val);
    if (argn >= 1)
    {
        Arg *arg_i = pikaTuple_getArg(val, 0);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        group_n = arg_getInt(arg_i);
    }
    int *vec = obj_getPtr(self, "_vec");
    if (!vec)
        raise_error;
    int ven = obj_getInt(self, "_ven");
    if (!ven || group_n >= ven)
    {
        obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    tu_getNew(tu, tu_obj);
    tu_append(tu, vec[group_n * 2], Int);
    tu_append(tu, vec[group_n * 2 + 1], Int);
    return tu_obj;
}

void re_Pattern___del__(PikaObj *self)
{
    void *_re = obj_getPtr(self, "_re");
    if (!_re)
        return;
    pcre *re = (pcre *)_re;
    pcre_free(re);
}

void re_Pattern___init__(PikaObj *self)
{
    if (!obj_isArgExist(self, "_re"))
    {
        obj_setPtr(self, "_re", NULL);
        obj_setStr(self, "_b", "");
        obj_setInt(self, "_n", -1);
    }
}
PikaObj *re_Pattern_findall(PikaObj *self, char *subject, PikaTuple *val)
{
    int flags = 0;
    flags = _get_flags(val);
    if (flags < 0)
    {
        obj_setErrorCode(self, __LINE__);
        return NULL;
    }
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre *re = obj_getPtr(self, "_re");
    Any list = __findall(re, subject, flags, 1);
    if (!list)
        raise_error;
    return list;
}
PikaObj *re_Pattern_match(PikaObj *self, char *subject, PikaTuple *val)
{
    int flags = 0;
    flags = _get_flags(val);
    if (flags < 0)
    {
        obj_setErrorCode(self, __LINE__);
        return NULL;
    }
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre *re = obj_getPtr(self, "_re");
    int ven = -1;
    int *vec = re_match2(re, subject, strlen(subject), &ven, flags);
    if (!vec)
    {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);

        return NULL;
    }
    Any m = newNormalObj(New_re_Match);
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
PikaObj *re_Pattern_fullmatch(PikaObj *self, char *subject, PikaTuple *val)
{
    int flags = 0;
    flags = _get_flags(val);
    if (flags < 0)
    {
        obj_setErrorCode(self, __LINE__);
        return NULL;
    }
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre *re = obj_getPtr(self, "_re");
    int ven = -1;
    int *vec = re_fullmatch2(re, subject, strlen(subject), &ven, flags);
    if (!vec)
    {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    Any m = newNormalObj(New_re_Match);
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
PikaObj *re_Pattern_search(PikaObj *self, char *subject, PikaTuple *val)
{
    int flags = 0;
    flags = _get_flags(val);
    if (flags < 0)
    {
        obj_setErrorCode(self, __LINE__);
        return NULL;
    }
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre *re = obj_getPtr(self, "_re");
    Any m = newNormalObj(New_re_Match);
    int ven = -1;

    int *vec = re_search2(re, subject, strlen(subject), &ven, flags);
    if (!vec)
    {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
char *re_Pattern_sub(PikaObj *self, char *repl, char *subjet, PikaTuple *val)
{
    int flags = 0;
    int count = 0;
    int argn = pikaTuple_getSize(val);
    if (argn >= 1)
    {
        Arg *arg_i = pikaTuple_getArg(val, 0);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        count = arg_getInt(arg_i);
    }
    if (argn >= 2)
    {
        Arg *arg_i = pikaTuple_getArg(val, 1);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags = arg_getInt(arg_i);
        if (flags | PCRE_ONLY_ASCII)
        {
            flags &= ~(PCRE_ONLY_ASCII | PCRE_UTF8);
        }
    }

    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre *re = obj_getPtr(self, "_re");
    int length = strlen(subjet);
    int matched_times = 0;
    char *s = re_subn2(re, repl, subjet, length, count, flags, &matched_times);
    obj_setInt(self, "_n", matched_times);

    if (!s)
    {
        obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    if (s == subjet)
    {
        obj_setStr(self, "_b", subjet);
        return obj_getStr(self, "_b");
    }

    int len = strlen(s);
    char *b = (char *)malloc(len + 1);
    if (!b)
    {
        free(s);
        return NULL;
    }
    memcpy(b, s, len);
    b[len] = 0;
    obj_setStr(self, "_b", b);
    free(b);
    free(s);
    return obj_getStr(self, "_b");
}
PikaObj *re_Pattern_subn(PikaObj *self, char *repl, char *subjet, PikaTuple *val)
{
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    int flags = 0;
    int count = 0;
    int argn = pikaTuple_getSize(val);
    if (argn >= 1)
    {
        Arg *arg_i = pikaTuple_getArg(val, 0);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        count = arg_getInt(arg_i);
    }
    if (argn >= 2)
    {
        Arg *arg_i = pikaTuple_getArg(val, 1);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags = arg_getInt(arg_i);
        if (flags | PCRE_ONLY_ASCII)
        {
            flags &= ~(PCRE_ONLY_ASCII | PCRE_UTF8);
        }
    }
    pcre *re = obj_getPtr(self, "_re");
    Any res = __subn(re, repl, subjet, count, flags, 1);
    if (!res)
        raise_error;
    return res;
}
PikaObj *re_Pattern_split(PikaObj *self, char *subject, PikaTuple *val)
{
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre *re = obj_getPtr(self, "_re");
    int flags = PCRE_UTF8;
    int max_split = 0;
    int argn = pikaTuple_getSize(val);
    if (argn >= 1)
    {
        Arg *arg_i = pikaTuple_getArg(val, 0);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        max_split = arg_getInt(arg_i);
    }
    if (argn >= 2)
    {
        Arg *arg_i = pikaTuple_getArg(val, 1);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags = arg_getInt(arg_i);
        if (flags | PCRE_ONLY_ASCII)
        {
            flags &= ~(PCRE_ONLY_ASCII | PCRE_UTF8);
        }
    }
    Any list = __split(re, subject, max_split, flags, 1);
    if (!list)
        raise_error;
    return list;
}

int _get_flags(PikaTuple *val)
{
    int flags = PCRE_UTF8;
    int argn = pikaTuple_getSize(val);
    if (argn >= 1)
    {
        Arg *arg_i = pikaTuple_getArg(val, 0);
        if (arg_getType(arg_i) != ARG_TYPE_INT)
        {
            return -1;
        }
        flags |= arg_getInt(arg_i);
        if (flags & PCRE_ONLY_ASCII)
        {
            flags &= ~(PCRE_ONLY_ASCII | PCRE_UTF8);
        }
    }
    return flags;
}

PikaObj *__split(void *pattern__or__re,
                 char *subject,
                 int max_split,
                 int flags,
                 int mode_re)

{
    int sub_length = strlen(subject);
    int j2 = 0;
    int _m_n = 0, m_n = 0;
    int brackets = -1;
    int **vcs;
    if (mode_re)
        vcs = re_searchall2((pcre *)pattern__or__re, subject, sub_length, &_m_n, &brackets, flags);
    else
        vcs = re_searchall((char *)pattern__or__re, subject, sub_length, &_m_n, &brackets, flags);
    m_n = _m_n;
    char *b = NULL;
    Arg *str_arg1;
    // Arg *sub_arg;
    if (!vcs)
    {
        return NULL;
    }
    if (max_split && max_split < m_n)
        m_n = max_split;
    Any list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(list);
    int start = 0;

    if (brackets == 1)
    {
        for (int i = 0; i < m_n; i++)
        {
            int *v = vcs[i];
            int length = v[0] - start;
            if (length)
            {
                b = malloc(length + 1);
                if (!b)
                    goto e_er;
                b[length] = 0;
                memcpy(b, subject + start, length);
            }
            else
            {
                b = (char *)"";
            }
            str_arg1 = arg_newStr(b);
            PikaStdData_List_append(list, str_arg1);
            arg_deinit(str_arg1);
            if (length)
                free(b);
            start = v[1];
        }
        if (start <= sub_length)
        {
            str_arg1 = arg_newStr(subject + start);
            PikaStdData_List_append(list, str_arg1);
            arg_deinit(str_arg1);
        }
        goto exit;
    }

    for (int i = 0; i < m_n; i++)
    {
        int *v = vcs[i];
        int length = v[0] - start;
        b = malloc(length + 1);
        if (!b)
            goto e_er;
        memcpy(b, subject + start, length);
        b[length] = 0;
        str_arg1 = arg_newStr(b);
        PikaStdData_List_append(list, str_arg1);
        arg_deinit(str_arg1);

        for (int j = 1; j < brackets; j++)
        {
            j2 = j * 2;
            int length2 = v[j2 + 1] - v[j2];
            if (length2 > length)
            {
                free(b);
                length = length2;
                b = malloc(length + 1);
                if (!b)
                    goto e_er;
            }
            b[length2] = 0;
            memcpy(b, subject + v[j2], length2);

            str_arg1 = arg_newStr(b);
            PikaStdData_List_append(list, str_arg1);
            arg_deinit(str_arg1);
        }
        start = v[1];
        free(b);
    }
    if (start <= sub_length)
    {
        str_arg1 = arg_newStr(subject + start);
        PikaStdData_List_append(list, str_arg1);
        arg_deinit(str_arg1);
    }
    goto exit;
e_er:
    if (list)
    {
        obj_deinit(list);
        list = NULL;
    }
exit:
    if (vcs)
        re_free_searchall(vcs, _m_n);
    return list;
}

PikaObj *__findall(void *pattern__or__re,
                   char *subject,
                   int flags,
                   int mode_re)
{
    int length = strlen(subject);
    int j2 = 0;
    int m_n = -1;
    int brackets = -1;
    int **vcs;
    if (mode_re)
        vcs = re_searchall2((pcre *)pattern__or__re, subject, length, &m_n, &brackets, flags);
    else
        vcs = re_searchall((char *)pattern__or__re, subject, length, &m_n, &brackets, flags);

    char *b = NULL;
    Arg *str_arg1;
    Arg *sub_arg;
    if (!vcs)
    {
        if (m_n < 0)
            return NULL;
        Any list = newNormalObj(New_PikaStdData_List);
        PikaStdData_List___init__(list);

        return list;
    }
    Any list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(list);
    PikaTuple *tu;
    Any sub_list = NULL;
    if (brackets == 1)
    {
        for (int i = 0; i < m_n; i++)
        {
            int *v = vcs[i];
            length = v[1] - v[0];
            if (length)
            {
                b = malloc(length + 1);
                if (!b)
                    goto e_er;
                b[length] = 0;
                memcpy(b, subject + v[0], length);
            }
            else
            {
                b = (char *)"";
            }
            str_arg1 = arg_newStr(b);
            PikaStdData_List_append(list, str_arg1);
            arg_deinit(str_arg1);
            if (length)
                free(b);
        }
        goto exit;
    }

    for (int i = 0; i < m_n; i++)
    {
        int *v = vcs[i];
        length = v[1] - v[0];
        b = malloc(length + 1);
        if (!b)
            goto e_er;
        tu = New_pikaTuple();

        for (int j = 1; j < brackets; j++)
        {
            j2 = j * 2;
            length = v[j2 + 1] - v[j2];
            b[length] = 0;
            memcpy(b, subject + v[j2], length);
            tu_append(tu, b, Str);
        }
        sub_list = newNormalObj(New_PikaStdData_Tuple);
        obj_setPtr(sub_list, "list", tu);
        sub_arg = arg_newObj(sub_list);
        PikaStdData_List_append(list, sub_arg);
        arg_deinit(sub_arg);
        free(b);
    }
    goto exit;
e_er:
    if (list)
    {
        obj_deinit(list);
        list = NULL;
    }
exit:
    if (vcs)
        re_free_searchall(vcs, m_n);
    return list;
}

PikaObj *__subn(void *pattern__or__re,
                char *repl,
                char *subjet,
                int count,
                int flags,
                int mode_re)
{
    int length = strlen(subjet);
    int matched_times = 0;
    char *s;
    if (mode_re)
        s = re_subn2((pcre *)pattern__or__re, repl, subjet, length, count, flags, &matched_times);
    else
        s = pcre_subn((char *)pattern__or__re, repl, subjet, length, count, flags, &matched_times);

    if (!s)
    {
        return NULL;
    }
    if (s == subjet)
    {
        PikaTuple *yup = New_pikaTuple();
        tu_append(yup, s, Str);
        tu_append(yup, 0, Int);

        Any tuple_obj = newNormalObj(New_PikaStdData_Tuple);
        obj_setPtr(tuple_obj, "list", yup);
        return tuple_obj;
    }

    PikaTuple *yup = New_pikaTuple();
    tu_append(yup, s, Str);
    free(s);

    tu_append(yup, matched_times, Int);

    Any tuple_obj = newNormalObj(New_PikaStdData_Tuple);
    obj_setPtr(tuple_obj, "list", yup);
    return tuple_obj;
}
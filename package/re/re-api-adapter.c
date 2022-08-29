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

void re_Match___init__args(PikaObj* self, char* sub, int* vec, int ven);

void pre_init_re(PikaObj* self) {
    obj_setInt(self, "I", PCRE_CASELESS);
    obj_setInt(self, "M", PCRE_MULTILINE);
    obj_setInt(self, "IGNORECASE", PCRE_CASELESS);
    obj_setInt(self, "MULTILINE", PCRE_MULTILINE);
    obj_setInt(self, "DOTALL", PCRE_DOTALL);
}

void re___init__(PikaObj* self) {
    pre_init_re(self);
}

PikaObj* re_findall(PikaObj* self,
                    char* pattern,
                    char* subject,
                    PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }

    PikaObj* list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(list);
    PikaObj* sub_list;
    int length = strlen(subject);
    // int n = 0;
    flags |= PCRE_UTF8;
    int j2 = 0;
    int m_n = -1;
    int brackets = -1;
    int** vcs = re_searchall(pattern, subject, length, &m_n, &brackets, flags);
    char* b = NULL;
    Arg* str_arg1;
    Arg* sub_arg;
    if (!vcs) {
        if (m_n < 0)
            obj_setErrorCode(self, -__LINE__);
        return list;
    }
    if (brackets == 1) {
        for (int i = 0; i < m_n; i++) {
            int* v = vcs[i];
            length = v[1] - v[0];
            if (length) {
                b = malloc(length + 1);
                if (!b)
                    goto e_er;
                b[length] = 0;
                memcpy(b, subject + v[0], length);
            } else {
                b = (char*)"";
            }
            str_arg1 = arg_newStr(b);
            PikaStdData_List_append(list, str_arg1);
            arg_deinit(str_arg1);
            if (length)
                free(b);
        }
        goto e_er;
    }

    for (int i = 0; i < m_n; i++) {
        int* v = vcs[i];
        length = v[1] - v[0];
        b = malloc(length + 1);
        if (!b)
            goto e_er;
        sub_list = newNormalObj(New_PikaStdData_List);
        PikaStdData_List___init__(sub_list);
        for (int j = 0; j < brackets; j++) {
            j2 = j * 2;
            length = v[j2 + 1] - v[j2];
            b[length] = 0;
            memcpy(b, subject + v[j2], length);

            str_arg1 = arg_newStr(b);
            PikaStdData_List_append(sub_list, str_arg1);
            arg_deinit(str_arg1);
        }
        sub_arg = arg_newRef(sub_list);
        PikaStdData_List_append(list, sub_arg);
        arg_deinit(sub_arg);
        free(b);
    }
e_er:
    if (vcs)
        re_free_searchall(vcs, m_n);
    return list;

    // char **res = pcre_findall(pattern, subject, length, &n, flags);
    // if (!res)
    //     return list;
    // for (int i = 0; i < n; i++)
    // {
    //     Arg *str_arg1 = arg_newStr(res[i]);
    //     PikaStdData_List_append(list, str_arg1);
    //     arg_deinit(str_arg1);
    // }
    // re_free_findall(res, n);
    // return list;
}
PikaObj* re_match(PikaObj* self, char* pattern, char* subject, PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }
    PikaObj* m = newNormalObj(New_re_Match);
    int ven = -1;
    flags |= PCRE_UTF8;

    int* vec = pcre_match(pattern, subject, strlen(subject), &ven, flags);
    if (!vec) {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
PikaObj* re_fullmatch(PikaObj* self,
                      char* pattern,
                      char* subject,
                      PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }
    PikaObj* m = newNormalObj(New_re_Match);
    int ven = -1;
    flags |= PCRE_UTF8;

    int* vec = pcre_fullmatch(pattern, subject, strlen(subject), &ven, flags);
    if (!vec) {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    re_Match___init__args(m, subject, vec, ven);
    return m;
}

PikaObj* re_search(PikaObj* self,
                   char* pattern,
                   char* subject,
                   PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }
    PikaObj* m = newNormalObj(New_re_Match);
    int ven = -1;
    flags |= PCRE_UTF8;

    int* vec = pcre_search(pattern, subject, strlen(subject), &ven, flags);
    if (!vec) {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
char* re_sub(PikaObj* self,
             char* pattern,
             char* repl,
             char* subjet,
             PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }
    int length = strlen(subjet);
    flags |= PCRE_UTF8;

    char* s = pcre_sub(pattern, repl, subjet, length, flags);
    if (!s) {
        obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    if (s == subjet) {
        obj_setStr(self, "_b", subjet);
        return obj_getStr(self, "_b");
    }

    int len = strlen(s);
    char* b = (char*)malloc(len + 1);
    if (!b) {
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
PikaObj* re_compile(PikaObj* self, char* pattern) {
    const char* error;
    int erroffset;
    pcre* re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (!re) {
        obj_setErrorCode(self, erroffset);
        return NULL;
    }

    PikaObj* m = newNormalObj(New_re_Pattern);
    obj_setPtr(m, "_re", re);
    return m;
}

void re_Match___del__(PikaObj* self) {
    void* vec = obj_getPtr(self, "_vec");
    if (!vec)
        return;
    free(vec);
}
void re_Match___init__(PikaObj* self) {
    if (!obj_isArgExist(self, "_vec")) {
        obj_setPtr(self, "_vec", NULL);
        obj_setStr(self, "_b", "");
        obj_setInt(self, "_ven", 0);
        obj_setStr(self, "_s", "");
    }
}
void re_Match___init__args(PikaObj* self, char* sub, int* vec, int ven) {
    obj_setPtr(self, "_vec", vec);
    obj_setStr(self, "_b", "");
    obj_setInt(self, "_ven", ven);
    obj_setStr(self, "_s", sub);
}
char* re_Match_group(PikaObj* self, int n) {
    int* vec = obj_getPtr(self, "_vec");
    if (!vec)
        return NULL;
    char* s = obj_getStr(self, "_s");
    if (!s)
        return NULL;
    int ven = obj_getInt(self, "_ven");
    if (n >= ven || n < 0) {
        obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    int len = vec[n * 2 + 1] - vec[n * 2];
    if (!len)
        return "";
    char* b = (char*)malloc(len + 1);
    if (!b)
        return NULL;
    memcpy(b, s + vec[n * 2], len);
    b[len] = 0;
    obj_setStr(self, "_b", b);
    free(b);
    return obj_getStr(self, "_b");
}
PikaObj* re_Match_groups(PikaObj* self) {
    PikaObj* list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(list);

    int* vec = obj_getPtr(self, "_vec");
    if (!vec)
        return list;
    char* s = obj_getStr(self, "_s");
    if (!s)
        return list;
    int ven = obj_getInt(self, "_ven");
    if (!ven)
        return list;
    for (int i = 0; i < ven; i++) {
        Arg* str_arg1;
        int len = vec[i * 2 + 1] - vec[i * 2];
        if (len) {
            char* b = (char*)malloc(len + 1);
            if (!b)
                return NULL;
            memcpy(b, s + vec[i * 2], len);
            b[len] = 0;
            str_arg1 = arg_newStr(b);
            free(b);
        } else {
            str_arg1 = arg_newStr("");
        }
        PikaStdData_List_append(list, str_arg1);
        arg_deinit(str_arg1);
    }
    return list;
}
PikaObj* re_Match_span(PikaObj* self, int group_n) {
    PikaObj* list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(list);

    int* vec = obj_getPtr(self, "_vec");
    if (!vec)
        return list;
    int ven = obj_getInt(self, "_ven");
    if (!ven || group_n >= ven) {
        obj_setErrorCode(self, -__LINE__);
        return list;
    }

    Arg* spos = arg_newInt(vec[group_n * 2]);
    Arg* epos = arg_newInt(vec[group_n * 2 + 1]);
    PikaStdData_List_append(list, spos);
    PikaStdData_List_append(list, epos);

    arg_deinit(spos);
    arg_deinit(epos);
    return list;
}

void re_Pattern___del__(PikaObj* self) {
    void* _re = obj_getPtr(self, "_re");
    if (!_re)
        return;
    pcre* re = (pcre*)_re;
    pcre_free(re);
}

void re_Pattern___init__(PikaObj* self) {
    if (!obj_isArgExist(self, "_re")) {
        obj_setPtr(self, "_re", NULL);
        obj_setStr(self, "_b", "");
    }
}

PikaObj* re_Pattern_findall(PikaObj* self, char* subject, PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre* re = obj_getPtr(self, "_re");

    PikaObj* list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(list);
    PikaObj* sub_list;
    int length = strlen(subject);
    // int n = 0;
    flags |= PCRE_UTF8;
    int j2 = 0;
    int m_n = -1;
    int brackets = 0;
    int** vcs = re_searchall2(re, subject, length, &m_n, &brackets, flags);
    char* b = NULL;
    Arg* str_arg1;
    Arg* sub_arg;

    if (!vcs) {
        if (m_n < 0)
            obj_setErrorCode(self, -__LINE__);

        return list;
    }
    if (brackets == 1) {
        for (int i = 0; i < m_n; i++) {
            int* v = vcs[i];
            length = v[1] - v[0];
            if (length) {
                b = malloc(length + 1);
                if (!b)
                    goto e_er;
                b[length] = 0;
                memcpy(b, subject + v[0], length);
            } else {
                b = (char*)"";
            }
            str_arg1 = arg_newStr(b);
            PikaStdData_List_append(list, str_arg1);
            arg_deinit(str_arg1);
            if (length)
                free(b);
        }
        goto e_er;
    }

    for (int i = 0; i < m_n; i++) {
        int* v = vcs[i];
        length = v[1] - v[0];
        b = malloc(length + 1);
        if (!b)
            goto e_er;
        sub_list = newNormalObj(New_PikaStdData_List);
        PikaStdData_List___init__(sub_list);
        for (int j = 0; j < brackets; j++) {
            j2 = j * 2;
            length = v[j2 + 1] - v[j2];
            b[length] = 0;
            memcpy(b, subject + v[j2], length);

            str_arg1 = arg_newStr(b);
            PikaStdData_List_append(sub_list, str_arg1);
            arg_deinit(str_arg1);
        }
        sub_arg = arg_newPtr(ARG_TYPE_OBJECT, sub_list);
        PikaStdData_List_append(list, sub_arg);
        arg_deinit(sub_arg);
        free(b);
    }
e_er:
    if (vcs)
        re_free_searchall(vcs, m_n);
    return list;
}

PikaObj* re_Pattern_match(PikaObj* self, char* subject, PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre* re = obj_getPtr(self, "_re");
    PikaObj* m = newNormalObj(New_re_Match);
    int ven = -1;
    flags |= PCRE_UTF8;

    int* vec = re_match2(re, subject, strlen(subject), &ven, flags);
    if (!vec) {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);

        return NULL;
    }
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
PikaObj* re_Pattern_fullmatch(PikaObj* self, char* subject, PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre* re = obj_getPtr(self, "_re");
    PikaObj* m = newNormalObj(New_re_Match);
    int ven = -1;
    flags |= PCRE_UTF8;

    int* vec = re_fullmatch2(re, subject, strlen(subject), &ven, flags);
    if (!vec) {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);

        return NULL;
    }
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
PikaObj* re_Pattern_search(PikaObj* self, char* subject, PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre* re = obj_getPtr(self, "_re");
    PikaObj* m = newNormalObj(New_re_Match);
    int ven = -1;
    flags |= PCRE_UTF8;

    int* vec = re_search2(re, subject, strlen(subject), &ven, flags);
    if (!vec) {
        if (ven < 0)
            obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    re_Match___init__args(m, subject, vec, ven);
    return m;
}
char* re_Pattern_sub(PikaObj* self, char* repl, char* subjet, PikaTuple* val) {
    int flags = 0;
    for (size_t i = 0; i < tuple_getSize(val); i++) {
        Arg* arg_i = tuple_getArg(val, i);
        if (arg_getType(arg_i) != ARG_TYPE_INT) {
            obj_setErrorCode(self, -__LINE__);
            return NULL;
        }
        flags |= arg_getInt(arg_i);
    }
    if (!obj_isArgExist(self, "_re"))
        return NULL;
    pcre* re = obj_getPtr(self, "_re");
    int length = strlen(subjet);
    flags |= PCRE_UTF8;

    char* s = re_sub2(re, repl, subjet, length, flags);
    if (!s) {
        obj_setErrorCode(self, -__LINE__);
        return NULL;
    }
    if (s == subjet) {
        obj_setStr(self, "_b", subjet);
        return obj_getStr(self, "_b");
    }

    int len = strlen(s);
    char* b = (char*)malloc(len + 1);
    if (!b) {
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

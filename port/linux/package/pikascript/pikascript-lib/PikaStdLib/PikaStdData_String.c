#include "PikaStdData_String.h"
#include "PikaStdData_List.h"
#include "PikaStdData_String_Util.h"
#include "dataStrs.h"

Arg* PikaStdData_String___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_setRef(NULL, "", self);
}

void PikaStdData_String_set(PikaObj* self, char* s) {
    obj_setStr(self, "str", s);
}

void PikaStdData_String___init__(PikaObj* self, char* s) {
    PikaStdData_String_set(self, s);
}

char* PikaStdData_String_get(PikaObj* self) {
    return obj_getStr(self, "str");
}

Arg* PikaStdData_String___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    char* str = obj_getStr(self, "str");
    uint16_t len = strGetSize(str);
    Arg* res = NULL;
    char char_buff[] = " ";
    if (__iter_i < len) {
        char_buff[0] = str[__iter_i];
        res = arg_setStr(NULL, "", (char*)char_buff);
    } else {
        return arg_setNull(NULL);
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}

Arg* PikaStdData_String___get__(PikaObj* self, Arg* __key) {
    int key_i = arg_getInt(__key);
    char* str = obj_getStr(self, "str");
    uint16_t len = strGetSize(str);
    char char_buff[] = " ";
    if (key_i < len) {
        char_buff[0] = str[key_i];
        return arg_setStr(NULL, "", (char*)char_buff);
    } else {
        return arg_setNull(NULL);
    }
}

void PikaStdData_String___set__(PikaObj* self, Arg* __key, Arg* __val) {
    int key_i = arg_getInt(__key);
    char* str = obj_getStr(self, "str");
    char* val = arg_getStr(__val);
    uint16_t len = strGetSize(str);
    if (key_i >= len) {
        obj_setErrorCode(self, 1);
        __platform_printf("Error String Overflow\r\n");
        return;
    }
    str[key_i] = val[0];
}

char* PikaStdData_String___str__(PikaObj* self) {
    return obj_getStr(self, "str");
}

int PikaStdData_String_startwith(PikaObj* self, char* prefix) {
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

int PikaStdData_String_endwith(PikaObj* self, char* suffix) {
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
        char* token = strsPopToken(&buffs, str, sign);
        /* 用 arg_set<type> 的 api 创建 arg */
        Arg* token_arg = arg_setStr(NULL, "", token);
        /* 添加到 list 对象 */
        PikaStdData_List_append(list, token_arg);
        /* 销毁 arg */
        arg_deinit(token_arg);
    }

    strsDeinit(&buffs);
    return list;
}

int PikaStdData_String___len__(PikaObj* self) {
    char* str = obj_getStr(self, "str");
    return strGetSize(str);
}

char* PikaStdData_String_strip(PikaObj* self) {
    Args buffs = {0};
    char* str = strsCopy(&buffs, obj_getStr(self, "str"));
    /* strip */
    char* str_start = str;
    for (size_t i = 0; i < strGetSize(str); i++) {
        if (str[i] != ' ') {
            str_start = (char*)(str + i);
            break;
        }
    }

    for (int i = strGetSize(str) - 1; i >= 0; i--) {
        if (str[i] != ' ') {
            str[i + 1] = '\0';
            break;
        }
    }

    obj_setStr(self, "_buf", str_start);
    strsDeinit(&buffs);
    return obj_getStr(self, "_buf");
}

char* PikaStdData_String_replace(PikaObj* self, char* new, char* old) {
    Args buffs = {0};
    char* str = strsCopy(&buffs, obj_getStr(self, "str"));
    str = strsReplace(&buffs, str, old, new);
    obj_setStr(self, "_buf", str);
    strsDeinit(&buffs);
    return obj_getStr(self, "_buf");
}

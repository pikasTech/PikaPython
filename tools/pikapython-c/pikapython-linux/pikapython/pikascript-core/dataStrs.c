/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "dataStrs.h"
#include "PikaPlatform.h"
#include "dataString.h"

Args* New_strBuff(void) {
    return New_args(NULL);
}

char* strsRemovePrefix(Args* buffs_p, char* inputStr, char* prefix) {
    int32_t size = strGetSize(inputStr);
    char* buff = args_getBuff(buffs_p, size);
    return strRemovePrefix(inputStr, prefix, buff);
}

char* strsGetDirectStr(Args* buffs_p, char* argPath) {
    char* directStr = NULL;
    directStr = strsCut(buffs_p, argPath, '"', '"');
    if (NULL != directStr) {
        return directStr;
    }
    directStr = strsCut(buffs_p, argPath, '\'', '\'');
    if (NULL != directStr) {
        return directStr;
    }
    return NULL;
}

char* strsAppend(Args* buffs_p, char* strOrigin, char* strToAppend) {
    pika_assert(NULL != strToAppend);
    pika_assert(NULL != strOrigin);
    int32_t size = strGetSize(strOrigin) + strGetSize(strToAppend);
    char* buff = args_getBuff(buffs_p, size);
    char* strOut = strCopy(buff, strOrigin);
    strAppend(strOut, strToAppend);
    return strOut;
}

char* strsReturnOut(Args* buffs, Args* outbuffs, char* str) {
    char* line_out = strsCopy(outbuffs, str);
    strsDeinit(buffs);
    return line_out;
}

char* strsGetLastToken(Args* buffs_p, char* argPath, char sign) {
    int32_t size = strGetSize(argPath);
    char* buff = args_getBuff(buffs_p, size);
    return strGetLastToken(buff, argPath, sign);
}

char* strsCut(Args* buffs_p, char* strIn, char startSign, char endSign) {
    int32_t size = strGetSize(strIn);
    char* buff = args_getBuff(buffs_p, size);
    return strCut(buff, strIn, startSign, endSign);
}

char* strsDeleteChar(Args* buffs_p, char* strIn, char ch) {
    int32_t size = strGetSize(strIn);
    return strDeleteChar(args_getBuff(buffs_p, size), strIn, ch);
}

static uint32_t getSizeOfFirstToken(char* str, char sign) {
    uint32_t size = strGetSize(str);
    for (uint32_t i = 0; i < size; i++) {
        if (str[i] == sign) {
            return i;
        }
    }
    return size;
}

char* strsGetFirstToken(Args* buffs_p, char* strIn, char sign) {
    int32_t size = getSizeOfFirstToken(strIn, sign);
    return strGetFirstToken(args_getBuff(buffs_p, size), strIn, sign);
}

char* strsPopToken(Args* buffs_p, char** tokens, char sign) {
    return strsCopy(buffs_p, strPopFirstToken(tokens, sign));
}

char* strsPopLine(Args* buffs_p, char** tokens) {
    return strsCopy(buffs_p, strPopFirstToken(tokens, '\n'));
}

char* strsCopy(Args* buffs_p, char* source) {
    pika_assert(source != NULL);
    int32_t size = strGetSize(source);
    char* buff = args_getBuff(buffs_p, size);
    return strCopy(buff, source);
}

char* strsCacheArg(Args* buffs_p, Arg* arg) {
    pika_assert(arg != NULL);
    char* res = strsCopy(buffs_p, arg_getStr(arg));
    arg_deinit(arg);
    return res;
}

char* strsFormat(Args* buffs_p, uint16_t buffSize, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char* res = args_getBuff(buffs_p, buffSize);
    pika_platform_vsnprintf(res, buffSize, fmt, args);
    va_end(args);
    return res;
}

Arg* arg_strAppend(Arg* arg_in, char* str_to_append) {
    pika_assert(NULL != str_to_append);
    Args buffs = {0};
    char* str_out = strsAppend(&buffs, arg_getStr(arg_in), str_to_append);
    Arg* arg_out = arg_newStr(str_out);
    arg_deinit(arg_in);
    strsDeinit(&buffs);
    return arg_out;
}

char* strsReplace(Args* buffs_p, char* orig, char* rep, char* with) {
    char* result;   // the return string
    char* ins;      // the next insert point
    char* tmp;      // varies
    int len_rep;    // length of rep (the string to remove)
    int len_with;   // length of with (the string to replace rep with)
    int len_front;  // distance between rep and end of last rep
    int count;      // number of replacements
    pika_assert(NULL != orig);
    pika_assert(NULL != rep);

    /* no need replace, skip */
    if (NULL == strstr(orig, rep)) {
        return orig;
    }

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL;  // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);
    // count the number of replacements needed
    ins = orig;
    tmp = strstr(ins, rep);
    count = 0;
    while (tmp) {
        count++;
        ins = tmp + len_rep;
        tmp = strstr(ins, rep);
    }
    tmp =
        args_getBuff(buffs_p, strlen(orig) + (len_with - len_rep) * count + 1);
    result = tmp;
    if (NULL == result) {
        return NULL;
    }
    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep;  // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

char* strsGetLine(Args* buffs_p, char* code) {
    int32_t lineSize = strGetLineSize(code);
    char* line_buff = args_getBuff(buffs_p, lineSize + 1);
    return strGetLine(line_buff, code);
}

void strsDeinit(Args* buffs_p) {
    link_deinit_stack(buffs_p);
}

char* strsPathFormat(Args* buffs_p, char* input) {
    int32_t size = strGetSize(input);
    char* buff = args_getBuff(buffs_p, size);
    strPathFormat(input, buff);
    return buff;
}

char* strsPathJoin(Args* buffs_p, char* input1, char* input2) {
    int32_t size = strGetSize(input1) + strGetSize(input2) + 1;
    char* buff = args_getBuff(buffs_p, size);
    strPathJoin(input1, input2, buff);
    return buff;
}

char* strsPathGetFolder(Args* buffs_p, char* input) {
    int32_t size = strGetSize(input);
    char* buff = args_getBuff(buffs_p, size);
    strPathGetFolder(input, buff);
    return buff;
}

char* strsPathGetFileName(Args* buffs_p, char* input) {
    int32_t size = strGetSize(input);
    char* buff = args_getBuff(buffs_p, size);
    strPathGetFileName(input, buff);
    return buff;
}

char* strsTransfer(Args* buffs, char* str, size_t* iout_p) {
    char* transfered_str = args_getBuff(buffs, strGetSize(str));
    size_t i_out = 0;
    size_t len = strGetSize(str);
    for (size_t i = 0; i < len; i++) {
        /* eg. replace '\x33' to '3' */
        if ((str[i] == '\\') && (str[i + 1] == 'x')) {
            char hex_str[] = "0x00";
            hex_str[2] = str[i + 2];
            hex_str[3] = str[i + 3];
            char hex = (char)strtoll(hex_str, NULL, 0);
            transfered_str[i_out++] = hex;
            i += 3;
            continue;
        }
        if (str[i] == '\\') {
            switch (str[i + 1]) {
                case 'r':
                    transfered_str[i_out++] = '\r';
                    break;
                case 'n':
                    transfered_str[i_out++] = '\n';
                    break;
                case 't':
                    transfered_str[i_out++] = '\t';
                    break;
                case 'b':
                    transfered_str[i_out++] = '\b';
                    break;
                case '\\':
                    transfered_str[i_out++] = '\\';
                    break;
                case '\'':
                    transfered_str[i_out++] = '\'';
                    break;
                case '\"':
                    transfered_str[i_out++] = '\"';
                    break;
                case '?':
                    transfered_str[i_out++] = '\?';
                    break;
                default:
                    transfered_str[i_out++] = str[i];
                    break;
            }
            i += 1;
            continue;
        }
        /* normal char */
        transfered_str[i_out++] = str[i];
    }
    *iout_p = i_out;
    return transfered_str;
}

char* strsFilePreProcess_ex(Args* outbuffs, char* lines, char* endwith) {
    Args buffs = {0};
    /* replace the "\r\n" to "\n" */
    lines = strsReplace(&buffs, lines, "\r\n", "\n");
    /* clear the void line */
    lines = strsReplace(&buffs, lines, "\n\n", "\n");
    /* add '\n' at the end */
    lines = strsAppend(&buffs, lines, endwith);
    char* result = strsCopy(outbuffs, lines);
    strsDeinit(&buffs);
    return result;
}

char* strsFilePreProcess(Args* outbuffs, char* lines) {
    return strsFilePreProcess_ex(outbuffs, lines, "\n\n");
}

char* strsSubStr(Args* buffs_p, char* name_start, char* name_end) {
    int32_t size = name_end - name_start;
    char* buff = args_getBuff(buffs_p, size + 1);
    for (int32_t i = 0; i < size; i++) {
        buff[i] = name_start[i];
    }
    buff[size] = '\0';
    return buff;
}

char* strsRepeat(Args* buffs, char* str, int num) {
    int32_t size = strGetSize(str) * num;
    char* buff = args_getBuff(buffs, size + 1);
    for (int32_t i = 0; i < num; i++) {
        for (int32_t j = 0; j < strGetSize(str); j++) {
            buff[i * strGetSize(str) + j] = str[j];
        }
    }
    buff[size] = '\0';
    return buff;
}

static void _add_indentation(const char* input, int spaces, char* output) {
    if (input == NULL || output == NULL || spaces < 0) {
        return;
    }

    const char* current = input;
    int output_index = 0;

    // Iterate over the input string
    while (*current) {
        // Add spaces at the beginning of each new line
        if (current == input || *(current - 1) == '\n') {
            for (int i = 0; i < spaces; ++i) {
                output[output_index++] = ' ';
            }
        }
        // Copy the current character
        output[output_index++] = *current++;
    }

    // Add the null terminator to the string
    output[output_index] = '\0';
}

char* strsAddIndentation(Args* buffs, char* str, int spaces) {
    int lines = 1;
    for (const char* current = str; *current; current++) {
        if (*current == '\n') {
            lines++;
        }
    }
    int output_length = strGetSize(str) + spaces * lines;
    char* buff = args_getBuff(buffs, output_length + 1);
    _add_indentation(str, spaces, buff);
    return buff;
}

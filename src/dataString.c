/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "dataString.h"
#include "PikaPlatform.h"
#include "dataMemory.h"

char* strCut(char* strOut, char* strIn, char startSign, char endSign) {
    int32_t Size = strGetSize(strIn);
    int32_t iStart = 0;
    int32_t iEnd = Size;
    uint8_t isStart = 0;
    uint8_t isEnd = 0;
    for (int32_t i = 0; i < Size; i++) {
        if (strIn[i] == startSign) {
            iStart = i;
            isStart = 1;
            break;
        }
    }
    for (int32_t i = Size - 1; i >= 0; i--) {
        if (strIn[i] == endSign) {
            iEnd = i;
            isEnd = 1;
            break;
        }
    }
    int outi = 0;
    for (int32_t i = iStart + 1; i < iEnd; i++) {
        strOut[outi] = strIn[i];
        outi++;
    }
    /* add \0 */
    strOut[outi] = 0;
    if (isStart && isEnd) {
        /* succeed */
        return strOut;
    }
    /* failed */
    return NULL;
}

char* strDeleteChar(char* strOut, char* strIn, char ch) {
    int32_t iOut = 0;
    uint32_t size = strGetSize(strIn);
    for (uint32_t i = 0; i < size; i++) {
        if (ch == strIn[i]) {
            continue;
        }
        strOut[iOut] = strIn[i];
        iOut++;
    }
    /* add \0 */
    strOut[iOut] = 0;
    return strOut;
}

char* strAppendWithSize(char* strOut, char* pData, int32_t Size) {
    int32_t strOut_i = strGetSize(strOut);
    for (int32_t i = 0; i < Size; i++) {
        strOut[strOut_i + i] = pData[i];
    }
    strOut_i += Size;
    // add \0 to the end of strOut
    strOut[strOut_i] = 0;

    return strOut;
}
const char bracketStart[] = {'(', '[', '{', '\'', '\"'};
const char bracketEnd[] = {')', ']', '}', '\'', '\"'};
#define BRACKET_TYPE_NUM (sizeof(bracketStart) / sizeof(char))

int _strCountSign(char* strIn, char sign, PIKA_BOOL bracketDepth0) {
    int32_t iCount = 0;
    int32_t iTotalDepth = 0;
    PIKA_BOOL bEscaped = PIKA_FALSE;
    for (size_t i = 0; strIn[i] != '\0'; i++) {
        if (!bracketDepth0) {
            if (strIn[i] == sign) {
                iCount++;
            }
            continue;
        }
        char cCurrentChar = strIn[i];
        if (cCurrentChar == '\\') {
            bEscaped = !bEscaped;
            continue;
        }
        if (!bEscaped) {
            for (int j = 0; j < BRACKET_TYPE_NUM; j++) {
                if (cCurrentChar == bracketStart[j]) {
                    iTotalDepth++;
                } else if (cCurrentChar == bracketEnd[j]) {
                    iTotalDepth--;
                }
            }
        }
        if (cCurrentChar == sign && iTotalDepth == 0) {
            iCount++;
        }
        bEscaped = PIKA_FALSE;
    }
    return iCount;
}

int32_t strCountSign(char* strIn, char sign) {
    return _strCountSign(strIn, sign, 0);
}

char* strReplaceChar(char* strIn, char src, char dst) {
    while (*strIn) {
        if (*strIn == src) {
            *strIn = dst;
        }
        strIn++;
    }
    return strIn;
}

int32_t strGetTokenNum(char* strIn, char sign) {
    return strCountSign(strIn, sign) + 1;
}

size_t strGetSize(char* pData) {
    pika_assert(pData != NULL);
    return strlen(pData);
}

char* strPointToLastToken(char* strIn, char sign) {
    pika_assert(NULL != strIn);
    if (!strIsContain(strIn, sign)) {
        return strIn;
    }
    int32_t size = strGetSize(strIn);
    for (int32_t i = size - 1; i > -1; i--) {
        if (strIn[i] == sign) {
            return strIn + i + 1;
        }
    }
    return strIn;
}

char* strPopLastToken(char* strIn, char sign) {
    char* last_token = strPointToLastToken(strIn, sign);
    if (last_token != strIn) {
        *(last_token - 1) = 0;
    }
    return last_token;
}

char* strGetLastToken(char* strOut, char* strIn, char sign) {
    int32_t size = strGetSize(strIn);
    int32_t buffSize = 0;
    for (int32_t i = size - 1; i > -1; i--) {
        if (strIn[i] != sign) {
            strOut[size - i - 1] = strIn[i];
            buffSize++;
        }
        if (strIn[i] == sign) {
            break;
        }
    }
    int32_t i = 0;
    for (i = 0; i < buffSize / 2; i++) {
        char buff = strOut[i];
        strOut[i] = strOut[buffSize - i - 1];
        strOut[buffSize - i - 1] = buff;
    }
    strOut[buffSize] = 0;
    return strOut;
}

char* strPopFirstToken(char** strIn, char sign) {
    char* strIn_ = *strIn;
    char* pos = strchr(strIn_, sign);
    if (pos != NULL) {
        /* found the first sign */
        *pos = 0;
        *strIn = pos + 1;
        return strIn_;
    }
    /* no found */
    *strIn = strchr(strIn_, 0);
    return strIn_;
}

char* strGetFirstToken(char* strOut, char* strIn, char sign) {
    int32_t size = strGetSize(strIn);
    for (int32_t i = 0; i < size; i++) {
        if (strIn[i] != sign) {
            strOut[i] = strIn[i];
        }
        if (strIn[i] == sign) {
            break;
        }
    }
    return strOut;
}

char* strAppend(char* strOut, char* pData) {
    uint32_t Size = 0;
    Size = strGetSize(pData);
    return strAppendWithSize(strOut, pData, Size);
}

int32_t strIsStartWith(char* str, char* strStart) {
    if (NULL == str || NULL == strStart) {
        /* input is null */
        return 0;
    }
    /* fast return */
    if (str[0] != strStart[0]) {
        return 0;
    }
    uint32_t size = strGetSize(strStart);
    if (0 == strncmp(str, strStart, size)) {
        return 1;
    }
    return 0;
}

int32_t strEqu(char* str1, char* str2) {
    if (NULL == str1 || NULL == str2) {
        return 0;
    }
    return !strcmp(str1, str2);
}

char* strRemovePrefix(char* inputStr, char* prefix, char* outputStr) {
    if (!strIsStartWith(inputStr, prefix)) {
        return NULL;
    }
    size_t len = strGetSize(inputStr);
    for (uint32_t i = strGetSize(prefix); i < len; i++) {
        outputStr[i - strGetSize(prefix)] = inputStr[i];
    }
    return outputStr;
}

int32_t strIsContain(char* str, char ch) {
    pika_assert(NULL != str);
    while (*str) {
        if (*str == ch) {
            return 1;
        }
        str++;
    }
    return 0;
}

char* strCopy(char* strBuff, char* strIn) {
    pika_platform_memcpy(strBuff, strIn, strGetSize(strIn) + 1);
    return strBuff;
}

int32_t strGetLineSize(char* str) {
    int i = 0;
    while (1) {
        if (str[i] == '\n') {
            return i;
        }
        i++;
    }
}

char* strGetLine(char* strOut, char* strIn) {
    int32_t lineSize = strGetLineSize(strIn);
    pika_platform_memcpy(strOut, strIn, lineSize);
    strOut[lineSize] = 0;
    return strOut;
}

char* strGetLastLine(char* strOut, char* strIn) {
    int32_t size = strGetSize(strIn);
    char sign = '\n';
    uint32_t beginIndex = 0;

    /* skip the latest '\n' */
    for (int32_t i = size - 2; i > -1; i--) {
        if (strIn[i] == sign) {
            beginIndex = i + 1;
            break;
        }
    }

    pika_platform_memcpy(strOut, strIn + beginIndex, size - beginIndex);
    strOut[size - beginIndex + 1] = 0;
    return strOut;
}

int strPathFormat(char* input, char* output) {
    int len = strlen(input);
    int i = 0;
    int j = 0;
    for (i = 0; i < len; i++) {
        if (input[i] == '\\') {
            output[j++] = '/';
        } else {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
    return j;
}

int strPathJoin(char* input1, char* input2, char* output) {
    /* format */
    size_t input1_len = strlen(input1);
    size_t input2_len = strlen(input2);
    /* if input1 is all space */
    if (input1_len == 0) {
        strPathFormat(input2, output);
        return 0;
    }
    char* input1_format = (char*)pikaMalloc(input1_len + 1);
    char* input2_format = (char*)pikaMalloc(input2_len + 1);
    strPathFormat(input1, input1_format);
    strPathFormat(input2, input2_format);
    /* join */
    int len1 = strlen(input1_format);
    int len2 = strlen(input2_format);
    int i = 0;
    int j = 0;
    for (i = 0; i < len1; i++) {
        output[j++] = input1_format[i];
    }
    if (input1_format[len1 - 1] != '/') {
        output[j++] = '/';
    }
    if (input2_format[0] == '/') {
        i = 1;
    } else {
        i = 0;
    }
    for (; i < len2; i++) {
        output[j++] = input2_format[i];
    }
    output[j] = '\0';
    /* free */
    pikaFree(input1_format, input1_len + 1);
    pikaFree(input2_format, input2_len + 1);
    return j;
}

int strPathGetFolder(char* input, char* output) {
    size_t input_len = strlen(input);
    char* input_format = (char*)pikaMalloc(input_len + 1);
    strPathFormat(input, input_format);
    int len = strlen(input_format);
    int i = 0;
    int j = 0;
    for (i = 0; i < len; i++) {
        if (input_format[i] == '/') {
            j = i;
        }
    }
    for (i = 0; i < j; i++) {
        output[i] = input_format[i];
    }
    output[i] = '\0';
    pikaFree(input_format, input_len + 1);
    return i;
}

int strPathGetFileName(char* input, char* output) {
    if (!strIsContain(input, '/') && !strIsContain(input, '\\')) {
        strCopy(output, input);
        return 0;
    };
    size_t input_len = strlen(input);
    char* input_format = (char*)pikaMalloc(input_len + 1);
    strPathFormat(input, input_format);
    int len = strlen(input_format);
    int i = 0;
    int j = 0;
    for (i = 0; i < len; i++) {
        if (input_format[i] == '/') {
            j = i;
        }
    }
    for (i = j + 1; i < len; i++) {
        output[i - j - 1] = input_format[i];
    }
    output[i - j - 1] = '\0';
    pikaFree(input_format, input_len + 1);
    return i - j - 1;
}

int strGetIndent(char* string) {
    int indent = 0;
    int len = strGetSize(string);
    for (int j = 0; j < len; j++) {
        if (string[j] == ' ') {
            indent++;
        } else {
            return indent;
        }
    }
    return indent;
}

int strIsBlank(char* string) {
    int len = strGetSize(string);
    for (int j = 0; j < len; j++) {
        if (string[j] != ' ' && string[j] != '\t' && string[j] != '\r' &&
            string[j] != '\n') {
            return 0;
        }
    }
    return 1;
}

int strOnly(char* string, char ch) {
    int len = strGetSize(string);
    if (len == 0) {
        return 0;
    }
    for (int j = 0; j < len; j++) {
        if (string[j] != ch) {
            return 0;
        }
    }
    return 1;
}

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
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __MY_TEST_TOOLS_H
#define __MY_TEST_TOOLS_H
#include "PikaPlatform.h"

/* size */
size_t strGetSize(char* pData);
/* append */
char* strAppend(char* strOut, char* pData);
char* strAppend_unlimited(char* strOut, char* pData);
char* strAppendWithSize(char* strOut, char* pData, int32_t Size);
/* cut */
char* strCut(char* strOut, char* strIn, char startSign, char endSign);
/* pika_assert */
int32_t strIsStartWith(char* str, char* strStart);
int32_t strEqu(char* str1, char* str2);
/* delete */
char* strDeleteEnter(char* str);
char* strDeleteChar(char* strOut, char* strIn, char ch);
/* prefix */
char* strRemovePrefix(char* inputStr, char* prefix, char* outputStr);
/* token */
int32_t strGetToken(char* string, char** argv, char sign);
char* strPopFirstToken(char** strIn, char sign);
int32_t strCountSign(char* strIn, char sign);
int32_t strGetLineNum(char* strIn);
int32_t strGetTokenNum(char* strIn, char sign);
char* strGetFirstToken(char* strOut, char* strIn, char sign);
char* strGetLastToken(char* strOut, char* strIn, char sign);
char* strClear(char* str);
int32_t strIsContain(char* str, char ch);
char* strCopy(char* strBuff, char* strIn);
char* strGetLastLine(char* strOut, char* strIn);
char* strPointToLastToken(char* strIn, char sign);
char* strGetLine(char* strOut, char* strIn);
int32_t strGetLineSize(char* str);
char* strPopLastToken(char* strIn, char sign);
char* strGetLastLine(char* strOut, char* strIn);
char* strReplaceChar(char* strIn, char src, char dst);

int strPathFormat(char* input, char* output);
int strPathJoin(char* input1, char* input2, char* output);
int strPathGetFolder(char* input, char* output);
int strPathGetFileName(char* input, char* output);
int strGetIndent(char* string);
int charIsBlank(char ch);
int strIsBlank(char* string);
int strOnly(char* string, char ch);
int _strCountSign(char* strIn, char sign, pika_bool bracketDepth0);
char* strFind(char* string, char ch);

#endif
#ifdef __cplusplus
}
#endif

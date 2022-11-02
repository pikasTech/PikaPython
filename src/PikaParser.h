/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
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

#ifndef __PIKA_PARSER__H
#define __PIKA_PARSER__H
#include "PikaVM.h"
#include "dataQueueObj.h"
#include "dataStack.h"

typedef enum TokenType {
    TOKEN_strEnd = 0,
    TOKEN_symbol,
    TOKEN_keyword,
    TOKEN_operator,
    TOKEN_devider,
    TOKEN_literal,
} TokenType;

enum StmtType {
    STMT_reference,
    STMT_tuple,
    STMT_string,
    STMT_bytes,
    STMT_number,
    STMT_method,
    STMT_chain,
    STMT_operator,
    STMT_import,
    STMT_list,
    STMT_slice,
    STMT_dict,
    STMT_none,
};

typedef struct Asmer Asmer;
struct Asmer {
    char* asm_code;
    uint8_t block_deepth_now;
    uint8_t is_new_line;
    char* line_pointer;
};

typedef enum _GenRuleValType {
    VAL_NONEVAL,
    VAL_DYNAMIC,
    VAL_STATIC_,
} GenRuleValType;

typedef struct GenRule {
    char* ins;
    GenRuleValType type;
    char* ast;
    char* val;
} GenRule;

typedef struct LexToken LexToken;
struct LexToken {
    char* tokenStream;
    enum TokenType type;
    char* pyload;
};

typedef struct Cursor ParsetState;
struct Cursor {
    char* tokenStream;
    uint16_t length;
    uint16_t iter_index;
    int8_t branket_deepth;
    struct LexToken token1;
    struct LexToken token2;
    Arg* last_token;
    Args* iter_buffs;
    Args* buffs_p;
    PIKA_RES result;
};

char* Parser_fileToAsm(Args* outBuffs, char* filename);
char* Parser_linesToAsm(Args* outBuffs, char* multiLine);
PIKA_RES Parser_linesToBytes(ByteCodeFrame* bf, char* py_lines);
char* Parser_linesToArray(char* lines);

char* instructUnit_fromAsmLine(Args* outBuffs, char* pikaAsm);
ByteCodeFrame* byteCodeFrame_appendFromAsm(ByteCodeFrame* bf, char* pikaAsm);

#define Cursor_forEach(cursor)   \
    _Cursor_beforeIter(&cursor); \
    for (int __i = 0; __i < cursor.length; __i++)

#define Cursor_forEachTokenExistPs(cursor, tokenStream) \
    /* init parserStage */                              \
    _Cursor_init(&cursor);                              \
    _Cursor_parse(&cursor, tokenStream);                \
    Cursor_forEach(cursor)

#define Cursor_forEachToken(cursor, tokenStream) \
    struct Cursor cursor;                        \
    Cursor_forEachTokenExistPs(cursor, tokenStream)

uint16_t TokenStream_getSize(char* tokenStream);

#endif
